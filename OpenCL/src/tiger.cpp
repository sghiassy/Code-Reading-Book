/*************************************************
* Tiger Source File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/tiger.h>

namespace OpenCL {

/*************************************************
* Tiger Hash                                     *
*************************************************/
void Tiger::hash(const byte input[BLOCKSIZE])
   {
   for(u32bit j = 0; j != 8; j++)
      X[j] = make_u64bit(input[8*j+7], input[8*j+6], input[8*j+5],
                         input[8*j+4], input[8*j+3], input[8*j+2],
                         input[8*j+1], input[8*j]);
   u64bit A = digest[0], B = digest[1], C = digest[2];

   round(A, B, C, X[0], 5); round(B, C, A, X[1], 5); round(C, A, B, X[2], 5);
   round(A, B, C, X[3], 5); round(B, C, A, X[4], 5); round(C, A, B, X[5], 5);
   round(A, B, C, X[6], 5); round(B, C, A, X[7], 5);
   X[0] -= X[7] ^ 0xA5A5A5A5A5A5A5A5; X[1] ^= X[0];
   X[2] += X[1]; X[3] -= X[2] ^ ((~X[1]) << 19); X[4] ^= X[3];
   X[5] += X[4]; X[6] -= X[5] ^ ((~X[4]) >> 23); X[7] ^= X[6];
   X[0] += X[7]; X[1] -= X[0] ^ ((~X[7]) << 19); X[2] ^= X[1];
   X[3] += X[2]; X[4] -= X[3] ^ ((~X[2]) >> 23); X[5] ^= X[4];
   X[6] += X[5]; X[7] -= X[6] ^ 0x0123456789ABCDEF;
   round(C, A, B, X[0], 7); round(A, B, C, X[1], 7); round(B, C, A, X[2], 7);
   round(C, A, B, X[3], 7); round(A, B, C, X[4], 7); round(B, C, A, X[5], 7);
   round(C, A, B, X[6], 7); round(A, B, C, X[7], 7);
   X[0] -= X[7] ^ 0xA5A5A5A5A5A5A5A5; X[1] ^= X[0];
   X[2] += X[1]; X[3] -= X[2] ^ ((~X[1]) << 19); X[4] ^= X[3];
   X[5] += X[4]; X[6] -= X[5] ^ ((~X[4]) >> 23); X[7] ^= X[6];
   X[0] += X[7]; X[1] -= X[0] ^ ((~X[7]) << 19); X[2] ^= X[1];
   X[3] += X[2]; X[4] -= X[3] ^ ((~X[2]) >> 23); X[5] ^= X[4];
   X[6] += X[5]; X[7] -= X[6] ^ 0x0123456789ABCDEF;
   round(B, C, A, X[0], 9); round(C, A, B, X[1], 9); round(A, B, C, X[2], 9);
   round(B, C, A, X[3], 9); round(C, A, B, X[4], 9); round(A, B, C, X[5], 9);
   round(B, C, A, X[6], 9); round(C, A, B, X[7], 9);

   digest[0] ^= A; digest[1] = B - digest[1]; digest[2] += C;
   }

/*************************************************
* Update a Tiger Hash                            *
*************************************************/
void Tiger::update_hash(const byte input[], u32bit length)
   {
   count += length;
   buffer.copy(position, input, length);
   if(position + length >= BLOCKSIZE)
      {
      hash(buffer);
      input += (BLOCKSIZE - position);
      length -= (BLOCKSIZE - position);
      while(length >= BLOCKSIZE)
         {
         hash(input);
         input += BLOCKSIZE;
         length -= BLOCKSIZE;
         }
      buffer.copy(input, length);
      position = 0;
      }
   position += length;
   }

/*************************************************
* Tiger Round                                    *
*************************************************/
void Tiger::round(u64bit& A, u64bit& B, u64bit& C, u64bit msg, u32bit mul)
   {
   C ^= msg;
   A -= SBOX1[get_byte(7, C)] ^ SBOX2[get_byte(5, C)] ^
        SBOX3[get_byte(3, C)] ^ SBOX4[get_byte(1, C)];
   B += SBOX1[get_byte(0, C)] ^ SBOX2[get_byte(2, C)] ^
        SBOX3[get_byte(4, C)] ^ SBOX4[get_byte(6, C)];
   B *= mul;
   }

/*************************************************
* Finalize a Tiger Hash                          *
*************************************************/
void Tiger::final(byte output[HASHLENGTH])
   {
   buffer[position] = 0x01;
   for(u32bit j = position+1; j != BLOCKSIZE; j++)
      buffer[j] = 0;
   if(position >= BLOCKSIZE - 8)
      { hash(buffer); buffer.clear(); }
   for(u32bit j = BLOCKSIZE - 8; j != BLOCKSIZE; j++)
      buffer[j] = get_byte(7 - (j % 8), 8 * count);
   hash(buffer);
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(7 - (j % 8), digest[j/8]);
   clear();
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void Tiger::clear() throw()
   {
   X.clear();
   buffer.clear();
   digest[0] = 0x0123456789ABCDEF;
   digest[1] = 0xFEDCBA9876543210;
   digest[2] = 0xF096A5B4C3B2E187;
   count = position = 0;
   }

}
