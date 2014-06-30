/*************************************************
* ARC4 Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_ARC4_H__
#define OPENCL_ARC4_H__

#include <opencl/opencl.h>

namespace OpenCL {

class ARC4 : public StreamCipher
   {
   public:
      static const u32bit KEYLENGTH = 32;
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      static std::string name() { return "ARC4"; }
      ARC4(u32bit = 0);
      ~ARC4() { clear(); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      const u32bit SKIP;
      void cipher(const byte[], byte[], u32bit);
      void generate();
      SecureBuffer<byte, BUFFERSIZE> buffer;
      SecureBuffer<u32bit, 256> state;
      u32bit X, Y, position;
   };

struct MARK4 : public ARC4
   {
   static std::string name() { return "MARK4"; }
   MARK4() : ARC4(256) {}
   };

}

#endif
