#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <atomic>

#pragma once
class TaskQueueClient {
	public:
		static std::vector<uint8_t> tasks;
		static std::mutex mutex;
		static std::condition_variable cv;
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
		
		// if we return -1, then the thread was terminated
		// else, a whole number will be returned (uint8_t)
		// this method is blocking
		int32_t get_task() { 
			// as soon as lock goes out of scope, lock is released -> mutex is released
			std::unique_lock<std::mutex> lock(mutex);
			if (tasks.size() == 0) {
				// this wait releases the hold on lock -> mutex is released
				cv.wait(lock, []{ return tasks.size() > 0 || termination_flag; });
			}
			if (termination_flag) return -1;
			int32_t task = tasks.back();
			tasks.pop_back();
			return task;
		}
};