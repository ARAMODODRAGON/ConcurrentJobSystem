#include <iostream>
#include <array>
#include <chrono>
#include <cjs\cjs.hpp>

#define REPEAT 10000000

struct Somejob final : public cjs::ijob {
	double a = 0.0;
	void execute() override {
		for (size_t i = 0; i < REPEAT; i++) {
			a += sqrt(30.0 + double(i)) * sqrt((double)i);
		}
	}
};

#define MULTITEST 

int main(int argc, char** argv) {

	std::array<Somejob, 10> jobs;
	#ifdef MULTITEST

	cjs::work_queue queue;

	// code you want to time here

	// create workers and attach
	std::array<cjs::worker_thread, 11> workers;
	for (auto& worker : workers) worker.attach_to(&queue);

	// start timer
	std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now());

	// run jobs
	for (auto& job : jobs) queue.submit(&job);
	// await
	while (queue.size() > 0) { }

	// end timer and print
	std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
	std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(_end - _start).count();

	// detach workers
	for (auto& worker : workers) worker.attach_to(nullptr);
	#else

	// start timer
	std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now());

	// run
	for (auto& job : jobs) job.execute();

	// end timer and print
	std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
	std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(_end - _start).count();

	#endif
	return 0;
}