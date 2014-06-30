/*************************************************
* Blowfish Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_BLOWFISH_H__
#define OPENCL_BLOWFISH_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Blowfish : public BlockCipher
   {
   public:
      static std::string name() { return "Blowfish"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      Blowfish() : BlockCipher(name(), BLOCKSIZE, 1, KEYLENGTH) {}
   private:
      static const u32bit PBOX[18], SBOX1[256], SBOX2[256],
                                    SBOX3[256], SBOX4[256];
      void generate_sbox(u32bit[], u32bit, u32bit&, u32bit&) const;
      void round(u32bit&, u32bit&, u32bit) const;
      SecureBuffer<u32bit, 256> Sbox1, Sbox2, Sbox3, Sbox4;
      SecureBuffer<u32bit, 18> Pbox;
   };

}

#endif
