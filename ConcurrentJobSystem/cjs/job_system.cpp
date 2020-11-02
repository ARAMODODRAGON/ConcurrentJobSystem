#include "job_system.hpp"
#include <thread>
#include <vector>
#include <queue>
#include "interfaces.hpp"

namespace cjs {

	using mutex_guard = std::lock_guard<std::mutex>;

	namespace {

		// the id of the main thread
		const std::thread::id main_thread_id = std::this_thread::get_id();

		// job system state
		std::atomic_bool isRunning = false;
		std::atomic_bool shouldQuit = false;

		// contains relevent data to a specific worker
		struct worker {
			std::thread thr;
		};

		// all the workers
		std::vector<worker> allWorkers;

		// function that runs on seperate threads
		void worker_func(uint32 id);

		// the queue
		std::queue<detail::work> workQueue;
		std::mutex queueLock; // lock makes it thread safe

	}

	bool job_system::is_caller_main_thread() {
		return std::this_thread::get_id() == main_thread_id;
	}

	void job_system::init(int32 threads) {
		if (!is_caller_main_thread() || isRunning) return;

		// if threads < 1 then it means that this is a subtractive amount
		if (threads < 1) threads = std::thread::hardware_concurrency() - threads;

		isRunning = true;
		shouldQuit = false;

		// create the threads
		allWorkers.resize(threads);
		for (uint32 i = 0; i < allWorkers.size(); ++i) {
			allWorkers[i].thr = std::thread(worker_func, i);
		}

	}

	void job_system::exit() {
		if (!is_caller_main_thread() || !isRunning) return;

		shouldQuit = true;

		// join all threads
		for (uint32 i = 0; i < allWorkers.size(); ++i) {
			allWorkers[i].thr.join();
		}
		allWorkers.resize(0);

		isRunning = false;
	}

	void job_system::submit_work(detail::work work) {
		if (work.is_valid()) {
			mutex_guard lock(queueLock);
			workQueue.push(work);
			return;
		}
		// failed to submit any work
		#ifdef _DEBUG
		throw std::runtime_error("submitted work was not valid");
		#endif
	}

	uint32 job_system::get_worker_count() {
		return allWorkers.size();
	}

	namespace {
		std::mutex m;
		void printm(int t) {
			std::lock_guard<std::mutex> lg(m);
			printf("accessedcount:%i\n", t);
		}
	}
	namespace {

		void worker_func(uint32 id) {
			while (isRunning && !shouldQuit) {

				// grab the next peice of work
				detail::work work;
				{
					mutex_guard lock(queueLock);
					if (workQueue.size() > 0) {
						work = workQueue.front();
						// only pops if all threads have accessed the work
						if (workQueue.front().type == detail::work::TYPE_FENCE) {
							workQueue.front().accessedCount += 1;
							if (workQueue.front().accessedCount == job_system::get_worker_count())
								workQueue.pop();
						}
						// pops the given work
						else workQueue.pop();
					}
				}

				// run the job or fence
				switch (work.type) {
					case detail::work::TYPE_IJOB:
						work.job->execute();
						break;
					case detail::work::TYPE_FENCE:
						work.fence->__sync();
						break;
					default: break;
				}
			}
		}

	} // !namespace
} // !namespace cjs