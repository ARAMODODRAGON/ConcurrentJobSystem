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

			// a number used to count how many threads have accessed this work
			std::atomic_uint32_t accessedCount;

			// checks if this work struct is valid
			// this is not valid if no job or fence is attached
			bool is_valid() const { return job; }

			// constructs an invalid work struct
			work() : type(TYPE_NULL), job(nullptr), accessedCount(0) { }
			
			// constructs with a job
			work(ijob* job_) : type(TYPE_IJOB), job(job_), accessedCount(0) { }

			// constructs with a fence
			work(ifence* fence_) : type(TYPE_FENCE), fence(fence_), accessedCount(0) { }

			// constructs with a job function
			#ifndef DONT_USE_FUNCTIONAL
			work(std::function<void(void*)> jobfunc_, void* jobdata_ = nullptr) 
				: type(TYPE_JOBFUNC), jobdata(jobdata_), jobfunc(jobfunc_), accessedCount(0) { }
			#endif



			work(const work& other) : type(other.type) {
				switch (type) {
					case cjs::detail::work::TYPE_NULL:
						job = nullptr;
						break;
					case cjs::detail::work::TYPE_IJOB:
						job = other.job;
						break;
					case cjs::detail::work::TYPE_FENCE:
						fence = other.fence;
						break;
					default:break;
				}
			}

			work& operator=(const work& other) {
				type = other.type;
				switch (type) {
					case cjs::detail::work::TYPE_NULL:
						job = nullptr;
						break;
					case cjs::detail::work::TYPE_IJOB:
						job = other.job;
						break;
					case cjs::detail::work::TYPE_FENCE:
						fence = other.fence;
						break;
					default:break;
				}
				return *this;
			}

		};

	}

}

#endif // !CJS_DETAIL_WORK_HPP