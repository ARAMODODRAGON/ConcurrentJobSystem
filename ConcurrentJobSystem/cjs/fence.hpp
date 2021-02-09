#ifndef CJS_FENCE_HPP
#define CJS_FENCE_HPP

namespace cjs {

	class ifence {
	public:
		virtual ~ifence() = 0 { }
		virtual void execute() = 0;
	};

	// can quickly stop and start worker threads
	// not very performance efficent 
	class quick_fence : protected ifence {
	public:
		
		quick_fence();
		~quick_fence() final;

	private:
		void execute() override;
	};

	// stops the threads but may not wake them immediately
	// saves performance by completely stopping the thread until its next needed
	class slow_fence : protected ifence {
	public:
		
		slow_fence();
		~slow_fence() final;

	private:
		void execute() override;
	};

}

#endif // !CJS_FENCE_HPP