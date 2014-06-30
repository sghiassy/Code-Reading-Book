/*************************************************
* Encoder Header File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_ENCODER_H__
#define OPENCL_ENCODER_H__

#include <opencl/filters.h>

namespace OpenCL {

class Base64
   {
   public:
      static const u32bit INPUTSIZE = 3, OUTPUTSIZE = 4, CODE_BITS = 6;
      static void encode(const byte[INPUTSIZE], byte[OUTPUTSIZE]);
      static void decode(const byte[OUTPUTSIZE], byte[INPUTSIZE]);
      static bool is_valid(byte in) { return (BASE64_TO_BIN[in] != 0x80); }
   private:
      static const byte BASE64_TO_BIN[256];
   };

class Hex
   {
   public:
      static const u32bit INPUTSIZE = 1, OUTPUTSIZE = 2, CODE_BITS = 4;
      static void encode(byte in, byte out[OUTPUTSIZE]) { encode(&in, out); }
      static void decode(const byte in[OUTPUTSIZE], byte& out)
         { decode(in, &out); }

      static void encode(const byte[INPUTSIZE], byte[OUTPUTSIZE]);
      static void decode(const byte[OUTPUTSIZE], byte[INPUTSIZE]);
      static bool is_valid(byte in) { return (HEX_TO_BIN[in] != 0x80); }
   private:
      static const byte HEX_TO_BIN[256];
   };

typedef EncoderFilter<Base64> Base64Encoder;
typedef DecoderFilter<Base64> Base64Decoder;
typedef EncoderFilter<Hex>    HexEncoder;
typedef DecoderFilter<Hex>    HexDecoder;

}

#endif
