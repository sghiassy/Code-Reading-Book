/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32cmap.c,v 3.10 1996/12/23 06:38:25 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 *
 * THOMAS ROELL, KEVIN E. MARTIN, AND RICKARD E. FAITH DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THOMAS
 * ROELL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 *
 * Header: /proj/X11/mit/server/ddx/xf86/vga/RCS/vgaCmap.c,v 1.2 1991/06/27 00:03:01 root Exp
 */
/* $XConsortium: mach32cmap.c /main/9 1996/10/27 11:46:38 kaleb $ */


#include "X.h"
#include "Xproto.h"
#include "compiler.h"

#include "mach32.h"

#ifdef XFreeXDGA
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif      

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];
				/* current colormap for each screen */

int
mach32ListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}

int
mach32GetInstalledColormaps(pScreen, pmap)
     ScreenPtr		pScreen;
     ColormapPtr	*pmap;
{
  *pmap = InstalledMaps[pScreen->myNum];
  return(1);
}

void
mach32StoreColors(pmap, ndef, pdefs)
     ColormapPtr	pmap;
     int		ndef;
     xColorItem	        *pdefs;
{
    int		i;
    xColorItem	directDefs[256];
    extern LUTENTRY mach32savedLUT[256];

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
	return;

    if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
	ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
	pdefs = directDefs;
    }

    for (i = 0; i < ndef; i++) {
	/* Return the n most significant bits from a 16-bit value.
	 * For VGA, n = 6.  For 8-bit DACs, n = 8.
	 */
	if (mach32DAC8Bit) {
	    mach32savedLUT[pdefs[i].pixel].r = pdefs[i].red >> 8;
	    mach32savedLUT[pdefs[i].pixel].g = pdefs[i].green >> 8;
	    mach32savedLUT[pdefs[i].pixel].b = pdefs[i].blue >> 8;
	} else {
	    mach32savedLUT[pdefs[i].pixel].r = pdefs[i].red >> 10;
	    mach32savedLUT[pdefs[i].pixel].g = pdefs[i].green >> 10;
	    mach32savedLUT[pdefs[i].pixel].b = pdefs[i].blue >> 10;
	}
	if (xf86VTSema
#ifdef XFreeXDGA
	    || ((mach32InfoRec.directMode & XF86DGADirectGraphics)
	        && !(mach32InfoRec.directMode & XF86DGADirectColormap))
	    || (mach32InfoRec.directMode & XF86DGAHasColormap)
#endif
	    ) {
	    outb(DAC_W_INDEX, pdefs[i].pixel);
	    if (mach32DAC8Bit) {
		outb(DAC_DATA, pdefs[i].red >> 8);
		outb(DAC_DATA, pdefs[i].green >> 8);
		outb(DAC_DATA, pdefs[i].blue >> 8);
	    } else {
		outb(DAC_DATA, pdefs[i].red >> 10);
		outb(DAC_DATA, pdefs[i].green >> 10);
		outb(DAC_DATA, pdefs[i].blue >> 10);
	    }
	}
    }
    checkCursorColor = TRUE;
}


void
mach32InstallColormap(pmap)
     ColormapPtr	pmap;
{
  ColormapPtr oldmap = InstalledMaps[pmap->pScreen->myNum];
  int         entries;
  Pixel *     ppix;
  xrgb *      prgb;
  xColorItem *defs;
  int         i;


  if (pmap == oldmap)
    return;

  if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    entries = (pmap->pVisual->redMask |
	       pmap->pVisual->greenMask |
	       pmap->pVisual->blueMask) + 1;
  else
    entries = pmap->pVisual->ColormapEntries;

  ppix = (Pixel *)ALLOCATE_LOCAL( entries * sizeof(Pixel));
  prgb = (xrgb *)ALLOCATE_LOCAL( entries * sizeof(xrgb));
  defs = (xColorItem *)ALLOCATE_LOCAL(entries * sizeof(xColorItem));

  if ( oldmap != NOMAPYET)
    WalkTree( pmap->pScreen, TellLostMap, &oldmap->mid);

  InstalledMaps[pmap->pScreen->myNum] = pmap;

  for ( i=0; i<entries; i++) ppix[i] = i;

  QueryColors( pmap, entries, ppix, prgb);

  for ( i=0; i<entries; i++) /* convert xrgbs to xColorItems */
    {
      defs[i].pixel = ppix[i];
      defs[i].red = prgb[i].red;
      defs[i].green = prgb[i].green;
      defs[i].blue = prgb[i].blue;
      defs[i].flags =  DoRed|DoGreen|DoBlue;
    }

  mach32StoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
  mach32RenewCursorColor(pmap->pScreen);
  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}


void
mach32UninstallColormap(pmap)
     ColormapPtr pmap;
{
  ColormapPtr defColormap;
  
  if ( pmap != InstalledMaps[pmap->pScreen->myNum] )
    return;

  defColormap = (ColormapPtr) LookupIDByType( pmap->pScreen->defColormap,
					      RT_COLORMAP);

  if (defColormap == InstalledMaps[pmap->pScreen->myNum])
    return;

  (*pmap->pScreen->InstallColormap) (defColormap);
}


/* This is for the screen saver */
void
mach32RestoreColor0(pScreen)
     ScreenPtr pScreen;
{
  Pixel       pix = 0;
  xrgb        rgb;

  if (InstalledMaps[pScreen->myNum] == NOMAPYET)
    return;

  QueryColors(InstalledMaps[pScreen->myNum], 1, &pix, &rgb);

  outb(DAC_W_INDEX, 0);
  if (mach32DAC8Bit) {
    outb(DAC_DATA, rgb.red >> 8);
    outb(DAC_DATA, rgb.green >> 8);
    outb(DAC_DATA, rgb.blue >> 8);
  } else {
    outb(DAC_DATA, rgb.red >> 10);
    outb(DAC_DATA, rgb.green >> 10);
    outb(DAC_DATA, rgb.blue >> 10);
  }
}
