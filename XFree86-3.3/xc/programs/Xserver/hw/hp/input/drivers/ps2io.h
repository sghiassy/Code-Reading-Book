/* $XConsortium: ps2io.h,v 1.1 94/05/28 14:32:49 dpw Exp $ */
/*
 * @(#)ps2io.h: Revision: 1.1 $ $Date: 94/12/15 09:22:14
 * Locker:  
 */

#ifndef _SYS_PS2IO_INCLUDED /* allows multiple inclusion */
#define _SYS_PS2IO_INCLUDED

#ifndef _SYS_STDSYMS_INCLUDED
#ifdef _KERNEL_BUILD
#    include "../h/stdsyms.h"
#else  /* ! _KERNEL_BUILD */
#    include <sys/stdsyms.h>
#endif /* _KERNEL_BUILD */
#endif   /* _SYS_STDSYMS_INCLUDED  */

#ifdef _KERNEL_BUILD
#  include "../h/ioctl.h"
#else  /* ! _KERNEL_BUILD */
#  include <sys/ioctl.h>
#endif /* _KERNEL_BUILD */

struct ps2_4 {
   unsigned char b[4];
};

/*
 * Device ioctl() command defines
 */

#define PS2_INDICATORS 	_IOW('P', 0x01, struct ps2_4)
#define PS2_IDENT	_IOR('P', 0x02, struct ps2_4)
#define PS2_SCANCODE	_IOWR('P', 0x03, struct ps2_4)
#define PS2_ENABLE	_IO('P', 0x04)
#define PS2_DISABLE	_IO('P', 0x05)
#define PS2_STREAMMODE	_IO('P', 0x06)
#define PS2_SAMPLERATE  _IOW('P', 0x07, struct ps2_4)
#define PS2_RESET	_IOR('P', 0x08, struct ps2_4)
#define PS2_RESOLUTION	_IOW('P', 0x09, struct ps2_4)
#define PS2_ALL_TMAT 		_IO('P', 0x0A)
#define PS2_ALL_MKBRK 		_IO('P', 0x0B)
#define PS2_ALL_TMAT_MKBRK 	_IO('P', 0x0C)
#define PS2_ALL_MK 		_IO('P', 0x0D)
#define PS2_KEY_MKBRK		_IOW('P', 0x0E, struct ps2_4)
#define PS2_KEY_MAKE		_IOW('P', 0x0F, struct ps2_4)
#define PS2_KEY_TMAT		_IOW('P', 0x10, struct ps2_4)
#define PS2_RATEDELAY		_IOW('P', 0x11, struct ps2_4)

#define PS2_PORTSTAT	_IOR('P', 0x12, struct ps2_4)
#define PS2_TEST	_IOW('P', 0x13, char)
#define PS2_SETDEFAULT	_IO('P', 0x14)
#define PS2_PROMPTMODE	_IO('P', 0x15)
#define PS2_REPORT	_IOR('P', 0x16, struct ps2_4)
#define PS2_STATUS	_IOR('P', 0x17, struct ps2_4)
#define PS2_2TO1_SCALING	_IO('P', 0x18)
#define PS2_1TO1_SCALING	_IO('P', 0x19)

#define PS2FAKE _IOW('P',0x0F,char)     /* fake a character */

/*
#define _IOR('P',0x ,)
 */

/* Values for PS2_PORTSTAT first return byte */

#define PS2_NONE        0
#define PS2_MOUSE       1
#define PS2_KEYBD       2
#define PS2_UNKNOWN     3

/* Bit mask values for ps2 devices */

#define PS2_NONE_BIT    0x0001
#define PS2_MOUSE_BIT   0x0002
#define PS2_KEYBD_BIT   0x0004
#define PS2_UNKNOWN_BIT 0x0008

/* Bit mask values for PS2_PORTSTAT second return byte */

#define INTERFACE_HAS_ITE	0x01
#define PORT_HAS_FIRST_KEYBD	0x02
#define PORT_HAS_FIRST_MOUSE	0x04

/* values for PS2_SCANCODE */

#define GET_SCANCODE	0x00
#define SCANCODE_1	0x01
#define SCANCODE_2	0x02
#define SCANCODE_3	0x03

/* values for PS2_SAMPLERATE */

#define SAMPLE_10	0x0A
#define SAMPLE_20	0x14
#define SAMPLE_40	0x28
#define SAMPLE_60	0x3C
#define SAMPLE_80	0x50
#define SAMPLE_100	0x64
#define SAMPLE_200	0xC8

/* values for PS2_INDICATORS */

#define  CAPS_LED	0x04
#define  NUM_LED	0x02
#define  SCROLL_LED	0x01

/* values for PS2_RESOLUTION */
/*  (RES_3 is te default)             Resolution in counts/mm */
                               /*  200 DPI          320 DPI   */
			       /*---------          -------   */
#define	RES_1		0x00   /*        1                1   */
#define	RES_2		0x01   /*        2                3   */
#define	RES_3		0x02   /*        4                6   */
#define	RES_4		0x03   /*        8               12   */

#endif /* _SYS_PS2IO_INCLUDED */
