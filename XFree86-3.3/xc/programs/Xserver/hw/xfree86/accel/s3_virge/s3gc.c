/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3gc.c,v 3.2 1996/12/27 07:02:33 dawes Exp $ */
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


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

*/

/*
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3gc.c /main/2 1996/10/25 11:34:48 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "cfb.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "region.h"

#include "mistruct.h"
#include "mibstore.h"
#include "migc.h"

#include "cfbmskbits.h"
#include "cfb8bit.h"

#include "xf86bcache.h"
#include "xf86fcache.h"
#include "xf86text.h"

#include "s3v.h"

static void s3ValidateGC();

static GCFuncs cfbFuncs =
{
   s3ValidateGC,
   miChangeGC,
   miCopyGC,
   miDestroyGC,
   miChangeClip,
   miDestroyClip,
   miCopyClip,
};

static GCOps s3Ops =
{
   s3SolidFSpans,
   s3SetSpans,
   cfbPutImage,
   s3CopyArea,
   s3CopyPlane,
   s3PolyPoint,
   miWideLine,
   miPolySegment,
   miPolyRectangle,
   miPolyArc,
   miFillPolygon,
   s3PolyFillRect,
   miPolyFillArc,
   xf86PolyText8,
   xf86PolyText16,
   xf86ImageText8,
   xf86ImageText16,
   miImageGlyphBlt,
   miPolyGlyphBlt,
   miPushPixels,
   NULL,
};

static GCOps cfbTEOps1Rect =
{
   cfbSolidSpansCopy,
   cfbSetSpans,
   cfbPutImage,
   s3CopyArea,
   s3CopyPlane,
   cfbPolyPoint,
   cfb8LineSS1Rect,
   cfb8SegmentSS1Rect,
   miPolyRectangle,
#if PPW == 4
   cfbZeroPolyArcSS8Copy,
#else
   miZeroPolyArc,
#endif
   cfbFillPoly1RectCopy,
   cfbPolyFillRect,
   cfbPolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfbTEGlyphBlt8,
   cfbPolyGlyphBlt8,
   cfbPushPixels8,
   NULL,
};

static GCOps cfbTEOps =
{
   cfbSolidSpansCopy,
   cfbSetSpans,
   cfbPutImage,
   s3CopyArea,
   s3CopyPlane,
   cfbPolyPoint,
   cfbLineSS,
   cfbSegmentSS,
   miPolyRectangle,
#if PPW == 4
   cfbZeroPolyArcSS8Copy,
#else
   miZeroPolyArc,
#endif
   miFillPolygon,
   cfbPolyFillRect,
   cfbPolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfbTEGlyphBlt8,
   cfbPolyGlyphBlt8,
   cfbPushPixels8,
   NULL,
};

static GCOps cfbNonTEOps1Rect =
{
   cfbSolidSpansCopy,
   cfbSetSpans,
   cfbPutImage,
   s3CopyArea,
   s3CopyPlane,
   cfbPolyPoint,
   cfbLineSS,
   cfbSegmentSS,
   miPolyRectangle,
#if PPW == 4
   cfbZeroPolyArcSS8Copy,
#else
   miZeroPolyArc,
#endif
   cfbFillPoly1RectCopy,
   cfbPolyFillRect,
   cfbPolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfbImageGlyphBlt8,
   cfbPolyGlyphBlt8,
   cfbPushPixels8,
   NULL,
};

static GCOps cfbNonTEOps =
{
   cfbSolidSpansCopy,
   cfbSetSpans,
   cfbPutImage,
   s3CopyArea,
   s3CopyPlane,
   cfbPolyPoint,
   cfbLineSS,
   cfbSegmentSS,
   miPolyRectangle,
#if PPW == 4
   cfbZeroPolyArcSS8Copy,
#else
   miZeroPolyArc,
#endif
   miFillPolygon,
   cfbPolyFillRect,
   cfbPolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfbImageGlyphBlt8,
   cfbPolyGlyphBlt8,
   cfbPushPixels8,
   NULL,
};

static GCOps *
matchCommon(pGC, devPriv)
     GCPtr pGC;
     cfbPrivGCPtr devPriv;
{
   if (pGC->lineWidth != 0)
      return 0;
   if (pGC->lineStyle != LineSolid)
      return 0;
   if (pGC->fillStyle != FillSolid)
      return 0;
   if (devPriv->rop != GXcopy)
      return 0;
   if (pGC->font &&
       FONTMAXBOUNDS(pGC->font, rightSideBearing) -
       FONTMINBOUNDS(pGC->font, leftSideBearing) <= 32 &&
       FONTMINBOUNDS(pGC->font, characterWidth) >= 0) {
      if (TERMINALFONT(pGC->font)
#if PPW == 4
	  && FONTMAXBOUNDS(pGC->font, characterWidth) >= 4
#endif
	 )
	 if (devPriv->oneRect)
	    return &cfbTEOps1Rect;
	 else
	    return &cfbTEOps;
      else if (devPriv->oneRect)
	 return &cfbNonTEOps1Rect;
      else
	 return &cfbNonTEOps;
   }
   return 0;
}

Bool
s3CreateGC(pGC)
     register GCPtr pGC;
{
   cfbPrivGC *pPriv;

   switch (pGC->depth) {
     case 1:
	return (mfbCreateGC(pGC));
     case PSZ:
	break;
     default:
	ErrorF("cfbCreateGC: unsupported depth: %d\n", pGC->depth);
	return FALSE;
   }
   pGC->clientClip = NULL;
   pGC->clientClipType = CT_NONE;

 /*
  * some of the output primitives aren't really necessary, since they will be
  * filled in ValidateGC because of dix/CreateGC() setting all the change
  * bits.  Others are necessary because although they depend on being a color
  * frame buffer, they don't change
  */

   pGC->ops = &cfbNonTEOps;
   pGC->funcs = &cfbFuncs;

 /* cfb wants to translate before scan conversion */
   pGC->miTranslate = 1;

   pPriv = (cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr);
   pPriv->rop = pGC->alu;
   pPriv->oneRect = FALSE;
   pPriv->fExpose = TRUE;
   pPriv->freeCompClip = FALSE;
   pPriv->pRotatedPixmap = (PixmapPtr) NULL;
   return TRUE;
}

/*
 * create a private op array for a gc
 */

static GCOps *
cfbCreateOps(prototype)
     GCOps *prototype;
{
   GCOps *ret;
   extern Bool Must_have_memory;

   /* XXX */ Must_have_memory = TRUE;
   ret = (GCOps *) xalloc(sizeof(GCOps));
   /* XXX */ Must_have_memory = FALSE;
   if (!ret)
      return 0;
   *ret = *prototype;
   ret->devPrivate.val = 1;
   return ret;
}


/*
 * Clipping conventions if the drawable is a window CT_REGION ==>
 * pCompositeClip really is the composite CT_other ==> pCompositeClip is the
 * window clip region if the drawable is a pixmap CT_REGION ==>
 * pCompositeClip is the translated client region clipped to the pixmap
 * boundary CT_other ==> pCompositeClip is the pixmap bounding box
 */

static void
s3ValidateGC(pGC, changes, pDrawable)
     register GCPtr pGC;
     Mask  changes;
     DrawablePtr pDrawable;
{
   WindowPtr pWin;
   int   mask;			/* stateChanges */
   int   index;			/* used for stepping through bitfields */
   int   new_rrop;
   int   new_line, new_text, new_fillspans, new_fillarea;
   int   new_rotate;
   int   xrot, yrot;

 /* flags for changing the proc vector */
   cfbPrivGCPtr devPriv;
   int   oneRect;

   new_rotate = pGC->lastWinOrg.x != pDrawable->x ||
      pGC->lastWinOrg.y != pDrawable->y;

   pGC->lastWinOrg.x = pDrawable->x;
   pGC->lastWinOrg.y = pDrawable->y;
   if (pDrawable->type == DRAWABLE_WINDOW) {
      pWin = (WindowPtr) pDrawable;
   } else {
      pWin = (WindowPtr) NULL;
   }

   devPriv = ((cfbPrivGCPtr) (pGC->devPrivates[cfbGCPrivateIndex].ptr));

   new_rrop = FALSE;
   new_line = FALSE;
   new_text = FALSE;
   new_fillspans = FALSE;
   new_fillarea = FALSE;

 /*
  * if the client clip is different or moved OR the subwindowMode has changed
  * OR the window's clip has changed since the last validation we need to
  * recompute the composite clip
  */

   if ((changes & (GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode)) ||
     (pDrawable->serialNumber != (pGC->serialNumber & DRAWABLE_SERIAL_BITS))
      ) {
      ScreenPtr pScreen = pGC->pScreen;

      if (pWin) {
	 RegionPtr pregWin;
	 Bool  freeTmpClip, freeCompClip;

	 if (pGC->subWindowMode == IncludeInferiors) {
	    pregWin = NotClippedByChildren(pWin);
	    freeTmpClip = TRUE;
	 } else {
	    pregWin = &pWin->clipList;
	    freeTmpClip = FALSE;
	 }
	 freeCompClip = devPriv->freeCompClip;

       /*
        * if there is no client clip, we can get by with just keeping the
        * pointer we got, and remembering whether or not should destroy (or
        * maybe re-use) it later.  this way, we avoid unnecessary copying of
        * regions.  (this wins especially if many clients clip by children
        * and have no client clip.)
        */
	 if (pGC->clientClipType == CT_NONE) {
	    if (freeCompClip)
	       (*pScreen->RegionDestroy) (devPriv->pCompositeClip);
	    devPriv->pCompositeClip = pregWin;
	    devPriv->freeCompClip = freeTmpClip;
	 } else {

	  /*
	   * we need one 'real' region to put into the composite clip. if
	   * pregWin the current composite clip are real, we can get rid of
	   * one. if pregWin is real and the current composite clip isn't,
	   * use pregWin for the composite clip. if the current composite
	   * clip is real and pregWin isn't, use the current composite clip.
	   * if neither is real, create a new region.
	   */

	    (*pScreen->TranslateRegion) (pGC->clientClip,
					 pDrawable->x + pGC->clipOrg.x,
					 pDrawable->y + pGC->clipOrg.y);

	    if (freeCompClip) {
	       (*pGC->pScreen->Intersect) (devPriv->pCompositeClip,
					   pregWin, pGC->clientClip);
	       if (freeTmpClip)
		  (*pScreen->RegionDestroy) (pregWin);
	    } else if (freeTmpClip) {
	       (*pScreen->Intersect) (pregWin, pregWin, pGC->clientClip);
	       devPriv->pCompositeClip = pregWin;
	    } else {
	       devPriv->pCompositeClip = (*pScreen->RegionCreate) (NullBox,
								   0);
	       (*pScreen->Intersect) (devPriv->pCompositeClip,
				      pregWin, pGC->clientClip);
	    }
	    devPriv->freeCompClip = TRUE;
	    (*pScreen->TranslateRegion) (pGC->clientClip,
					 -(pDrawable->x + pGC->clipOrg.x),
					 -(pDrawable->y + pGC->clipOrg.y));

	 }
      }
    /* end of composite clip for a window */
      else {
	 BoxRec pixbounds;

       /* XXX should we translate by drawable.x/y here ? */
	 pixbounds.x1 = 0;
	 pixbounds.y1 = 0;
	 pixbounds.x2 = pDrawable->width;
	 pixbounds.y2 = pDrawable->height;

	 if (devPriv->freeCompClip)
	    (*pScreen->RegionReset) (devPriv->pCompositeClip, &pixbounds);
	 else {
	    devPriv->freeCompClip = TRUE;
	    devPriv->pCompositeClip = (*pScreen->RegionCreate) (&pixbounds,
								1);
	 }

	 if (pGC->clientClipType == CT_REGION) {
	    (*pScreen->TranslateRegion) (devPriv->pCompositeClip,
					 -pGC->clipOrg.x, -pGC->clipOrg.y);
	    (*pScreen->Intersect) (devPriv->pCompositeClip,
				   devPriv->pCompositeClip,
				   pGC->clientClip);
	    (*pScreen->TranslateRegion) (devPriv->pCompositeClip,
					 pGC->clipOrg.x, pGC->clipOrg.y);
	 }
      }				/* end of composute clip for pixmap */
      oneRect = REGION_NUM_RECTS(devPriv->pCompositeClip) == 1;
      if (oneRect != devPriv->oneRect)
	 new_line = TRUE;
      devPriv->oneRect = oneRect;
   }
   mask = changes;
   while (mask) {
      index = lowbit(mask);
      mask &= ~index;

    /*
     * this switch acculmulates a list of which procedures might have to
     * change due to changes in the GC.  in some cases (e.g. changing one 16
     * bit tile for another) we might not really need a change, but the code
     * is being paranoid. this sort of batching wins if, for example, the alu
     * and the font have been changed, or any other pair of items that both
     * change the same thing.
     */
      switch (index) {
	case GCFunction:
	case GCForeground:
	   new_rrop = TRUE;
	   break;
	case GCPlaneMask:
	   new_rrop = TRUE;
	   new_text = TRUE;
	   break;
	case GCBackground:
	   break;
	case GCLineStyle:
	case GCLineWidth:
	   new_line = TRUE;
	   break;
	case GCJoinStyle:
	case GCCapStyle:
	   break;
	case GCFillStyle:
	   new_text = TRUE;
	   new_fillspans = TRUE;
	   new_line = TRUE;
	   new_fillarea = TRUE;
	   break;
	case GCFillRule:
	   break;
	case GCTile:
	   new_fillspans = TRUE;
	   new_fillarea = TRUE;
	   break;

	case GCStipple:
	   if (pGC->stipple) {
	      int   width = pGC->stipple->drawable.width;
	      PixmapPtr nstipple;

	      if ((width <= 32) && !(width & (width - 1)) &&
		  (nstipple = cfbCopyPixmap(pGC->stipple))) {
		 cfbPadPixmap(nstipple);
		 cfbDestroyPixmap(pGC->stipple);
		 pGC->stipple = nstipple;
	      }
	   }
	   new_fillspans = TRUE;
	   new_fillarea = TRUE;
	   break;

	case GCTileStipXOrigin:
	   new_rotate = TRUE;
	   break;

	case GCTileStipYOrigin:
	   new_rotate = TRUE;
	   break;

	case GCFont:
	   new_text = TRUE;
	   break;
	case GCSubwindowMode:
	   break;
	case GCGraphicsExposures:
	   break;
	case GCClipXOrigin:
	   break;
	case GCClipYOrigin:
	   break;
	case GCClipMask:
	   break;
	case GCDashOffset:
	   break;
	case GCDashList:
	   break;
	case GCArcMode:
	   break;
	default:
	   break;
      }
   }

 /*
  * If the drawable has changed,  check its depth & ensure suitable entries
  * are in the proc vector.
  */
   if (pDrawable->serialNumber != (pGC->serialNumber & (DRAWABLE_SERIAL_BITS))) {
      new_fillspans = TRUE;	/* deal with FillSpans later */
   }
   if (new_rotate || new_fillspans) {
      Bool  new_pix = FALSE;

      xrot = pGC->patOrg.x + pDrawable->x;
      yrot = pGC->patOrg.y + pDrawable->y;

      switch (pGC->fillStyle) {
	case FillTiled:
	   if (!pGC->tileIsPixel) {
	      int   width = pGC->tile.pixmap->drawable.width * PSZ;

	      if ((width <= PPW*PSZ) && !(width & (width - 1))) {
		 cfbCopyRotatePixmap(pGC->tile.pixmap,
				     &devPriv->pRotatedPixmap,
				     xrot, yrot);
		 new_pix = TRUE;
	      }
	   }
	   break;
#if (PPW == 4)
	case FillStippled:
	case FillOpaqueStippled:
	   {
	      int   width = pGC->stipple->drawable.width;

	      if ((width <= 32) && !(width & (width - 1))) {
		 mfbCopyRotatePixmap(pGC->stipple,
				     &devPriv->pRotatedPixmap, xrot, yrot);
		 new_pix = TRUE;
	      }
	   }
	   break;
#endif
      }
      if (!new_pix && devPriv->pRotatedPixmap) {
	 cfbDestroyPixmap(devPriv->pRotatedPixmap);
	 devPriv->pRotatedPixmap = (PixmapPtr) NULL;
      }
   }
   if (new_rrop) {
      int   old_rrop;

      old_rrop = devPriv->rop;
      devPriv->rop = cfbReduceRasterOp(pGC->alu, pGC->fgPixel,
				       pGC->planemask,
				       &devPriv->and, &devPriv->xor);
      if (old_rrop == devPriv->rop)
	 new_rrop = FALSE;
      else {
#if PPW ==  4
	 new_line = TRUE;
	 new_text = TRUE;
#endif
	 new_fillspans = TRUE;
	 new_fillarea = TRUE;
      }
   }
   if (pWin && pGC->ops->devPrivate.val != 2) {
      if (pGC->ops->devPrivate.val == 1)
	 miDestroyGCOps(pGC->ops);

      pGC->ops = cfbCreateOps(&s3Ops);
      pGC->ops->devPrivate.val = 2;

    /*
     * Make sure that everything is properly initialized the first time
     * through
     */
      new_rrop = new_line = new_text = new_fillspans = new_fillarea = TRUE;
   } else if (!pWin && (new_rrop || new_fillspans || new_text || new_fillarea || new_line)) {
      GCOps *newops;

      if ((newops = matchCommon(pGC, devPriv))) {
	 if (pGC->ops->devPrivate.val)
	    miDestroyGCOps(pGC->ops);
	 pGC->ops = newops;
	 new_rrop = new_line = new_fillspans = new_text = new_fillarea = 0;
      } else {
	 if (!pGC->ops->devPrivate.val) {
	    pGC->ops = cfbCreateOps(pGC->ops);
	    pGC->ops->devPrivate.val = 1;
	 } else if (pGC->ops->devPrivate.val != 1) {
	    miDestroyGCOps(pGC->ops);
	    pGC->ops = cfbCreateOps(&cfbNonTEOps);
	    pGC->ops->devPrivate.val = 1;
	    new_rrop = new_line = new_text = new_fillspans = new_fillarea = TRUE;
	 }
      }
   }
 /* deal with the changes we've collected */
   if (new_line) {
      if (pWin) {
	 if (pGC->lineWidth == 0)
	    pGC->ops->PolyArc = miZeroPolyArc;
	 else
	    pGC->ops->PolyArc = miPolyArc;
	 pGC->ops->PolySegment = miPolySegment;
	 switch (pGC->lineStyle) {
	   case LineSolid:
	      if (pGC->lineWidth == 0) {
		 if (pGC->fillStyle == FillSolid) {
		    pGC->ops->Polylines = s3Line;
		    pGC->ops->PolySegment = s3Segment;
		 } else
		    pGC->ops->Polylines = miZeroLine;
	      } else
		 pGC->ops->Polylines = miWideLine;
	      break;
	   case LineOnOffDash:
	   case LineDoubleDash:
#define USES3DLINE
#if defined(USES3DLINE)
	      if (pGC->lineWidth == 0) {
		 if (pGC->fillStyle == FillSolid) {
		    pGC->ops->Polylines = s3Dline;
		    pGC->ops->PolySegment = s3Dsegment;
		 } else
		    pGC->ops->Polylines = miWideDash;
	      } else
		 pGC->ops->Polylines = miWideDash;
#else
	      pGC->ops->Polylines = miWideDash;
#endif
	      break;
	 }
      } else {
	 pGC->ops->FillPolygon = miFillPolygon;
	 if (devPriv->oneRect && pGC->fillStyle == FillSolid) {
	    switch (devPriv->rop) {
	      case GXcopy:
		 pGC->ops->FillPolygon = cfbFillPoly1RectCopy;
		 break;
	      default:
		 pGC->ops->FillPolygon = cfbFillPoly1RectGeneral;
		 break;
	    }
	 }
	 if (pGC->lineWidth == 0) {
#if PPW == 4
	    if ((pGC->lineStyle == LineSolid) && (pGC->fillStyle == FillSolid)) {
	       switch (devPriv->rop) {
		 case GXxor:
		    pGC->ops->PolyArc = cfbZeroPolyArcSS8Xor;
		    break;
		 case GXcopy:
		    pGC->ops->PolyArc = cfbZeroPolyArcSS8Copy;
		    break;
		 default:
		    pGC->ops->PolyArc = cfbZeroPolyArcSS8General;
		    break;
	       }
	    } else
#endif
	       pGC->ops->PolyArc = miZeroPolyArc;
	 } else
	    pGC->ops->PolyArc = miPolyArc;
	 pGC->ops->PolySegment = miPolySegment;
	 switch (pGC->lineStyle) {
	   case LineSolid:
	      if (pGC->lineWidth == 0) {
		 if (pGC->fillStyle == FillSolid) {
		    pGC->ops->Polylines = cfbLineSS;
		    pGC->ops->PolySegment = cfbSegmentSS;
		 } else
		    pGC->ops->Polylines = miZeroLine;
	      } else
		 pGC->ops->Polylines = miWideLine;
	      break;
	   case LineOnOffDash:
	   case LineDoubleDash:
	      if (pGC->lineWidth == 0 && pGC->fillStyle == FillSolid) {
		 pGC->ops->Polylines = cfbLineSD;
		 pGC->ops->PolySegment = cfbSegmentSD;
	      } else
		 pGC->ops->Polylines = miWideDash;
	      break;
	 }
      }
   }
   if (new_text && (pGC->font)) {
      if (pWin) {
	 pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
	 pGC->ops->ImageGlyphBlt = miImageGlyphBlt;
      } else {
	 if (FONTMAXBOUNDS(pGC->font, rightSideBearing) -
	     FONTMINBOUNDS(pGC->font, leftSideBearing) > 32 ||
	     FONTMINBOUNDS(pGC->font, characterWidth) < 0) {
	    pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
	    pGC->ops->ImageGlyphBlt = miImageGlyphBlt;
	 } else {
#if PPW == 4
	    if (pGC->fillStyle == FillSolid) {
	       if (devPriv->rop == GXcopy)
		  pGC->ops->PolyGlyphBlt = cfbPolyGlyphBlt8;
	       else
		  pGC->ops->PolyGlyphBlt = cfbPolyGlyphRop8;
	    } else
#endif
	       pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
	  /* special case ImageGlyphBlt for terminal emulator fonts */
	    if (TERMINALFONT(pGC->font) &&
		(pGC->planemask & PMSK) == PMSK
#if PPW == 4
		&& FONTMAXBOUNDS(pGC->font, characterWidth) >= 4
#endif
	       ) {
#if PPW == 4
	       pGC->ops->ImageGlyphBlt = cfbTEGlyphBlt8;
#else
	       pGC->ops->ImageGlyphBlt = cfbTEGlyphBlt;
#endif
	    } else {
#if PPW == 4
	       pGC->ops->ImageGlyphBlt = cfbImageGlyphBlt8;
#else
	       pGC->ops->ImageGlyphBlt = miImageGlyphBlt;
#endif
	    }
	 }
      }
   }
   if (new_fillspans) {
      if (pWin) {
	 switch (pGC->fillStyle) {
	   case FillSolid:
	      pGC->ops->FillSpans = s3SolidFSpans;
	      break;
	   case FillTiled:
	      pGC->ops->FillSpans = s3TiledFSpans;
	      break;
	   case FillStippled:
	      pGC->ops->FillSpans = s3StipFSpans;
	      break;
	   case FillOpaqueStippled:
	      pGC->ops->FillSpans = s3OStipFSpans;
	      break;
	   default:
	      FatalError("s3ValidateGC: illegal fillStyle\n");
	 }
      } else {
	 switch (pGC->fillStyle) {
	   case FillSolid:
	      switch (devPriv->rop) {
		case GXcopy:
		   pGC->ops->FillSpans = cfbSolidSpansCopy;
		   break;
		case GXxor:
		   pGC->ops->FillSpans = cfbSolidSpansXor;
		   break;
		default:
		   pGC->ops->FillSpans = cfbSolidSpansGeneral;
		   break;
	      }
	      break;

	   case FillTiled:
	      if (devPriv->pRotatedPixmap) {
		 if (pGC->alu == GXcopy && (pGC->planemask & PMSK) == PMSK)
		    pGC->ops->FillSpans = cfbTile32FSCopy;
		 else
		    pGC->ops->FillSpans = cfbTile32FSGeneral;
	      } else
		 pGC->ops->FillSpans = cfbUnnaturalTileFS;
	      break;
	   case FillStippled:

#if PPW == 4
	      if (devPriv->pRotatedPixmap)
		 pGC->ops->FillSpans = cfb8Stipple32FS;
	      else
#endif
		 pGC->ops->FillSpans = cfbUnnaturalStippleFS;
	      break;
	   case FillOpaqueStippled:
#if PPW == 4
	      if (devPriv->pRotatedPixmap)
		 pGC->ops->FillSpans = cfb8OpaqueStipple32FS;
	      else
#endif
		 pGC->ops->FillSpans = cfbUnnaturalStippleFS;
	      break;
	   default:
	      FatalError("s3ValidateGC: illegal fillStyle\n");
	 }
      }
   }				/* end of new_fillspans */
   if (new_fillarea) {
      if (pWin) {
	 pGC->ops->PolyFillRect = s3PolyFillRect;
	 pGC->ops->PolyFillArc = miPolyFillArc;
	 pGC->ops->PushPixels = miPushPixels;
      } else {
#if PPW != 4
	 pGC->ops->PolyFillRect = miPolyFillRect;
	 if (pGC->fillStyle == FillSolid || pGC->fillStyle == FillTiled) {
	    pGC->ops->PolyFillRect = cfbPolyFillRect;
	 }
#endif
#if PPW == 4
	 pGC->ops->PushPixels = mfbPushPixels;
	 if (pGC->fillStyle == FillSolid && devPriv->rop == GXcopy)
	    pGC->ops->PushPixels = cfbPushPixels8;
#endif
	 pGC->ops->PolyFillArc = miPolyFillArc;
	 if (pGC->fillStyle == FillSolid) {
	    switch (devPriv->rop) {
	      case GXcopy:
		 pGC->ops->PolyFillArc = cfbPolyFillArcSolidCopy;
		 break;
	      default:
		 pGC->ops->PolyFillArc = cfbPolyFillArcSolidGeneral;
		 break;
	    }
	 }
      }
   }
}


