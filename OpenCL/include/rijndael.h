/*************************************************
* Rijndael Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_RIJNDAEL_H__
#define OPENCL_RIJNDAEL_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Rijndael : public BlockCipher
   {
   public:
      static std::string name() { return "Rijndael"; }
      static const u32bit BLOCKSIZE = 16, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); DK.clear(); rounds = 2; }
      Rijndael() : BlockCipher(name(), BLOCKSIZE, 16, 32, 8) {}
   public:
      static const byte SE[256], SD[256];
      static const u32bit TE0[256], TE1[256], TE2[256], TE3[256],
                          TD0[256], TD1[256], TD2[256], TD3[256];
      static u32bit S(u32bit);
      SecureBuffer<u32bit, 64> EK, DK;
      u32bit rounds;
   };

typedef Rijndael AES;

}

#endif
