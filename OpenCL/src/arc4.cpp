/*************************************************
* ARC4 Source File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/arc4.h>

namespace OpenCL {

/*************************************************
* Generate ARC4 Cipher Stream                    *
*************************************************/
void ARC4::cipher(const byte in[], byte out[], u32bit length)
   {
   while(length >= BUFFERSIZE - position)
      {
      xor_buf(out, in, buffer + position, BUFFERSIZE - position);
      length -= (BUFFERSIZE - position);
      in += (BUFFERSIZE - position);
      out += (BUFFERSIZE - position);
      generate();
      }
   xor_buf(out, in, buffer + position, length);
   position += length;
   }

/*************************************************
* Generate the internal state                    *
*************************************************/
void ARC4::generate()
   {
   for(u32bit j = 0; j != BUFFERSIZE; j++)
      {
      X++;
      Y += state[X % 256];
      std::swap(state[X % 256], state[Y % 256]);
      buffer[j] = (byte)state[(state[X % 256] + state[Y % 256]) % 256];
      }
   position = 0;
   }

/*************************************************
* ARC4 Key Schedule                              *
*************************************************/
void ARC4::set_key(const byte key[], u32bit length) throw(InvalidKeyLength)
   {
   if(!valid_keylength(length))
      throw InvalidKeyLength(name(), length);
   clear();
   for(u32bit j = 0; j != 256; j++)
      state[j] = j;
   for(u32bit j = 0, state_index = 0; j != 256; j++)
      {
      state_index = (state_index + key[j % length] + state[j]) % 256;
      std::swap(state[j], state[state_index]);
      }
   X = Y = 0;
   for(u32bit j = 0; j <= SKIP; j += BUFFERSIZE)
      generate();
   position += (SKIP % BUFFERSIZE);
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void ARC4::clear() throw()
   {
   state.clear();
   buffer.clear();
   position = X = Y = 0;
   }

/*************************************************
* ARC4 Constructor                               *
*************************************************/
ARC4::ARC4(u32bit s) : StreamCipher(name(), 1, KEYLENGTH), SKIP(s)
   {
   clear();
   }

}
