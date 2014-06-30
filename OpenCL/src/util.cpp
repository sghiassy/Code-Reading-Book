/*************************************************
* Utility Functions Source File                  *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/util.h>

namespace OpenCL {

/*************************************************
* XOR arrays together                            *
*************************************************/
void xor_buf(byte data[], const byte mask[], u32bit length)
   {
   while(length >= 8)
      {
      data[0] ^= mask[0]; data[1] ^= mask[1];
      data[2] ^= mask[2]; data[3] ^= mask[3];
      data[4] ^= mask[4]; data[5] ^= mask[5];
      data[6] ^= mask[6]; data[7] ^= mask[7];
      data += 8; mask += 8; length -= 8;
      }
   for(u32bit j = 0; j != length; j++)
      data[j] ^= mask[j];
   }

void xor_buf(byte out[], const byte in[], const byte mask[], u32bit length)
   {
   while(length >= 8)
      {
      out[0] = in[0] ^ mask[0]; out[1] = in[1] ^ mask[1];
      out[2] = in[2] ^ mask[2]; out[3] = in[3] ^ mask[3];
      out[4] = in[4] ^ mask[4]; out[5] = in[5] ^ mask[5];
      out[6] = in[6] ^ mask[6]; out[7] = in[7] ^ mask[7];
      in += 8; out += 8; mask += 8; length -= 8;
      }
   for(u32bit j = 0; j != length; j++)
      out[j] = in[j] ^ mask[j];
   }

/*************************************************
* Byte Reversal Functions                        *
*************************************************/
u16bit reverse_bytes(u16bit input)
   {
   return rotate_left(input, 8);
   }

u32bit reverse_bytes(u32bit input)
   {
   input = ((input & 0xFF00FF00) >> 8) | ((input & 0x00FF00FF) << 8);
   return rotate_left(input, 16);
   }

u64bit reverse_bytes(u64bit input)
   {
   input = ((input & 0xFF00FF00FF00FF00) >>  8) |
           ((input & 0x00FF00FF00FF00FF) <<  8);
   input = ((input & 0xFFFF0000FFFF0000) >> 16) |
           ((input & 0x0000FFFF0000FFFF) << 16);
   return rotate_left(input, 32);
   }

/*************************************************
* Convert an integer into a string               *
*************************************************/
std::string to_string(u32bit n)
   {
   if(n)
      {
      std::string lenstr;
      while(n > 0)
         {
         lenstr = (char)('0' + n % 10) + lenstr;
         n /= 10;
         }
      return lenstr;
      }
   else
      return "zero";
   }

}
