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
using namespace std::chrono_literals;


bool stopflag = false;

condition_variable cv;
mutex queue_mutex;



void work() {
    while (true) {
        unique_lock<mutex> latch(queue_mutex);
        cv.wait(latch);
        if (stopflag) return;
        cout << "Hello \n";
        latch.unlock();
    }
}

void finish() {
    cout << "finish called \n";
    stopflag = true;
    cv.notify_all();
}

int main() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    thread t(work);
    cv.notify_one();
    this_thread::sleep_for(1s);
    cout << "Main thread woke up \n";
    finish();
    t.join();
}