/*************************************************
* Low Level Types Header File                    *
* (C) 1999-2001 The OpenCL Project               *
*************************************************/

#ifndef OPENCL_TYPES_H__
#define OPENCL_TYPES_H__

namespace OpenCL {

typedef unsigned char byte;
typedef unsigned short u16bit;
typedef unsigned int u32bit;

typedef signed int s32bit;

#if defined(_MSC_VER) || defined(__BORLANDC__)
   typedef unsigned __int64 u64bit;
#elif defined(__KCC)
   typedef unsigned __long_long u64bit;
#elif defined(__GNUG__)
   __extension__ typedef unsigned long long u64bit;
#else
   typedef unsigned long long u64bit;
#endif

}

namespace OpenCL_types {

typedef OpenCL::byte byte;
typedef OpenCL::u32bit u32bit;

}

#endif
