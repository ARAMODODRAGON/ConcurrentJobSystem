#ifndef CJS_WORK_QUEUE_HPP
#define CJS_WORK_QUEUE_HPP
#include <mutex>
#include <thread>
#include <vector>
#include <inttypes.h>

namespace cjs {

	class ijob;
	class worker_thread;
	class ifence;

	// optional options for setting work
	enum class work_opt : uint8_t {
		// default value
		none = 0,
		// invokes the given job once by all threads
		work_all = 0b1
	};

	class work_queue {

		// non copyable
		work_queue(const work_queue&) = delete;
		work_queue& operator=(const work_queue&) = delete;

	public:

		// constructs an invalid work queue
		work_queue();

		// constructs the work queue with "worker_thread_count" number of workers attached
		work_queue(const size_t worker_thread_count);

		// destructor (self explanatory)
		~work_queue();

		// (also self explanatory)
		work_queue(work_queue&& other);

		// (also also self explanatory)
		work_queue& operator=(work_queue&& other);

		// submit a job to be worked on
		void submit(ijob* job_object, work_opt opt = work_opt::none);

		// submit a function to call
		void submit(void(*job_func)(), work_opt opt = work_opt::none);

		// submit a fence
		void submit(ifence* job_fence);

	private:

		using mutex = std::mutex;
		using mutex_guard = std::lock_guard<std::mutex>;
		using atomic_bool = std::atomic_bool;

		struct work_node {
			work_node* next;

			union {
				ijob* object;
				void(*func)();
				ifence* fence;
			};
			work_opt option;
			size_t call_count;

			enum : uint8_t {
				type_object,
				type_func,
				type_fence
			} type;
		};

		std::thread* worker_threads;
		size_t worker_sz;

		work_node* work_list_front;
		work_node* work_list_back;
		size_t list_sz;
		mutex list_lock;

		work_node* node_pool; 
		size_t pool_sz;
		mutex pool_lock;
		
		atomic_bool should_quit;
		void worker_thread(uint32_t threadID);

	};

}

#endif // !CJS_WORK_QUEUE_HPP