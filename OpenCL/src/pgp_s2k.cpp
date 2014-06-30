/*************************************************
* OpenPGP S2K Source File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/pgp_s2k.h>

namespace OpenCL {

/*************************************************
* Derive a key using the OpenPGP S2K algorithm   *
*************************************************/
SymmetricKey OpenPGP_S2K::derive(const std::string& pw, u32bit keylen) const
   {
   static const byte ZERO_BYTE[1] = { 0 };
   SecureVector<byte> key(keylen), hash_buf(hash->HASHLENGTH);

   u32bit pass = 0, generated = 0, to_hash = iterations();
   if(to_hash < pw.size() + salt.size())
      to_hash = pw.size() + salt.size();

   hash->clear();
   while(keylen > generated)
      {
      for(u32bit j = 0; j != pass; j++)
         hash->update(ZERO_BYTE, 1);

      u32bit left = to_hash;
      while(left >= pw.size() + salt.size())
         {
         hash->update(salt, salt.size());
         hash->update(pw);
         left -= pw.size() + salt.size();
         }
      if(left <= salt.size())
         hash->update(salt, left);
      else
         {
         hash->update(salt, salt.size());
         left -= salt.size();
         hash->update((const byte*)pw.c_str(), left);
         }

      hash->final(hash_buf);
      key.copy(generated, hash_buf, hash->HASHLENGTH);
      generated += hash->HASHLENGTH;
      pass++;
      }
   return SymmetricKey(key, key.size());
   }

/*************************************************
* OpenPGP S2K Constructor                        *
*************************************************/
OpenPGP_S2K::OpenPGP_S2K(HashFunction* h, u32bit i) : S2K(true)
   {
   hash = h;
   set_iterations(i);
   }

}
