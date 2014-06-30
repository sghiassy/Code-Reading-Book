/*************************************************
* Adler32 Source File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/adler32.h>

namespace OpenCL {

/*************************************************
* Adler32 Hash                                   *
*************************************************/
void Adler32::hash(const byte input[], u32bit length)
   {
   while(length >= 16)
      {
      S1 += input[ 0]; S2 += S1; S1 += input[ 1]; S2 += S1;
      S1 += input[ 2]; S2 += S1; S1 += input[ 3]; S2 += S1;
      S1 += input[ 4]; S2 += S1; S1 += input[ 5]; S2 += S1;
      S1 += input[ 6]; S2 += S1; S1 += input[ 7]; S2 += S1;
      S1 += input[ 8]; S2 += S1; S1 += input[ 9]; S2 += S1;
      S1 += input[10]; S2 += S1; S1 += input[11]; S2 += S1;
      S1 += input[12]; S2 += S1; S1 += input[13]; S2 += S1;
      S1 += input[14]; S2 += S1; S1 += input[15]; S2 += S1;
      input += 16;
      length -= 16;
      }
   for(u32bit j = 0; j != length; j++)
      {
      S1 += input[j]; S2 += S1;
      }
   S1 %= 65521;
   S2 %= 65521;
   }

/*************************************************
* Update an Adler32 Hash                         *
*************************************************/
void Adler32::update_hash(const byte input[], u32bit length)
   {
   static const u32bit PROCESS_AMOUNT = 5552;
   while(length >= PROCESS_AMOUNT)
      {
      hash(input, PROCESS_AMOUNT);
      input += PROCESS_AMOUNT;
      length -= PROCESS_AMOUNT;
      }
   hash(input, length);
   }

/*************************************************
* Finalize an Adler32 Hash                       *
*************************************************/
void Adler32::final(byte output[HASHLENGTH])
   {
   output[0] = get_byte(2, S2);
   output[1] = get_byte(3, S2);
   output[2] = get_byte(2, S1);
   output[3] = get_byte(3, S1);
   clear();
   }

}
