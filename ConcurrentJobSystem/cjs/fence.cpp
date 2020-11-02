#include "fence.hpp"
#include "job_system.hpp"

namespace {
	std::mutex m;
	void printm(float t) {
		std::lock_guard<std::mutex> lg(m);
		printf("time:%f\n", t);
	}
}

namespace cjs {

	fence::fence() : is_submitted(false), synced_threads(0), wait_threads(false), invalid(false) { }

	fence::~fence() {
		// if fence is destructed, we need to make sure that the workers get through the fence
		wait_and_unblock();
	}

	void fence::submit(bool blockthreads) {
		if (!job_system::is_caller_main_thread()) return;
		// set apropriate values
		is_submitted = true;
		synced_threads = 0;
		wait_threads = blockthreads;
		// submit the work 
		job_system::submit_work(this);
	}

	void fence::wait() const {
		if (!job_system::is_caller_main_thread() || !is_submitted) return;
		// wait for the threads
		while (synced_threads != job_system::get_worker_count()) { }
	}

	void fence::unblock_threads() {
		if (!job_system::is_caller_main_thread() || !is_submitted) return;
		// unblock
		wait_threads = false;
	}

	void fence::wait_and_unblock() {
		// unblock them first
		unblock_threads();
		// then wait for the threads
		wait();
	}

	void fence::reset() {
		if (invalid && !is_submitted) {
			is_submitted = false;
			synced_threads = 0;
			wait_threads = false;
			invalid = false;
		}
	}

	void fence::__sync() {
		if (job_system::is_caller_main_thread()) return;
		// this thread is synced so we let them know
		++synced_threads;
		// wait for all threads to sync
		while (synced_threads != job_system::get_worker_count()) { }
		// waits for main thread to unblock
		while (wait_threads == true) { }
		// the fence has been used so it becomes invalid
		invalid = true;
	}

}