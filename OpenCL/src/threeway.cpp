/*************************************************
* ThreeWay Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/threeway.h>

namespace OpenCL {

/*************************************************
* ThreeWay Encryption                            *
*************************************************/
void ThreeWay::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit A0 = make_u32bit(in[ 0], in[ 1], in[ 2], in[ 3]),
          A1 = make_u32bit(in[ 4], in[ 5], in[ 6], in[ 7]),
          A2 = make_u32bit(in[ 8], in[ 9], in[10], in[11]);
   for(u32bit j = 0; j != 11; j++)
      {
      A0 ^= EK[3*j]; A1 ^= EK[3*j+1]; A2 ^= EK[3*j+2];
      theta(A0, A1, A2);
      pi_gamma_pi(A0, A1, A2);
      }
   A0 ^= EK[33]; A1 ^= EK[34]; A2 ^= EK[35]; theta(A0, A1, A2);
   out[ 0] = get_byte(0, A0); out[ 1] = get_byte(1, A0);
   out[ 2] = get_byte(2, A0); out[ 3] = get_byte(3, A0);
   out[ 4] = get_byte(0, A1); out[ 5] = get_byte(1, A1);
   out[ 6] = get_byte(2, A1); out[ 7] = get_byte(3, A1);
   out[ 8] = get_byte(0, A2); out[ 9] = get_byte(1, A2);
   out[10] = get_byte(2, A2); out[11] = get_byte(3, A2);
   }

/*************************************************
* ThreeWay Decryption                            *
*************************************************/
void ThreeWay::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit A0 = make_u32bit(in[ 3], in[ 2], in[ 1], in[ 0]),
          A1 = make_u32bit(in[ 7], in[ 6], in[ 5], in[ 4]),
          A2 = make_u32bit(in[11], in[10], in[ 9], in[ 8]);
   mu(A0, A1, A2);
   for(u32bit j = 0; j != 11; j++)
      {
      A0 ^= DK[3*j]; A1 ^= DK[3*j+1]; A2 ^= DK[3*j+2];
      theta(A0, A1, A2);
      pi_gamma_pi(A0, A1, A2);
      }
   A0 ^= DK[33]; A1 ^= DK[34]; A2 ^= DK[35];
   theta(A0, A1, A2); mu(A0, A1, A2);
   out[ 0] = get_byte(3, A0); out[ 1] = get_byte(2, A0);
   out[ 2] = get_byte(1, A0); out[ 3] = get_byte(0, A0);
   out[ 4] = get_byte(3, A1); out[ 5] = get_byte(2, A1);
   out[ 6] = get_byte(1, A1); out[ 7] = get_byte(0, A1);
   out[ 8] = get_byte(3, A2); out[ 9] = get_byte(2, A2);
   out[10] = get_byte(1, A2); out[11] = get_byte(0, A2);
   }

/*************************************************
* ThreeWay Mu Function                           *
*************************************************/
void ThreeWay::mu(u32bit& A0, u32bit& A1, u32bit& A2)
   {
   u32bit T0;
   T0 = ((A0 & 0xAAAAAAAA) >> 1) | ((A0 & 0x55555555) << 1);
   T0 = ((T0 & 0xCCCCCCCC) >> 2) | ((T0 & 0x33333333) << 2);
   T0 = ((T0 & 0xF0F0F0F0) >> 4) | ((T0 & 0x0F0F0F0F) << 4);
   A1 = ((A1 & 0xAAAAAAAA) >> 1) | ((A1 & 0x55555555) << 1);
   A1 = ((A1 & 0xCCCCCCCC) >> 2) | ((A1 & 0x33333333) << 2);
   A1 = ((A1 & 0xF0F0F0F0) >> 4) | ((A1 & 0x0F0F0F0F) << 4);
   A0 = ((A2 & 0xAAAAAAAA) >> 1) | ((A2 & 0x55555555) << 1);
   A0 = ((A0 & 0xCCCCCCCC) >> 2) | ((A0 & 0x33333333) << 2);
   A0 = ((A0 & 0xF0F0F0F0) >> 4) | ((A0 & 0x0F0F0F0F) << 4);
   A2 = T0;
   }

/*************************************************
* ThreeWay Theta Function                        *
*************************************************/
void ThreeWay::theta(u32bit& A0, u32bit& A1, u32bit& A2)
   {
   u32bit C = rotate_left(A0 ^ A1 ^ A2, 16) ^ rotate_left(A0 ^ A1 ^ A2, 8);
   u32bit B0 = (A0 << 24) ^ (A0 >> 24) ^ (A1 <<  8) ^ (A2 >>  8);
   u32bit B1 = (A1 << 24) ^ (A1 >> 24) ^ (A2 <<  8) ^ (A0 >>  8);
   u32bit B2 = (A2 << 24) ^ (A2 >> 24) ^ (A0 <<  8) ^ (A1 >>  8);
   A0 ^= B0 ^ C; A1 ^= B1 ^ C; A2 ^= B2 ^ C;
   }

/*************************************************
* ThreeWay Pi-Gamma-Pi Sequence                  *
*************************************************/
void ThreeWay::pi_gamma_pi(u32bit& A0, u32bit& A1, u32bit& A2)
   {
   u32bit B0 = rotate_left(A0, 22), B1 = rotate_left(A2, 1);
   A0 = rotate_left(B0 ^ (A1 | (~B1)), 1);
   A2 = rotate_left(B1 ^ (B0 | (~A1)), 22);
   A1 ^= B1 | (~B0);
   }

/*************************************************
* ThreeWay Key Schedule                          *
*************************************************/
void ThreeWay::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   static const u32bit RCE[12] = {
      0x00000B0B, 0x00001616, 0x00002C2C, 0x00005858, 0x0000B0B0, 0x00007171,
      0x0000E2E2, 0x0000D5D5, 0x0000BBBB, 0x00006767, 0x0000CECE, 0x00008D8D };
   static const u32bit RCD[12] = {
      0x0000B1B1, 0x00007373, 0x0000E6E6, 0x0000DDDD, 0x0000ABAB, 0x00004747,
      0x00008E8E, 0x00000D0D, 0x00001A1A, 0x00003434, 0x00006868, 0x0000D0D0 };
   for(u32bit j = 0; j != 3; j++)
      EK[j] = make_u32bit(key[4*j  ], key[4*j+1], key[4*j+2], key[4*j+3]);
   DK = EK;
   theta(DK[0], DK[1], DK[2]);
   mu(DK[0], DK[1], DK[2]);
   for(u32bit j = 0; j != 3; j++)
      DK[j] = reverse_bytes(DK[j]);
   for(u32bit j = 3; j != 36; j++)
      { EK[j] = EK[j-3]; DK[j] = DK[j-3]; }
   for(u32bit j = 0; j != 36; j += 3)
      {
      EK[j] ^= (RCE[j/3] << 16); EK[j+2] ^= RCE[j/3];
      DK[j] ^= (RCD[j/3] << 16); DK[j+2] ^= RCD[j/3];
      }
   }

}
