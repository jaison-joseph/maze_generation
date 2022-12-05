#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>

// god bless chatgpt

// This is the queue of jobs that the worker threads will process
std::queue<int> jobs;

// This mutex and condition variable are used to control access to the jobs queue
std::mutex queue_mutex;
std::condition_variable queue_cv;

// This flag is used to signal to the worker threads when all jobs have been added to the queue
bool done = false;

// This is the function that the worker threads will execute
void worker_function()
{
  while (true)
  {
    // Wait until there is a job in the queue or the "done" flag is set
    std::unique_lock<std::mutex> lock(queue_mutex);
    queue_cv.wait(lock, []{return !jobs.empty() || done;});

    // If the "done" flag is set, then break out of the loop and exit the thread
    if (done)
    {
      break;
    }

    // Dequeue the next job from the queue
    int job = jobs.front();
    jobs.pop();

    // Release the lock on the queue while we process the job
    lock.unlock();

    // Process the job
    std::cout << "Worker thread processing job: " << job << std::endl;
  }
}

int main()
{
  // Create the worker threads
  std::vector<std::thread> workers;
  for (int i = 0; i < 7; i++)
  {
    workers.emplace_back(worker_function);
  }

  // Add some jobs to the queue
  for (int i = 0; i < 10; i++)
  {
    jobs.push(i);
  }

  // Signal to the worker threads that all jobs have been added to the queue
  {
    std::lock_guard<std::mutex> lock(queue_mutex);
    done = true;
  }
  queue_cv.notify_all();

  // Wait for the worker threads to finish
  for (auto& worker : workers)
  {
    worker.join();
  }

  return 0;
}
