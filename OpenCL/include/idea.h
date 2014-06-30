/*************************************************
* IDEA Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_IDEA_H__
#define OPENCL_IDEA_H__

#include <opencl/opencl.h>

namespace OpenCL {

class IDEA : public BlockCipher
   {
   public:
      static std::string name() { return "IDEA"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); DK.clear(); }
      IDEA() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static void mul(u16bit&, u16bit);
      static u16bit mul_inv(u16bit);
      SecureBuffer<u16bit, 52> EK, DK;
   };

}

#endif
