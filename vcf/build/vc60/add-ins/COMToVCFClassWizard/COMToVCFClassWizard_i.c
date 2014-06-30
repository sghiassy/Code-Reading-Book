/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Aug 06 20:49:14 2001
 */
/* Compiler settings for D:\code\vcf\build\vc60\add-ins\COMToVCFClassWizard\COMToVCFClassWizard.idl:
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

const IID LIBID_COMTOVCFCLASSWIZARDLib = {0xb398ee76,0xe839,0x487c,{0xba,0xfd,0x72,0x25,0xe8,0xbe,0xbd,0xad}};


const IID IID_ICommands = {0x4abb8ee9,0xb42b,0x42cb,{0x99,0x3f,0x98,0x55,0x54,0xf4,0x1e,0x61}};


const CLSID CLSID_Commands = {0x9ff50665,0x30a0,0x4e5d,{0xbd,0x3f,0x8b,0x66,0x72,0xa0,0xc7,0x92}};


const CLSID CLSID_COMToVCFClassWizard = {0x1973ab9d,0xdc6e,0x4f1f,{0x84,0x06,0x81,0x8b,0x16,0xe1,0xda,0x79}};


#ifdef __cplusplus
}
#endif

