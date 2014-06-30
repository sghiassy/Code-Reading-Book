/*************************************************
* CAST5 Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CAST5_H__
#define OPENCL_CAST5_H__

#include <opencl/opencl.h>

namespace OpenCL {

class CAST5 : public BlockCipher
   {
   public:
      static std::string name() { return "CAST5"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { MK.clear(); RK.clear(); }
      CAST5() : BlockCipher(name(), BLOCKSIZE, 11, 16) {}
   private:
      static const u32bit S5[256], S6[256], S7[256], S8[256];
      void round1(u32bit&, u32bit, u32bit) const;
      void round2(u32bit&, u32bit, u32bit) const;
      void round3(u32bit&, u32bit, u32bit) const;
      static void key_schedule(SecureBuffer<u32bit, 16>&,
                               SecureBuffer<u32bit, 4>&);
      SecureBuffer<u32bit, 16> MK, RK;
   };

extern const u32bit CAST_SBOX1[256];
extern const u32bit CAST_SBOX2[256];
extern const u32bit CAST_SBOX3[256];
extern const u32bit CAST_SBOX4[256];

}

#endif
