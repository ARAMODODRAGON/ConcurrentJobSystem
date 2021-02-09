#ifndef CJS_DETAIL_HPP
#define CJS_DETAIL_HPP
#include "../ijob.hpp"

namespace cjs {
	namespace detail {

		struct work final {

			ijob* object = nullptr;

			void(*func)(void*) = nullptr;
			void* func_val = nullptr;

			enum : uint8_t {
				type_object,
				type_func,
				//type_fence,
				type_none
			} type = type_none;
		};

	}
}

#endif // !CJS_DETAIL_HPP