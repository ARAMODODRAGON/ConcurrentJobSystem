#ifndef CJS_CONTEXT_HPP
#define CJS_CONTEXT_HPP
#include <thread>
#include <vector>
#include <atomic>
#include "handle.hpp"

namespace cjs {

	// a multithreading context that contains the worker threads and a work queue
	// implements std::thread
	// prioritizes handles containing jobs rather than the internal queue
	template<class HandleTy, class ThreadTy = std::thread, void(HandleTy::job_t::* executefunc)() = &HandleTy::job_t::execute>
	class basic_context {
	public:
		using handle_t = HandleTy;
		using thread_t = ThreadTy;
		using job_t = typename handle_t::job_t;
		using storage_t = typename HandleTy::storage_t;
		using mutex_t = typename HandleTy::mutex_t;
		using lock_guard_t = std::lock_guard<mutex_t >;

		basic_context(const basic_context&) = delete;
		basic_context& operator=(const basic_context&) = delete;
		basic_context(basic_context&&) = delete;
		basic_context& operator=(basic_context&&) = delete;

		// initializes the worker threads
		basic_context(size_t threadcount) : m_shouldquit(false) {
			m_workers.resize(threadcount);
			size_t index = 0;
			for (auto& worker : m_workers)
				worker = thread_t(_worker_thread, index++, this);
		}

		~basic_context() {
			m_shouldquit = true;
			for (auto& worker : m_workers)
				worker.join();
		}

		// submits a job to the internal queue to be worked on asap
		void submit(job_t* job) {
			lock_guard_t _(m_lock);
			m_queue.push_back(job);
		}

		// submits a handle to be worked on
		void submit(handle_t* handle) {
			lock_guard_t _(m_lock);
			if (handle->size() > 0 && handle->status() != handle_status::submitted) {
				m_handles.emplace_back();
				qhandle& qh = m_handles.back();
				qh.handle = handle;
				qh.iter = handle->begin();
				handle->__mark_submitted();
			}
		}

		// the size of the internal queue
		size_t size() const {
			lock_guard_t _(m_lock);
			return m_queue.size();
		}

	private:
		mutex_t m_lock;
		storage_t m_queue;
		struct qhandle final {
			handle_t* handle = nullptr;
			std::atomic_size_t finishedcount = 0;
			typename handle_t::iterator_t iter;
		};
		std::list<qhandle> m_handles;
		std::vector<thread_t> m_workers;
		std::atomic_bool m_shouldquit;

		static void _worker_thread(const size_t thr_index, basic_context* context) {
			while (!context->m_shouldquit) {
				bool ishandlejob = false;
				job_t* job = context->_get_job(&ishandlejob);
				if (job) {
					(job->*executefunc)();
					if (ishandlejob) {
						context->_fin_job();
					}
				}
			}
		}

		job_t* _get_job(bool* ishandlejob) {
			lock_guard_t _(m_lock);

			if (m_handles.size() > 0) {
				qhandle& qh = m_handles.front();
				auto iter = qh.iter;
				if (qh.iter != qh.handle->end()) {
					qh.iter++;
					(*ishandlejob) = true;
					return *iter;
				}
			}

			if (m_queue.size() > 0) {
				job_t* job = m_queue.front();
				m_queue.pop_front();
				return job;
			}

			return nullptr;
		}

		void _fin_job() {
			lock_guard_t _(m_lock);
			if (m_handles.size() > 0) {
				qhandle& qh = m_handles.front();
				if (qh.handle->size() == ++(qh.finishedcount)) {
					qh.handle->__mark_complete();
					m_handles.pop_front();
				}
			}
		}

	};

}

#endif // !CJS_CONTEXT_HPP