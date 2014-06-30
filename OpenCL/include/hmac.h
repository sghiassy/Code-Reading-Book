/*************************************************
* HMAC Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_HMAC_H__
#define OPENCL_HMAC_H__

#include <opencl/opencl.h>

namespace OpenCL {

template<typename H>
class HMAC : public MessageAuthCode
   {
   public:
      static std::string name() { return "HMAC<" + H::name() + ">"; }
      static const u32bit MACLENGTH = H::HASHLENGTH, KEYLENGTH = H::BLOCKSIZE;
      void final(byte[MACLENGTH]);
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { hash.clear(); i_key.clear(); o_key.clear(); }
      HMAC() : MessageAuthCode(name(), MACLENGTH, 1, KEYLENGTH) {}
   private:
      static const u32bit BLOCKSIZE = H::BLOCKSIZE;
      void update_mac(const byte[], u32bit);
      H hash;
      SecureBuffer<byte, BLOCKSIZE> i_key, o_key;
   };

/*************************************************
* Update an HMAC Calculation                     *
*************************************************/
template<typename H>
void HMAC<H>::update_mac(const byte input[], u32bit length)
   {
   hash.update(input, length);
   }

/*************************************************
* Finalize a HMAC Calculation                    *
*************************************************/
template<typename H>
void HMAC<H>::final(byte mac[MACLENGTH])
   {
   hash.final(mac);
   hash.update(o_key, BLOCKSIZE);
   hash.update(mac, MACLENGTH);
   hash.final(mac);
   hash.update(i_key, BLOCKSIZE);
   }

/*************************************************
* HMAC Key Setup                                 *
*************************************************/
template<typename H>
void HMAC<H>::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   clear();
   i_key.set(0x36);
   o_key.set(0x5C);
   xor_buf(i_key, key, length);
   xor_buf(o_key, key, length);
   hash.update(i_key, BLOCKSIZE);
   }

}

#endif
