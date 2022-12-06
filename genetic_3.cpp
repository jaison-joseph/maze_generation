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

void init() {
    for (int i = 0 ; i < size_ ; ++i) {
        pathFinder_lk_[i].fill(1'000'000);
    }
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

// easier to divide by powers of 2
void uniformMutation(array<array<bool, size_>, size_>& m) {
     // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> getNum(0,64); // distribution in range [0, 64]
    for (auto&i : m) {
        for (auto& j : i) {
            if ((float(getNum(rng))/64.0f) <= Pm_) {
                j = !j;
            }
        }
    }
}

void uniformCrossover(array<array<bool, size_>, size_>& m1, array<array<bool, size_>, size_>& m2) {
     // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    bool foo;
    uniform_int_distribution<mt19937::result_type> getNum(0,64); // distribution in range [1, 64]
    for (int i = 0 ; i < size_ ; ++i) {
        for (int j = 0 ; j < size_ ; ++j) {
            if (float(getNum(rng))/64.0f <= Pc_) {
                foo = m1[i][j];
                m1[i][j] = m2[i][j];
                m2[i][j] = foo;   
            }
        }
    }
}

// returns min dist from entrance -> exit in maze
// if no path, returns 1'000'000
int pathFinder(
    array<array<bool, size_>, size_>& maze,
    array<int, 2> start, 
    array<int, 2> end
) {
    // fitness() already checks the same, this is (I think) redundant
    // if (maze[start[0]][start[1]]) {
    //     return 0;
    // }
    // if (maze[end[0]][end[1]]) {
    //     return 0;
    // }

    array<array<int, size_>, size_> lk = pathFinder_lk_;
    lk[start[0]][start[1]] = 0;

    vector<array<int, 2>> q = {{start}};
    array<int, 2> pt;
    array<int, 2> foo;
    int dist;

    while (q.size() > 0) {
        pt = q.back();
        q.pop_back();
        dist = lk[pt[0]][pt[1]];
        dist += 1;
        
        if (dist > lk[end[0]][end[1]]) 
            continue;

        foo = {pt[0]+1, pt[1]};
        if (foo[0] < size_ && lk[foo[0]][foo[1]] > dist && !maze[foo[0]][foo[1]]) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo != end)
                q.push_back(foo);
        }

        foo = {pt[0], pt[1]+1};
        if (foo[1] < size_ && lk[foo[0]][foo[1]] > dist && !maze[foo[0]][foo[1]]) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo != end)
                q.push_back(foo);
        }
        foo = {pt[0]-1, pt[1]};
        if (foo[0] >= 0 && lk[foo[0]][foo[1]] > dist && !maze[foo[0]][foo[1]]) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo != end)
                q.push_back(foo);
        }

        foo = {pt[0], pt[1]-1};
        if (foo[1] >= 0 && lk[foo[0]][foo[1]] > dist && !maze[foo[0]][foo[1]]) {
            lk [foo[0]] [foo[1]] = dist;
            if (foo != end)
                q.push_back(foo);
        }
    }

    // cout << "\n lk: \n" << lk;

    return lk[end[0]][end[1]] == 1'000'000 ? 0 : lk[end[0]][end[1]];
}

// so apparently I overthought the fitness function godammit
// maze[i][j] is true => wall, false => empty
// this version adds cache 
int fitness_3(array<array<bool, size_>, size_>& maze) {
    
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
                dist = pathFinder(maze, path[i], path[i+1]);
                cache[{path[i], path[i+1]}] = dist; 
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

// maze[i][j] is true => wall, false => empty
int fitness_debug(array<array<bool, size_>, size_>& maze) {
    
   if (maze[entrance_[0]][entrance_[1]]) {
        cout << "\n entrance blocked";
        return 0;
    }
    if (maze[exit_[0]][exit_[1]]) {
        cout << "\n exit blocked";
        return 0;
    }
    for (auto& c : checkpoints_) {
        if (maze[c[0]][c[1]]) {
            cout << "\n checkpoint blocked";
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
    map<
        array<array<int, 2>, 2>, 
        int
    > cache;
    int ct = 0;
    do {
        path.clear();
        path.push_back(entrance_);
        path.insert(path.end(), c.begin(), c.end());
        path.push_back(exit_);
        totalDist = 0;
        pathFailed = false;
        for (int i = 0 ; i < path.size() - 1 ; ++i) {
            if (cache.find({path[i], path[i+1]}) != cache.end()) {
                dist = cache[{path[i], path[i+1]}];
                cout << ct++ << "hit \n";
            }
            else if (cache.find({path[i+1], path[i]}) != cache.end()) {
                dist = cache[{path[i+1], path[i]}];
                cout << ct++ << "hit \n";
            }
            else {
                dist = pathFinder(maze, path[i], path[i+1]);
                cache[{path[i], path[i+1]}] = dist; 
            }
            dist = pathFinder(maze, path[i], path[i+1]);
            if (dist == 0) {
                pathFailed = true;
                break;
            }
            totalDist += dist;
            // cout << 1; // just to see how successful each of these paths are; to decide whether its worth caching results
        }
        // cout << '\n';
        if (!pathFailed) {
            checkpointDistances.push_back(totalDist);
        }
    } while (next_permutation(c.begin(), c.end()));

    for(auto [key, val] : cache) {
        cout    << "\n "
                << '['
                << "(" << key[0][0] << ", " << key[0][1] << ")"
                << ", "
                << "(" << key[1][0] << ", " << key[1][1] << ")"
                << ']'
                << " -> " << val;
    }

    if (checkpointDistances.size() == 0) {
        cout << "\n no path that traverses all checkpoints";
        return 0;
    }
    // min element returns an iterator, so we derefence it
    bestResult = *min_element(checkpointDistances.begin(), checkpointDistances.end());

    if (bestResult == 0) {
        cout << "\n no path that traverses all checkpoints";
        return 0;
    }
    return bestResult;
}

void testUniformMutation() {
    array<array<bool, size_>, size_> m;
    for (auto&i : m) {
        i.fill(false);
    }
    cout << "before mutation: \n" << m;
    uniformMutation(m);
    cout << "after mutation: \n" << m;
}

void testUniformCrossover() {
    array<array<bool, size_>, size_> m1, m2;
    for (auto&i : m1) 
        i.fill(false);
    for (auto&i : m2) 
        i.fill(true);
    cout << "before mutation: \n" << m1 << "\n " << m2;
    uniformCrossover(m1, m2);
    cout << "before mutation: \n" << m1 << "\n " << m2;
}

void testrandomNumberGenerator() {
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> getNum(1,1000);
    cout << "printing 100 numbers in the range[1, 100]: ";
    for (int i = 0 ; i < 100 ; ++i) {
        cout << float(getNum(rng))/1000.0f << " ," ;
    }
}

void testFitness_2() {
    // converted later into a 2d bool array; 
    // the purpose of this is to design the maze to be tested, 
    // the 5's are the checkpoint locations
    array<array<int, size_>, size_> designer = {
        array<int, size_> {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 5, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},
        array<int, size_> {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,},   
        array<int, size_> {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0,}
    };

    array<array<bool, size_>, size_> maze;
    for (int i = 0 ; i < size_ ; ++i)
        for (int j = 0 ; j < size_ ; ++j)
            maze[i][j] = (designer[i][j] == 1);

    cout << "\n pathfinder result: " << pathFinder(maze, entrance_, exit_);
    cout << "\n fitness result: " << fitness_3(maze);
}

void testShuffle() {
    array<array<int, 2>, 4> c = checkpoints_;
    int ct = 0;
    do {
        cout << ++ct << ". ";
        for (array<int, 2>& i : c) {
            cout << i << ", ";
        }
        // cout << c[0] << ", " << c[1] << ", " << c[2] << ", " << c[3] << '\n';
        cout << '\n';
    } while (next_permutation(c.begin(), c.end()));
}

void testPathFinder() {
    auto m = genMaze();
    auto result = pathFinder(m, entrance_, exit_);
    cout << "\n path dist: " << result;
}

void saveMazes(array<array<array<bool, size_>, size_>, populationSize_>& mazes, string label) {
    ofstream myfile;
    myfile.open ("genetic_3_results.txt");
    myfile << label;
    for (auto&m : mazes) {
        myfile << m << '\n';
    }
    myfile.close();
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

// no heap allocation
// no stats collection
void runner_3() {
    array<array<array<bool, size_>, size_>, populationSize_> population;
    for (auto& p : population)
        p = genMaze();
    array<int, 7> fitnesses;
    array<int, 7> sortedFitnesses;
    vector<int> indices;
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    // indices of the two fittest members, then the two weakest members
    int i1, i2, i3, i4;
    string label;
    array<int, populationSize_> allIndices;
    // fills allIndices with numbers starting from 0
    // https://stackoverflow.com/questions/4803898/fill-in-the-int-array-from-zero-to-defined-number
    iota(allIndices.begin(), allIndices.end(), 0);
    for (int g = 0 ; g < generations_ ; ++g) {
        for (int m = 0 ; m < matingEventsPerGeneration_ ; ++m) {
        //    cout << m;
            indices.clear();
            // https://en.cppreference.com/w/cpp/algorithm/sample
            // randomly select 7 indices from the population array
            sample(
                allIndices.begin(), allIndices.end(), 
                back_inserter(indices),
                7, 
                mt19937{random_device{}()}
            );

            // cout << "\n indices: ";
            // for (auto&x : indices)
            //     cout << x << ", ";
            
            // find the fitness of the 7 population members

            // auto start = chrono::steady_clock::now();
            for (int i = 0 ; i < 7 ; ++i) {
                sortedFitnesses[i] = fitnesses[i] = fitness_3(population[indices[i]]);
            }
            // auto end = chrono::steady_clock::now();
            // auto diff = end-start;
            // cout<<"\n time: "<< chrono::duration<double, milli>(diff).count()<<" ms";
            
            // https://stackoverflow.com/questions/9025084/sorting-a-vector-in-descending-order
            // find the fittest two members
            // the fitnesses are sorted in descending order
            sort(sortedFitnesses.begin(), sortedFitnesses.end(), greater<int>());

            // cout << "\n fitnesses: ";
            // for (auto&x : fitnesses)
            //     cout << x << ", ";

            // for (auto&x : sortedFitnesses)
            //     cout << x << ", ";

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

        }
        cout << "\n sortedFitnesses: " << sortedFitnesses;
    //    label = "\n";
    //    label += "-----------------------------";
    //    label += (" end of generation " + to_string(g+1) + ' ');
    //    label += "-----------------------------";
    //    label += '\n';
    //    cout << label;
    }
    saveMazes(population, label);
    // saveGenerationStats(generationStats);
    // saveMatingEventStats(matingEventStats);
}

// only save the mazes with fitness > 0
void runner_4() {
    array<array<array<bool, size_>, size_>, populationSize_> population;
    for (auto& p : population)
        p = genMaze();
    array<int, 7> fitnesses;
    array<int, 7> sortedFitnesses;
    vector<int> indices;
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    // indices of the two fittest members, then the two weakest members
    int i1, i2, i3, i4;
    string label;
    array<int, populationSize_> allIndices;
    // fills allIndices with numbers starting from 0
    // https://stackoverflow.com/questions/4803898/fill-in-the-int-array-from-zero-to-defined-number
    iota(allIndices.begin(), allIndices.end(), 0);
    for (int g = 0 ; g < generations_ ; ++g) {
        for (int m = 0 ; m < matingEventsPerGeneration_ ; ++m) {
        //    cout << m;
            indices.clear();
            // https://en.cppreference.com/w/cpp/algorithm/sample
            // randomly select 7 indices from the population array
            sample(
                allIndices.begin(), allIndices.end(), 
                back_inserter(indices),
                7, 
                mt19937{random_device{}()}
            );

            // cout << "\n indices: ";
            // for (auto&x : indices)
            //     cout << x << ", ";
            
            // find the fitness of the 7 population members

            // auto start = chrono::steady_clock::now();
            for (int i = 0 ; i < 7 ; ++i) {
                sortedFitnesses[i] = fitnesses[i] = fitness_3(population[indices[i]]);
            }
            // auto end = chrono::steady_clock::now();
            // auto diff = end-start;
            // cout<<"\n time: "<< chrono::duration<double, milli>(diff).count()<<" ms";
            
            // https://stackoverflow.com/questions/9025084/sorting-a-vector-in-descending-order
            // find the fittest two members
            // the fitnesses are sorted in descending order
            sort(sortedFitnesses.begin(), sortedFitnesses.end(), greater<int>());

            // cout << "\n fitnesses: ";
            // for (auto&x : fitnesses)
            //     cout << x << ", ";

            // for (auto&x : sortedFitnesses)
            //     cout << x << ", ";

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

        }
        cout << "\n sortedFitnesses: " << sortedFitnesses;
    //    label = "\n";
    //    label += "-----------------------------";
    //    label += (" end of generation " + to_string(g+1) + ' ');
    //    label += "-----------------------------";
    //    label += '\n';
    //    cout << label;
    }
    vector<array<array<bool, size_>, size_>> niceOnes;
    for (auto&p : population) {
        auto result = fitness_3(p);
        if (result > 0) {
            cout << "\n fitness: " << result;
            niceOnes.push_back(p);
        }
    }
    saveMazes_2(niceOnes, label);
    // saveGenerationStats(generationStats);
    // saveMatingEventStats(matingEventStats);
}

void testSaveMazes() {
    array<array<array<bool, size_>, size_>, populationSize_> mazes ;
    for (auto&m : mazes)
        m = genMaze();
    saveMazes(mazes, string("hello there"));
}

// g++ -std=c++17 -O3 -Wl,--stack=16777216 -pthread genetic_3.cpp -o a
int main() {
    init();
    // testPathFinder();
    // auto start = chrono::steady_clock::now();
    runner_4();
    // auto end = chrono::steady_clock::now();
    // auto diff = end - start;
    // cout << "\n time: " << chrono::duration<double, milli>(diff).count() << " ms";
    return 0;
}