/*************************************************
* Serpent Header File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SERPENT_H__
#define OPENCL_SERPENT_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Serpent : public BlockCipher
   {
   public:
      static std::string name() { return "Serpent"; }
      static const u32bit BLOCKSIZE = 16, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { round_key.clear(); }
      Serpent() : BlockCipher(name(), BLOCKSIZE,
                              KEYLENGTH / 2, KEYLENGTH, 8) {}
   private:
      static void SBoxE1(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE2(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE3(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE4(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE5(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE6(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE7(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxE8(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD1(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD2(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD3(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD4(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD5(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD6(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD7(u32bit&, u32bit&, u32bit&, u32bit&);
      static void SBoxD8(u32bit&, u32bit&, u32bit&, u32bit&);
      static void transform(u32bit&, u32bit&, u32bit&, u32bit&);
      static void i_transform(u32bit&, u32bit&, u32bit&, u32bit&);
      void key_xor(u32bit, u32bit&, u32bit&, u32bit&, u32bit&) const;
      SecureBuffer<u32bit, 132> round_key;
   };

}

#endif
