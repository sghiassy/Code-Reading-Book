/*************************************************
* RIPE-MD128 Header File                         *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_RMD128_H__
#define OPENCL_RMD128_H__

#include <opencl/opencl.h>

namespace OpenCL {

class RIPEMD128 : public HashFunction
   {
   public:
      static std::string name() { return "RIPE-MD128"; }
      static const u32bit BLOCKSIZE = 64, HASHLENGTH = 16;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      RIPEMD128() : HashFunction(name(), HASHLENGTH) { clear(); }
   private:
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void F1(u32bit&, u32bit, u32bit, u32bit, u32bit, byte, u32bit);
      static void F2(u32bit&, u32bit, u32bit, u32bit, u32bit, byte, u32bit);
      static void F3(u32bit&, u32bit, u32bit, u32bit, u32bit, byte, u32bit);
      static void F4(u32bit&, u32bit, u32bit, u32bit, u32bit, byte, u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u32bit, 16> M;
      SecureBuffer<u32bit, 4> digest;
      u64bit count, position;
   };

}

#endif
