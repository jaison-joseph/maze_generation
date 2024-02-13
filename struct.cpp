#include <iostream>       // std::cout
#include <array>

using namespace std;

int main() {
    struct foo {
        int func(int x) {return x * 2;}
    }; 

    foo obj;
    cout << endl << obj.func(10) << endl;
}