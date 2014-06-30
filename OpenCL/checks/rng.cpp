/* This file is in the public domain */

#include <string>

#include <opencl/filters.h>

#include <opencl/square.h>
#include <opencl/randpool.h>
#include <opencl/x917.h>

#if defined(OPENCL_EXT_ENTROPY_SRC_DEVRANDOM)
#include <opencl/devrand.h>
#endif

#if defined(OPENCL_EXT_ENTROPY_SRC_PTHREAD)
#include <opencl/pthr_ent.h>
#endif

using namespace OpenCL;

/*************************************************/
/*
   This is the global RNG used in various spots. Seems to make most sense to
   declare it here.
*/
OpenCL::X917<OpenCL::Square> local_rng;

OpenCL::RandomNumberGenerator& rng = local_rng;
/*************************************************/

/* Not too useful generally; just dumps random bits */
template<typename R>
class RNG_Filter : public Filter
   {
   public:
      void write(const byte[], u32bit);
   private:
      static const u32bit BUFFERSIZE = OpenCL::DEFAULT_BUFFERSIZE;
      R rng;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      u32bit position;
   };

template<typename B>
void RNG_Filter<B>::write(const byte input[], u32bit length)
   {
   buffer.copy(position, input, length);
   if(position + length >= BUFFERSIZE)
      {
      rng.randomize(buffer, BUFFERSIZE);
      send(buffer, BUFFERSIZE);
      input += (BUFFERSIZE - position);
      length -= (BUFFERSIZE - position);
      while(length >= BUFFERSIZE)
         {
         /* This actually totally ignores the input, but it doesn't matter,
            because this is only for benchmark purposes and we just want to
            test speed. Anyway, if the RNG is good you can't tell the diff */
         rng.randomize(buffer, BUFFERSIZE);
         send(buffer, BUFFERSIZE);
         input += BUFFERSIZE;
         length -= BUFFERSIZE;
         }
       buffer.copy(input, length);
      position = 0;
      }
   position += length;
   }

/* A wrappr class to convert an EntropySource into a psudoe-RNG */
template<typename E>
class ES_TO_RNG
   {
   public:
      void randomize(byte buf[], u32bit size)
         {
         u32bit need = size;
         while(need) need -= es.slow_poll(buf + size - need, need);
         }

   private:
      E es;
   };

Filter* lookup_rng(const std::string& algname)
   {
   if(algname == "X917<Square>")
      return new RNG_Filter< X917<Square> >;
   else if(algname == "Randpool")
      return new RNG_Filter<Randpool>;

#if defined(OPENCL_EXT_ENTROPY_SRC_DEVRANDOM)
   else if(algname == "EntropySrc_DevRandom")
      return new RNG_Filter< ES_TO_RNG<DevRandom_EntropySource> >;
#endif

#if defined(OPENCL_EXT_ENTROPY_SRC_PTHREAD)
   else if(algname == "EntropySrc_Pthread")
      return new RNG_Filter< ES_TO_RNG<Pthread_EntropySource> >;
#endif
   else return 0;
   }
