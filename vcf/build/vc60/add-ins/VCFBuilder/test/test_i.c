/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Jun 25 01:02:52 2001
 */
/* Compiler settings for D:\code\vcf\build\vc60\add-ins\VCFBuilder\test\test.odl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
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

const IID LIBID_Test = {0xC9E1E33B,0x5547,0x4DD9,{0x83,0x8C,0xCA,0x91,0xB0,0x89,0x7D,0x10}};


const IID IID_ICommands = {0x39034FA1,0xFA4B,0x4038,{0xA0,0xC1,0x36,0x30,0x01,0x74,0xC1,0x4F}};


const CLSID CLSID_Commands = {0x4113B436,0x08CA,0x46B4,{0xAA,0x5E,0x40,0xE9,0x8E,0xCC,0x66,0x41}};


const CLSID CLSID_ApplicationEvents = {0xE1B396D5,0x6621,0x4D0A,{0xBE,0x16,0x3B,0xA0,0x6D,0xA6,0x8C,0x9D}};


const CLSID CLSID_DebuggerEvents = {0xB92A3135,0x775B,0x4E93,{0xB4,0x4C,0xC7,0x7D,0x3D,0x0B,0xE7,0x9F}};


#ifdef __cplusplus
}
#endif

