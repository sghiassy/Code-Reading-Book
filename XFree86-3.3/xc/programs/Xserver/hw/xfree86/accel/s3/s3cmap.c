/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3cmap.c,v 3.17 1996/12/23 06:41:39 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * THOMAS ROELL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THOMAS ROELL OR KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * 
 */

/*
 * Modified by Amancio Hasty and Jon Tombs
 */
/* $XConsortium: s3cmap.c /main/15 1996/10/28 05:12:13 kaleb $ */


#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"
#include "cfb.h"

#include "s3.h"
#include "regs3.h"

#ifdef XFreeXDGA
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

extern unsigned char xf86rGammaMap[], xf86gGammaMap[], xf86bGammaMap[];

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];

/* current colormap for each screen */

LUTENTRY currents3dac[256];
extern int currents3dac_border;

int
s3ListInstalledColormaps(pScreen, pmaps)
     ScreenPtr pScreen;
     Colormap *pmaps;
{

 /*
  * By the time we are processing requests, we can guarantee that there is
  * always a colormap installed
  */

   *pmaps = InstalledMaps[pScreen->myNum]->mid;
   return (1);
}

void
s3RestoreDACvalues()
{
   int i;

   if (xf86VTSema) {
      BLOCK_CURSOR;
      outb(DAC_W_INDEX, 0);

      for (i=0; i < 256; i++) {
	 outb(DAC_DATA, currents3dac[i].r);
	 outb(DAC_DATA, currents3dac[i].g);
	 outb(DAC_DATA, currents3dac[i].b);
      }
      UNBLOCK_CURSOR;
   }
}

int
s3GetInstalledColormaps(pScreen, pmap)
     ScreenPtr        pScreen;
     ColormapPtr      *pmap;
{
  *pmap = InstalledMaps[pScreen->myNum];
  return(1);
}


void
s3StoreColors(pmap, ndef, pdefs)
     ColormapPtr pmap;
     int   ndef;
     xColorItem *pdefs;
{
   int   i;
   xColorItem directDefs[256];
   extern Bool s3DAC8Bit;
   int border_changed = 0;

   if (pmap != InstalledMaps[pmap->pScreen->myNum])
      return;

   if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
      ndef = cfbExpandDirectColors(pmap, ndef, pdefs, directDefs);
      pdefs = directDefs;
   }
   BLOCK_CURSOR;
   for (i = 0; i < ndef; i++) {
      unsigned char r, g, b;

      if (s3DAC8Bit) {
         r = currents3dac[pdefs[i].pixel].r =
	    xf86rGammaMap[pdefs[i].red   >> 8];
         g = currents3dac[pdefs[i].pixel].g =
	    xf86gGammaMap[pdefs[i].green >> 8];
         b = currents3dac[pdefs[i].pixel].b =
	    xf86bGammaMap[pdefs[i].blue  >> 8];
      } else {
         r = currents3dac[pdefs[i].pixel].r =
	    xf86rGammaMap[pdefs[i].red   >> 8] >> 2;
         g = currents3dac[pdefs[i].pixel].g =
	    xf86gGammaMap[pdefs[i].green >> 8] >> 2;
         b = currents3dac[pdefs[i].pixel].b =
	    xf86bGammaMap[pdefs[i].blue  >> 8] >> 2;
      }
      if (xf86VTSema 
#ifdef XFreeXDGA
	  || ((s3InfoRec.directMode & XF86DGADirectGraphics)
	      && !(s3InfoRec.directMode & XF86DGADirectColormap))
	  || (s3InfoRec.directMode & XF86DGAHasColormap)
#endif
         ) {
	 outb(DAC_W_INDEX, pdefs[i].pixel);
	 outb(DAC_DATA, r);
	 outb(DAC_DATA, g);
	 outb(DAC_DATA, b);
      }
      if (pdefs[i].pixel == currents3dac_border) 
	 border_changed = 1;
   }
   if (border_changed) {
      int ilow=255, sum;
      int low=currents3dac[ilow].r + currents3dac[ilow].g + currents3dac[ilow].b;

      /* looking for lowest/black LUT entry */
      for (i=0; low>0 && i<256; i++) {
	 sum = currents3dac[i].r + currents3dac[i].g + currents3dac[i].b;
	 if (sum < low) {
	    low = sum;
	    ilow = i; 
	 }
      }
      currents3dac_border = ilow;
      if (xf86VTSema 
#ifdef XFreeXDGA
	  || ((s3InfoRec.directMode & XF86DGADirectGraphics)
	      && !(s3InfoRec.directMode & XF86DGADirectColormap))
	  || (s3InfoRec.directMode & XF86DGAHasColormap)
#endif
         ) {
         i = inb(vgaIOBase + 0x0A);   /* reset flip-flop */
         outb(0x3C0, 0x11 | 0x20);
      
         /* change AR11 border color ... */
         i = inb(vgaIOBase + 0x0A);   /* reset flip-flop */
         outb(0x3C0, 0x11 | 0x20);
         outb(0x3C0, currents3dac_border);
      }
      ((vgaHWPtr)vgaNewVideoState)->Attribute[0x11] = currents3dac_border;
   }
   UNBLOCK_CURSOR;
}

void
s3InstallColormap(pmap)
     ColormapPtr pmap;
{
   ColormapPtr oldmap = InstalledMaps[pmap->pScreen->myNum];
   int   entries;
   Pixel *ppix;
   xrgb *prgb;
   xColorItem *defs;
   int   i;

   if (pmap == oldmap)
      return;

   if ((pmap->pVisual->class | DynamicClass) == DirectColor)
      entries = (pmap->pVisual->redMask |
		 pmap->pVisual->greenMask |
		 pmap->pVisual->blueMask) + 1;
   else
      entries = pmap->pVisual->ColormapEntries;

   ppix = (Pixel *) ALLOCATE_LOCAL(entries * sizeof(Pixel));
   prgb = (xrgb *) ALLOCATE_LOCAL(entries * sizeof(xrgb));
   defs = (xColorItem *) ALLOCATE_LOCAL(entries * sizeof(xColorItem));

   if (oldmap != NOMAPYET)
      WalkTree(pmap->pScreen, TellLostMap, &oldmap->mid);

   InstalledMaps[pmap->pScreen->myNum] = pmap;

   for (i = 0; i < entries; i++)
      ppix[i] = i;

   QueryColors(pmap, entries, ppix, prgb);

   for (i = 0; i < entries; i++) {	/* convert xrgbs to xColorItems */
      defs[i].pixel = ppix[i];
      defs[i].red = prgb[i].red;
      defs[i].green = prgb[i].green;
      defs[i].blue = prgb[i].blue;
      defs[i].flags = DoRed | DoGreen | DoBlue;
   }

   s3StoreColors(pmap, entries, defs);

   WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
   s3RenewCursorColor(pmap->pScreen);
   DEALLOCATE_LOCAL(ppix);
   DEALLOCATE_LOCAL(prgb);
   DEALLOCATE_LOCAL(defs);
}

void
s3UninstallColormap(pmap)
     ColormapPtr pmap;
{
   ColormapPtr defColormap;

   if (pmap != InstalledMaps[pmap->pScreen->myNum])
      return;

   defColormap = (ColormapPtr) LookupIDByType(pmap->pScreen->defColormap,
					      RT_COLORMAP);

   if (defColormap == InstalledMaps[pmap->pScreen->myNum])
      return;

   (*pmap->pScreen->InstallColormap) (defColormap);
}

/* This is for the screen saver */
void
s3RestoreColor0(pScreen)
     ScreenPtr pScreen;
{
   Pixel pix = 0;
   xrgb  rgb;
   extern Bool s3DAC8Bit;
   
   if (InstalledMaps[pScreen->myNum] == NOMAPYET)
      return;

   QueryColors(InstalledMaps[pScreen->myNum], 1, &pix, &rgb);

   BLOCK_CURSOR;
   outb(DAC_W_INDEX, 0);
   if (s3DAC8Bit) {
      outb(DAC_DATA, xf86rGammaMap[rgb.red   >> 8]);
      outb(DAC_DATA, xf86gGammaMap[rgb.green >> 8]);
      outb(DAC_DATA, xf86bGammaMap[rgb.blue  >> 8]);
   } else {
      outb(DAC_DATA, xf86rGammaMap[rgb.red   >> 8] >> 2);
      outb(DAC_DATA, xf86gGammaMap[rgb.green >> 8] >> 2);
      outb(DAC_DATA, xf86bGammaMap[rgb.blue  >> 8] >> 2);
   }
   UNBLOCK_CURSOR;
}
