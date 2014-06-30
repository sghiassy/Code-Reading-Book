/*************************************************
* MD4 Source File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/md4.h>

namespace OpenCL {

/*************************************************
* MD4 Hash                                       *
*************************************************/
void MD4::hash(const byte input[BLOCKSIZE])
   {
   for(u32bit j = 0; j != 16; j++)
      M[j] = make_u32bit(input[4*j+3], input[4*j+2], input[4*j+1], input[4*j]);

   u32bit A = digest[0], B = digest[1], C = digest[2], D = digest[3];

   FF(A,B,C,D,M[ 0], 3);   FF(D,A,B,C,M[ 1], 7);   FF(C,D,A,B,M[ 2],11);
   FF(B,C,D,A,M[ 3],19);   FF(A,B,C,D,M[ 4], 3);   FF(D,A,B,C,M[ 5], 7);
   FF(C,D,A,B,M[ 6],11);   FF(B,C,D,A,M[ 7],19);   FF(A,B,C,D,M[ 8], 3);
   FF(D,A,B,C,M[ 9], 7);   FF(C,D,A,B,M[10],11);   FF(B,C,D,A,M[11],19);
   FF(A,B,C,D,M[12], 3);   FF(D,A,B,C,M[13], 7);   FF(C,D,A,B,M[14],11);
   FF(B,C,D,A,M[15],19);

   GG(A,B,C,D,M[ 0], 3);   GG(D,A,B,C,M[ 4], 5);   GG(C,D,A,B,M[ 8], 9);
   GG(B,C,D,A,M[12],13);   GG(A,B,C,D,M[ 1], 3);   GG(D,A,B,C,M[ 5], 5);
   GG(C,D,A,B,M[ 9], 9);   GG(B,C,D,A,M[13],13);   GG(A,B,C,D,M[ 2], 3);
   GG(D,A,B,C,M[ 6], 5);   GG(C,D,A,B,M[10], 9);   GG(B,C,D,A,M[14],13);
   GG(A,B,C,D,M[ 3], 3);   GG(D,A,B,C,M[ 7], 5);   GG(C,D,A,B,M[11], 9);
   GG(B,C,D,A,M[15],13);

   HH(A,B,C,D,M[ 0], 3);   HH(D,A,B,C,M[ 8], 9);   HH(C,D,A,B,M[ 4],11);
   HH(B,C,D,A,M[12],15);   HH(A,B,C,D,M[ 2], 3);   HH(D,A,B,C,M[10], 9);
   HH(C,D,A,B,M[ 6],11);   HH(B,C,D,A,M[14],15);   HH(A,B,C,D,M[ 1], 3);
   HH(D,A,B,C,M[ 9], 9);   HH(C,D,A,B,M[ 5],11);   HH(B,C,D,A,M[13],15);
   HH(A,B,C,D,M[ 3], 3);   HH(D,A,B,C,M[11], 9);   HH(C,D,A,B,M[ 7],11);
   HH(B,C,D,A,M[15],15);

   digest[0] += A;   digest[1] += B;   digest[2] += C;   digest[3] += D;
   }

/*************************************************
* MD4 FF Function                                *
*************************************************/
void MD4::FF(u32bit& A, u32bit B, u32bit C, u32bit D, u32bit msg, u32bit shift)
   {
   A += (D ^ (B & (C ^ D))) + msg;
   A  = rotate_left(A, shift);
   }

/*************************************************
* MD4 GG Function                                *
*************************************************/
void MD4::GG(u32bit& A, u32bit B, u32bit C, u32bit D, u32bit msg, u32bit shift)
   {
   A += ((B & C) | (D & (B | C))) + msg + 0x5A827999;
   A  = rotate_left(A, shift);
   }

/*************************************************
* MD4 HH Function                                *
*************************************************/
void MD4::HH(u32bit& A, u32bit B, u32bit C, u32bit D, u32bit msg, u32bit shift)
   {
   A += (B ^ C ^ D) + msg + 0x6ED9EBA1;
   A  = rotate_left(A, shift);
   }

/*************************************************
* Update a MD4 Hash                              *
*************************************************/
void MD4::update_hash(const byte input[], u32bit length)
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
* Finalize a MD4 Hash                            *
*************************************************/
void MD4::final(byte output[HASHLENGTH])
   {
   buffer[position] = 0x80;
   for(u32bit j = position+1; j != BLOCKSIZE; j++)
      buffer[j] = 0;
   if(position >= BLOCKSIZE - 8)
      { hash(buffer); buffer.clear(); }
   for(u32bit j = BLOCKSIZE - 8; j != BLOCKSIZE; j++)
      buffer[j] = get_byte(7 - (j % 8), 8 * count);
   hash(buffer);
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(3 - (j % 4), digest[j/4]);
   clear();
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void MD4::clear() throw()
   {
   M.clear();
   buffer.clear();
   digest[0] = 0x67452301;
   digest[1] = 0xEFCDAB89;
   digest[2] = 0x98BADCFE;
   digest[3] = 0x10325476;
   count = position = 0;
   }

}
