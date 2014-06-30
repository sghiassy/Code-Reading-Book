/*************************************************
* Cipher Mode Base Header File                   *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_MODEBASE_H__
#define OPENCL_MODEBASE_H__

#include <string>
#include <opencl/filtbase.h>
#include <opencl/symkey.h>

namespace OpenCL {

typedef SymmetricKey BlockCipherModeIV;

/*************************************************
* Cipher Mode Base Class                         *
*************************************************/
template<typename B>
class CipherMode : public Filter
   {
   public:
      static const u32bit BLOCKSIZE = B::BLOCKSIZE, IVSIZE = B::BLOCKSIZE,
                          KEYLENGTH = B::KEYLENGTH;
      std::string name() const { return alg_name; }
      void reset_iv(const BlockCipherModeIV&);
      CipherMode(const std::string&, const BlockCipherKey&,
                                     const BlockCipherModeIV&);
   protected:
      std::string alg_name;
      B cipher;
      SecureBuffer<byte, BLOCKSIZE> buffer, state;
      u32bit position;
   };

/*************************************************
* Cipher Mode Constructor                        *
*************************************************/
template<typename B>
CipherMode<B>::CipherMode(const std::string& n, const BlockCipherKey& key,
                                                const BlockCipherModeIV& iv)
   {
   alg_name = n;
   cipher.set_key(key, key.length());
   reset_iv(iv);
   position = 0;
   }

/*************************************************
* Reset the IV                                   *
*************************************************/
template<typename B>
void CipherMode<B>::reset_iv(const BlockCipherModeIV& new_iv)
   {
   if(new_iv.length() != IVSIZE)
      throw Invalid_IVLength(name(), new_iv.length());
   state.copy(new_iv, new_iv.length());
   buffer.clear();
   position = 0;
   }

}

#endif
