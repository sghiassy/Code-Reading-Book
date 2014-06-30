/*************************************************
* SymmetricKey Header File                       *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SYMKEY_H__
#define OPENCL_SYMKEY_H__

#include <string>
#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* SymmetricKey                                   *
*************************************************/
class SymmetricKey
   {
   public:
      operator const byte* () const { return key; }
      u32bit length() const { return key.size(); }

      SymmetricKey& operator^=(const SymmetricKey&);

      void change(const byte[], u32bit);
      void change(const SecureVector<byte>& v) { key = v; }
      void change(const std::string&);
      void change(RandomNumberGenerator&, u32bit);

      SymmetricKey(const std::string& str = "") { change(str); }
      SymmetricKey(const byte in[], u32bit len) { change(in, len); }
      SymmetricKey(RandomNumberGenerator& r, u32bit len) { change(r, len); }
      SymmetricKey(const SecureVector<byte>& v) { change(v); }
   private:
      SecureVector<byte> key;
   };

/*************************************************
* XOR Operation for SymmetricKeys                *
*************************************************/
SymmetricKey operator^(const SymmetricKey&, const SymmetricKey&);

typedef SymmetricKey BlockCipherKey;
typedef SymmetricKey StreamCipherKey;
typedef SymmetricKey MACKey;

}

#endif
