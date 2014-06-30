/*************************************************
* Version Information Header File                *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_VERSION_H__
#define OPENCL_VERSION_H__

#include <string>
#include <opencl/config.h>

namespace OpenCL {

/*************************************************
* Get information describing the version         *
*************************************************/
std::string version_string();
u32bit version_major();
u32bit version_minor();
u32bit version_patch();

}

#endif
