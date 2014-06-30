/*************************************************
* Tiger Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_TIGER_H__
#define OPENCL_TIGER_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Tiger : public HashFunction
   {
   public:
      static std::string name() { return "Tiger"; }
      static const u32bit BLOCKSIZE = 64, HASHLENGTH = 24;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      Tiger() : HashFunction(name(), HASHLENGTH) { clear(); }
   private:
      static const u64bit SBOX1[256], SBOX2[256], SBOX3[256], SBOX4[256];
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void round(u64bit&, u64bit&, u64bit&, u64bit, u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u64bit, 8> X;
      SecureBuffer<u64bit, 3> digest;
      u64bit count, position;
   };

}

#endif
