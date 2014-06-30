/*************************************************
* /dev/random Entropy Source Source File         *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <fstream>
#include <opencl/devrand.h>

namespace OpenCL {

/*************************************************
* Gather Entropy from /dev/random                *
*************************************************/
u32bit DevRandom_EntropySource::slow_poll(byte random[], u32bit length)
   {
   std::ifstream random_device(device.c_str());
   if(!random_device)
     throw Stream_IO_Error("Opening the file " + device + " failed");
   random_device.read((char*)random, length);
   return length;
   }

}
