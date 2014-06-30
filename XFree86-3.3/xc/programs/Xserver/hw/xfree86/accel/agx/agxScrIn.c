/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxScrIn.c,v 3.10 1996/12/23 06:32:57 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or MIT not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun and
M.I.T. make no representations about the suitability of this
software for any purpose. It is provided "as is" without any
express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

KEVIN E. MARTIN  AND HENRY A. WORTH DISCLAIMS ALL WARRANTIES 
WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES 
OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS 
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES 
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
Modified for the AGX    by Henry A. Worth  (haw30@eng.amdahl.com)

********************************************************/
/* $XConsortium: agxScrIn.c /main/6 1996/02/21 17:18:26 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "Xproto.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormap.h"
#include "colormapst.h"
#include "cfb.h"
#include "cfb16.h"
#ifdef AGX_32BPP
#include "cfb32.h"
#endif
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "cfbmskbits.h"
#include "mibstore.h"
#include "agx.h"
#include "regagx.h"
#include "hercRamDac.h"
#include "xf86Priv.h"

extern RegionPtr mfbPixmapToRegion();
extern Bool mfbRegisterCopyPlaneProc();

extern int defaultColorVisualClass;
extern xrgb xf86weight;

extern RegionPtr miCopyPlane();


static unsigned long cfbGeneration = 0;

miBSFuncRec agxBSFuncRec = {
    agxSaveAreas,
    agxRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
agxScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize; int ysize;	/* in pixels */
    int dpix; int dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    VisualPtr visuals;
    DepthPtr depths;
    int nvisuals;
    int ndepths;
    int rootdepth;
    VisualID defaultVisual;
    int bitsPerRGB;
    int	i;
    Bool Rstatus;
    VisualPtr visual;
    pointer oldDevPrivate;

    rootdepth = 0;
    bitsPerRGB = 6;
    switch (agxInfoRec.bitsPerPixel) {
    case 8:
	if (xf86Dac8Bit)
	    bitsPerRGB = 8;
	break;
    case 16:
	if (xf86weight.red == 5 && xf86weight.green == 5
	    && xf86weight.blue == 5)
	    bitsPerRGB = 5;
	break;
    case 32:
	bitsPerRGB = 8;
	break;
    }
	
    if (cfbGeneration != serverGeneration) {
	/* Only TrueColor for 16/32bpp */
	if (agxInfoRec.bitsPerPixel > 8) {
	    if (!cfbSetVisualTypes(agxInfoRec.depth, 1 << TrueColor,
				   bitsPerRGB))
		return FALSE;
	}
	if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
	    &defaultVisual, 1<<(agxInfoRec.bitsPerPixel - 1), bitsPerRGB))
	    return FALSE;
	cfbGeneration = serverGeneration;
    }

    if (rootdepth > 8) {
    /*
     * There are several possible color weightings at 16/24bpp.
     * Set them up here.
     */
        if( !hercBigDAC && rootdepth > 16 ) {
            /* RGBX */
            for (i = 0, visual = visuals; i < nvisuals; i++, visual++) {
               if (visual->class == DirectColor || visual->class == TrueColor) {
                   visual->offsetBlue = xf86weight.green + xf86weight.red;
                   visual->offsetGreen = xf86weight.red;
                   visual->offsetRed = 0;
                   visual->blueMask = ((1 << xf86weight.blue) - 1)
                           << visual->offsetBlue;
                   visual->greenMask = ((1 << xf86weight.green) - 1)
                           << visual->offsetGreen;
                   visual->redMask = (1 << xf86weight.red) - 1;
                }
           }
        }
        else {
           /* Hercules 2MB Graphite -- BGRX for 24/32BPP */
           for (i = 0, visual = visuals; i < nvisuals; i++, visual++) {
	       if (visual->class == DirectColor || visual->class == TrueColor) {
	           visual->offsetRed = xf86weight.green + xf86weight.blue;
	           visual->offsetGreen = xf86weight.blue;
	           visual->offsetBlue = 0;
	           visual->redMask = ((1 << xf86weight.red) - 1)
			   << visual->offsetRed;
	           visual->greenMask = ((1 << xf86weight.green) - 1)
			   << visual->offsetGreen;
	           visual->blueMask = (1 << xf86weight.blue) - 1;
	        }
           }
        }
    }

    cfbWindowPrivateIndex = cfbGCPrivateIndex = -1;
    pScreen->defColormap = FakeClientID(0);
    pScreen->whitePixel = (Pixel) 1;
    pScreen->blackPixel = (Pixel) 0;
    XF86FLIP_PIXELS();
    pScreen->QueryBestSize = mfbQueryBestSize;
    /* SaveScreen */
    pScreen->GetImage = agxGetImage;
    pScreen->GetSpans = agxGetSpans;
#if 1
    pScreen->PaintWindowBackground = agxPaintWindow;
    pScreen->PaintWindowBorder = agxPaintWindow;
#else
    pScreen->PaintWindowBackground = miPaintWindow;
    pScreen->PaintWindowBorder = miPaintWindow;
#endif
    pScreen->CopyWindow = agxCopyWindow;
    pScreen->RealizeFont = agxRealizeFont;
    pScreen->UnrealizeFont = agxUnrealizeFont;
    switch (rootdepth) {
    case 8:
	pScreen->CreateGC = agxCreateGC;
        if (!cfbAllocatePrivates(pScreen, &cfbWindowPrivateIndex,
	    &cfbGCPrivateIndex))
	    return FALSE;
	pScreen->CreateWindow = cfbCreateWindow;
	pScreen->DestroyWindow = cfbDestroyWindow;
	pScreen->PositionWindow = cfbPositionWindow;
	pScreen->ChangeWindowAttributes = cfbChangeWindowAttributes;
	pScreen->RealizeWindow = cfbMapWindow;
	pScreen->UnrealizeWindow = cfbUnmapWindow;
	pScreen->CreatePixmap = cfbCreatePixmap;
	pScreen->DestroyPixmap = cfbDestroyPixmap;
	mfbRegisterCopyPlaneProc (pScreen, agxCopyPlane);
	break;
    case 15:
    case 16:
	pScreen->CreateGC = agxCreateGC16;
        if (!cfb16AllocatePrivates(pScreen, &cfbWindowPrivateIndex,
	    &cfbGCPrivateIndex))
	    return FALSE;
	pScreen->CreateWindow = cfb16CreateWindow;
	pScreen->DestroyWindow = cfb16DestroyWindow;
	pScreen->PositionWindow = cfb16PositionWindow;
	pScreen->ChangeWindowAttributes = cfb16ChangeWindowAttributes;
	pScreen->RealizeWindow = cfb16MapWindow;
	pScreen->UnrealizeWindow = cfb16UnmapWindow;
	pScreen->CreatePixmap = cfb16CreatePixmap;
	pScreen->DestroyPixmap = cfb16DestroyPixmap;
	mfbRegisterCopyPlaneProc (pScreen, agxCopyPlane);	
	break;
#ifdef AGX_32BPP
     case 24:
     case 32:
	pScreen->CreateGC = agxCreateGC32;
        if (!cfb32AllocatePrivates(pScreen, &cfbWindowPrivateIndex,
	    &cfbGCPrivateIndex))
	    return FALSE;
	pScreen->CreateWindow = cfb32CreateWindow;
	pScreen->DestroyWindow = cfb32DestroyWindow;
	pScreen->PositionWindow = cfb32PositionWindow;
	pScreen->ChangeWindowAttributes = cfb32ChangeWindowAttributes;
	pScreen->RealizeWindow = cfb32MapWindow;
	pScreen->UnrealizeWindow = cfb32UnmapWindow;
	pScreen->CreatePixmap = cfb32CreatePixmap;
	pScreen->DestroyPixmap = cfb32DestroyPixmap;
	mfbRegisterCopyPlaneProc (pScreen, agxCopyPlane);	
	break;
#endif
     default:
	    FatalError("root depth %d not (yet?) supported\n", rootdepth);
    }
    pScreen->CreateColormap = cfbInitializeColormap;
    pScreen->DestroyColormap = (DestroyColormapProcPtr)NoopDDA;
    pScreen->ResolveColor = cfbResolveColor;
    pScreen->BitmapToRegion = mfbPixmapToRegion;
/* XXX: new cursor?   pScreen->BlockHandler = agxBlockHandler;*/

    if (rootdepth != 8) {
	oldDevPrivate = pScreen->devPrivate;
    }
    Rstatus = miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			&agxBSFuncRec);
#ifdef AGX_32BPP
    if (rootdepth > 16) {
	pScreen->CreateScreenResources = cfb32CreateScreenResources;
	pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    else 
#endif
    if (rootdepth > 8) {
	pScreen->CreateScreenResources = cfb16CreateScreenResources;
	pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    return Rstatus;

}
