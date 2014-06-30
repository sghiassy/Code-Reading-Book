/* $XConsortium: s3pc98.h /main/6 1996/10/23 13:22:40 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree98/accel/s3nec/s3pc98.h,v 3.5 1996/12/23 07:05:52 dawes Exp $ */



#define	PW	0x10
#define	PW805I	0x20
#define	PCSKB	0x30
#define	PCHKB	0x40
#define	NECWAB	0x50
#define	NECWAB928_E_C	0x50 /* External C-bus     e.g. WAB-B */
#define	NECWAB928_E_L	0x58 /* External Local-Bus e.g. WAB-A */
#define	NECWAB928_I	0x5C /* Internal           e.g. Ap2/U8W */
#define	NECWAB864_I	0x5D /* Internal           e.g. Xn /U8W */
#define	PCSKB4	0x60
#define	PWLB	0x70
#define	PW968	0x80
#define	GA968	0x90

#define	PC98_NEC_INDX1	0xfa2	/* PC98 INDEX1 */
#define	PC98_NEC_DATA1	0xfa3	/* PC98 DATA1  */                 
#define	PC98_NEC_INDX2	0xfaa	/* PC98 INDEX2 */               
#define	PC98_NEC_DATA2	0xfab	/* PC98 DATA2  */                 

#define PW_WinAdd	0xf0	/* 0x00F00000 */
#define XKB_WinAdd	0xea	/* 0x00EA0000 */
#define SKB4_WinAdd	0x3e0	/* 0x03E00000 */
#define NEC_WinAdd	0xf0	/* 0x00F00000 */
#define PWLB_WinAdd	0xf000	/* 0xF0000000 */

#define S3PC98SERVER_ENTER	1
#define S3PC98SERVER_LEAVE	0

Bool s3EnterLeaveMachdep(
#if NeedFunctionPrototypes
	int
#endif
);
