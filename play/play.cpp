#include <iostream>
#include <vector>

using namespace std;

ostream& operator<<(ostream& os, vector<int>& x)
{
    for (int&i : x) {
        os << i << ", ";
    }
    os << '\n';
    // os << dt.mo << '/' << dt.da << '/' << dt.yr;
    return os;
}

int main() {
    vector<vector<int>> foo;
    foo.reserve(5);
    cout << "\n main size: " << foo.size();
    for (int i = 0 ; i < 5 ; ++i) {
        foo.push_back(vector<int> (4, 0));
    }
    cout << "\n main size: " << foo.size();
    for (auto& i : foo)
        cout << "\n size: " << i.size();
    return 0;
}
