#include <iostream>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <array>

using namespace std;

const int size_ = 30;

// god bless chatgpt

struct fitnessArgs {
    array<array<bool, size_>, size_>& maze;
    array<int, 7>& fitnesses;
    array<int, 7>& sortedFitnesses;
    int index;
};

struct check {
    int foo;
    int bar;
};

ostream& operator<<(ostream& os, check& m) {
    os << '(' << m.foo << ", " << m.bar << ")\n";
    return os;
}

queue<fitnessArgs> q;

// This is the queue of jobs that the worker threads will process
queue<int> jobs;

// This mutex and condition variable are used to control access to the jobs queue
mutex queue_mutex;
condition_variable queue_cv;

// This flag is used to signal to the worker threads when all jobs have been added to the queue
bool done = false;

// This is the function that the worker threads will execute
void worker_function()
{
  while (true)
  {
    // Wait until there is a job in the queue or the "done" flag is set
    unique_lock<mutex> lock(queue_mutex);
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
    cout << "Worker thread processing job: " << job << endl;
  }
}

void runJobs() {
  // Create the worker threads
  vector<thread> workers;
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
  // {
    lock_guard<mutex> lock(queue_mutex);
    done = true;
  // }
  queue_cv.notify_all();

  // Wait for the worker threads to finish
  for (auto& worker : workers)
  {
    worker.join();
  }
}

int main() {
    // check a;
    // vector<check> v;
    // for (int i = 0 ; i < 5 ; ++i) {
    //     a.foo = i;
    //     a.bar = i+1;
    //     v.push_back(a);
    // }
    // for (auto& t : v)
    //     cout << t;
    runJobs();
}
