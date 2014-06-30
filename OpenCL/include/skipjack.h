/*************************************************
* Skipjack Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SKIPJACK_H__
#define OPENCL_SKIPJACK_H__

#include <opencl/opencl.h>

namespace OpenCL {

class Skipjack : public BlockCipher
   {
   public:
      static std::string name() { return "Skipjack"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 10;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      Skipjack() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const byte F[256];
      void step_A(u16bit&, u16bit&, u32bit) const;
      void step_B(u16bit&, u16bit&, u32bit) const;
      void step_Ai(u16bit&, u16bit&, u32bit) const;
      void step_Bi(u16bit&, u16bit&, u32bit) const;
      SecureBuffer<byte, 256> FTABLE[10];
   };

}

#endif
