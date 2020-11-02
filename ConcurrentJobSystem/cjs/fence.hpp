#ifndef CJS_FENCE_HPP
#define CJS_FENCE_HPP
#include "common.hpp"
#include "interfaces.hpp"

namespace cjs {

	// used to block and sync all worker threads
	// becomes invalid after use but can be reset using fence::reset();
	struct fence : ifence {

		// constructs a valid fence
		fence();

		// stops the main thread if this is a submitted fence so the working threads can sync
		~fence() final;

		// not copyable
		fence(const fence&) = delete;
		fence& operator=(const fence&) = delete;

		// submits this fence to the work queue
		// can only be submitted once
		// blockthreads - makes threads wait for main to wake them up (does not actually sleep threads)
		void submit(bool blockthreads = false);

		// waits for the threads to sync and then returns
		void wait() const;

		// unblocks the waiting threads if they were blocked by fence::submit
		void unblock_threads();

		// waits for the threads to sync and then unblocks them
		void wait_and_unblock();

		// checks if this is a valid fence
		// fence is invalid if it has been used
		bool is_valid() const { return !invalid; }

		// resets this fence so it can be used again
		void reset();

		// internal functions
		virtual void __sync() override;

	private:

		std::atomic_bool is_submitted;
		std::atomic_uint32_t synced_threads;
		std::atomic_bool wait_threads;
		std::atomic_bool invalid;

	};

}

#endif // !CJS_FENCE_HPP