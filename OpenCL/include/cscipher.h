/*************************************************
* CS-Cipher Header File                          *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CS_CIPHER_H__
#define OPENCL_CS_CIPHER_H__

#include <opencl/opencl.h>

namespace OpenCL {

class CS_Cipher : public BlockCipher
   {
   public:
      static std::string name() { return "CS-Cipher"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); }
      CS_Cipher() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const byte P[256], TE1[256], TD1[256], TD2[256], ROT[256];
      SecureBuffer<byte, 200> EK;
   };

}

#endif
