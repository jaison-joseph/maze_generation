#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <atomic>
#include "task_queue.cpp"

// Static member definitions
std::vector<uint8_t> TaskQueueClient::tasks;
std::mutex TaskQueueClient::mutex;
std::condition_variable TaskQueueClient::cv;

void stress_test() {
    const int NUM_TASKS = 1000000;
    const int NUM_THREADS = 10;
    
    std::atomic<int> tasks_consumed{0};
    std::vector<std::thread> workers;
    
    TaskQueueClient q;
    
    // Add 1 million tasks
    std::cout << "Adding " << NUM_TASKS << " tasks..." << std::endl;
    auto add_start = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_TASKS; i++) {
        q.add_task(static_cast<uint8_t>(i % 256));
    }
    
    auto add_end = std::chrono::high_resolution_clock::now();
    auto add_duration = std::chrono::duration_cast<std::chrono::milliseconds>(add_end - add_start);
    std::cout << "Added " << NUM_TASKS << " tasks in " << add_duration.count() << " ms" << std::endl;
    
    // Start drain timing
    std::cout << "Starting drain test with " << NUM_THREADS << " threads..." << std::endl;
    auto drain_start = std::chrono::high_resolution_clock::now();
    
    // Launch worker threads
    for (int i = 0; i < NUM_THREADS; i++) {
        // workers.emplace_back([&tasks_consumed, &q, i]() {
        workers.emplace_back([&q]() {
            // int local_count = 0;
            while (true) {
                int32_t task = q.get_task();
                if (task == -1) break; // terminated
                
                // local_count++;
				// tasks_consumed.fetch_add(1);
                // int total = tasks_consumed.fetch_add(1) + 1;
                
                // Print progress every 100k tasks
                // if (total % 100000 == 0) {
                //     std::cout << "Progress: " << total << "/" << NUM_TASKS 
                //               << " (" << (100.0 * total / NUM_TASKS) << "%)" << std::endl;
                // }
            }
            // std::cout << "Thread " << i << " consumed " << local_count << " tasks" << std::endl;
        });
    }
    
    // Wait until all tasks are consumed
    // while (tasks_consumed.load() < NUM_TASKS) {
    while (q.tasks.size() > 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    // Stop drain timing
    auto drain_end = std::chrono::high_resolution_clock::now();
    
    // Terminate threads
    q.terminate_threads();
    
    // Join all threads
    for (auto& worker : workers) {
        worker.join();
    }
    
    // Calculate and print results
    auto drain_duration = std::chrono::duration_cast<std::chrono::milliseconds>(drain_end - drain_start);
    auto drain_duration_micro = std::chrono::duration_cast<std::chrono::microseconds>(drain_end - drain_start);
    
    std::cout << "\n=== RESULTS ===" << std::endl;
    std::cout << "Tasks consumed: " << tasks_consumed.load() << std::endl;
    std::cout << "Drain time: " << drain_duration.count() << " ms (" 
              << drain_duration_micro.count() << " μs)" << std::endl;
    std::cout << "Throughput: " << (NUM_TASKS * 1000.0) / drain_duration.count() 
              << " tasks/second" << std::endl;
    std::cout << "Average time per task: " 
              << (double)drain_duration_micro.count() / NUM_TASKS << " μs" << std::endl;
}

int main() {
    stress_test();
    return 0;
}