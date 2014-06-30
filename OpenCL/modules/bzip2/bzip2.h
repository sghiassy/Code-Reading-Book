/*************************************************
* Bzip Compressor Header File                    *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_EXT_BZIP2_H__
#define OPENCL_EXT_BZIP2_H__

#include <opencl/filters.h>

namespace OpenCL {

class Bzip_Compress : public Filter
   {
   public:
      void write(const byte input[], u32bit length);
      void flush();
      void final();
      void clear();

      Bzip_Compress(u32bit l = 9) : level((l >= 9) ? 9 : l)
         { ready = false; bz = 0; }
      ~Bzip_Compress() { clear(); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      const u32bit level;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      struct bz_stream_wrapper* bz;
      bool ready;
   };

class Bzip_Decompress : public Filter
   {
   public:
      void write(const byte input[], u32bit length);
      void final();
      void clear();

      Bzip_Decompress(bool s = false) : small_mem(s)
         { done = ready = false; bz = 0;}
      ~Bzip_Decompress() { clear(); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      const bool small_mem;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      struct bz_stream_wrapper* bz;
      bool ready, done;
   };

}

#endif
