#ifndef CJS_FENCE_HPP
#define CJS_FENCE_HPP
#include "common.hpp"

namespace cjs {

	class ifence {
	public:
		virtual ~ifence() = 0 { }
		virtual void _join() = 0;
		virtual void _tell_thread_count(size_t count) = 0;
	};

	// can quickly stop and start worker threads
	// not very performance efficent 
	class quick_fence : public ifence {
	public:
		
		quick_fence();
		~quick_fence() final;

		// checks if this fence is still valid
		bool is_valid() const;

		// checks if this fence is still valid
		operator bool() const;

		// waits for all threads to be blocked by this fence
		void await();

		// lets all threads resume running
		void resume();

		// resets the fence so it can be reused
		void reset();

		// await() then resume() then reset()
		void await_and_resume();

	private:
		
		std::atomic_bool m_shouldresume;
		std::atomic_size_t m_threadcount;
		std::atomic_size_t m_joinedcount;
		
		void _join() override;
		void _tell_thread_count(size_t count) override;
	};

	// stops the threads but may not wake them immediately
	// saves performance by completely stopping the thread until its next needed
	//class slow_fence : public ifence {
	//public:
	//	
	//	slow_fence();
	//	~slow_fence() final;
	//
	//private:
	//	void _join() override;
	//	void _mark_done() override;
	//};

}

#include "fence.inl"

#endif // !CJS_FENCE_HPP