/*************************************************
* TEA Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_TEA_H__
#define OPENCL_TEA_H__

#include <opencl/opencl.h>

namespace OpenCL {

class TEA : public BlockCipher
   {
   public:
      static std::string name() { return "TEA"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { K.clear(); }
      TEA() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const u32bit ROUNDS = 32, DELTA = 0x9E3779B9;
      SecureBuffer<u32bit, 4> K;
   };

}

#endif
