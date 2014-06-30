/*************************************************
* CS-Cipher Source File                          *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/cscipher.h>

namespace OpenCL {

/*************************************************
* CS-Cipher Encryption                           *
*************************************************/
void CS_Cipher::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   SecureBuffer<byte, 16> buf;
   xor_buf(buf + 8, in, EK, 8);
   for(u32bit j = 8; j != 200; j += 16)
      {
      buf[ 0] = P[TE1[buf[ 8]] ^ buf[ 9]] ^ EK[j     ];
      buf[ 1] = P[TE1[buf[10]] ^ buf[11]] ^ EK[j +  1];
      buf[ 2] = P[TE1[buf[12]] ^ buf[13]] ^ EK[j +  2];
      buf[ 3] = P[TE1[buf[14]] ^ buf[15]] ^ EK[j +  3];
      buf[ 4] = P[ROT[buf[ 8]] ^ buf[ 9]] ^ EK[j +  4];
      buf[ 5] = P[ROT[buf[10]] ^ buf[11]] ^ EK[j +  5];
      buf[ 6] = P[ROT[buf[12]] ^ buf[13]] ^ EK[j +  6];
      buf[ 7] = P[ROT[buf[14]] ^ buf[15]] ^ EK[j +  7];
      buf[ 8] = P[TE1[buf[ 0]] ^ buf[ 1]] ^ EK[j +  8];
      buf[ 9] = P[TE1[buf[ 2]] ^ buf[ 3]] ^ EK[j +  9];
      buf[10] = P[TE1[buf[ 4]] ^ buf[ 5]] ^ EK[j + 10];
      buf[11] = P[TE1[buf[ 6]] ^ buf[ 7]] ^ EK[j + 11];
      buf[12] = P[ROT[buf[ 0]] ^ buf[ 1]] ^ EK[j + 12];
      buf[13] = P[ROT[buf[ 2]] ^ buf[ 3]] ^ EK[j + 13];
      buf[14] = P[ROT[buf[ 4]] ^ buf[ 5]] ^ EK[j + 14];
      buf[15] = P[ROT[buf[ 6]] ^ buf[ 7]] ^ EK[j + 15];
      }
   out[0] = buf[ 8]; out[1] = buf[ 9]; out[2] = buf[10]; out[3] = buf[11];
   out[4] = buf[12]; out[5] = buf[13]; out[6] = buf[14]; out[7] = buf[15];
   }

/*************************************************
* CS-Cipher Decryption                           *
*************************************************/
void CS_Cipher::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   SecureBuffer<byte, 16> buf;
   xor_buf(buf + 8, in, EK + 192, 8);
   for(s32bit j = 176; j >= 0; j -= 16)
      {
      buf[ 0] = TD1[P[buf[ 8]] ^ P[buf[12]]]              ^ EK[j +  8];
      buf[ 1] = TD2[P[buf[ 8]] ^ P[buf[12]]] ^ P[buf[12]] ^ EK[j +  9];
      buf[ 2] = TD1[P[buf[ 9]] ^ P[buf[13]]]              ^ EK[j + 10];
      buf[ 3] = TD2[P[buf[ 9]] ^ P[buf[13]]] ^ P[buf[13]] ^ EK[j + 11];
      buf[ 4] = TD1[P[buf[10]] ^ P[buf[14]]]              ^ EK[j + 12];
      buf[ 5] = TD2[P[buf[10]] ^ P[buf[14]]] ^ P[buf[14]] ^ EK[j + 13];
      buf[ 6] = TD1[P[buf[11]] ^ P[buf[15]]]              ^ EK[j + 14];
      buf[ 7] = TD2[P[buf[11]] ^ P[buf[15]]] ^ P[buf[15]] ^ EK[j + 15];
      buf[ 8] = TD1[P[buf[ 0]] ^ P[buf[ 4]]]              ^ EK[j +  0];
      buf[ 9] = TD2[P[buf[ 0]] ^ P[buf[ 4]]] ^ P[buf[ 4]] ^ EK[j +  1];
      buf[10] = TD1[P[buf[ 1]] ^ P[buf[ 5]]]              ^ EK[j +  2];
      buf[11] = TD2[P[buf[ 1]] ^ P[buf[ 5]]] ^ P[buf[ 5]] ^ EK[j +  3];
      buf[12] = TD1[P[buf[ 2]] ^ P[buf[ 6]]]              ^ EK[j +  4];
      buf[13] = TD2[P[buf[ 2]] ^ P[buf[ 6]]] ^ P[buf[ 6]] ^ EK[j +  5];
      buf[14] = TD1[P[buf[ 3]] ^ P[buf[ 7]]]              ^ EK[j +  6];
      buf[15] = TD2[P[buf[ 3]] ^ P[buf[ 7]]] ^ P[buf[ 7]] ^ EK[j +  7];
      }
   out[0] = buf[ 8]; out[1] = buf[ 9]; out[2] = buf[10]; out[3] = buf[11];
   out[4] = buf[12]; out[5] = buf[13]; out[6] = buf[14]; out[7] = buf[15];
   }

/*************************************************
* CS-Cipher Key Schedule                         *
*************************************************/
void CS_Cipher::set_key(const byte key[], u32bit length)
   throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   static const byte C[8] = { 0xB7, 0xE1, 0x51, 0x62, 0x8A, 0xED, 0x2A, 0x6A };
   static const byte D[8] = { 0xBF, 0x71, 0x58, 0x80, 0x9C, 0xF4, 0xF3, 0xC7 };
   SecureBuffer<byte, 8> buffer, matrix, K[11];
   K[0].copy(key + 8, 8);
   K[1].copy(key, 8);
   for(u32bit j = 2; j != 11; j++)
      {
      xor_buf(buffer, K[j-1], P + 8*(j-2), 8);
      for(u32bit k = 0; k != 8; k++)
         buffer[k] = P[buffer[k]];
      matrix.clear();
      for(u32bit k = 0, mask = 0x80; k != 8; k++, mask >>= 1)
         for(u32bit l = 0; l != 8; l++)
            matrix[k] |= (k >= l) ? ((buffer[l] & mask) << (k-l)) :
                                    ((buffer[l] & mask) >> (l-k));
      xor_buf(K[j], matrix, K[j-2], 8);
      }
   for(u32bit j = 0; j != 192; j += 24)
      {
      EK.copy(j, K[j/24 + 2], 8);
      EK.copy(j +  8, C, 8);
      EK.copy(j + 16, D, 8);
      }
   EK.copy(192, K[10], 8);
   }

}
