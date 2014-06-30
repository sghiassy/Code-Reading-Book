/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_colexp.c,v 1.2 1997/01/12 10:52:28 dawes Exp $ */

/*
 *
 * Copyright 1994 by H. Hanemaayer, Utrecht, The Netherlands
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of H. Hanemaayer not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  H. Hanemaayer makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * H. HANEMAAYER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL H. HANEMAAYER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  H. Hanemaayer, <hhanemaa@cs.ruu.nl>
 */
/*
 * Modified for use with Chips by David Bateman <dbateman@ee.uts.edu.au>
 * Modified for Sis by Xavier Ducoin (xavier@rd.lectra.fr)
 */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "xf86.h"
#include "vgaBank.h"
#include "vga.h"		       /* For vgaInfoRec. */
#include "xf86_HWlib.h"
#include "xf86_OSlib.h"

#include "compiler.h"
#include "sis_driver.h"


#include "sis_Blitter.h"


/* Bit masks for left edge (indexed with first left-to-right bit number). */

static unsigned char leftbitmask[8] =
{
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};

/* Bit masks for right edge (indexed with number of pixels left). */

static unsigned char rightbitmask[9] =
{
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe, 0xff
};

/* Rotate the bits of double-word left */
#define rotateleft(n, bits) (((bits) << (n)) | ((bits) >> (32 - (n))))

void 
sisMMIOColorExpandStippleFill(x, y, w, h, bits_in, sh, sox, soy, bg,
    fg, destpitch, isTransparent)
    int x, y, w, h;
    unsigned long *bits_in;
    int sh, sox, soy, bg, fg, destpitch;
    int isTransparent;
{
    int destaddr;
    int bank;
    unsigned char *base;	       /* Video window base address. */
    int nspans, bitoffset;
    int leftmask, leftbcount, midlcount, rightbcount, rightmask;
    int bytealignment;

    unsigned long *new_bits;
    int i, syindex;
    int n, minlines, oneextralimit, startsyindex;
    unsigned char *destp;
    int line, srcline;
    unsigned char	*patternRegPtr ;
    int 		j, k;
    unsigned int	op ;
    int			srcpitch ;
    int			patternHeight ;


#ifdef DEBUG
    ErrorF("SIS:sisMMIOColorExpandStippleFill(%d, %d, %d, %d, %d, %d, %d, %d)\n",
	x, y, w, h, sh, sox, soy, destpitch);
#endif

    if (w == 0)
	return;

    /* Reverse per-byte bit order of the stipple. */
    new_bits = (unsigned long *)ALLOCATE_LOCAL(sh * 4);
    for (i = 0; i < sh; i++) {
	if (bits_in[i] == 0xffffffff)
	    new_bits[i] = 0xffffffff;
	else {
	    unsigned long bits;

	    /* Rotate so that data is correctly aligned to */
	    /* origin for writing dwords to framebuffer. */
	    bits = rotateleft(32 - ((x-sox)%32), bits_in[i]);
	    /* Reverse each of the four bytes. */
	    ((unsigned char *)new_bits)[i * 4] =
		byte_reversed[(unsigned char)bits];
	    ((unsigned char *)new_bits)[i * 4 + 1] =
		byte_reversed[(unsigned char)(bits >> 8)];
	    ((unsigned char *)new_bits)[i * 4 + 2] =
		byte_reversed[(unsigned char)(bits >> 16)];
	    ((unsigned char *)new_bits)[i * 4 + 3] =
		byte_reversed[(unsigned char)(bits >> 24)];
	}
    }
    destaddr = y * destpitch + x;
    sisSETDSTADDR(destaddr);

    op  = sisCMDCOLEXP | sisTOP2BOTTOM | sisLEFT2RIGHT ;

    if (isTransparent) {
	sisSETFGCOLOR8(fg);
	sisSETROPFG(0xf0); 	/* pat copy */
	sisSETROPBG(0xAA); 	/* dst */
	op |= sisPATFG | sisSRCBG ; 
    } else {
	sisSETBGCOLOR8(bg);
	sisSETFGCOLOR8(fg);
	sisSETROPFG(0xf0);	/* pat copy */
	sisSETROPBG(0xcc); 	/* copy */
	op |= sisPATFG | sisSRCBG ;
    }
    

    if ( SISchipset == SIS86C205 )
	patternHeight = 16 ;
    else
	patternHeight = 8 ;

    if ( (sh  <= patternHeight) && (sh%patternHeight == 0) ) {
	patternRegPtr =  sisSETPATREG();
	k = 0 ;
	for ( j = 0 ; j < 4 ; j++ ) {
	    for ( i = 0 ; i < patternHeight ; i++ ) {
		patternRegPtr[k++] = ((unsigned char *)new_bits)[((i+y-soy)%sh)*4+j];
	    }
	}
	sisSETSRCADDR(0);
	op |= sisPATREG | sisSRCSYSTEM ;
	srcpitch = 0;
	sisSETHEIGHTWIDTH(h-1, w-1);
	sisSETPITCH(srcpitch, destpitch);
    }
    else {
	patternRegPtr = (unsigned char *)vgaLinearBase + sisBLTPatternAddress ;
	k = (y - soy) % sh ;
	for ( i = 0 ; i < sh ; i++ ) {
	    ((unsigned int *)patternRegPtr)[i] = new_bits[k++];
	    k = k%sh;
	}

	srcpitch = 4 ;
	sisSETSRCADDR(sisBLTPatternAddress);
	op |= sisCMDENHCOLEXP ;
	sisSETPITCH(srcpitch, destpitch);
    }

    if ( (sh  <= patternHeight) && (sh%patternHeight == 0) ) {
	/* using the builtin pattern register 
         * do it in one time 
	 */
    	sisSETCMD(op);
	sisBLTWAIT;
    }
    else {
	/* must fill the area in several times
         * pattern by pattern 
	 */
	for ( i = 0 ; i < h ; i += sh ) {
	    int lh,lw;
	    destaddr = (i+y) * destpitch + x;
	    if ( (i + sh) > h )
		lh = h - i - 1;
	    else
		lh = sh - 1 ;
	    for ( j = 0 ; j < w ; j += 32 ) {
		if ( (j+32) > w )
		    lw = w - j -1;
		else
		    lw = 31 ;
		sisSETHEIGHTWIDTH(lh, lw);
		sisSETDSTADDR(destaddr);
		sisSETCMD(op);
		sisBLTWAIT;
		destaddr += 32 ;
	    }
	}
    }

    DEALLOCATE_LOCAL(new_bits);
}

/*
 * Auxilliary function for the left edge; width is contained within one
 * byte of the source bitmap.
 * Assumes that some of the BLT registers are already initialized.
 * Static function, no need for seperate MMIO version name.
 */

#define _siscfbBLTWriteBitmap sisMMIOBLTWriteBitmap
#define _siscfbBLTWriteBitmapLeftEdge sisMMIOBLTWriteBitmapLeftEdge

static __inline__ void _siscfbBLTWriteBitmapLeftEdge(x, y, w, h, src, bwidth,
srcx, srcy, bg, fg, destpitch)
    int x, y, w, h;
    unsigned char *src;
    int bwidth, srcx, srcy, bg, fg, destpitch;
{
    int destaddr, i, shift;
    unsigned long dworddata;
    unsigned char *srcp;
    unsigned char *base;

    destaddr = y * destpitch + x;

    sisSETSRCADDR(0);
    sisSETDSTADDR(destaddr);
    sisSETROP(sisAluConv[GXcopy & 0xf]); 

    sisSETHEIGHTWIDTH(h-1,w-1);
    sisSETCMD( sisCMDCOLEXP | sisSRCSYSTEM |
	      sisTOP2BOTTOM | sisLEFT2RIGHT);

    /* Calculate pointer to origin in bitmap. */
    srcp = bwidth * srcy + (srcx >> 3) + src;
    shift = (srcx & 7);

    for (i = 0; i < h; i++) {
	dworddata = (byte_reversed[*srcp] << shift);
	srcp += bwidth;
	*(unsigned int *)sisBltDataWindow = dworddata;
    }
#if 0
    /* Make sure that a multiple of qwords are transfer for the HiQV
     * architesisure
     */
    if (h & 0x1) *(unsigned int *)sisBltDataWindow = 0;
#endif

    sisBLTWAIT;


}

static __inline__ void
sisBitmapTransfer(w, h, srcpitch, srcaddr, base)
    int w, h, srcpitch;
    unsigned char *srcaddr;
    unsigned char *base;
{
    unsigned char *data;
    int i, line;

#ifdef DEBUG
    ErrorF("SIS:sisBitmapTransferText(%d, %d, %d, 0x%X, 0x%X)\n", w, h,
	srcpitch, srcaddr, base);
#endif

    line = 0;
    data = (unsigned char *)ALLOCATE_LOCAL((w + 3) & ~3);
    while (line < h) {
	i = 0;
	while (i < w) {
	    *(data + i) = byte_reversed[*(srcaddr + i)];
	    i++;
	}
#if 1
        for(i = 0; i < (((w + 3) & ~3) >> 2); i++)
            *(unsigned int *)base=*(unsigned int *)(data + 4 * i);
#else
	memcpy(base, data, ((w + 3) & ~3));	/* Multiple of Double-Word */
#endif
	srcaddr += srcpitch;
	line++;
    }
    DEALLOCATE_LOCAL(data);
}

#define _siscfbBLTWriteBitmap sisMMIOBLTWriteBitmap

void _siscfbBLTWriteBitmap(x, y, w, h, src, bwidth, srcx, srcy, bg, fg, 
destpitch)
    int x, y, w, h;
    unsigned char *src;
    int bwidth, srcx, srcy, bg, fg, destpitch;
{
    int destaddr;
    int skipleft;
    int bytewidth;	/* Area width in bytes. */
    unsigned char *srcp;
    unsigned char *base;


#ifdef DEBUG
    ErrorF("SIS:sisMMIOBLTWriteBitmap(%d, %d, %d, %d, 0x%X, %d, %d, %d, %d)\n",
	   x, y, w, h, src, bwidth, srcx, srcy, destpitch);
#endif

    sisSETBGCOLOR8(bg);
    sisSETFGCOLOR8(fg);
    sisSETPITCH(0,destpitch);

    /* Handle of the left edge (skipleft pixels wide). */ 
    skipleft = 8 - (srcx & 7);
    if (skipleft == 8)
        skipleft = 0;
    if (skipleft) {
	/* Draw left edge. */
	int done;
	done = 0;
	if (skipleft >= w) {
	    skipleft = w;
	    done = 1;
	}
	sisMMIOBLTWriteBitmapLeftEdge(x, y, skipleft, h, src, bwidth,
		srcx, srcy, bg, fg, destpitch);
	if (done)
	    return;
    }

    x += skipleft;
    srcx += skipleft;
    w -= skipleft;

    /* Do the rest with a color-expand BitBLT transfer. */
    destaddr = y * destpitch + x;

    /* Calculate pointer to origin in bitmap. */
    srcp = bwidth * srcy + (srcx >> 3) + src;

    /* Number of bytes to be written for each bitmap scanline. */
    bytewidth = (w + 7) / 8;

    sisSETSRCADDR(0);
    sisSETPITCH(0,destpitch);
    sisSETDSTADDR(destaddr);
    sisSETROP(sisAluConv[GXcopy & 0xf] );

    sisSETHEIGHTWIDTH(h-1,w-1);
    sisSETCMD( sisCMDCOLEXP | sisSRCSYSTEM |
	      sisTOP2BOTTOM | sisLEFT2RIGHT);


    sisBitmapTransfer(bytewidth, h, bwidth, srcp, sisBltDataWindow);
#if 0
    if (((((w + 31) & ~31) >> 5) * h) & 0x1)
	*(unsigned int *)sisBltDataWindow = 0;
#endif
    sisBLTWAIT;

}

