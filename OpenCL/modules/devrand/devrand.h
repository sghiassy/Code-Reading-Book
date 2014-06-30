/*************************************************
* /dev/random Entropy Source Header File         *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_EXT_ENTROPY_SRC_DEVRANDOM_H__
#define OPENCL_EXT_ENTROPY_SRC_DEVRANDOM_H__

#include <string>
#include <opencl/opencl.h>

namespace OpenCL {

class DevRandom_EntropySource : public EntropySource
   {
   public:
      u32bit slow_poll(byte[], u32bit);
      u32bit fast_poll(byte buf[], u32bit len) { return slow_poll(buf, len); }
      DevRandom_EntropySource(const std::string& dev = "/dev/urandom")
         : device(dev) {}
   private:
      const std::string device;
   };

}

#endif
