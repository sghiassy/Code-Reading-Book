/*************************************************
* ThreeWay Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_THREEWAY_H__
#define OPENCL_THREEWAY_H__

#include <opencl/opencl.h>

namespace OpenCL {

class ThreeWay : public BlockCipher
   {
   public:
      static std::string name() { return "ThreeWay"; }
      static const u32bit BLOCKSIZE = 12, KEYLENGTH = 12;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { EK.clear(); DK.clear(); }
      ThreeWay() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static void mu(u32bit&, u32bit&, u32bit&);
      static void theta(u32bit&, u32bit&, u32bit&);
      static void pi_gamma_pi(u32bit&, u32bit&, u32bit&);
      SecureBuffer<u32bit, 36> EK, DK;
   };

}

#endif
