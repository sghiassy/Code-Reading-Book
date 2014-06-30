/*************************************************
* CRC24 Header File                              *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_CRC24_H__
#define OPENCL_CRC24_H__

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* CRC24                                          *
*************************************************/
class CRC24 : public HashFunction
   {
   public:
      static std::string name() { return "CRC24"; }
      static const u32bit HASHLENGTH = 3;
      void final(byte[HASHLENGTH]);
      void clear() throw() { crc = 0xB704CE; }
      CRC24() : HashFunction(name(), HASHLENGTH) { clear(); }
      ~CRC24() { clear(); }
   private:
      static const u32bit TABLE[256];
      void update_hash(const byte[], u32bit);
      u32bit crc;
   };

}

#endif
