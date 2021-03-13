
namespace cjs {

	inline quick_fence::quick_fence()
		: m_shouldresume(false), m_threadcount(0) { }

	inline quick_fence::~quick_fence() {
		await_and_resume();
	}

	inline bool quick_fence::is_valid() const {
		return m_threadcount != 0;
	}

	inline quick_fence::operator bool() const {
		return is_valid();
	}

	inline void quick_fence::await() {
		while (m_joinedcount != m_threadcount && m_threadcount != 0);
	}

	inline void quick_fence::resume() {
		m_shouldresume = true;
		m_threadcount = m_joinedcount = 0;
	}

	inline void quick_fence::reset() {
		CJS_ASSERT(m_threadcount == 0, "Cannot reset while fence is still in queue");
		m_shouldresume = false;
	}

	inline void quick_fence::await_and_resume() {
		await();
		resume();
	}

	inline void quick_fence::_join() {
		++m_joinedcount;
		while (!m_shouldresume);
	}

	inline void quick_fence::_tell_thread_count(size_t count) {
		m_threadcount = count;
	}
}