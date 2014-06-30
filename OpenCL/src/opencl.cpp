/*************************************************
* OpenCL Base Interface Source File              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* Symmetric Algorithm Constructor                *
*************************************************/
SymmetricAlgorithm::SymmetricAlgorithm(const std::string& n,
   u32bit key_min, u32bit key_max, u32bit key_mod) :
   Algorithm(n),
   max(key_max ? key_max : key_min),
   min(key_min),
   mod(key_mod) {}

/*************************************************
* Query if the keylength is valid                *
*************************************************/
bool SymmetricAlgorithm::valid_keylength(u32bit length) const
   {
   return (length >= min && length <= max && length % mod == 0);
   }

/*************************************************
* Block Cipher Constructor                       *
*************************************************/
BlockCipher::BlockCipher(const std::string& n, u32bit block,
   u32bit key_min, u32bit key_max, u32bit key_mod) :
   SymmetricAlgorithm(n, key_min, key_max, key_mod),
   BLOCKSIZE(block),
   KEYLENGTH(key_max ? key_max : key_min) {}

/*************************************************
* Stream Cipher Constructor                      *
*************************************************/
StreamCipher::StreamCipher(const std::string& n,
   u32bit key_min, u32bit key_max, u32bit key_mod) :
   SymmetricAlgorithm(n, key_min, key_max, key_mod),
   KEYLENGTH(key_max ? key_max : key_min) {}

/*************************************************
* Random Access Stream Cipher Constructor        *
*************************************************/
RandomAccessStreamCipher::RandomAccessStreamCipher(const std::string& n,
   u32bit key_min, u32bit key_max, u32bit key_mod) :
   StreamCipher(n, key_min, key_max, key_mod) {}

/*************************************************
* MAC Constructor                                *
*************************************************/
MessageAuthCode::MessageAuthCode(const std::string& n, u32bit mac_len,
   u32bit key_min, u32bit key_max, u32bit key_mod) :
   SymmetricAlgorithm(n, key_min, key_max, key_mod),
   MACLENGTH(mac_len),
   KEYLENGTH(key_max ? key_max : key_min) {}

/*************************************************
* Encrypt/decrypt with a StreamCipher            *
*************************************************/
void StreamCipher::encrypt(const byte in[], byte out[], u32bit len)
   {
   cipher(in, out, len);
   }

void StreamCipher::decrypt(const byte in[], byte out[], u32bit len)
   {
   cipher(in, out, len);
   }

void StreamCipher::encrypt(byte in[], u32bit len) { cipher(in, in, len); }

void StreamCipher::decrypt(byte in[], u32bit len) { cipher(in, in, len); }

/*************************************************
* Hashing                                        *
*************************************************/
void HashFunction::update(const byte in[], u32bit n) { update_hash(in, n); }

void HashFunction::update(const std::string& str)
   {
   update((const byte*)str.c_str(), str.size());
   }

void HashFunction::process(const byte in[], u32bit len, byte hash[])
   {
   update(in, len);
   final(hash);
   }

void HashFunction::process(const std::string& in, byte hash[])
   {
   update(in);
   final(hash);
   }

/*************************************************
* MACing                                         *
*************************************************/
void MessageAuthCode::update(const byte in[], u32bit n) { update_mac(in, n); }

void MessageAuthCode::update(const std::string& str)
   {
   update((const byte*)str.c_str(), str.size());
   }

void MessageAuthCode::process(const byte in[], u32bit len, byte mac[])
   {
   update(in, len);
   final(mac);
   }

void MessageAuthCode::process(const std::string& in, byte mac[])
   {
   update(in);
   final(mac);
   }

/*************************************************
* Randomize an array of bytes                    *
*************************************************/
void RandomNumberGenerator::randomize(byte out[], u32bit len)
   {
   for(u32bit j = 0; j != len; j++)
      out[j] ^= random();
   }

}
