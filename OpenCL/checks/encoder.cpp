/* This file is in the public domain */
/* This file handles fixed-bit codes and any (de)compressors */

#include <string>

#include <opencl/config.h>
#include <opencl/encoder.h>

#ifdef OPENCL_EXT_BZIP2
#include <opencl/bzip2.h>
#endif

#ifdef OPENCL_EXT_GZIP
#include <opencl/gzip.h>
#endif

#ifdef OPENCL_EXT_ZLIB
#include <opencl/zlib.h>
#endif

OpenCL::Filter* lookup_encoder(const std::string& algname)
   {
   if(algname == "Base64_Encode") return new OpenCL::Base64Encoder;
   if(algname == "Base64_Decode") return new OpenCL::Base64Decoder;

#ifdef OPENCL_EXT_BZIP2
   if(algname == "Bzip_Compress") return new OpenCL::Bzip_Compress;
   if(algname == "Bzip_Decompress") return new OpenCL::Bzip_Decompress;
#endif

#ifdef OPENCL_EXT_GZIP
   if(algname == "Gzip_Compress") return new OpenCL::Gzip_Compress;
   if(algname == "Gzip_Decompress") return new OpenCL::Gzip_Decompress;
#endif

#ifdef OPENCL_EXT_ZLIB
   if(algname == "Zlib_Compress") return new OpenCL::Zlib_Compress;
   if(algname == "Zlib_Decompress") return new OpenCL::Zlib_Decompress;
#endif

   else return 0;
   }
