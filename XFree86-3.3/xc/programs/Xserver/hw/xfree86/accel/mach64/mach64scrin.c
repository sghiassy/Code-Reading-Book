/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64scrin.c,v 3.8 1996/12/23 06:39:26 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.
Copyright 1993,1994 by Kevin E. Martin, Chapel Hill, North Carolina.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the name of Sun not be used in
advertising or publicity pertaining to distribution  of  the
software  without specific prior written permission. Sun
makes no representations about the suitability of this
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

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD TO
THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)

********************************************************/
/* $XConsortium: mach64scrin.c /main/7 1996/02/21 17:29:09 kaleb $ */

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
#include "cfb32.h"
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "cfbmskbits.h"
#include "mibstore.h"
#include "mach64.h"
#include "xf86Priv.h"

extern RegionPtr mfbPixmapToRegion();
extern Bool cfbAllocatePrivates();
extern Bool cfb16AllocatePrivates();
extern Bool cfb32AllocatePrivates();
extern Bool cfbInitVisuals();
extern Bool miScreenInit();
extern Bool mfbRegisterCopyPlaneProc();
extern RegionPtr miCopyPlane();
extern void mach64BlockHandler();
extern Bool mach64DestroyPixmap();

extern int defaultColorVisualClass;
extern xrgb xf86weight;

static unsigned long cfbGeneration = 0;

miBSFuncRec mach64BSFuncRec = {
    mach64SaveAreas,
    mach64RestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};


/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
mach64ScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
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
    switch (mach64InfoRec.bitsPerPixel) {
    case 8:
	if (mach64DAC8Bit)
	    bitsPerRGB = 8;
	else
	    bitsPerRGB = 6;
	break;
    case 16:
	if (mach64WeightMask == RGB16_555)
	    bitsPerRGB = 5;
	else
	    bitsPerRGB = 6;
	break;
    case 32:
	bitsPerRGB = 8;
	break;
    default:
	FatalError("mach64ScreenInit: unsupported depth %d \n",
		    mach64InfoRec.bitsPerPixel);
    }
	
    if (cfbGeneration != serverGeneration) 
    {
	/* Only TrueColor for 16/32bpp */
	if (mach64InfoRec.bitsPerPixel > 8) 
	{
	    if (!cfbSetVisualTypes(mach64InfoRec.depth, 1 << TrueColor,
				   bitsPerRGB))
	    {
		return FALSE;
	    }
	}
	if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
	    &defaultVisual, 1<<(mach64InfoRec.bitsPerPixel - 1), bitsPerRGB))
	{
	    return FALSE;
	}
	cfbGeneration = serverGeneration;
    }

    if (mach64InfoRec.bitsPerPixel == 16 || mach64InfoRec.bitsPerPixel == 32) 
    {
        /*
         * There are several possible color weightings at 16bpp and 32bpp.
         * Set them up here.
         */
        for (i = 0, visual = visuals; i < nvisuals; i++, visual++) 
	{
	    if (visual->class == DirectColor || visual->class == TrueColor) 
	    {
		if (mach64InfoRec.bitsPerPixel == 16) {
	        	visual->offsetRed = xf86weight.green + xf86weight.blue;
	        	visual->offsetGreen = xf86weight.blue;
	        	visual->offsetBlue = 0;
		} else if (mach64InfoRec.bitsPerPixel == 32) {
		    switch (mach64RamdacSubType) {
		    case DAC_ATI68875:
		    case DAC_CH8398:
		    case DAC_STG1702:
		    case DAC_STG1703:
		    case DAC_ATT20C408:
			visual->offsetRed = 24;
			visual->offsetGreen = 16;
			visual->offsetBlue = 8;
			break;
		    case DAC_INTERNAL:
		    case DAC_IBMRGB514:
			visual->offsetRed = 16;
			visual->offsetGreen = 8;
			visual->offsetBlue = 0;
			break;
		    default:
			visual->offsetRed = 0;
			visual->offsetGreen = 8;
			visual->offsetBlue = 16;
			break;
		    }
		}
		visual->redMask = ((1 << xf86weight.red) - 1)
					<< visual->offsetRed;
		visual->greenMask = ((1 << xf86weight.green) - 1)
					<< visual->offsetGreen;
		visual->blueMask = ((1 << xf86weight.blue) - 1)
					<< visual->offsetBlue;
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
    pScreen->PaintWindowBackground = mach64PaintWindow;
    pScreen->PaintWindowBorder = mach64PaintWindow;
    pScreen->CopyWindow = mach64CopyWindow;
    pScreen->RealizeFont = mach64RealizeFont;
    pScreen->UnrealizeFont = mach64UnrealizeFont;
    pScreen->CreateGC = mach64CreateGC;
    switch (mach64InfoRec.bitsPerPixel) {
    case 8:
        if (!cfbAllocatePrivates(pScreen, &cfbWindowPrivateIndex,
            &cfbGCPrivateIndex))
            return FALSE;
        pScreen->GetImage = mach64GetImage;
        pScreen->GetSpans = cfbGetSpans;
	pScreen->CreateWindow = cfbCreateWindow;
	pScreen->DestroyWindow = cfbDestroyWindow;
	pScreen->PositionWindow = cfbPositionWindow;
	pScreen->ChangeWindowAttributes = cfbChangeWindowAttributes;
	pScreen->RealizeWindow = cfbMapWindow;
	pScreen->UnrealizeWindow = cfbUnmapWindow;
	pScreen->CreatePixmap = cfbCreatePixmap;
	pScreen->DestroyPixmap = mach64DestroyPixmap;
	/* InstallColormap, UninstallColormap, ListInstalledColormaps,
	 * StoreColors in mach64.c */
	mfbRegisterCopyPlaneProc (pScreen, cfbCopyPlane);
	break;
    case 16:
        if (!cfb16AllocatePrivates(pScreen, &cfbWindowPrivateIndex,
	    &cfbGCPrivateIndex))
	    return FALSE;
        pScreen->GetImage = mach64GetImage;
        pScreen->GetSpans = cfb16GetSpans;
	pScreen->CreateWindow = cfb16CreateWindow;
	pScreen->DestroyWindow = cfb16DestroyWindow;
	pScreen->PositionWindow = cfb16PositionWindow;
	pScreen->ChangeWindowAttributes = cfb16ChangeWindowAttributes;
	pScreen->RealizeWindow = cfb16MapWindow;
	pScreen->UnrealizeWindow = cfb16UnmapWindow;
	pScreen->CreatePixmap = cfb16CreatePixmap;
	pScreen->DestroyPixmap = mach64DestroyPixmap;
	/* InstallColormap, UninstallColormap, ListInstalledColormaps,
	 * StoreColors in mach64.c */
	mfbRegisterCopyPlaneProc (pScreen, cfb16CopyPlane);
	break;
    case 32:
        if (!cfb32AllocatePrivates(pScreen, &cfbWindowPrivateIndex,
	    &cfbGCPrivateIndex))
	    return FALSE;
        pScreen->GetImage = mach64GetImage;
        pScreen->GetSpans = cfb32GetSpans;
	pScreen->CreateWindow = cfb32CreateWindow;
	pScreen->DestroyWindow = cfb32DestroyWindow;
	pScreen->PositionWindow = cfb32PositionWindow;
	pScreen->ChangeWindowAttributes = cfb32ChangeWindowAttributes;
	pScreen->RealizeWindow = cfb32MapWindow;
	pScreen->UnrealizeWindow = cfb32UnmapWindow;
	pScreen->CreatePixmap = cfb32CreatePixmap;
	pScreen->DestroyPixmap = mach64DestroyPixmap;
	/* InstallColormap, UninstallColormap, ListInstalledColormaps,
	 * StoreColors in mach64.c */
	mfbRegisterCopyPlaneProc (pScreen, cfb32CopyPlane);
	break;
    default:
	FatalError("root depth %d not supported\n", mach64InfoRec.bitsPerPixel);
    }
    pScreen->CreateColormap = cfbInitializeColormap;
    pScreen->DestroyColormap = (DestroyColormapProcPtr)NoopDDA;
    pScreen->ResolveColor = cfbResolveColor;
    pScreen->BitmapToRegion = mfbPixmapToRegion;
    pScreen->BlockHandler = mach64BlockHandler; 

    if (rootdepth != 8) {
	oldDevPrivate = pScreen->devPrivate;
    }
    Rstatus = miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			&mach64BSFuncRec);

    if (rootdepth > 16) 
    {
        pScreen->CreateScreenResources = cfb32CreateScreenResources;
        pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr = pScreen->devPrivate;
        pScreen->devPrivate = oldDevPrivate;
    }
    else if (rootdepth > 8) 
    {
	pScreen->CreateScreenResources = cfb16CreateScreenResources;
	pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    return Rstatus;
}
