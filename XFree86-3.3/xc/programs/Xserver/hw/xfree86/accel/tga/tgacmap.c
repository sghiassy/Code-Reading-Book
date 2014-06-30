/* $XConsortium: tgacmap.c /main/2 1996/10/24 10:56:00 kaleb $ */
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

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tgacmap.c,v 3.1 1996/12/27 07:03:47 dawes Exp $ */

/*
 * Modified by Amancio Hasty and Jon Tombs
 * 
 * Adapted to the I128 chipset by Robin Cutshaw
 *
 * Adapted for the DEC TGA by Alan Hourihane <alanh@fairlite.demon.co.uk>
 */


#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"

#include "tga.h"
#include "tga_regs.h"
#include "tga_presets.h"

extern unsigned char xf86rGammaMap[], xf86gGammaMap[], xf86bGammaMap[];
extern struct tgamem tgamem;

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];

/* current colormap for each screen */

LUTENTRY currenttgadac[256];

int
tgaListInstalledColormaps(pScreen, pmaps)
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
tgaRestoreDACvalues()
{
   int i;

   if (xf86VTSema) {
      BLOCK_CURSOR;
      BT485_WRITE(0x00, BT485_ADDR_PAL_WRITE);
      TGA_WRITE_REG(BT485_DATA_PAL, TGA_RAMDAC_SETUP_REG);

      for (i=0; i < 256; i++) {
	TGA_WRITE_REG(currenttgadac[i].r|(BT485_DATA_PAL<<8),TGA_RAMDAC_REG);
	TGA_WRITE_REG(currenttgadac[i].g|(BT485_DATA_PAL<<8),TGA_RAMDAC_REG);
	TGA_WRITE_REG(currenttgadac[i].b|(BT485_DATA_PAL<<8),TGA_RAMDAC_REG);
      }
      UNBLOCK_CURSOR;
   }
}

int
tgaGetInstalledColormaps(pScreen, pmap)
     ScreenPtr        pScreen;
     ColormapPtr      *pmap;
{
  *pmap = InstalledMaps[pScreen->myNum];
  return(1);
}


void
tgaStoreColors(pmap, ndef, pdefs)
     ColormapPtr pmap;
     int   ndef;
     xColorItem *pdefs;
{
   int   i;
   xColorItem directDefs[256];
   extern Bool tgaDAC8Bit;

   if (pmap != InstalledMaps[pmap->pScreen->myNum])
      return;

   if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
      ndef = cfbExpandDirectColors(pmap, ndef, pdefs, directDefs);
      pdefs = directDefs;
   }
   BLOCK_CURSOR;
   for (i = 0; i < ndef; i++) {
      unsigned char r, g, b;

      if (tgaDAC8Bit) {
         r = currenttgadac[pdefs[i].pixel].r =
	    xf86rGammaMap[pdefs[i].red   >> 8];
         g = currenttgadac[pdefs[i].pixel].g =
	    xf86gGammaMap[pdefs[i].green >> 8];
         b = currenttgadac[pdefs[i].pixel].b =
	    xf86bGammaMap[pdefs[i].blue  >> 8];
      } else {
         r = currenttgadac[pdefs[i].pixel].r =
	    xf86rGammaMap[pdefs[i].red   >> 8] >> 2;
         g = currenttgadac[pdefs[i].pixel].g =
	    xf86gGammaMap[pdefs[i].green >> 8] >> 2;
         b = currenttgadac[pdefs[i].pixel].b =
	    xf86bGammaMap[pdefs[i].blue  >> 8] >> 2;
      }
      if (xf86VTSema) {
	 BT485_WRITE(pdefs[i].pixel, BT485_ADDR_PAL_WRITE); 
	 TGA_WRITE_REG(BT485_DATA_PAL, TGA_RAMDAC_SETUP_REG);
	 TGA_WRITE_REG(r|(BT485_DATA_PAL<<8), TGA_RAMDAC_REG);
	 TGA_WRITE_REG(g|(BT485_DATA_PAL<<8), TGA_RAMDAC_REG);
	 TGA_WRITE_REG(b|(BT485_DATA_PAL<<8), TGA_RAMDAC_REG); 
      }
   }
   UNBLOCK_CURSOR;
}

void
tgaInstallColormap(pmap)
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

   tgaStoreColors(pmap, entries, defs);

   WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
#ifdef WORKWORKWORK
   tgaRenewCursorColor(pmap->pScreen);
#endif
   DEALLOCATE_LOCAL(ppix);
   DEALLOCATE_LOCAL(prgb);
   DEALLOCATE_LOCAL(defs);
}

void
tgaUninstallColormap(pmap)
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
tgaRestoreColor0(pScreen)
     ScreenPtr pScreen;
{
   Pixel pix = 0;
   xrgb  rgb;
   extern Bool tgaDAC8Bit;
   
   if (InstalledMaps[pScreen->myNum] == NOMAPYET)
      return;

   QueryColors(InstalledMaps[pScreen->myNum], 1, &pix, &rgb);

   BLOCK_CURSOR;
   BT485_WRITE(0x00, BT485_ADDR_PAL_WRITE);
   TGA_WRITE_REG(BT485_DATA_PAL, TGA_RAMDAC_SETUP_REG);
   if (tgaDAC8Bit) {
	TGA_WRITE_REG(xf86rGammaMap[rgb.red >> 8]|(BT485_DATA_PAL << 8),
						TGA_RAMDAC_REG);
	TGA_WRITE_REG(xf86gGammaMap[rgb.green >> 8]|(BT485_DATA_PAL << 8),
						TGA_RAMDAC_REG);
	TGA_WRITE_REG(xf86bGammaMap[rgb.blue >> 8]|(BT485_DATA_PAL << 8),
						TGA_RAMDAC_REG);
   } else {
	TGA_WRITE_REG((xf86rGammaMap[rgb.red >> 8] >> 2)|(BT485_DATA_PAL << 8),
						TGA_RAMDAC_REG);
	TGA_WRITE_REG((xf86gGammaMap[rgb.green >> 8] >> 2)|(BT485_DATA_PAL << 8),
						TGA_RAMDAC_REG);
	TGA_WRITE_REG((xf86bGammaMap[rgb.blue >> 8] >> 2)|(BT485_DATA_PAL << 8),
						TGA_RAMDAC_REG);
   }
   UNBLOCK_CURSOR;
}

void
tgaUnblankScreen(pScreen)
	ScreenPtr	pScreen;
{
}

void
tgaBlankScreen(pScreen)
	ScreenPtr	pScreen;
{
}
