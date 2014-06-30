/*************************************************
* Triple EDE Mode Header File                    *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_TRIPLE_EDE_MODE_H__
#define OPENCL_TRIPLE_EDE_MODE_H__

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* Triple Key EDE Mode                            *
*************************************************/
template<typename B>
class Triple : public BlockCipher
   {
   public:
      static std::string name() { return "Triple<" + B::name() + ">"; }
      static const u32bit BLOCKSIZE = B::BLOCKSIZE,
                          KEYLENGTH = 3*B::KEYLENGTH;
      void encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
         { C1.encrypt(in, out); C2.decrypt(out); C3.encrypt(out); }
      void decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
         { C3.decrypt(in, out); C2.encrypt(out); C1.decrypt(out); }
      void encrypt(byte block[BLOCKSIZE]) const { encrypt(block, block); }
      void decrypt(byte block[BLOCKSIZE]) const { decrypt(block, block); }
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw() { C1.clear(); C2.clear(); C3.clear(); }
      Triple() : BlockCipher(name(), BLOCKSIZE, 3, KEYLENGTH, 3) {}
   private:
      B C1, C2, C3;
   };

/*************************************************
* Triple Key EDE Mode Key Setup                  *
*************************************************/
template<typename B>
void Triple<B>::set_key(const byte key[], u32bit length)
   throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   C1.set_key(key                   , length / 3);
   C2.set_key(key +     (length / 3), length / 3);
   C3.set_key(key + 2 * (length / 3), length / 3);
   }

}

#endif
