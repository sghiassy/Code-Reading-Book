/* This file is in the public domain */

#include <string>

#include <opencl/filters.h>
#include <opencl/encoder.h>
#include <opencl/symkey.h>
using namespace OpenCL_types;

OpenCL::Filter* lookup_block(const std::string&,
                             const OpenCL::BlockCipherKey&);
OpenCL::Filter* lookup_mode(const std::string&, const OpenCL::BlockCipherKey&,
                            const OpenCL::SymmetricKey&);
OpenCL::Filter* lookup_stream(const std::string&,
                              const OpenCL::StreamCipherKey&);
OpenCL::Filter* lookup_hash(const std::string&);
OpenCL::Filter* lookup_mac(const std::string&, const OpenCL::MACKey&);
OpenCL::Filter* lookup_rng(const std::string&);
OpenCL::Filter* lookup_encoder(const std::string&);
OpenCL::Filter* lookup_block(const std::string&, const OpenCL::SymmetricKey&);
OpenCL::Filter* lookup_s2k(const std::string&, const OpenCL::SymmetricKey&);

OpenCL::Filter* lookup(const std::string& algname, const std::string& hex_key,
                       const std::string& hex_iv)
   {
   OpenCL::SymmetricKey key(hex_key), iv(hex_iv);
   OpenCL::Filter* filter;

   filter = lookup_block(algname, key);
   if(filter) return filter;

   filter = lookup_mode(algname, key, iv);
   if(filter) return filter;

   filter = lookup_stream(algname, key);
   if(filter) return filter;

   filter = lookup_hash(algname);
   if(filter) return filter;

   filter = lookup_mac(algname, key);
   if(filter) return filter;

   filter = lookup_rng(algname);
   if(filter) return filter;

   filter = lookup_encoder(algname);
   if(filter) return filter;

   filter = lookup_s2k(algname, key);
   if(filter) return filter;

   return 0;
   }
