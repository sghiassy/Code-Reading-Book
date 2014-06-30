/*************************************************
* RC6 Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_RC6_H__
#define OPENCL_RC6_H__

#include <opencl/opencl.h>

namespace OpenCL {

class RC6 : public BlockCipher
   {
   public:
      static std::string name() { return "RC5"; }
      static const u32bit BLOCKSIZE = 16, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { S.clear(); }
      RC6() : BlockCipher(name(), BLOCKSIZE, 1, KEYLENGTH) {}
   private:
      static const u32bit TABLESIZE = 44;
      void encrypt_round(u32bit&, u32bit, u32bit&, u32bit, u32bit) const;
      void decrypt_round(u32bit&, u32bit, u32bit&, u32bit, u32bit) const;
      SecureBuffer<u32bit, TABLESIZE> S;
   };

}

#endif
