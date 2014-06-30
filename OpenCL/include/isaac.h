/*************************************************
* ISAAC Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_ISAAC_H__
#define OPENCL_ISAAC_H__

#include <opencl/opencl.h>

namespace OpenCL {

class ISAAC : public StreamCipher
   {
   public:
      static std::string name() { return "ISAAC"; }
      static const u32bit KEYLENGTH = 32;
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void clear() throw();
      ISAAC() : StreamCipher(name(), 1, KEYLENGTH) { clear(); }
      ~ISAAC() { clear(); }
   private:
      static const u32bit BUFFERSIZE = DEFAULT_BUFFERSIZE;
      void cipher(const byte[], byte[], u32bit);
      void generate();
      SecureBuffer<u32bit, 256> state;
      SecureBuffer<byte, BUFFERSIZE> buffer;
      u32bit A, B, C, position;
   };

}

#endif
