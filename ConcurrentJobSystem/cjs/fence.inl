
namespace cjs {

	inline quick_fence::quick_fence()
		: m_done(false), m_shouldresume(false) { }

	inline quick_fence::~quick_fence() {
		await_and_resume();
	}

	inline void quick_fence::await() {
		while (!m_done);
	}

	inline void quick_fence::resume() {
		m_shouldresume = true;
	}

	inline void quick_fence::reset() {
		m_done = m_shouldresume = false;
	}

	inline void quick_fence::await_and_resume() {
		await();
		resume();
	}

	inline void quick_fence::_await() {
		while (!m_shouldresume);
	}

	inline void quick_fence::_mark_done() {
		m_done = true;
	}
}