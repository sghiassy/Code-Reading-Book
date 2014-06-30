/*************************************************
* DESX Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_DESX_H__
#define OPENCL_DESX_H__

#include <opencl/des.h>

namespace OpenCL {

class DESX : public BlockCipher
   {
   public:
      static std::string name() { return "DESX"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 24;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { des.clear(); K1.clear(); K2.clear(); }
      DESX() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      SecureBuffer<byte, BLOCKSIZE> K1, K2;
      DES des;
   };

}

#endif
