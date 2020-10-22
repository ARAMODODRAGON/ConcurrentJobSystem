#ifndef CJS_FENCE_HPP
#define CJS_FENCE_HPP
#include "common.hpp"
#include "interfaces.hpp"

namespace cjs {

	enum class fence_opt : uint8 {
		// syncronizes the threads and lets them continue
		dont_wait,
		// syncronizes the threads but forces them to wait for 
		// the main thread to start them up again
		wait_for_main
	};

	// used to block and sync all worker threads
	// becomes invalid after use but can be reset using fence::reset();
	struct fence : ifence {

		fence();
		~fence() final;

		// checks if the fence is valid
		bool is_valid() const;
		operator bool() const;

		// resets this fence
		void reset();

		// submits this fence to the job queue
		// requires option to decide if
		// fence_opt::dont_wait - threads will sync, then start working immediately after
		// fence_opt::wait_for_main - threads will sync, then wait for the main thread to call 'sync_and_start()'
		void submit(const fence_opt option_ = cjs::fence_opt::dont_wait);

		// waits for the threads to sync up
		// should be used so the main thread can wait for the worker threads to reach this fence
		void wait_for_sync();

		// starts the threads back up 
		// waits for the threads to sync before letting them go
		// only works if the submitted option is 'fence_opt::wait_for_main'
		void sync_and_start();

	private:

		fence_opt option;
		std::atomic_uint waitingThreads;
		std::atomic_bool shouldWait;

		virtual bool __try_sync() override;
		
	};

}

#endif // !CJS_FENCE_HPP