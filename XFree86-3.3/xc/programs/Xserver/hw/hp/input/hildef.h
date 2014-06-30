#ifndef HILDEF_H
#define HILDEF_H
/* $XConsortium: hildef.h,v 8.36 95/01/24 21:13:11 gildea Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright (c) 1988 by Hewlett-Packard Company

Permission to use, copy, modify, and distribute this software 
and its documentation for any purpose and without fee is hereby 
granted, provided that the above copyright notice appear in all 
copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
Hewlett-Packard not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.

*/
/*
** 	File: hildefs.h
**
**	 defines for hil devices to the X environment.
**
*/

#include  "x_serialdrv.h"
#include  "sys/param.h"
#include  "X.h"
#include  "scrnintstr.h"
#include "misc.h"
#include "dixstruct.h"

/***************************************************************/
/* KEEP THE FOLLOWING IN SYNC WITH THE DIX DEFINITION          */
/***************************************************************/

#define	MAXNAMLEN		255
#define READ_SIZ	     	2000	/* leave room for partial packets*/
#define BUF_SIZ			2048	/* size of static buffer to use  */

# define MAX_X_NAMELEN		64
# define MAX_AXES		8
# define ILLEGAL  		-1
# define UP_MASK   		1 << 0
# define HIL_POLL_HDR_BITS	0xE3
# define MOTION_MASK 		0x0F
# define KEY_DATA_MASK 		0x70
# define SET1_KEY_MASK 		1 << 6
# define PROXIMITY_IN   	0x8e
# define PROXIMITY_OUT		0x8f
# define BUTTON_BASE 		0x80
# define BUTTON_1_OFFSET 	0x7e


#define VERTICAL	0
#define HORIZONTAL	1
#define MATRIX		2

#define NOWRAP		0
#define WRAP		1
#define DEFAULT		2
#define SAMESCREEN	3
#define CHANGE_BY_TWO	4

# define CHORDING_OFF		0
# define CHORDING_ON		1
# define CHORDING_DEFAULT	2

# define LATCHING_OFF		0
# define LATCHING_ON		1

# define SCREEN_CHANGE_DEFAULT	255

# define IS_SERIAL_DEVICE	0x40
# define OPEN_THIS_DEVICE	0x20
# define SECOND_LOGICAL_DEVICE	0x10
# define MERGED_DEVICE		0x08

# define DATA_SIZE_BITS		0x07

#define	HIL_ABSOLUTE	0x40	/* Device has absolute positioning data */
#define HIL_16_BITS 	0x20	/* Device has position data 16 bit accuracy */
#define HIL_IOB		0x10	/* Device has I/O description byte */
#define HIL_NUM_AXES	0x03	/* Number of axes supported */

#define HAS_LEDS	0xf0	/* Device has leds                        */
#define HILIOB_PAA	0x80	/* Device supports prompt and acknowledge */
#define HILIOB_NPA	0x70	/* Number of prompts & acknowledges supported */
#define HILIOB_PIO	0x08	/* Device supports Proximity In/Out */
#define HILIOB_BUTTONS	0x07	/* Number of buttons on device */

#define HILPRH_KEYSET   0x60    /* Keycode set bits            */
#define HILPRH_KEYSET1  0x40    /* Keycode set 1 data          */

#define NLOCK		3
#define CAPSCODE	0x37
#define KBSIZE		32	/* bytes to hold 256 bits (1 per key/button */
#define ExpectUpKey(d,code) (d->kb_exp_up[code>>3] |= (1<<(code & 7)))
#define DontExpectUpKey(d,code) (d->kb_exp_up[code>>3] &= ~(1<<(code & 7)))
#define DeviceHasLeds(d) (d->iob & HILIOB_NPA)
#define KeyHasLed(dev,d,cd) ((dev->key->modifierMap[cd] & d->led[0]) || \
(dev->key->modifierMap[cd] & d->led[1]) || \
(dev->key->modifierMap[cd] & d->led[2]) || \
(dev->key->modifierMap[cd] & d->led[3]))

#define UpIsExpected(d,code) (d->kb_exp_up[code>>3] & (1<<(code & 7)))
#define KeyIsIgnored(d,code) (d->kb_ignore[code>>3] & (1<<(code & 7)))
#define IgnoreKey(d,code) (d->kb_ignore[code>>3] |= (1<<(code & 7)))
#define UnignoreKey(d,code) (d->kb_ignore[code>>3] &= ~(1<<(code & 7)))

#define KeyDownEvent(ev) (ev->u.u.type==KeyPress | ev->u.u.type==DeviceKeyPress)
#define ButtonDownEvent(ev) (ev->u.u.type==ButtonPress | \
			     ev->u.u.type==DeviceButtonPress)
#define KeyUpEvent(ev) (ev->u.u.type==KeyRelease | \
			     ev->u.u.type==DeviceKeyRelease)

#define ITF_KATAKANA	0xdd
#define ITF_JAPANESE	0xc2
#define PS2_HIL_JIS	0xc1
#define PS2_DIN_JIS	"PS2_DIN_JIS"
#define IsLockKey(dev,code) (dev->key->modifierMap[code] & LockMask ? \
				LockMapIndex : 0)
#define IsMod2Key(dev,code) (dev->key->modifierMap[code] & Mod2Mask ? \
				Mod2MapIndex : 0)
#define IsJapaneseEnv(pHP)  (pHP->id == ITF_KATAKANA || \
                             pHP->id == ITF_JAPANESE || \
                             pHP->id == PS2_HIL_JIS || \
                             !strcmp(pHP->d.keymap_name, PS2_DIN_JIS))

#define IsToggleKey(dev,pHP,code) (IsLockKey(dev,code))

/* This is the Kana Lock solution that APPO initially wanted.
#define IsToggleKey(dev,pHP,code) (IsLockKey(dev,code) || \
       (IsJapaneseEnv(pHP) && IsMod2Key(dev,code)))
*/

#define KeyIsDown(dev, code) (dev->key && \
    (dev->key->down[code >> 3] & (1 << (code & 7))))
#define KeyIsRepeating(dev, code) (dev->kbdfeed && \
    (dev->kbdfeed->ctrl.autoRepeats[code >> 3] & (1 << (code & 7))))

typedef struct _DeviceClients *DeviceClientsPtr;

typedef struct _DeviceClients {
    DeviceClientsPtr	next;
    ClientPtr		client;	  /* which client wants this device       */
    XID			resource; /* id for putting into resource manager */
    int			mode;
    int			count;	  /* # of open requests for this client   */
} DeviceClients;

typedef  struct	 _indevices {
    u_char	id;		/* device HIL id		*/
    u_char	iob;		/* I/O descriptor Byte 		*/
    float	scaleX; 	/* Tablet scaling 		*/
    float	scaleY; 	/* Tablet scaling 		*/
    DeviceClientsPtr clients;	/* clients using device 	*/
    ScreenPtr	pScreen;  	/* Screen pointer is on         */
    int   	coords[MAX_AXES];/* current coords of device    */
    Atom	x_atom;		/* atom for x type		*/
    u_int	button_state;   /* device button state          */
    int		change_xmax;
    int		change_ymax;
    int		change_ymin;
    int  	change_xmin;
    short	change_amt;
    short	id_detail;
    u_char	dev_type;	/* HIL device type		*/
    u_char  	sent_button;	/* flag for button sent		*/
    u_char  	ignoremask;	/* for button emulation         */
    u_char  	savebutton;	/* saved button			*/
    char	x_type;		/* MOUSE or KEYBOARD		*/ 
    u_char	mode;     	/* abs or rel movement  	*/
    u_char	use;		/* device use                   */
    u_char	pad2;		/* reserved              	*/
    u_char	hpflags;	/* hp-specific feature flags    */
    u_char	led[NLOCK+1];
    u_char	kb_exp_up[KBSIZE];
    u_char	kb_ignore[KBSIZE];
    char 	entry[MAX_NM];
    char	driver_name[MAX_NM];   /* filled in by X server	*/
    int		*dpmotionBuf;
    int		*dheadmotionBuf;
    HPInputDeviceHeader d;
    SerialProcs s;
}  HPInputDevice;


struct	dev_info {
    unsigned int	timestamp;
    unsigned char	poll_hdr;
    unsigned char	dev_data[36];
    HPInputDevice	*hil_dev;
}; 

#endif
