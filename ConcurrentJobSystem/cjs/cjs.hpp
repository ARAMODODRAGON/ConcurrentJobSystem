#ifndef CJS_TYPEDEFS_HPP
#define CJS_TYPEDEFS_HPP
#include "ijob.hpp"
#include "context.hpp"

namespace cjs {

	using handle = basic_handle<ijob>;

	using context = basic_context<handle>;

}

#endif // !CJS_TYPEDEFS_HPP