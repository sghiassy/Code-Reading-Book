/*************************************************
* MD5MAC Header File                             *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_MD5MAC_H__
#define OPENCL_MD5MAC_H__

#include <opencl/opencl.h>

namespace OpenCL {

class MD5MAC : public MessageAuthCode
   {
   public:
      static std::string name() { return "MD5MAC"; }
      static const u32bit MACLENGTH = 16, KEYLENGTH = 16;
      void final(byte[MACLENGTH]);
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      MD5MAC() : MessageAuthCode(name(), MACLENGTH, 1, KEYLENGTH) { clear(); }
   private:
      static const u32bit BLOCKSIZE = 64;
      void update_mac(const byte[], u32bit);
      void hash(const byte[BLOCKSIZE]);
      void FF(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit) const;
      void GG(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit) const;
      void HH(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit) const;
      void II(u32bit&, u32bit, u32bit, u32bit, u32bit, u32bit, u32bit) const;
      SecureBuffer<byte, BLOCKSIZE> buffer, K3;
      SecureBuffer<u32bit, 16> M;
      SecureBuffer<u32bit, 4> digest, K1, K2;
      u64bit count, position;
   };

}

#endif
