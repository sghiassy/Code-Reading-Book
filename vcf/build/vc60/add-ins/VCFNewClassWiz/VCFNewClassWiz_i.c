
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 5.03.0280 */
/* at Sun Sep 16 12:06:06 2001
 */
/* Compiler settings for D:\code\vcf\build\vc60\add-ins\VCFNewClassWiz\VCFNewClassWiz.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
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

MIDL_DEFINE_GUID(IID, LIBID_VCFNEWCLASSWIZLib,0xc29b2483,0x13bc,0x4ac3,0xa4,0x8d,0x3e,0x14,0x52,0x8c,0xc3,0xe0);


MIDL_DEFINE_GUID(IID, IID_ICommands,0x8a71b15c,0x3e59,0x4b00,0x90,0xb3,0x19,0x18,0xeb,0x03,0x3a,0x04);


MIDL_DEFINE_GUID(CLSID, CLSID_Commands,0x35145f45,0x39a4,0x4381,0x92,0xa4,0x9a,0x0d,0xeb,0xd0,0xaa,0xac);


MIDL_DEFINE_GUID(CLSID, CLSID_VCFNewClassWiz,0x488146ca,0x4461,0x465c,0xbe,0x69,0x72,0x11,0x18,0x5f,0xa3,0x08);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif



#endif /* !defined(_M_IA64) && !defined(_M_AXP64)*/

