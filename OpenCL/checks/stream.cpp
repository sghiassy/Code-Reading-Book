/* This file is in the public domain */

#include <string>

#include <opencl/filters.h>
#include <opencl/arc4.h>
#include <opencl/isaac.h>
#include <opencl/seal.h>
using namespace OpenCL;

// Possibly these should be in the library...
template<u32bit X, u32bit B> struct SEAL_PARAM : public SEAL
   { SEAL_PARAM() : SEAL(X, B) {} };

Filter* lookup_stream(const std::string& algname, const SymmetricKey& key)
   {
   if(algname == "ARC4")
      return new StreamCipherFilter<ARC4>(key);
   if(algname == "MARK-4")
      return new StreamCipherFilter<MARK4>(key);
   else if(algname == "ISAAC")
      return new StreamCipherFilter<ISAAC>(key);

   /* This is "give me some sort of SEAL", used for the benchmarks */
   else if(algname == "SEAL")
      return new StreamCipherFilter< SEAL_PARAM<0, 64> >(key);

   /* These specific variants are requested by name for the validation tests */
   else if(algname == "SEAL<0,8>")
      return new StreamCipherFilter< SEAL_PARAM<0, 8> >(key);
   else if(algname == "SEAL<0x013577AF,4>")
      return new StreamCipherFilter< SEAL_PARAM<0x013577AF, 4> >(key);

   else return 0;
   }
