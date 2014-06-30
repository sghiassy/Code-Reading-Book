/*************************************************
* MD5 Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_MD5_H__
#define OPENCL_MD5_H__

#include <opencl/opencl.h>

namespace OpenCL {

class MD5 : public HashFunction
   {
   public:
      static std::string name() { return "MD5"; }
      static const u32bit BLOCKSIZE = 64, HASHLENGTH = 16;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      MD5() : HashFunction(name(), HASHLENGTH) { clear(); }
   private:
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      static void FF(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit);
      static void GG(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit);
      static void HH(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit);
      static void II(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit);
      SecureBuffer<byte, BLOCKSIZE> buffer;
      SecureBuffer<u32bit, 16> M;
      SecureBuffer<u32bit, 4> digest;
      u64bit count, position;
   };

}

#endif
