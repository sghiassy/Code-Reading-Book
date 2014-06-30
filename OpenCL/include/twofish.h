/*************************************************
* Twofish Header File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_TWOFISH_H__
#define OPENCL_TWOFISH_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Twofish : public BlockCipher
   {
   public:
      static std::string name() { return "Twofish"; }
      static const u32bit BLOCKSIZE = 16, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      Twofish() : BlockCipher(name(), BLOCKSIZE,
                              KEYLENGTH / 2, KEYLENGTH, 8) {}
   private:
      static const u32bit MDS0[256], MDS1[256], MDS2[256], MDS3[256];
      static const byte Q0[256], Q1[256], RS[32];
      static const byte EXP_TO_POLY[255], POLY_TO_EXP[255];
      static void rs_mul(byte[4], byte, byte, byte, byte, byte);
      void encrypt_round(u32bit, u32bit, u32bit&, u32bit&, u32bit) const;
      void decrypt_round(u32bit, u32bit, u32bit&, u32bit&, u32bit) const;
      SecureBuffer<u32bit, 256> SBox0, SBox1, SBox2, SBox3;
      SecureBuffer<u32bit, 40> round_key;
   };

}

#endif
