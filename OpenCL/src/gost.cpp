/*************************************************
* GOST Source File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/gost.h>

namespace OpenCL {

/*************************************************
* GOST Encryption                                *
*************************************************/
void GOST::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit N1 = make_u32bit(in[3], in[2], in[1], in[0]),
          N2 = make_u32bit(in[7], in[6], in[5], in[4]);
   round(N2, N1 + EK[ 0]); round(N1, N2 + EK[ 1]);
   round(N2, N1 + EK[ 2]); round(N1, N2 + EK[ 3]);
   round(N2, N1 + EK[ 4]); round(N1, N2 + EK[ 5]);
   round(N2, N1 + EK[ 6]); round(N1, N2 + EK[ 7]);
   round(N2, N1 + EK[ 0]); round(N1, N2 + EK[ 1]);
   round(N2, N1 + EK[ 2]); round(N1, N2 + EK[ 3]);
   round(N2, N1 + EK[ 4]); round(N1, N2 + EK[ 5]);
   round(N2, N1 + EK[ 6]); round(N1, N2 + EK[ 7]);
   round(N2, N1 + EK[ 0]); round(N1, N2 + EK[ 1]);
   round(N2, N1 + EK[ 2]); round(N1, N2 + EK[ 3]);
   round(N2, N1 + EK[ 4]); round(N1, N2 + EK[ 5]);
   round(N2, N1 + EK[ 6]); round(N1, N2 + EK[ 7]);
   round(N2, N1 + EK[ 7]); round(N1, N2 + EK[ 6]);
   round(N2, N1 + EK[ 5]); round(N1, N2 + EK[ 4]);
   round(N2, N1 + EK[ 3]); round(N1, N2 + EK[ 2]);
   round(N2, N1 + EK[ 1]); round(N1, N2 + EK[ 0]);
   out[0] = get_byte(3, N2); out[1] = get_byte(2, N2);
   out[2] = get_byte(1, N2); out[3] = get_byte(0, N2);
   out[4] = get_byte(3, N1); out[5] = get_byte(2, N1);
   out[6] = get_byte(1, N1); out[7] = get_byte(0, N1);
   }

/*************************************************
* GOST Decryption                                *
*************************************************/
void GOST::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit N1 = make_u32bit(in[3], in[2], in[1], in[0]),
          N2 = make_u32bit(in[7], in[6], in[5], in[4]);
   round(N2, N1 + EK[ 0]); round(N1, N2 + EK[ 1]);
   round(N2, N1 + EK[ 2]); round(N1, N2 + EK[ 3]);
   round(N2, N1 + EK[ 4]); round(N1, N2 + EK[ 5]);
   round(N2, N1 + EK[ 6]); round(N1, N2 + EK[ 7]);
   round(N2, N1 + EK[ 7]); round(N1, N2 + EK[ 6]);
   round(N2, N1 + EK[ 5]); round(N1, N2 + EK[ 4]);
   round(N2, N1 + EK[ 3]); round(N1, N2 + EK[ 2]);
   round(N2, N1 + EK[ 1]); round(N1, N2 + EK[ 0]);
   round(N2, N1 + EK[ 7]); round(N1, N2 + EK[ 6]);
   round(N2, N1 + EK[ 5]); round(N1, N2 + EK[ 4]);
   round(N2, N1 + EK[ 3]); round(N1, N2 + EK[ 2]);
   round(N2, N1 + EK[ 1]); round(N1, N2 + EK[ 0]);
   round(N2, N1 + EK[ 7]); round(N1, N2 + EK[ 6]);
   round(N2, N1 + EK[ 5]); round(N1, N2 + EK[ 4]);
   round(N2, N1 + EK[ 3]); round(N1, N2 + EK[ 2]);
   round(N2, N1 + EK[ 1]); round(N1, N2 + EK[ 0]);
   out[0] = get_byte(3, N2); out[1] = get_byte(2, N2);
   out[2] = get_byte(1, N2); out[3] = get_byte(0, N2);
   out[4] = get_byte(3, N1); out[5] = get_byte(2, N1);
   out[6] = get_byte(1, N1); out[7] = get_byte(0, N1);
   }

/*************************************************
* GOST Round                                     *
*************************************************/
void GOST::round(u32bit& left, u32bit right)
   {
   right = SBOX1[get_byte(0, right)] | SBOX2[get_byte(1, right)] |
           SBOX3[get_byte(2, right)] | SBOX4[get_byte(3, right)];
   left ^= rotate_left(right, 11);
   }

/*************************************************
* GOST Key Schedule                              *
*************************************************/
void GOST::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   for(u32bit j = 0; j != 8; j++)
      EK[j] = make_u32bit(key[4*j+3], key[4*j+2], key[4*j+1], key[4*j]);
   }

}
