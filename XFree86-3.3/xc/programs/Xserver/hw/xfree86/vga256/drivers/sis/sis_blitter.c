/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_blitter.c,v 1.1 1997/01/12 10:43:06 dawes Exp $ */

/*
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for Sis by Xavier Ducoin (xavier@rd.lectra.fr)
 *
 */


#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfbrrop.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include	"mergerop.h"
#include        "vgaBank.h"

#include "compiler.h"
#include "xf86.h"
#include "vga.h"
#include "sis_driver.h"


#include "sis_Blitter.h"


/* alu to C&T conversion for use with source data */
int sisAluConv[] =
{
    0x00,			       /* dest = 0; GXclear, 0 */
    0x88,			       /* dest &= src; GXand, 0x1 */
    0x44,			       /* dest = src & ~dest; GXandReverse, 0x2 */
    0xCC,			       /* dest = src; GXcopy, 0x3 */
    0x22,			       /* dest &= ~src; GXandInverted, 0x4 */
    0xAA,			       /* dest = dest; GXnoop, 0x5 */
    0x66,			       /* dest = ^src; GXxor, 0x6 */
    0xEE,			       /* dest |= src; GXor, 0x7 */
    0x11,			       /* dest = ~src & ~dest;GXnor, 0x8 */
    0x99,			       /*?? dest ^= ~src ;GXequiv, 0x9 */
    0x55,			       /* dest = ~dest; GXInvert, 0xA */
    0xDD,			       /* dest = src|~dest ;GXorReverse, 0xB */
    0x33,			       /* dest = ~src; GXcopyInverted, 0xC */
    0xBB,			       /* dest |= ~src; GXorInverted, 0xD */
    0x77,			       /*?? dest = ~src|~dest ;GXnand, 0xE */
    0xFF,			       /* dest = 0xFF; GXset, 0xF */
};


int
sisMMIOSetFGColorBPP(fgcolor, mask)
    unsigned long fgcolor;
    unsigned int mask;
{
    switch (vgaBitsPerPixel) {
    case 8:
	sisSETFGCOLOR8(fgcolor);
	mask = 0xFF;
	break;
    case 16:
	sisSETFGCOLOR16(fgcolor);
	mask = 0xFFFF;
	break;
    case 24:
	sisSETFGCOLOR24(fgcolor);
	mask = 0xFFFFFF;
	break;
    }
    return 0;
}


int
sisMMIOSetBGColorBPP(bgcolor, mask)
    unsigned long bgcolor;
    unsigned int mask;
{
    switch (vgaBitsPerPixel) {
    case 8:
	sisSETBGCOLOR8(bgcolor);
	mask = 0xFF;
	break;
    case 16:
	sisSETBGCOLOR16(bgcolor);
	mask = 0xFFFF;
	break;
    case 24:
	sisSETBGCOLOR24(bgcolor);
	mask = 0xFFFFFF;
	break;
    }
    return 0;
}

/*lowlevel function */

void
sisMMIOBitBlt(psrcBase, pdstBase, widthSrc, widthDst, x, y,
    x1, y1, w, h, xdir, ydir, alu, planemask)
    unsigned char *psrcBase, *pdstBase;		/* start of src and dst bitmaps */
    int widthSrc, widthDst;
    int x, y, x1, y1, w, h;
    int xdir, ydir;
    int alu;
    unsigned long planemask;
{
    unsigned long pSrc, pDst;
    unsigned int op,rop;

#ifdef DEBUG
    ErrorF("SIS:sisMMIOBitBlt(0x%X, 0x%X, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %x, %x)\n",
	psrcBase, pdstBase, widthSrc, widthDst, x, y, x1, y1, w, h,
	xdir, ydir, alu, planemask);
#endif

    if (alu & 0xF == GXnoop)
	return;

    op = sisCMDBLT ;

    if (alu & 0x10) {
	/* Source is Background colour */
	op |= sisSRCBG;
    } 

    rop = sisAluConv[alu & 0xF];
    
    if (alu & 0x40) {
	ErrorF("not yet suppoerted\n");
	/*op |= sisBGTRANSPARENT;	       BG transparent */
    }

    if (psrcBase && !CHECKSCREEN(psrcBase))
	op |= sisSRCSYSTEM;	       /*source is system memory */
    else if (  !(alu&0x10))
	op |= sisSRCVIDEO;

    if (xdir == 1) {
	op |= sisLEFT2RIGHT;
	pSrc = x;
	pDst = x1;
    } else {
	op |= sisRIGHT2LEFT;
	pSrc = x + w - 1;
	pDst = x1 + w - 1;
    }
    if (ydir == 1) {
	op |= sisTOP2BOTTOM;
	pSrc += y * widthSrc;
	pDst += y1 * widthDst;
    } else {
	op |= sisBOTTOM2TOP;
	pSrc += (y + h - 1) * widthSrc;
	pDst += (y1 + h - 1) * widthDst;
    }

    pSrc *= vgaBytesPerPixel;
    pDst *= vgaBytesPerPixel;

    sisBLTWAIT;
#if 1
    if ( (vgaBytesPerPixel>1) && !(alu & 0x10) && (xdir == -1) ) {
        /* Hack for bpp > 8.
	 * With the copyArea somes points remain.
         * so need more investigations.
	 */
        pSrc += vgaBytesPerPixel-1; 
	pDst += vgaBytesPerPixel-1;
    }
#endif
    sisSETROP(rop);
    sisSETSRCADDR(pSrc);
    sisSETDSTADDR(pDst);
    sisSETHEIGHTWIDTH(h-1, w * vgaBytesPerPixel-1);
    sisSETPITCH(widthSrc * vgaBytesPerPixel, widthDst * vgaBytesPerPixel);
    sisSETCMD(op);
    sisBLTWAIT;

}





/*
 * This function performs a 8x8 tile fill with the BitBLT engine.
 */


void
sisMMIOBLT8x8PatternFill(destaddr, w, h, pattern, yrot, patternpitch,
    destpitch)
    unsigned int destaddr;
    int w, h, yrot;
    unsigned char *pattern;
    int patternpitch;
    int destpitch;
{
    int 		i,j;
    unsigned char	*sisBLTPatternRegister;

#ifdef DEBUG
    ErrorF("SIS:sisMMIOBLT8x8PatternFill(%d, %d, %d, 0x%X, %d, %d)\n",
	destaddr, w, h, pattern, patternpitch, destpitch);
#endif

    sisBLTPatternRegister = sisSETPATREG();

    for (i = 0; i < 8; i++) {
	for ( j=0; j < 8 ; j++) {
	    *sisBLTPatternRegister = *( (unsigned char *)pattern +
	       patternpitch * (( i + yrot)%8) + j ) ;
	    sisBLTPatternRegister++;
	}
    }
    sisSETPITCH(8 * vgaBytesPerPixel, destpitch * vgaBytesPerPixel);
    sisSETDSTADDR(destaddr * vgaBytesPerPixel);
    sisSETHEIGHTWIDTH(h-1, w * vgaBytesPerPixel-1);
    sisSETCMD(sisCMDBLT | sisPATREG | sisTOP2BOTTOM | sisLEFT2RIGHT );
    sisBLTWAIT;
}





