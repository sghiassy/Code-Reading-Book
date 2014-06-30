#ifndef __CIR_BLITLG_H
#define __CIR_BLITLG_H

/* $XConsortium: cir_blitLG.h /main/3 1996/10/25 10:31:11 kaleb $ */
/*
 *
 * Copyright 1996 by Corin Anderson, Bellevue, Washington, USA
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Corin Anderson not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Corin Anderson makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * CORIN ANDERSON DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL CORIN ANDERSON BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Corin Anderson, <corina@bdc.cirrus.com>
 *
 * cir_blitLG.h
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_blitLG.h,v 3.7 1997/01/19 12:50:55 dawes Exp $ */


/* This header file defines the necessary structures, contstants, and 
   variables for using the bitBLT engine on a Laguna family graphics 
   accelerator.  */


void CirrusLgDoBitbltCopy();
void CirrusLgFillRectSolidCopy ();
void CirrusLgFillBoxSolid ();
void CirrusLgPolyFillRect();
void CirrusLgFillRectSolid(
    DrawablePtr	    pDrawable,
    GCPtr	    pGC,
    int		    nBox,
    BoxPtr	    pBox);
extern RegionPtr CirrusLgCopyArea16();
extern RegionPtr CirrusLgCopyArea24();
extern RegionPtr CirrusLgCopyArea32();
extern void CirrusLgCopyWindow();


enum {                            /* Offsets into MMIO space for bitBLT regs */
  STATUS       = 0x0400,
  OP0_opRDRAM  = 0x0520,
  OP1_opRDRAM  = 0x0540,
  OP2_opRDRAM  = 0x0560,
  OP0_opMRDRAM = 0x0524,
  OP1_opMRDRAM = 0x0544,
  OP2_opMRDRAM = 0x0564,
  OP0_opSRAM   = 0x0528,
  OP1_opSRAM   = 0x0548,
  OP2_opSRAM   = 0x0568,
  OP1_opMSRAM  = 0x054A,
  OP2_opMSRAM  = 0x056A,
  DRAWDEF      = 0x0584,
  BLTDEF       = 0x0586,
  BLTEXT_EX    = 0x0700,
  MBLTEXT_EX   = 0x0720,
  MONOQW       = 0x0588,
  QFREE        = 0x0404,
  PATOFF       = 0x052A,
  HOSTDATA     = 0x0800,
  OP_opBGCOLOR = 0x05E4,
  OP_opFGCOLOR = 0x05E0,
  bltCONTROL   = 0x0402,
  BITMASK      = 0x05E8
};

enum { HOSTDATASIZE = 2048 };   /* The HOSTDATA port is 2048 BYTES */
 
enum {                      /* OR these together to form a bitBLT mode */
  HOST2SCR   = 0x1120,      /* CPU/Screen transfer modes */
  SCR2HOST   = 0x2010,
  HOST2PAT   = 0x1102,
  HOST2SRAM2 = 0x6020,      /* CPU to SRAM2 transfer */

  SCR2SCR    = 0x1110,      /* Screen/Screen transfers */
  COLORSRC   = 0x0000,      /* Source is color data */
  MONOSRC    = 0x0040,      /* Source is mono data (color expansion) */
  COLORTRANS = 0x0009,      /* Transparent screen/screen transfer */
  COLORFILL  = 0x0070,      /* Solid color fill mode */
  SRAM1SCR2SCR = 0x1180,    /* Pattern fill, source from SRAM1 */

  PAT2SCR    = 0x1109,      /* Pattern/Screen transfers */
  COLORPAT   = 0x0000,      /* Pattern is color data */
  MONOPAT    = 0x0004,      /* Pattern is mono data (color expansion) */
  SRAM2PAT2SCR   = 0x1108,  /* SRAM2 is pattern source */

  PATeqSRC   = 0x0800,      /* The Pattern and Source operands are the same */

  BLITUP     = 0x8000       /* The blit is proceeding from bottom to top */
};

enum {                      /* Select transparency compare */
  TRANSBG   = 0x0100,
  TRANSFG   = 0x0300,
  TRANSEQ   = 0x0100,
  TRANSNE   = 0x0300,
  TRANSNONE = 0x0000
};

extern int lgCirrusRop[16];  /* Defined in cir_blitLG.c */


/* Use the function call as opposed to the macro.  The reason is that
   the function call uses a volatile variable, which is necessary for 
   forcing the STATUS register to be read each time.  Using just the macro,
   only one read will be performed, and the machine will hang. */
int LgReady(void);
/*
#define LgREADY() \
  ((*(unsigned char *)(cirrusMMIOBase + STATUS) & 0x07) == 0x00)
*/

#define LgSETROP(rop) \
  *(unsigned short *)(cirrusMMIOBase + DRAWDEF) = (rop);

#define LgSETTRANSPARENCY(trans) \
  *(unsigned short *)(cirrusMMIOBase + DRAWDEF) = \
  (trans) | (*(unsigned short *)(cirrusMMIOBase + DRAWDEF) & 0x00FF);

#define LgSETMODE(mode) \
  *(unsigned short *)(cirrusMMIOBase + BLTDEF) = (mode);

#define LgSETDSTXY(X, Y) \
  *(unsigned long *)(cirrusMMIOBase + OP0_opRDRAM) = (((Y) << 16) | (X));

#define LgSETSRCXY(X, Y) \
  *(unsigned long *)(cirrusMMIOBase + OP1_opRDRAM) = (((Y) << 16) | (X));
    
#define LgSETPATXY(X, Y) \
  *(unsigned long *)(cirrusMMIOBase + OP2_opRDRAM) = (((Y) << 16) | (X));

#define LgSETTRANSMASK(X, Y) LgSETPATXY(X, Y)

#define LgSETSRAMDST(offset) \
  *(unsigned short *)(cirrusMMIOBase + OP0_opSRAM) = (offset);

#define LgSETSRAM1OFFSET(offset) \
  *(unsigned short *)(cirrusMMIOBase + OP2_opSRAM) = (offset);

#define LgSETSRAM2OFFSET(offset) \
  *(unsigned short *)(cirrusMMIOBase + OP2_opSRAM) = (offset);

#define LgSETMSRAM1OFFSET(offset) \
  *(unsigned short *)(cirrusMMIOBase + OP1_opMSRAM) = (offset);

#define LgSETMSRAM2OFFSET(offset) \
  *(unsigned short *)(cirrusMMIOBase + OP2_opMSRAM) = (offset);

#define LgSETMDSTXY(X, Y) \
  *(unsigned long *)(cirrusMMIOBase + OP0_opMRDRAM) = (((Y) << 16) | (X));

#define LgSETMSRCXY(X, Y) \
  *(unsigned long *)(cirrusMMIOBase + OP1_opMRDRAM) = (((Y) << 16) | (X));
    
#define LgSETMPATXY(X, Y) \
  *(unsigned long *)(cirrusMMIOBase + OP2_opMRDRAM) = (((Y) << 16) | (X));

#define LgSETMTRANSMASK(X, Y) LgSETMPATXY(X, Y)

#define LgSETPHASE0(phase) \
  *(unsigned long *)(cirrusMMIOBase + OP0_opRDRAM) = (phase);

#define LgSETPHASE1(phase) \
  *(unsigned long *)(cirrusMMIOBase + OP1_opRDRAM) = (phase);

#define LgSETPHASE2(phase) \
  *(unsigned long *)(cirrusMMIOBase + OP2_opRDRAM) = (phase);

#define LgSETMPHASE0(phase) \
  *(unsigned long *)(cirrusMMIOBase + OP0_opMRDRAM) = (phase);

#define LgSETMPHASE1(phase) \
  *(unsigned long *)(cirrusMMIOBase + OP1_opMRDRAM) = (phase);

#define LgSETEXTENTS(width, height)  \
  *(unsigned long *)(cirrusMMIOBase + BLTEXT_EX) = (((height) << 16)|(width));

#if 0
#define LgSETMEXTENTS(width, height)  \
  *(unsigned long *)(cirrusMMIOBase + MBLTEXT_EX) = (((height) << 16)|(width));
#else
/* For monochrome (byte) blits, we need to set how many QWORDs of data 
   encompass the X extent.  Write this piece of data into MONOQW. */
#define LgSETMEXTENTS(width, height)  \
  { \
    *(unsigned short *)(cirrusMMIOBase + MONOQW) = ((width + 7) >> 3);  \
    *(unsigned long *)(cirrusMMIOBase + MBLTEXT_EX) = \
           (((height) << 16)|(width));  \
  }

/*
    *(unsigned short *)(cirrusMMIOBase + MBLTEXT_EX) = height;
    *(unsigned short *)(cirrusMMIOBase + MBLTEXT_EX + 2) = width;
*/
#endif

#define LgHOSTDATAWRITE(data)  \
  *(unsigned long *)(cirrusMMIOBase + HOSTDATA) = (data);

#define LgHOSTDATAREAD()  \
  (*(unsigned long *)(cirrusMMIOBase + HOSTDATA))

#define LgSETBACKGROUND(color) \
  *(unsigned long *)(cirrusMMIOBase + OP_opBGCOLOR) = (color);

#define LgSETFOREGROUND(color) \
  *(unsigned long *)(cirrusMMIOBase + OP_opFGCOLOR) = (color);

#define LgSETPATOFF(xoff, yoff) \
  *(unsigned short *)(cirrusMMIOBase + PATOFF) = (((yoff) << 8) | (xoff));

#define LgSETSWIZZLE() \
  *(unsigned short *)(cirrusMMIOBase + bltCONTROL) |= 0x0400;

#define LgCLEARSWIZZLE() \
  *(unsigned short *)(cirrusMMIOBase + bltCONTROL) &= ~0x0400;

#define LgSETBITMASK(m) \
  *(unsigned int *)(cirrusMMIOBase + BITMASK) = m;


#endif  /* __CIR_BLITLG_H */
