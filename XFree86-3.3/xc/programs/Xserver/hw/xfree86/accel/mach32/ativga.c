/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/ativga.c,v 3.6 1996/12/23 06:38:18 dawes Exp $ */
/***************************************************************************
 * Start of VGA font saving and restoration code.
 * Created: Sun Jun 27 12:50:09 1993 by faith@cs.unc.edu
 *
 * Reference used:
 *   "VGA WONDER Programmer's Reference,"
 *      ATI Technologies, 1991.
 *      Release 1.2 -- Reference #PRG28800
 *      (Part No. 10709B0412)
 *
 * Some code from xf86/vga256/drivers/ati/driver.c, which was written by:
 * Thomas Roell (roell@informatik.tu-muenchen.de), Per Lindqvist
 * (pgd@compuram.bbt.se), Doug Evans (dje@sspiff.UUCP), and Rik Faith
 * (faith@cs.unc.edu).
 *
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by Rickard E. Faith
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Thomas Roell not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THOMAS ROELL, PER LINDQVIST, DOUG EVANS, RICKARD E. FAITH AND KEVIN
 * E. MARTIN DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL, PER LINDQVIST, DOUG EVANS, RICKARD E. FAITH
 * OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 *
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: ativga.c /main/5 1996/02/21 17:25:49 kaleb $ */


#include "X.h"
#include "misc.h"
#include "input.h"
#include "dix.h"
#include "os.h"
#include "xf86.h"
#include "xf86_OSproc.h"
#include "vga.h"
#include "regmach32.h"

typedef struct {
   vgaHWRec std;
   unsigned char ATIExtRegBank[11]; /* ATI Registers B0,B1,B2,B3,
				       B5, B6,B8,B9, BE,A6,A7 */
} SaveBlock;

pointer        vgaBase  = NULL;
pointer        vgaNewVideoState = NULL;

#define save   ((SaveBlock *)vgaNewVideoState)

static short   ATIExtReg = 0x1ce;
int            vgaIOBase;
int            vgaInterlaceType = VGA_DIVIDE_VERT;

/* Keep vgaHW.c happy */
void (*vgaSaveScreenFunc)() = vgaHWSaveScreen;

#define ER_B0   0               /* Extended Register indices (ATIExtRegBank) */
#define ER_B1   1
#define ER_B2   2
#define ER_B3   3
#define ER_B5   4
#define ER_B6   5
#define ER_B8   6
#define ER_B9   7
#define ER_BE   8
#define ER_A6   9
#define ER_A7   10

#define ATIReg0  ATIExtRegBank[ER_B0]
#define ATIReg1  ATIExtRegBank[ER_B1]
#define ATIReg2  ATIExtRegBank[ER_B2]
#define ATIReg3  ATIExtRegBank[ER_B3]
#define ATIReg5  ATIExtRegBank[ER_B5]
#define ATIReg6  ATIExtRegBank[ER_B6]
#define ATIReg8  ATIExtRegBank[ER_B8]
#define ATIReg9  ATIExtRegBank[ER_B9]
#define ATIRegE  ATIExtRegBank[ER_BE]
#define ATIRegA6 ATIExtRegBank[ER_A6]
#define ATIRegA7 ATIExtRegBank[ER_A7]

static int inATI(index)
     int index;
{
   outb(ATIExtReg, index);
   return inb(ATIExtReg + 1);
}

void mach32SaveVGAInfo(screen_idx)
     int screen_idx;
{
   unsigned char b2_save;
   unsigned char b8_save;

   if (!vgaBase) {
      vgaBase = xf86MapVidMem(screen_idx, VGA_REGION, (pointer)0xa0000,
			      64 * 1024);
   }

   vgaIOBase = (inb(0x3cc) & 0x01) ? 0x3D0 : 0x3B0;
   
   /* This part is copied from ATISave() in
    * xf86/vga256/drivers/ati/driver.c
    */

   /* Unlock ATI specials */
   outb(ATIExtReg, (((b8_save = inATI(0xb8)) & 0xC0) << 8) | 0xb8);

   b2_save = inATI(0xb2);
   outb(ATIExtReg, 0x00b2);	/* segment select 0 */

   vgaNewVideoState = vgaHWSave(vgaNewVideoState, sizeof(SaveBlock));

   save->ATIReg0  = inATI(0xb0);
   save->ATIReg1  = inATI(0xb1);
   save->ATIReg2  = b2_save;
   save->ATIReg5  = inATI(0xb5);
   save->ATIReg6  = inATI(0xb6);
   save->ATIReg3  = inATI(0xb3);
   save->ATIReg8  = b8_save;
   save->ATIRegE  = inATI(0xbe);
   save->ATIReg9  = inATI(0xb9);
   save->ATIRegA6 = inATI(0xa6);
   save->ATIRegA7 = inATI(0xa7);
   
#if 0
  /*
   * TvR: don't know whether this is neccesary for mach32
   * Should be removed if it isn't needed. 
   */
   /* Enable video */
   inb(vgaIOBase + 0x0A); /* reset flip-flop */
   outb( 0x3c0, 0x20 );
#endif
}

void mach32RestoreVGAInfo()
{
   /* This routine is mostly from ATIRestore() in
    * xf86/vga256/drivers/ati/driver.c
    */

   /* Unlock ATI specials */
   outw(ATIExtReg, ((inATI(0xb8) & 0xC0) << 8) | 0xb8);

   /* Load ATI Extended registers */
   outw(ATIExtReg, (save->ATIReg0 << 8) | 0xb0);
   outw(ATIExtReg, (save->ATIReg1 << 8) | 0xb1);
   outw(ATIExtReg, (save->ATIReg2 << 8) | 0xb2);
   outw(ATIExtReg, (save->ATIReg3 << 8) | 0xb3);
   outw(ATIExtReg, (save->ATIReg5 << 8) | 0xb5);
   outw(ATIExtReg, (save->ATIReg6 << 8) | 0xb6);
   outw(ATIExtReg, (save->ATIRegE << 8) | 0xbE);
   outw(ATIExtReg, (save->ATIReg9 << 8) | 0xb9);
   outw(ATIExtReg, (save->ATIRegA6 << 8) | 0xa6);
   outw(ATIExtReg, (save->ATIRegA7 << 8) | 0xa7);

   /*
    * Last but not least ATIReg8 -- according to vgadoc's this lock the 
    * ATI special registers
    */
   outw(ATIExtReg, (save->ATIReg8 << 8) | 0xb8);

   /*
    * Restore the generic vga registers
    */
   vgaHWRestore((vgaHWPtr)save);
}

/*
 * End of VGA font saving and restoration code.
 *
 ***************************************************************************/
