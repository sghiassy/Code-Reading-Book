/* $XConsortium: svgaFb.c,v 1.4 93/10/12 11:26:00 dpw Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "svga.h"
#include "cfb.h"
#include "mi.h"
#include "mibstore.h"
#include "mipointer.h"
#include "misprite.h"
#include "colormapst.h"
#include "resource.h"
#include "pixmapstr.h"
#include "rgb.h"
#include "windowstr.h"
#ifdef MITSHM
 extern void ShmRegisterFbFuncs();
#endif

extern miBSFuncRec svgaBankBSFuncRec;

static Bool ScreenIsSaved = FALSE;
static ColormapPtr pInstalledMap;
static RGB clut[1<<PSZ];


/* ARGSUSED */
static Bool
svgaSVGACursorOffScreen(
    ScreenPtr *pScreen,
    int       *x,
    int       *y
)
{
  return FALSE;
}

/* ARGSUSED */
static void
svgaSVGACrossScreen(
    ScreenPtr pScreen,
    Bool      entering
)
{
}

static void
svgaSVGAWrapCursor(
    ScreenPtr pScreen,
    int       x,
    int       y
)
{
  miPointerWarpCursor(pScreen, x, y);
}

static miPointerScreenFuncRec svgaScreenFuncsRec = {
  svgaSVGACursorOffScreen,
  svgaSVGACrossScreen,
  svgaSVGAWrapCursor,
};

Bool
svgaSVGABlankScreen(
    ScreenPtr pScreen,
    int       on
)
{
  int i;

  if (!svgaVTActive) return TRUE;

  switch(on) {

  case SCREEN_SAVER_FORCER:
    break;

  case SCREEN_SAVER_OFF:
    ScreenIsSaved = FALSE;

    for (i = 0; i < (1<<PSZ); i++)
      svgaSVPMISetColor(pScreen, i, clut[i].red, clut[i].green, clut[i].blue);
    break;

  case SCREEN_SAVER_ON:
    ScreenIsSaved = TRUE;

    for (i = 0; i < (1<<PSZ); i++)
      svgaSVPMISetColor(pScreen, i, 0, 0, 0);
    break;
  }

  return TRUE;
}

static void
svgaSVGAStoreColors(
    ColormapPtr pmap,
    int         ndef,
    xColorItem  *pdefs
)
{
  int         idef, shift, ndx;
  xColorItem  directDefs[1<<PSZ];

  if (pmap != pInstalledMap)
    return;

  if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    {
      ndef = cfbExpandDirectColors(pmap, ndef, pdefs, directDefs);
      pdefs = directDefs;
    }

  shift = 16 - pmap->pVisual->bitsPerRGBValue;

  for(idef = 0; idef < ndef; idef++) {
    ndx = pdefs[idef].pixel;
    clut[ndx].red = pdefs[idef].red >> shift;
    clut[ndx].green = pdefs[idef].green >> shift;
    clut[ndx].blue = pdefs[idef].blue >> shift;

    if (svgaVTActive && !ScreenIsSaved)
      svgaSVPMISetColor(pmap->pScreen,
			ndx, clut[ndx].red, clut[ndx].green, clut[ndx].blue);
  }
}

static void
svgaSVGAInstallColormap(
    ColormapPtr pmap
)
{
  int         entries;
  Pixel *     ppix;
  xrgb *      prgb;
  xColorItem *defs;
  int         i;

  if (pmap == pInstalledMap)
    return;

  if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    entries = (pmap->pVisual->redMask |
	       pmap->pVisual->greenMask |
	       pmap->pVisual->blueMask) + 1;
  else
    entries = pmap->pVisual->ColormapEntries;

  ppix = (Pixel*)ALLOCATE_LOCAL(entries * sizeof(Pixel));
  prgb = (xrgb*)ALLOCATE_LOCAL(entries * sizeof(xrgb));
  defs = (xColorItem*)ALLOCATE_LOCAL(entries * sizeof(xColorItem));

  if (pInstalledMap != NULL)
    WalkTree( pmap->pScreen, TellLostMap, &pInstalledMap->mid);
  pInstalledMap = pmap;

  for (i = 0; i < entries; i++ ) ppix[i] = i;
  QueryColors( pmap, entries, ppix, prgb);

  for (i = 0; i < entries; i++)
    {
      defs[i].pixel = ppix[i];
      defs[i].red = prgb[i].red;
      defs[i].green = prgb[i].green;
      defs[i].blue = prgb[i].blue;
      defs[i].flags =  DoRed|DoGreen|DoBlue;
    }
  svgaSVGAStoreColors(pmap, entries, defs);
  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);

  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}

static void
svgaSVGAUninstallColormap(
    ColormapPtr pmap
)
{
  if (pmap == pInstalledMap) {
    pmap = (ColormapPtr)LookupIDByType(pmap->pScreen->defColormap,
				       RT_COLORMAP);
    (*pmap->pScreen->InstallColormap)(pmap);
  }    
}

/* ARGSUSED */
static int
svgaSVGAListInstalledColormaps(
    ScreenPtr pScreen,
    Colormap  *pmaps
)
{
  *pmaps = pInstalledMap->mid;

  return 1;
}

static Bool
svgaSVGACreateScreenResources(
    ScreenPtr pScreen
)
{
  PixmapPtr pPixmap;
  ulong     bytesPerScanLine = (ulong)pScreen->devPrivate;

  pPixmap = (*pScreen->CreatePixmap)(pScreen, 0, 0, pScreen->rootDepth);
  if (!pPixmap)
    return FALSE;

  if (!(*pScreen->ModifyPixmapHeader)(pPixmap,
				      pScreen->width, pScreen->height,
				      pScreen->rootDepth, pScreen->rootDepth,
				      bytesPerScanLine,
				      pScreen->devPrivate))
    return FALSE;

  pPixmap->drawable.class = 1; /* ### */
  pScreen->devPrivate = (pointer)pPixmap;

  return TRUE;
}

/* ARGSUSED */
static Bool
svgaSVGACloseScreen(
    int         index,
    ScreenPtr   pScreen
)
{
  int      d;
  DepthPtr depths = pScreen->allowedDepths;

  for (d = 0; d < pScreen->numDepths; d++) Xfree(depths[d].vids);
  Xfree(depths);
  Xfree(pScreen->visuals);

  svgaSVPMISetText(pScreen);

  return TRUE;
}

/* ARGSUSED */
Bool
svgaSVGAScreenInit(
    int        index,
    ScreenPtr  pScreen,
    int        argc,
    char       **argv
)
{
  ulong       winAAttributes, winBAttributes, winABase, winBBase;
  int         winGranularity, winSize, bytesPerScanLine, bitsRGB;
  int         XResolution, YResolution;
  VisualPtr   visuals, pVisual;
  DepthPtr    depths;
  int         nvisuals, ndepths, rootdepth;
  VisualID    defaultVisual;
  ColormapPtr pCmap;
  ushort      zeros = 0x0000;
  ushort      ones  = 0xffff; 

  if (!svgaSVPMIOpen(pScreen, argc, argv,
		     &winAAttributes, &winBAttributes,
		     &winGranularity,
		     &winSize,
		     &winABase, &winBBase,
		     &bytesPerScanLine,
		     &XResolution,
		     &YResolution,
		     &bitsRGB))
    return FALSE;

  if (!cfbSetupScreen(pScreen,
		      NULL,
		      XResolution, YResolution, 
		      75, 75,
		      bytesPerScanLine))
    return FALSE;

  rootdepth = 0;

  if (!cfbInitVisuals(&visuals, &depths, &nvisuals, &ndepths, &rootdepth,
		       &defaultVisual, 1<<(PSZ-1), bitsRGB))
        return FALSE;

  pScreen->SaveScreen = svgaSVGABlankScreen;
  pScreen->InstallColormap = svgaSVGAInstallColormap;
  pScreen->UninstallColormap = svgaSVGAUninstallColormap;
  pScreen->ListInstalledColormaps = svgaSVGAListInstalledColormaps;
  pScreen->StoreColors = svgaSVGAStoreColors;

  if (!miScreenInit(pScreen, NULL,
		    XResolution, YResolution,
		    75, 75,
		    bytesPerScanLine,
		    rootdepth, ndepths, depths,
		    defaultVisual, nvisuals, visuals,
		    NULL))
    return FALSE;

  pScreen->CloseScreen = svgaSVGACloseScreen;
  pScreen->CreateScreenResources = svgaSVGACreateScreenResources;

  svgaSVPMISetGraphics(pScreen);

  if (!svgaBankScreenInit(pScreen,
			  winAAttributes, winBAttributes,
			  winGranularity,
			  winSize,
			  winABase, winBBase,
			  bytesPerScanLine,
			  XResolution, YResolution,
			  svgaSVPMISetWindow))
    return FALSE;

#ifdef MITSHM
  ShmRegisterFbFuncs(pScreen);
#endif

  miInitializeBackingStore(pScreen, &svgaBankBSFuncRec);

  if (!miDCInitialize(pScreen, &svgaScreenFuncsRec))
    return FALSE;

  pScreen->whitePixel = 1;
  pScreen->blackPixel = 0;

  for (pVisual = pScreen->visuals;
       pVisual->vid != pScreen->rootVisual;
       pVisual++)
    ;

  if (CreateColormap(pScreen->defColormap, pScreen, pVisual, &pCmap,
		     (pVisual->class & DynamicClass) ? AllocNone : AllocAll,
		     0)
      != Success)

    return FALSE;

  if ((AllocColor(pCmap, &ones, &ones, &ones,
		  &(pScreen->whitePixel), 0) != Success) ||
      (AllocColor(pCmap, &zeros, &zeros, &zeros,
		  &(pScreen->blackPixel), 0) != Success))
    
      return FALSE;

  (*pScreen->InstallColormap)(pCmap);

  pScreen->devPrivate = (pointer)bytesPerScanLine;

  return TRUE;
}
