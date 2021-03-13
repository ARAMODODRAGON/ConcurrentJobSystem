#ifndef CJS_LOCKED_ARRAY_HPP
#define CJS_LOCKED_ARRAY_HPP
#include <stdexcept>
#include <type_traits>
#include <mutex>

namespace cjs {

	// array that locks each individual index using a mutex
	template<typename T, size_t sz>
	class array {

		// thread safe accessor 
		struct accessor {
			accessor(T& data, std::mutex& lock);
			operator T() const;
			accessor& operator=(const T& val);
		private:
			T& m_data;
			std::mutex& m_lock;
		};

		using lock_guard = std::lock_guard<std::mutex>;

	public:

		// default constructs every member
		array();

		// constructs every member with the given value
		array(const T& val);

		// copy & move
		array(const array& other);
		array& operator=(const array& other);
		array(array&& other);
		array& operator=(array&& other);

		// safely gets the item at the given index
		accessor operator[](size_t i);

		// safely returns a copy of the item at the index
		T operator[](size_t i) const;

		// safely invokes a callable at the given index
		template<typename Callable>
		void invoke_on(size_t i, Callable callable);

		constexpr size_t size();

	private:
		std::mutex m_locks[sz];
		T m_data[sz];
	};

	template<typename T, size_t sz>
	inline array<T, sz>::array() { }

	template<typename T, size_t sz>
	inline array<T, sz>::array(const T& val) {
		for (size_t i = 0; i < size(); i++) {
			m_data[i] = val;
		}
	}

	template<typename T, size_t sz>
	inline array<T, sz>::array(const array& other) {
		for (size_t i = 0; i < size(); i++) {
			lock_guard _0(other.m_locks[i]);
			m_data[i] = other[i];
		}
	}

	template<typename T, size_t sz>
	inline array<T, sz>& array<T, sz>::operator=(const array& other) {
		for (size_t i = 0; i < size(); i++) {
			lock_guard _0(m_locks[i]);
			lock_guard _1(other.m_locks[i]);
			m_data[i] = other[i];
		}
		return *this;
	}

	template<typename T, size_t sz>
	inline array<T, sz>::array(array&& other) {
		for (size_t i = 0; i < size(); i++) {
			lock_guard _0(other.m_locks[i]);
			m_data[i] = std::move(other[i]);
		}
	}

	template<typename T, size_t sz>
	inline array<T, sz>& array<T, sz>::operator=(array&& other) {
		for (size_t i = 0; i < size(); i++) {
			lock_guard _0(m_locks[i]);
			lock_guard _1(other.m_locks[i]);
			m_data[i] = std::move(other[i]);
		}
		return *this;
	}

	template<typename T, size_t sz>
	inline cjs::array<T, sz>::accessor cjs::array<T, sz>::operator[](size_t i) {
		return accessor(m_data[i], m_locks[i]);
	}

	template<typename T, size_t sz>
	inline T cjs::array<T, sz>::operator[](size_t i) const {
		lock_guard _(m_locks[i]);
		return m_data[i];
	}

	template<typename T, size_t sz>
	template<typename Callable>
	inline void cjs::array<T, sz>::invoke_on(size_t i, Callable callable) {
		lock_guard _(m_locks[i]);
		callable(m_data[i]);
	}

	template<typename T, size_t sz>
	inline constexpr size_t cjs::array<T, sz>::size() {
		return sz;
	}

	template<typename T, size_t sz>
	inline cjs::array<T, sz>::accessor::accessor(T& data, std::mutex& lock)
		: m_data(data), m_lock(lock) { }

	template<typename T, size_t sz>
	inline cjs::array<T, sz>::accessor::operator T() const {
		lock_guard _(m_lock);
		return m_data;
	}

	template<typename T, size_t sz>
	inline cjs::array<T, sz>::accessor& cjs::array<T, sz>::accessor::operator=(const T& val) {
		lock_guard _(m_lock);
		m_data = val;
		return *this;
	}

}

#endif // !CJS_LOCKED_ARRAY_HPP