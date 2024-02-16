
#include <iostream>
#include <algorithm>
#include <array>
#include <random>
#include <future>


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
random_device dev;
mt19937 rng(dev());
const uint_fast32_t rng_upper_bound_ = 1'000'000;
uniform_int_distribution<mt19937::result_type> getNum(1,rng_upper_bound_); // distribution in range [1, 10**6]
const uint_fast32_t new_pm_ = rng_upper_bound_ * Pm_;
const uint_fast32_t new_pc_ = rng_upper_bound_ * Pc_;

static inline void uniformCrossover(array<array<bool, size_>, size_>& m1, array<array<bool, size_>, size_>& m2) {
     // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    
    bool foo;
    for (int i = 0 ; i < size_ ; ++i) {
        for (int j = 0 ; j < size_ ; ++j) {
            if (getNum(rng) <= new_pc_) {
                foo = m1[i][j];
                m1[i][j] = m2[i][j];
                m2[i][j] = foo;   
            }
        }
    }
}

typedef array<array<bool, size_>, size_> arrtype;
void work(arrtype& m1, arrtype& m2, int start_i, int end_i) {
    bool foo;
    for (int i = start_i ; i < end_i ; ++i) {
        for (int j = 0 ; j < size_ ; ++j) {
            if (getNum(rng) <= new_pc_) {
                foo = m1[i][j];
                m1[i][j] = m2[i][j];
                m2[i][j] = foo;   
            }
        }
    }
}

void uniformCrossover_parallel(array<array<bool, size_>, size_>& m1, array<array<bool, size_>, size_>& m2) {
     // https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
    
    future<void> result1 = async(work, ref(m1), ref(m2), 0, 15);
    future<void> result2 = async(work, ref(m1), ref(m2), 15, size_);
    result1.get();
    result2.get();   
}

int main() {
    array<array<bool, size_>, size_> m1 = {};
    array<array<bool, size_>, size_> m2 = {};
    for (auto& i : m2) {
        for (auto& j : i) {
            j = true;
        }
    }
    for (int i = 0 ; i < 200'000 ; i++) {
        uniformCrossover_parallel(ref(m1), ref(m2));
    }

    for (auto& i : m1) {
        for (auto j : i) {
            cout << (j ? "t " : "f ");
        }
        cout << endl;
    }
    cout << endl;
    for (auto& i : m2) {
        for (auto j : i) {
            cout << (j ? "t " : "f ");
        }
        cout << endl;
    }
}   