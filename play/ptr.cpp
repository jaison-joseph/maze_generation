#include <array>
#include <iostream>

using namespace std;

typedef array<int, 7> arr_t;

void printer(std::array<int, 7>* foo) {
    for (int i = 0 ; i < 7 ; i++) {
        for (int j = 0 ; j < 7 ; j++) {
            cout << foo[i][j] << ", ";
        }   
        cout << endl;
    }
}

int main() {
    array<array<int, 7>, 7> local;
    for (int i = 0 ; i < 7 ; i++) {
        for (int j = 0 ; j < 7 ; j++) {
        local[i][j] = i;
        }
    }
    printer(local.data());
}