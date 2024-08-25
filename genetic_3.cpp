#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <array>
#include <algorithm>
#include <iterator>
#include <random>
#include <numeric>
#include <string>
#include <chrono>
#include <thread>
#if _WIN32
#include <atomic>
#include <mutex>
#include <condition_variable>
#elif __linux__
#include <atomic>
#include <mutex>
#include <condition_variable>
#endif

// #define SAVE_MAZES 1


using namespace std;

const int size_= 30;
const int populationSize_ = 120;
const float Pc_ = 0.05;
const float Pm_ = 0.01;
const float fill_ = 0.05;
const int generations_ = 250;
const int matingEventsPerGeneration_ = 2000;
const int totalMatingEvents_ = generations_ * matingEventsPerGeneration_;
const array<int, 2> entrance_ = {0, 0};
const array<int, 2> exit_ = {size_-1, size_-1};
const array<array<int, 2>, 4> checkpoints_ = {
    array<int, 2> {6, 24},
    array<int, 2> {12, 18},
    array<int, 2> {18, 12},
    array<int, 2> {24, 6}
};
array<array<int, size_>, size_> pathFinder_lk_;

map<array<array<int, 2>, 2>, vector<int>> points2pathIdxs;

random_device dev;
mt19937 rng(dev());
const int rng_upper_bound_ = 1'000'000;
uniform_int_distribution<mt19937::result_type> getNum(1,rng_upper_bound_); // distribution in range [1, 10**6]
const int new_pm_ = rng_upper_bound_ * Pm_;
const int new_pc_ = rng_upper_bound_ * Pc_;

const int NUM_THREADS_ = 7;
typedef array<array<array<bool, size_>, size_>, populationSize_> populationType;
typedef array<int, 7> inputType;
typedef array<int, 7> outputType;
std::array<int, 7> threadInputs;

/**** VERSION 2 */
std::mutex m;                       // for the 2 cv's below
std::condition_variable cond;       // for lock-step
std::condition_variable cond_cc;    // to hold threads ahead of the rest
int num_threads;                    // used by barrier logic, represents number of active thread workers, decremented on exit
int waiting_for_increment;          // number of threads past cond_cc, waiting for cond
int global_cc;                      // global counter for number of iterations, increment to run another round of worker thread work
bool termination_flag_;             // set to true to get worker threads to complete execution
std::mutex barrier_mutex;           // for the main thread to wait, until all threads have finished task
bool barrier_completed;             // also for the main thread
std::condition_variable barrier_cv; // also for the main thread


#ifdef DEBUG_COUNT
unsigned long int debug_pathFinderCount = 0;
#endif

void init() {
    for (int i = 0 ; i < size_ ; ++i) {
        pathFinder_lk_[i].fill(1'000'000);
    }

    /**
     * 
     * Does order of this impact performance? If so, by how much
    */
    points2pathIdxs[{{{ 0,  0}, { 6, 24}}}] = {0, 1, 2, 3, 4, 5};
    points2pathIdxs[{{{ 0,  0}, {12, 18}}}] = {6, 7, 8, 9, 10, 11};
    points2pathIdxs[{{{ 0,  0}, {18, 12}}}] = {12, 13, 14, 15, 16, 17};
    points2pathIdxs[{{{ 0,  0}, {24,  6}}}] = {18, 19, 20, 21, 22, 23};
    points2pathIdxs[{{{ 6, 24}, {12, 18}}}] = {0, 1, 6, 7, 12, 14, 16, 17, 18, 20, 22, 23};
    points2pathIdxs[{{{ 6, 24}, {18, 12}}}] = {2, 3, 6, 8, 10, 11, 12, 13, 19, 20, 21, 22};
    points2pathIdxs[{{{ 6, 24}, {24,  6}}}] = {4, 5, 7, 8, 9, 10, 13, 14, 15, 16, 18, 19};
    points2pathIdxs[{{{ 6, 24}, {29, 29}}}] = {9, 11, 15, 17, 21, 23};
    points2pathIdxs[{{{12, 18}, {18, 12}}}] = {0, 2, 4, 5, 8, 9, 14, 15, 18, 19, 21, 23};
    points2pathIdxs[{{{12, 18}, {24,  6}}}] = {1, 2, 3, 4, 10, 11, 12, 13, 15, 17, 20, 21};
    points2pathIdxs[{{{12, 18}, {29, 29}}}] = {3, 5, 13, 16, 19, 22};
    points2pathIdxs[{{{18, 12}, {24,  6}}}] = {0, 1, 3, 5, 6, 7, 9, 11, 16, 17, 22, 23};
    points2pathIdxs[{{{18, 12}, {29, 29}}}] = {1, 4, 7, 10, 18, 20};
    points2pathIdxs[{{{24,  6}, {29, 29}}}] = {0, 2, 6, 8, 12, 14};
}

// overload of << for a maze
ostream& operator<<(ostream& os, array<array<bool, size_>, size_>& m) {
    for (array<bool, size_>&i : m) {
        for (bool j : i) {
            os << j << ", ";
        }
        os << '\n';
    }
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// overload for lookup in the pathfinder
ostream& operator<<(ostream& os, array<array<int, size_>, size_>& m) {
    for (array<int, size_>&i : m) {
        for (int j : i) {
            os << j << ", ";
        }
        os << '\n';
    }
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// overload of << for an int array of size 2 used as indices in this program
ostream& operator<<(ostream& os, array<int, 2>& m) {
    os << '(' << m[0] << ", " << m[1] << ')';
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// overload of << for an int array of size 7 in the mating event population selection for 'evolution'
ostream& operator<<(ostream& os, array<int, 7>& m) {
    os << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << ", " << m[4] << ", " << m[5] << ", " << m[6];
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// overload of << for vector of int type
ostream& operator<<(ostream& os, vector<int>& x) {
    for (int& i : x) {
        os << i << ", ";
    }
    os << '\n';
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

array<array<bool, size_>, size_> genMaze() {
    // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> getNum(1,1000); // distribution in range [1, 6]
    array<array<bool, size_>, size_> maze;
    for (auto&i : maze) {
        for (auto&j : i) {
            j = (float(getNum(rng))/1000.0f <= fill_);
        }
    }
    return maze;
}

void uniformMutationAndCrossover(array<array<bool, size_>, size_>& m1, array<array<bool, size_>, size_>& m2) {
    for (int i = 0 ; i < size_ ; i++) {
        for (int j = 0 ; j < size_ ; j++) {
            if (getNum(rng) <= new_pc_) {
                bool foo = m1[i][j] ^ m2[i][j];
                m1[i][j] ^= foo;
                m2[i][j] ^= foo;   
            }
            if (getNum(rng) <= new_pm_) {
                m1[i][j] = !m1[i][j];
            }
            if (getNum(rng) <= new_pm_) {
                m2[i][j] = !m2[i][j];
            }
        }
    }
}

// returns min dist from entrance -> exit in maze
// if no path, returns 1'000'000

/**
 * n1: changeed foo = {pt[0]+1, pt[1]}; to foo[0]=pt[0]+1 ; foo[1] = pt[1]
 * n2: change foo from an array of 2 into two ints, x & y
*/
int pathFinder(
    const array<array<bool, size_>, size_>& maze,
    const array<int, 2>& start, 
    const array<int, 2>& end
) {
    // fitness() already checks the same, this is (I think) redundant
    // if (maze[start[0]][start[1]]) {
    //     return 0;
    // }
    // if (maze[end[0]][end[1]]) {
    //     return 0;
    // }

    // thread_local array<array<int, size_>, size_> lk;
    // for (int i = 0 ; i < size_ ; ++i) {
    //     pathFinder_lk_[i].fill(1'000'000);
    // }
    array<array<int, size_>, size_> lk = pathFinder_lk_;
    lk[start[0]][start[1]] = 0;


    vector<int> q = {};
    q.reserve(100);
    q.push_back(start[0]);
    q.push_back(start[1]);
    int dist;
    int x,y;
    int e1, e2;

    e1 = end[0];
    e2 = end[1];

    while (q.size() > 0) {
        // pt = q.back()
        y = q.back();
        q.pop_back();
        x = q.back();
        q.pop_back();
        dist = lk[x][y];
        dist += 1;
        
        if (dist > lk[end[0]][end[1]]) 
            continue;

        // foo = {pt[0]+1, pt[1]};
        x++; // (1, 0)
        if (x < size_ && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }

        // foo = {pt[0], pt[1]+1};
        x--;
        y++; // (0, 1)
        if (y < size_ && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }
        // foo = {pt[0]-1, pt[1]};
        x--;
        y--; // (-1, 0)
        if (x >= 0 && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }

        // foo = {pt[0], pt[1]-1};
        x++;
        y--; // (0, -1)
        if (y >= 0 && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }
    }

    // cout << "\n lk: \n" << lk;

    // return lk[end[0]][end[1]] == 1'000'000 ? 0 : lk[end[0]][end[1]];
    return lk[end[0]][end[1]];
}

__attribute__((hot)) int fitness_4(const array<array<bool, size_>, size_>& maze) {
    
    if (maze[entrance_[0]][entrance_[1]] || maze[exit_[0]][exit_[1]]) {
        return 0;
    }
    for (auto& c : checkpoints_) {
        if (maze[c[0]][c[1]]) {
            return 0;
        }
    }
 
    int totalDist, dist, bestResult;
    int key;
    bestResult = 1'000'000;

    alignas(128) array<int, 24> pathDist;
    pathDist.fill(0);
    // array<int, 24> pathDist __attribute__((aligned(32)));
    // __builtin_memset(pathDist.data(), 0, sizeof(pathDist));

    // precompute pair-wise distances
    for (const auto& [key, value] : points2pathIdxs) {
        dist = pathFinder(maze, key[0], key[1]);
        // fill in paths
        for (const int& idx : value) {
            pathDist[idx] += dist;
        }
    }

    bestResult = *min_element(pathDist.begin(), pathDist.end());
    if (bestResult >= 1'000'000)
        return 0;
    return bestResult;
}

void saveMazes(vector<array<array<bool, size_>, size_>>& mazes, string label) {
    ofstream myfile;
    myfile.open ("genetic_3_results_save_2.txt");
    myfile << label;
    for (auto&m : mazes) {
        myfile << m << '\n';
    }
    myfile.close();
}

void barrier_init(const int& n) {
    num_threads = n;
    waiting_for_increment = 0;
    termination_flag_ = false;
    barrier_completed = false;
    global_cc = -1;
}

void work(int x, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    int localitercount = 0;
    std::unique_lock<std::mutex> lock(m, std::defer_lock); // Create the lock but don't lock immediately

    while(true) {
        // Inlined barrier_wait
        lock.lock();
        cond_cc.wait(lock, [&localitercount]{
            return global_cc == localitercount;
        });
        
        waiting_for_increment++;
        
        if (waiting_for_increment == num_threads) {
            cond.notify_all();
        } else {
            cond_cc.notify_all();
            cond.wait(lock);
        }
        lock.unlock();

        // check if termination flag is set. If yes, break the loop
        if (termination_flag_) break;

        localitercount += 1;

        // work
        output1[x] = output2[x] = fitness_4(population[input[x]]);

        // Inlined barrier_done
        lock.lock();
        waiting_for_increment--;
        if (waiting_for_increment == 0) {
            barrier_completed = true;
            barrier_cv.notify_one();
        }
        lock.unlock();
    }

    // Inlined barrier_exit
    lock.lock();
    num_threads--;
    cond_cc.notify_all(); // for the ahead-of-time n+1 cc threads
    if (waiting_for_increment == num_threads) {
        cond.notify_all();
    }
    lock.unlock();
}

// same as work, but records stats for the fitness_4 function
void work_2(int x, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    int localitercount = 0;
    std::unique_lock<std::mutex> lock(m, std::defer_lock); // Create the lock but don't lock immediately
    std::vector<double> timings;
    int result;
    while(true) {
        // Inlined barrier_wait
        lock.lock();
        cond_cc.wait(lock, [&localitercount]{
            return global_cc == localitercount;
        });
        
        waiting_for_increment++;
        
        if (waiting_for_increment == num_threads) {
            cond.notify_all();
        } else {
            cond_cc.notify_all();
            cond.wait(lock);
        }
        lock.unlock();

        // check if termination flag is set. If yes, break the loop
        if (termination_flag_) break;

        localitercount += 1;

        auto start = std::chrono::high_resolution_clock::now();
        
        // work
        result = fitness_4(population[input[x]]);
        
        auto end = std::chrono::high_resolution_clock::now();

        std::chrono::duration<double, std::micro> duration = end - start;
        timings.push_back(duration.count());

        output1[x] = output2[x] = result;


        // Inlined barrier_done
        lock.lock();
        waiting_for_increment--;
        if (waiting_for_increment == 0) {
            barrier_completed = true;
            barrier_cv.notify_one();
        }
        lock.unlock();
    }

    // Inlined barrier_exit
    lock.lock();
    if (!timings.empty()) {
        double average = std::accumulate(timings.begin(), timings.end(), 0.0) / timings.size();
        std::cout << "Average time for the timed statement: " << average << " microseconds" << std::endl;
    }
    num_threads--;
    cond_cc.notify_all(); // for the ahead-of-time n+1 cc threads
    if (waiting_for_increment == num_threads) {
        cond.notify_all();
    }
    lock.unlock();
}

void thread_pool_init(std::array<std::thread, NUM_THREADS_>& workers, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i] = std::thread(work, i, ref(population), ref(input), ref(output1), ref(output2));
    }
}

void runner() {
    array<array<array<bool, size_>, size_>, populationSize_> population;
    for (auto& p : population)
        p = genMaze();
    array<int, 7> fitnesses;
    array<int, 7> sortedFitnesses;
    array<int, 7> indices;
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    // indices of the two fittest members, then the two weakest members
    int i1, i2, i3, i4;
    string label;


    std::array<std::thread, NUM_THREADS_> workers;
    barrier_init(NUM_THREADS_);
    thread_pool_init(workers, ref(population), ref(indices), ref(fitnesses), ref(sortedFitnesses));
    
    auto randomDevice = mt19937{random_device{}()};
    const int numIterations_ = generations_ * matingEventsPerGeneration_;
    random_device sample_dev;
    mt19937 sample_rng(sample_dev());
    uniform_int_distribution<mt19937::result_type> sample_getNum(0,populationSize_-1); // distribution in range [1, 10**6]
    
    for (int g = 0 ; g < numIterations_ ; ++g) {

        barrier_completed = false;

        for (int i = 0 ; i < 7 ; i++) {
            indices[i] = sample_getNum(sample_rng);
        }

        global_cc++;
        cond_cc.notify_all();
        {
            std::unique_lock<std::mutex> lock(m);
            barrier_cv.wait(lock, []{ return barrier_completed; });
        }

        // since we passed a ref of outputs to each thread, no copying from threadOutput -> wherever it was supposed to be

        // auto end = chrono::steady_clock::now();
        // auto diff = end-start;
        // cout<<"\n time: "<< chrono::duration<double, milli>(diff).count()<<" ms";
        
        // https://stackoverflow.com/questions/9025084/sorting-a-vector-in-descending-order
        // find the fittest two members
        // the fitnesses are sorted in descending order
        sort(sortedFitnesses.begin(), sortedFitnesses.end(), greater<int>());

        // https://stackoverflow.com/questions/22342581/returning-the-first-index-of-an-element-in-a-vector-in-c
        // i1 and i2 contain the indices (wrt fitness[]) of the two fittest elements
        i1 = find(fitnesses.begin(), fitnesses.end(), sortedFitnesses[0]) - fitnesses.begin();
        i2 = find(fitnesses.begin(), fitnesses.end(), sortedFitnesses[1]) - fitnesses.begin();
        // i1 and i2 cannot be the same
        while (i1 == i2 || fitnesses[i2] != sortedFitnesses[1]) {
            i2 = (i2 + 1)%7;
        }

        // cout << "\n i1: " << i1 << " | i2: " << i2 ;

        // m1 and m2 are copies of the two fittest mazes
        m1 = population[indices[i1]];
        m2 = population[indices[i2]];

        // cout << "\n m1 and m2 BEFORE evolution: \n";
        // cout << m1 << "\n\n" << m2;

        // evolution ....
        uniformMutationAndCrossover(m1, m2);
        //not as performant as you'd think, the workload is too little
        // #pragma omp parallel sections
        // {
        //     #pragma omp section
        //     {
        //         uniformMutation2(m1);
        //     }
        //     #pragma omp section
        //     {
        //         uniformMutation2(m2);
        //     }
        // }
        

        // cout << "\n m1 and m2 AFTER evolution: \n";
        // cout << m1 << "\n\n" << m2;

        // now i3 and i4 will contain indces of the weakest 2 elements
        i3 = find(fitnesses.begin(), fitnesses.end(), sortedFitnesses[6]) - fitnesses.begin();
        i4 = find(fitnesses.begin(), fitnesses.end(), sortedFitnesses[5]) - fitnesses.begin();
        // i1, i2, i3 and i4 have to be different cannot be the same
        while (i3 == i1 || i3 == i2 || fitnesses[i3] != sortedFitnesses[6]) {
            i3 = (i3 + 1)%7;
        }
        while (i4 == i1 || i4 == i2 || i4 == i3 || fitnesses[i4] != sortedFitnesses[5]) {
            i4 = (i4 + 1)%7;
        }

        // cout << "\n i3: " << i3 << " | i4: " << i4 ;

        // overwrite the weakest two with the modified fittest two
        population[indices[i3]] = m1;
        population[indices[i4]] = m2;
        // cout << "\n sortedFitnesses: " << sortedFitnesses;
    }
    
    // terminate workers, join threads
    termination_flag_ = true;
    global_cc++;
    cond_cc.notify_all();

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i].join();
    }

    #ifdef DEBUG_COUNT
    cout << "pathfinder call count: " << debug_pathFinderCount << endl;
    #endif

    #ifdef SAVE_MAZES
    // save logic
    vector<array<array<bool, size_>, size_>> niceOnes;
    for (auto&p : population) {
        auto result = fitness_4(p);
        if (result > 0) {
            cout << "\n fitness: " << result;
            niceOnes.push_back(p);
        }
    }
    saveMazes(niceOnes, label);
    #endif
}

// g++ -std=c++17 -O3 -Wl,--stack=16777216 -pthread genetic_3.cpp -o a
int main() {
    init();
    runner();
    return 0;
}