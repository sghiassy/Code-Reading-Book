/* $XFree86: xc/programs/Xserver/iplan2p4/iplscrinit.c,v 3.0 1996/08/18 01:55:06 dawes Exp $ */
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
/* $XConsortium: iplscrinit.c,v 5.32 94/04/17 20:29:00 dpw Exp $ */

/* Modified nov 94 by Martin Schaller (Martin_Schaller@maus.r.de) for use with
interleaved planes */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormap.h"
#include "colormapst.h"
#include "ipl.h"
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "mibstore.h"


miBSFuncRec iplBSFuncRec = {
    iplSaveAreas,
    iplRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

Bool
iplCloseScreen (index, pScreen)
    int		index;
    ScreenPtr	pScreen;
{
    int	    d;
    DepthPtr	depths = pScreen->allowedDepths;

    for (d = 0; d < pScreen->numDepths; d++)
	xfree (depths[d].vids);
    xfree (depths);
    xfree (pScreen->visuals);
#ifdef CFB_NEED_SCREEN_PRIVATE
    xfree (pScreen->devPrivates[iplScreenPrivateIndex].ptr);
#else
    xfree (pScreen->devPrivate);
#endif
    return TRUE;
}

Bool
iplSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    int	i;
    extern RegionPtr	(*iplPuntCopyPlane)();

    if (!iplAllocatePrivates(pScreen, (int *) 0, (int *) 0))
	return FALSE;
    pScreen->defColormap = FakeClientID(0);
    /* let CreateDefColormap do whatever it wants for pixels */ 
    pScreen->blackPixel = pScreen->whitePixel = (Pixel) 0;
    pScreen->QueryBestSize = mfbQueryBestSize;
    /* SaveScreen */
    pScreen->GetImage = iplGetImage;
    pScreen->GetSpans = iplGetSpans;
    pScreen->CreateWindow = iplCreateWindow;
    pScreen->DestroyWindow = iplDestroyWindow;
    pScreen->PositionWindow = iplPositionWindow;
    pScreen->ChangeWindowAttributes = iplChangeWindowAttributes;
    pScreen->RealizeWindow = iplMapWindow;
    pScreen->UnrealizeWindow = iplUnmapWindow;
    pScreen->PaintWindowBackground = iplPaintWindow;
    pScreen->PaintWindowBorder = iplPaintWindow;
    pScreen->CopyWindow = iplCopyWindow;
    pScreen->CreatePixmap = iplCreatePixmap;
    pScreen->DestroyPixmap = iplDestroyPixmap;
    pScreen->RealizeFont = mfbRealizeFont;
    pScreen->UnrealizeFont = mfbUnrealizeFont;
    pScreen->CreateGC = iplCreateGC;
    pScreen->CreateColormap = iplInitializeColormap;
    pScreen->DestroyColormap = (void (*)())NoopDDA;
#ifdef	STATIC_COLOR
    pScreen->InstallColormap = iplInstallColormap;
    pScreen->UninstallColormap = iplUninstallColormap;
    pScreen->ListInstalledColormaps = iplListInstalledColormaps;
    pScreen->StoreColors = (void (*)())NoopDDA;
#endif
    pScreen->ResolveColor = iplResolveColor;
    pScreen->BitmapToRegion = mfbPixmapToRegion;

    mfbRegisterCopyPlaneProc (pScreen, iplCopyPlane);
    return TRUE;
}

#ifdef CFB_NEED_SCREEN_PRIVATE
Bool
iplCreateScreenResources(pScreen)
    ScreenPtr pScreen;
{
    Bool retval;

    pointer oldDevPrivate = pScreen->devPrivate;
    pScreen->devPrivate = pScreen->devPrivates[iplScreenPrivateIndex].ptr;
    retval = miCreateScreenResources(pScreen);
    pScreen->devPrivates[iplScreenPrivateIndex].ptr = pScreen->devPrivate;
    pScreen->devPrivate = oldDevPrivate;
    return retval;
}
#endif

iplFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    int	i, j;
#ifdef CFB_NEED_SCREEN_PRIVATE
    pointer oldDevPrivate;
#endif
    VisualPtr	visuals;
    DepthPtr	depths;
    int		nvisuals;
    int		ndepths;
    int		rootdepth;
    VisualID	defaultVisual;

    rootdepth = 0;
    if (!iplInitVisuals (&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
			 &defaultVisual,((unsigned long)1<<(INTER_PLANES-1)), 8))
	return FALSE;
#ifdef CFB_NEED_SCREEN_PRIVATE
    oldDevPrivate = pScreen->devPrivate;
#endif
    if (! miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			(miBSFuncPtr) 0))
	return FALSE;
    /* overwrite miCloseScreen with our own */
    pScreen->CloseScreen = iplCloseScreen;
    /* init backing store here so we can overwrite CloseScreen without stepping
     * on the backing store wrapped version */
    miInitializeBackingStore (pScreen, &iplBSFuncRec);
#ifdef CFB_NEED_SCREEN_PRIVATE
    pScreen->CreateScreenResources = iplCreateScreenResources;
    pScreen->devPrivates[iplScreenPrivateIndex].ptr = pScreen->devPrivate;
    pScreen->devPrivate = oldDevPrivate;
#endif
    return TRUE;
}

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
iplScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    if (!iplSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width))
	return FALSE;
    if (!iplFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width))
	return FALSE;
#if INTER_PLANES == 2
/* This shouldn't be necessary */
    PixmapWidthPaddingInfo[2].padPixelsLog2 = 4;
    PixmapWidthPaddingInfo[2].padRoundUp = 15;
    PixmapWidthPaddingInfo[2].padBytesLog2 = 2;
#endif
    return TRUE;
}
