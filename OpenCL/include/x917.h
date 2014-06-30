/*************************************************
* ANSI X9.17 RNG Header File                     *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_X917_H__
#define OPENCL_X917_H__

#include <opencl/opencl.h>

namespace OpenCL {

template<typename B>
class X917 : public RandomNumberGenerator
   {
   public:
      static std::string name() { return "X917<" + B::name() + ">"; }
      byte random();
      void randomize(byte[], u32bit);
      void add_entropy(const byte[], u32bit) throw();
      void add_entropy(EntropySource&, bool = true);
      void clear() throw();
      X917(u32bit = 16);
   private:
      static const u32bit BLOCKSIZE = B::BLOCKSIZE,
                          MAX_OUT_PER_BUFFER = BLOCKSIZE / 2;
      const u32bit ITERATIONS_BEFORE_RESEED;
      void generate(u64bit);
      void reseed();
      B cipher;
      SecureBuffer<byte, BLOCKSIZE> buffer, state, tstamp;
      u32bit position, iteration;
   };

/*************************************************
* Generate a random byte                         *
*************************************************/
template<typename B>
byte X917<B>::random()
   {
   if(position == MAX_OUT_PER_BUFFER)
      {
      generate(system_clock());
      if(++iteration == ITERATIONS_BEFORE_RESEED)
         reseed();
      }
   return buffer[position++];
   }

/*************************************************
* Generate a buffer of random bytes              *
*************************************************/
template<typename B>
void X917<B>::randomize(byte out[], u32bit length)
   {
   while(position && length) { out[0] ^= random(); out++; length--; }
   while(length >= MAX_OUT_PER_BUFFER)
      {
      generate(system_clock());
      if(++iteration == ITERATIONS_BEFORE_RESEED)
         reseed();
      xor_buf(out, buffer, MAX_OUT_PER_BUFFER);
      out += MAX_OUT_PER_BUFFER;
      length -= MAX_OUT_PER_BUFFER;
      }
   while(length) { out[0] ^= random(); out++; length--; }
   }

/*************************************************
* Refill the internal state                      *
*************************************************/
template<typename B>
void X917<B>::generate(u64bit input)
   {
   xor_buf(tstamp, (byte*)&input, 8);
   cipher.encrypt(tstamp);
   xor_buf(buffer, state, tstamp, BLOCKSIZE);
   cipher.encrypt(buffer);
   xor_buf(state, buffer, tstamp, BLOCKSIZE);
   cipher.encrypt(state);
   position = 0;
   }

/*************************************************
* Add entropy to internal state                  *
*************************************************/
template<typename B>
void X917<B>::add_entropy(const byte data[], u32bit length) throw()
   {
   while(length)
      {
      u32bit added = std::min(state.size(), length);
      xor_buf(state, data, added);
      generate(system_clock());
      length -= added;
      data += added;
      }
   reseed();
   }

/*************************************************
* Add entropy to internal state                  *
*************************************************/
template<typename B>
void X917<B>::add_entropy(EntropySource& source, bool slowpoll)
   {
   SecureVector<byte> entropy(slowpoll ? (128) : (2*BLOCKSIZE));
   u32bit returned;
   if(slowpoll) returned = source.slow_poll(entropy, entropy.size());
   else         returned = source.fast_poll(entropy, entropy.size());
   add_entropy(entropy, returned);
   }

/*************************************************
* Reseed the internal state                      *
*************************************************/
template<typename B>
void X917<B>::reseed()
   {
   SecureBuffer<byte, B::KEYLENGTH> key;
   generate(system_clock());
   state = buffer;
   generate(system_clock());
   tstamp = buffer;
   randomize(key, key.size());
   cipher.set_key(key, key.size());
   generate(system_time());
   position = iteration = 0;
   }

/*************************************************
* Clear memory of sensitive data                 *
*************************************************/
template<typename B>
void X917<B>::clear() throw()
   {
   cipher.clear();
   buffer.clear();
   tstamp.clear();
   state.clear();
   position = iteration = 0;
   }

/*************************************************
* X917 Constructor                               *
*************************************************/
template<typename B>
X917<B>::X917(u32bit n) : RandomNumberGenerator(name()),
                          ITERATIONS_BEFORE_RESEED(n + 16)
   {
   clear();
   reseed();
   }


}

#endif
