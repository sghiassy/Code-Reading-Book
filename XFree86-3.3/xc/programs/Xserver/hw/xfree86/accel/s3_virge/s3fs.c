/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3fs.c,v 3.5.2.1 1997/05/24 08:36:00 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to
distribution  of  the software  without specific prior
written permission. Sun and X Consortium make no
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

********************************************************/

/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.

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

KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

******************************************************************/

/*
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3fs.c /main/4 1996/10/25 15:37:25 kaleb $ */


#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "mi.h"
#include "mispans.h"
#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "misc.h"
#include "xf86.h"
#include "s3v.h"

extern int s3MAX_SLOTS;

void
s3SolidFSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   nInit;		/* number of spans to fill */
     DDXPointPtr pptInit;	/* pointer to list of start points */
     int  *pwidthInit;		/* pointer to list of n widths */
     int   fSorted;
{
   int   n;			/* number of spans to fill */
   register DDXPointPtr ppt;	/* pointer to list of start points */
   register int *pwidth;	/* pointer to list of n widths */
   DDXPointPtr initPpt;
   int *initPwidth;

   if (!xf86VTSema /*|| ((pGC->planemask & s3BppPMask) != s3BppPMask)*/)
   {
      if (xf86VTSema) WaitIdleEmpty();
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbSolidSpansGeneral(pDrawable, pGC,
			      nInit, pptInit, pwidthInit, fSorted);
         break;
      case 16:
	 cfb16SolidSpansGeneral(pDrawable, pGC,
				nInit, pptInit, pwidthInit, fSorted);
         break;
      case 24:
	 cfb24SolidSpansGeneral(pDrawable, pGC,
				nInit, pptInit, pwidthInit, fSorted);
         break;
      case 32:
	 cfb32SolidSpansGeneral(pDrawable, pGC,
				nInit, pptInit, pwidthInit, fSorted);
         break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }

   if (pDrawable->type != DRAWABLE_WINDOW) {
      switch (pDrawable->bitsPerPixel) {
	case 1:
	   ErrorF("should call mfbSolidFillSpans\n");
	   break;
	case 8:
        case 16:
        case 24:
        case 32:
	   ErrorF("should call cfbSolidFillSpans\n");
	   break;
	default:
	   ErrorF("Unsupported pixmap depth\n");
	   break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);

   if (n <= 0) return;

   initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));

   initPpt = ppt = (DDXPointRec *) ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
   if (!ppt || !pwidth) {
      if (ppt)
	 DEALLOCATE_LOCAL(ppt);
      if (pwidth)
	 DEALLOCATE_LOCAL(pwidth);
      return;
   }
   n = miClipSpans(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
		   pptInit, pwidthInit, nInit,
		   ppt, pwidth, fSorted);
   if (n > 0) {
     BLOCK_CURSOR;
     WaitIdle();
     WaitQueue(4);
     SETB_PAT_FG_CLR(pGC->fgPixel);
     ;SET_FRGD_MIX(FSS_FRGDCOL | s3alu[pGC->alu]);
     ;SET_WRT_MASK(pGC->planemask);
     SETB_MONO_PAT0(~0);
     SETB_MONO_PAT1(~0);
     SETB_CMD_SET(s3_gcmd | CMD_BITBLT | CMD_AUTOEXEC |
                  MIX_MONO_PATT |
                  INC_Y | INC_X | s3alu_sp[pGC->alu] );

     while (n--) {
       SETB_BLT(ppt->x, ppt->y, ppt->x, ppt->y, ((short)*pwidth) - 1, 1, INC_X);
       ppt++;
       pwidth++;
     }

     WaitIdle();
     SETB_CMD_SET(CMD_NOP);
     UNBLOCK_CURSOR;
   }
   DEALLOCATE_LOCAL(initPpt);
   DEALLOCATE_LOCAL(initPwidth);
}

void
s3TiledFSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   nInit;		/* number of spans to fill */
     DDXPointPtr pptInit;	/* pointer to list of start points */
     int  *pwidthInit;		/* pointer to list of n widths */
     int   fSorted;
{
   int   n;			/* number of spans to fill */
   register DDXPointPtr ppt;	/* pointer to list of start points */
   register int *pwidth;	/* pointer to list of n widths */
   int   xrot, yrot, width, height, pixWidth;
   PixmapPtr pPix = pGC->tile.pixmap;
   DDXPointPtr initPpt;
   int *initPwidth;

   if (!xf86VTSema /*|| ((pGC->planemask & s3BppPMask) != s3BppPMask)*/)
   {
      if (xf86VTSema) WaitIdleEmpty();
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbUnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
         break;
      case 16:
	 cfb16UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
         break;
      case 24:
	 cfb24UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
         break;
      case 32:
	 cfb32UnnaturalTileFS(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted);
         break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }

   if (pDrawable->type != DRAWABLE_WINDOW) {
      switch (pDrawable->bitsPerPixel) {
	case 1:
	   ErrorF("should call mfbTiledFillSpans\n");
	   break;
	case 8:
	case 16:
	case 24:
	case 32:
	   ErrorF("should call cfbTiledFillSpans\n");
	   break;
	default:
	   ErrorF("Unsupported pixmap depth\n");
	   break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);

   if (n <= 0) return;

   initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));

   initPpt = ppt = (DDXPointRec *) ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
   if (!ppt || !pwidth) {
      if (ppt)
	 DEALLOCATE_LOCAL(ppt);
      if (pwidth)
	 DEALLOCATE_LOCAL(pwidth);
      return;
   }
   n = miClipSpans(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
		   pptInit, pwidthInit, nInit,
		   ppt, pwidth, fSorted);
   if (n > 0) {
     xrot = pDrawable->x + pGC->patOrg.x;
     yrot = pDrawable->y + pGC->patOrg.y;

     if (pPix == (PixmapPtr) 0) {
       ErrorF("s3TiledFSpans:  PixmapPtr tile.pixmap == NULL\n");
       return;
     }
     width = pPix->drawable.width;
     height = pPix->drawable.height;
     pixWidth = PixmapBytePad(width, pPix->drawable.depth);

     while (n--) {
       (s3ImageFillFunc) (ppt->x, ppt->y, *pwidth, 1,
			 pPix->devPrivate.ptr, pixWidth,
			 width, height, xrot, yrot,
			 s3alu[pGC->alu], pGC->planemask);
       ppt++;
       pwidth++;
     }
   }
   DEALLOCATE_LOCAL(initPpt);
   DEALLOCATE_LOCAL(initPwidth);
}

void
s3StipFSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   nInit;		/* number of spans to fill */
     DDXPointPtr pptInit;	/* pointer to list of start points */
     int  *pwidthInit;		/* pointer to list of n widths */
     int   fSorted;
{
   int   n;			/* number of spans to fill */
   register DDXPointPtr ppt;	/* pointer to list of start points */
   register int *pwidth;	/* pointer to list of n widths */
   int   xrot, yrot, width, height, pixWidth;
   PixmapPtr pPix = pGC->stipple;
   DDXPointPtr initPpt;
   int *initPwidth;

   if (!xf86VTSema /*|| ((pGC->planemask & s3BppPMask) != s3BppPMask)*/)
   {
      if (xf86VTSema) WaitIdleEmpty();
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbUnnaturalStippleFS(pDrawable, pGC,
			       nInit, pptInit, pwidthInit, fSorted);
         break;
      case 16:
	 cfb16UnnaturalStippleFS(pDrawable, pGC,
				 nInit, pptInit, pwidthInit, fSorted);
         break;
      case 24:
	 cfb24UnnaturalStippleFS(pDrawable, pGC,
				 nInit, pptInit, pwidthInit, fSorted);
         break;
      case 32:
	 cfb32UnnaturalStippleFS(pDrawable, pGC,
				 nInit, pptInit, pwidthInit, fSorted);
         break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }

   if (pDrawable->type != DRAWABLE_WINDOW) {
      switch (pDrawable->bitsPerPixel) {
	case 1:
	   ErrorF("should call mfbStippleFillSpans\n");
	   break;
	case 8:
	case 16:
	case 24:
	case 32:
	   ErrorF("should call cfbStippleFillSpans\n");
	   break;
	default:
	   ErrorF("Unsupported pixmap depth\n");
	   break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);

   if (n <= 0) return;

   initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));

   initPpt = ppt = (DDXPointRec *) ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
   if (!ppt || !pwidth) {
      if (ppt)
	 DEALLOCATE_LOCAL(ppt);
      if (pwidth)
	 DEALLOCATE_LOCAL(pwidth);
      return;
   }
   n = miClipSpans(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
		   pptInit, pwidthInit, nInit,
		   ppt, pwidth, fSorted);

   if (n > 0) {
     xrot = pDrawable->x + pGC->patOrg.x;
     yrot = pDrawable->y + pGC->patOrg.y;

     if (pPix == (PixmapPtr) 0) {
        ErrorF("s3StipFSpans:  PixmapPtr stipple == NULL\n");
        return;
     }
     width = pPix->drawable.width;
     height = pPix->drawable.height;
     pixWidth = PixmapBytePad(width, pPix->drawable.depth);

     while (n--) {
       s3ImageStipple(ppt->x, ppt->y, *pwidth, 1,
		     pPix->devPrivate.ptr, pixWidth, width, height,
		     xrot, yrot, pGC->fgPixel,
		     s3alu[pGC->alu], pGC->planemask);
       ppt++;
       pwidth++;
     }
   }
   DEALLOCATE_LOCAL(initPpt);
   DEALLOCATE_LOCAL(initPwidth);
}

void
s3OStipFSpans(pDrawable, pGC, nInit, pptInit, pwidthInit, fSorted)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   nInit;		/* number of spans to fill */
     DDXPointPtr pptInit;	/* pointer to list of start points */
     int  *pwidthInit;		/* pointer to list of n widths */
     int   fSorted;
{
   int   n;			/* number of spans to fill */
   register DDXPointPtr ppt;	/* pointer to list of start points */
   register int *pwidth;	/* pointer to list of n widths */
   int   xrot, yrot, width, height, pixWidth;
   PixmapPtr pPix = pGC->stipple;
   DDXPointPtr initPpt;
   int *initPwidth;

   if (!xf86VTSema /*|| ((pGC->planemask & s3BppPMask) != s3BppPMask)*/)
   {
      if (xf86VTSema) WaitIdleEmpty();
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbUnnaturalStippleFS(pDrawable, pGC,
			       nInit, pptInit, pwidthInit, fSorted);
         break;
      case 16:
	 cfb16UnnaturalStippleFS(pDrawable, pGC,
				 nInit, pptInit, pwidthInit, fSorted);
         break;
      case 24:
	 cfb24UnnaturalStippleFS(pDrawable, pGC,
				 nInit, pptInit, pwidthInit, fSorted);
         break;
      case 32:
	 cfb32UnnaturalStippleFS(pDrawable, pGC,
				 nInit, pptInit, pwidthInit, fSorted);
         break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }

   if (pDrawable->type != DRAWABLE_WINDOW) {
      switch (pDrawable->bitsPerPixel) {
	case 1:
	   ErrorF("should call mfbOpStippleFillSpans\n");
	   break;
	case 8:
	case 16:
	case 24:
	case 32:
	   ErrorF("should call cfbOpStippleFillSpans\n");
	   break;
	default:
	   ErrorF("Unsupported pixmap depth\n");
	   break;
      }
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);

   if (n <= 0) return;

   initPwidth = pwidth = (int *)ALLOCATE_LOCAL(n * sizeof(int));

   initPpt = ppt = (DDXPointRec *) ALLOCATE_LOCAL(n * sizeof(DDXPointRec));
   if (!ppt || !pwidth) {
      if (ppt)
	 DEALLOCATE_LOCAL(ppt);
      if (pwidth)
	 DEALLOCATE_LOCAL(pwidth);
      return;
   }
   n = miClipSpans(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip,
		   pptInit, pwidthInit, nInit,
		   ppt, pwidth, fSorted);

   if (n > 0) {
     xrot = pDrawable->x + pGC->patOrg.x;
     yrot = pDrawable->y + pGC->patOrg.y;

     if (pPix == (PixmapPtr) 0) {
       ErrorF("s3StipFSpans:  PixmapPtr stipple == NULL\n");
       return;
     }
     width = pPix->drawable.width;
     height = pPix->drawable.height;
     pixWidth = PixmapBytePad(width, pPix->drawable.depth);

     while (n--) {
       s3ImageOpStipple(ppt->x, ppt->y, *pwidth, 1,
		       pPix->devPrivate.ptr, pixWidth,
		       width, height,
		       xrot, yrot, pGC->fgPixel, pGC->bgPixel,
		       s3alu[pGC->alu], pGC->planemask);
       ppt++;
       pwidth++;
     }
   }
   DEALLOCATE_LOCAL(initPpt);
   DEALLOCATE_LOCAL(initPwidth);
}
