/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxFS.c,v 3.10 1996/12/23 06:32:41 dawes Exp $ */
/*

Copyright (c) 1987  X Consortium

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


Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the name of Sun not be used in advertising
or publicity pertaining to distribution  of  the software
without specific prior written permission. Sun makes no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
Copyright 1994 by Henry A. Worth, Sunnyvale, California.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN, RICKARD E. FAITH, AND HENRY A. WORTH DISCLAIM ALL 
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)

*/
/* $XConsortium: agxFS.c /main/4 1996/02/21 17:17:28 kaleb $ */


#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"

#include "regagx.h"
#include "agx.h"

void
agxSolidFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    DDXPointPtr initPpt;
    int *initPwidth;
    unsigned short width;

    if (!xf86VTSema)
    {
      switch (agxInfoRec.bitsPerPixel) {
      case 8:
         cfbSolidSpansGeneral(pDrawable, pGC,
                              nInit, pptInit, pwidthInit, fSorted);
         break;
      case 16:
         cfb16SolidSpansGeneral(pDrawable, pGC,
                                nInit, pptInit, pwidthInit, fSorted);
         break;
#ifdef AGX_32BPP
      case 32:
         cfb32SolidSpansGeneral(pDrawable, pGC,
                                nInit, pptInit, pwidthInit, fSorted);
         break;
#endif
      }
      return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
	switch (pDrawable->bitsPerPixel) {
	    case 1:
		ErrorF("should call mfbSolidFillSpans\n");
		break;
	    case 8:
	    case 16:
	    case 32:
		ErrorF("should call cfbSolidFillSpans\n");
		break;
	    default:
		ErrorF("Unsupported pixmap depth\n");
		break;
	}
	return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand( ((cfbPrivGC *)
                                   (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                      ->pCompositeClip );
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans( ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))
                        ->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted);

    GE_WAIT_IDLE();

    MAP_SET_SRC_AND_DST( GE_MS_MAP_A );

    GE_OUT_B(GE_FRGD_MIX, pGC->alu);
    GE_OUT_D(GE_FRGD_CLR, pGC->fgPixel);
    GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);

    GE_OUT_W( GE_PIXEL_OP,
              GE_OP_PAT_FRGD
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );

    while (n--) {
       if (*pwidth) {
          unsigned int mapX = (ppt->y) << 16 | (ppt->x);  
          GE_WAIT_IDLE();
          GE_OUT_D( GE_DEST_MAP_X, mapX );
          GE_OUT_D( GE_OP_DIM_WIDTH, *pwidth-1 );
          GE_START_CMDW( GE_OPW_BITBLT
                         | GE_OPW_FRGD_SRC_CLR
                         | GE_OPW_SRC_MAP_A
                         | GE_OPW_DEST_MAP_A   );
        }
        ppt++;
        pwidth++;
    }

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);

    GE_WAIT_IDLE_EXIT();
}

void
agxTiledFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->tile.pixmap;
    DDXPointPtr initPpt;
    int *initPwidth;

    if (!xf86VTSema)
    {
      switch (agxInfoRec.bitsPerPixel) {
      case 8:
         cfbUnnaturalTileFS(pDrawable, pGC, nInit, pptInit, 
                            pwidthInit, fSorted);
         break;
      case 16:
         cfb16UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, 
                              pwidthInit, fSorted);
         break;
#ifdef AGX_32BPP
      case 32:
         cfb32UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, 
                              pwidthInit, fSorted);
         break;
#endif
      }
      return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
      switch (pDrawable->bitsPerPixel) {
        case 1:
           ErrorF("should call mfbTiledFillSpans\n");
           break;
        case 8:
        case 16:
        case 32:
           ErrorF("should call cfbTiledFillSpans\n");
           break;
        default:
           ErrorF("Unsupported pixmap depth\n");
           break;
      }
      return;
    }
    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand( ((cfbPrivGC *)
                                   (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                      ->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans( ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))
                        ->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted );

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("agxTiledFSpans:  PixmapPtr tile.pixmap == NULL\n");
	return;
    }

    agxFSpansTile( pDrawable, n, ppt, pwidth,
                   pPix, xrot, yrot,
		   pGC->alu, pGC->planemask );

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
agxStipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->stipple;
    DDXPointPtr initPpt;
    int *initPwidth;

    if (!xf86VTSema)
    {
      switch (agxInfoRec.bitsPerPixel) {
      case 8:
         cfbUnnaturalStippleFS(pDrawable, pGC,
                               nInit, pptInit, pwidthInit, fSorted);
         break;
      case 16:
         cfb16UnnaturalStippleFS(pDrawable, pGC,
                                 nInit, pptInit, pwidthInit, fSorted);
         break;
#ifdef AGX_32BPP
      case 32:
         cfb32UnnaturalStippleFS(pDrawable, pGC,
                                 nInit, pptInit, pwidthInit, fSorted);
         break;
#endif
      }
      return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
	switch (pDrawable->bitsPerPixel) {
	    case 1:
		ErrorF("should call mfbStippleFillSpans\n");
		break;
	    case 8:
	    case 16:
	    case 32:
		ErrorF("should call cfbStippleFillSpans\n");
		break;
	    default:
		ErrorF("Unsupported pixmap depth\n");
		break;
	}
	return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand( ((cfbPrivGC *)
                                   (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                                       ->pCompositeClip);
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans( ((cfbPrivGC *)(pGC->devPrivates[cfbGCPrivateIndex].ptr))
                        ->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted );

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("agxStipFSpans:  PixmapPtr stipple == NULL\n");
	return;
    }

    agxFSpansStipple( pDrawable, n, ppt, pwidth,
                      pPix, xrot, yrot, 
                      pGC->fgPixel, 0,
                      pGC->alu, MIX_DST,
                      pGC->planemask );

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}

void
agxOStipFSpans (pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
    DrawablePtr pDrawable;
    GCPtr       pGC;
    int         nInit;                  /* number of spans to fill */
    DDXPointPtr pptInit;                /* pointer to list of start points */
    int         *pwidthInit;            /* pointer to list of n widths */
    int         fSorted;
{
    int n;                      /* number of spans to fill */
    register DDXPointPtr ppt;   /* pointer to list of start points */
    register int *pwidth;       /* pointer to list of n widths */
    int xrot, yrot, width, height, pixWidth;
    PixmapPtr pPix = pGC->stipple;
    DDXPointPtr initPpt;
    int *initPwidth;

    if (!xf86VTSema)
    {
       switch (agxInfoRec.bitsPerPixel) {
       case 8:
          cfbUnnaturalStippleFS(pDrawable, pGC,
                                nInit, pptInit, pwidthInit, fSorted);
          break;
       case 16:
          cfb16UnnaturalStippleFS(pDrawable, pGC,
                                  nInit, pptInit, pwidthInit, fSorted);
          break;
#ifdef AGX_32BPP
       case 32:
          cfb32UnnaturalStippleFS(pDrawable, pGC,
                                  nInit, pptInit, pwidthInit, fSorted);
          break;
#endif
       }
       return;
    }

    if (pDrawable->type != DRAWABLE_WINDOW) {
	switch (pDrawable->bitsPerPixel) {
	    case 1:
		ErrorF("should call mfbOpStippleFillSpans\n");
		break;
	    case 8:
	    case 16:
	    case 32:
		ErrorF("should call cfbOpStippleFillSpans\n");
		break;
	    default:
		ErrorF("Unsupported pixmap depth\n");
		break;
	}
	return;
    }

    if (!(pGC->planemask))
        return;

    n = nInit * miFindMaxBand( ((cfbPrivGC *)(pGC
                                    ->devPrivates[cfbGCPrivateIndex].ptr))
                                        ->pCompositeClip );
    initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));
    initPpt = ppt = (DDXPointRec *)ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
    if(!ppt || !pwidth)
    {
        if (ppt) DEALLOCATE_LOCAL(ppt);
        if (pwidth) DEALLOCATE_LOCAL(pwidth);
        return;
    }
    n = miClipSpans( ((cfbPrivGC *)
                        (pGC->devPrivates[cfbGCPrivateIndex].ptr))
                           ->pCompositeClip,
                     pptInit, pwidthInit, nInit,
                     ppt, pwidth, fSorted );

    xrot = pDrawable->x + pGC->patOrg.x;
    yrot = pDrawable->y + pGC->patOrg.y;

    if (pPix == (PixmapPtr)0) {
	ErrorF("agxStipFSpans:  PixmapPtr stipple == NULL\n");
	return;
    }

    agxFSpansStipple( pDrawable, n, ppt, pwidth,
                        pPix, xrot, yrot, 
                        pGC->fgPixel, pGC->bgPixel,
                        pGC->alu, pGC->alu,
                        pGC->planemask );

    DEALLOCATE_LOCAL(initPpt);
    DEALLOCATE_LOCAL(initPwidth);
}
