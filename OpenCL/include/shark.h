/*************************************************
* SHARK Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SHARK_H__
#define OPENCL_SHARK_H__

#include <opencl/opencl.h>

namespace OpenCL {

class SHARK : public BlockCipher
   {
   public:
      static std::string name() { return "SHARK"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); DK.clear(); }
      SHARK() : BlockCipher(name(), BLOCKSIZE, 1, KEYLENGTH) {}
   private:
      static const byte SE[256], SD[256], Log[256], ALog[255];
      static const u64bit TE0[256], TE1[256], TE2[256], TE3[256],
                          TE4[256], TE5[256], TE6[256], TE7[256],
                          TD0[256], TD1[256], TD2[256], TD3[256],
                          TD4[256], TD5[256], TD6[256], TD7[256];
      static u64bit transform(u64bit);
      static byte mul(byte, byte);
      SecureBuffer<u64bit, 7> EK, DK;
   };

}

#endif
