#include <iostream>
#include <array>
#include <chrono>
#include <cjs\cjs.hpp>

#define REPEAT 70000000

struct Somejob final : public cjs::ijob {
	double a = 0.0;
	void execute() override {
		for (size_t i = 0; i < REPEAT; i++) {
			a += sqrt(30.0 + double(i + 1)) * sqrt((double)i + 1.0);
		}
	}
};

void ThreadingTest() {
	std::array<Somejob, 10> jobs;
	for (size_t i = 0; i < jobs.size(); i++) jobs[i].a = (double)i;
	
	// queue and fence
	cjs::work_queue queue;
	cjs::fence fence;

	// create workers and attach
	std::array<cjs::worker_thread, 10> workers;
	for (auto& worker : workers) worker.attach_to(&queue);

	// start timer
	std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now());

	// run jobs
	for (auto& job : jobs) queue.submit(&job);

	// attach fence so we can wait for the jobs to finish
	queue.submit(&fence);
	fence.await_and_resume();

	// end timer and print
	std::cout << "Multithreaded test: ";
	std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
	std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(_end - _start).count() << std::endl;

	// detach workers
	for (auto& worker : workers) worker.attach_to(nullptr);

}

void NormalTest() {
	std::array<Somejob, 10> jobs;
	for (size_t i = 0; i < jobs.size(); i++) jobs[i].a = (double)i;

	// start timer
	std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now());

	// run
	for (auto& job : jobs) job.execute();

	// end timer and print
	std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
	std::cout << "Single thread test: ";
	std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(_end - _start).count() << std::endl;

}

int main(int argc, char** argv) {

	NormalTest();
	ThreadingTest();

	return 0;
}