/*************************************************
* SAFER-SK128 Header File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SAFER_SK128_H__
#define OPENCL_SAFER_SK128_H__

#include <opencl/opencl.h>

namespace OpenCL {

class SAFER_SK128 : public BlockCipher
   {
   public:
      static std::string name() { return "SAFER-SK128"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      static const u32bit DEFAULT_ROUNDS = 10, MAX_ROUNDS = 13;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      SAFER_SK128(u32bit = DEFAULT_ROUNDS);
   private:
      static const byte EXP[256], LOG[512], BIAS[208], KEY_INDEX[208];
      const u32bit ROUNDS;
      SecureBuffer<byte, 8> MK;
      SecureBuffer<byte, 256> SE[104], SD[104];
   };

}

#endif
