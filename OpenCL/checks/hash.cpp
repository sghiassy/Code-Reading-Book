/* This file is in the public domain */

#include <string>

#include <opencl/filters.h>

#include <opencl/adler32.h>
#include <opencl/crc24.h>
#include <opencl/crc32.h>
#include <opencl/haval.h>
#include <opencl/md2.h>
#include <opencl/md4.h>
#include <opencl/md5.h>
#include <opencl/rmd128.h>
#include <opencl/rmd160.h>
#include <opencl/sha1.h>
#include <opencl/sha256.h>
#include <opencl/sha512.h>
#include <opencl/tiger.h>
using namespace OpenCL;

Filter* lookup_hash(const std::string& algname)
   {
   if(algname == "Adler32")         return new HashFilter<Adler32>;
   else if(algname == "CRC24")      return new HashFilter<CRC24>;
   else if(algname == "CRC32")      return new HashFilter<CRC32>;
   else if(algname == "HAVAL-128")  return new HashFilter<HAVAL_128>;
   else if(algname == "HAVAL-160")  return new HashFilter<HAVAL_160>;
   else if(algname == "HAVAL-192")  return new HashFilter<HAVAL_192>;
   else if(algname == "HAVAL-224")  return new HashFilter<HAVAL_224>;
   else if(algname == "HAVAL-256")  return new HashFilter<HAVAL_256>;
   else if(algname == "MD2")        return new HashFilter<MD2>;
   else if(algname == "MD4")        return new HashFilter<MD4>;
   else if(algname == "MD5")        return new HashFilter<MD5>;
   else if(algname == "RIPE-MD128") return new HashFilter<RIPEMD128>;
   else if(algname == "RIPE-MD160") return new HashFilter<RIPEMD160>;
   else if(algname == "SHA-1")      return new HashFilter<SHA1>;
   else if(algname == "SHA2-256")   return new HashFilter<SHA2_256>;
   else if(algname == "SHA2-512")   return new HashFilter<SHA2_512>;
   else if(algname == "Tiger")      return new HashFilter<Tiger>;

   else return 0;
   }
