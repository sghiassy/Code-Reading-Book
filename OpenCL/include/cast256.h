/*************************************************
* CAST256 Header File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CAST256_H__
#define OPENCL_CAST256_H__

#include <opencl/opencl.h>

namespace OpenCL {

class CAST256 : public BlockCipher
   {
   public:
      static std::string name() { return "CAST256"; }
      static const u32bit BLOCKSIZE = 16, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { MK.clear(); RK.clear(); }
      CAST256() : BlockCipher(name(), BLOCKSIZE, 4, 32, 4) {}
   private:
      static const u32bit KEY_MASK[192];
      static const byte   KEY_ROT[192];
      static void round1(u32bit&, u32bit, u32bit, u32bit);
      static void round2(u32bit&, u32bit, u32bit, u32bit);
      static void round3(u32bit&, u32bit, u32bit, u32bit);
      SecureBuffer<u32bit, 48> MK;
      SecureBuffer<byte, 48> RK;
   };

extern const u32bit CAST_SBOX1[256];
extern const u32bit CAST_SBOX2[256];
extern const u32bit CAST_SBOX3[256];
extern const u32bit CAST_SBOX4[256];

}

#endif
