/*************************************************
* SEAL Gamma Function Source File                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/seal_gam.h>
#include <opencl/sha1.h>

namespace OpenCL {

/*************************************************
* Gamma Constructor                              *
*************************************************/
Gamma::Gamma(const byte UK[20])
   {
   for(u32bit j = 0; j != 20; j++)
      K[j/4] = (K[j/4] << 8) + UK[j];
   last_index = 0xFFFFFFFF;
   }

/*************************************************
* Gamma Function                                 *
*************************************************/
u32bit Gamma::operator()(u32bit index)
   {
   u32bit new_index = index / 5;
   if(new_index != last_index)
      {
      SecureBuffer<byte, 80> W;
      for(u32bit j = 0; j != 4; j++)
         W[j] = get_byte(j, new_index);
      SHA1 sha1;
      sha1.digest = K;
      sha1.hash(W);
      digest = sha1.digest;
      last_index = new_index;
      }
   return digest[index % 5];
   }

}
