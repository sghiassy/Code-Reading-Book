/*************************************************
* Randpool Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_RANDPOOL_H__
#define OPENCL_RANDPOOL_H__

#include <opencl/opencl.h>
#include <opencl/md5.h>

namespace OpenCL {

class Randpool : public RandomNumberGenerator
   {
   public:
      static std::string name() { return "Randpool"; }
      byte random();
      void randomize(byte[], u32bit);
      void add_entropy(const byte[], u32bit) throw();
      void add_entropy(EntropySource&, bool = true);
      void clear() throw();
      Randpool();
   private:
      typedef MD5 RANDPOOL_HASH;
      static const u32bit BUFFERSIZE = RANDPOOL_HASH::HASHLENGTH,
                          POOLSIZE = 32 * BUFFERSIZE;
      void generate(u64bit);
      void mix_pool();
      SecureBuffer<byte, POOLSIZE> pool;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      u32bit position;
   };

}

#endif
