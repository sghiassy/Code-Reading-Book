/* $XConsortium: vgabppscrin.c,v 1.2 95/06/19 19:33:39 kaleb Exp $ */
/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/xf86scrin.c,v 3.8.2.1 1997/05/21 15:02:56 dawes Exp $ */
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

/*
 * This is the screeninit function for the XAA code, derived from the
 * cfb ScreenInit function.
 *
 * In addition to initializing the acceleration interface, it also has
 * to take into consideration the fact that in truecolor modes (>=
 * 16bpp) we have the reversed RGB order (blue is at lowest bit number)
 * that is a tradition in PC/VGA devices.
 *
 * This file is compiled four times, with VGA256 defined (PSZ == 8),
 * PSZ == 16, 24 and with PSZ == 32. It could also be compiled with
 * PSZ == 8 but without VGA256, for 8bpp support on a linear framebuffer
 * that is not dependent on vga256
 *
 */


#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormap.h"
#include "colormapst.h"
#ifdef VGA256
/*
 * VGA256 is a special case. We don't use cfb for non-accelerated
 * functions, but instead use their vga256 equivalents.
 */
#include "vga256.h"
#include "vga256map.h"
#else
#include "cfb.h"
#include "cfbmskbits.h"
#endif
#include "mi.h"
#include "mistruct.h"
#include "regionstr.h"
#include "dix.h"
#include "mibstore.h"

#include "gcstruct.h"
#include "xf86.h"
#include "xf86Priv.h"	/* for xf86weight */
#include "vga.h"

#include "xf86xaa.h"
#include "xf86gcmap.h"
#include "xf86maploc.h"
#include "xf86local.h"
#include "xf86pcache.h"

#ifdef VGA256
#define vgabppScreenInit xf86XAAScreenInitvga256
#else
#if PSZ == 8
#define vgabppScreenInit xf86XAAScreenInit8bpp
#endif
#if PSZ == 16
#define vgabppScreenInit xf86XAAScreenInit16bpp
#endif
#if PSZ == 24
#define vgabppScreenInit xf86XAAScreenInit24bpp
#endif
#if PSZ == 32
#define vgabppScreenInit xf86XAAScreenInit32bpp
#endif
#endif

/* This is defined in cfbscrinit.c */
extern Bool cfbCreateScreenResources();

/* We need to define this here instead of use the cfb one. */
static miBSFuncRec xf86BSFuncRec = {
    cfbSaveAreas,
    cfbRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

static vgaFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
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
    VisualPtr	visual;
    int		BitsPerRGB;

    rootdepth = 0;

    BitsPerRGB = xf86weight.green;
#if PSZ > 8
    /* Only TrueColor for 16/32bpp */
    if (!cfbSetVisualTypes(xf86AccelInfoRec.ServerInfoRec->depth,
    1 << TrueColor, BitsPerRGB))
	return FALSE;
#endif
    if (!cfbInitVisuals (&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
			 &defaultVisual,((unsigned long)1<<(PSZ-1)), BitsPerRGB))
	return FALSE;

#if PSZ > 8
    /*
     * Now correct the RGB order of direct/truecolor visuals for the
     * 16/24/32bpp SVGA server.
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

    pScreen->defColormap = FakeClientID(0);
#endif

    /*
     * The only reason to have these lines here is that this they
     * depend on PSZ, and xf86initacl.c is compiled only once.
     * They must be set before xf86InitializeAcceleration.
     */
    if (!xf86AccelInfoRec.ImageWriteFallBack) {
        xf86AccelInfoRec.ImageWriteFallBack = xf86ImageWriteFallBack;
    }
    if (!xf86AccelInfoRec.ImageWrite) {
        xf86AccelInfoRec.ImageWrite = xf86ImageWriteFallBack;
    }
#if PSZ != 24
    if (!xf86AccelInfoRec.WriteBitmapFallBack)
        xf86AccelInfoRec.WriteBitmapFallBack = xf86WriteBitmapFallBack;
#endif
#ifdef VGA256
    /*
     * vga256 passes the "virtual" address for the banking logic
     * in pbits. Use the real mapped address instead.
     */
    if (vgaLinearBase)
        xf86AccelInfoRec.FramebufferBase = vgaLinearBase;
    else
        xf86AccelInfoRec.FramebufferBase = vgaBase;
#else
    xf86AccelInfoRec.FramebufferBase = pbits;
#endif
    /* This may not be the right way to get the number of bits of pixel. */
    xf86AccelInfoRec.BitsPerPixel = PSZ;
    xf86AccelInfoRec.FramebufferWidth = width;
    /* It might be worthwhile to only define 24 bits for 32bpp. */
    xf86AccelInfoRec.FullPlanemask = PMSK;

    xf86InitializeAcceleration(pScreen);

    if (serverGeneration == 1 && OFLG_ISSET(OPTION_XAA_BENCHMARK,
    &(xf86AccelInfoRec.ServerInfoRec->options)))
        xf86Bench();

    /* This is important. */
    pScreen->CreateGC = xf86CreateGC;

#ifdef CFB_NEED_SCREEN_PRIVATE
    oldDevPrivate = pScreen->devPrivate;
#endif
    if (! miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			rootdepth, ndepths, depths,
			defaultVisual, nvisuals, visuals,
			(miBSFuncPtr) 0))
	return FALSE;
    /* overwrite miCloseScreen with our own */
    pScreen->CloseScreen = cfbCloseScreen;
    /* init backing store here so we can overwrite CloseScreen without stepping
     * on the backing store wrapped version */
    miInitializeBackingStore (pScreen, &xf86BSFuncRec);
#ifdef CFB_NEED_SCREEN_PRIVATE
    pScreen->CreateScreenResources = cfbCreateScreenResources;
    pScreen->devPrivates[cfbScreenPrivateIndex].ptr = pScreen->devPrivate;
    pScreen->devPrivate = oldDevPrivate;
#endif

#ifdef PIXPRIV
    xf86PixmapIndex = AllocatePixmapPrivateIndex();
    if (!AllocatePixmapPrivate(pScreen, xf86PixmapIndex,
			       sizeof(xf86PixPrivRec)))
	return FALSE;
#endif

    xf86GCInfoRec.OffScreenCopyWindowFallBack = cfbCopyWindow;
    xf86GCInfoRec.CopyAreaFallBack = cfbCopyArea;
    xf86GCInfoRec.PolyFillRectSolidFallBack = cfbPolyFillRect;
    /*
     * Even though the BitBlt helper function is almost identical
     * for cfb8/16/24/32, the right one must be used.
     */
    xf86GCInfoRec.cfbBitBltDispatch = cfbBitBlt;
#ifdef VGA256
    xf86AccelInfoRec.VerticalLineGXcopyFallBack = fastvga256VertS;
    xf86AccelInfoRec.BresenhamLineFallBack = fastvga256BresS;
    xf86AccelInfoRec.UsingVGA256 = TRUE;
#else
    xf86AccelInfoRec.VerticalLineGXcopyFallBack = cfbVertS;
    xf86AccelInfoRec.BresenhamLineFallBack = cfbBresS;
    xf86AccelInfoRec.UsingVGA256 = FALSE;
#endif
    xf86GCInfoRec.CopyPlane1toNFallBack = xf86CopyPlane1toN;
    xf86GCInfoRec.ImageGlyphBltFallBack = xf86ImageGlyphBltFallBack;
    xf86GCInfoRec.PolyGlyphBltFallBack = xf86PolyGlyphBltFallBack;
    xf86GCInfoRec.FillSpansFallBack = xf86FillSpansFallBack;
    xf86AccelInfoRec.FillRectTiledFallBack = xf86FillRectTileFallBack;
    xf86AccelInfoRec.FillRectStippledFallBack = xf86FillRectStippledFallBack;
    xf86AccelInfoRec.FillRectOpaqueStippledFallBack =
        xf86FillRectOpaqueStippledFallBack;
    xf86AccelInfoRec.xf86GetLongWidthAndPointer = xf86cfbGetLongWidthAndPointer;

    return TRUE;
}

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
vgabppScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
    register ScreenPtr pScreen;
    pointer pbits;		/* pointer to screen bitmap */
    int xsize, ysize;		/* in pixels */
    int dpix, dpiy;		/* dots per inch */
    int width;			/* pixel width of frame buffer */
{
    if (!cfbSetupScreen(pScreen, pbits, xsize, ysize, dpix, dpiy, width))
	return FALSE;

#ifdef VGA256
    /* These override what was being set in cfbSetupScreen() */
    pScreen->GetImage = vga256GetImage;
    pScreen->GetSpans = vga256GetSpans;
    /*
     * It may be better to do PaintWindow via mi so that new-style
     * acceleration is used. This happens in xf86InitializeAcceleration().
     */
    pScreen->PaintWindowBackground = vga256PaintWindow;
    pScreen->PaintWindowBorder = vga256PaintWindow;
    pScreen->CopyWindow = vga256CopyWindow;
    pScreen->CreateGC = vga256CreateGC;

    mfbRegisterCopyPlaneProc (pScreen, vga256CopyPlane);
#endif

    return vgaFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy,
        width);
}
