/*************************************************
* CBC Mode Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CBC_H__
#define OPENCL_CBC_H__

#include <opencl/modebase.h>
#include <opencl/mode_pad.h>

namespace OpenCL {

/*************************************************
* CBC Encryption                                 *
*************************************************/
template<typename B, typename P>
class CBC_Encryption : public CipherMode<B>
   {
   public:
      static std::string name() { return "CBC_Encryption<" + B::name() + ">"; }
      void write(const byte[], u32bit);
      void final();
      CBC_Encryption(const BlockCipherKey&, const BlockCipherModeIV&);
   };

/*************************************************
* CBC Decryption                                 *
*************************************************/
template<typename B, typename P>
class CBC_Decryption : public CipherMode<B>
   {
   public:
      static std::string name() { return "CBC_Decryption<" + B::name() + ">"; }
      void write(const byte[], u32bit);
      void final();
      CBC_Decryption(const BlockCipherKey&, const BlockCipherModeIV&);
   private:
      SecureBuffer<byte, B::BLOCKSIZE> temp;
   };

}

#endif

#ifndef OPENCL_CBC_ICC__
#define OPENCL_CBC_ICC__
#include <opencl/cbc.icc>
#endif
