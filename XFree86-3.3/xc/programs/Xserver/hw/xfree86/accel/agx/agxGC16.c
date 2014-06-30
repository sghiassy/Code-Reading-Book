/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxGC16.c,v 3.3 1996/12/23 06:32:44 dawes Exp $ */
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
Modified for the AGX    by Henry A. Worth

*/
/* $XConsortium: agxGC16.c /main/3 1996/02/21 17:17:40 kaleb $ */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 */


#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "cfb.h"
#include "cfb16.h"
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

#include "agx.h"

static void agxValidateGC16();
void agxChangeGC(), agxCopyGC(), agxDestroyGC();
void agxChangeClip(), agxDestroyClip(), agxCopyClip();
void agxDestroyOps();

static GCFuncs cfbFuncs =
{
   agxValidateGC16,
   agxChangeGC,
   agxCopyGC,
   agxDestroyGC,
   agxChangeClip,
   agxDestroyClip,
   agxCopyClip,
};

static GCOps agxOps =
{
   agxSolidFSpans,
   agxSetSpans,
   cfb16PutImage,
   agxCopyArea,
   agxCopyPlane,
   miPolyPoint,
   miWideLine,
   miPolySegment,
   miPolyRectangle,
   miPolyArc,
   agxFillPolygon,
   agxPolyFillRect,
   agxPolyFillArc,
   agxPolyText8,
   agxPolyText16,
   agxImageText8,
   agxImageText16,
   miImageGlyphBlt,
   miPolyGlyphBlt,
   miPushPixels,
   NULL,
};

static GCOps cfbTEOps1Rect =
{
   cfb16SolidSpansCopy,
   cfb16SetSpans,
   cfb16PutImage,
   agxCopyArea,
   agxCopyPlane,
   cfb16PolyPoint,
   cfb16LineSS1Rect,
   cfb16SegmentSS1Rect,
   miPolyRectangle,
   cfb16ZeroPolyArcSSCopy,
   cfb16FillPoly1RectCopy,
   cfb16PolyFillRect,
   cfb16PolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfb16ImageGlyphBlt8,
   cfb16PolyGlyphBlt8,
   mfbPushPixels,
   NULL,
};

static GCOps cfbTEOps =
{
   cfb16SolidSpansCopy,
   cfb16SetSpans,
   cfb16PutImage,
   agxCopyArea,
   agxCopyPlane,
   cfb16PolyPoint,
   cfb16LineSS,
   cfb16SegmentSS,
   miPolyRectangle,
   cfb16ZeroPolyArcSSCopy,
   miFillPolygon,
   cfb16PolyFillRect,
   cfb16PolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfb16ImageGlyphBlt8,
   cfb16PolyGlyphBlt8,
   mfbPushPixels,
   NULL,
};

#if 0 /* XXX defined but not used */
static GCOps cfbNonTEOps1Rect =
{
   cfb16SolidSpansCopy,
   cfb16SetSpans,
   cfb16PutImage,
   agxCopyArea,
   agxCopyPlane,
   cfb16PolyPoint,
   cfb16LineSS1Rect,
   cfb16SegmentSS1Rect,
   miPolyRectangle,
   cfb16ZeroPolyArcSSCopy,
   cfb16FillPoly1RectCopy,
   cfb16PolyFillRect,
   cfb16PolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfb16ImageGlyphBlt8,
   cfb16PolyGlyphBlt8,
   mfbPushPixels,
   NULL,
};
#endif

static GCOps cfbNonTEOps =
{
   cfb16SolidSpansCopy,
   cfb16SetSpans,
   cfb16PutImage,
   agxCopyArea,
   agxCopyPlane,
   cfb16PolyPoint,
   cfb16LineSS,
   cfb16SegmentSS,
   miPolyRectangle,
   cfb16ZeroPolyArcSSCopy,
   miFillPolygon,
   cfb16PolyFillRect,
   cfb16PolyFillArcSolidCopy,
   miPolyText8,
   miPolyText16,
   miImageText8,
   miImageText16,
   cfb16ImageGlyphBlt8,
   cfb16PolyGlyphBlt8,
   mfbPushPixels,
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
	 if (devPriv->oneRect)
	    return &cfbTEOps1Rect;
	 else
	    return &cfbTEOps;
   }
   return (GCOps *) 0;
}

Bool
agxCreateGC16(pGC)
     register GCPtr pGC;
{
   cfbPrivGC *pPriv;

   if (PixmapWidthPaddingInfo[pGC->depth].padPixelsLog2 == LOG2_BITMAP_PAD)
       return (mfbCreateGC(pGC));
   if (pGC->depth != agxInfoRec.depth) {
        ErrorF("agxCreateGC: unsupported depth: %d\n", pGC->depth);
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
 * Clipping conventions if the drawable is a window CT_REGION ==>
 * pCompositeClip really is the composite CT_other ==> pCompositeClip is the
 * window clip region if the drawable is a pixmap CT_REGION ==>
 * pCompositeClip is the translated client region clipped to the pixmap
 * boundary CT_other ==> pCompositeClip is the pixmap bounding box
 */

static void
agxValidateGC16(pGC, changes, pDrawable)
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
		  (nstipple = cfb16CopyPixmap(pGC->stipple))) {
		 cfb16PadPixmap(nstipple);
		 cfb16DestroyPixmap(pGC->stipple);
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
	   if (!pGC->tileIsPixel) { /* XXX: check constants */
	      int   width = pGC->tile.pixmap->drawable.width * 16;

	      if ((width <= 32) && !(width & (width - 1))) {
		 cfb16CopyRotatePixmap(pGC->tile.pixmap,
				     &devPriv->pRotatedPixmap,
				     xrot, yrot);
		 new_pix = TRUE;
	      }
	   }
	   break;
#if 0 /* XXX: why nothing? */
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
	 cfb16DestroyPixmap(devPriv->pRotatedPixmap);
	 devPriv->pRotatedPixmap = (PixmapPtr) NULL;
      }
   }
   if (new_rrop) {
      int   old_rrop;

      old_rrop = devPriv->rop;
      devPriv->rop = cfb16ReduceRasterOp(pGC->alu, pGC->fgPixel,
				       pGC->planemask,
				       &devPriv->and, &devPriv->xor);
      if (old_rrop == devPriv->rop)
	 new_rrop = FALSE;
      else {
	 new_fillspans = TRUE;
	 new_fillarea = TRUE;
      }
   }
   if (pWin && pGC->ops->devPrivate.val != 2) {
      if (pGC->ops->devPrivate.val == 1)
	 agxDestroyOps(pGC->ops);

      pGC->ops = miCreateGCOps(&agxOps);
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
	    agxDestroyOps(pGC->ops);
	 pGC->ops = newops;
	 new_rrop = new_line = new_fillspans = new_text = new_fillarea = 0;
      } else {
	 if (!pGC->ops->devPrivate.val) {
	    pGC->ops = agxCreateOps(pGC->ops);
	    pGC->ops->devPrivate.val = 1;
	 } else if (pGC->ops->devPrivate.val != 1) {
	    miDestroyGCOps(pGC->ops);
	    pGC->ops = agxCreateOps(&cfbNonTEOps);
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
              if(pGC->lineWidth == 0)
              {
                  if (pGC->fillStyle == FillSolid)
                  {
                    if(!AGX_14_ONLY(agxChipId)) {
                       pGC->ops->Polylines = agxLine;
                       pGC->ops->PolySegment = agxSegment;
                    }
                    else {
                       pGC->ops->Polylines = miZeroLine;
                    }
                  }
                  else
                      pGC->ops->Polylines = miZeroLine;
              }
              else
                  pGC->ops->Polylines = miWideLine;
              break;
	   case LineOnOffDash:
	   case LineDoubleDash:
              if(pGC->lineWidth == 0)
              {
                  if (pGC->fillStyle == FillSolid)
                  {
                    if(!AGX_14_ONLY(agxChipId)) {
                       pGC->ops->Polylines = agxDLine;
                       pGC->ops->PolySegment = agxDSegment;
                    }
                    else {
                       pGC->ops->Polylines = miWideDash;
                    }
                  }
                  else
                      pGC->ops->Polylines = miWideDash;
              }
              else
                 pGC->ops->Polylines = miWideDash;
	      break;
	 }
      } else {
	 pGC->ops->FillPolygon = miFillPolygon;
	 if (devPriv->oneRect && pGC->fillStyle == FillSolid) {
	    switch (devPriv->rop) {
	      case GXcopy:
		 pGC->ops->FillPolygon = cfb16FillPoly1RectCopy;
		 break;
	      default:
		 pGC->ops->FillPolygon = cfb16FillPoly1RectGeneral;
		 break;
	    }
	 }
	 if (pGC->lineWidth == 0) {
	    if ((pGC->lineStyle == LineSolid) && (pGC->fillStyle == FillSolid)) {
	       switch (devPriv->rop) {
		 case GXxor:
		    pGC->ops->PolyArc = cfb16ZeroPolyArcSSXor;
		    break;
		 case GXcopy:
		    pGC->ops->PolyArc = cfb16ZeroPolyArcSSCopy;
		    break;
		 default:
		    pGC->ops->PolyArc = cfb16ZeroPolyArcSSGeneral;
		    break;
	       }
	    } else
	       pGC->ops->PolyArc = miZeroPolyArc;
	 } else
	    pGC->ops->PolyArc = miPolyArc;
	 pGC->ops->PolySegment = miPolySegment;
	 switch (pGC->lineStyle) {
	   case LineSolid:
	      if (pGC->lineWidth == 0) {
		 if (pGC->fillStyle == FillSolid) {
		    pGC->ops->Polylines = cfb16LineSS;
		    pGC->ops->PolySegment = cfb16SegmentSS;
		 } else
		    pGC->ops->Polylines = miZeroLine;
	      } else
		 pGC->ops->Polylines = miWideLine;
	      break;
	   case LineOnOffDash:
	   case LineDoubleDash:
	      if (pGC->lineWidth == 0 && pGC->fillStyle == FillSolid) {
		 pGC->ops->Polylines = cfb16LineSD;
		 pGC->ops->PolySegment = cfb16SegmentSD;
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
	    if (pGC->fillStyle == FillSolid) {
	       if (devPriv->rop == GXcopy)
		  pGC->ops->PolyGlyphBlt = cfb16PolyGlyphBlt8;
	       else
		  pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
	    } else {
	       pGC->ops->PolyGlyphBlt = miPolyGlyphBlt;
	  /* special case ImageGlyphBlt for terminal emulator fonts */
           /* XXX: This needs sorting out */
	       pGC->ops->ImageGlyphBlt = miImageGlyphBlt;

	    }
	 }
      }
   }



    if (new_fillspans) {
      if (pWin) {
	switch (pGC->fillStyle) {
	  case FillSolid:
	      pGC->ops->FillSpans = agxSolidFSpans;
	      break;
	  case FillTiled:
	      pGC->ops->FillSpans = agxTiledFSpans;
	      break;
	  case FillStippled:
	      pGC->ops->FillSpans = agxStipFSpans;
	      break;
	  case FillOpaqueStippled:
	      pGC->ops->FillSpans = agxOStipFSpans;
	      break;
	  default:
	      FatalError("agxValidateGC16: illegal fillStyle\n");
	  }
        } else {
	switch (pGC->fillStyle) {
	  case FillSolid:
	    switch (devPriv->rop) {
	    case GXcopy:
		pGC->ops->FillSpans = cfb16SolidSpansCopy;
		break;
	    case GXxor:
		pGC->ops->FillSpans = cfb16SolidSpansXor;
		break;
	    default:
		pGC->ops->FillSpans = cfb16SolidSpansGeneral;
		break;
	    }
	    break;
	  case FillTiled:
	    if (devPriv->pRotatedPixmap)
	    {
		if (pGC->alu == GXcopy && (pGC->planemask & 0xffff) == 0xffff)
		      pGC->ops->FillSpans = cfb16Tile32FSCopy;
		else
		      pGC->ops->FillSpans = cfb16Tile32FSGeneral;
	    }
	    else
		pGC->ops->FillSpans = cfb16UnnaturalTileFS;
	    break;
	case FillStippled:
		pGC->ops->FillSpans = cfb16UnnaturalStippleFS;
	    break;
	case FillOpaqueStippled:
		pGC->ops->FillSpans = cfb16UnnaturalStippleFS;
	    break;
	default:
	    FatalError("agxValidateGC16: illegal fillStyle\n");
	}
      }
    }
#if 0
else {
	switch (pGC->fillStyle) {
	case FillSolid:
	    pGC->ops->FillSpans = cfb16SolidSpansGeneral;
	    break;
	case FillTiled:
	    pGC->ops->FillSpans = cfb16UnnaturalTileFS;
	    break;
	case FillStippled:
	case FillOpaqueStippled:
	    pGC->ops->FillSpans = cfb16UnnaturalStippleFS;
	    break;
	}
    }
#endif
     /* end of new_fillspans */

    if (new_fillarea) {
	if (pWin) {
            pGC->ops->PolyFillRect = agxPolyFillRect;
            pGC->ops->PolyFillArc = agxPolyFillArc;
            pGC->ops->PushPixels = miPushPixels;
            pGC->ops->FillPolygon = agxFillPolygon;
	} else { /* pWin */
	   pGC->ops->PolyFillRect = miPolyFillRect;
	   if (pGC->fillStyle == FillSolid || pGC->fillStyle == FillTiled)
	   {
	      pGC->ops->PolyFillRect = cfb16PolyFillRect;
	   }
	   pGC->ops->PolyFillArc = miPolyFillArc;
	   if (pGC->fillStyle == FillSolid)
	   {
		switch (devPriv->rop)
		{
		case GXcopy:
		    pGC->ops->PolyFillArc = cfb16PolyFillArcSolidCopy;
		    break;
		default:
		    pGC->ops->PolyFillArc = cfb16PolyFillArcSolidGeneral;
		    break;
		}
	   }
	} /* if pWin */
    } /* if new_fillarea */
}



