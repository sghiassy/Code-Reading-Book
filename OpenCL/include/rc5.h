/*************************************************
* RC5 Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_RC5_H__
#define OPENCL_RC5_H__

#include <opencl/opencl.h>

namespace OpenCL {

class RC5 : public BlockCipher
   {
   public:
      static std::string name() { return "RC5"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 32, DEFAULT_ROUNDS = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { S.clear(); }
      RC5(u32bit r = DEFAULT_ROUNDS);
   private:
      const u32bit ROUNDS;
      SecureVector<u32bit> S;
   };

struct RC5_12 : public RC5
   {
   RC5_12() : RC5(12) {}
   };

struct RC5_16 : public RC5
   {
   RC5_16() : RC5(16) {}
   };

}

#endif
