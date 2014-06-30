/*************************************************
* CAST5 Source File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/cast5.h>

namespace OpenCL {

/*************************************************
* CAST5 Encryption                               *
*************************************************/
void CAST5::encrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit left  = make_u32bit(in[0], in[1], in[2], in[3]),
          right = make_u32bit(in[4], in[5], in[6], in[7]);
   round1(left, right, 0); round2(right, left, 1); round3(left, right, 2);
   round1(right, left, 3); round2(left, right, 4); round3(right, left, 5);
   round1(left, right, 6); round2(right, left, 7); round3(left, right, 8);
   round1(right, left, 9); round2(left, right,10); round3(right, left,11);
   round1(left, right,12); round2(right, left,13); round3(left, right,14);
   round1(right, left,15);
   out[0] = get_byte(0, right); out[1] = get_byte(1, right);
   out[2] = get_byte(2, right); out[3] = get_byte(3, right);
   out[4] = get_byte(0, left);  out[5] = get_byte(1, left);
   out[6] = get_byte(2, left);  out[7] = get_byte(3, left);
   }

/*************************************************
* CAST5 Decryption                               *
*************************************************/
void CAST5::decrypt(const byte in[BLOCKSIZE], byte out[BLOCKSIZE]) const
   {
   u32bit left  = make_u32bit(in[0], in[1], in[2], in[3]),
          right = make_u32bit(in[4], in[5], in[6], in[7]);
   round1(left, right,15); round3(right, left,14); round2(left, right,13);
   round1(right, left,12); round3(left, right,11); round2(right, left,10);
   round1(left, right, 9); round3(right, left, 8); round2(left, right, 7);
   round1(right, left, 6); round3(left, right, 5); round2(right, left, 4);
   round1(left, right, 3); round3(right, left, 2); round2(left, right, 1);
   round1(right, left, 0);
   out[0] = get_byte(0, right); out[1] = get_byte(1, right);
   out[2] = get_byte(2, right); out[3] = get_byte(3, right);
   out[4] = get_byte(0, left);  out[5] = get_byte(1, left);
   out[6] = get_byte(2, left);  out[7] = get_byte(3, left);
   }

/*************************************************
* CAST5 Round Type 1                             *
*************************************************/
void CAST5::round1(u32bit& left, u32bit right, u32bit round) const
   {
   u32bit temp = rotate_left(MK[round] + right, RK[round]);
   left ^= (CAST_SBOX1[get_byte(0, temp)] ^ CAST_SBOX2[get_byte(1, temp)]) -
            CAST_SBOX3[get_byte(2, temp)] + CAST_SBOX4[get_byte(3, temp)];
   }

/*************************************************
* CAST5 Round Type 2                             *
*************************************************/
void CAST5::round2(u32bit& left, u32bit right, u32bit round) const
   {
   u32bit temp = rotate_left(MK[round] ^ right, RK[round]);
   left ^= (CAST_SBOX1[get_byte(0, temp)]  - CAST_SBOX2[get_byte(1, temp)] +
            CAST_SBOX3[get_byte(2, temp)]) ^ CAST_SBOX4[get_byte(3, temp)];
   }

/*************************************************
* CAST5 Round Type 3                             *
*************************************************/
void CAST5::round3(u32bit& left, u32bit right, u32bit round) const
   {
   u32bit temp = rotate_left(MK[round] - right, RK[round]);
   left ^= ((CAST_SBOX1[get_byte(0, temp)]  + CAST_SBOX2[get_byte(1, temp)]) ^
             CAST_SBOX3[get_byte(2, temp)]) - CAST_SBOX4[get_byte(3, temp)];
   }

/*************************************************
* CAST5 Key Schedule                             *
*************************************************/
void CAST5::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   clear();
   SecureBuffer<u32bit, 4> X;
   for(u32bit j = 0; j != length; j++)
      X[j/4] = (X[j/4] << 8) + key[j];
   key_schedule(MK, X);
   key_schedule(RK, X);
   for(u32bit j = 0; j != 16; j++)
      RK[j] %= 32;
   }

/*************************************************
* S-Box Based Key Expansion                      *
*************************************************/
void CAST5::key_schedule(SecureBuffer<u32bit, 16>& K,
                         SecureBuffer<u32bit,  4>& X)
   {
   SecureBuffer<u32bit, 4> Z;

   #define x(num) get_byte(num%4, X[num/4])
   #define z(num) get_byte(num%4, Z[num/4])

   Z[0]  = X[0] ^ S5[x(13)] ^ S6[x(15)] ^ S7[x(12)] ^ S8[x(14)] ^ S7[x( 8)];
   Z[1]  = X[2] ^ S5[z( 0)] ^ S6[z( 2)] ^ S7[z( 1)] ^ S8[z( 3)] ^ S8[x(10)];
   Z[2]  = X[3] ^ S5[z( 7)] ^ S6[z( 6)] ^ S7[z( 5)] ^ S8[z( 4)] ^ S5[x( 9)];
   Z[3]  = X[1] ^ S5[z(10)] ^ S6[z( 9)] ^ S7[z(11)] ^ S8[z( 8)] ^ S6[x(11)];
   K[ 0] = S5[z( 8)] ^ S6[z( 9)] ^ S7[z( 7)] ^ S8[z( 6)] ^ S5[z( 2)];
   K[ 1] = S5[z(10)] ^ S6[z(11)] ^ S7[z( 5)] ^ S8[z( 4)] ^ S6[z( 6)];
   K[ 2] = S5[z(12)] ^ S6[z(13)] ^ S7[z( 3)] ^ S8[z( 2)] ^ S7[z( 9)];
   K[ 3] = S5[z(14)] ^ S6[z(15)] ^ S7[z( 1)] ^ S8[z( 0)] ^ S8[z(12)];
   X[0]  = Z[2] ^ S5[z( 5)] ^ S6[z( 7)] ^ S7[z( 4)] ^ S8[z( 6)] ^ S7[z( 0)];
   X[1]  = Z[0] ^ S5[x( 0)] ^ S6[x( 2)] ^ S7[x( 1)] ^ S8[x( 3)] ^ S8[z( 2)];
   X[2]  = Z[1] ^ S5[x( 7)] ^ S6[x( 6)] ^ S7[x( 5)] ^ S8[x( 4)] ^ S5[z( 1)];
   X[3]  = Z[3] ^ S5[x(10)] ^ S6[x( 9)] ^ S7[x(11)] ^ S8[x( 8)] ^ S6[z( 3)];
   K[ 4] = S5[x( 3)] ^ S6[x( 2)] ^ S7[x(12)] ^ S8[x(13)] ^ S5[x( 8)];
   K[ 5] = S5[x( 1)] ^ S6[x( 0)] ^ S7[x(14)] ^ S8[x(15)] ^ S6[x(13)];
   K[ 6] = S5[x( 7)] ^ S6[x( 6)] ^ S7[x( 8)] ^ S8[x( 9)] ^ S7[x( 3)];
   K[ 7] = S5[x( 5)] ^ S6[x( 4)] ^ S7[x(10)] ^ S8[x(11)] ^ S8[x( 7)];
   Z[0]  = X[0] ^ S5[x(13)] ^ S6[x(15)] ^ S7[x(12)] ^ S8[x(14)] ^ S7[x( 8)];
   Z[1]  = X[2] ^ S5[z( 0)] ^ S6[z( 2)] ^ S7[z( 1)] ^ S8[z( 3)] ^ S8[x(10)];
   Z[2]  = X[3] ^ S5[z( 7)] ^ S6[z( 6)] ^ S7[z( 5)] ^ S8[z( 4)] ^ S5[x( 9)];
   Z[3]  = X[1] ^ S5[z(10)] ^ S6[z( 9)] ^ S7[z(11)] ^ S8[z( 8)] ^ S6[x(11)];
   K[ 8] = S5[z( 3)] ^ S6[z( 2)] ^ S7[z(12)] ^ S8[z(13)] ^ S5[z( 9)];
   K[ 9] = S5[z( 1)] ^ S6[z( 0)] ^ S7[z(14)] ^ S8[z(15)] ^ S6[z(12)];
   K[10] = S5[z( 7)] ^ S6[z( 6)] ^ S7[z( 8)] ^ S8[z( 9)] ^ S7[z( 2)];
   K[11] = S5[z( 5)] ^ S6[z( 4)] ^ S7[z(10)] ^ S8[z(11)] ^ S8[z( 6)];
   X[0]  = Z[2] ^ S5[z( 5)] ^ S6[z( 7)] ^ S7[z( 4)] ^ S8[z( 6)] ^ S7[z( 0)];
   X[1]  = Z[0] ^ S5[x( 0)] ^ S6[x( 2)] ^ S7[x( 1)] ^ S8[x( 3)] ^ S8[z( 2)];
   X[2]  = Z[1] ^ S5[x( 7)] ^ S6[x( 6)] ^ S7[x( 5)] ^ S8[x( 4)] ^ S5[z( 1)];
   X[3]  = Z[3] ^ S5[x(10)] ^ S6[x( 9)] ^ S7[x(11)] ^ S8[x( 8)] ^ S6[z( 3)];
   K[12] = S5[x( 8)] ^ S6[x( 9)] ^ S7[x( 7)] ^ S8[x( 6)] ^ S5[x( 3)];
   K[13] = S5[x(10)] ^ S6[x(11)] ^ S7[x( 5)] ^ S8[x( 4)] ^ S6[x( 7)];
   K[14] = S5[x(12)] ^ S6[x(13)] ^ S7[x( 3)] ^ S8[x( 2)] ^ S7[x( 8)];
   K[15] = S5[x(14)] ^ S6[x(15)] ^ S7[x( 1)] ^ S8[x( 0)] ^ S8[x(13)];

   #undef x
   #undef z
   }

}
