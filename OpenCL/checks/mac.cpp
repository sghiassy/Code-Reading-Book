/* This file is in the public domain */

#include <string>

#include <opencl/filters.h>

#include <opencl/des.h>
#include <opencl/square.h>

#include <opencl/md5.h>
#include <opencl/rmd128.h>
#include <opencl/rmd160.h>
#include <opencl/sha1.h>

#include <opencl/emac.h>
#include <opencl/hmac.h>
#include <opencl/md5mac.h>
using namespace OpenCL;

Filter* lookup_mac(const std::string& algname, const SymmetricKey& key)
   {
   if(algname == "EMAC<Square>")
      return new MACFilter< EMAC<Square> >(key);
   else if(algname == "EMAC<DES>")
      return new MACFilter< EMAC<DES> >(key);
   else if(algname == "HMAC-MD5")
      return new MACFilter< HMAC<MD5> >(key);
   else if(algname == "HMAC-SHA1")
      return new MACFilter< HMAC<SHA1> >(key);
   else if(algname == "HMAC-RIPE-MD128")
      return new MACFilter< HMAC<RIPEMD128> >(key);
   else if(algname == "HMAC-RIPE-MD160")
      return new MACFilter< HMAC<RIPEMD160> >(key);
   else if(algname == "MD5-MAC") return new MACFilter<MD5MAC>(key);

   else return 0;
   }
