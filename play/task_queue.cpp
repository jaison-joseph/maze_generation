#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <atomic>

/**
 * 
 * Simple queue for 1 producer, multiple consumer
 * 
 * methods:
 * 
 * TaskQueueClient()
 * 		- constructor
 * 
 * void add_task(uint8_t x)
 * 		- non blocking
 * 		- add a task to task queue
 * 		- x: input to queue
 * 
 * void terminate_threads()
 * 		- stops task processing
 * 		- calling any method on this object after calling
 * 		terminate_threads is UB
 * 
 * int32_t get_task()
 * 		- blocking method for consumers
 * 		- if return value is -1, then terminate_threads was called
 * 		- else, the return value is the task value
*/
#pragma once
class TaskQueueClient {
	public:
		// actual store of tasks, we ensure atomic access to the store among producer/consumers
		static std::vector<uint8_t> tasks;
		// for synchronization, we construct unique_lock w/ this mutex
		static std::mutex mutex;
		// for synchronization, used for avoiding busy waiting
		static std::condition_variable cv;
		// we use 'inline' since we can give it initial value in declaration itself
		inline static bool termination_flag = false;

		TaskQueueClient() {
			termination_flag = false;
		}
		
		void add_task(uint8_t x) {
			{
				std::unique_lock<std::mutex> lock(mutex);
				tasks.push_back(x);
			}
			cv.notify_one();
		}

		void terminate_threads() {
			termination_flag = true;
			cv.notify_all();
		}
		
		int32_t get_task() { 
			// as soon as lock goes out of scope, lock is released -> mutex is released
			std::unique_lock<std::mutex> lock(mutex);
			if (tasks.size() == 0) {
				// this wait releases the hold on lock -> mutex is released
				cv.wait(lock, []{ 
					// this condition is the second check to ensure
					// that we don't get hold of lock unless 
					// 1. tasks queue is non-empty
					// 		OR
					// 2. termination flag is set
					return tasks.size() > 0 || termination_flag; 
				});
			}
			if (termination_flag) return -1;
			int32_t task = tasks.back();
			tasks.pop_back();
			return task;
		}
};