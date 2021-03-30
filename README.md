# ConcurrentJobSystem
 
### What is this?
This is a general use job system that I threw together. You're allowed to use this in your projects as long as you credit me.

### How do I use it?
See the following:

```cpp
#include <cjs/cjs.hpp>

// create the context with 10 worker threads
cjs::context* context = new cjs::context(10);

struct MyJob : public cjs::ijob {
 void execute() override {
  // do something here
 }
};

// create a job to submit
MyJob job;
context->submit(&job);

// create a handle to store multiple jobs
cjs::handle handle;
for (size_t i = 0; i < 10; i++) handle += new MyJob();

// submit all jobs in the handle
context->submit(&handle);

// you can check the status 
if (handle.status() == cjs::handle_status::complete) {
 // do something
}

// or you can wait for the jobs to complete
handle.await_complete();

for (auto* job : handle) delete job;
handle.clear();

// deleting the context will join all the threads
delete context;
context = nullptr;

```

### Installation
This is a header only library and as per usual only needs to be added to your includes. 

Download can be found [here](https://github.com/ARAMODODRAGON/ConcurrentJobSystem/releases/download/v0.2.1/cjs.0.2.1.zip) or in the side bar.

### Credits
Domara Shlimon 
