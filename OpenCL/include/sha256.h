/*************************************************
* SHA2-256 Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SHA2_256_H__
#define OPENCL_SHA2_256_H__

#include <opencl/opencl.h>

namespace OpenCL {

class SHA2_256 : public HashFunction
   {
   public:
      static std::string name() { return "SHA2-256"; }
      static const u32bit BLOCKSIZE = 64, HASHLENGTH = 32;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      SHA2_256() : HashFunction(name(), HASHLENGTH) { clear(); }
   private:
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void F1(u32bit, u32bit, u32bit, u32bit&, u32bit,
                     u32bit, u32bit, u32bit&, u32bit, u32bit);
      static u32bit rho(u32bit, u32bit, u32bit, u32bit);
      static u32bit sigma(u32bit, u32bit, u32bit, u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u32bit, 64> W;
      SecureBuffer<u32bit, 8> digest;
      u64bit count, position;
   };

}

#endif
