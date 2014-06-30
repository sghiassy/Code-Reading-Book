/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/xf86RamDac.h,v 3.6 1996/12/23 06:33:10 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Henry A. Worth,  Sunnyvale, California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin and Henry A.
 * Worth make no representations about the suitability of this software 
 * for any purpose.  It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR 
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Amancio Hasty and Jon Tombs
 * Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 *
 */
/* $XConsortium: xf86RamDac.h /main/6 1996/02/21 17:19:05 kaleb $ */

#ifndef XF86_RAMDAC_H
#define XF86_RAMDAC_H

#include "compiler.h"
#include <X11/Xfuncproto.h>


#define UNKNOWN_DAC       -1
#define NORMAL_DAC         0
#define BT485_DAC          1
#define ATT20C505_DAC      2
#define TI3020_DAC         3
#define BT481_DAC          4
#define BT482_DAC          5
#define BT484_DAC          6
#define ATT20C504_DAC      7
#define SC15025_DAC	   8
#define SC15021_DAC	   9
#define HERC_SMALL_DAC	  10
#define HERC_DUAL_DAC	  11
#define XGA_DAC           12
#define ATT490_DAC        13

#define DAC_IS_BT485_SERIES    (xf86RamDacType == BT485_DAC \
			        || xf86RamDacType == BT484_DAC \
			        || xf86RamDacType == ATT20C505_DAC \
			        || xf86RamDacType == ATT20C504_DAC)
#define DAC_IS_TI3020	       (xf86RamDacType == TI3020_DAC)
#define DAC_IS_BT481_SERIES    (xf86RamDacType == BT481_DAC \
			        || xf86RamDacType == BT482_DAC)
#define HERC_DAC_PROBE 	       (xf86RamDacType == HERC_SMALL_DAC \
			        || xf86RamDacType == HERC_DUAL_DAC)
#define DAC_IS_SC1502X         (xf86RamDacType == SC15025_DAC \
                                || xf86RamDacType == SC15021_DAC)
#define DAC_IS_ATT490	       (xf86RamDacType == ATT490_DAC)

extern Bool xf86DacSyncOnGreen;
extern Bool xf86Dac8Bit;
extern int  xf86RamDacType;
extern int  xf86RamDacBPP;
extern int  xf86MaxCurs;
extern int  xf86FrameX0;
extern int  xf86FrameY0;
extern int  xf86MaxClock;

extern unsigned char xf86SwapBits[256];

/* union for saving/restoring RamDac state */
union xf86RamDacSave {

   struct {
       unsigned char Com0;
       unsigned char Com1;
       unsigned char Com2;
       unsigned char Com3;
   } Bt485;
 
   struct {
       unsigned char ComA;
       unsigned char ComB;
   } Bt481;

   struct {
       unsigned char Cmd;
       unsigned char AuxCntl;
       unsigned char SecCntl;
   } Sc1502x;

   struct {
       unsigned char Dummy;
   } Ti;

   struct {
       unsigned char ComA;
   } Att490;



};

 
_XFUNCPROTOBEGIN

extern void (*xf86OutRamDacReg)(
#if NeedFunctionPrototypes
        unsigned short,
        unsigned char,
        unsigned char
#endif
);

extern void (*xf86OutRamDacData)(
#if NeedFunctionPrototypes
        unsigned short,
        unsigned char
#endif
);

extern unsigned char (*xf86InRamDacReg)(
#if NeedFunctionPrototypes
        unsigned short
#endif
);

extern void (*xf86RamDacHWSave)(
#if NeedFunctionPrototype
        union xf86RamDacSave *
#endif
);

extern void (*xf86RamDacHWRestore)(
#if NeedFunctionPrototype
        union xf86RamDacSave *
#endif
);

extern void (*xf86RamDacInit)(
#if NeedFunctionPrototype
        void
#endif
);


/* generic RAMDAC probe routine */
extern void xf86ProbeRamDac(
#if NeedFunctionPrototype
        void
#endif
); 

/* RAMDAC globals setup routine */
extern void xf86SetUpRamDac(
#if NeedFunctionPrototype
        void
#endif
); 

extern void xf86RamDacHWNoop( 
#if NeedFunctionPrototype
        union xf86RamDacSave * save )
#endif
);

extern void xf86RamDacNoopInit(
#if NeedFunctionPrototype
        void
#endif
);


_XFUNCPROTOEND

#endif /* XF86_RAMDAC_H */
