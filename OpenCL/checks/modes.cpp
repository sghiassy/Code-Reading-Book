/* This file is in the public domain */

#include <string>

#include <opencl/des.h>
#include <opencl/rc5.h>

#include <opencl/cbc.h>
#include <opencl/cfb.h>
#include <opencl/ofb.h>
using namespace OpenCL;

// This is soooo stupid, but the only really good test of CBC-Pad is in
// RFC 2040, which uses RC5 with 8 rounds (!!!)
// Don't you even think about using this elsewhere. :)
struct RC5_8 : public RC5 { RC5_8() : RC5(8) {} };

Filter* lookup_mode(const std::string& algname, const BlockCipherKey& key,
                    const BlockCipherModeIV& iv)
   {
   if(algname == "CBC_Encryption<DES,NoPadding>")
      return new CBC_Encryption<DES,NoPadding>(key, iv);
   else if(algname == "CBC_Decryption<DES,NoPadding>")
      return new CBC_Decryption<DES,NoPadding>(key, iv);

   else if(algname == "CBC_Encryption<DES,PKCS7>")
      return new CBC_Encryption<DES,PKCSPadding>(key, iv);
   else if(algname == "CBC_Decryption<DES,PKCS7>")
      return new CBC_Decryption<DES,PKCSPadding>(key, iv);
   else if(algname == "CBC_Encryption<RC5_8,PKCS7>")
      return new CBC_Encryption<RC5_8,PKCSPadding>(key, iv);
   else if(algname == "CBC_Decryption<RC5_8,PKCS7>")
      return new CBC_Decryption<RC5_8,PKCSPadding>(key, iv);

   else if(algname == "CBC_Encryption<DES,OneAndZeros>")
      return new CBC_Encryption<DES,OneAndZeros>(key, iv);
   else if(algname == "CBC_Decryption<DES,OneAndZeros>")
      return new CBC_Decryption<DES,OneAndZeros>(key, iv);

   else if(algname == "CFB_Encryption<DES>(8)")
      return new CFB_Encryption<DES>(key, iv);
   else if(algname == "CFB_Decryption<DES>(8)")
      return new CFB_Decryption<DES>(key, iv);

   else if(algname == "CFB_Encryption<DES>(4)")
      return new CFB_Encryption<DES>(key, iv, 4);
   else if(algname == "CFB_Decryption<DES>(4)")
      return new CFB_Decryption<DES>(key, iv, 4);

   else if(algname == "CFB_Encryption<DES>(2)")
      return new CFB_Encryption<DES>(key, iv, 2);
   else if(algname == "CFB_Decryption<DES>(2)")
      return new CFB_Decryption<DES>(key, iv, 2);

   else if(algname == "CFB_Encryption<DES>(1)")
      return new CFB_Encryption<DES>(key, iv, 1);
   else if(algname == "CFB_Decryption<DES>(1)")
      return new CFB_Decryption<DES>(key, iv, 1);

   else if(algname == "OFB<DES>")
      return new OFB<DES>(key, iv);

   else if(algname == "Counter<DES>")
      return new Counter<DES>(key, iv);

   else return 0;
   }
