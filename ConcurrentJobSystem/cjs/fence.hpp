#ifndef CJS_FENCE_HPP
#define CJS_FENCE_HPP

namespace cjs {

	class ifence {
	public:
		virtual ~ifence() = 0 { }
		virtual void _execute() = 0;
	};

	// can quickly stop and start worker threads
	// not very performance efficent 
	class quick_fence : public ifence {
	public:
		
		quick_fence();
		~quick_fence() final;

	public:
		virtual void _execute();
	};

	// stops the threads but may not wake them immediately
	// saves performance by completely stopping the thread until its next needed
	class slow_fence : public ifence {
	public:
		
		slow_fence();
		~slow_fence() final;

	public:
		virtual void _execute();
	};

}

#endif // !CJS_FENCE_HPP