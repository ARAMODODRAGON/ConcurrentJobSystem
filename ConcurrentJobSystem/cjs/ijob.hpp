#ifndef CJS_IJOB_HPP
#define CJS_IJOB_HPP

namespace cjs {

	class ijob {
	public:
		virtual ~ijob() = 0 { }
		virtual void execute() = 0;
	};

}

#endif // CJS_!IJOB_HPP