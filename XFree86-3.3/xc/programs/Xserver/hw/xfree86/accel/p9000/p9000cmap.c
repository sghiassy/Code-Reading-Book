/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000cmap.c,v 3.11.2.1 1997/05/10 07:02:53 hohndel Exp $ */
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
 * ERIK NYGREN, THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO
 * GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK
 * NYGREN, THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, OR TIAGO GONS BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Tiago Gons (tiago@comosjn.hobby.nl)
 * Modified for the P9000 by Erik Nygren (nygren@mit.edu)
 *
 */
/* $XConsortium: p9000cmap.c /main/9 1996/10/27 11:46:45 kaleb $ */

/* Note that the outb's and inb's in here should be changed to use the
 * p9000OutBtReg, etc routines.  It's not needed yet because
 * everything in here is in the 03c[6789] range. *TO*DO*
 */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"
#include "cfb.h"

#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"

#ifdef XFreeXDGA
#include "extnsionst.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];
				/* current colormap for each screen */

static LUTENTRY currentp9000dac[256];

extern ScrnInfoRec p9000InfoRec;

int
p9000ListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}

/*  p9000InitLUT()
 *  Loads the Look-Up Table with all black. 
 *  Assumes 8-bit board is in use.
 */
void p9000InitLUT(void)
{
    short i;

    outb(BT_WRITE_ADDR, 0);

    /* Load the first 16 LUT entries */
    for (i = 0; i < 256; i++)
      {
	outb(BT_RAMDAC_DATA, 0);
	outb(BT_RAMDAC_DATA, 0);
	outb(BT_RAMDAC_DATA, 0);
      }
    outb(BT_PIXEL_MASK,0xff);
}

/* p9000WriteLUT
 * Writes out a color lookup table to the RAMDAC
 */
void p9000WriteLUT(LUTENTRY rgbtable[256])
{
  int i;

  outb(BT_WRITE_ADDR, 0);  
  for (i = 0; i < 256; i++) {
    outb(BT_RAMDAC_DATA, rgbtable[i].r);
    outb(BT_RAMDAC_DATA, rgbtable[i].g);
    outb(BT_RAMDAC_DATA, rgbtable[i].b);
  }
}

/* p9000ReadLUT
 * Reads the current color lookup table from the RAMDAC
 */
void p9000ReadLUT(LUTENTRY rgbtable[256])
{
  int i;

  outb(BT_READ_ADDR, 0);  
  for (i = 0; i < 256; i++) {
    rgbtable[i].r = inb(BT_RAMDAC_DATA);
    rgbtable[i].g = inb(BT_RAMDAC_DATA);
    rgbtable[i].b = inb(BT_RAMDAC_DATA);
  }
}

void
p9000RestoreDACvalues()
{
  if (xf86VTSema)
    {
      p9000WriteLUT(currentp9000dac);
    }
}

void
p9000StoreColors(pmap, ndef, pdefs)
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
        currentp9000dac[pdefs[i].pixel].r = pdefs[i].red >> 8;
        currentp9000dac[pdefs[i].pixel].g = pdefs[i].green >> 8;
        currentp9000dac[pdefs[i].pixel].b = pdefs[i].blue >> 8;
	if (xf86VTSema
#ifdef XFreeXDGA
	    || ((p9000InfoRec.directMode & XF86DGADirectGraphics)
	        && !(p9000InfoRec.directMode & XF86DGADirectColormap))
	    || (p9000InfoRec.directMode & XF86DGAHasColormap)
#endif
	    ) {
	    outb(BT_WRITE_ADDR, pdefs[i].pixel);
	    outb(BT_RAMDAC_DATA, pdefs[i].red >> 8);
	    outb(BT_RAMDAC_DATA, pdefs[i].green >> 8);
	    outb(BT_RAMDAC_DATA, pdefs[i].blue >> 8);
	}
    }

}


void
p9000InstallColormap(pmap)
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

  p9000StoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
  
  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}


void
p9000UninstallColormap(pmap)
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
