/*************************************************
* RC2 Source File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/rc2.h>

namespace OpenCL {

/*************************************************
* RC2 Encryption                                 *
*************************************************/
void RC2::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u16bit R0 = make_u16bit(in[1], in[0]), R1 = make_u16bit(in[3], in[2]),
          R2 = make_u16bit(in[5], in[4]), R3 = make_u16bit(in[7], in[6]);
   mix(R0, R1, R2, R3, 0); mix(R0, R1, R2, R3, 1); mix(R0, R1, R2, R3, 2);
   mix(R0, R1, R2, R3, 3); mix(R0, R1, R2, R3, 4); mash(R0, R1, R2, R3);
   mix(R0, R1, R2, R3, 5); mix(R0, R1, R2, R3, 6); mix(R0, R1, R2, R3, 7);
   mix(R0, R1, R2, R3, 8); mix(R0, R1, R2, R3, 9); mix(R0, R1, R2, R3,10);
   mash(R0, R1, R2, R3);   mix(R0, R1, R2, R3,11); mix(R0, R1, R2, R3,12);
   mix(R0, R1, R2, R3,13); mix(R0, R1, R2, R3,14); mix(R0, R1, R2, R3,15);
   out[0] = get_byte(1, R0); out[1] = get_byte(0, R0);
   out[2] = get_byte(1, R1); out[3] = get_byte(0, R1);
   out[4] = get_byte(1, R2); out[5] = get_byte(0, R2);
   out[6] = get_byte(1, R3); out[7] = get_byte(0, R3);
   }

/*************************************************
* RC2 Decryption                                 *
*************************************************/
void RC2::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u16bit R0 = make_u16bit(in[1], in[0]), R1 = make_u16bit(in[3], in[2]),
          R2 = make_u16bit(in[5], in[4]), R3 = make_u16bit(in[7], in[6]);
   rmix(R0, R1, R2, R3,15); rmix(R0, R1, R2, R3,14); rmix(R0, R1, R2, R3,13);
   rmix(R0, R1, R2, R3,12); rmix(R0, R1, R2, R3,11); rmash(R0, R1, R2, R3);
   rmix(R0, R1, R2, R3,10); rmix(R0, R1, R2, R3, 9); rmix(R0, R1, R2, R3, 8);
   rmix(R0, R1, R2, R3, 7); rmix(R0, R1, R2, R3, 6); rmix(R0, R1, R2, R3, 5);
   rmash(R0, R1, R2, R3);   rmix(R0, R1, R2, R3, 4); rmix(R0, R1, R2, R3, 3);
   rmix(R0, R1, R2, R3, 2); rmix(R0, R1, R2, R3, 1); rmix(R0, R1, R2, R3, 0);
   out[0] = get_byte(1, R0); out[1] = get_byte(0, R0);
   out[2] = get_byte(1, R1); out[3] = get_byte(0, R1);
   out[4] = get_byte(1, R2); out[5] = get_byte(0, R2);
   out[6] = get_byte(1, R3); out[7] = get_byte(0, R3);
   }

/*************************************************
* RC2 Mix Function                               *
*************************************************/
void RC2::mix(u16bit& R0, u16bit& R1, u16bit& R2,
              u16bit& R3, u32bit round) const
   {
   R0 += (R1 & ~R3) + (R2 & R3) + K[4*round    ]; R0 = rotate_left(R0, 1);
   R1 += (R2 & ~R0) + (R3 & R0) + K[4*round + 1]; R1 = rotate_left(R1, 2);
   R2 += (R3 & ~R1) + (R0 & R1) + K[4*round + 2]; R2 = rotate_left(R2, 3);
   R3 += (R0 & ~R2) + (R1 & R2) + K[4*round + 3]; R3 = rotate_left(R3, 5);
   }

/*************************************************
* RC2 R-Mix Function                             *
*************************************************/
void RC2::rmix(u16bit& R0, u16bit& R1, u16bit& R2,
               u16bit& R3, u32bit round) const
   {
   R3 = rotate_right(R3, 5); R3 -= (R0 & ~R2) + (R1 & R2) + K[4*round + 3];
   R2 = rotate_right(R2, 3); R2 -= (R3 & ~R1) + (R0 & R1) + K[4*round + 2];
   R1 = rotate_right(R1, 2); R1 -= (R2 & ~R0) + (R3 & R0) + K[4*round + 1];
   R0 = rotate_right(R0, 1); R0 -= (R1 & ~R3) + (R2 & R3) + K[4*round + 0];
   }

/*************************************************
* RC2 Mash Function                              *
*************************************************/
void RC2::mash(u16bit& R0, u16bit& R1, u16bit& R2, u16bit& R3) const
   {
   R0 += K[R3 % 64]; R1 += K[R0 % 64]; R2 += K[R1 % 64]; R3 += K[R2 % 64];
   }

/*************************************************
* RC2 R-Mash Function                            *
*************************************************/
void RC2::rmash(u16bit& R0, u16bit& R1, u16bit& R2, u16bit& R3) const
   {
   R3 -= K[R2 % 64]; R2 -= K[R1 % 64]; R1 -= K[R0 % 64]; R0 -= K[R3 % 64];
   }

/*************************************************
* RC2 Key Schedule                               *
*************************************************/
void RC2::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   SecureBuffer<byte, 128> L;
   L.copy(key, length);
   for(u32bit j = length; j != 128; j++)
      L[j] = TABLE[(L[j-1] + L[j-length]) % 256];
   L[128-length] = TABLE[L[128-length]];
   for(s32bit j = 127-length; j >= 0; j--)
      L[j] = TABLE[L[j+1] ^ L[j+length]];
   for(u32bit j = 0; j != 64; j++)
      K[j] = make_u16bit(L[2*j+1], L[2*j]);
   }

}
