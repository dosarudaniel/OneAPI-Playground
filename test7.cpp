#include <CL/sycl.hpp>
#include <iostream>
#include <AdePT/1/Atomic.h>
#include <atomic>

#define N 1

// Example data structure containing several atomics
struct SomeStruct {
  adept::Atomic_t<int> var_int;
  adept::Atomic_t<float> var_float;
  SomeStruct() {}
  static SomeStruct *MakeInstanceAt(void *addr)
  {
    SomeStruct *obj = new (addr) SomeStruct();
    return obj;
  }
};

// Kernel function to perform atomic addition
void testAdd(SomeStruct *s)  {
  // Test fetch_add, fetch_sub
  //for (int i=0; i<N; i++){
    s->var_int.fetch_add(1);
    s->var_float.fetch_add(1);
    //s++; 
    // }
}

//______________________________________________________________________________________
int main(void)
{

  sycl::default_selector device_selector;

  sycl::queue q_ct1(device_selector);
  std::cout <<  "Running on "
	    << q_ct1.get_device().get_info<cl::sycl::info::device::name>()
	    << "\n";

  // Allocate the content of SomeStruct in a buffer

  char *buffer = nullptr;
  buffer        = (char*) sycl::malloc_shared(sizeof(SomeStruct)*N, q_ct1);
  // SomeStruct *a = SomeStruct::MakeInstanceAt(buffer);
  char *s = buffer;

  for (int i=0; i<N; i++) {
      std::cout<<i;
      SomeStruct *a = SomeStruct::MakeInstanceAt(buffer);
      buffer += sizeof(SomeStruct);
      std::cout << i <<","<< a->var_int.load() << "\n";
  }

    // Wait memory to reach device
  q_ct1.wait_and_throw();

  // Define the kernels granularity: 10K blocks of 32 treads each
  sycl::range<3> nblocks(1, 1, 10000), nthreads(1, 1, 32);

  q_ct1.submit([&](sycl::handler &cgh) {
    cgh.parallel_for(sycl::nd_range<3>(nblocks * nthreads, nthreads), [=](sycl::nd_item<3> item_ct1) {
	testAdd(buffer);
    });
  });

  q_ct1.wait_and_throw();
  
  for (int i=0; i<N; i++) {
      std::cout<<i;
      std::cout << i <<","<< s->var_int.load() << "\n";
      s++;
  }

  
  return(0);
}
