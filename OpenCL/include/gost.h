/*************************************************
* GOST Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_GOST_H__
#define OPENCL_GOST_H__

#include <opencl/opencl.h>

namespace OpenCL {

class GOST : public BlockCipher
   {
   public:
      static std::string name() { return "GOST"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); }
      GOST() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const u32bit SBOX1[256], SBOX2[256], SBOX3[256], SBOX4[256];
      static void round(u32bit&, u32bit);
      SecureBuffer<u32bit, 8> EK;
   };

}

#endif
