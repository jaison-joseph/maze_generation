#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <random>

using namespace std;

const int8_t size_= 30;
const float Pc_ = 0.05;
const float Pm_ = 0.01;
const float fill_ = 0.05;
const int8_t generations_ = 1;
const int8_t matingEventsPerGeneration_ = 2'000;
const array<int8_t, 2> entrance_ = {0, 0};
const array<int8_t, 2> exit_ = {size_-1, size_-1};
// const array<array<int8_t, 2>, 4> checkpoint8_ts_ = {
//     array<int8_t, 2> {6, 24},
//     array<int8_t, 2> {12, 18},
//     array<int8_t, 2> {18, 12},
//     array<int8_t, 2> {24, 6}
// };
const array<array<int8_t, 2>, 4> checkpoint8_ts_ = {
    array<int8_t, 2> {1, 1},
    array<int8_t, 2> {12, 18},
    array<int8_t, 2> {18, 18},
    array<int8_t, 2> {23, 24}
};

// overload of << for a maze
ostream& operator<<(ostream& os, array<array<bool, size_>, size_>& m)
{
    for (array<bool, size_>&i : m) {
        for (bool j : i) {
            os << j << ", ";
        }
        os << '\n';
    }
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

array<array<bool, size_>, size_> genMaze() {
    // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    uniform_int8_t_distribution<mt19937::result_type> getNum(1,1000); // distribution in range [1, 6]
    array<array<bool, size_>, size_> maze;
    for (auto&i : maze) {
        for (auto&j : i) {
            j = (float(getNum(rng))/1000.0f <= fill_);
        }
    }
    return maze;
}

void uniformMutation(array<array<bool, size_>, size_>& m) {
     // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    random_device dev;
    mt19937 rng(dev());
    uniform_int8_t_distribution<mt19937::result_type> getNum(1,1000); // distribution in range [1, 1000]
    for (auto&i : m) {
        for (auto& j : i) {
            if ((float(getNum(rng))/1000.0f) <= Pm_) {
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
    uniform_int8_t_distribution<mt19937::result_type> getNum(1,1000); // distribution in range [1, 1000]
    for (int8_t i = 0 ; i < size_ ; ++i) {
        for (int8_t j = 0 ; j < size_ ; ++j) {
            if (float(getNum(rng))/1000.0f <= Pc_) {
                foo = m1[i][j];
                m1[i][j] = m2[i][j];
                m2[i][j] = foo;   
            }
        }
    }
}

// maze[i][j] is true => wall, false => empty
int8_t fitness(array<array<bool, size_>, size_>& maze) {
    
    if (maze[entrance_[0]][entrance_[1]]) {
        return 0;
    }
    if (maze[exit_[0]][exit_[1]]) {
        return 0;
    }
    for (auto& c : checkpoint8_ts_) {
        if (maze[c[0]][c[1]]) {
            return 0;
        }
    }

    // i,j are coordinates on maze
    // lk[i][j][0]: dist from entrance
    // lk[i][j][1]: number of checkpoint8_ts on shortest path from entrance to [i][j]
    array<array<array<int8_t, 2>, size_>, size_> lk;
    for (auto& i : lk) {
        for (auto& j : i) {
            j[0] = 1'000'000;
            j[1] = 0;
        }
    }
    lk[entrance_[0]][entrance_[1]][0] = 0; 

    vector<array<int8_t, 2>> q = {{entrance_}};
    int8_t count = 0;
    array<int8_t, 2> pt;
    array<int8_t, 2> foo;
    int8_t dist, checkpointCount;
    vector<array<int8_t, 2>> newPts;
    bool isCheckpoint;

    while (q.size() > 0) {
        
        pt = q.back();
        q.pop_back();
        dist = lk[pt[0]][pt[1]][0];
        checkpointCount= lk[pt[0]][pt[1]][1];
        dist += 1;
        newPts.clear();

        foo = {pt[0]+1, pt[1]};
        if (foo[0] < size_ && lk[foo[0]][foo[1]][0] > dist && !maze[foo[0]][foo[1]]) {
            newPts.push_back(foo);
        }

        foo = {pt[0], pt[1]+1};
        if (foo[1] < size_ && lk[foo[0]][foo[1]][0] > dist && !maze[foo[0]][foo[1]]) {
            newPts.push_back(foo);
        }
        foo = {pt[0]-1, pt[1]};
        if (foo[0] >= 0 && lk[foo[0]][foo[1]][0] > dist && !maze[foo[0]][foo[1]]) {
            newPts.push_back(foo);
        }

        foo = {pt[0], pt[1]-1};
        if (foo[1] >= 0 && lk[foo[0]][foo[1]][0] > dist && !maze[foo[0]][foo[1]]) {
            newPts.push_back(foo);
        }

        for (auto& newPt : newPts) {
            // if |newPt| is lesser distance or same distance and more checkpoint8_ts 
            if (
                dist < lk[newPt[0]][newPt[1]][0] ||
                (
                    (dist == lk[newPt[0]][newPt[1]][0])
                    &&
                    (lk[newPt[0]][newPt[1]][1] < checkpointCount)
                )
            ) {
                // if newPt is a checkpoint8_t
                if (find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), newPt) != checkpoint8_ts_.end()) {
                    lk[newPt[0]][newPt[1]] = {dist, checkpointCount + 1};
                }
                else {
                    lk[newPt[0]][newPt[1]] = {dist, checkpointCount};
                }
                // you don't want to add newPt to queue if it's already an exit point8_t
                if (newPt != exit_) {
                    q.push_back(newPt);
                }
            }
        }
    }
    if (lk[exit_[0]][exit_[1]][1] == checkpoint8_ts_.size())
        return lk[exit_[0]][exit_[1]][0];
    return 0;
}

// maze[i][j] is true => wall, false => empty
int8_t fitness_debug(array<array<bool, size_>, size_>& maze) {
    
    if (maze[entrance_[0]][entrance_[1]]) {
        cout << "\n entrance is blocked";
        return 0;
    }
    if (maze[exit_[0]][exit_[1]]) {
        cout << "\n exit is blocked"; 
        return 0;
    }
    for (auto& c : checkpoint8_ts_) {
        if (maze[c[0]][c[1]]) {
            cout << "\n checkpoint8_t is blocked";
            return 0;
        }
    }

    // i,j are coordinates on maze
    // lk[i][j][0]: dist from entrance
    // lk[i][j][1]: number of checkpoint8_ts on shortest path from entrance to [i][j]
    array<array<array<int8_t, 2>, size_>, size_> lk;
    for (auto& i : lk) {
        for (auto& j : i) {
            j[0] = 120;
            j[1] = 0;
        }
    }
    lk[entrance_[0]][entrance_[1]][0] = 0; 

    vector<array<int8_t, 2>> q = {{entrance_}};
    int8_t count = 0;
    array<int8_t, 2> pt;
    array<int8_t, 2> foo;
    int8_t dist, checkpointCount;
    vector<array<int8_t, 2>> newPts;
    bool isCheckpoint, isUsable, canUpdate;
    const array<array<int8_t, 2>, 4> diffs = {
        array<int8_t, 2> {+1,  0},
        array<int8_t, 2> { 0, +1},
        array<int8_t, 2> {-1,  0},
        array<int8_t, 2> { 0, -1},
    };

    while (q.size() > 0) {
        
        pt = q.back();
        q.pop_back();
        dist = lk[pt[0]][pt[1]][0];
        checkpointCount= lk[pt[0]][pt[1]][1];
        dist += 1;
        newPts.clear();

        for (auto& d : diffs) {
            foo = {pt[0]+d[0], pt[1]+d[1]};
            isUsable = foo[0] >= 0 && foo[1] >= 0 && foo[0] < size_ && foo[1] < size_ ;
            if (!isUsable)
                continue;
            if (maze[foo[0]][foo[1]])
                continue;
            isCheckpoint = find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), foo) != checkpoint8_ts_.end();
            if (isCheckpoint)
                checkpointCount += 1;
            canUpdate = 
                (dist < lk[foo[0]][foo[1]][0])
                ||
                (
                    (dist == lk[foo[0]][foo[1]][0])
                    &&
                    (lk[foo[0]][foo[1]][1] < checkpointCount)
                );
            if (canUpdate) {
                lk[foo[0]][foo[1]] = {dist, checkpointCount};
            }
            if (foo != exit_) {
                q.push_back(foo);
            }
            if (isCheckpoint)
                checkpointCount -= 1;    
        }

        // foo = {pt[0]+1, pt[1]};
        // isCheckpoint = find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), foo) != checkpoint8_ts_.end();
        // if (foo[0] < size_ && !maze[foo[0]][foo[1]]) {
        //     newPts.push_back(foo);
        // }

        // foo = {pt[0], pt[1]+1};
        // isCheckpoint = find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), foo) != checkpoint8_ts_.end();
        // if (foo[1] < size_ && !maze[foo[0]][foo[1]]) {
        //     newPts.push_back(foo);
        // }

        // foo = {pt[0]-1, pt[1]};
        // isCheckpoint = find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), foo) != checkpoint8_ts_.end();
        // if (foo[0] >= 0 && !maze[foo[0]][foo[1]]) {
        //     newPts.push_back(foo);
        // }

        // foo = {pt[0], pt[1]-1};
        // isCheckpoint = find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), foo) != checkpoint8_ts_.end();
        // if (foo[1] >= 0 && !maze[foo[0]][foo[1]]) {
        //     newPts.push_back(foo);
        // }

        // for (auto& newPt : newPts) {
        //     isCheckpoint = (find(checkpoint8_ts_.begin(), checkpoint8_ts_.end(), newPt) != checkpoint8_ts_.end());
        //     // if |newPt| is lesser distance or same distance and more checkpoint8_ts 
        //     if ( 
        //         dist < lk[newPt[0]][newPt[1]][0] 
        //         ||
        //         (
        //             (dist <= lk[newPt[0]][newPt[1]][0])
        //             &&
        //             (
        //                 (lk[newPt[0]][newPt[1]][1] <= checkpointCount)
        //                 ||
        //                 (
        //                     isCheckpoint
        //                     &&
        //                     (lk[newPt[0]][newPt[1]][1] <= checkpointCount+1)
        //                 )
        //             )
        //         )
        //     ) {
        //         // if newPt is a checkpoint8_t
        //         if (isCheckpoint) {
        //             lk[newPt[0]][newPt[1]] = {dist, checkpointCount + 1};
        //         }
        //         else {
        //             lk[newPt[0]][newPt[1]] = {dist, checkpointCount};
        //         }
        //         // you don't want to add newPt to queue if it's already an exit point8_t
        //         if (newPt != exit_) {
        //             q.push_back(newPt);
        //         }
        //         // else {
        //         //     cout << "\n found a path!";
        //         // }
        //     }
        // }
    }
    cout << "\n exit path stats: dist: " << lk[exit_[0]][exit_[1]][0];
    cout << " | checkpoint8_ts: " << lk[exit_[0]][exit_[1]][1]; 
    if (lk[exit_[0]][exit_[1]][1] == checkpoint8_ts_.size())
        return lk[exit_[0]][exit_[1]][0];
    return 0;
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
    uniform_int8_t_distribution<mt19937::result_type> getNum(1,1000);
    cout << "print8_ting 100 numbers in the range[1, 100]: ";
    for (int8_t i = 0 ; i < 100 ; ++i) {
        cout << float(getNum(rng))/1000.0f << " ," ;
    }
}

void testFitness() {
    auto m = genMaze();
    cout << '\n' << m;
    cout << "\n fitness: " << fitness_debug(m);
}

int8_t main() {
    testFitness();
    return 0;
}