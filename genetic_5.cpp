// START OF GENETIC_5.CPP

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
#include <queue>
#include <utility> // for pair
#include <assert.h>

#if _WIN32
#include <atomic>
#include <mutex>
#include <condition_variable>
#elif __linux__
#include <atomic>
#include <mutex>
#include <condition_variable>
#endif

using namespace std;

// #define SAVE_MAZES 1

/**
 * TODO:
 * we can divide the probability space of 120 into 7 regions, and each thread can then individually pick a maze from it's subspace
 * this should make thhings a little more faster
 * 
*/

#include "play/task_queue_pi.h"
#include "something.cpp"

vector<pair<int, int>> TaskQueueClient_pi::tasks;
mutex TaskQueueClient_pi::mutex;
condition_variable TaskQueueClient_pi::cv;
bool TaskQueueClient_pi::termination_flag;
mutex TaskQueueClient_pi::output_mutex;
pair<int, int> TaskQueueClient_pi::termination_symbol {-1, -1};

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
typedef array<pair<int, int>, 7> outputType;
array<int, 7> threadInputs;

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

void saveMazes(vector<array<array<bool, size_>, size_>>& mazes, string label) {
    ofstream myfile;
    myfile.open ("genetic_3_results_save_2.txt");
    myfile << label;
    for (auto&m : mazes) {
        myfile << m << '\n';
    }
    myfile.close();
}


void init() {
    for (int i = 0 ; i < size_ ; ++i) {
        pathFinder_lk_[i].fill(1'000'000);
    }

    /**
     * 
     * Does order of this impact performance? If so, by how much
    */
	/**
	The maze has 1 start point, one end point, and 4 checkpoints
	start point: (0, 0)
	end point: (29, 29)
	check points: (6, 24), (24, 6), (18, 12), (12, 18)

	If we consider all permutations of the checkpoints (4!), that gives us 24 ways to traverse the checkpoints, 
	then there are 24 ways to get to the end of the maze; these are the values of this map (numbers 0 - 23)

	For each maze, we need to compute the shortest distance from start to finish

	To that end, we don't run a path finder across the entire maze. Instead, we compute distance between 
	start/end/check-points and check-points (one can see traversing the maze as simply starting at start, 
	moving between checkpoints, and then moving to end) and then keep a map that stores total distance for
	all possible 24 ways (this is pathDist in fitness_4)

	The map below is telling us, given a key e.g. {{{ 0,  0}, { 6, 24}}}:
		the key represents the path from (0, 0) to a checkpoint (6, 24)
		the values of this key ({0, 1, 2, 3, 4, 5}) tell us which of the 24 possible paths
			traverse from (0, 0) to (6, 24); the paths 0, 1, 2, 3, 4, 5 use them. For these paths, 
			we add the distance from (0, 0) to (6, 24) to the total result of paths 0, 1, 2, 3, 4, 5

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

std::condition_variable barrier_cv;       // for lock-step

void work(int x, const populationType& population, outputType& output, int& tasks_consumed) {

    pair<int, int> termination_symbol {-1, -1};
	TaskQueueClient_pi q;

	while (true) {

		pair<int, int> task = q.get_task(); // blocking
		if (task.second == -1) break; // terminated

		// work
		task.first = fitness_4(population[task.second]);

		// store the output
		{
            lock_guard<mutex> lock(TaskQueueClient_pi::output_mutex);
            output[tasks_consumed++] = task;
		}
		
		// wake up master thread if 'this' thread is last to finish in lock-step
		if (tasks_consumed == NUM_THREADS_) {
			barrier_cv.notify_one();
		}
	}
}

void thread_pool_init(array<thread, NUM_THREADS_>& workers, const populationType& population, outputType& output1, int& tasks_consumed) {
    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i] = thread(work, i, ref(population), ref(output1), ref(tasks_consumed));
		set_thread_max_priority(std::ref(workers[i]));
		pin_cpu(std::ref(workers[i]), i);
    }
}

void runner() {
    array<array<array<bool, size_>, size_>, populationSize_> population;
    for (auto& p : population)
        p = genMaze();
	
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    // indices of the two fittest members, then the two weakest members
    int i1, i2, i3, i4;
    string label;
    
	// random number gen stuff
    auto randomDevice = mt19937{random_device{}()};
    const int numIterations_ = generations_ * matingEventsPerGeneration_;
    random_device sample_dev;
    mt19937 sample_rng(sample_dev());
    uniform_int_distribution<mt19937::result_type> sample_getNum(0,populationSize_-1); // distribution in range [1, 10**6]

	// for task queue
	// queue element format: <fitnesses, indices>
	// before, we had 2 separate NUM_THREADS_ size arrays for fitnesses and indices
	TaskQueueClient_pi q;

	// to collect processed output by worker threads
	array<pair<int, int>, 7> outputs;

	// for synchronization
	int tasks_consumed = NUM_THREADS_;

	// starting up thread pool
	array<thread, NUM_THREADS_> workers;

    thread_pool_init(workers, ref(population), ref(outputs), ref(tasks_consumed));
    
    for (int g = 0 ; g < numIterations_ ; ++g) {

		// assert(tasks_consumed == NUM_THREADS_);
		tasks_consumed = 0;

		// add tasks for worker threads
        for (int i = 0 ; i < NUM_THREADS_; i++) {
            // the threads are listening for additions to task queue
			q.add_task(0, sample_getNum(sample_rng));
        }

		// wait for all tasks to be completed
		// while (tasks_consumed != NUM_THREADS_) {
		// 	this_thread::sleep_for(chrono::microseconds(1));
		// }
		{
            std::unique_lock<std::mutex> lock(TaskQueueClient_pi::output_mutex);

            barrier_cv.wait(lock, [&]{ return tasks_consumed == NUM_THREADS_; });
            // barrier_cv.wait(lock);
        }
        
        // auto end = chrono::steady_clock::now();
        // auto diff = end-start;
        // cout<<"\n time: "<< chrono::duration<double, milli>(diff).count()<<" ms";
        
        
		// Sort indices based on fitness values (in descending order)
		// sort(indices_by_fitness.begin(), indices_by_fitness.end(), 
		// 	[&fitnesses](int a, int b) { return fitnesses[a] > fitnesses[b]; });
		sort(
			outputs.begin(), outputs.end(), 
			[](const pair<int, int>& a, const pair<int, int>& b){ return a.first > b.first; }
		);

		// Now indices_by_fitness[0] is the index of the largest element,
		// indices_by_fitness[1] is the index of the second largest, etc.

		// Get the indices directly
		i1 = outputs[0].second; // Index of largest
		i2 = outputs[1].second; // Index of second largest
		i3 = outputs[6].second; // Index of smallest
		i4 = outputs[5].second; // Index of second smallest

        // cout << "\n i1: " << i1 << " | i2: " << i2 ;

        // m1 and m2 are copies of the two fittest mazes
        m1 = population[i1];
        m2 = population[i2];

        // cout << "\n m1 and m2 BEFORE evolution: \n";
        // cout << m1 << "\n\n" << m2;

        // evolution ....
        uniformMutationAndCrossover(m1, m2);
        

        // cout << "\n m1 and m2 AFTER evolution: \n";
        // cout << m1 << "\n\n" << m2;


        // cout << "\n i3: " << i3 << " | i4: " << i4 ;

        // overwrite the weakest two with the modified fittest two
        population[i3] = m1;
        population[i4] = m2;

        // cout << "sortedFitnesses: "
		//  << outputs[0].first << ", "
		//  << outputs[1].first << ", "
		//  << outputs[2].first << ", "
		//  << outputs[3].first << ", "
		//  << outputs[4].first << ", "
		//  << outputs[5].first << ", "
		//  << outputs[6].first << endl;
		
    }
    
    // terminate workers, join threads
    q.terminate_threads();

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i].join();
    }

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

// END OF GENETIC_5.CPP