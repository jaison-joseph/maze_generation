#include <iostream>       // std::cout
#include <thread>         // std::thread
#include <array>

using namespace std::chrono_literals;

const int NUM_THREADS_ = 7;
bool termination_flag_ = false;

std::array<int, NUM_THREADS_> inputs;

std::atomic<int> readyCount (0);
std::atomic<int> iterCount (-1);

// std::array<int, NUM_THREADS_> outputs;

void foo() 
{
  // do stuff...
}

void bar(int x)
{
    inputs[x] = x + 1;
}

void work(int x) {
    int localitercount = 0;
    while(true) {

        // wait until signaled
        // readyCount == 0 ensures that the work won't happen until there's an input from master thread

        while (localitercount != iterCount) {}

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

void thread_pool_init(std::array<std::thread, NUM_THREADS_>& workers) {
    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i] = std::thread(work, i);
    }
}

static inline void thread_pool_launch_iteration() {
    readyCount = NUM_THREADS_;
    iterCount += 1;
}

static inline void thread_pool_wait_iteration_completion() {
    while (readyCount != 0) {}
}

void thread_pool_terminate(std::array<std::thread, NUM_THREADS_>& workers) {
    termination_flag_ = true;

    readyCount = NUM_THREADS_;
    iterCount += 1;

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        workers[i].join();
    }
}



/**
 * 
 * We want an array of threads,
 * 
 * they wait on a condition variable
 * 
 * once they are signaled to wake up (condition variable notify one of), they read argument from a shared piece of memory, and execute
 * 
 * they write the result onto a shared piece of memory
 * 
 * 
 * first, we see if we can safely read and write from the shared piece of memory
 * 
*/

int main() 
{
    std::array<std::thread, NUM_THREADS_> workers;

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        inputs[i] = i;
    }

    thread_pool_init(workers);

    for (int iter = 0 ; iter < 3 ; iter++) {


        // set the inputs
        for (int i = 0 ; i < NUM_THREADS_ ; i++) {
            inputs[i] = i + iter;
        }

        // make the threads go brrrr
        thread_pool_launch_iteration();

        // std::cout << "end of iteration " << iter << std::endl;

        // wait until everyone's done
        thread_pool_wait_iteration_completion();

        for (int i = 0 ; i < NUM_THREADS_ ; i++) {
            std::cout << inputs[i] << ", ";
        }
        std::cout << std::endl;

    }

    //set termination flag
    thread_pool_terminate(workers);

    for (int i = 0 ; i < NUM_THREADS_ ; i++) {
        std::cout << inputs[i] << ", ";
    }

    std::cout << "foo and bar completed.\n";

    return 0;
}

// 102345634126503406102