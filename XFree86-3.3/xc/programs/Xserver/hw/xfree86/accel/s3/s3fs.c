/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3fs.c,v 3.10 1996/12/23 06:41:46 dawes Exp $ */
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
/* $XConsortium: s3fs.c /main/8 1996/10/24 07:11:16 kaleb $ */


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
#include  "xf86.h"
#include "s3.h"
#include "regs3.h"

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

   if (!xf86VTSema)
   {
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
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
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

   BLOCK_CURSOR;
   WaitQueue16_32(3,5);
   SET_FRGD_COLOR((pGC->fgPixel));
   SET_FRGD_MIX(FSS_FRGDCOL | s3alu[pGC->alu]);
   SET_WRT_MASK(pGC->planemask);

   while (n--) {
      WaitQueue(5);
      SET_CURPT(ppt->x, ppt->y);
      SET_AXIS_PCNT(((short)*pwidth) - 1, 0);
      SET_CMD(CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

      ppt++;
      pwidth++;
   }

   WaitQueue(2);
   SET_MIX(FSS_FRGDCOL | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;

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

   if (!xf86VTSema)
   {
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
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
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

   if (!xf86VTSema)
   {
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
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
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

   if (!xf86VTSema)
   {
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
      return;
   }
   if (!(pGC->planemask))
      return;

   n = nInit * miFindMaxBand(((cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr))->pCompositeClip);
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

   DEALLOCATE_LOCAL(initPpt);
   DEALLOCATE_LOCAL(initPwidth);
}
