/*************************************************
* Lion Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_LION_H__
#define OPENCL_LION_H__

#include <opencl/opencl.h>

namespace OpenCL {

template<typename H, typename S, u32bit BS = 1024>
class Lion : public BlockCipher
   {
   public:
      static std::string name()
         { return "Lion<" + H::name() + "," + S::name() + ">"; }
      static const u32bit BLOCKSIZE = BS, KEYLENGTH = 2*H::HASHLENGTH;
      void encrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void decrypt(const byte[BLOCKSIZE], byte[BLOCKSIZE]) const;
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      Lion();
   private:
      static const u32bit LEFT_SIZE  = KEYLENGTH / 2,
                          RIGHT_SIZE = BS - LEFT_SIZE;
      mutable H hash;
      mutable S cipher;
      SecureBuffer<byte, LEFT_SIZE> key1, key2;
   };

/*************************************************
* Lion Encryption                                *
*************************************************/
template<typename H, typename S, u32bit BS>
void Lion<H,S,BS>::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   SecureBuffer<byte, LEFT_SIZE> buffer;

   xor_buf(buffer, in, key1, LEFT_SIZE);
   cipher.set_key(buffer, LEFT_SIZE);
   cipher.encrypt(in + LEFT_SIZE, out + LEFT_SIZE, RIGHT_SIZE);

   hash.process(out + LEFT_SIZE, RIGHT_SIZE, buffer);
   xor_buf(out, in, buffer, LEFT_SIZE);

   xor_buf(buffer, out, key2, LEFT_SIZE);
   cipher.set_key(buffer, LEFT_SIZE);
   cipher.encrypt(out + LEFT_SIZE, RIGHT_SIZE);
   }

/*************************************************
* Lion Decryption                                *
*************************************************/
template<typename H, typename S, u32bit BS>
void Lion<H,S,BS>::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   SecureBuffer<byte, LEFT_SIZE> buffer;

   xor_buf(buffer, in, key2, LEFT_SIZE);
   cipher.set_key(buffer, LEFT_SIZE);
   cipher.encrypt(in + LEFT_SIZE, out + LEFT_SIZE, RIGHT_SIZE);

   hash.process(out + LEFT_SIZE, RIGHT_SIZE, buffer);
   xor_buf(out, in, buffer, LEFT_SIZE);

   xor_buf(buffer, out, key1, LEFT_SIZE);
   cipher.set_key(buffer, LEFT_SIZE);
   cipher.encrypt(out + LEFT_SIZE, RIGHT_SIZE);
   }

/*************************************************
* Lion Key Schedule                              *
*************************************************/
template<typename H, typename S, u32bit BS>
void Lion<H,S,BS>::set_key(const byte key[], u32bit length)
   throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   clear();
   key1.copy(key,              length / 2);
   key2.copy(key + length / 2, length / 2);
   }

/*************************************************
* Lion Constructor                               *
*************************************************/
template<typename H, typename S, u32bit BS>
Lion<H,S,BS>::Lion() : BlockCipher(name(), BLOCKSIZE, 2, KEYLENGTH, 2)
   {
   if(!cipher.valid_keylength(LEFT_SIZE))
      throw Exception(name() + ": This stream/hash combination is invalid");
   if(2*LEFT_SIZE + 1 > BLOCKSIZE)
      throw Invalid_Argument(name() + ": Chosen block size is too small");
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
template<typename H, typename S, u32bit BS>
void Lion<H,S,BS>::clear() throw()
   {
   hash.clear();
   cipher.clear();
   key1.clear();
   key2.clear();
   }

}

#endif
