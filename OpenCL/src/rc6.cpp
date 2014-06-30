/*************************************************
* RC6 Source File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/rc6.h>

namespace OpenCL {

/*************************************************
* RC6 Encryption                                 *
*************************************************/
void RC6::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit A = make_u32bit(in[ 3], in[ 2], in[ 1], in[ 0]),
          B = make_u32bit(in[ 7], in[ 6], in[ 5], in[ 4]),
          C = make_u32bit(in[11], in[10], in[ 9], in[ 8]),
          D = make_u32bit(in[15], in[14], in[13], in[12]);
   B += S[0]; D += S[1];      encrypt_round(A,B,C,D, 1);
   encrypt_round(B,C,D,A, 2); encrypt_round(C,D,A,B, 3);
   encrypt_round(D,A,B,C, 4); encrypt_round(A,B,C,D, 5);
   encrypt_round(B,C,D,A, 6); encrypt_round(C,D,A,B, 7);
   encrypt_round(D,A,B,C, 8); encrypt_round(A,B,C,D, 9);
   encrypt_round(B,C,D,A,10); encrypt_round(C,D,A,B,11);
   encrypt_round(D,A,B,C,12); encrypt_round(A,B,C,D,13);
   encrypt_round(B,C,D,A,14); encrypt_round(C,D,A,B,15);
   encrypt_round(D,A,B,C,16); encrypt_round(A,B,C,D,17);
   encrypt_round(B,C,D,A,18); encrypt_round(C,D,A,B,19);
   encrypt_round(D,A,B,C,20); A += S[42]; C += S[43];
   out[ 0] = get_byte(3, A); out[ 1] = get_byte(2, A);
   out[ 2] = get_byte(1, A); out[ 3] = get_byte(0, A);
   out[ 4] = get_byte(3, B); out[ 5] = get_byte(2, B);
   out[ 6] = get_byte(1, B); out[ 7] = get_byte(0, B);
   out[ 8] = get_byte(3, C); out[ 9] = get_byte(2, C);
   out[10] = get_byte(1, C); out[11] = get_byte(0, C);
   out[12] = get_byte(3, D); out[13] = get_byte(2, D);
   out[14] = get_byte(1, D); out[15] = get_byte(0, D);
   }

/*************************************************
* RC6 Decryption                                 *
*************************************************/
void RC6::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit A = make_u32bit(in[ 3], in[ 2], in[ 1], in[ 0]),
          B = make_u32bit(in[ 7], in[ 6], in[ 5], in[ 4]),
          C = make_u32bit(in[11], in[10], in[ 9], in[ 8]),
          D = make_u32bit(in[15], in[14], in[13], in[12]);
   C -= S[43]; A -= S[42];    decrypt_round(D,A,B,C,20);
   decrypt_round(C,D,A,B,19); decrypt_round(B,C,D,A,18);
   decrypt_round(A,B,C,D,17); decrypt_round(D,A,B,C,16);
   decrypt_round(C,D,A,B,15); decrypt_round(B,C,D,A,14);
   decrypt_round(A,B,C,D,13); decrypt_round(D,A,B,C,12);
   decrypt_round(C,D,A,B,11); decrypt_round(B,C,D,A,10);
   decrypt_round(A,B,C,D, 9); decrypt_round(D,A,B,C, 8);
   decrypt_round(C,D,A,B, 7); decrypt_round(B,C,D,A, 6);
   decrypt_round(A,B,C,D, 5); decrypt_round(D,A,B,C, 4);
   decrypt_round(C,D,A,B, 3); decrypt_round(B,C,D,A, 2);
   decrypt_round(A,B,C,D, 1); D -= S[1]; B -= S[0];
   out[ 0] = get_byte(3, A); out[ 1] = get_byte(2, A);
   out[ 2] = get_byte(1, A); out[ 3] = get_byte(0, A);
   out[ 4] = get_byte(3, B); out[ 5] = get_byte(2, B);
   out[ 6] = get_byte(1, B); out[ 7] = get_byte(0, B);
   out[ 8] = get_byte(3, C); out[ 9] = get_byte(2, C);
   out[10] = get_byte(1, C); out[11] = get_byte(0, C);
   out[12] = get_byte(3, D); out[13] = get_byte(2, D);
   out[14] = get_byte(1, D); out[15] = get_byte(0, D);
   }

/*************************************************
* RC6 Encryption Round                           *
*************************************************/
void RC6::encrypt_round(u32bit& A, u32bit B, u32bit& C,
                        u32bit D, u32bit round) const
   {
   u32bit T1 = rotate_left(B*(2*B+1), 5);
   u32bit T2 = rotate_left(D*(2*D+1), 5);
   A = rotate_left(A ^ T1, T2 % 32) + S[2*round];
   C = rotate_left(C ^ T2, T1 % 32) + S[2*round+1];
   }

/*************************************************
* RC6 Decryption Round                           *
*************************************************/
void RC6::decrypt_round(u32bit& A, u32bit B, u32bit& C,
                        u32bit D, u32bit round) const
   {
   u32bit T1 = rotate_left(B*(2*B+1), 5);
   u32bit T2 = rotate_left(D*(2*D+1), 5);
   C = rotate_right(C - S[2*round+1], T1 % 32) ^ T2;
   A = rotate_right(A - S[2*round  ], T2 % 32) ^ T1;
   }

/*************************************************
* RC6 Key Schedule                               *
*************************************************/
void RC6::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   const u32bit WORD_KEYLENGTH = (((length - 1) / 4) + 1),
                MIX_ROUNDS     = 3*std::max(WORD_KEYLENGTH, S.size());
   S[0] = 0xB7E15163;
   for(u32bit j = 1; j != TABLESIZE; j++)
      S[j] = S[j-1] + 0x9E3779B9;

   SecureBuffer<u32bit, 8> K;
   for(s32bit j = length-1; j >= 0; j--)
      K[j/4] = (K[j/4] << 8) + key[j];
   for(u32bit j = 0, A = 0, B = 0; j != MIX_ROUNDS; j++)
      {
      A = rotate_left(S[j % TABLESIZE] + A + B, 3);
      B = rotate_left(K[j % WORD_KEYLENGTH] + A + B, (A + B) % 32);
      S[j % TABLESIZE] = A;
      K[j % WORD_KEYLENGTH] = B;
      }
   }

}
