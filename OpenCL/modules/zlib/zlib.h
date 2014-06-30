/*************************************************
* Zlib Compressor Header File                    *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_EXT_ZLIB_H__
#define OPENCL_EXT_ZLIB_H__

#include <opencl/filters.h>

namespace OpenCL {

class Zlib_Compress : public Filter
   {
   public:
      void write(const byte input[], u32bit length);
      void flush();
      void final();
      void clear();

      Zlib_Compress(u32bit l = 9) : level((l >= 9) ? 9 : l)
         { ready = false; zlib = 0; }
      ~Zlib_Compress() { clear(); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      const u32bit level;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      struct zlib_stream_wrapper* zlib;
      bool ready;
   };

class Zlib_Decompress : public Filter
   {
   public:
      void write(const byte input[], u32bit length);
      void final();
      void clear();

      Zlib_Decompress() { done = ready = false; zlib = 0;}
      ~Zlib_Decompress() { clear(); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      struct zlib_stream_wrapper* zlib;
      bool ready, done;
   };

}

#endif
