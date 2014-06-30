/*************************************************
* Adler32 Header File                            *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_ADLER32_H__
#define OPENCL_ADLER32_H__

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* Adler32                                        *
*************************************************/
class Adler32 : public HashFunction
   {
   public:
      static std::string name() { return "Adler32"; }
      static const u32bit HASHLENGTH = 4;
      void final(byte[HASHLENGTH]);
      void clear() throw() { S1 = 1; S2 = 0; }
      Adler32() : HashFunction(name(), HASHLENGTH) { clear(); }
      ~Adler32() { clear(); }
   private:
      void update_hash(const byte[], u32bit);
      void hash(const byte[], u32bit);
      u32bit S1, S2;
   };

}

#endif
