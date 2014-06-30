/*************************************************
* Rijndael Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/rijndael.h>

namespace OpenCL {

/*************************************************
* Rijndael Encryption                            *
*************************************************/
void Rijndael::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit B0 = make_u32bit(in[ 0], in[ 1], in[ 2], in[ 3]),
          B1 = make_u32bit(in[ 4], in[ 5], in[ 6], in[ 7]),
          B2 = make_u32bit(in[ 8], in[ 9], in[10], in[11]),
          B3 = make_u32bit(in[12], in[13], in[14], in[15]);
   B0 ^= EK[0]; B1 ^= EK[1]; B2 ^= EK[2]; B3 ^= EK[3];
   u32bit T0 = 0, T1 = 0, T2 = 0, T3 = 0;
   for(u32bit j = 1; j != rounds; j++)
      {
      T0 = TE0[get_byte(0, B0)] ^ TE1[get_byte(1, B1)] ^
           TE2[get_byte(2, B2)] ^ TE3[get_byte(3, B3)] ^ EK[4*j  ];
      T1 = TE0[get_byte(0, B1)] ^ TE1[get_byte(1, B2)] ^
           TE2[get_byte(2, B3)] ^ TE3[get_byte(3, B0)] ^ EK[4*j+1];
      T2 = TE0[get_byte(0, B2)] ^ TE1[get_byte(1, B3)] ^
           TE2[get_byte(2, B0)] ^ TE3[get_byte(3, B1)] ^ EK[4*j+2];
      T3 = TE0[get_byte(0, B3)] ^ TE1[get_byte(1, B0)] ^
           TE2[get_byte(2, B1)] ^ TE3[get_byte(3, B2)] ^ EK[4*j+3];
      B0 = T0; B1 = T1; B2 = T2; B3 = T3;
      }
   T0 = make_u32bit(SE[get_byte(0, B0)], SE[get_byte(1, B1)],
                    SE[get_byte(2, B2)], SE[get_byte(3, B3)]) ^ EK[4*rounds];
   T1 = make_u32bit(SE[get_byte(0, B1)], SE[get_byte(1, B2)],
                    SE[get_byte(2, B3)], SE[get_byte(3, B0)]) ^ EK[4*rounds+1];
   T2 = make_u32bit(SE[get_byte(0, B2)], SE[get_byte(1, B3)],
                    SE[get_byte(2, B0)], SE[get_byte(3, B1)]) ^ EK[4*rounds+2];
   T3 = make_u32bit(SE[get_byte(0, B3)], SE[get_byte(1, B0)],
                    SE[get_byte(2, B1)], SE[get_byte(3, B2)]) ^ EK[4*rounds+3];
   out[ 0] = get_byte(0, T0); out[ 1] = get_byte(1, T0);
   out[ 2] = get_byte(2, T0); out[ 3] = get_byte(3, T0);
   out[ 4] = get_byte(0, T1); out[ 5] = get_byte(1, T1);
   out[ 6] = get_byte(2, T1); out[ 7] = get_byte(3, T1);
   out[ 8] = get_byte(0, T2); out[ 9] = get_byte(1, T2);
   out[10] = get_byte(2, T2); out[11] = get_byte(3, T2);
   out[12] = get_byte(0, T3); out[13] = get_byte(1, T3);
   out[14] = get_byte(2, T3); out[15] = get_byte(3, T3);
   }

/*************************************************
* Rijndael Decryption                            *
*************************************************/
void Rijndael::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit B0 = make_u32bit(in[ 0], in[ 1], in[ 2], in[ 3]),
          B1 = make_u32bit(in[ 4], in[ 5], in[ 6], in[ 7]),
          B2 = make_u32bit(in[ 8], in[ 9], in[10], in[11]),
          B3 = make_u32bit(in[12], in[13], in[14], in[15]);
   B0 ^= DK[0]; B1 ^= DK[1]; B2 ^= DK[2]; B3 ^= DK[3];
   u32bit T0, T1, T2, T3;
   for(u32bit j = 1; j != rounds; j++)
      {
      T0 = TD0[get_byte(0, B0)] ^ TD1[get_byte(1, B3)] ^
           TD2[get_byte(2, B2)] ^ TD3[get_byte(3, B1)] ^ DK[4*j];
      T1 = TD0[get_byte(0, B1)] ^ TD1[get_byte(1, B0)] ^
           TD2[get_byte(2, B3)] ^ TD3[get_byte(3, B2)] ^ DK[4*j+1];
      T2 = TD0[get_byte(0, B2)] ^ TD1[get_byte(1, B1)] ^
           TD2[get_byte(2, B0)] ^ TD3[get_byte(3, B3)] ^ DK[4*j+2];
      T3 = TD0[get_byte(0, B3)] ^ TD1[get_byte(1, B2)] ^
           TD2[get_byte(2, B1)] ^ TD3[get_byte(3, B0)] ^ DK[4*j+3];
      B0 = T0; B1 = T1; B2 = T2; B3 = T3;
      }
   T0 = make_u32bit(SD[get_byte(0, B0)], SD[get_byte(1, B3)],
                    SD[get_byte(2, B2)], SD[get_byte(3, B1)]) ^ DK[4*rounds];
   T1 = make_u32bit(SD[get_byte(0, B1)], SD[get_byte(1, B0)],
                    SD[get_byte(2, B3)], SD[get_byte(3, B2)]) ^ DK[4*rounds+1];
   T2 = make_u32bit(SD[get_byte(0, B2)], SD[get_byte(1, B1)],
                    SD[get_byte(2, B0)], SD[get_byte(3, B3)]) ^ DK[4*rounds+2];
   T3 = make_u32bit(SD[get_byte(0, B3)], SD[get_byte(1, B2)],
                    SD[get_byte(2, B1)], SD[get_byte(3, B0)]) ^ DK[4*rounds+3];
   out[ 0] = get_byte(0, T0); out[ 1] = get_byte(1, T0);
   out[ 2] = get_byte(2, T0); out[ 3] = get_byte(3, T0);
   out[ 4] = get_byte(0, T1); out[ 5] = get_byte(1, T1);
   out[ 6] = get_byte(2, T1); out[ 7] = get_byte(3, T1);
   out[ 8] = get_byte(0, T2); out[ 9] = get_byte(1, T2);
   out[10] = get_byte(2, T2); out[11] = get_byte(3, T2);
   out[12] = get_byte(0, T3); out[13] = get_byte(1, T3);
   out[14] = get_byte(2, T3); out[15] = get_byte(3, T3);
   }

/*************************************************
* Rijndael Key Schedule                          *
*************************************************/
void Rijndael::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   static const u32bit RC[10] = {
      0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000,
      0x40000000, 0x80000000, 0x1B000000, 0x36000000 };
   rounds = (length / 4) + 6;
   for(u32bit j = 0; j != length / 4; j++)
      EK[j] = make_u32bit(key[4*j], key[4*j+1], key[4*j+2], key[4*j+3]);
   if(length == 16)
      {
      for(u32bit j = 4; j < 4*(rounds+1); j += 4)
         {
         EK[j] = EK[j-4] ^ S(rotate_left(EK[j-1], 8)) ^ RC[(j-4)/4];
         EK[j+1] = EK[j-3] ^ EK[j  ]; EK[j+2] = EK[j-2] ^ EK[j+1];
         EK[j+3] = EK[j-1] ^ EK[j+2];
         }
      }
   else if(length == 24)
      {
      for(u32bit j = 6; j < 4*(rounds+1); j += 6)
         {
         EK[j] = EK[j-6] ^ S(rotate_left(EK[j-1], 8)) ^ RC[(j-6)/6];
         EK[j+1] = EK[j-5] ^ EK[j  ]; EK[j+2] = EK[j-4] ^ EK[j+1];
         EK[j+3] = EK[j-3] ^ EK[j+2]; EK[j+4] = EK[j-2] ^ EK[j+3];
         EK[j+5] = EK[j-1] ^ EK[j+4];
         }
      }
   else if(length == 32)
      {
      for(u32bit j = 8; j < 4*(rounds+1); j += 8)
         {
         EK[j] = EK[j-8] ^ S(rotate_left(EK[j-1], 8)) ^ RC[(j-8)/8];
         EK[j+1] = EK[j-7] ^ EK[j  ]; EK[j+2] = EK[j-6] ^ EK[j+1];
         EK[j+3] = EK[j-5] ^ EK[j+2]; EK[j+4] = EK[j-4] ^ S(EK[j+3]);
         EK[j+5] = EK[j-3] ^ EK[j+4]; EK[j+6] = EK[j-2] ^ EK[j+5];
         EK[j+7] = EK[j-1] ^ EK[j+6];
         }
      }
   for(u32bit j = 0; j != 4*(rounds+1); j += 4)
      {
      DK[j  ] = EK[4*rounds-j  ];
      DK[j+1] = EK[4*rounds-j+1];
      DK[j+2] = EK[4*rounds-j+2];
      DK[j+3] = EK[4*rounds-j+3];
      }
   for(u32bit j = 4; j != length + 24; j++)
      DK[j] = TD0[SE[get_byte(0, DK[j])]] ^ TD1[SE[get_byte(1, DK[j])]] ^
              TD2[SE[get_byte(2, DK[j])]] ^ TD3[SE[get_byte(3, DK[j])]];
   }

/*************************************************
* Rijndael Byte Substitution                     *
*************************************************/
u32bit Rijndael::S(u32bit input)
   {
   return make_u32bit(SE[get_byte(0, input)], SE[get_byte(1, input)],
                      SE[get_byte(2, input)], SE[get_byte(3, input)]);
   }

}
