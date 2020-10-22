#ifndef CJS_DETAIL_WORK_HPP
#define CJS_DETAIL_WORK_HPP

#define DONT_USE_FUNCTIONAL
#ifndef DONT_USE_FUNCTIONAL
#include <functional>
#endif 

#include "../common.hpp"

namespace cjs {

	struct ijob;
	struct ifence;

	namespace detail {

		struct work {
			enum : uint8 {
				TYPE_NULL,
				TYPE_IJOB,
				TYPE_FENCE
				#ifndef DONT_USE_FUNCTIONAL
				, TYPE_JOBFUNC
				#endif
			} type;

			union {
				// object-oriented job
				ijob* job;

				// fence used to syncronize threads
				ifence* fence;

				// data that should be paired with a jobfunc
				#ifndef DONT_USE_FUNCTIONAL
				void* jobdata;
				#endif
			};

			// a non oo job 
			#ifndef DONT_USE_FUNCTIONAL
			std::function<void(void*)> jobfunc;
			#endif

			// checks if this work struct is valid
			// this is not valid if no job or fence is attached
			bool is_valid() const { return job; }

			// constructs an invalid work struct
			work() : type(TYPE_NULL), job(nullptr) { }
			
			// constructs with a job
			work(ijob* job_) : type(TYPE_IJOB), job(job_) { }

			// constructs with a fence
			work(ifence* fence_) : type(TYPE_FENCE), fence(fence_) { }

			// constructs with a job function
			#ifndef DONT_USE_FUNCTIONAL
			work(std::function<void(void*)> jobfunc_, void* jobdata_ = nullptr) 
				: type(TYPE_JOBFUNC), jobdata(jobdata_), jobfunc(jobfunc_) { }
			#endif

		};

	}

}

#endif // !CJS_DETAIL_WORK_HPP