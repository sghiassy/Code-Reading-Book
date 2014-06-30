/*************************************************
* CRC32 Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CRC32_H__
#define OPENCL_CRC32_H__

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* CRC32                                          *
*************************************************/
class CRC32 : public HashFunction
   {
   public:
      static std::string name() { return "CRC32"; }
      static const u32bit HASHLENGTH = 4;
      void final(byte[HASHLENGTH]);
      void clear() throw() { crc = 0xFFFFFFFF; }
      CRC32() : HashFunction(name(), HASHLENGTH) { clear(); }
      ~CRC32() { clear(); }
   private:
      static const u32bit TABLE[256];
      void update_hash(const byte[], u32bit);
      u32bit crc;
   };

}

#endif
