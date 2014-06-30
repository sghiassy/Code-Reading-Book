/*************************************************
* MISTY1 Header File                             *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_MISTY1_H__
#define OPENCL_MISTY1_H__

#include <opencl/opencl.h>

namespace OpenCL {

class MISTY1 : public BlockCipher
   {
   public:
      static std::string name() { return "MISTY1"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 16;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); }
      MISTY1() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const byte S7[128], EK_ORDER[100], DK_ORDER[100];
      static const u16bit S9[512];
      void FL_E(u16bit&, u16bit&, u16bit&, u16bit&, u32bit) const;
      void FL_D(u16bit&, u16bit&, u16bit&, u16bit&, u32bit) const;
      void FO_E(u16bit,  u16bit,  u16bit&, u16bit&, u32bit) const;
      void FO_D(u16bit,  u16bit,  u16bit&, u16bit&, u32bit) const;
      static u16bit FI(u16bit, u16bit, u16bit);
      SecureBuffer<u16bit, 100> EK, DK;
   };

}

#endif
