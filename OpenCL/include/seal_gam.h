/*************************************************
* SEAL Gamma Function Header File                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SEAL_GAMMA_H__
#define OPENCL_SEAL_GAMMA_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Gamma
   {
   public:
      Gamma(const byte[20]);
      u32bit operator()(u32bit);
   private:
      SecureBuffer<u32bit, 5> K, digest;
      u32bit last_index;
   };

}

#endif
