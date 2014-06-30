/*************************************************
* DES Header File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_DES_H__
#define OPENCL_DES_H__

#include <opencl/opencl.h>

namespace OpenCL {

class DES : public BlockCipher
   {
   public:
      static std::string name() { return "DES"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 8;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { round_key.clear(); }
      DES() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      static const u32bit SPBOX1[256], SPBOX2[256], SPBOX3[256], SPBOX4[256],
                          SPBOX5[256], SPBOX6[256], SPBOX7[256], SPBOX8[256];
      static const u64bit IPTAB1[256], IPTAB2[256], FPTAB1[256], FPTAB2[256];
      static void IP(u32bit&, u32bit&);
      static void FP(u32bit&, u32bit&);
      void raw_encrypt(u32bit&, u32bit&) const;
      void raw_decrypt(u32bit&, u32bit&) const;
      void round(u32bit&, u32bit, u32bit) const;
      SecureBuffer<u32bit, 32> round_key;
      friend class TripleDES;
   };

class TripleDES : public BlockCipher
   {
   public:
      static std::string name() { return "TripleDES"; }
      static const u32bit BLOCKSIZE = 8, KEYLENGTH = 24;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { des1.clear(); des2.clear(); des3.clear(); }
      TripleDES() : BlockCipher(name(), BLOCKSIZE, KEYLENGTH) {}
   private:
      DES des1, des2, des3;
   };

}

#endif
