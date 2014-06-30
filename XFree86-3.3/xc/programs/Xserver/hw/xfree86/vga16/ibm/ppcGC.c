/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcGC.c,v 3.6 1996/12/23 06:53:00 dawes Exp $ */
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


Copyright IBM Corporation 1987,1988,1989
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that 
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.


Copyright IBM Corporation 1987,1988
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
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

*/

/* $XConsortium: ppcGC.c /main/6 1996/02/21 17:57:38 kaleb $ */

#include "X.h"
#include "Xproto.h"
#include "windowstr.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "misc.h"
#include "dixfont.h" /* GJA */
#include "gcstruct.h"
#include "cursorstr.h"
#include "region.h"

#include "mi.h"
#include "mistruct.h"

#include "OScompiler.h"

#include "ppc.h"

#include "ibmTrace.h"

#include "vgaVideo.h"

extern int ibmAllowBackingStore ;
extern int mfbGCPrivateInterest;
#define ppcGCInterestValidateMask \
( GCLineStyle | GCLineWidth | GCJoinStyle | GCBackground | GCForeground \
| GCFunction | GCPlaneMask | GCFillStyle | GC_CALL_VALIDATE_BIT         \
| GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode )


/* GJA -- we modified the following function to get rid of
 * the records in file vgaData.c
 */
static GCFuncs vgaGCFuncs = {
	ppcValidateGC,
	(void (*)())NoopDDA,
	(void (*)())NoopDDA,
	ppcDestroyGC,
	ppcChangeClip,
	ppcDestroyClip,
	ppcCopyClip
};


static ppcPrivGC vgaPrototypeGCPriv = {
	GXcopy,	/* unsigned char	rop */
	0,	/* unsigned char	ropOpStip */
	0,	/* unsigned char	ropFillArea */
	TRUE,		/* short	fExpose */
	FALSE,		/* short	freeCompClip */
	NullPixmap,	/* PixmapPtr	pRotatedPixmap */
	0,	/* RegionPtr	pCompositeClip */
	ppcAreaFill,	/* void 	(* FillArea)() */
		{
		    VGA_ALLPLANES,	/* unsigned long	planemask */
		    1,			/* unsigned long	fgPixel */
		    0,			/* unsigned long	bgPixel */
		    GXcopy,		/* int			alu */
		    FillSolid,		/* int			fillStyle */
		}, /* ppcReducedRrop	colorRrop  */
	-1,	/* short lastDrawableType */
	-1,	/* short lastDrawableDepth */
	(void (*)())NoopDDA, /* GJA -- void (* cacheIGBlt)() */
	(void (*)())NoopDDA, /* GJA -- void (* cachePGBlt) () */
	0	/* pointer devPriv */
} ;

void v16ImageGlyphBlt(), v16PolyFillArc(), v16ZeroPolyArc();

static GCOps vgaGCOps = {
	ppcSolidWindowFS,	/*  void (* FillSpans)() */
	ppcSetSpans,		/*  void (* SetSpans)()	 */
	miPutImage,		/*  void (* PutImage)()	 */
	vga16CopyArea,		/*  RegionPtr (* CopyArea)()	 */
	miCopyPlane,		/*  void (* CopyPlane)() */
	ppcPolyPoint,		/*  void (* PolyPoint)() */
	miZeroLine,		/*  void (* Polylines)() */
	miPolySegment,		/*  void (* PolySegment)() */
	miPolyRectangle,	/*  void (* PolyRectangle)() */
	v16ZeroPolyArc,		/*  void (* PolyArc)()	 */
	miFillPolygon,		/*  void (* FillPolygon)() */
	miPolyFillRect,		/*  void (* PolyFillRect)() */
	v16PolyFillArc,		/*  void (* PolyFillArc)() */
	miPolyText8,		/*  int (* PolyText8)()	 */
	miPolyText16,		/*  int (* PolyText16)() */
	miImageText8,		/*  void (* ImageText8)() */
	miImageText16,		/*  void (* ImageText16)() */
	(void (*)()) v16ImageGlyphBlt,	/*  GJA -- void (* ImageGlyphBlt)() */
	(void (*)()) miPolyGlyphBlt,	/*  GJA -- void (* PolyGlyphBlt)() */
	miPushPixels,		/*  void (* PushPixels)() */
#ifdef NEED_LINEHELPER
	miMiter,		/*  void (* LineHelper)() */
#endif
	NULL
};

Bool
ppcCreateGC( pGC )
register GCPtr pGC ;
{
	ppcScrnPriv *devScrnPriv = (ppcScrnPriv *) /* GJA */
		pGC->pScreen->devPrivate ;
	ppcPrivGC *pPriv ;
	GCOps *pOps ;
	static DDXPointRec zeroOrg = { 0, 0 }; /* GJA */

	if ( pGC->depth == 1 )
		{
		return (mfbCreateGC(pGC));
		}

	if ( !( pPriv = (ppcPrivGC *) xalloc( sizeof( ppcPrivGC ) ) ) )
		return FALSE ;

	if ( !( pOps = (GCOps *) xalloc( sizeof( GCOps ) ) ) ) {
		xfree(pPriv);
		return FALSE;
	}
	
        /* Now we initialize the GC fields */
	pGC->miTranslate = 1;
	pGC->tileIsPixel = 1;
	pGC->unused = 0;
	pGC->planemask = VGA_ALLPLANES;
	pGC->fgPixel = VGA_BLACK_PIXEL;
	pGC->bgPixel = VGA_WHITE_PIXEL;
	pGC->funcs = &vgaGCFuncs;
	/* ops, -- see below */
	
	/* GJA: I don't like this code:
         * they allocated a mfbPrivGC, ignore the allocated data and place
         * a pointer to a ppcPrivGC in its slot.
         */
	*pPriv = vgaPrototypeGCPriv;
	(pGC->devPrivates[mfbGCPrivateIndex].ptr) = (pointer) pPriv;

	/* Set the vgaGCOps */
	*pOps = vgaGCOps;
	pOps->devPrivate.val = 1;
	pGC->ops = pOps;

	return TRUE ;
}

void
ppcDestroyGC( pGC )
    register GC	*pGC ;

{
    register ppcPrivGC *pPriv ;

    TRACE( ( "ppcDestroyGC(pGC=0x%x)\n", pGC ) ) ;

    pPriv = (ppcPrivGC *) ( pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;

    /* (ef) 11/9/87 -- ppc doesn't use rotated tile or stipple, but */
    /*		*does* call mfbValidateGC under some conditions.    */
    /*		mfbValidateGC *does* use rotated tile and stipple   */
    if ( pPriv->pRotatedPixmap )
	mfbDestroyPixmap( pPriv->pRotatedPixmap ) ;

    if ( pPriv->freeCompClip && pPriv->pCompositeClip )
	(* pGC->pScreen->RegionDestroy)( pPriv->pCompositeClip ) ;
    if(pGC->ops->devPrivate.val) xfree( pGC->ops );
    xfree( pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
    return ;
}

Mask
ppcChangePixmapGC( pGC, changes )
register GC *pGC ;
register Mask changes ;
{
register ppcPrivGCPtr devPriv = (ppcPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;
register unsigned long int index ; /* used for stepping through bitfields */
register Mask bsChanges = 0 ;

#define LOWBIT( x ) ( x & - x ) /* Two's complement */
while ( index = LOWBIT( changes ) ) {
    switch ( index ) {

	case GCLineStyle:
	case GCLineWidth:
	    pGC->ops->Polylines = ( ! pGC->lineWidth )
		 ? miZeroLine
		 : ( ( pGC->lineStyle == LineSolid )
			 ? miWideLine : miWideDash ) ;
	    changes &= ~( GCLineStyle | GCLineWidth ) ;
	    break ;

	case GCJoinStyle:
#ifdef NEED_LINEHELPER
	    pGC->ops->LineHelper = ( pGC->joinStyle == JoinMiter )
			    ? miMiter : miNotMiter ;
#endif
	    changes &= ~ index ; /* i.e. changes &= ~ GCJoinStyle */
	    break ;

	case GCBackground:
	    if ( pGC->fillStyle != FillOpaqueStippled ) {
		changes &= ~ index ; /* i.e. changes &= ~GCBackground */
		break ;
	    } /* else Fall Through */
	case GCForeground:
	    if ( pGC->fillStyle == FillTiled ) {
		changes &= ~ index ; /* i.e. changes &= ~GCForeground */
		break ;
	    } /* else Fall Through */
	case GCFunction:
	case GCPlaneMask:
	case GCFillStyle:
	    { /* new_fill */
		int fillStyle = devPriv->colorRrop.fillStyle ;
		/* install a suitable fillspans */
		if ( fillStyle == FillSolid )
		    pGC->ops->FillSpans = ppcSolidPixmapFS ;
		else if ( fillStyle == FillStippled )
		    pGC->ops->FillSpans = ppcStipplePixmapFS ;
		else if ( fillStyle == FillOpaqueStippled )
		    pGC->ops->FillSpans = ppcOpStipplePixmapFS ;
		else /*  fillStyle == FillTiled */
		    pGC->ops->FillSpans = ppcTilePixmapFS ;
		changes &= ~( GCBackground | GCForeground
			    | GCFunction | GCPlaneMask | GCFillStyle ) ;
		break ;
	    } /* end of new_fill */

	default:
	    ErrorF( "ppcChangePixmapGC: Unexpected GC Change\n" ) ;
	    changes &= ~ index ; /* Remove it anyway */
	    break ;
	}
}

return bsChanges ;
}

/* Clipping conventions
	if the drawable is a window
	    CT_REGION ==> pCompositeClip really is the composite
	    CT_other ==> pCompositeClip is the window clip region
	if the drawable is a pixmap
	    CT_REGION ==> pCompositeClip is the translated client region
		clipped to the pixmap boundary
	    CT_other ==> pCompositeClip is the pixmap bounding box
*/

void
ppcValidateGC( pGC, changes, pDrawable )
    register GCPtr	pGC ;
    register Mask	changes ;
    DrawablePtr		pDrawable ;
{
    register ppcPrivGCPtr devPriv ;
    ppcScrnPriv *devScrnPriv = (ppcScrnPriv *) /* GJA */
		pGC->pScreen->devPrivate ;
    WindowPtr pWin ;
    Mask bsChanges = 0 ;

    devPriv = (ppcPrivGCPtr) (pGC->devPrivates[mfbGCPrivateIndex].ptr ) ;

    if ( pDrawable->type != devPriv->lastDrawableType ) {
	devPriv->lastDrawableType = pDrawable->type ;
	bsChanges |= vgaChangeGCtype( pGC, devPriv ) ;
	changes = ~0 ;
    }

    if ( pDrawable->depth == 1 ) {
/*	ibmAbort(); */
	NeverCalled();
    }

    if ( pDrawable->type == DRAWABLE_WINDOW ) {
	pWin = (WindowPtr) pDrawable ;
	pGC->lastWinOrg.x = pWin->drawable.x ;
	pGC->lastWinOrg.y = pWin->drawable.y ;
    }
    else {
	pWin = (WindowPtr) NULL ;
	pGC->lastWinOrg.x = 0 ;
	pGC->lastWinOrg.y = 0 ;
    }

    changes &= ppcGCInterestValidateMask ;
    /* If Nothing REALLY Changed, Just Return */
    if ( pDrawable->serialNumber == (pGC->serialNumber & DRAWABLE_SERIAL_BITS) )
	if ( !( changes &= ~ GC_CALL_VALIDATE_BIT ) )
	    return ;

    /* GJA -- start of cfb code */
    /*
     * if the client clip is different or moved OR the subwindowMode has
     * changed OR the window's clip has changed since the last validation
     * we need to recompute the composite clip 
     */
  
     if ((changes & (GCClipXOrigin|GCClipYOrigin|GCClipMask|GCSubwindowMode)) ||
 	(pDrawable->serialNumber != (pGC->serialNumber & DRAWABLE_SERIAL_BITS))
 	)
     {
 	ScreenPtr pScreen = pGC->pScreen;
  
 	if (pWin) {
 	    RegionPtr   pregWin;
 	    Bool        freeTmpClip, freeCompClip;
  
 	    if (pGC->subWindowMode == IncludeInferiors) {
 		pregWin = NotClippedByChildren(pWin);
 		freeTmpClip = TRUE;
  	    }
  	    else {
 		pregWin = &pWin->clipList;
 		freeTmpClip = FALSE;
  	    }
 	    freeCompClip = devPriv->freeCompClip;
  
 	    /*
 	     * if there is no client clip, we can get by with just keeping
 	     * the pointer we got, and remembering whether or not should
 	     * destroy (or maybe re-use) it later.  this way, we avoid
 	     * unnecessary copying of regions.  (this wins especially if
 	     * many clients clip by children and have no client clip.) 
 	     */
 	    if (pGC->clientClipType == CT_NONE) {
 		if (freeCompClip)
 		    (*pScreen->RegionDestroy) (devPriv->pCompositeClip);
 		devPriv->pCompositeClip = pregWin;
 		devPriv->freeCompClip = freeTmpClip;
  	    }
  	    else {
 		/*
 		 * we need one 'real' region to put into the composite
 		 * clip. if pregWin the current composite clip are real,
 		 * we can get rid of one. if pregWin is real and the
 		 * current composite clip isn't, use pregWin for the
 		 * composite clip. if the current composite clip is real
 		 * and pregWin isn't, use the current composite clip. if
 		 * neither is real, create a new region. 
 		 */
  
 		(*pScreen->TranslateRegion)(pGC->clientClip,
 					    pDrawable->x + pGC->clipOrg.x,
 					    pDrawable->y + pGC->clipOrg.y);
 						  
 		if (freeCompClip)
 		{
 		    (*pGC->pScreen->Intersect)(devPriv->pCompositeClip,
 					       pregWin, pGC->clientClip);
 		    if (freeTmpClip)
 			(*pScreen->RegionDestroy)(pregWin);
  		}
 		else if (freeTmpClip)
 		{
 		    (*pScreen->Intersect)(pregWin, pregWin, pGC->clientClip);
 		    devPriv->pCompositeClip = pregWin;
  		}
 		else
 		{
 		    devPriv->pCompositeClip = (*pScreen->RegionCreate)(NullBox,
 								       0);
 		    (*pScreen->Intersect)(devPriv->pCompositeClip,
 					  pregWin, pGC->clientClip);
 		}
 		devPriv->freeCompClip = TRUE;
 		(*pScreen->TranslateRegion)(pGC->clientClip,
 					    -(pDrawable->x + pGC->clipOrg.x),
 					    -(pDrawable->y + pGC->clipOrg.y));
 						  
  	    }
 	}			/* end of composite clip for a window */
  	else {
 	    BoxRec      pixbounds;
  
 	    /* XXX should we translate by drawable.x/y here ? */
 	    pixbounds.x1 = 0;
 	    pixbounds.y1 = 0;
 	    pixbounds.x2 = pDrawable->width;
 	    pixbounds.y2 = pDrawable->height;
  
 	    if (devPriv->freeCompClip)
 		(*pScreen->RegionReset)(devPriv->pCompositeClip, &pixbounds);
  	    else {
 		devPriv->freeCompClip = TRUE;
 		devPriv->pCompositeClip = (*pScreen->RegionCreate)(&pixbounds,
 								   1);
  	    }
  
 	    if (pGC->clientClipType == CT_REGION)
 	    {
 		(*pScreen->TranslateRegion)(devPriv->pCompositeClip,
 					    -pGC->clipOrg.x, -pGC->clipOrg.y);
 		(*pScreen->Intersect)(devPriv->pCompositeClip,
 				      devPriv->pCompositeClip,
 				      pGC->clientClip);
 		(*pScreen->TranslateRegion)(devPriv->pCompositeClip,
 					    pGC->clipOrg.x, pGC->clipOrg.y);
 	    }
	}			/* end of composute clip for pixmap */
     }
    /* GJA -- End of cfb code */

    changes &= ~ ( GCClipXOrigin | GCClipYOrigin | GCClipMask | GCSubwindowMode
		| GC_CALL_VALIDATE_BIT ) ;

    /* If needed, Calculate the Color Reduced Raster-Op */
    if ( changes & ( GCFillStyle | GCBackground | GCForeground
		   | GCPlaneMask | GCFunction ) )
		ppcGetReducedColorRrop( pGC, pDrawable->depth,
					&devPriv->colorRrop ) ;

	(* ( ( pDrawable->type == DRAWABLE_WINDOW )
	     ? vgaChangeWindowGC
	     : ppcChangePixmapGC ) )( pGC, changes ) ;

    return ;
}
