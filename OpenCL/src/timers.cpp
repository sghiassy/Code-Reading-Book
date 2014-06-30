/*************************************************
* Timestamp Functions Source File                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <ctime>
#include <opencl/util.h>

namespace OpenCL {

/*************************************************
* Timestamp Functions                            *
*************************************************/
u64bit system_time()
   {
   return std::time(0);
   }

u64bit system_clock()
   {
   return std::clock();
   }

}
