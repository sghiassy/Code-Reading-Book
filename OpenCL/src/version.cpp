/*************************************************
* OpenCL Version Information                     *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#include <opencl/opencl.h>

namespace OpenCL {

/*************************************************
* Return the version as a string                 *
*************************************************/
std::string version_string()
   {
   return "OpenCL 0.7.6";
   }

/*************************************************
* Return parts of the version as numbers         *
*************************************************/
u32bit version_major() { return 0; }
u32bit version_minor() { return 7; }
u32bit version_patch() { return 6; }

}
