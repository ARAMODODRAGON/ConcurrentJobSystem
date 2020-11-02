#ifndef CJS_JOB_SYSTEM_HPP
#define CJS_JOB_SYSTEM_HPP
#include "common.hpp"
#include "detail\work.hpp"

namespace cjs {

	class job_system {
	public:

		// initializes the job system
		// -default 0 will determine the number of threads automatically
		// -a positive thread count will create 'threads' number of threads
		// -a negative thread count will determine the amount automatically 
		// and reduce the number by 'threads' amount
		static void init(int32 threads = 0);

		// waits for work to finish and closes all threads
		static void exit();

		// submits work to the queue
		// types of work include: ijob & ifence
		static void submit_work(detail::work work);


		// helper functions

		// returns true on if the calling thread is the main thread
		static bool is_caller_main_thread();

		// returns the number of worker threads
		static uint32 get_worker_count();

		// non constructable/copyable/moveable (static only)
		job_system() = delete;
		job_system(const job_system&) = delete;
		job_system& operator=(const job_system&) = delete;
		job_system(job_system&&) = delete;
		job_system& operator=(job_system&&) = delete;
	};

}

#endif // !CJS_JOB_SYSTEM_HPP