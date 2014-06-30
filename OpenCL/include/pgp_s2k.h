/*************************************************
* OpenPGP S2K Header File                        *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_OPENPGP_S2K_H__
#define OPENCL_OPENPGP_S2K_H__

#include <opencl/s2k.h>

namespace OpenCL {

class OpenPGP_S2K : public S2K
   {
   public:
      SymmetricKey derive(const std::string&, u32bit) const;

      OpenPGP_S2K(HashFunction*, u32bit = 0);
      ~OpenPGP_S2K() { delete hash; }
   private:
      HashFunction* hash;
   };

}

#endif
