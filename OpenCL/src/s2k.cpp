/*************************************************
* S2K Base Class Source File                     *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/s2k.h>

namespace OpenCL {

/*************************************************
* Set the number of iterations                   *
*************************************************/
void S2K::set_iterations(u32bit i)
   {
   if(can_change_iterations())
      iter = i;
   else
      throw Exception("S2K: Iteration count cannot be set by this S2K method");
   }

/*************************************************
* Change the salt                                *
*************************************************/
void S2K::change_salt(const byte s[], u32bit len)
   {
   salt.create(len);
   salt.copy(s, len);
   }

/*************************************************
* Change the salt                                *
*************************************************/
void S2K::change_salt(const SecureVector<byte>& s)
   {
   change_salt(s.ptr(), s.size());
   }

/*************************************************
* Change the salt                                *
*************************************************/
void S2K::change_salt(RandomNumberGenerator& rng, u32bit len)
   {
   SecureVector<byte> k(len);
   rng.randomize(k, len);
   change_salt(k, k.size());
   }

}
