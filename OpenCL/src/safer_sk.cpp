/*************************************************
* SAFER-SK128 Source File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/safer_sk.h>

namespace OpenCL {

/*************************************************
* SAFER-SK128 Encryption                         *
*************************************************/
void SAFER_SK128::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   byte A = in[0], B = in[1], C = in[2], D = in[3],
        E = in[4], F = in[5], G = in[6], H = in[7], X, Y;
   for(u32bit j = 0; j != 8*ROUNDS; j += 8)
      {
      A = SE[j  ][A]; B = SE[j+1][B]; C = SE[j+2][C]; D = SE[j+3][D];
      E = SE[j+4][E]; F = SE[j+5][F]; G = SE[j+6][G]; H = SE[j+7][H];
      B += A; D += C; F += E; H += G; A += B; C += D; E += F; G += H;
      C += A; G += E; D += B; H += F; A += C; E += G; B += D; F += H;
      H += D; Y = D + H; D = B + F; X = B + D; B = A + E;
      A += B; F = C + G; E = C + F; C = X; G = Y;
      }
   out[0] = A ^ MK[0]; out[1] = B + MK[1]; out[2] = C + MK[2];
   out[3] = D ^ MK[3]; out[4] = E ^ MK[4]; out[5] = F + MK[5];
   out[6] = G + MK[6]; out[7] = H ^ MK[7];
   }

/*************************************************
* SAFER-SK128 Decryption                         *
*************************************************/
void SAFER_SK128::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   byte A = in[0] ^ MK[0], E = in[1] - MK[1], B = in[2] - MK[2],
        F = in[3] ^ MK[3], C = in[4] ^ MK[4], G = in[5] - MK[5],
        D = in[6] - MK[6], H = in[7] ^ MK[7], X, Y;
   for(s32bit j = 8*(ROUNDS-1); j >= 0; j -= 8)
      {
      A -= E; B -= F; C -= G; D -= H; E -= A; F -= B; G -= C; H -= D;
      A -= C; E -= G; B -= D; F -= H; C -= A; G -= E; D -= B; H -= F;
      A -= B; C -= D; E -= F; G -= H; B -= A; D -= C; F -= E; H -= G;
      A = SD[j  ][A]; H = SD[j+7][H]; X = SD[j+4][E]; E = SD[j+1][B];
      B = SD[j+2][C]; Y = SD[j+5][F]; F = SD[j+3][D]; D = SD[j+6][G];
      C = X; G = Y;
      }
   out[0] = A; out[1] = E; out[2] = B; out[3] = F;
   out[4] = C; out[5] = G; out[6] = D; out[7] = H;
   }

/*************************************************
* SAFER-SK128 Key Schedule                       *
*************************************************/
void SAFER_SK128::set_key(const byte key[], u32bit length)
   throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);

   SecureBuffer<byte, 216> EK;
   SecureBuffer<byte, 18> KB;

   for(u32bit j = 0; j != 8; j++)
      {
      KB[ 8] ^= KB[j] = rotate_left(key[j], 5);
      KB[17] ^= KB[j+9] = EK[j] = key[j+8];
      }
   for(u32bit j = 0; j != ROUNDS; j++)
      {
      for(u32bit k = 0; k != 18; k++)
         KB[k] = rotate_left(KB[k], 6);
      for(u32bit k = 0; k != 16; k++)
         EK[16*j+k+8] = KB[KEY_INDEX[16*j+k]] + BIAS[16*j+k];
      }

   for(u32bit j = 0; j != 104; j += 8)
      {
      for(u32bit k = 0; k != 256; k++)
         {
         SE[j  ][k] = EXP[k ^ EK[2*j   ]] + EK[2*j+ 8];
         SE[j+1][k] = LOG[k + EK[2*j+ 1]] ^ EK[2*j+ 9];
         SE[j+2][k] = LOG[k + EK[2*j+ 2]] ^ EK[2*j+10];
         SE[j+3][k] = EXP[k ^ EK[2*j+ 3]] + EK[2*j+11];
         SE[j+4][k] = EXP[k ^ EK[2*j+ 4]] + EK[2*j+12];
         SE[j+5][k] = LOG[k + EK[2*j+ 5]] ^ EK[2*j+13];
         SE[j+6][k] = LOG[k + EK[2*j+ 6]] ^ EK[2*j+14];
         SE[j+7][k] = EXP[k ^ EK[2*j+ 7]] + EK[2*j+15];

         SD[j  ][k] = LOG[k - EK[2*j+ 8] + 256] ^ EK[2*j+ 0];
         SD[j+1][k] = EXP[k ^ EK[2*j+ 9]      ] - EK[2*j+ 1];
         SD[j+2][k] = EXP[k ^ EK[2*j+10]      ] - EK[2*j+ 2];
         SD[j+3][k] = LOG[k - EK[2*j+11] + 256] ^ EK[2*j+ 3];
         SD[j+4][k] = LOG[k - EK[2*j+12] + 256] ^ EK[2*j+ 4];
         SD[j+5][k] = EXP[k ^ EK[2*j+13]      ] - EK[2*j+ 5];
         SD[j+6][k] = EXP[k ^ EK[2*j+14]      ] - EK[2*j+ 6];
         SD[j+7][k] = LOG[k - EK[2*j+15] + 256] ^ EK[2*j+ 7];
         }
      }
   MK.copy(EK + 16*ROUNDS, 8);
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void SAFER_SK128::clear() throw()
   {
   MK.clear();
   for(u32bit j = 0; j != 104; j++)
      SE[j].clear();
   for(u32bit j = 0; j != 104; j++)
      SD[j].clear();
   }

/*************************************************
* SAFER-SK128 Constructor                        *
*************************************************/
SAFER_SK128::SAFER_SK128(u32bit r) : BlockCipher(name(), BLOCKSIZE, KEYLENGTH),
                                     ROUNDS(r)
   {
   if(ROUNDS > MAX_ROUNDS || ROUNDS == 0)
      throw Invalid_Argument(name() + ": Invalid number of rounds");
   }

}
