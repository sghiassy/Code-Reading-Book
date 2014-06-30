/*************************************************
* MISTY1 Source File                             *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/misty1.h>

namespace OpenCL {

/*************************************************
* MISTY1 Encryption                              *
*************************************************/
void MISTY1::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u16bit B0 = make_u16bit(in[0], in[1]), B1 = make_u16bit(in[2], in[3]),
          B2 = make_u16bit(in[4], in[5]), B3 = make_u16bit(in[6], in[7]);

   FL_E(B0, B1, B2, B3,  0);
   FO_E(B0, B1, B2, B3,  1);
   FO_E(B2, B3, B0, B1,  2);
   FL_E(B0, B1, B2, B3,  3);
   FO_E(B0, B1, B2, B3,  4);
   FO_E(B2, B3, B0, B1,  5);
   FL_E(B0, B1, B2, B3,  6);
   FO_E(B0, B1, B2, B3,  7);
   FO_E(B2, B3, B0, B1,  8);
   FL_E(B0, B1, B2, B3,  9);
   FO_E(B0, B1, B2, B3, 10);
   FO_E(B2, B3, B0, B1, 11);
   FL_E(B0, B1, B2, B3, 12);

   out[0] = get_byte(0, B2); out[1] = get_byte(1, B2);
   out[2] = get_byte(0, B3); out[3] = get_byte(1, B3);
   out[4] = get_byte(0, B0); out[5] = get_byte(1, B0);
   out[6] = get_byte(0, B1); out[7] = get_byte(1, B1);
   }

/*************************************************
* MISTY1 Decryption                              *
*************************************************/
void MISTY1::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u16bit B0 = make_u16bit(in[4], in[5]), B1 = make_u16bit(in[6], in[7]),
          B2 = make_u16bit(in[0], in[1]), B3 = make_u16bit(in[2], in[3]);

   FL_D(B0, B1, B2, B3,  0);
   FO_D(B2, B3, B0, B1,  1);
   FO_D(B0, B1, B2, B3,  2);
   FL_D(B0, B1, B2, B3,  3);
   FO_D(B2, B3, B0, B1,  4);
   FO_D(B0, B1, B2, B3,  5);
   FL_D(B0, B1, B2, B3,  6);
   FO_D(B2, B3, B0, B1,  7);
   FO_D(B0, B1, B2, B3,  8);
   FL_D(B0, B1, B2, B3,  9);
   FO_D(B2, B3, B0, B1, 10);
   FO_D(B0, B1, B2, B3, 11);
   FL_D(B0, B1, B2, B3, 12);

   out[0] = get_byte(0, B0); out[1] = get_byte(1, B0);
   out[2] = get_byte(0, B1); out[3] = get_byte(1, B1);
   out[4] = get_byte(0, B2); out[5] = get_byte(1, B2);
   out[6] = get_byte(0, B3); out[7] = get_byte(1, B3);
   }

/*************************************************
* MISTY1 FL_E Function                           *
*************************************************/
void MISTY1::FL_E(u16bit& B0, u16bit& B1, u16bit& B2, u16bit& B3,
                  u32bit round) const
   {
   B1 ^= B0 & EK[8 * round];
   B0 ^= B1 | EK[8 * round + 1];
   B3 ^= B2 & EK[8 * round + 2];
   B2 ^= B3 | EK[8 * round + 3];
   }

/*************************************************
* MISTY1 FL_D Function                           *
*************************************************/
void MISTY1::FL_D(u16bit& B0, u16bit& B1, u16bit& B2, u16bit& B3,
                   u32bit round) const
   {
   B2 ^= B3 | DK[8 * round];
   B3 ^= B2 & DK[8 * round + 1];
   B0 ^= B1 | DK[8 * round + 2];
   B1 ^= B0 & DK[8 * round + 3];
   }

/*************************************************
* MISTY1 FO_E Function                           *
*************************************************/
void MISTY1::FO_E(u16bit B0, u16bit B1, u16bit& B2, u16bit& B3,
                  u32bit round) const
   {
   u32bit offset = 10 * (round - (round / 3)) + 4 * ((round / 3)) - 6;
   B0  = FI(B0 ^ EK[offset    ], EK[offset + 1], EK[offset + 2]) ^ B1;
   B1  = FI(B1 ^ EK[offset + 3], EK[offset + 4], EK[offset + 5]) ^ B0;
   B0  = FI(B0 ^ EK[offset + 6], EK[offset + 7], EK[offset + 8]) ^ B1;
   B1 ^= EK[offset + 9]; B2 ^= B1; B3 ^= B0;
   }

/*************************************************
* MISTY1 FO_D Function                           *
*************************************************/
void MISTY1::FO_D(u16bit B0, u16bit B1, u16bit& B2, u16bit& B3,
                  u32bit round) const
   {
   u32bit offset = 10 * (round - (round / 3)) + 4 * ((round / 3)) - 6;
   B0  = FI(B0 ^ DK[offset    ], DK[offset + 1], DK[offset + 2]) ^ B1;
   B1  = FI(B1 ^ DK[offset + 3], DK[offset + 4], DK[offset + 5]) ^ B0;
   B0  = FI(B0 ^ DK[offset + 6], DK[offset + 7], DK[offset + 8]) ^ B1;
   B1 ^= DK[offset + 9]; B2 ^= B1; B3 ^= B0;
   }

/*************************************************
* MISTY1 FI Function                             *
*************************************************/
u16bit MISTY1::FI(u16bit input, u16bit key7, u16bit key9)
   {
   u16bit D9 = input >> 7, D7 = input & 0x7F;
   D9 = S9[D9] ^ D7;
   D7 = (S7[D7] ^ key7 ^ D9) & 0x7F;
   D9 = S9[D9 ^ key9] ^ D7;
   return (u16bit)((D7 << 9) | D9);
   }

/*************************************************
* MISTY1 Key Schedule                            *
*************************************************/
void MISTY1::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   SecureBuffer<u16bit, 32> KS;
   for(u32bit j = 0; j != KEYLENGTH; j++)
      KS[j] = make_u16bit(key[2*j], key[2*j+1]);
   for(u32bit j = 0; j != 8; j++)
      {
      KS[j+ 8] = FI(KS[j], KS[(j+1) % 8] >> 9, KS[(j+1) % 8] & 0x1FF);
      KS[j+16] = KS[j+8] >> 9;
      KS[j+24] = KS[j+8] & 0x1FF;
      }
   for(u32bit j = 0; j != 100; j++)
      {
      EK[j] = KS[EK_ORDER[j]];
      DK[j] = KS[DK_ORDER[j]];
      }
   }

}
