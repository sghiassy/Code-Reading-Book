/*************************************************
* MD2 Source File                                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/md2.h>

namespace OpenCL {

/*************************************************
* MD2 Hash                                       *
*************************************************/
void MD2::hash(const byte input[BLOCKSIZE])
   {
   X.copy(16, input, BLOCKSIZE);
   xor_buf(X + 32, X, X + 16, BLOCKSIZE);
   byte T = 0;
   for(u32bit j = 0; j != 18; j++)
      {
      for(u32bit k = 0; k != 48; k += 8)
         {
         T = X[k  ] ^= SBOX[T]; T = X[k+1] ^= SBOX[T];
         T = X[k+2] ^= SBOX[T]; T = X[k+3] ^= SBOX[T];
         T = X[k+4] ^= SBOX[T]; T = X[k+5] ^= SBOX[T];
         T = X[k+6] ^= SBOX[T]; T = X[k+7] ^= SBOX[T];
         }
      T += j;
      }
   T = checksum[15];
   for(u32bit j = 0; j != BLOCKSIZE; j++)
      T = checksum[j] ^= SBOX[input[j] ^ T];
   }

/*************************************************
* Update a MD2 Hash                              *
*************************************************/
void MD2::update_hash(const byte input[], u32bit length)
   {
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
* Finalize a MD2 Hash                            *
*************************************************/
void MD2::final(byte output[HASHLENGTH])
   {
   for(u32bit j = position; j != BLOCKSIZE; j++)
      buffer[j] = (byte)(BLOCKSIZE - position);
   hash(buffer);
   hash(checksum);
   copy_mem(output, X.ptr(), HASHLENGTH);
   clear();
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void MD2::clear() throw()
   {
   X.clear();
   checksum.clear();
   buffer.clear();
   position = 0;
   }

}
