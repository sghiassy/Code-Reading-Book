/*************************************************
* SymmetricKey Source File                       *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/symkey.h>
#include <opencl/encoder.h>

namespace OpenCL {

/*************************************************
* Create a SymmetricKey from a byte string       *
*************************************************/
void SymmetricKey::change(const byte in[], u32bit n)
   {
   key.create(n);
   key.copy(in, n);
   }

/*************************************************
* Create a SymmetricKey from RNG output          *
*************************************************/
void SymmetricKey::change(RandomNumberGenerator& rng, u32bit len)
   {
   key.create(len);
   rng.randomize(key, len);
   }

/*************************************************
* Create a SymmeticKey from a hex string         *
*************************************************/
void SymmetricKey::change(const std::string& hex)
   {
   if(hex.length() % 2 != 0)
      throw Invalid_Argument("SymmeticKey: hex string must encode full bytes");
   key.create(hex.length() / 2);
   for(u32bit j = 0; j != key.size(); j++)
      Hex::decode((const byte*)hex.c_str() + 2*j, key[j]);
   }

/*************************************************
* XOR Operation for SymmetricKeys                *
*************************************************/
SymmetricKey& SymmetricKey::operator^=(const SymmetricKey& k)
   {
   if(&k == this) { key.clear(); return (*this); }
   xor_buf(key.ptr(), k, std::min(length(), k.length()));
   return (*this);
   }

/*************************************************
* XOR Operation for SymmetricKeys                *
*************************************************/
SymmetricKey operator^(const SymmetricKey& k1, const SymmetricKey& k2)
   {
   SecureVector<byte> ret(std::max(k1.length(), k2.length()));
   ret.copy(k1, k1.length());
   xor_buf(ret, k2, k2.length());
   return SymmetricKey(ret);
   }

}
