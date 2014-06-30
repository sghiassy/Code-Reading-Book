/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000scrin.c,v 3.12 1996/12/23 06:40:53 dawes Exp $ */
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

KEVIN E. MARTIN, ERIK NYGREN, AND RICKARD E. FAITH DISCLAIM ALL
WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK
NYGREN OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
Modified for the P9000 by Erik Nygren (nygren@mit.edu)

********************************************************/
/* $XConsortium: p9000scrin.c /main/7 1996/02/21 17:33:07 kaleb $ */

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
#include "p9000.h"
#include "p9000reg.h"
#include "xf86Priv.h"

extern RegionPtr mfbPixmapToRegion();
extern Bool cfbAllocatePrivates();
extern Bool cfb16AllocatePrivates();
extern Bool cfb32AllocatePrivates();
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

miBSFuncRec p9000BSFuncRec;

miBSFuncRec p9000BSFuncRec8 = {
    cfbSaveAreas,
    cfbRestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

miBSFuncRec p9000BSFuncRec16 = {
    cfb16SaveAreas,
    cfb16RestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

miBSFuncRec p9000BSFuncRec32 = {
    cfb32SaveAreas,
    cfb32RestoreAreas,
    (void (*)()) 0,
    (PixmapPtr (*)()) 0,
    (PixmapPtr (*)()) 0,
};

/* dts * (inch/dot) * (25.4 mm / inch) = mm */
Bool
p9000ScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width)
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
  switch (p9000InfoRec.bitsPerPixel) 
    {
    case 8:
      if (p9000DAC8Bit)
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
	
    if (cfbGeneration != serverGeneration) 
      {
	/* Only TrueColor for 16/32bpp */
	if (p9000InfoRec.bitsPerPixel > 8) {
	    if (!cfbSetVisualTypes(p9000InfoRec.depth, 1 << TrueColor,
				   bitsPerRGB))
		return FALSE;
	}
	if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
			    &defaultVisual,
			    1<<(p9000InfoRec.bitsPerPixel - 1), bitsPerRGB))
	  return FALSE;
	cfbGeneration = serverGeneration;
      }

  if (rootdepth == 15 || rootdepth == 16 || rootdepth == 24)
    {
      /*
       * There are several possible color weightings at 16bpp an 32bpp.
       * Set them up here.
       */
      for (i = 0, visual = visuals; i < nvisuals; i++, visual++)
	if (visual->class == DirectColor || visual->class == TrueColor) 
	  {
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
  pScreen->RealizeFont = mfbRealizeFont;
  pScreen->UnrealizeFont = mfbUnrealizeFont;
  switch (rootdepth)
    {
    case 8:
      p9000BSFuncRec = p9000BSFuncRec8;
      pScreen->GetImage = cfbGetImage;
      pScreen->GetSpans = cfbGetSpans;
#ifdef P9000_ACCEL
      if (!p9000NoAccel)
	{
	  /***** ACCELERATED *****/
	  pScreen->CopyWindow = p9000CopyWindow;
	  pScreen->CreateGC = p9000CreateGC;
          pScreen->PaintWindowBackground = p9000PaintWindow;
          pScreen->PaintWindowBorder = p9000PaintWindow;
	}
      else
	{
#endif
	  /***** FRAME BUFFER (NOT ACCEL)  *****/
	  pScreen->CopyWindow = cfbCopyWindow;
	  pScreen->CreateGC = cfbCreateGC;
          pScreen->PaintWindowBackground = cfbPaintWindow;
          pScreen->PaintWindowBorder = cfbPaintWindow;
#ifdef P9000_ACCEL
	}
#endif
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
       * StoreColors in p9000.c */
      mfbRegisterCopyPlaneProc (pScreen, cfbCopyPlane);
      break;
    case 15:
    case 16:
      p9000BSFuncRec = p9000BSFuncRec16;
      pScreen->GetImage = cfb16GetImage;
      pScreen->GetSpans = cfb16GetSpans;
#ifdef P9000_ACCEL
      if (!p9000NoAccel)
	{
	  /***** ACCELERATED *****/
	  pScreen->CopyWindow = p9000CopyWindow;
	  pScreen->CreateGC = p9000CreateGC16;
          pScreen->PaintWindowBackground = p900016PaintWindow;
          pScreen->PaintWindowBorder = p900016PaintWindow;
	}
      else
	{
#endif
	  /***** FRAME BUFFER (NOT ACCEL)  *****/
	  pScreen->CopyWindow = cfb16CopyWindow;
          pScreen->CreateGC = cfb16CreateGC;
          pScreen->PaintWindowBackground = cfb16PaintWindow;
          pScreen->PaintWindowBorder = cfb16PaintWindow;
#ifdef P9000_ACCEL
	}
#endif
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
       * StoreColors in p9000.c */
	mfbRegisterCopyPlaneProc (pScreen, cfb16CopyPlane);
      break;
    case 24:
      p9000BSFuncRec = p9000BSFuncRec32;
      pScreen->GetImage = cfb32GetImage;
      pScreen->GetSpans = cfb32GetSpans;
#ifdef P9000_ACCEL
      if (!p9000NoAccel)
	{
	  /***** ACCELERATED *****/
	  pScreen->CopyWindow = p9000CopyWindow;
	  pScreen->CreateGC = p9000CreateGC32;
          pScreen->PaintWindowBackground = cfb32PaintWindow;
          pScreen->PaintWindowBorder = cfb32PaintWindow;
	}
      else
	{
#endif
	  /***** FRAME BUFFER (NOT ACCEL)  *****/
	  pScreen->CopyWindow = cfb32CopyWindow;
          pScreen->CreateGC = cfb32CreateGC;
          pScreen->PaintWindowBackground = cfb32PaintWindow;
          pScreen->PaintWindowBorder = cfb32PaintWindow;
#ifdef P9000_ACCEL
	}
#endif
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
      /* InstallColormap, UninstallColormap, ListInstalledColormaps,
       * StoreColors in p9000.c */
	mfbRegisterCopyPlaneProc (pScreen, cfb32CopyPlane);
      break;
    default:
      FatalError("root depth %d not supported\n", rootdepth);
    }
  pScreen->CreateColormap = cfbInitializeColormap;
  pScreen->DestroyColormap = (DestroyColormapProcPtr)NoopDDA;
  pScreen->ResolveColor = cfbResolveColor;
  pScreen->BitmapToRegion = mfbPixmapToRegion;
#if 0  /* What's this for?!  *TO*DO* */
  pScreen->BlockHandler = p9000BlockHandler;
#endif

  if (rootdepth != 8)
    {
      oldDevPrivate = pScreen->devPrivate;
    }
  Rstatus = miScreenInit(pScreen, pbits, xsize, ysize, dpix, dpiy, width,
			 rootdepth, ndepths, depths, defaultVisual,
			 nvisuals, visuals, &p9000BSFuncRec);
  switch (rootdepth)
    {
    case 15:
    case 16:
      pScreen->CreateScreenResources = cfb16CreateScreenResources;
      pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr = pScreen->devPrivate;
      pScreen->devPrivate = oldDevPrivate;
      break;
    case 24:
      pScreen->CreateScreenResources = cfb32CreateScreenResources;
      pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr = pScreen->devPrivate;
      pScreen->devPrivate = oldDevPrivate;
      break;
    }

    return Rstatus;
}
