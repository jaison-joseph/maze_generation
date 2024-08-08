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
#elif __linux__
#include <atomic>
#endif

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

#ifdef DEBUG_COUNT
unsigned long int debug_pathFinderCount = 0;
#endif

void init() {
    for (int i = 0 ; i < size_ ; ++i) {
        pathFinder_lk_[i].fill(1'000'000);
    }

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
        #pragma clang loop vectorize(enable)
        for (auto&j : i) {
            j = (float(getNum(rng))/1000.0f <= fill_);
        }
    }
    return maze;
}


void uniformMutation(array<array<bool, size_>, size_>& m) {
    for (int i = 0 ; i < size_ ; i++) {
        for (int j = 0 ; j < size_ ; j++) {
            if (getNum(rng) <= new_pm_) {
                m[i][j] = !m[i][j];
            }
        }
    }
}

void uniformMutation2(array<array<bool, size_>, size_>& m) {
    static array<array<int, size_>, size_> rng_values;
    for (auto& row : rng_values) {
        for (auto& val : row) {
            val = getNum(rng);
        }
    }
    #pragma clang loop vectorize(enable)
    for (int i = 0; i < size_; i++) {
        for (int j = 0; j < size_; j++) {
            if (rng_values[i][j] <= new_pm_) {
                m[i][j] = !m[i][j];
            }
        }
    }
}

void uniformCrossover(array<array<bool, size_>, size_>& m1, array<array<bool, size_>, size_>& m2) {
    
    for (int i = 0 ; i < size_ ; ++i) {
        #pragma clang loop vectorize(enable)
        for (int j = 0 ; j < size_ ; ++j) {
            if (getNum(rng) <= new_pc_) {
                bool foo = m1[i][j] ^ m2[i][j];
                m1[i][j] ^= foo;
                m2[i][j] ^= foo;   
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

    array<array<int, size_>, size_> lk = pathFinder_lk_;
    // thread_local array<array<int, size_>, size_> lk;
    for (int i = 0 ; i < size_ ; ++i) {
        pathFinder_lk_[i].fill(1'000'000);
    }
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
        x++;
        if (x < size_ && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }

        // foo = {pt[0], pt[1]+1};
        x--;
        y++;
        if (y < size_ && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }
        // foo = {pt[0]-1, pt[1]};
        x--;
        y--;
        if (x >= 0 && lk[x][y] > dist && !maze[x][y]) {
            lk [x] [y] = dist;
            if (x != e1 || y != e2) {
                q.push_back(x);
                q.push_back(y);
            }
        }

        // foo = {pt[0], pt[1]-1};
        x++;
        y--;
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

__attribute__((always_inline)) int hashh(const int& i, const int& j, const int& k, const int& l) {
    return (i << 15) | (j << 10) | (k << 5) | l;
}

int fitness_3_og(const array<array<bool, size_>, size_>& maze) {
    if (maze[entrance_[0]][entrance_[1]]) {
        return 0;
    }
    if (maze[exit_[0]][exit_[1]]) {
        return 0;
    }
    for (auto& c : checkpoints_) {
        if (maze[c[0]][c[1]]) {
            return 0;
        }
    }
    
    vector<int> checkpointDistances;
    checkpointDistances.clear();
    vector<array<int, 2>> path;
    // a copy for the permutations
    array<array<int, 2>, 4> c = checkpoints_; 
    int totalDist, dist, bestResult;
    bool pathFailed;
    map<array<array<int, 2>, 2>, int> cache;
    do {
        path.clear();
        path.push_back(entrance_);
        path.insert(path.end(), c.begin(), c.end());
        path.push_back(exit_);
        totalDist = 0;
        pathFailed = false;
        for (int i = 0 ; i < path.size() - 1 ; ++i) {
            if (cache.find({path[i], path[i+1]}) != cache.end())
                dist = cache[{path[i], path[i+1]}];
            else if (cache.find({path[i+1], path[i]}) != cache.end())
                dist = cache[{path[i+1], path[i]}];
            else {
                #ifdef DEBUG_COUNT
                debug_pathFinderCount += 1;
                #endif
                dist = pathFinder(maze, path[i], path[i+1]);
                cache[{path[i], path[i+1]}] = dist; 
                // alsocache[path[i][0]][path[i][1]][path[i+1][0]][path[i+1][1]] = dist;
            }
            if (dist == 0) {
                pathFailed = true;
                break;
            }
            totalDist += dist;
        }
        if (!pathFailed) {
            checkpointDistances.push_back(totalDist);
        }
    } while (next_permutation(c.begin(), c.end()));
    if (checkpointDistances.size() == 0)
        return 0;
    // min element returns an iterator, so we derefence it
    bestResult = *min_element(checkpointDistances.begin(), checkpointDistances.end());
    return bestResult;
}

// so apparently I overthought the fitness function
// maze[i][j] is true => wall, false => empty
// this version adds cache 

int fitness_3(const array<array<bool, size_>, size_>& maze) {
    if (maze[entrance_[0]][entrance_[1]]) {
        return 0;
    }
    if (maze[exit_[0]][exit_[1]]) {
        return 0;
    }
    for (auto& c : checkpoints_) {
        if (maze[c[0]][c[1]]) {
            return 0;
        }
    }

    static const std::vector<std::vector<std::array<int, 2>>> all_paths = {
        {{0, 0}, {6, 24}, {12, 18}, {18, 12}, {24, 6}, {29, 29}},
        {{0, 0}, {6, 24}, {12, 18}, {24, 6}, {18, 12}, {29, 29}},
        {{0, 0}, {6, 24}, {18, 12}, {12, 18}, {24, 6}, {29, 29}},
        {{0, 0}, {6, 24}, {18, 12}, {24, 6}, {12, 18}, {29, 29}},
        {{0, 0}, {6, 24}, {24, 6}, {12, 18}, {18, 12}, {29, 29}},
        {{0, 0}, {6, 24}, {24, 6}, {18, 12}, {12, 18}, {29, 29}},
        {{0, 0}, {12, 18}, {6, 24}, {18, 12}, {24, 6}, {29, 29}},
        {{0, 0}, {12, 18}, {6, 24}, {24, 6}, {18, 12}, {29, 29}},
        {{0, 0}, {12, 18}, {18, 12}, {6, 24}, {24, 6}, {29, 29}},
        {{0, 0}, {12, 18}, {18, 12}, {24, 6}, {6, 24}, {29, 29}},
        {{0, 0}, {12, 18}, {24, 6}, {6, 24}, {18, 12}, {29, 29}},
        {{0, 0}, {12, 18}, {24, 6}, {18, 12}, {6, 24}, {29, 29}},
        {{0, 0}, {18, 12}, {6, 24}, {12, 18}, {24, 6}, {29, 29}},
        {{0, 0}, {18, 12}, {6, 24}, {24, 6}, {12, 18}, {29, 29}},
        {{0, 0}, {18, 12}, {12, 18}, {6, 24}, {24, 6}, {29, 29}},
        {{0, 0}, {18, 12}, {12, 18}, {24, 6}, {6, 24}, {29, 29}},
        {{0, 0}, {18, 12}, {24, 6}, {6, 24}, {12, 18}, {29, 29}},
        {{0, 0}, {18, 12}, {24, 6}, {12, 18}, {6, 24}, {29, 29}},
        {{0, 0}, {24, 6}, {6, 24}, {12, 18}, {18, 12}, {29, 29}},
        {{0, 0}, {24, 6}, {6, 24}, {18, 12}, {12, 18}, {29, 29}},
        {{0, 0}, {24, 6}, {12, 18}, {6, 24}, {18, 12}, {29, 29}},
        {{0, 0}, {24, 6}, {12, 18}, {18, 12}, {6, 24}, {29, 29}},
        {{0, 0}, {24, 6}, {18, 12}, {6, 24}, {12, 18}, {29, 29}},
        {{0, 0}, {24, 6}, {18, 12}, {12, 18}, {6, 24}, {29, 29}}
    };
    
    int totalDist, dist, bestResult;
    map<int, int> cache;
    int key;
    bestResult = 1'000'000;
    for (const vector<array<int, 2>>& path : all_paths) {
        totalDist = 0;
        for (int i = 0 ; i < path.size() - 1 ; ++i) {
            key = path[i][0] << 15 | path[i][1] << 10 | path[i+1][0] << 5 | path[i+1][1];
            if (cache.find(key) != cache.end()) {
                dist = cache[key];
            } 
            else {
                #ifdef DEBUG_COUNT
                debug_pathFinderCount += 1;
                #endif
                dist = pathFinder(maze, path[i], path[i+1]);
                cache[key] = dist; 
                cache[path[i+1][0] << 15 | path[i+1][1] << 10 | path[i][0] << 5 | path[i][1]] = dist; 
            }
            if (dist == 1'000'000) {
                totalDist = 1'000'000'000;
                break;
            }
            totalDist += dist;
        }
        if (totalDist < bestResult) {
            bestResult = totalDist;
        }
    }
    if (bestResult >= 1'000'000)
        return 0;
    return bestResult;
}


int fitness_4(const array<array<bool, size_>, size_>& maze) {
    
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

    array<int, 24> pathDist;
    pathDist.fill(0);

    // precompute pair-wise distances
    for (const auto& [key, value] : points2pathIdxs) {
        dist = pathFinder(maze, key[0], key[1]);
        // fill in paths
        #pragma clang loop vectorize(assume_safety)
        for (const int& idx : value) {
            pathDist[idx] += dist;
        }
    }
    bestResult = *min_element(pathDist.begin(), pathDist.end());
    if (bestResult >= 1'000'000)
        return 0;
    return bestResult;
}


void saveMazes_2(vector<array<array<bool, size_>, size_>>& mazes, string label) {
    ofstream myfile;
    myfile.open ("genetic_3_results_save_2.txt");
    myfile << label;
    for (auto&m : mazes) {
        myfile << m << '\n';
    }
    myfile.close();
}


// void saveMatingEventStats(vector<vector<int>>& stats) {
void saveMatingEventStats(array<array<int, 7>, totalMatingEvents_>& stats) {
    ofstream myfile;
    myfile.open ("matingEventStats.txt");
    for (auto& row : stats) {
        myfile << row << '\n';
    }
    myfile.close();
}

// void saveGenerationStats(vector<vector<int>>& stats) {
void saveGenerationStats(array<array<int, populationSize_>, generations_>& stats) {
    ofstream myfile;
    myfile.open ("generationStats.txt");
    for (auto& row : stats) {
        for (auto& n : row) {
            myfile << n << ", ";
        }
        myfile << '\n';
    }
    myfile.close();
}

#define USE_REFERENCES 1;

const int NUM_THREADS_ = 7;
typedef array<array<array<bool, size_>, size_>, populationSize_> populationType;
typedef array<int, 7> inputType;
typedef array<int, 7> outputType;
std::array<int, 7> threadInputs;

std::atomic<int> readyCount (0);
std::atomic<int> iterCount (-1);

/**** VERSION 2 */
std::mutex m;
std::mutex m_end;
std::condition_variable cond;
std::condition_variable cond_cc;
std::atomic<int> num_threads;
std::atomic<int> waiting_for_increment;
std::atomic<int> global_cc;
std::mutex barrier_mutex;
std::atomic<bool> barrier_completed{false};
std::atomic<bool> termination_flag_{false};
std::condition_variable barrier_cv;

void barrier_init(int n) {
    num_threads.store(n);
    waiting_for_increment.store(0);
    barrier_completed.store(false);
    global_cc.store(-1);
}

void barrier_wait(int cc) {
    std::unique_lock<std::mutex> lock(m);
    cond_cc.wait(lock, [cc]{
        return global_cc.load() == cc;
    });
    
    waiting_for_increment++;
    
    if (waiting_for_increment.load() == num_threads) {
        cond.notify_all();
    } else {
        cond_cc.notify_all();
        cond.wait(lock);
    }
}

void barrier_wait(int cc, long tid) {
    std::unique_lock<std::mutex> lock(m);
    cond_cc.wait(lock, [cc, tid]{
        std::cout << "thread " << tid << " (" << cc << ") waiting cc check (" << global_cc.load() << ") \n";
        return global_cc.load() == cc;
    });
    
    std::cout << "thread " << tid << " (" << cc << ") free \n";
    
    waiting_for_increment++;
    
    if (waiting_for_increment.load() == num_threads) {
        std::cout << "thread " << tid << " (" << cc << ") signaling condition...\n";
        cond.notify_all();
    } else {
        std::cout << "thread " << tid << " (" << cc << ") waiting for others \n";
        cond_cc.notify_all();
        cond.wait(lock);
    }
}

void barrier_done(int cc, long tid) {
    std::unique_lock<std::mutex> lock(m);
    waiting_for_increment--;
    if (waiting_for_increment.load() == 0) {
        // cout << "thread " << tid << " sets barrier_completed to true \n";
        barrier_completed.store(true);
        barrier_cv.notify_one();
    }
}

void barrier_exit() {
    std::unique_lock<std::mutex> lock(m);
    num_threads.store(num_threads.load() - 1);
    cond_cc.notify_all(); // for the ahead-of-time n+1 cc threads
    if (waiting_for_increment.load() == num_threads.load()) {
        cond.notify_all();
    }
}

void work(int x, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    int localitercount = 0;
    while(true) {

        // wait until signaled
        // readyCount == 0 ensures that the work won't happen until there's an input from master thread

        while (localitercount != iterCount) {}

        // check if termination flag is set. If yes, return True
        if (termination_flag_) break;

        localitercount += 1;

        // work
        // std::cout << x << std::endl;
        // output1[x] = output2[x] = fitness_3_og(population[input[x]]);
        output1[x] = output2[x] = fitness_4(population[input[x]]);

        // std::this_thread::sleep_for(100ms);
        // std::this_thread::sleep_for (std::chrono::seconds(x));

        // end of loop
        readyCount -= 1;
    }
    barrier_exit();
}

void work_3(int x, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    int localitercount = 0;
    while(true) {

        // wait until signaled
        // readyCount == 0 ensures that the work won't happen until there's an input from master thread

        //:) std::cout << x << " waiting " << std::endl;

        barrier_wait(localitercount);

        // check if termination flag is set. If yes, return True
        if (termination_flag_.load()) break;

        localitercount += 1;

        // work
        //:) std::cout << x << " got work " << std::endl;
        // output1[x] = output2[x] = fitness_3_og(population[input[x]]);
        output1[x] = output2[x] = fitness_4(population[input[x]]);


        // std::this_thread::sleep_for(100ms);
        // std::this_thread::sleep_for (std::chrono::seconds(x));

        // end of loop
        // readyCount--;

        barrier_done(localitercount, x);
    }
    barrier_exit();
}

void work2(int x, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    int localitercount = 0;
    std::vector<double> timings;
    int result;

    while(true) {
        while (localitercount != iterCount) {}

        if (termination_flag_) break;

        localitercount += 1;

        // Start timing
        auto start = std::chrono::high_resolution_clock::now();

        // The line we want to time
        result = fitness_4(population[input[x]]);

        // End timing
        auto end = std::chrono::high_resolution_clock::now();

        output1[x] = output2[x] = result;

        // Calculate duration in microseconds
        std::chrono::duration<double, std::micro> duration = end - start;
        timings.push_back(duration.count());

        readyCount -= 1;
    }

    // Calculate and print average time
    if (!timings.empty()) {
        double average = std::accumulate(timings.begin(), timings.end(), 0.0) / timings.size();
        std::cout << "Average time for the timed statement: " << average << " microseconds" << std::endl;
    }
}

void thread_pool_init(std::array<std::thread, NUM_THREADS_>& workers, const populationType& population, const inputType& input, outputType& output1, outputType& output2) {
    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i] = std::thread(work_3, i, ref(population), ref(input), ref(output1), ref(output2));
    }
    // cout << " initialization done";
}

static inline void thread_pool_launch_iteration() {
    readyCount = NUM_THREADS_;
    iterCount += 1;
}

static inline void thread_pool_wait_iteration_completion() {
    while (readyCount != 0) {}
}

void thread_pool_terminate(std::array<std::thread, NUM_THREADS_>& workers) {
    termination_flag_ = true;

    readyCount = NUM_THREADS_;
    iterCount += 1;

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i].join();
    }
}

// only save the mazes with fitness > 0
// r0: original
// r1: change the double loop to a single loop
void runner_4() {
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
    thread_pool_init(workers, ref(population), ref(indices), ref(fitnesses), ref(sortedFitnesses));
    
    auto randomDevice = mt19937{random_device{}()};
    const int numIterations_ = generations_ * matingEventsPerGeneration_;
    random_device sample_dev;
    mt19937 sample_rng(sample_dev());
    uniform_int_distribution<mt19937::result_type> sample_getNum(0,populationSize_-1); // distribution in range [1, 10**6]
    
    for (int g = 0 ; g < numIterations_ ; ++g) {

            for (int i = 0 ; i < 7 ; i++) {
                indices[i] = sample_getNum(sample_rng);
            }

            thread_pool_launch_iteration();


            thread_pool_wait_iteration_completion();

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
            uniformCrossover(m1, m2);
            uniformMutation(m1);
            uniformMutation(m2);
            // uniformMutation2(m1);
            // uniformMutation2(m2);
            

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

        // }
        cout << "\n sortedFitnesses: " << sortedFitnesses;
    //    label = "\n";
    //    label += "-----------------------------";
    //    label += (" end of generation " + to_string(g+1) + ' ');
    //    label += "-----------------------------";
    //    label += '\n';
    //    cout << label;
    }
    thread_pool_terminate(workers);
    // vector<array<array<bool, size_>, size_>> niceOnes;
    // for (auto&p : population) {
    //     auto result = fitness_3(p);
    //     if (result > 0) {
    //         // cout << "\n fitness: " << result;
    //         niceOnes.push_back(p);
    //     }
    // }
    #ifdef DEBUG_COUNT
    cout << "pathfinder call count: " << debug_pathFinderCount << endl;
    #endif
    // saveMazes_2(niceOnes, label);
    // saveGenerationStats(generationStats);
    // saveMatingEventStats(matingEventStats);
}

void runner_5() {
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

            barrier_completed.store(false);

            for (int i = 0 ; i < 7 ; i++) {
                indices[i] = sample_getNum(sample_rng);
            }

            global_cc++;
            cond_cc.notify_all();
            {
                std::unique_lock<std::mutex> lock(m);
                barrier_cv.wait(lock, []{ return barrier_completed.load(); });
            }
            // while (!barrier_completed.load()) {
            //    std::this_thread::sleep_for(std::chrono::seconds(1)); 
            // }
            // std::this_thread::sleep_for(std::chrono::seconds(1));

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
            uniformCrossover(m1, m2);
            uniformMutation(m1);
            uniformMutation(m2);
            // uniformMutation2(m1);
            // uniformMutation2(m2);
            

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

        // }
        // cout << "\n sortedFitnesses: " << sortedFitnesses;
    //    label = "\n";
    //    label += "-----------------------------";
    //    label += (" end of generation " + to_string(g+1) + ' ');
    //    label += "-----------------------------";
    //    label += '\n';
    //    cout << label;
    }
    // cout << "END OF MASTER THREAD LOOP \n";
    // thread_pool_terminate(workers);
    termination_flag_.store(true);
    global_cc++;
    cond_cc.notify_all();

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i].join();
    }
    // vector<array<array<bool, size_>, size_>> niceOnes;
    // for (auto&p : population) {
    //     auto result = fitness_3(p);
    //     if (result > 0) {
    //         // cout << "\n fitness: " << result;
    //         niceOnes.push_back(p);
    //     }
    // }
    #ifdef DEBUG_COUNT
    cout << "pathfinder call count: " << debug_pathFinderCount << endl;
    #endif
    // saveMazes_2(niceOnes, label);
    // saveGenerationStats(generationStats);
    // saveMatingEventStats(matingEventStats);
}

// g++ -std=c++17 -O3 -Wl,--stack=16777216 -pthread genetic_3.cpp -o a
int main() {
    init();
    runner_5();
    return 0;
}
