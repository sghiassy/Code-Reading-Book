/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vgainit/vgabppscrin.c,v 3.6 1996/12/23 07:04:35 dawes Exp $ */
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
/* $XConsortium: vgabppscrin.c /main/4 1996/10/19 18:15:02 kaleb $ */

/*
 * This is the cfb16/24/32 ScreenInit function, modified for the XFree86
 * 8/16/24/32bpp SVGA server. The problem is that there doesn't seem to be
 * a safe way to convince the standard init routines to use the
 * reversed RGB order (blue is at lowest bit number) that is a
 * tradition in PC/VGA devices.
 *
 * This file is compiled tree times, with PSZ == 16, 24 and with PSZ == 32.
 *
 * The callable functions are vga16bppScreenInit,  vga24bppScreenInit 
 * and vga32bppScreenInit.
 *
 */


#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "resource.h"
#include "colormap.h"
#include "colormapst.h"
#include "cfb.h"
#include "mi.h"
#include "mistruct.h"
#include "dix.h"
#include "cfbmskbits.h"
#include "mibstore.h"

#include "gcstruct.h"
#include "xf86.h"
#include "xf86Priv.h"	/* for xf86weight */
#include "vga.h"

#if 1
#if PSZ == 16
#define vgabppScreenInit vga16bppScreenInit
#endif
#if PSZ == 24
#define vgabppScreenInit vga24bppScreenInit
#endif
#if PSZ == 32
#define vgabppScreenInit vga32bppScreenInit
#endif
#else
#ifdef VGA256
#define vgabppScreenInit xf86XAAScreenInitvga256
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
static miBSFuncRec vgaBSFuncRec = {
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
    VisualPtr   visual;
    int		BitsPerRGB;

    rootdepth = 0;
    BitsPerRGB = xf86weight.green;
#if PSZ > 8
    /* Only TrueColor for 16/32bpp */
    if (!cfbSetVisualTypes(vga256InfoRec.depth, 1 << TrueColor, BitsPerRGB))
	return FALSE;
#endif
    if (!cfbInitVisuals (&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
			 &defaultVisual,((unsigned long)1<<(PSZ-1)), BitsPerRGB))
	return FALSE;

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
    miInitializeBackingStore (pScreen, &vgaBSFuncRec);
#ifdef CFB_NEED_SCREEN_PRIVATE
    pScreen->CreateScreenResources = cfbCreateScreenResources;
    pScreen->devPrivates[cfbScreenPrivateIndex].ptr = pScreen->devPrivate;
    pScreen->devPrivate = oldDevPrivate;
#endif
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
    if (!vgaFinishScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width))
        return FALSE;
}
