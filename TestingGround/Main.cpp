#include <iostream>
#include <array>
#include <chrono>
#define REPEAT 70000000
#include <cjs\cjs.hpp>


struct Somejob final : public cjs::ijob {
	double a = 0.0;
	void execute() override {
		for (size_t i = 0; i < REPEAT; i++) {
			a += sqrt(30.0 + double(i + 1)) * sqrt((double)i + 1.0);
		}
	}
};

void ThreadingTest() {
	struct jobpair {
		cjs::job_status status;
		cjs::shared<Somejob> job;
	};

	constexpr size_t count = 10;
	std::array<jobpair, count> jobs;
	for (size_t i = 0; i < jobs.size(); i++) {
		jobs[i].job.reset(new Somejob());
		jobs[i].job->a = (double)i;
	}

	// queue and handle
	cjs::context* context = new cjs::context();

	// start timer
	std::chrono::steady_clock::time_point _start(std::chrono::steady_clock::now());

	// add jobs to handle and then submit
	for (size_t i = 0; i < count; ++i) context->submit(jobs[i].job, &(jobs[i].status));

	// await
	for (auto& job : jobs) cjs::await_complete(job.status);

	// end timer and print
	std::cout << "Multithreaded test: ";
	std::chrono::steady_clock::time_point _end(std::chrono::steady_clock::now());
	std::cout << std::chrono::duration_cast<std::chrono::duration<double>>(_end - _start).count() << std::endl;

	delete context; context = nullptr;
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

struct JobTest : public cjs::ijob {
	~JobTest() {
		printf("Deleted!\n");
	}
	void execute() override {
		printf("Test!\n");
	}
};

int main(int argc, char** argv) {

	NormalTest();
	ThreadingTest();

	return 0;
}