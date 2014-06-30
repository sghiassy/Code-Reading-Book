/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8cmap.c,v 3.1 1996/12/23 06:39:47 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
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
 * THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO GONS DISCLAIM
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO GONS BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)
 *
 */
/* $XConsortium: mach8cmap.c /main/3 1996/02/21 17:29:56 kaleb $ */


#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"

#include "mach8.h"
#include "regmach8.h"

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];
				/* current colormap for each screen */

static LUTENTRY currentmach8dac[256];

int
mach8ListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}


void
mach8RestoreDACvalues()
{
    int       i;

    if (xf86VTSema) {
        outb(DAC_W_INDEX, 0);

        for (i = 0; i < 256; i++) {
          outb(DAC_DATA, currentmach8dac[i].r);
          outb(DAC_DATA, currentmach8dac[i].g);
          outb(DAC_DATA, currentmach8dac[i].b);
      }
    }
}


void
mach8StoreColors(pmap, ndef, pdefs)
     ColormapPtr	pmap;
     int		ndef;
     xColorItem	        *pdefs;
{
    int		i;
    xColorItem	directDefs[256];

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
	return;

    if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
	ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
	pdefs = directDefs;
    }

    for (i = 0; i < ndef; i++) {
        currentmach8dac[pdefs[i].pixel].r = pdefs[i].red >> 10;
        currentmach8dac[pdefs[i].pixel].g = pdefs[i].green >> 10;
        currentmach8dac[pdefs[i].pixel].b = pdefs[i].blue >> 10;
	if (xf86VTSema) {
	    outb(DAC_W_INDEX, pdefs[i].pixel);
	    outb(DAC_DATA, pdefs[i].red >> 10);
	    outb(DAC_DATA, pdefs[i].green >> 10);
	    outb(DAC_DATA, pdefs[i].blue >> 10);
	}
    }
}


void
mach8InstallColormap(pmap)
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

  mach8StoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
  
  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}


void
mach8UninstallColormap(pmap)
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
mach8RestoreColor0(pScreen)
     ScreenPtr pScreen;
{
  Pixel       pix = 0;
  xrgb        rgb;

  if (InstalledMaps[pScreen->myNum] == NOMAPYET)
    return;

  QueryColors(InstalledMaps[pScreen->myNum], 1, &pix, &rgb);

  outb(DAC_W_INDEX, 0);
  outb(DAC_DATA, rgb.red >> 10);
  outb(DAC_DATA, rgb.green >> 10);
  outb(DAC_DATA, rgb.blue >> 10);
}
