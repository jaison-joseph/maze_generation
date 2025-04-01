#pragma once
#include <thread>

#ifdef __APPLE__
#pragma once
#include <mach/thread_act.h>
#include <mach/thread_policy.h>
// pins CPU to core specified by argument
// https://github.com/vusec/revanc/blob/master/source/darwin/thread.c
int pin_cpu(std::thread& t, const int i)
{
	// pthread_t thread = pthread_self();
	pthread_t thread = t.native_handle();
	thread_port_t mach_thread = pthread_mach_thread_np(thread);

	thread_affinity_policy_data_t policy = { i };
	thread_policy_set(mach_thread, THREAD_AFFINITY_POLICY, (thread_policy_t)&policy, 1);

	return 0;
}
#elif defined(__linux__)
void pin_cpu(std::thread& t, int cpu_id) {
    // Create a cpu_set_t object representing a set of CPUs
    cpu_set_t cpuset;
    
    // Clear the set
    CPU_ZERO(&cpuset);
    
    // Add the specified CPU to the set
    CPU_SET(cpu_id, &cpuset);
    
    // Set the affinity using the thread's native handle
    int rc = pthread_setaffinity_np(t.native_handle(),
                                   sizeof(cpu_set_t), &cpuset);
    
    // Check for errors
    if (rc != 0) {
        std::cerr << "Error calling pthread_setaffinity_np: " << rc << "\n";
    }
}
#endif

void set_thread_max_priority(std::thread& t) {
    // Get the native handle of the std::thread
    pthread_t handle = t.native_handle();
    
    // Create a scheduling parameter struct
    struct sched_param param;
    
    // Set the priority to the maximum for the policy
    param.sched_priority = sched_get_priority_max(SCHED_RR);
    
    // Apply the scheduling policy and priority
    int result = pthread_setschedparam(handle, SCHED_RR, &param);
    
    if (result != 0) {
        std::cerr << "Failed to set thread priority: " << result << std::endl;
    }
}