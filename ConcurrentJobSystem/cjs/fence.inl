
namespace cjs {

	inline fence::fence()
		: m_done(false), m_shouldresume(false) { }

	inline fence::~fence() {
		await_and_resume();
	}

	inline void fence::await() {
		while (!m_done);
	}

	inline void fence::resume() {
		m_shouldresume = true;
	}

	inline void fence::reset() {
		m_done = m_shouldresume = false;
	}

	inline void fence::await_and_resume() {
		await();
		resume();
	}

	inline void fence::_join() {
		while (!m_shouldresume);
	}

	inline void fence::_mark_done() {
		m_done = true;
	}
}