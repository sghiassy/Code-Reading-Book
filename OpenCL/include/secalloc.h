/*************************************************
* Secure Memory Allocator Header File            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SECURE_ALLOCATION_H__
#define OPENCL_SECURE_ALLOCATION_H__

#include <cstring>
#include <opencl/util.h>

namespace OpenCL {

/*************************************************
* SecureAllocator                                *
*************************************************/
template<typename T>
struct SecureAllocator
   {
   static T* allocate(u32bit n)
      {
      if(n == 0) return 0;
      T* ptr = new T[n];
      //fprintf(stderr, "ALLOC: create %d at %p\n", n, ptr);
      lock(ptr, n);
      clear(ptr, n);
      return ptr;
      }
   static void deallocate(T* ptr, u32bit n)
      {
      if(ptr == 0) return;
      clear(ptr, n);
      //fprintf(stderr, "ALLOC: destroy %d at %p\n", n, ptr);
      unlock(ptr, n);
      delete[] ptr;
      }
   static void clear(T* ptr, u32bit n)  { std::memset(ptr, 0, sizeof(T)*n); }
   static void lock(T* ptr, u32bit n)   { lock_mem(ptr, sizeof(T)*n); }
   static void unlock(T* ptr, u32bit n) { unlock_mem(ptr, sizeof(T)*n); }
   };

}

#endif
