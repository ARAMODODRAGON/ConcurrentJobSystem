#ifndef CJS_INTERFACES_HPP
#define CJS_INTERFACES_HPP

namespace cjs {

	// base for all jobs
	struct ijob {
		virtual ~ijob() = 0 { }
		virtual void execute() = 0;
	};

	// base for all fence types
	// TODO: needs to be implemented
	struct ifence;

}

#endif // !CJS_INTERFACES_HPP