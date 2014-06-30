/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/vgaCmap.c,v 3.8 1997/01/18 06:54:01 dawes Exp $ */
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
/* $XConsortium: vgaCmap.c /main/4 1996/02/21 17:22:09 kaleb $ */

#include "X.h"
#include "Xproto.h"
#include "windowstr.h"
#include "compiler.h"

#include "xf86.h"
#include "vga.h"
#include "w32.h"

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
vgaListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}


static void
set_overscan(overscan)
    int overscan;
{
    inb(0x3da);
    GlennsIODelay();
    outb(0x3c0, 0x11);
    GlennsIODelay();
    outb(0x3c0, overscan);
    GlennsIODelay();
    inb(0x3da);
    GlennsIODelay();
    outb(0x3c0, 0x20);
    GlennsIODelay();
} 


static void
set_clu_entry(entry, cmap)
    int entry;
    unsigned char *cmap;
{
    outb(0x3c8, entry);
    GlennsIODelay();
    outb(0x3c9, cmap[0]);
    GlennsIODelay();
    outb(0x3c9, cmap[1]);
    GlennsIODelay();
    outb(0x3c9, cmap[2]);
    GlennsIODelay();
} 


static W32Blanked = FALSE;

static char black_cmap[] = {0x0, 0x0, 0x0};

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
    unsigned char old_overscan; 
    unsigned char tmp_overscan;
   
    extern RamdacShift;

    if (pmap != InstalledMaps[pmap->pScreen->myNum])
        return;

    /* GJA -- We don't want cfb code right now (in vga16 server) */
    if ((pmap->pVisual->class | DynamicClass) == DirectColor)
    {
        ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
        pdefs = directDefs;
    }

    for(i = 0; i < ndef; i++)
    {
        if (pdefs[i].pixel == overscan)
	{
	    old_overscan = overscan; 
	    new_overscan = TRUE;
	}
        cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[pdefs[i].pixel*3];
        cmap[0] = pdefs[i].red   >> RamdacShift;
        cmap[1] = pdefs[i].green >> RamdacShift;
        cmap[2] = pdefs[i].blue  >> RamdacShift;

        if ((xf86VTSema 
#ifdef XFreeXDGA
	     || ((vga256InfoRec.directMode & XF86DGADirectGraphics)
		 && !(vga256InfoRec.directMode & XF86DGADirectColormap))
	     || (vga256InfoRec.directMode & XF86DGAHasColormap)
#endif
	    ) && (!W32Blanked || pdefs[i].pixel != overscan))
	{
	    outb(0x3C8, pdefs[i].pixel);
	    GlennsIODelay();
	    outb(0x3C9, cmap[0]);
	    GlennsIODelay();
	    outb(0x3C9, cmap[1]);
	    GlennsIODelay();
	    outb(0x3C9, cmap[2]);
	    GlennsIODelay();
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
/* CHANGE to SUMMATION--GGLGGL */ 
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
		|| (vga256InfoRec.directMode & XF86DGAHasColormap)
#endif
	       )
	    {
		if (W32Blanked)
		    set_clu_entry(overscan, black_cmap);

	        (void)inb(vgaIOBase + 0x0A);
		GlennsIODelay();
	        outb(0x3C0, OVERSCAN);
		GlennsIODelay();
	        outb(0x3C0, overscan);
		GlennsIODelay();
	        (void)inb(vgaIOBase + 0x0A);
		GlennsIODelay();
	        outb(0x3C0, 0x20);
		GlennsIODelay();

		if (W32Blanked)
		{
		    cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[old_overscan * 3];
		    set_clu_entry (old_overscan, cmap);
		}
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


/*
 *    The power saver is for w32p_rev_c and later only--GGL. 
 */
Bool
W32SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    unsigned char state;
    unsigned char *cmap;
    unsigned char overscan; 

    if (on)
	SetTimeSinceLastInputEvent();
    if (xf86VTSema)
    {
	outb(vgaIOBase + 4, 0x34);
	state = inb(vgaIOBase + 5);
  
	if (on) {
	    state &= ~0x21;
	    W32Blanked = FALSE; 

	    overscan = ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN];
	    cmap = &((vgaHWPtr)vgaNewVideoState)->DAC[overscan * 3];

	    inb(vgaIOBase + 0x0A);
	    GlennsIODelay();
	    outb(0x3C0, 0x20);         
	    GlennsIODelay();

	    set_clu_entry(overscan, cmap);

#ifdef DPMSExtension
	    if (DPMSEnabled && W32pCAndLater)
	    {
		outb(vgaIOBase + 4, 0x34);
		GlennsIODelay();
		outb(vgaIOBase + 5, state);
		GlennsIODelay();
	    }
#endif
	} else {
	    state |= 0x21;
	    W32Blanked = TRUE; 
    
	    overscan = ((vgaHWPtr)vgaNewVideoState)->Attribute[OVERSCAN];
	    set_clu_entry(overscan, black_cmap);

	    inb(vgaIOBase + 0x0A);
	    GlennsIODelay();
	    outb(0x3C0, 0x00);         
	    GlennsIODelay();

#ifdef DPMSExtension
	    if (DPMSEnabled && W32pCAndLater)
	    {
		outb(vgaIOBase + 4, 0x34);
		GlennsIODelay();
		outb(vgaIOBase + 5, state);
		GlennsIODelay();
	    }
#endif
	}
    }
    return(TRUE);
}

