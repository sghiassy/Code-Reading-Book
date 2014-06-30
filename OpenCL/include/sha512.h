/*************************************************
* SHA2-512 Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SHA2_512_H__
#define OPENCL_SHA2_512_H__

#include <opencl/opencl.h>

namespace OpenCL {

class SHA2_512 : public HashFunction
   {
   public:
      static std::string name() { return "SHA2-512"; }
      static const u32bit BLOCKSIZE = 128, HASHLENGTH = 64;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      SHA2_512() : HashFunction(name(), HASHLENGTH) { clear(); }
   private:
      static const u64bit MAGIC[80];
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void F1(u64bit, u64bit, u64bit, u64bit&, u64bit,
                     u64bit, u64bit, u64bit&, u64bit, u64bit);
      static u64bit rho(u64bit, u32bit, u32bit, u32bit);
      static u64bit sigma(u64bit, u32bit, u32bit, u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u64bit, 80> W;
      SecureBuffer<u64bit, 8> digest;
      u64bit count, position;
   };

}

#endif
