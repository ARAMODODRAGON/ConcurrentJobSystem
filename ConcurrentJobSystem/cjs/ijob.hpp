#ifndef CJS_IJOB_HPP
#define CJS_IJOB_HPP

namespace cjs {

	// generic interface for jobs
	struct ijob {
		virtual ~ijob() = 0 { }
		virtual void execute() = 0;
	};

}

#endif // !CJS_IJOB_HPP