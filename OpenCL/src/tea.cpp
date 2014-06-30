/*************************************************
* TEA Source File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/tea.h>

namespace OpenCL {

/*************************************************
* TEA Encryption                                 *
*************************************************/
void TEA::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit left  = make_u32bit(in[0], in[1], in[2], in[3]),
          right = make_u32bit(in[4], in[5], in[6], in[7]);
   u32bit sum = 0;
   for(u32bit j = 0; j != ROUNDS; j++)
      {
      sum += DELTA;
      left  += ((right << 4) + K[0]) ^ (right + sum) ^ ((right >> 5) + K[1]);
      right += ((left  << 4) + K[2]) ^ (left  + sum) ^ ((left  >> 5) + K[3]);
      }
   out[0] = get_byte(0, left);  out[1] = get_byte(1, left);
   out[2] = get_byte(2, left);  out[3] = get_byte(3, left);
   out[4] = get_byte(0, right); out[5] = get_byte(1, right);
   out[6] = get_byte(2, right); out[7] = get_byte(3, right);
   }

/*************************************************
* TEA Decryption                                 *
*************************************************/
void TEA::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit left  = make_u32bit(in[0], in[1], in[2], in[3]),
          right = make_u32bit(in[4], in[5], in[6], in[7]);
   u32bit sum = DELTA * ROUNDS;
   for(u32bit j = 0; j != ROUNDS; j++)
      {
      right -= ((left  << 4) + K[2]) ^ (left  + sum) ^ ((left  >> 5) + K[3]);
      left  -= ((right << 4) + K[0]) ^ (right + sum) ^ ((right >> 5) + K[1]);
      sum -= DELTA;
      }
   out[0] = get_byte(0, left);  out[1] = get_byte(1, left);
   out[2] = get_byte(2, left);  out[3] = get_byte(3, left);
   out[4] = get_byte(0, right); out[5] = get_byte(1, right);
   out[6] = get_byte(2, right); out[7] = get_byte(3, right);
   }

/*************************************************
* TEA Key Schedule                               *
*************************************************/
void TEA::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   for(u32bit j = 0; j != 4; j++)
      K[j] = make_u32bit(key[4*j], key[4*j+1], key[4*j+2], key[4*j+3]);
   }

}
