#include <iostream>
#include <vector>
#include <unordered_map>
#include <array>
#include <algorithm>
#include <iterator>
#include <random>
#include <numeric>

using namespace std;
const int size_ = 3;

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

void oldSchool() {
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    for (auto&i : m1)
        i.fill(false);
    for (auto&i : m2)
        i.fill(true);
    int i = 0;
    for (auto&r : m2) 
        copy(r.begin(), r.end(), m1[i++].begin());
    m2[0][0] = false;
    cout << m1 << '\n' << m2;
}

void hmm() {
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    for (auto&i : m1)
        i.fill(false);
    for (auto&i : m2)
        i.fill(true);
    m1 = m2;
    m2[0][0] = false;
    cout << m1 << '\n' << m2;
}

void hmm_2() {
    array<array<array<bool, size_>, size_>, 2> population;
    array<array<bool, size_>, size_> m1;
    array<array<bool, size_>, size_> m2;
    for (auto&i : m1)
        i.fill(false);
    for (auto&i : m2)
        i.fill(true);
    bool flag = false;
    for (auto&m : population)
        for (auto&r : m)
            for (auto&c : r) {
                c = flag;
                flag = !flag;
            }

    m1 = population[0];
    // m1[0][0] = true;
    population[0][0][0] = true;
    cout << m1 << '\n' <<population[0] ;
}

int main() {
    hmm_2();
    return 0;
}