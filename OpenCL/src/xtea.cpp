/*************************************************
* XTEA Source File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/xtea.h>

namespace OpenCL {

/*************************************************
* XTEA Encryption                                *
*************************************************/
void XTEA::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit left  = make_u32bit(in[0], in[1], in[2], in[3]),
          right = make_u32bit(in[4], in[5], in[6], in[7]);
   for(u32bit j = 0; j != ROUNDS; j++)
      {
      left  += (((right << 4) ^ (right >> 5)) + right) ^ EK[2*j];
      right += (((left  << 4) ^ (left  >> 5)) +  left) ^ EK[2*j+1];
      }
   out[0] = get_byte(0, left);  out[1] = get_byte(1, left);
   out[2] = get_byte(2, left);  out[3] = get_byte(3, left);
   out[4] = get_byte(0, right); out[5] = get_byte(1, right);
   out[6] = get_byte(2, right); out[7] = get_byte(3, right);
   }

/*************************************************
* XTEA Decryption                                *
*************************************************/
void XTEA::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit left  = make_u32bit(in[0], in[1], in[2], in[3]),
          right = make_u32bit(in[4], in[5], in[6], in[7]);
   for(u32bit j = ROUNDS; j > 0; j--)
      {
      right -= (((left  << 4) ^ (left  >> 5)) +  left) ^ EK[2*j - 1];
      left  -= (((right << 4) ^ (right >> 5)) + right) ^ EK[2*j - 2];
      }
   out[0] = get_byte(0, left);  out[1] = get_byte(1, left);
   out[2] = get_byte(2, left);  out[3] = get_byte(3, left);
   out[4] = get_byte(0, right); out[5] = get_byte(1, right);
   out[6] = get_byte(2, right); out[7] = get_byte(3, right);
   }

/*************************************************
* XTEA Key Schedule                              *
*************************************************/
void XTEA::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   SecureBuffer<u32bit, 4> UK;
   for(u32bit j = 0; j != 4; j++)
      UK[j] = make_u32bit(key[4*j], key[4*j+1], key[4*j+2], key[4*j+3]);
   for(u32bit j = 0; j != 2*ROUNDS; j++)
      EK[j] = DELTAS[j] + UK[KEY_INDEX[j]];
   }

}
