#ifndef CJS_HANDLE_HPP
#define CJS_HANDLE_HPP
#include <list>
#include <mutex>
#include <thread>

namespace cjs {

	enum class handle_status : uint8_t {
		invalid,			// initial state
		submitted,			// when the handle is currently being worked on
		complete			// when the handles last submission to a context is completed
	};

	// contains a list of jobs to submit and run
	template<class JobTy, class StorageTy = std::list<JobTy*>, class MutexTy = std::mutex>
	struct basic_handle {
		using job_t = JobTy;
		using storage_t = StorageTy;
		using iterator_t = typename StorageTy::iterator;
		using mutex_t = MutexTy;
		using lock_guard_t = std::lock_guard<mutex_t>;

		// default constructor
		basic_handle() : m_status(handle_status::invalid) { }

		// destructor
		~basic_handle() = default;

		// copy and move

		basic_handle(const basic_handle& other) : m_status(handle_status::invalid) {
			lock_guard_t _(other.m_lock);
			m_storage = other.m_storage;
		}

		basic_handle& operator=(const basic_handle& other) {
			lock_guard_t _0(m_lock);
			lock_guard_t _1(other.m_lock);
			m_storage = other.m_storage;
			m_status = handle_status::invalid;
			return *this;
		}

		basic_handle(basic_handle&& right)
			: m_status(handle_status::invalid), m_storage(std::move(right.m_storage)) { }

		basic_handle& operator=(basic_handle&& right) {
			lock_guard_t _(m_lock);
			m_storage = std::move(right.m_storage);
			m_status = handle_status::invalid;
			return *this;
		}

		// adds the job to the back of the handle
		void push(job_t* job) {
			if (job != nullptr) {
				lock_guard_t _(m_lock);
				m_storage.push_back(job);
			}
		}

		// adds the job to the back of the handle
		basic_handle& operator+=(job_t* job) {
			push(job);
			return *this;
		}

		// pops and returns the topmost job from the handle
		job_t* pop() {
			lock_guard_t _(m_lock);
			job_t* job = m_storage.front();
			m_storage.pop_front();
			return job;
		}

		// returns the topmost job from the handle
		job_t* peek() {
			lock_guard_t _(m_lock);
			return m_storage.front();
		}

		// finds and removes the given job on the handle
		// returns true on success
		bool pop(const job_t* job) {
			lock_guard_t _(m_lock);
			for (auto it = begin(); it != end(); ++it) {
				if ((*it) == job) {
					m_storage.erase(it);
					return true;
				}
			}
			return false;
		}

		// returns the status of this handle
		handle_status status() const {
			lock_guard_t _(m_lock);
			return m_status;
		}

		// resets the handles status if its not currently being worked on
		void reset_status() {
			lock_guard_t _(m_lock);
			if (m_status != handle_status::submitted)
				m_status = handle_status::invalid;
		}

		// returns the size of the internal storage
		size_t size() const {
			lock_guard_t _(m_lock);
			return m_storage.size();
		}

		// returns an iterator starting at the begining of the handle
		iterator_t begin() {
			lock_guard_t _(m_lock);
			return m_storage.begin();
		}

		// returns an iterator at the end of the handle
		iterator_t end() {
			lock_guard_t _(m_lock);
			return m_storage.end();
		}

		// waits for the status to become complete
		void await_complete() const {
			if (status() == handle_status::submitted) {
				while (status() != handle_status::complete) {
					std::this_thread::yield();
				}
			}
		}

	private:
		storage_t m_storage;
		handle_status m_status;
		mutable MutexTy m_lock;
	public:
		void __mark_submitted() {
			lock_guard_t _(m_lock);
			m_status = handle_status::submitted;
		}
		void __mark_complete() {
			lock_guard_t _(m_lock);
			m_status = handle_status::complete;
		}
	};

}

#endif // !CJS_HANDLE_HPP