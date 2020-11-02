#ifndef CJS_INTERFACES_HPP
#define CJS_INTERFACES_HPP

namespace cjs {

	// base for all jobs
	struct ijob {

		virtual ~ijob() = 0 { }
		virtual void execute() = 0;

	};

	// base for all fence types
	struct ifence {

		virtual ~ifence() = 0 { }

		// interal functions
		virtual void __sync() = 0;

	};

}

#endif // !CJS_INTERFACES_HPP