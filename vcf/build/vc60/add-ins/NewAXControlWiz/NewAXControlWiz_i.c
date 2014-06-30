/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Thu Apr 19 22:21:31 2001
 */
/* Compiler settings for D:\code\vcf\build\vc60\add-ins\NewAXControlWiz\NewAXControlWiz.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


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

const IID LIBID_NEWAXCONTROLWIZLib = {0x978e9227,0x4c3b,0x4022,{0x8d,0xfe,0x6b,0xd1,0x6e,0x9c,0x21,0xfc}};


const IID IID_ICommands = {0xa3bb7b47,0x44a9,0x4f45,{0xa5,0xe4,0xab,0xa6,0xe5,0xbd,0x30,0x77}};


const CLSID CLSID_Commands = {0x0fc57ed5,0xc917,0x40d3,{0x82,0xbd,0x20,0x3e,0x7a,0x71,0xaf,0x8f}};


const CLSID CLSID_NewActiveXControlWiz = {0xb004cae4,0xa8e5,0x45c1,{0xb2,0xd0,0xb6,0x4b,0xc8,0xe7,0x18,0xec}};


#ifdef __cplusplus
}
#endif

