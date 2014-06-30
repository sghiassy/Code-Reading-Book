/*************************************************
* Encoder Source File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/encoder.h>

namespace OpenCL {

/*************************************************
* Base64 Encoder                                 *
*************************************************/
void Base64::encode(const byte in[INPUTSIZE], byte out[OUTPUTSIZE])
   {
   static const byte BIN_TO_BASE64[] =
      "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
   out[0] = BIN_TO_BASE64[((in[0] & 0xFC) >> 2)];
   out[1] = BIN_TO_BASE64[((in[0] & 0x03) << 4) | (in[1] >> 4)];
   out[2] = BIN_TO_BASE64[((in[1] & 0x0F) << 2) | (in[2] >> 6)];
   out[3] = BIN_TO_BASE64[((in[2] & 0x3F)     )];
   }

/*************************************************
* Base64 Decoder                                 *
*************************************************/
void Base64::decode(const byte in[OUTPUTSIZE], byte out[INPUTSIZE])
   {
   out[0] = (byte)((BASE64_TO_BIN[in[0]] << 2) | (BASE64_TO_BIN[in[1]] >> 4));
   out[1] = (byte)((BASE64_TO_BIN[in[1]] << 4) | (BASE64_TO_BIN[in[2]] >> 2));
   out[2] = (byte)((BASE64_TO_BIN[in[2]] << 6) | (BASE64_TO_BIN[in[3]]));
   }

/*************************************************
* Hex Encoder                                    *
*************************************************/
void Hex::encode(const byte in[INPUTSIZE], byte out[OUTPUTSIZE])
   {
   static const byte BIN_TO_HEX[] = "0123456789ABCDEF";
   out[0] = BIN_TO_HEX[((in[0] >> 4) & 0x0F)];
   out[1] = BIN_TO_HEX[((in[0]     ) & 0x0F)];
   }

/*************************************************
* Hex Decoder                                    *
*************************************************/
void Hex::decode(const byte in[OUTPUTSIZE], byte out[INPUTSIZE])
   {
   out[0] = (byte)((HEX_TO_BIN[in[0]] << 4) | HEX_TO_BIN[in[1]]);
   }

}
