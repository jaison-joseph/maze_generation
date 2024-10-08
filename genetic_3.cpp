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

// the length and width of the mazes
const int size_= 30;

// the number of mazes in a population
const int populationSize_ = 120;

// probability of uniform crossover
const float Pc_ = 0.05;

// probability of uniform mutation
const float Pm_ = 0.01;

// initial probability used to create the starting population
const float fill_ = 0.05;

// total number of generations for the program
const int generations_ = 250;

// each iteration of algorithm is a matingEvent, 
// a bunch of such events constitutes a generation
const int matingEventsPerGeneration_ = 2000;

// helper constant
const int totalMatingEvents_ = generations_ * matingEventsPerGeneration_;

// start indices of the entire maze
const array<int, 2> entrance_ = {0, 0};

// end indices of the entire maze
const array<int, 2> exit_ = {size_-1, size_-1};

// the checkpoints along the maze that the algorithm must have a path along
const array<array<int, 2>, 4> checkpoints_ = {
    array<int, 2> {6, 24},
    array<int, 2> {12, 18},
    array<int, 2> {18, 12},
    array<int, 2> {24, 6}
};

// this stores the default values used for the depth-first search
array<array<int, size_>, size_> pathFinderLookupDefault_;

// called at the start of the run function of the program
// used to initialize all variables
void init() {
    for (int i = 0 ; i < size_ ; ++i) {
        pathFinderLookupDefault_[i].fill(1'000'000);
    }
}

// helper: overload of << for a maze
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

// helper: overload for lookup in the pathfinder
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

// helper: overload of << for an int array of size 2 used as indices in this program
ostream& operator<<(ostream& os, array<int, 2>& m) {
    os << '(' << m[0] << ", " << m[1] << ')';
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// helper: overload of << for an int array of size 7 in the mating event population selection for 'evolution'
ostream& operator<<(ostream& os, array<int, 7>& m) {
    os << m[0] << ", " << m[1] << ", " << m[2] << ", " << m[3] << ", " << m[4] << ", " << m[5] << ", " << m[6];
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// helper: overload of << for vector of int type
ostream& operator<<(ostream& os, vector<int>& x) {
    for (int& i : x) {
        os << i << ", ";
    }
    os << '\n';
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

// this function is used to generate the initial popluation of mazes before we perform evolution
array<array<bool, size_>, size_> genMaze() {
    // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> getNum(1,1000); // distribution in range [1, 1000]
    array<array<bool, size_>, size_> maze;
    for (auto&i : maze) {
        for (auto&j : i) {
            j = (float(getNum(rng))/1000.0f <= fill_);
        }
    }
    return maze;
}

// this function flips the cells between two mazes provided as arguments
// with a probability of 0.01
void uniformMutation(array<array<bool, size_>, size_>& m) {
     // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    uniform_int_distribution<mt19937::result_type> getNum(1,100); // distribution in range [0, 64]
    for (auto&i : m) {
        for (auto& j : i) {
            if ((float(getNum(rng))/100.0f) <= Pm_) {
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
    uniform_int_distribution<mt19937::result_type> getNum(1,100); // distribution in range [1, 64]
    for (int i = 0 ; i < size_ ; ++i) {
        for (int j = 0 ; j < size_ ; ++j) {
            if (float(getNum(rng))/100.0f <= Pc_) {
                foo = m1[i][j];
                m1[i][j] = m2[i][j];
                m2[i][j] = foo;   
            }
        }
    }
}

// returns min dist between two points in a maze
// the algorithm is DFS, but it doesn't return immediately as soon as it reaches the exit 
// since other, shorter paths might exist

// TODO: replace with bfs (should work theoretically, but DFS might fare better when mazes are non-sparse)
// if no path, returns 0
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

    array<array<int, size_>, size_> lk = pathFinderLookupDefault_;
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

// maze[i][j] is true => wall, false => empty
// this version adds cache 

/**
 * 
 * The goal of this function is to find the fitness of the maze
 * The fitness is simply the shortest path from the upper left corner (entracne)
 * to the lower right corner (exit) of the maze that goes through all the checkpoints
 * 
 * Since we use a modified depth-first search (DFS*) to find the distance between two points in a maze,
 * if the maze is sparsely filled, the "breadth" of the search beyond a certain depth makes
 * the DFS "slow" for our purposes. The solution instead is to:
 * 
 * Replace (entrance)  < --- DFS* -- >  (exit)
 * 
 *          with
 * 
 * min(
 *      (entrance) <---DFS*--> (chkpt 1) <---DFS*--> (chkpt 2)  <---DFS*--> (chkpt 3)  <---DFS*--> (chkpt 4)  <---DFS*--> (exit),
 *      (entrance) <---DFS*--> (chkpt 2) <---DFS*--> (chkpt 1)  <---DFS*--> (chkpt 3)  <---DFS*--> (chkpt 4)  <---DFS*--> (exit),
 *      .... (we try all permutations of the 4 checkpoints between the entrance and exit)
 * )
 * 
 * The above is much more performant compared to the simple (entrance)  < --- DFS* -- >  (exit) search
 * 
 * Since the modified algorithm for calculating fitness has a lot of redundancies,
 * like (chkpt 1) <---DFS*--> (chkpt 2) and (chkpt 2) <---DFS*--> (chkpt 1), 
 * we cache such intermediate results to avoid redundant calculations
*/
int fitness(array<array<bool, size_>, size_>& maze) {
    
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

// helper method to write the results to a text file
void saveMazes_2(vector<array<array<bool, size_>, size_>>& mazes, string label) {
    ofstream myfile;
    myfile.open ("genetic_3_results_save_2.txt");
    myfile << label;
    for (auto&m : mazes) {
        myfile << m << '\n';
    }
    myfile.close();
}


// helper method for debugging
void saveMatingEventStats(array<array<int, 7>, totalMatingEvents_>& stats) {
    ofstream myfile;
    myfile.open ("matingEventStats.txt");
    for (auto& row : stats) {
        myfile << row << '\n';
    }
    myfile.close();
}
// helper method for debugging
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

/**
 * 
 * The heart of the program, here's the outline:
 * 1. initialize the population of mazes (done using the genMaze function)
 * 2. for each mating event:
 *      3. pick 7 mazes out of the population
 *      4. find the fitnesses of the selected 7
 *      5. copy the strongest two, and perform:
 *          6. Uniform crossover
 *             followed by,
 *          7. Uniform mutation
 * 
 *      8. Overwrite the weakest 2 among the selected 7, and overwrite it 
 *          with the (modified) strongest two
 * 9. repeat steps 2-8, for mating events per generation * number of generations
*/
void runner() {
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
                sortedFitnesses[i] = fitnesses[i] = fitness(population[indices[i]]);
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
            // remember that fitness is simply shortest path from entrace to exit via checkpoints
            // so, smaller is better
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
        auto result = fitness(p);
        if (result > 0) {
            cout << "\n fitness: " << result;
            niceOnes.push_back(p);
        }
    }
    saveMazes_2(niceOnes, label);
    // saveGenerationStats(generationStats);
    // saveMatingEventStats(matingEventStats);
}

// g++ -std=c++17 -O3 -Wl,--stack=16777216 -pthread genetic_3.cpp -o a
int main() {
    init();
    // testPathFinder();
    // auto start = chrono::steady_clock::now();
    runner();
    // auto end = chrono::steady_clock::now();
    // auto diff = end - start;
    // cout << "\n time: " << chrono::duration<double, milli>(diff).count() << " ms";
    return 0;
}
