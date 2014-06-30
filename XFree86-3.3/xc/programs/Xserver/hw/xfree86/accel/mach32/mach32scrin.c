/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32scrin.c,v 3.10 1996/12/23 06:38:46 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.
Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.

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

********************************************************/
/* $XConsortium: mach32scrin.c /main/8 1996/02/21 17:27:25 kaleb $ */


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
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "cfbmskbits.h"
#include "mibstore.h"
#include "mach32.h"
#include "xf86Priv.h"

extern RegionPtr mfbPixmapToRegion();
extern Bool cfbAllocatePrivates();
extern Bool cfb16AllocatePrivates();
extern Bool cfbInitVisuals();
extern Bool miScreenInit();
extern Bool mfbRegisterCopyPlaneProc();

extern int defaultColorVisualClass;
extern xrgb xf86weight;

extern RegionPtr miCopyPlane();

#if 0
#define _RZ(d) (((d) + 2) / 3)
#define _RS(d) 0
#define _RM(d) ((1 << _RZ(d)) - 1)
#define _GZ(d) (((d) - _RZ(d) + 1) / 2)
#define _GS(d) _RZ(d)
#define _GM(d) (((1 << _GZ(d)) - 1) << _GS(d))
#define _BZ(d) ((d) - _RZ(d) - _GZ(d))
#define _BS(d) (_RZ(d) + _GZ(d))
#define _BM(d) (((1 << _BZ(d)) - 1) << _BS(d))
#define _CE(d) (1 << _RZ(d))

static VisualRec visuals[] = {
/* vid  class        bpRGB cmpE nplan rMask gMask bMask oRed oGreen oBlue */
    0,  PseudoColor, _BP,  1<<PSZ,   PSZ,  0,   0,   0,   0,   0,   0,
    0,  DirectColor, _BP, _CE,       PSZ,  _RM, _GM, _BM, _RS, _GS, _BS,
    0,  GrayScale,   _BP,  1<<PSZ,   PSZ,  0,   0,   0,   0,   0,   0,
    0,  StaticGray,  _BP,  1<<PSZ,   PSZ,  0,   0,   0,   0,   0,   0,
    0,  StaticColor, _BP,  1<<PSZ,   PSZ,  _RM, _GM, _BM, _RS, _GS, _BS,
    0,  TrueColor,   _BP, _CE,       PSZ,  _RM, _GM, _BM, _RS, _GS, _BS
};

#define	NUMVISUALS	((sizeof visuals)/(sizeof visuals[0]))

static  VisualID VIDs[NUMVISUALS];

static DepthRec depths[] = {
/* depth	numVid		vids */
    1,		0,		NULL,
    8,		NUMVISUALS,	VIDs
};

#define NUMDEPTHS	((sizeof depths)/(sizeof depths[0]))
#endif

static unsigned long cfbGeneration = 0;

miBSFuncRec mach32BSFuncRec = {
    mach32SaveAreas,
    mach32RestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
mach32ScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
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
    switch (mach32InfoRec.bitsPerPixel) {
    case 8:
	if (mach32DAC8Bit)
	    bitsPerRGB = 8;
	break;
    case 16:
	if (xf86weight.red == 5 && xf86weight.green == 5
	    && xf86weight.blue == 5)
	    bitsPerRGB = 5;
	break;
    }
	
    if (cfbGeneration != serverGeneration) {
	/* Only TrueColor for 16/32bpp */
	if (mach32InfoRec.bitsPerPixel > 8) {
	    if (!cfbSetVisualTypes(mach32InfoRec.depth, 1 << TrueColor,
				   bitsPerRGB))
		return FALSE;
	}
	if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
	    &defaultVisual, 1<<(mach32InfoRec.bitsPerPixel - 1), bitsPerRGB))
	    return FALSE;
	cfbGeneration = serverGeneration;
    }

    if (rootdepth == 15 || rootdepth == 16) {
    /*
     * There are several possible color weightings at 16bpp.
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
    pScreen->whitePixel = (Pixel) 1;
    pScreen->blackPixel = (Pixel) 0;
    XF86FLIP_PIXELS();
    pScreen->QueryBestSize = mfbQueryBestSize;
    /* SaveScreen */
    pScreen->GetImage = mach32GetImage;
    pScreen->GetSpans = mach32GetSpans;
    pScreen->PaintWindowBackground = mach32PaintWindow;
    pScreen->PaintWindowBorder = mach32PaintWindow;
    pScreen->CopyWindow = mach32CopyWindow;
    pScreen->RealizeFont = mach32RealizeFont;
    pScreen->UnrealizeFont = mach32UnrealizeFont;
    pScreen->CreateGC = mach32CreateGC;
    switch (rootdepth) {
    case 8:
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
	/* InstallColormap, UninstallColormap, ListInstalledColormaps,
	 * StoreColors in mach32.c */
	if (mach32Use4MbAperture) {
	    mfbRegisterCopyPlaneProc (pScreen, cfbCopyPlane);
	} else {
	    mfbRegisterCopyPlaneProc (pScreen, miCopyPlane);
	}
	break;
    case 15:
    case 16:
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
	/* InstallColormap, UninstallColormap, ListInstalledColormaps,
	 * StoreColors in mach32.c */
	if (mach32Use4MbAperture) {
	    mfbRegisterCopyPlaneProc (pScreen, cfb16CopyPlane);
	} else {
	    mfbRegisterCopyPlaneProc (pScreen, miCopyPlane);
	}
	break;
	default:
	    FatalError("root depth %d not supported\n", rootdepth);
    }
    pScreen->CreateColormap = cfbInitializeColormap;
    pScreen->DestroyColormap = (DestroyColormapProcPtr)NoopDDA;
    pScreen->ResolveColor = cfbResolveColor;
    pScreen->BitmapToRegion = mfbPixmapToRegion;
    pScreen->BlockHandler = mach32BlockHandler;

    if (rootdepth != 8) {
	oldDevPrivate = pScreen->devPrivate;
    }
    Rstatus = miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			&mach32BSFuncRec);
    if (rootdepth != 8) {
	pScreen->CreateScreenResources = cfb16CreateScreenResources;
	pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr = pScreen->devPrivate;
	pScreen->devPrivate = oldDevPrivate;
    }
    return Rstatus;

}
