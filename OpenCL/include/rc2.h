/*************************************************
* RC2 Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_RC2_H__
#define OPENCL_RC2_H__

#include <opencl/opencl.h>

namespace OpenCL {

class RC2 : public BlockCipher
   {
   public:
      static std::string name() { return "RC2"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { K.clear(); }
      RC2() : BlockCipher(name(), BLOCKSIZE, 1, KEYLENGTH) {}
   private:
      static const byte TABLE[256];
      void mash(u16bit&, u16bit&, u16bit&, u16bit&) const;
      void rmash(u16bit&, u16bit&, u16bit&, u16bit&) const;
      void mix(u16bit&, u16bit&, u16bit&, u16bit&, u32bit) const;
      void rmix(u16bit&, u16bit&, u16bit&, u16bit&, u32bit) const;
      SecureBuffer<u16bit, 64> K;
   };

}

#endif
