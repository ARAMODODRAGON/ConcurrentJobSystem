#include <iostream>
#include "cjs\fence.hpp"

int main(int argc, char* argv[]) {

	/*
	cjs::fence begin;
	begin.submit();

	// submit some jobs

	cjs::fence collisionBegin;
	collisionBegin.submit();

	// submit the collision jobs
	
	cjs::fence collisionEnd;
	collisionEnd.submit(cjs::fence_opt::wait_for_main);
	
	// do some remaining stuff

	collisionEnd.sync_and_start();

	*/

	std::cout << "Hello World!" << std::endl;

	return 0;
}