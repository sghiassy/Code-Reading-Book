/*************************************************
* Zlib Compressor Source File                    *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <zlib.h>
#include <cstring>
#include <opencl/zlib.h>

namespace OpenCL {

/*************************************************
* Wrapper Type for Zlib Stream                   *
*************************************************/
struct zlib_stream_wrapper
   {
   z_stream stream;
   zlib_stream_wrapper()  { std::memset(&stream, 0, sizeof(z_stream)); }
   ~zlib_stream_wrapper() { std::memset(&stream, 0, sizeof(z_stream)); }
   };

/*************************************************
* Compress Input with Zlib                       *
*************************************************/
void Zlib_Compress::write(const byte input[], u32bit length)
   {
   if(!ready)
      {
      clear();
      zlib = new zlib_stream_wrapper;
      if(deflateInit(&(zlib->stream), level) != Z_OK)
         throw Exception("Zlib_Compress: Memory allocation error");
      ready = true;
      }

   zlib->stream.next_in = (Bytef*)input;
   zlib->stream.avail_in = length;

   while(zlib->stream.avail_in != 0)
      {
      zlib->stream.next_out = (Bytef*)buffer.ptr();
      zlib->stream.avail_out = buffer.size();
      deflate(&(zlib->stream), Z_NO_FLUSH);
      send(buffer, buffer.size() - zlib->stream.avail_out);
      }
   }

/*************************************************
* Flush the Zlib compressor                      *
*************************************************/
void Zlib_Compress::flush()
   {
   if(!ready) return;
   zlib->stream.next_in = 0;
   zlib->stream.avail_in = 0;

   while(true)
      {
      zlib->stream.next_out = (Bytef*)buffer.ptr();
      zlib->stream.avail_out = buffer.size();
      deflate(&(zlib->stream), Z_FULL_FLUSH);
      send(buffer, buffer.size() - zlib->stream.avail_out);
      if(zlib->stream.avail_out == buffer.size()) break;
      }
   }

/*************************************************
* Finish Compressing with Zlib                   *
*************************************************/
void Zlib_Compress::final()
   {
   if(!ready) return;
   zlib->stream.next_in = 0;
   zlib->stream.avail_in = 0;

   int rc = Z_OK;
   while(rc != Z_STREAM_END)
      {
      zlib->stream.next_out = (Bytef*)buffer.ptr();
      zlib->stream.avail_out = buffer.size();
      rc = deflate(&(zlib->stream), Z_FINISH);
      send(buffer, buffer.size() - zlib->stream.avail_out);
      }
   clear();
   }

/*************************************************
* Clean up Compression Context                   *
*************************************************/
void Zlib_Compress::clear()
   {
   if(!zlib) return;
   deflateEnd(&(zlib->stream));
   delete zlib;
   zlib = 0;
   ready = false;
   }

/*************************************************
* Decompress Input with Zlib                     *
*************************************************/
void Zlib_Decompress::write(const byte input[], u32bit length)
   {
   if(!ready)
      {
      clear();
      zlib = new zlib_stream_wrapper;
      if(inflateInit(&(zlib->stream)) != Z_OK)
         throw Exception("Zlib_Decompress: Memory allocation error");
      ready = true;
      }

   zlib->stream.next_in = (Bytef*)input;
   zlib->stream.avail_in = length;

   while(zlib->stream.avail_in != 0)
      {
      zlib->stream.next_out = (Bytef*)buffer.ptr();
      zlib->stream.avail_out = buffer.size();

      int rc = inflate(&(zlib->stream), Z_SYNC_FLUSH);
      if(rc != Z_OK && rc != Z_STREAM_END)
         {
         clear();
         if(rc == Z_DATA_ERROR)
            throw Decoding_Error("Zlib_Decompress: Data integrity error");
         if(rc == Z_NEED_DICT)
            throw Decoding_Error("Zlib_Decompress: Needs preset dictionary");
         if(rc == Z_MEM_ERROR)
            throw Exception("Zlib_Decompress: Memory allocation error");
         throw Exception("Zlib_Decompress: Unknown decompress error");
         }
      if(rc == Z_STREAM_END) ready = false;
      send(buffer, buffer.size() - zlib->stream.avail_out);
      }
   }

/*************************************************
* Finish Decompressing with Zlib                 *
*************************************************/
void Zlib_Decompress::final()
   {
   if(!ready) return;
   zlib->stream.next_in = 0;
   zlib->stream.avail_in = 0;

   int rc = Z_OK;
   while(rc != Z_STREAM_END)
      {
      zlib->stream.next_out = (Bytef*)buffer.ptr();
      zlib->stream.avail_out = buffer.size();
      rc = inflate(&(zlib->stream), Z_SYNC_FLUSH);
      if(rc != Z_OK && rc != Z_STREAM_END)
         {
         clear();
         throw Exception("Zlib_Decompress: Error finalizing decompression");
         }
      send(buffer, buffer.size() - zlib->stream.avail_out);
      }
   clear();
   }

/*************************************************
* Clean up Decompression Context                 *
*************************************************/
void Zlib_Decompress::clear()
   {
   if(!zlib) return;
   inflateEnd(&(zlib->stream));
   delete zlib;
   zlib = 0;
   ready = false;
   }

}
