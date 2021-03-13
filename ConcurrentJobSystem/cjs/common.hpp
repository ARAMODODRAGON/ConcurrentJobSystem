#ifndef CJS_COMMON_HPP
#define CJS_COMMON_HPP

#include <inttypes.h>
#include <assert.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>

#define CJS_NO_COPY(Type)				\
Type(const Type&) = delete;				\
Type& operator=(const Type&) = delete;	\

#define CJS_NO_MOVE(Type)			\
Type(Type&&) = delete;				\
Type& operator=(Type&&) = delete;	\

#if _DEBUG
#define CJS_ASSERT(val, msg) assert(val)
#else
#define CJS_ASSERT(val, msg)
#endif

namespace cjs {

	using mutex = std::mutex;
	using atomic_bool = std::atomic_bool;
	using mutex_guard = std::lock_guard<std::mutex>;
	using thread = std::thread;

	// returns true if the caller thread is the main thread
	bool is_this_main();

}

#include "common.inl"

#endif // !CJS_COMMON_HPP