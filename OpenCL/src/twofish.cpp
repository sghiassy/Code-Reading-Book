/*************************************************
* Twofish Source File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/twofish.h>

namespace OpenCL {

/*************************************************
* Twofish Encryption                             *
*************************************************/
void Twofish::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit A = make_u32bit(in[ 3], in[ 2], in[ 1], in[ 0]),
          B = make_u32bit(in[ 7], in[ 6], in[ 5], in[ 4]),
          C = make_u32bit(in[11], in[10], in[ 9], in[ 8]),
          D = make_u32bit(in[15], in[14], in[13], in[12]);
   A ^= round_key[0]; B ^= round_key[1]; C ^= round_key[2]; D ^= round_key[3];
   encrypt_round(A, B, C, D, 0); encrypt_round(C, D, A, B, 1);
   encrypt_round(A, B, C, D, 2); encrypt_round(C, D, A, B, 3);
   encrypt_round(A, B, C, D, 4); encrypt_round(C, D, A, B, 5);
   encrypt_round(A, B, C, D, 6); encrypt_round(C, D, A, B, 7);
   encrypt_round(A, B, C, D, 8); encrypt_round(C, D, A, B, 9);
   encrypt_round(A, B, C, D,10); encrypt_round(C, D, A, B,11);
   encrypt_round(A, B, C, D,12); encrypt_round(C, D, A, B,13);
   encrypt_round(A, B, C, D,14); encrypt_round(C, D, A, B,15);
   C ^= round_key[4]; D ^= round_key[5]; A ^= round_key[6]; B ^= round_key[7];
   out[ 0] = get_byte(3, C); out[ 1] = get_byte(2, C);
   out[ 2] = get_byte(1, C); out[ 3] = get_byte(0, C);
   out[ 4] = get_byte(3, D); out[ 5] = get_byte(2, D);
   out[ 6] = get_byte(1, D); out[ 7] = get_byte(0, D);
   out[ 8] = get_byte(3, A); out[ 9] = get_byte(2, A);
   out[10] = get_byte(1, A); out[11] = get_byte(0, A);
   out[12] = get_byte(3, B); out[13] = get_byte(2, B);
   out[14] = get_byte(1, B); out[15] = get_byte(0, B);
   }

/*************************************************
* Twofish Decryption                             *
*************************************************/
void Twofish::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit A = make_u32bit(in[ 3], in[ 2], in[ 1], in[ 0]),
          B = make_u32bit(in[ 7], in[ 6], in[ 5], in[ 4]),
          C = make_u32bit(in[11], in[10], in[ 9], in[ 8]),
          D = make_u32bit(in[15], in[14], in[13], in[12]);
   A ^= round_key[4]; B ^= round_key[5]; C ^= round_key[6]; D ^= round_key[7];
   decrypt_round(A, B, C, D,15); decrypt_round(C, D, A, B,14);
   decrypt_round(A, B, C, D,13); decrypt_round(C, D, A, B,12);
   decrypt_round(A, B, C, D,11); decrypt_round(C, D, A, B,10);
   decrypt_round(A, B, C, D, 9); decrypt_round(C, D, A, B, 8);
   decrypt_round(A, B, C, D, 7); decrypt_round(C, D, A, B, 6);
   decrypt_round(A, B, C, D, 5); decrypt_round(C, D, A, B, 4);
   decrypt_round(A, B, C, D, 3); decrypt_round(C, D, A, B, 2);
   decrypt_round(A, B, C, D, 1); decrypt_round(C, D, A, B, 0);
   C ^= round_key[0]; D ^= round_key[1]; A ^= round_key[2]; B ^= round_key[3];
   out[ 0] = get_byte(3, C); out[ 1] = get_byte(2, C);
   out[ 2] = get_byte(1, C); out[ 3] = get_byte(0, C);
   out[ 4] = get_byte(3, D); out[ 5] = get_byte(2, D);
   out[ 6] = get_byte(1, D); out[ 7] = get_byte(0, D);
   out[ 8] = get_byte(3, A); out[ 9] = get_byte(2, A);
   out[10] = get_byte(1, A); out[11] = get_byte(0, A);
   out[12] = get_byte(3, B); out[13] = get_byte(2, B);
   out[14] = get_byte(1, B); out[15] = get_byte(0, B);
   }

/*************************************************
* Twofish Encryption Round                       *
*************************************************/
void Twofish::encrypt_round(u32bit A, u32bit B, u32bit& C, u32bit& D,
                            u32bit round) const
   {
   u32bit X = SBox0[get_byte(3, A)] ^ SBox1[get_byte(2, A)] ^
              SBox2[get_byte(1, A)] ^ SBox3[get_byte(0, A)];
   u32bit Y = SBox0[get_byte(0, B)] ^ SBox1[get_byte(3, B)] ^
              SBox2[get_byte(2, B)] ^ SBox3[get_byte(1, B)];
   X += Y; Y += X + round_key[2*round + 9]; X += round_key[2*round + 8];
   C = rotate_right(C ^ X, 1); D = rotate_left(D, 1) ^ Y;
   }

/*************************************************
* Twofish Decryption Round                       *
*************************************************/
void Twofish::decrypt_round(u32bit A, u32bit B, u32bit& C, u32bit& D,
                            u32bit round) const
   {
   u32bit X = SBox0[get_byte(3, A)] ^ SBox1[get_byte(2, A)] ^
              SBox2[get_byte(1, A)] ^ SBox3[get_byte(0, A)];
   u32bit Y = SBox0[get_byte(0, B)] ^ SBox1[get_byte(3, B)] ^
              SBox2[get_byte(2, B)] ^ SBox3[get_byte(1, B)];
   X += Y; Y += X + round_key[2*round + 9]; X += round_key[2*round + 8];
   C = rotate_left(C, 1) ^ X; D = rotate_right(D ^ Y, 1);
   }

/*************************************************
* Twofish Key Schedule                           *
*************************************************/
void Twofish::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   SecureBuffer<byte, 16> S;
   for(u32bit j = 0; j != length; j++)
      rs_mul(S + 4*(j/8), key[j], RS[(4*j  )%32], RS[(4*j+1)%32],
                                  RS[(4*j+2)%32], RS[(4*j+3)%32]);
   if(length == 16)
      {
      for(u32bit j = 0; j != 256; j++)
         {
         SBox0[j] = MDS0[Q0[Q0[j]^S[ 0]]^S[ 4]];
         SBox1[j] = MDS1[Q0[Q1[j]^S[ 1]]^S[ 5]];
         SBox2[j] = MDS2[Q1[Q0[j]^S[ 2]]^S[ 6]];
         SBox3[j] = MDS3[Q1[Q1[j]^S[ 3]]^S[ 7]];
         }
      for(u32bit j = 0; j != 40; j += 2)
         {
         u32bit X = MDS0[Q0[Q0[j  ]^key[ 8]]^key[ 0]] ^
                    MDS1[Q0[Q1[j  ]^key[ 9]]^key[ 1]] ^
                    MDS2[Q1[Q0[j  ]^key[10]]^key[ 2]] ^
                    MDS3[Q1[Q1[j  ]^key[11]]^key[ 3]];
         u32bit Y = MDS0[Q0[Q0[j+1]^key[12]]^key[ 4]] ^
                    MDS1[Q0[Q1[j+1]^key[13]]^key[ 5]] ^
                    MDS2[Q1[Q0[j+1]^key[14]]^key[ 6]] ^
                    MDS3[Q1[Q1[j+1]^key[15]]^key[ 7]];
         Y = rotate_left(Y, 8); X += Y; Y += X;
         round_key[j] = X; round_key[j+1] = rotate_left(Y, 9);
         }
      }
   else if(length == 24)
      {
      for(u32bit j = 0; j != 256; j++)
         {
         SBox0[j] = MDS0[Q0[Q0[Q1[j]^S[ 0]]^S[ 4]]^S[ 8]];
         SBox1[j] = MDS1[Q0[Q1[Q1[j]^S[ 1]]^S[ 5]]^S[ 9]];
         SBox2[j] = MDS2[Q1[Q0[Q0[j]^S[ 2]]^S[ 6]]^S[10]];
         SBox3[j] = MDS3[Q1[Q1[Q0[j]^S[ 3]]^S[ 7]]^S[11]];
         }
      for(u32bit j = 0; j != 40; j += 2)
         {
         u32bit X = MDS0[Q0[Q0[Q1[j  ]^key[16]]^key[ 8]]^key[ 0]] ^
                    MDS1[Q0[Q1[Q1[j  ]^key[17]]^key[ 9]]^key[ 1]] ^
                    MDS2[Q1[Q0[Q0[j  ]^key[18]]^key[10]]^key[ 2]] ^
                    MDS3[Q1[Q1[Q0[j  ]^key[19]]^key[11]]^key[ 3]];
         u32bit Y = MDS0[Q0[Q0[Q1[j+1]^key[20]]^key[12]]^key[ 4]] ^
                    MDS1[Q0[Q1[Q1[j+1]^key[21]]^key[13]]^key[ 5]] ^
                    MDS2[Q1[Q0[Q0[j+1]^key[22]]^key[14]]^key[ 6]] ^
                    MDS3[Q1[Q1[Q0[j+1]^key[23]]^key[15]]^key[ 7]];
         Y = rotate_left(Y, 8); X += Y; Y += X;
         round_key[j] = X; round_key[j+1] = rotate_left(Y, 9);
         }
      }
   else if(length == 32)
      {
      for(u32bit j = 0; j != 256; j++)
         {
         SBox0[j] = MDS0[Q0[Q0[Q1[Q1[j]^S[ 0]]^S[ 4]]^S[ 8]]^S[12]];
         SBox1[j] = MDS1[Q0[Q1[Q1[Q0[j]^S[ 1]]^S[ 5]]^S[ 9]]^S[13]];
         SBox2[j] = MDS2[Q1[Q0[Q0[Q0[j]^S[ 2]]^S[ 6]]^S[10]]^S[14]];
         SBox3[j] = MDS3[Q1[Q1[Q0[Q1[j]^S[ 3]]^S[ 7]]^S[11]]^S[15]];
         }
      for(u32bit j = 0; j != 40; j += 2)
         {
         u32bit X = MDS0[Q0[Q0[Q1[Q1[j  ]^key[24]]^key[16]]^key[ 8]]^key[ 0]] ^
                    MDS1[Q0[Q1[Q1[Q0[j  ]^key[25]]^key[17]]^key[ 9]]^key[ 1]] ^
                    MDS2[Q1[Q0[Q0[Q0[j  ]^key[26]]^key[18]]^key[10]]^key[ 2]] ^
                    MDS3[Q1[Q1[Q0[Q1[j  ]^key[27]]^key[19]]^key[11]]^key[ 3]];
         u32bit Y = MDS0[Q0[Q0[Q1[Q1[j+1]^key[28]]^key[20]]^key[12]]^key[ 4]] ^
                    MDS1[Q0[Q1[Q1[Q0[j+1]^key[29]]^key[21]]^key[13]]^key[ 5]] ^
                    MDS2[Q1[Q0[Q0[Q0[j+1]^key[30]]^key[22]]^key[14]]^key[ 6]] ^
                    MDS3[Q1[Q1[Q0[Q1[j+1]^key[31]]^key[23]]^key[15]]^key[ 7]];
         Y = rotate_left(Y, 8); X += Y; Y += X;
         round_key[j] = X; round_key[j+1] = rotate_left(Y, 9);
         }
      }
   }

/*************************************************
* Do one column of the RS matrix multiplcation   *
*************************************************/
void Twofish::rs_mul(byte S[4], byte key,
                     byte RS1, byte RS2, byte RS3, byte RS4)
   {
   if(key)
      {
      byte temp = POLY_TO_EXP[key - 1];
      S[0] ^= EXP_TO_POLY[(temp + POLY_TO_EXP[RS1-1])%255];
      S[1] ^= EXP_TO_POLY[(temp + POLY_TO_EXP[RS2-1])%255];
      S[2] ^= EXP_TO_POLY[(temp + POLY_TO_EXP[RS3-1])%255];
      S[3] ^= EXP_TO_POLY[(temp + POLY_TO_EXP[RS4-1])%255];
      }
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void Twofish::clear() throw()
   {
   SBox0.clear();
   SBox1.clear();
   SBox2.clear();
   SBox3.clear();
   round_key.clear();
   }

}
