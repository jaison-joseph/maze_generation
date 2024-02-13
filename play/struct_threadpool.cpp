#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <array>

const int NUM_THREADS_ = 7;

struct ThreadPool {
    bool termination_flag_;
    std::atomic<int> readyCount;
    std::atomic<int> iterCount;
    std::array<std::thread, NUM_THREADS_> workers;

    void thread_pool_launch_iteration() {
        readyCount = NUM_THREADS_;
        iterCount += 1;
    }

    void thread_pool_wait_iteration_completion() {
        while (readyCount != 0) {}
    }

    void thread_pool_terminate() {
        termination_flag_ = true;

        readyCount = NUM_THREADS_;
        iterCount += 1;

        for (int i = 0 ; i < NUM_THREADS_ ; i++) {
            workers[i].join();
        }
    }
};

int main() 
{
    std::array<int, NUM_THREADS_> inputs;


    ThreadPool t;

    void work(int x) {
        int localitercount = 0;
        while(true) {

            // wait until signaled
            // readyCount == 0 ensures that the work won't happen until there's an input from master thread

            while (localitercount != t.iterCount) {}

            // check if termination flag is set. If yes, return True
            if (termination_flag_) break;

            localitercount += 1;

            // work
            // std::cout << x << std::endl;
            inputs[x] += 1;

            // std::this_thread::sleep_for(100ms);
            // std::this_thread::sleep_for (std::chrono::seconds(x));

            // end of loop
            readyCount -= 1;
        }
    }

    void thread_pool_init() {
        for (int i = 0 ; i < NUM_THREADS_ ; i++) {
            t.workers[i] = std::thread(work, i);
        }
        t.termination_flag_ = false;
        t.iterCount = -1;
        t.readyCount = 0;
        // cout << " initialization done";
    }

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        inputs[i] = i;
    }

    thread_pool_init();

    for (int iter = 0 ; iter < 3 ; iter++) {


        // set the inputs
        for (int i = 0 ; i < NUM_THREADS_ ; i++) {
            inputs[i] = i + iter;
        }

        // make the threads go brrrr
        t.thread_pool_launch_iteration();

        // std::cout << "end of iteration " << iter << std::endl;

        // wait until everyone's done
        t.thread_pool_wait_iteration_completion();

        for (int i = 0 ; i < NUM_THREADS_ ; i++) {
            std::cout << inputs[i] << ", ";
        }
        std::cout << std::endl;

    }

    //set termination flag
    t.thread_pool_terminate();

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        std::cout << inputs[i] << ", ";
    }

    std::cout << "foo and bar completed.\n";

    return 0;
}

// 102345634126503406102