/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_driver.h,v 1.1 1997/01/12 10:43:10 dawes Exp $ */

/*
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the authors not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The authors makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THE AUTHORS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for Sis by Xavier Ducoin (xavier@rd.lectra.fr)
 *
 */


/*#define DEBUG*/

#define SIS86C201 0
#define SIS86C202 1
#define SIS86C205 2

extern int SISchipset;

extern Bool sisLinearSupport;	       /*linear addressing enable */

extern Bool sisUseMMIO;
extern unsigned char *sisMMIOBase;
extern unsigned int sisBLTPatternAddress;
extern int sisBLTPatternOffscreenSize;
extern Bool sisAvoidImageBLT;
extern unsigned char *sisBltDataWindow;

extern Bool sisHWCursor;

extern int sisAluConv[];		       /* Map Alu to SIS ROP source data  */

/* 
 * Definitions for IO access to 32 bit ports
 */
extern int sisReg32MMIO[];
#define BR(x) sisReg32MMIO[x]


/*
 * Forward definitions for the functions that make up the driver.    See
 * the definitions of these functions for the real scoop.
 */

/* in sis_blitter.c */
extern void sisBitBlt();
extern void sisMMIOBitBlt();

/* in sis_BitBlt.c */
extern void siscfbDoBitbltCopy();
extern void siscfbFillBoxSolid();

/* in sis_solid.c */
extern void siscfbFillRectSolid();
extern void siscfbFillSolidSpansGeneral();
extern void sisMMIOFillRectSolid();
extern void sisMMIOFillSolidSpansGeneral();

/* in sis_blt16.c */
extern RegionPtr siscfb16CopyArea();
extern RegionPtr siscfb24CopyArea();
extern void siscfbCopyWindow();

/* in sis_line.c */
extern void sisMMIOLineSS();
extern void sisMMIOSegmentSS();

/* in sis_pntwin.c */
extern void sisPaintWindow();

/* in sis_FillRct.c */
extern void siscfbPolyFillRect();

/* in ct_FillSt.c */
extern void siscfbFillRectOpaqueStippled32();
extern void siscfbFillRectTransparentStippled32();
extern void sisMMIOFillRectOpaqueStippled32();
extern void sisMMIOFillRectTransparentStippled32();

/* in sis_teblt8.c */
extern void sisMMIOImageGlyphBlt();
extern void sisMMIOPolyGlyphBlt();





