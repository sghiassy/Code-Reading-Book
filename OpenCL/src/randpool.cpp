/*************************************************
* Randpool Source File                           *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/randpool.h>

namespace OpenCL {

/*************************************************
* Generate a random byte                         *
*************************************************/
byte Randpool::random()
   {
   if(position == BUFFERSIZE)
      generate(system_clock());
   return buffer[position++];
   }

/*************************************************
* Generate a buffer of random bytes              *
*************************************************/
void Randpool::randomize(byte out[], u32bit length)
   {
   while(length >= BUFFERSIZE - position)
      {
      xor_buf(out, buffer + position, BUFFERSIZE - position);
      length -= (BUFFERSIZE - position);
      out += (BUFFERSIZE - position);
      generate(system_clock());
      }
   xor_buf(out, buffer + position, length);
   position += length;
   }

/*************************************************
* Refill the buffer                              *
*************************************************/
void Randpool::generate(u64bit input)
   {
   RANDPOOL_HASH hash;
   hash.update((byte*)&input, 8);
   hash.update(pool, POOLSIZE);
   hash.final(buffer);
   xor_buf(pool, buffer, BUFFERSIZE);
   mix_pool();
   position = 0;
   }

/*************************************************
* Mix up the pool                                *
*************************************************/
void Randpool::mix_pool()
   {
   RANDPOOL_HASH hash;
   for(u32bit j = 0; j != POOLSIZE; j += BUFFERSIZE)
      {
      xor_buf(pool + (j + BUFFERSIZE) % POOLSIZE, pool + j, BUFFERSIZE);
      hash.process(pool + j, std::min(3 * BUFFERSIZE, POOLSIZE - j), pool + j);
      }
   }

/*************************************************
* Add entropy to internal state                  *
*************************************************/
void Randpool::add_entropy(const byte data[], u32bit length) throw()
   {
   while(length)
      {
      u32bit added = std::min(pool.size(), length);
      xor_buf(pool, data, added);
      generate(system_clock());
      length -= added;
      data += added;
      }
   generate(system_time());
   }

/*************************************************
* Add entropy to internal state                  *
*************************************************/
void Randpool::add_entropy(EntropySource& source, bool slowpoll)
   {
   SecureVector<byte> entropy(slowpoll ? (POOLSIZE / 8) : (POOLSIZE / 16));
   u32bit returned;
   if(slowpoll) returned = source.slow_poll(entropy, entropy.size());
   else         returned = source.fast_poll(entropy, entropy.size());
   add_entropy(entropy, returned);
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
void Randpool::clear() throw()
   {
   pool.clear();
   buffer.clear();
   position = 0;
   }

/*************************************************
* Randpool Constructor                           *
*************************************************/
Randpool::Randpool() : RandomNumberGenerator(name())
   {
   clear();
   generate(system_time());
   }

}
