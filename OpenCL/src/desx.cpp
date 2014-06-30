/*************************************************
* DESX Source File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/desx.h>

namespace OpenCL {

/*************************************************
* DESX Encryption                                *
*************************************************/
void DESX::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   xor_buf(out, in, K1, BLOCKSIZE);
   des.encrypt(out);
   xor_buf(out, K2, BLOCKSIZE);
   }

/*************************************************
* DESX Decryption                                *
*************************************************/
void DESX::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   xor_buf(out, in, K2, BLOCKSIZE);
   des.decrypt(out);
   xor_buf(out, K1, BLOCKSIZE);
   }

/*************************************************
* DESX Key Schedule                              *
*************************************************/
void DESX::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   K1.copy(key, BLOCKSIZE);
   des.set_key(key + BLOCKSIZE, BLOCKSIZE);
   K2.copy(key + 2 * BLOCKSIZE, BLOCKSIZE);
   }

}

