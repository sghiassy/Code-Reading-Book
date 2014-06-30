/*************************************************
* MD2 Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_MD2_H__
#define OPENCL_MD2_H__

#include <opencl/opencl.h>

namespace OpenCL {

class MD2 : public HashFunction
   {
   public:
      static std::string name() { return "MD2"; }
      static const u32bit BLOCKSIZE = 16, HASHLENGTH = 16;
      void final(byte[HASHLENGTH]);
      void clear() throw();
      MD2() : HashFunction(name(), HASHLENGTH) { position = 0; }
   private:
      static const byte SBOX[256];
      void update_hash(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      SecureBuffer<byte, 48> X;
      SecureBuffer<byte, BLOCKSIZE> buffer, checksum;
      u64bit position;
   };

}

#endif
