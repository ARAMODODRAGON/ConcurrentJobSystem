/*
MIT License

Copyright (c) 2022 Domara Shlimon

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef CJS_CJS_HPP
#define CJS_CJS_HPP
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <list>
#include <vector>
#include <memory>

namespace cjs {

	template<typename T>
	using shared = std::shared_ptr<T>;

	// basic interface for jobs
	struct ijob {
		virtual ~ijob() = 0 { }
		virtual void execute() = 0;
	};

	// completion status of job
	struct job_status final {
		enum type : uint8_t {
			incomplete,
			complete
		};

		job_status(type val = incomplete) : m_val(val) { }

		job_status& operator=(type val) {
			m_val = val;
			return *this;
		}

		bool operator==(job_status other) const {
			return m_val == other.m_val;
		}

		bool operator!=(job_status other) const {
			return m_val != other.m_val;
		}

	private:
		std::atomic<type> m_val;
	};

	// awaits for the job_status value to become job_status::complete
	void await_complete(const job_status* status);

	// awaits for the job_status value to become job_status::complete
	void await_complete(const job_status& status);

	// awaits for all job_status values to become job_status::complete
	template<typename RangeType>
	void await_complete(RangeType begin, RangeType end);

	// handles creating worker threads and distributing jobs to different threads
	template<typename ThreadTy>
	class basic_context final {
	public:
		basic_context(const basic_context&) = delete;
		basic_context& operator=(const basic_context&) = delete;
		basic_context(basic_context&&) = delete;
		basic_context& operator=(basic_context&&) = delete;

		// starts the worker threads
		// defaults to the maximum available threads on the system
		basic_context(size_t workerCount = 0);

		// closes the worker threads
		~basic_context();


		// submits a function to be worked on
		// optional completion status 
		void submit(std::function<void()> func, job_status* status = nullptr);

		// submits a job to be worked on
		// optional completion status 
		void submit(shared<ijob> job, job_status* status = nullptr);


	private:
		struct job_node final {
			std::function<void()> func;
			shared<ijob> job;
			job_status* status = nullptr;
		};
		std::atomic_bool m_exit;
		std::mutex m_lock;
		std::list<job_node> m_jobs;
		std::vector<ThreadTy> m_threads;

		static void _worker_thread(const size_t index, basic_context* c);
	};

	using context = basic_context<std::thread>;

	// implementations

	inline void await_complete(const job_status* status) {
		while (*status != job_status::complete) { }
	}
	
	inline void await_complete(const job_status& status) {
		await_complete(&status);
	}

	template<typename RangeType>
	inline void await_complete(RangeType begin, RangeType end) {
		for (auto it = begin; it != end; it++) {
			await_complete(&*it);
		}
	}

	template<typename ThreadTy>
	inline basic_context<ThreadTy>::basic_context(size_t workerCount) : m_exit(false) {
		if (workerCount == 0)
			workerCount = std::thread::hardware_concurrency() - 1;
		m_threads.resize(workerCount);
		for (size_t i = 0; i < m_threads.size(); i++) {
			m_threads[i] = std::thread(&basic_context<ThreadTy>::_worker_thread, i, this);
		}
	}

	template<typename ThreadTy>
	inline basic_context<ThreadTy>::~basic_context() {
		m_exit = true; // tell the worker threads to quit
		for (size_t i = 0; i < m_threads.size(); i++) {
			m_threads[i].join();
		}
	}

	template<typename ThreadTy>
	inline void basic_context<ThreadTy>::submit(std::function<void()> func, job_status* status) {
		job_node n;
		n.func = func;
		n.status = status;
		*status = job_status::incomplete;
		m_jobs.push_back(n);
	}

	template<typename ThreadTy>
	inline void basic_context<ThreadTy>::submit(shared<ijob> job, job_status* status) {
		job_node n;
		n.job = job;
		n.status = status;
		*status = job_status::incomplete;
		m_jobs.push_back(n);
	}

	template<typename ThreadTy>
	inline void basic_context<ThreadTy>::_worker_thread(const size_t index, basic_context* c) {
		job_node n;
		while (!c->m_exit) {
			// reset
			n.func = nullptr;
			n.job.reset();
			n.status = nullptr;

			// wait for job
			while ((!n.func && !n.job) && !c->m_exit) {
				std::lock_guard<std::mutex> _0(c->m_lock);

				// check if there are any jobs
				if (c->m_jobs.size() > 0) {
					n = c->m_jobs.front();
					c->m_jobs.pop_front();
				}
			}

			// if there is a job, run it
			if (!n.func != !n.job) {
				// run job
				if (n.func) n.func();
				if (n.job) n.job->execute();
				// set status
				if (n.status) *(n.status) = job_status::complete;
			}
		}
	}

}

#endif // !CJS_CJS_HPP