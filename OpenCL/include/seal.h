/*************************************************
* SEAL Header File                               *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_SEAL_H__
#define OPENCL_SEAL_H__

#include <opencl/opencl.h>

namespace OpenCL {

class SEAL : public RandomAccessStreamCipher
   {
   public:
      static std::string name() { return "SEAL"; }
      static const u32bit KEYLENGTH = 20;
      void set_key(const byte[], u32bit = KEYLENGTH) throw(InvalidKeyLength);
      void seek(u32bit);
      void clear() throw();
      SEAL(u32bit = 0, u32bit = 16);
   private:
      void cipher(const byte[], byte[], u32bit);
      void generate(u32bit);
      const u32bit START, STATESIZE;
      SecureVector<byte> state;
      SecureBuffer<u32bit, 512> T;
      SecureBuffer<u32bit, 256> S;
      SecureVector<u32bit> R;
      u32bit counter, position;
      };

}

#endif
