/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sat Jun 09 23:58:12 2001
 */
/* Compiler settings for D:\code\vcf\build\vc60\add-ins\VCFListener\VCFListener.idl:
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

const IID LIBID_VCFLISTENERLib = {0x78f2542b,0xc42e,0x4799,{0xa2,0x97,0x6f,0x77,0xa5,0x5f,0xb5,0x58}};


const IID IID_ICommands = {0x6df3dfa3,0x846d,0x46aa,{0x93,0xd0,0x87,0xb1,0xf2,0xdd,0xa2,0x08}};


const CLSID CLSID_Commands = {0xe86b1abd,0xf8be,0x48c2,{0xb9,0xe8,0x29,0x96,0x86,0xa3,0xe2,0xa9}};


const CLSID CLSID_VCFListener = {0x31300251,0x83a4,0x4183,{0xbb,0xbb,0x1a,0x06,0xee,0xce,0xbd,0xe2}};


#ifdef __cplusplus
}
#endif

