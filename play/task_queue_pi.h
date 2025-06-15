#pragma once
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>
#include <atomic>
#include <array>
#include <condition_variable>

// can't get this to compile. see borken task_queue_stress_test.cpp

/**
 * 
 * Simple queue for 1 producer, multiple consumer
 * 
 * methods:
 * 
 * TaskQueueClient(T termination_symbol)
 * 		- constructor
 * 		- termination_symbol is of type T, this will be returned by 
 * 			get_task() if terminate_threads was called in between 
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
 * T get_task()
 * 		- blocking method for consumers
 * 		- if return value is that of termination_symbol, then terminate_threads was called
 * 		- else, the return value is the task value
*/
class TaskQueueClient_pi {
public:
	// actual store of tasks, we ensure atomic access to the store among producer/consumers
	static std::vector<std::pair<int, int>> tasks;
	// for synchronization, we construct unique_lock w/ this mutex
	static std::mutex mutex;
	// for synchronization, used for avoiding busy waiting
	static std::condition_variable cv;
	
	static bool termination_flag;
	// to know how to signal callers of get_task that task queue workers were terminated
	static std::pair<int, int> termination_symbol;

	static std::mutex output_mutex;

	TaskQueueClient_pi(std::pair<int, int> termination_symbol) {
		termination_flag = false;
		this->termination_symbol = termination_symbol;
	}

	TaskQueueClient_pi() {
		termination_flag = false;
	}
	
	void add_task(std::pair<int, int> x) {
		{
			std::unique_lock<std::mutex> lock(mutex);
			tasks.push_back(x);
		}
		cv.notify_one();
	}

	void add_task(int x, int y) {
		{
			std::unique_lock<std::mutex> lock(mutex);
			tasks.emplace_back(std::pair<int, int> {x, y});
		}
		cv.notify_one();
	}

	void terminate_threads() {
		termination_flag = true;
		cv.notify_all();
	}
	
	std::pair<int, int> get_task() {
		if (termination_flag) return termination_symbol;
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
		if (termination_flag) return termination_symbol;
		std::pair<int, int> task = tasks.back();
		tasks.pop_back();
		return task;
	}
};