/*************************************************
* CBC Padding Methods Header File                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_PADDING_H__
#define OPENCL_PADDING_H__

#include <opencl/opencl.h>

namespace OpenCL {

class PKCSPadding
   {
   public:
      static void pad(byte[], u32bit, u32bit);
      static u32bit unpad(const byte[], u32bit);
      static u32bit pad_bytes(u32bit bs, u32bit pos) { return (bs - pos); }
      static bool valid_blocksize(u32bit);
      static std::string name() { return "PKCS7"; }
   };

class OneAndZeros
   {
   public:
      static void pad(byte[], u32bit, u32bit);
      static u32bit unpad(const byte[], u32bit);
      static u32bit pad_bytes(u32bit bs, u32bit pos) { return (bs - pos); }
      static bool valid_blocksize(u32bit);
      static std::string name() { return "OneAndZeros"; }
   };

class NoPadding
   {
   public:
      static void pad(byte[], u32bit, u32bit) { return; }
      static u32bit unpad(const byte[], u32bit size) { return size; }
      static u32bit pad_bytes(u32bit, u32bit) { return 0; }
      static bool valid_blocksize(u32bit) { return true; }
      static std::string name() { return "Null"; }
   };

}

#endif
