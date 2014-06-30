/*************************************************
* OFB Mode Header File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_OFB_H__
#define OPENCL_OFB_H__

#include <opencl/modebase.h>

namespace OpenCL {

/*************************************************
* OFB                                            *
*************************************************/
template<typename B>
class OFB : public CipherMode<B>
   {
   public:
      static std::string name() { return "OFB<" + B::name() + ">"; }
      void write(const byte[], u32bit);
      OFB(const BlockCipherKey&, const BlockCipherModeIV&);
   };

/*************************************************
* Counter                                        *
*************************************************/
template<typename B>
class Counter : public CipherMode<B>
   {
   public:
      static std::string name() { return "Counter<" + B::name() + ">"; }
      void write(const byte[], u32bit);
      Counter(const BlockCipherKey&, const BlockCipherModeIV&);
   };

}

#endif

#ifndef OPENCL_OFB_ICC__
#define OPENCL_OFB_ICC__
#include <opencl/ofb.icc>
#endif
