/*************************************************
* CFB Mode Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CFB_H__
#define OPENCL_CFB_H__

#include <opencl/modebase.h>

namespace OpenCL {

/*************************************************
* CFB Encryption                                 *
*************************************************/
template<typename B>
class CFB_Encryption : public CipherMode<B>
   {
   public:
      static std::string name() { return "CFB_Encryption<" + B::name() + ">"; }
      const u32bit FEEDBACKSIZE;
      void write(const byte[], u32bit);
      CFB_Encryption(const BlockCipherKey&, const BlockCipherModeIV&,
                     u32bit = BLOCKSIZE);
   private:
      void feedback();
   };

/*************************************************
* CFB Decryption                                 *
*************************************************/
template<typename B>
class CFB_Decryption : public CipherMode<B>
   {
   public:
      static std::string name() { return "CFB_Decryption<" + B::name() + ">"; }
      const u32bit FEEDBACKSIZE;
      void write(const byte[], u32bit);
      CFB_Decryption(const BlockCipherKey&, const BlockCipherModeIV&,
                     u32bit = BLOCKSIZE);
   private:
      void feedback();
   };

}

#endif

#ifndef OPENCL_CFB_ICC__
#define OPENCL_CFB_ICC__
#include <opencl/cfb.icc>
#endif
