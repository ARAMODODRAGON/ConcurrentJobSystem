#include "job_system.hpp"

static void dosomething();

namespace cjs {

	namespace {



	}

	void job_system::init(int32 threads) { }

	void job_system::exit() { }

	void job_system::submit_work(detail::work work) { }

	bool job_system::is_caller_main_thread() {
		return false;
	}

	uint32 job_system::get_worker_count() {
		return uint32();
	}

}