/*************************************************
* SHA1 Source File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/sha1.h>

namespace OpenCL {

/*************************************************
* SHA1 Hash                                      *
*************************************************/
void SHA1::hash(const byte input[BLOCKSIZE])
   {
   for(u32bit j = 0; j != 16; j++)
      W[j] = make_u32bit(input[4*j], input[4*j+1], input[4*j+2], input[4*j+3]);
   for(u32bit j = 16; j != 80; j++)
      W[j] = rotate_left((W[j-3] ^ W[j-8] ^ W[j-14] ^ W[j-16]), 1);

   u32bit A = digest[0], B = digest[1], C = digest[2],
          D = digest[3], E = digest[4];

   F1(A,B,C,D,E,W[ 0]);   F1(E,A,B,C,D,W[ 1]);   F1(D,E,A,B,C,W[ 2]);
   F1(C,D,E,A,B,W[ 3]);   F1(B,C,D,E,A,W[ 4]);   F1(A,B,C,D,E,W[ 5]);
   F1(E,A,B,C,D,W[ 6]);   F1(D,E,A,B,C,W[ 7]);   F1(C,D,E,A,B,W[ 8]);
   F1(B,C,D,E,A,W[ 9]);   F1(A,B,C,D,E,W[10]);   F1(E,A,B,C,D,W[11]);
   F1(D,E,A,B,C,W[12]);   F1(C,D,E,A,B,W[13]);   F1(B,C,D,E,A,W[14]);
   F1(A,B,C,D,E,W[15]);   F1(E,A,B,C,D,W[16]);   F1(D,E,A,B,C,W[17]);
   F1(C,D,E,A,B,W[18]);   F1(B,C,D,E,A,W[19]);

   F2(A,B,C,D,E,W[20]);   F2(E,A,B,C,D,W[21]);   F2(D,E,A,B,C,W[22]);
   F2(C,D,E,A,B,W[23]);   F2(B,C,D,E,A,W[24]);   F2(A,B,C,D,E,W[25]);
   F2(E,A,B,C,D,W[26]);   F2(D,E,A,B,C,W[27]);   F2(C,D,E,A,B,W[28]);
   F2(B,C,D,E,A,W[29]);   F2(A,B,C,D,E,W[30]);   F2(E,A,B,C,D,W[31]);
   F2(D,E,A,B,C,W[32]);   F2(C,D,E,A,B,W[33]);   F2(B,C,D,E,A,W[34]);
   F2(A,B,C,D,E,W[35]);   F2(E,A,B,C,D,W[36]);   F2(D,E,A,B,C,W[37]);
   F2(C,D,E,A,B,W[38]);   F2(B,C,D,E,A,W[39]);

   F3(A,B,C,D,E,W[40]);   F3(E,A,B,C,D,W[41]);   F3(D,E,A,B,C,W[42]);
   F3(C,D,E,A,B,W[43]);   F3(B,C,D,E,A,W[44]);   F3(A,B,C,D,E,W[45]);
   F3(E,A,B,C,D,W[46]);   F3(D,E,A,B,C,W[47]);   F3(C,D,E,A,B,W[48]);
   F3(B,C,D,E,A,W[49]);   F3(A,B,C,D,E,W[50]);   F3(E,A,B,C,D,W[51]);
   F3(D,E,A,B,C,W[52]);   F3(C,D,E,A,B,W[53]);   F3(B,C,D,E,A,W[54]);
   F3(A,B,C,D,E,W[55]);   F3(E,A,B,C,D,W[56]);   F3(D,E,A,B,C,W[57]);
   F3(C,D,E,A,B,W[58]);   F3(B,C,D,E,A,W[59]);

   F4(A,B,C,D,E,W[60]);   F4(E,A,B,C,D,W[61]);   F4(D,E,A,B,C,W[62]);
   F4(C,D,E,A,B,W[63]);   F4(B,C,D,E,A,W[64]);   F4(A,B,C,D,E,W[65]);
   F4(E,A,B,C,D,W[66]);   F4(D,E,A,B,C,W[67]);   F4(C,D,E,A,B,W[68]);
   F4(B,C,D,E,A,W[69]);   F4(A,B,C,D,E,W[70]);   F4(E,A,B,C,D,W[71]);
   F4(D,E,A,B,C,W[72]);   F4(C,D,E,A,B,W[73]);   F4(B,C,D,E,A,W[74]);
   F4(A,B,C,D,E,W[75]);   F4(E,A,B,C,D,W[76]);   F4(D,E,A,B,C,W[77]);
   F4(C,D,E,A,B,W[78]);   F4(B,C,D,E,A,W[79]);

   digest[0] += A; digest[1] += B; digest[2] += C;
   digest[3] += D; digest[4] += E;
   }

/*************************************************
* SHA1 F1 Function                               *
*************************************************/
void SHA1::F1(u32bit A, u32bit& B, u32bit C, u32bit D, u32bit& E, u32bit msg)
   {
   E += rotate_left(A, 5) + (D ^ (B & (C ^ D))) + msg + 0x5A827999;
   B  = rotate_left(B, 30);
   }

/*************************************************
* SHA1 F2 Function                               *
*************************************************/
void SHA1::F2(u32bit A, u32bit& B, u32bit C, u32bit D, u32bit& E, u32bit msg)
   {
   E += rotate_left(A, 5) + (B ^ C ^ D) + msg + 0x6ED9EBA1;
   B  = rotate_left(B, 30);
   }

/*************************************************
* SHA1 F3 Function                               *
*************************************************/
void SHA1::F3(u32bit A, u32bit& B, u32bit C, u32bit D, u32bit& E, u32bit msg)
   {
   E += rotate_left(A, 5) + ((B & C) | ((B | C) & D)) + msg + 0x8F1BBCDC;
   B  = rotate_left(B, 30);
   }

/*************************************************
* SHA1 F4 Function                               *
*************************************************/
void SHA1::F4(u32bit A, u32bit& B, u32bit C, u32bit D, u32bit& E, u32bit msg)
   {
   E += rotate_left(A, 5) + (B ^ C ^ D) + msg + 0xCA62C1D6;
   B  = rotate_left(B, 30);
   }

/*************************************************
* Update a SHA1 Hash                             *
*************************************************/
void SHA1::update_hash(const byte input[], u32bit length)
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
* Finalize a SHA1 Hash                           *
*************************************************/
void SHA1::final(byte output[HASHLENGTH])
   {
   buffer[position] = 0x80;
   for(u32bit j = position+1; j != BLOCKSIZE; j++)
      buffer[j] = 0;
   if(position >= BLOCKSIZE - 8)
      { hash(buffer); buffer.clear(); }
   for(u32bit j = BLOCKSIZE - 8; j != BLOCKSIZE; j++)
      buffer[j] = get_byte(j % 8, 8 * count);
   hash(buffer);
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(j % 4, digest[j/4]);
   clear();
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void SHA1::clear() throw()
   {
   W.clear();
   buffer.clear();
   digest[0] = 0x67452301;
   digest[1] = 0xEFCDAB89;
   digest[2] = 0x98BADCFE;
   digest[3] = 0x10325476;
   digest[4] = 0xC3D2E1F0;
   count = position = 0;
   }

}
