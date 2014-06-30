/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaCmap.c,v 3.15 1996/12/23 06:59:30 dawes Exp $ */
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
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: vgaCmap.c /main/15 1996/10/28 05:13:44 kaleb $ */

#include "X.h"
#include "Xproto.h"
#include "windowstr.h"
#include "compiler.h"

#include "xf86.h"
#include "vga.h"

#ifdef XFreeXDGA
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif


#define NOMAPYET        (ColormapPtr) 0

extern Bool clgd6225Lcd;

static ColormapPtr InstalledMaps[MAXSCREENS];
				/* current colormap for each screen */

int
vgaListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}

int
vgaGetInstalledColormaps(pScreen, pmaps)
     ScreenPtr		pScreen;
     ColormapPtr	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum];
  return(1);
}


void
vgaStoreColors(pmap, ndef, pdefs)
     ColormapPtr	pmap;
     int		ndef;
     xColorItem	        *pdefs;
{
    int		i;
    unsigned char *cmap, *tmp;
    xColorItem	directDefs[256];
    Bool          new_overscan = FALSE;
    unsigned char overscan = ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN];
    unsigned char tmp_overscan;

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
        return;

    /* GJA -- We don't want cfb code right now (in vga16 server) */
#ifndef XF86VGA16
    if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    {
        ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
        pdefs = directDefs;
    }
#endif

    for(i = 0; i < ndef; i++)
    {
        if (pdefs[i].pixel == overscan)
	{
	    new_overscan = TRUE;
	}
        cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[pdefs[i].pixel*3];
#ifndef PC98_EGC
#ifndef PC98_NEC480
	if (vgaDAC8BitComponents) {
            cmap[0] = pdefs[i].red   >> 8;
            cmap[1] = pdefs[i].green >> 8;
            cmap[2] = pdefs[i].blue  >> 8;
        }
        else {
            cmap[0] = pdefs[i].red   >> 10;
            cmap[1] = pdefs[i].green >> 10;
            cmap[2] = pdefs[i].blue  >> 10;
        }
#else /* PC98_NEC480 */
        cmap[0] = pdefs[i].red   >> 8;
        cmap[1] = pdefs[i].green >> 8;
        cmap[2] = pdefs[i].blue  >> 8;
#endif /* PC98_NEC480 */
#else
        cmap[0] = pdefs[i].red   >> 12;
        cmap[1] = pdefs[i].green >> 12;
        cmap[2] = pdefs[i].blue  >> 12;
#endif /* PC98_EGC */
	if (clgd6225Lcd)
	{
		/* The LCD doesn't like white */
		if (cmap[0] == 63) cmap[0]= 62;
		if (cmap[1] == 63) cmap[1]= 62;
		if (cmap[2] == 63) cmap[2]= 62;
	}

        if (xf86VTSema
#ifdef XFreeXDGA
	    || ((vga256InfoRec.directMode & XF86DGADirectGraphics)
	        && !(vga256InfoRec.directMode & XF86DGADirectColormap))
	    || (vga256InfoRec.directMode & XF86DGAHasColormap)
#endif
	   )
	{
#if !defined(PC98_EGC) && !defined(PC98_NEC480)
	    outb(0x3C8, pdefs[i].pixel);
	    DACDelay;
	    outb(0x3C9, cmap[0]);
	    DACDelay;
	    outb(0x3C9, cmap[1]);
	    DACDelay;
	    outb(0x3C9, cmap[2]);
	    DACDelay;
#else
	    /* also, PC9821Ne */
	    outb(0xa8, pdefs[i].pixel);
	    outb(0xac, cmap[0]);
	    outb(0xaa, cmap[1]);
	    outb(0xae, cmap[2]);
#endif /* PC98_EGC */
	}
    }	
    if (new_overscan)
    {
	new_overscan = FALSE;
        for(i = 0; i < ndef; i++)
        {
            if (pdefs[i].pixel == overscan)
	    {
	        if ((pdefs[i].red != 0) || 
	            (pdefs[i].green != 0) || 
	            (pdefs[i].blue != 0))
	        {
	            new_overscan = TRUE;
		    tmp_overscan = overscan;
        	    tmp = &((vgaHWPtr)vgaNewVideoState)->DAC[pdefs[i].pixel*3];
	        }
	        break;
	    }
        }
        if (new_overscan)
        {
            /*
             * Find a black pixel, or the nearest match.
             */
            for (i=255; i >= 0; i--)
	    {
                cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[i*3];
	        if ((cmap[0] == 0) && (cmap[1] == 0) && (cmap[2] == 0))
	        {
	            overscan = i;
	            break;
	        }
	        else
	        {
	            if ((cmap[0] < tmp[0]) && 
		        (cmap[1] < tmp[1]) && (cmap[2] < tmp[2]))
	            {
		        tmp = cmap;
		        tmp_overscan = i;
	            }
	        }
	    }
	    if (i < 0)
	    {
	        overscan = tmp_overscan;
	    }
	    ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN] = overscan;
            if (xf86VTSema
#ifdef XFreeXDGA
	        || ((vga256InfoRec.directMode & XF86DGADirectGraphics)
	            && !(vga256InfoRec.directMode & XF86DGADirectColormap))
	        || (vga256InfoRec.directMode&XF86DGAHasColormap)
#endif
	       )
	    {
#ifndef PC98_EGC
	        (void)inb(vgaIOBase + 0x0A);
	        outb(0x3C0, OVERSCAN);
	        outb(0x3C0, overscan);
	        (void)inb(vgaIOBase + 0x0A);
	        outb(0x3C0, 0x20);
#endif
	    }
        }
    }
}


void
vgaInstallColormap(pmap)
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

  vgaStoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
  
  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}


void
vgaUninstallColormap(pmap)
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

