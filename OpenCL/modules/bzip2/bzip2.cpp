/*************************************************
* Bzip Compressor Source File                    *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#define BZ_NO_STDIO
#include <bzlib.h>
#include <cstring>
#include <opencl/bzip2.h>

namespace OpenCL {

/*************************************************
* Wrapper Type for Bzip Stream                   *
*************************************************/
struct bz_stream_wrapper
   {
   bz_stream stream;
   bz_stream_wrapper()  { std::memset(&stream, 0, sizeof(bz_stream)); }
   ~bz_stream_wrapper() { std::memset(&stream, 0, sizeof(bz_stream)); }
   };

/*************************************************
* Compress Input with Bzip                       *
*************************************************/
void Bzip_Compress::write(const byte input[], u32bit length)
   {
   if(!ready)
      {
      clear();
      bz = new bz_stream_wrapper;
      if(BZ2_bzCompressInit(&(bz->stream), level, 0, 0) != BZ_OK)
         throw Exception("Bzip_Compress: Memory allocation error");
      ready = true;
      }

   bz->stream.next_in = (char*)input;
   bz->stream.avail_in = length;

   while(bz->stream.avail_in != 0)
      {
      bz->stream.next_out = (char*)buffer.ptr();
      bz->stream.avail_out = buffer.size();
      BZ2_bzCompress(&(bz->stream), BZ_RUN);
      send(buffer, buffer.size() - bz->stream.avail_out);
      }
   }

/*************************************************
* Flush the Bzip compressor                      *
*************************************************/
void Bzip_Compress::flush()
   {
   if(!ready) return;
   bz->stream.next_in = 0;
   bz->stream.avail_in = 0;

   int rc = BZ_OK;
   while(rc != BZ_RUN_OK)
      {
      bz->stream.next_out = (char*)buffer.ptr();
      bz->stream.avail_out = buffer.size();
      rc = BZ2_bzCompress(&(bz->stream), BZ_FLUSH);
      send(buffer, buffer.size() - bz->stream.avail_out);
      }
   }

/*************************************************
* Finish Compressing with Bzip                   *
*************************************************/
void Bzip_Compress::final()
   {
   if(!ready) return;
   bz->stream.next_in = 0;
   bz->stream.avail_in = 0;

   int rc = BZ_OK;
   while(rc != BZ_STREAM_END)
      {
      bz->stream.next_out = (char*)buffer.ptr();
      bz->stream.avail_out = buffer.size();
      rc = BZ2_bzCompress(&(bz->stream), BZ_FINISH);
      send(buffer, buffer.size() - bz->stream.avail_out);
      }
   clear();
   }

/*************************************************
* Clean up Compression Context                   *
*************************************************/
void Bzip_Compress::clear()
   {
   if(!bz) return;
   BZ2_bzCompressEnd(&(bz->stream));
   delete bz;
   bz = 0;
   ready = false;
   }

/*************************************************
* Decompress Input with Bzip                     *
*************************************************/
void Bzip_Decompress::write(const byte input[], u32bit length)
   {
   if(!ready)
      {
      clear();
      bz = new bz_stream_wrapper;
      if(BZ2_bzDecompressInit(&(bz->stream), 0, small_mem) != BZ_OK)
         throw Exception("Bzip_Decompress: Memory allocation error");
      ready = true;
      }

   bz->stream.next_in = (char*)input;
   bz->stream.avail_in = length;

   while(bz->stream.avail_in != 0)
      {
      bz->stream.next_out = (char*)buffer.ptr();
      bz->stream.avail_out = buffer.size();

      int rc = BZ2_bzDecompress(&(bz->stream));
      if(rc != BZ_OK && rc != BZ_STREAM_END)
         {
         clear();
         if(rc == BZ_DATA_ERROR)
            throw Decoding_Error("Bzip_Decompress: Data integrity error");
         if(rc == BZ_DATA_ERROR_MAGIC)
            throw Decoding_Error("Bzip_Decompress: Input not valid bzip data");
         if(rc == BZ_MEM_ERROR)
            throw Exception("Bzip_Decompress: Memory allocation error");
         throw Exception("Bzip_Decompress: Unknown decompress error");
         }
      if(rc == BZ_STREAM_END) ready = false;
      send(buffer, buffer.size() - bz->stream.avail_out);
      }
   }

/*************************************************
* Finish Decompressing with Bzip                 *
*************************************************/
void Bzip_Decompress::final()
   {
   if(!ready) return;
   bz->stream.next_in = 0;
   bz->stream.avail_in = 0;

   int rc = BZ_OK;
   while(rc != BZ_STREAM_END)
      {
      bz->stream.next_out = (char*)buffer.ptr();
      bz->stream.avail_out = buffer.size();
      rc = BZ2_bzDecompress(&(bz->stream));
      if(rc != BZ_OK && rc != BZ_STREAM_END)
         {
         clear();
         throw Exception("Bzip_Decompress: Error finalizing decompression");
         }
      send(buffer, buffer.size() - bz->stream.avail_out);
      }
   clear();
   }

/*************************************************
* Clean up Decompression Context                 *
*************************************************/
void Bzip_Decompress::clear()
   {
   if(!bz) return;
   BZ2_bzDecompressEnd(&(bz->stream));
   delete bz;
   bz = 0;
   ready = false;
   }

}
