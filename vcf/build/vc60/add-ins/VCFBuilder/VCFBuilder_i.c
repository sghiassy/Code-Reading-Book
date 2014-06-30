
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 5.03.0280 */
/* at Thu Sep 27 23:14:53 2001
 */
/* Compiler settings for D:\code\vcf\build\vc60\add-ins\VCFBuilder\VCFBuilder.odl:
    Os (OptLev=s), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#if !defined(_M_IA64) && !defined(_M_AXP64)

#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_VCFBuilder,0x1F4B4346,0x902F,0x4D1F,0x89,0x28,0x77,0x6F,0xDA,0x24,0x1E,0x51);


MIDL_DEFINE_GUID(IID, IID_ICommands,0xAAD052B9,0x5A96,0x4449,0xAA,0xE8,0x68,0xC8,0x5A,0x42,0x31,0x12);


MIDL_DEFINE_GUID(CLSID, CLSID_Commands,0xDEAF1BE8,0x2184,0x4FD1,0x8A,0x6F,0x4E,0x32,0x22,0xA9,0xBE,0x67);


MIDL_DEFINE_GUID(CLSID, CLSID_ApplicationEvents,0x2B3F0992,0x9E9B,0x40BE,0x81,0x00,0x7D,0x1E,0xCA,0x18,0x32,0x89);


MIDL_DEFINE_GUID(CLSID, CLSID_DebuggerEvents,0xF053F7EA,0x6784,0x4197,0x91,0x37,0x01,0x6B,0x6D,0xC2,0x1F,0x5E);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AXP64)*/

