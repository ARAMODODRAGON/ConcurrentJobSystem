#include <iostream>
#include "cjs\cjs.hpp"

namespace {
	std::mutex m;
	void printm(float t) {
		std::lock_guard<std::mutex> lg(m);
		printf("time:%f\n", t);
	}
}

struct TimedJob : public cjs::ijob {

	uint32_t time;

	TimedJob(uint32_t time_) : time(time_) {
		if (time < 0.0f) time = 0.0f;
	}

	virtual void execute() {
		printm(time);
		std::this_thread::sleep_for(std::chrono::seconds(time));
		printm(time);
	}
};

int main(int argc, char* argv[]) {

	// initialize the system
	cjs::job_system::init(3);

	// create timed jobs, the numbers are how many seconds each one takes
	TimedJob jobs[] { 1, 2, 2, 5 };
	constexpr size_t jobs_count = sizeof(jobs) / sizeof(TimedJob);

	// create our start and end fences
	cjs::fence start, end;

	// submit the fences and jobs

	start.submit(true);

	for (TimedJob& tj : jobs) cjs::job_system::submit_work(&tj);

	end.submit();

	// run the jobs and time it

	auto startT = std::chrono::steady_clock::now();

	start.wait_and_unblock();
	end.wait();

	auto endT = std::chrono::steady_clock::now();

	std::cout << "seconds: " << std::chrono::duration_cast<std::chrono::duration<float>>(endT - startT).count() << std::endl;

	// close the system
	cjs::job_system::exit();
	return 0;
}