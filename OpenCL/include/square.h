/*************************************************
* Square Header File                             *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SQUARE_H__
#define OPENCL_SQUARE_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Square : public BlockCipher
   {
   public:
      static std::string name() { return "Square"; }
      static const u32bit BLOCKSIZE = 16, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      Square() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const byte SE[256], SD[256], Log[256], ALog[255];
      static const u32bit TE0[256], TE1[256], TE2[256], TE3[256],
                          TD0[256], TD1[256], TD2[256], TD3[256];
      static void transform(u32bit[4]);
      static byte mul(byte, byte);
      SecureBuffer<u32bit, 28> EK, DK;
      SecureBuffer<byte, 32> ME, MD;
   };

}

#endif
