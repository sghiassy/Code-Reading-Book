/*************************************************
* SHA2-512 Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/sha512.h>

namespace OpenCL {

/*************************************************
* SHA2-512 Hash                                  *
*************************************************/
void SHA2_512::hash(const byte input[BLOCKSIZE])
   {
   for(u32bit j = 0; j != 16; j++)
      W[j] = make_u64bit(input[8*j  ], input[8*j+1], input[8*j+2],
                         input[8*j+3], input[8*j+4], input[8*j+5],
                         input[8*j+6], input[8*j+7]);
   for(u32bit j = 16; j != 80; j++)
      W[j] = sigma(W[j- 2], 19, 61,  6) + W[j- 7] +
             sigma(W[j-15],  1,  8,  7) + W[j-16];

   u64bit A = digest[0], B = digest[1], C = digest[2],
          D = digest[3], E = digest[4], F = digest[5],
          G = digest[6], H = digest[7];

   F1(A,B,C,D,E,F,G,H,W[ 0],MAGIC[ 0]);   F1(H,A,B,C,D,E,F,G,W[ 1],MAGIC[ 1]);
   F1(G,H,A,B,C,D,E,F,W[ 2],MAGIC[ 2]);   F1(F,G,H,A,B,C,D,E,W[ 3],MAGIC[ 3]);
   F1(E,F,G,H,A,B,C,D,W[ 4],MAGIC[ 4]);   F1(D,E,F,G,H,A,B,C,W[ 5],MAGIC[ 5]);
   F1(C,D,E,F,G,H,A,B,W[ 6],MAGIC[ 6]);   F1(B,C,D,E,F,G,H,A,W[ 7],MAGIC[ 7]);
   F1(A,B,C,D,E,F,G,H,W[ 8],MAGIC[ 8]);   F1(H,A,B,C,D,E,F,G,W[ 9],MAGIC[ 9]);
   F1(G,H,A,B,C,D,E,F,W[10],MAGIC[10]);   F1(F,G,H,A,B,C,D,E,W[11],MAGIC[11]);
   F1(E,F,G,H,A,B,C,D,W[12],MAGIC[12]);   F1(D,E,F,G,H,A,B,C,W[13],MAGIC[13]);
   F1(C,D,E,F,G,H,A,B,W[14],MAGIC[14]);   F1(B,C,D,E,F,G,H,A,W[15],MAGIC[15]);
   F1(A,B,C,D,E,F,G,H,W[16],MAGIC[16]);   F1(H,A,B,C,D,E,F,G,W[17],MAGIC[17]);
   F1(G,H,A,B,C,D,E,F,W[18],MAGIC[18]);   F1(F,G,H,A,B,C,D,E,W[19],MAGIC[19]);
   F1(E,F,G,H,A,B,C,D,W[20],MAGIC[20]);   F1(D,E,F,G,H,A,B,C,W[21],MAGIC[21]);
   F1(C,D,E,F,G,H,A,B,W[22],MAGIC[22]);   F1(B,C,D,E,F,G,H,A,W[23],MAGIC[23]);
   F1(A,B,C,D,E,F,G,H,W[24],MAGIC[24]);   F1(H,A,B,C,D,E,F,G,W[25],MAGIC[25]);
   F1(G,H,A,B,C,D,E,F,W[26],MAGIC[26]);   F1(F,G,H,A,B,C,D,E,W[27],MAGIC[27]);
   F1(E,F,G,H,A,B,C,D,W[28],MAGIC[28]);   F1(D,E,F,G,H,A,B,C,W[29],MAGIC[29]);
   F1(C,D,E,F,G,H,A,B,W[30],MAGIC[30]);   F1(B,C,D,E,F,G,H,A,W[31],MAGIC[31]);
   F1(A,B,C,D,E,F,G,H,W[32],MAGIC[32]);   F1(H,A,B,C,D,E,F,G,W[33],MAGIC[33]);
   F1(G,H,A,B,C,D,E,F,W[34],MAGIC[34]);   F1(F,G,H,A,B,C,D,E,W[35],MAGIC[35]);
   F1(E,F,G,H,A,B,C,D,W[36],MAGIC[36]);   F1(D,E,F,G,H,A,B,C,W[37],MAGIC[37]);
   F1(C,D,E,F,G,H,A,B,W[38],MAGIC[38]);   F1(B,C,D,E,F,G,H,A,W[39],MAGIC[39]);
   F1(A,B,C,D,E,F,G,H,W[40],MAGIC[40]);   F1(H,A,B,C,D,E,F,G,W[41],MAGIC[41]);
   F1(G,H,A,B,C,D,E,F,W[42],MAGIC[42]);   F1(F,G,H,A,B,C,D,E,W[43],MAGIC[43]);
   F1(E,F,G,H,A,B,C,D,W[44],MAGIC[44]);   F1(D,E,F,G,H,A,B,C,W[45],MAGIC[45]);
   F1(C,D,E,F,G,H,A,B,W[46],MAGIC[46]);   F1(B,C,D,E,F,G,H,A,W[47],MAGIC[47]);
   F1(A,B,C,D,E,F,G,H,W[48],MAGIC[48]);   F1(H,A,B,C,D,E,F,G,W[49],MAGIC[49]);
   F1(G,H,A,B,C,D,E,F,W[50],MAGIC[50]);   F1(F,G,H,A,B,C,D,E,W[51],MAGIC[51]);
   F1(E,F,G,H,A,B,C,D,W[52],MAGIC[52]);   F1(D,E,F,G,H,A,B,C,W[53],MAGIC[53]);
   F1(C,D,E,F,G,H,A,B,W[54],MAGIC[54]);   F1(B,C,D,E,F,G,H,A,W[55],MAGIC[55]);
   F1(A,B,C,D,E,F,G,H,W[56],MAGIC[56]);   F1(H,A,B,C,D,E,F,G,W[57],MAGIC[57]);
   F1(G,H,A,B,C,D,E,F,W[58],MAGIC[58]);   F1(F,G,H,A,B,C,D,E,W[59],MAGIC[59]);
   F1(E,F,G,H,A,B,C,D,W[60],MAGIC[60]);   F1(D,E,F,G,H,A,B,C,W[61],MAGIC[61]);
   F1(C,D,E,F,G,H,A,B,W[62],MAGIC[62]);   F1(B,C,D,E,F,G,H,A,W[63],MAGIC[63]);
   F1(A,B,C,D,E,F,G,H,W[64],MAGIC[64]);   F1(H,A,B,C,D,E,F,G,W[65],MAGIC[65]);
   F1(G,H,A,B,C,D,E,F,W[66],MAGIC[66]);   F1(F,G,H,A,B,C,D,E,W[67],MAGIC[67]);
   F1(E,F,G,H,A,B,C,D,W[68],MAGIC[68]);   F1(D,E,F,G,H,A,B,C,W[69],MAGIC[69]);
   F1(C,D,E,F,G,H,A,B,W[70],MAGIC[70]);   F1(B,C,D,E,F,G,H,A,W[71],MAGIC[71]);
   F1(A,B,C,D,E,F,G,H,W[72],MAGIC[72]);   F1(H,A,B,C,D,E,F,G,W[73],MAGIC[73]);
   F1(G,H,A,B,C,D,E,F,W[74],MAGIC[74]);   F1(F,G,H,A,B,C,D,E,W[75],MAGIC[75]);
   F1(E,F,G,H,A,B,C,D,W[76],MAGIC[76]);   F1(D,E,F,G,H,A,B,C,W[77],MAGIC[77]);
   F1(C,D,E,F,G,H,A,B,W[78],MAGIC[78]);   F1(B,C,D,E,F,G,H,A,W[79],MAGIC[79]);

   digest[0] += A; digest[1] += B; digest[2] += C;
   digest[3] += D; digest[4] += E; digest[5] += F;
   digest[6] += G; digest[7] += H;
   }

/*************************************************
* SHA2-512 Compression Function                  *
*************************************************/
void SHA2_512::F1(u64bit A, u64bit B, u64bit C, u64bit& D,
                  u64bit E, u64bit F, u64bit G, u64bit& H,
                  u64bit msg, u64bit magic)
   {
   magic += rho(E, 14, 18, 41) + ((E & F) ^ (~E & G)) + msg;
   D += magic + H;
   H += magic + rho(A, 28, 34, 39) + ((A & B) ^ (A & C) ^ (B & C));
   }

/*************************************************
* SHA2-512 Rho Function                          *
*************************************************/
u64bit SHA2_512::rho(u64bit X, u32bit rot1, u32bit rot2, u32bit rot3)
   {
   return (rotate_right(X, rot1) ^ rotate_right(X, rot2) ^
           rotate_right(X, rot3));
   }

/*************************************************
* SHA2-512 Sigma Function                        *
*************************************************/
u64bit SHA2_512::sigma(u64bit X, u32bit rot1, u32bit rot2, u32bit shift)
   {
   return (rotate_right(X, rot1) ^ rotate_right(X, rot2) ^ (X >> shift));
   }

/*************************************************
* Update a SHA2-512 Hash                         *
*************************************************/
void SHA2_512::update_hash(const byte input[], u32bit length)
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
* Finalize a SHA2-512 Hash                       *
*************************************************/
void SHA2_512::final(byte output[HASHLENGTH])
   {
   buffer[position] = 0x80;
   for(u32bit j = position+1; j != BLOCKSIZE; j++)
      buffer[j] = 0;
   if(position >= BLOCKSIZE - 16)
      { hash(buffer); buffer.clear(); }
   for(u32bit j = BLOCKSIZE - 8; j != BLOCKSIZE; j++)
      buffer[j] = get_byte(j % 8, 8 * count);
   hash(buffer);
   for(u32bit j = 0; j != HASHLENGTH; j++)
      output[j] = get_byte(j % 8, digest[j/8]);
   clear();
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void SHA2_512::clear() throw()
   {
   W.clear();
   buffer.clear();
   digest[0] = 0x6A09E667F3BCC908;
   digest[1] = 0xBB67AE8584CAA73B;
   digest[2] = 0x3C6EF372FE94F82B;
   digest[3] = 0xA54FF53A5F1D36F1;
   digest[4] = 0x510E527FADE682D1;
   digest[5] = 0x9B05688C2B3E6C1F;
   digest[6] = 0x1F83D9ABFB41BD6B;
   digest[7] = 0x5BE0CD19137E2179;
   count = position = 0;
   }

}
