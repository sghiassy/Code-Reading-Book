/*************************************************
* SHA1 Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SHA1_H__
#define OPENCL_SHA1_H__

#include <opencl/opencl.h>

namespace OpenCL {

class SHA1 : public HashFunction
   {
   public:
      static std::string name() { return "SHA1"; }
      static const u32bit BLOCKSIZE = 64, HASHLENGTH = 20;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      SHA1() : HashFunction(name(), HASHLENGTH) { clear(); }
   private:
      friend class Gamma;
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void F1(u32bit, u32bit&, u32bit, u32bit, u32bit&, u32bit);
      static void F2(u32bit, u32bit&, u32bit, u32bit, u32bit&, u32bit);
      static void F3(u32bit, u32bit&, u32bit, u32bit, u32bit&, u32bit);
      static void F4(u32bit, u32bit&, u32bit, u32bit, u32bit&, u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u32bit, 80> W;
      SecureBuffer<u32bit, 5> digest;
      u64bit count, position;
   };

}

#endif
