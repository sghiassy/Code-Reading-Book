/* This file is in the public domain */

#include <vector>
#include <string>
#include <cstdlib>

#include <opencl/filters.h>
#include <opencl/md5.h>
#include <opencl/sha1.h>

#include <opencl/pgp_s2k.h>
using namespace OpenCL;

class S2K_Filter : public Filter
   {
   public:
      void write(const byte in[], u32bit len)
         { passphrase += std::string((const char*)in, len); }
      void final()
         {
         SymmetricKey x = s2k->derive(passphrase, outlen);
         send(x, x.length());
         }
      S2K_Filter(S2K* algo, const SymmetricKey& s, u32bit o, u32bit i = 0)
         {
         s2k = algo;
         outlen = o;
         s2k->set_iterations(i);
         s2k->change_salt(s, s.length());
         }
      ~S2K_Filter() { delete s2k; }
   private:
      std::string passphrase;
      S2K* s2k;
      u32bit outlen;
   };

Filter* lookup_s2k(const std::string& algname, const SymmetricKey& salt)
   {
   // OpenPGP S2K names are of the form OpenPGP-S2K(hash,outlen,iterations)
   // Since everything is dynamic we can be very general about what we
   // accept here. However this does assume the name is well formed.
   if(algname.find("OpenPGP-S2K") != std::string::npos)
      {
      u32bit iterations, outlen;
      HashFunction* hash = 0;

      std::string params = algname;
      params.erase(params.find("OpenPGP-S2K("), 12);
      params.erase(params.find(")"), 1);
      params += ',';

      std::vector<std::string> substr;
      std::string::size_type start = 0, end = params.find(',');
      while(end != std::string::npos)
         {
         substr.push_back(params.substr(start, end-start));
         start = end+1;
         end = params.find(',', start);
         }
      if(substr.size() != 3)
         throw Exception("OpenPGP_S2K name decoding failed");

      if(substr[0] == "SHA1")     hash = new SHA1;
      else if(substr[0] == "MD5") hash = new MD5;

      outlen = atoi(substr[1].c_str());
      iterations = atoi(substr[2].c_str());

      return new S2K_Filter(new OpenPGP_S2K(hash), salt, outlen, iterations);
      }

   else return 0;
   }
