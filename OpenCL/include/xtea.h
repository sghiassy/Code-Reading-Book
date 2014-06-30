/*************************************************
* XTEA Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_XTEA_H__
#define OPENCL_XTEA_H__

#include <opencl/opencl.h>

namespace OpenCL {

class XTEA : public BlockCipher
   {
   public:
      static std::string name() { return "XTEA"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); }
      XTEA() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const u32bit ROUNDS = 32;
      static const u32bit DELTAS[2*ROUNDS];
      static const byte KEY_INDEX[2*ROUNDS];
      SecureBuffer<u32bit, 2*ROUNDS> EK;
   };

}

#endif
