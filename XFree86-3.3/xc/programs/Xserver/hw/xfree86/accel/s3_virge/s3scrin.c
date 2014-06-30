/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3scrin.c,v 3.3.2.1 1997/05/24 08:36:03 dawes Exp $ */
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

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/
/*
Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the S3 by Jon N. Tombs (jon@esix2.us.es)
*/
/* $XConsortium: s3scrin.c /main/4 1996/10/25 15:37:52 kaleb $ */

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
#include "cfb24.h"
#include "cfb32.h"
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "cfbmskbits.h"
#include "mibstore.h"
#include "s3v.h"
#include "xf86Priv.h"

extern RegionPtr mfbPixmapToRegion();
extern Bool mfbRegisterCopyPlaneProc();

extern int defaultColorVisualClass;
extern xrgb xf86weight;

extern RegionPtr miCopyPlane();


static unsigned long cfbGeneration = 0;

miBSFuncRec s3BSFuncRec = {
    s3SaveAreas,
    s3RestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
s3ScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
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
    pointer oldDevPrivate=NULL;

    rootdepth = 0;
    bitsPerRGB = 6;
    switch (s3InfoRec.bitsPerPixel) {
    case 8:
	if (s3DAC8Bit)
	    bitsPerRGB = 8;
	break;
    case 16:
	if (xf86weight.red == 5 && xf86weight.green == 5
	    && xf86weight.blue == 5)
	    bitsPerRGB = 5;
	break;
    case 24:
    case 32:
	bitsPerRGB = 8;
	break;
    }

    if (cfbGeneration != serverGeneration) {
	/* Only TrueColor for 16/24/32bpp */
	if (s3InfoRec.bitsPerPixel > 8) {
	    if (!cfbSetVisualTypes(s3InfoRec.depth, 1 << TrueColor,
				   bitsPerRGB))
		return FALSE;
	}
	if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
	    &defaultVisual, 1<<(s3InfoRec.bitsPerPixel - 1), bitsPerRGB))
	    return FALSE;
	cfbGeneration = serverGeneration;
    }

    if (rootdepth == 15 || rootdepth == 16 || rootdepth == 24 || rootdepth == 32) {
    /*
     * There are several possible color weightings at 16/24bpp.
     * Set them up here.
     */
        for (i = 0, visual = visuals; i < nvisuals; i++, visual++)
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

    cfbWindowPrivateIndex = cfbGCPrivateIndex = -1;
    pScreen->defColormap = FakeClientID(0);
    /*
     * Set whitePixel to 1 and blackPixel to 0 to workaround problems
     * with some S3 chips (Trio64/32, 805i)
     */
    pScreen->whitePixel = (Pixel) 1;
    pScreen->blackPixel = (Pixel) 0;
    XF86FLIP_PIXELS();
    pScreen->QueryBestSize = mfbQueryBestSize;
    /* SaveScreen */
    pScreen->GetImage = s3GetImage;
    pScreen->GetSpans = s3GetSpans;
    pScreen->PaintWindowBackground = miPaintWindow;
    pScreen->PaintWindowBorder = miPaintWindow;
    pScreen->CopyWindow = s3CopyWindow;
    pScreen->RealizeFont = s3RealizeFont;
    pScreen->UnrealizeFont = s3UnrealizeFont;
#if 0
    if (xf86bpp == 24)
       rootdepth = 24;  /* HACK24 ! */
#endif
    switch (xf86bpp) {
    case 8:
	pScreen->CreateGC = s3CreateGC;
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
	mfbRegisterCopyPlaneProc (pScreen, s3CopyPlane);
	break;
    case 15:
    case 16:
	pScreen->CreateGC = s3CreateGC16;
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
	mfbRegisterCopyPlaneProc (pScreen, s3CopyPlane);
	break;
     case 24:
	pScreen->CreateGC = s3CreateGC24;
        if (!cfb24AllocatePrivates(pScreen, &cfbWindowPrivateIndex,
	    &cfbGCPrivateIndex))
	    return FALSE;
	pScreen->CreateWindow = cfb24CreateWindow;
	pScreen->DestroyWindow = cfb24DestroyWindow;
	pScreen->PositionWindow = cfb24PositionWindow;
	pScreen->ChangeWindowAttributes = cfb24ChangeWindowAttributes;
	pScreen->RealizeWindow = cfb24MapWindow;
	pScreen->UnrealizeWindow = cfb24UnmapWindow;
	pScreen->CreatePixmap = cfb24CreatePixmap;
	pScreen->DestroyPixmap = cfb24DestroyPixmap;
	mfbRegisterCopyPlaneProc (pScreen, s3CopyPlane);
	break;
     case 32:
	pScreen->CreateGC = s3CreateGC32;
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
	mfbRegisterCopyPlaneProc (pScreen, s3CopyPlane);
	break;
     default:
	    FatalError("root depth %d not (yet?) supported\n", rootdepth);
    }
    pScreen->CreateColormap = cfbInitializeColormap;
    pScreen->DestroyColormap = (DestroyColormapProcPtr)NoopDDA;
    pScreen->ResolveColor = cfbResolveColor;
    pScreen->BitmapToRegion = mfbPixmapToRegion;
/* XXX: new cursor?   pScreen->BlockHandler = s3BlockHandler;*/

    if (rootdepth != 8) {
	oldDevPrivate = pScreen->devPrivate;
    }
    Rstatus = miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			&s3BSFuncRec);
    if (xf86bpp == 32 /*> 24*/) {
	pScreen->CreateScreenResources = cfb32CreateScreenResources;
	pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    else if (xf86bpp == 24 /*> 16*/) {
	pScreen->CreateScreenResources = cfb24CreateScreenResources;
	pScreen->devPrivates[cfb24ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    else if (xf86bpp == 16 /*> 8*/) {
	pScreen->CreateScreenResources = cfb16CreateScreenResources;
	pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    return Rstatus;

}
