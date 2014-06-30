/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_solid.c,v 1.1 1997/01/12 10:43:11 dawes Exp $ */

/*
 * 
 * Copyright (c) 1987  X Consortium
 * 
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * 
 * Except as contained in this notice, the name of the X Consortium shall
 * not be used in advertising or otherwise to promote the sale, use or
 * other dealings in this Software without prior written authorization
 * from the X Consortium.
 * 
 * 
 * Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.
 * 
 * All Rights Reserved
 * 
 * Permission  to  use,  copy,  modify,  and  distribute   this
 * software  and  its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright no-
 * tice  appear  in all copies and that both that copyright no-
 * tice and this permission notice appear in  supporting  docu-
 * mentation,  and  that the name of Sun not be used in advertising
 * or publicity pertaining to distribution  of  the software
 * without specific prior written permission. Sun makes no 
 * representations about the suitability of this software for 
 * any purpose. It is provided "as is" without any express or 
 * implied warranty.
 * 
 * SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
 * NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
 * ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
 * PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 * THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * 
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 * Copyright 1994 by Harm Hanemaayer, Utrecht, The Netherlands
 * Copyright 1996 by David Bateman <dbateman@ee.uts.edu.au>
 * 
 * All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its 
 * documentation for any purpose and without fee is hereby granted, 
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in 
 * supporting documentation, and that the name of Digital not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  
 * 
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * 
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 * KEVIN E. MARTIN, RICKARD E. FAITH, AND HENRY A. WORTH DISCLAIM ALL 
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE
 * AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 * Modified for Cirrus by Harm Hanemaayer (hhanemaa@cs.ruu.nl)
 * Modified for Chips by David Bateman (dbateman@ee.uts.edu.au)
 * Modified for Sis by Xavier Ducoin (xavier@rd.lectra.fr)
 * 
 */


#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "xf86.h"
#include "vga256.h"
#include "vga.h"
#include "cfbrrop.h"
#include "mergerop.h"
#include "compiler.h"
#include "sis_driver.h"


#include "sis_Blitter.h"


/*
 * Accelerated solid filled spans for 8, 16 and 24bpp.
 * Each span is drawn with a seperate BitBLT command.
 * Command overhead is reduced as far as possible.
 */

#define _siscfbFillSolidSpansGeneral sisMMIOFillSolidSpansGeneral
void
sisMMIOFillSolidSpansGeneral(pDrawable, pGC, nInit, pptInit, pwidthInit,
    fSorted)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int nInit;			       /* number of spans to fill */
    DDXPointPtr pptInit;	       /* pointer to list of start points */
    int *pwidthInit;		       /* pointer to list of n widths */
    int fSorted;
{
    int n;			       /* number of spans to fill */
    register DDXPointPtr ppt;	       /* pointer to list of start points */
    register int *pwidth;	       /* pointer to list of n widths */
    DDXPointPtr initPpt;
    int *initPwidth;
    unsigned short width;
    int pitch;
    unsigned int op, rop;

    RROP_DECLARE;

#ifdef DEBUG
    ErrorF("SIS:sisMMIOFillSolidSpansGeneral: Called ");
#endif

    /* We only handle on-screen spans with full planemask. */
    if (vgaBitsPerPixel == 8 && (!xf86VTSema ||
	    (pGC->planemask & 0xFF) != 0xFF)) {
	vga256SolidSpansGeneral(pDrawable, pGC,
	    nInit, pptInit, pwidthInit, fSorted);
	return;
    } else if (vgaBitsPerPixel == 16 && (!xf86VTSema ||
	    (pGC->planemask & 0xFFFF) != 0xFFFF)) {
	cfb16SolidSpansGeneral(pDrawable, pGC,
	    nInit, pptInit, pwidthInit, fSorted);
	return;
    } else if (vgaBitsPerPixel == 24 && (!xf86VTSema ||
	    (pGC->planemask & 0xFFFFFF) != 0xFFFFFF)) {
	cfb24SolidSpansGeneral(pDrawable, pGC,
	    nInit, pptInit, pwidthInit, fSorted);
	return;
    }
    if (pDrawable->type != DRAWABLE_WINDOW) {
	/* 
	 * This would be handled better by writing decent ValidateGC
	 * code for the SVGA server.
	 */
	if (vgaBitsPerPixel == 8)
	    cfbSolidSpansGeneral(pDrawable, pGC,
		nInit, pptInit, pwidthInit, fSorted);
	else if (vgaBitsPerPixel == 16)
	    cfb16SolidSpansGeneral(pDrawable, pGC,
		nInit, pptInit, pwidthInit, fSorted);
	else
	    cfb24SolidSpansGeneral(pDrawable, pGC,
		nInit, pptInit, pwidthInit, fSorted);
	return;
    }
    if (!(pGC->planemask))
	return;

/* Set up the foreground color here, because for 24bpp, we might need
 * to bug out.
 */

    switch (vgaBitsPerPixel) {
    case 8:
	sisSETFGCOLOR8(pGC->fgPixel);
	sisSETBGCOLOR8(pGC->fgPixel);
	break;
    case 16:
	sisSETFGCOLOR16(pGC->fgPixel);
	sisSETBGCOLOR16(pGC->fgPixel);
	break;
    case 24:
	sisSETFGCOLOR24(pGC->fgPixel);
	sisSETBGCOLOR24(pGC->fgPixel);
	break;
    }

    n = nInit * miFindMaxBand(((cfbPrivGC *)
	    (pGC->devPrivates[cfbGCPrivateIndex].ptr))
	->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));

    initPpt = ppt = (DDXPointRec *) ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if (!ppt || !pwidth) {
	if (ppt)
	    DEALLOCATE_LOCAL(ppt);
	if (pwidth)
	    DEALLOCATE_LOCAL(pwidth);
	return;
    }
    n = miClipSpans(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))
	->pCompositeClip,
	pptInit, pwidthInit, nInit,
	ppt, pwidth, fSorted);

    RROP_FETCH_GC(pGC);

    pitch = vga256InfoRec.displayWidth * vgaBytesPerPixel;

    /*
     * Source bit pattern is irrelevant, background and foregound color
     * are set to the same value.
     */

    /* Set up the invariant BitBLT parameters. */
    rop = sisAluConv[pGC->alu & 0xF] ;
    op =  sisTOP2BOTTOM | sisLEFT2RIGHT | sisSRCBG ;

    sisSETPITCH(0, pitch);

    while (n--) {
	int destaddr;
	int width_in_bytes;

	/* Do the address calculation while the previous blit is running. */

	destaddr = ppt->y * pitch + (ppt->x * vgaBytesPerPixel);
	width_in_bytes = *pwidth * vgaBytesPerPixel;

	sisBLTWAIT;
	sisSETROP(rop);
	sisSETDSTADDR(destaddr);
	/* according to SIS Programming Guide ===>  h-1,w-1*/
	sisSETHEIGHTWIDTH(0, width_in_bytes-1);
	sisSETCMD(op|sisCMDBLT);
	ppt++;
	pwidth++;
    }

    sisBLTWAIT;

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

/*
 * Optimized accelerated solid filled rectangles for 8, 16 and 24bpp.
 * Command overhead is reduced as far as possible.
 */

void
sisMMIOFillRectSolid(pDrawable, pGC, nBox, pBox)
    DrawablePtr pDrawable;
    GCPtr pGC;
    int nBox;
    BoxPtr pBox;
{
    int pitch;
    unsigned int op, rop;

    RROP_DECLARE;
#ifdef DEBUG
    ErrorF("SIS:sisMMIOFillRectSolid: Called ");
#endif

    /* We only handle on-screen fills with full planemask. */
    if (vgaBitsPerPixel == 8 && (!xf86VTSema ||
	    (pGC->planemask & 0xFF) != 0xFF || pDrawable->type != DRAWABLE_WINDOW)) {
	vga256FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	return;
    } else if (vgaBitsPerPixel == 16 && (!xf86VTSema ||
		(pGC->planemask & 0xFFFF) != 0xFFFF ||
	    pDrawable->type != DRAWABLE_WINDOW)) {
	cfb16FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	return;
    } else if (vgaBitsPerPixel == 24 && (!xf86VTSema ||
		(pGC->planemask & 0xFFFFFF) != 0xFFFFFF ||
	    pDrawable->type != DRAWABLE_WINDOW)) {
	cfb24FillRectSolidGeneral(pDrawable, pGC, nBox, pBox);
	return;
    }
    RROP_FETCH_GC(pGC);

    pitch = vga256InfoRec.displayWidth * vgaBytesPerPixel;

    switch (vgaBitsPerPixel) {
    case 8:
	sisSETFGCOLOR8(pGC->fgPixel);
	sisSETBGCOLOR8(pGC->fgPixel);
	break;
    case 16:
	sisSETFGCOLOR16(pGC->fgPixel);
	sisSETBGCOLOR16(pGC->fgPixel);
	break;
    case 24:
	sisSETFGCOLOR24(pGC->fgPixel);
	sisSETBGCOLOR24(pGC->fgPixel);
	break;
    }

    /* Set up the invariant BitBLT parameters. */
    rop = sisAluConv[pGC->alu & 0xF] ;
    op =  sisTOP2BOTTOM | sisLEFT2RIGHT | sisSRCBG ;
    sisSETPITCH(0, pitch);

    while (nBox) {
	int height, width_in_bytes, destaddr;

	height = pBox->y2 - pBox->y1;
	width_in_bytes = (pBox->x2 - pBox->x1) * vgaBytesPerPixel;
	destaddr = pBox->y1 * pitch + (pBox->x1 * vgaBytesPerPixel);

	sisBLTWAIT;
	sisSETROP(rop);
	sisSETDSTADDR(destaddr);
	/* according to SIS Programming Guide ===>  h-1,w-1*/
	sisSETHEIGHTWIDTH(height-1, width_in_bytes-1);
	sisSETCMD(op|sisCMDBLT);

	nBox--;
	pBox++;
    }
    sisBLTWAIT;
}
