# ConcurrentJobSystem
 
### What is this?
This is a general use job system that I threw together. You're allowed to use this in your projects as long as you credit me.

### How do I use it?
See the following:

```cpp
#include <cjs/cjs.hpp>

// worker threads are the seperate threads
cjs::worker_thread workers[4];

// holds a list of cjs::ijobs to be worked on 
cjs::work_queue queue;

// starts the threads and has them start working on the jobs on the queue
// jobs that are worked on are discarded and arnt memory managed
for (auto& worker : workers) worker.attach_to(&queue);

// create our job and submit it to be worked on
struct MyJob : public ijob {
 void execute() override {
  // do something here
 }
};

MyJob jobs[10];
for (auto& job : jobs) queue.submit(&job);

// the threads are already running and will work on those jobs in the background
// we can wait for them to finish using a fence
cjs::quick_fence fence;
queue.submit(&fence);
fence.await_and_resume();

// threads stop when they are destructed or when detached from the queue
for (auto& worker : workers) worker.attach_to(nullptr);

```

### Installation
This is a header only library and as per usual only needs to be added to your includes. 

Download can be found [here](https://github.com/ARAMODODRAGON/ConcurrentJobSystem/releases/download/v0.1.1/cjs.v0.1.1.zip) or in the side bar.

### Credits
Domara Shlimon 
