/*************************************************
* Luby-Rackoff Header File                       *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_LUBY_RACKOFF_H__
#define OPENCL_LUBY_RACKOFF_H__

#include <opencl/opencl.h>

namespace OpenCL {

template<typename H>
class LubyRackoff : public BlockCipher
   {
   public:
      static std::string name() { return "LubyRackoff<" + H::name() + ">"; }
      static const u32bit BLOCKSIZE = 2*H::HASHLENGTH, KEYLENGTH = 32;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { K1.clear(); K2.clear(); }
      LubyRackoff() : BlockCipher(name(), BLOCKSIZE, 2, KEYLENGTH, 2) {}
   private:
      static const u32bit HASHLENGTH = H::HASHLENGTH;
      mutable H hash;
      SecureBuffer<byte, KEYLENGTH/2> K1, K2;
      u32bit keylength;
   };

/*************************************************
* Luby-Rackoff Encryption                        *
*************************************************/
template<typename H>
void LubyRackoff<H>::encrypt(const byte in[BLOCKSIZE],
                                   byte out[BLOCKSIZE]) const
   {
   SecureBuffer<byte, HASHLENGTH> buffer;
   hash.update(K1, keylength);
   hash.update(in, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out + BLOCKSIZE/2, in + BLOCKSIZE/2, buffer, BLOCKSIZE/2);

   hash.update(K2, keylength);
   hash.update(in + BLOCKSIZE/2, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out, in, buffer, BLOCKSIZE/2);

   hash.update(K1, keylength);
   hash.update(out, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out + BLOCKSIZE/2, buffer, BLOCKSIZE/2);

   hash.update(K2, keylength);
   hash.update(out + BLOCKSIZE/2, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out, buffer, BLOCKSIZE/2);
   }

/*************************************************
* Luby-Rackoff Decryption                        *
*************************************************/
template<typename H>
void LubyRackoff<H>::decrypt(const byte in[BLOCKSIZE],
                                   byte out[BLOCKSIZE]) const
   {
   SecureBuffer<byte, HASHLENGTH> buffer;
   hash.update(K2, keylength);
   hash.update(in + BLOCKSIZE/2, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out, in, buffer, BLOCKSIZE/2);

   hash.update(K1, keylength);
   hash.update(in, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out + BLOCKSIZE/2, in + BLOCKSIZE/2, buffer, BLOCKSIZE/2);

   hash.update(K2, keylength);
   hash.update(out + BLOCKSIZE/2, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out, buffer, BLOCKSIZE/2);

   hash.update(K1, keylength);
   hash.update(out, BLOCKSIZE/2);
   hash.final(buffer);
   xor_buf(out + BLOCKSIZE/2, buffer, BLOCKSIZE/2);
   }

/*************************************************
* Luby-Rackoff Key Schedule                      *
*************************************************/
template<typename H>
void LubyRackoff<H>::set_key(const byte key[], u32bit length)
   throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   K1.copy(key, length / 2);
   K2.copy(key + length / 2, length / 2);
   keylength = length / 2;
   }

}

#endif
