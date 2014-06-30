/*************************************************
* Exceptions Source File                         *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/exceptn.h>
#include <opencl/util.h>

namespace OpenCL {

/*************************************************
* Constructor for InvalidKeyLength               *
*************************************************/
InvalidKeyLength::InvalidKeyLength(const std::string& name, u32bit length)
   {
   set_msg(name + " cannot accept a key of length " + to_string(length));
   }

/*************************************************
* Constructor for Invalid_BlockSize              *
*************************************************/
Invalid_BlockSize::Invalid_BlockSize(const std::string& mode,
                                     const std::string& pad)
   {
   set_msg("Padding method " + pad + " cannot be used with " + mode);
   }

/*************************************************
* Constructor for Invalid_IVLength               *
*************************************************/
Invalid_IVLength::Invalid_IVLength(const std::string& mode, u32bit bad_len)
   {
   set_msg("IV length " + to_string(bad_len) + " is invalid for " + mode);
   }

}
