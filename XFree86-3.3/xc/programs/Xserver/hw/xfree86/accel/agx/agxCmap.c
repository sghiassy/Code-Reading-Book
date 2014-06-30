/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxCmap.c,v 3.9 1996/12/23 06:32:32 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1994    by Henry A. Worth, Sunnyvale, California.
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
 * THOMAS ROELL and HENRY A. WORTH DISCLAIMS ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL AUTHORS BE 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, 
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS 
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE 
 * OF THIS SOFTWARE.
 *
 *  Rewritten for the AGX by Henry A Worth (haw30@eng.amdahl.com)
 *
 */
/* $XConsortium: agxCmap.c /main/9 1996/10/27 11:46:30 kaleb $ */

#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"

#include "agx.h"

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
agxListInstalledColormaps(pScreen, pmaps)
     ScreenPtr	pScreen;
     Colormap	*pmaps;
{
  /* By the time we are processing requests, we can guarantee that there
   * is always a colormap installed */
  
  *pmaps = InstalledMaps[pScreen->myNum]->mid;
  return(1);
}

void
agxStoreColors(pmap, ndef, pdefs)
   ColormapPtr	pmap;
   int		ndef;
   xColorItem	        *pdefs;
{
   int		i;
   xColorItem	directDefs[256];
   extern LUTENTRY agxsavedLUT[256];
   unsigned char oldIndex;
   unsigned char palIdx;
   unsigned int  palIdxReg;
   unsigned char palDataIdx;
   unsigned int  palDataReg;
   unsigned char overScan = agxCRTCRegs.overscan;
   unsigned char newOverScan = FALSE;

   if (pmap != InstalledMaps[pmap->pScreen->myNum])
      return;

   if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
      ndef = cfbExpandDirectColors (pmap, ndef, pdefs, directDefs);
      pdefs = directDefs;
   }

   if(XGA_PALETTE_CONTROL(agxChipId)) {
      palIdx     = IR_CUR_PAL_INDEX_LO;
      palIdxReg  = agxByteData;
      palDataIdx = IR_PAL_DATA;
      palDataReg = agxByteData;
   }
   else {
      palIdx     = 0;
      palIdxReg  = VGA_PAL_WRITE_INDEX;
      palDataIdx = 0;
      palDataReg = VGA_PAL_DATA;
   } 
       
   for (i = 0; i < ndef; i++) {
      unsigned int red, green, blue, idx;
      
      idx = pdefs[i].pixel;
      if (idx == overScan)
          newOverScan = TRUE;
      /*
       * Return the 8 most significant bits from a 16-bit value.
       * The original XGA used most-significant 6-bits (unlike VGA's LS 6-bits)
       *
       */
      if (xf86Dac8Bit) {  /* XGA 8-bit or 6-bit */
         red   = agxsavedLUT[idx].r = pdefs[i].red >> 8;
         green = agxsavedLUT[idx].g = pdefs[i].green >> 8;
         blue  = agxsavedLUT[idx].b = pdefs[i].blue >> 8;
      }
      else {  /* VGA style 6-bit */
         red   = agxsavedLUT[idx].r = pdefs[i].red >> 10;
         green = agxsavedLUT[idx].g = pdefs[i].green >> 10;
         blue  = agxsavedLUT[idx].b = pdefs[i].blue >> 10;
      }

      if ( xf86VTSema
#ifdef XFreeXDGA
           || ((agxInfoRec.directMode & XF86DGADirectGraphics)
	       && !(agxInfoRec.directMode & XF86DGADirectColormap))
           || (agxInfoRec.directMode & XF86DGAHasColormap)
#endif
      ) {
         oldIndex = inb(agxIdxReg); 
         outb(agxIdxReg,  palIdx);
         outb(palIdxReg,  idx);
         outb(agxIdxReg,  palDataIdx);
         outb(palDataReg, red);
         outb(palDataReg, green);
         outb(palDataReg, blue);
         outb(agxIdxReg, oldIndex);
      }
   }

   if (newOverScan) {
      unsigned int idx, tmp;

      newOverScan = FALSE;
      for(i = 0; i < ndef; i++) {
         idx = pdefs[i].pixel;
         if (idx == overScan)
         {
            if ((pdefs[i].red != 0)
                || (pdefs[i].green != 0)
                || (pdefs[i].blue != 0)) {
               newOverScan = TRUE;
               tmp = idx;
            }
            break;
        }
      }
      if (newOverScan) {
         /*
          * Find a black pixel, or the nearest match.
          */
         for (i=255; i >= 0; i--) {
            if ((agxsavedLUT[i].r == 0)
                && (agxsavedLUT[i].g == 0)
                && (agxsavedLUT[i].b == 0)) {
               overScan = i;
               break;
            }
            else {
               if ((agxsavedLUT[i].r < agxsavedLUT[tmp].r)
                   && (agxsavedLUT[i].g < agxsavedLUT[tmp].g)
                   && (agxsavedLUT[i].b < agxsavedLUT[tmp].b)) {
                  tmp = i;
               }
            }
         }
         if (i < 0) {
            overScan = tmp;
         }
         agxCRTCRegs.overscan = overScan;
         if ( xf86VTSema
#ifdef XFreeXDGA
              || (agxInfoRec.directMode & XF86DGAHasColormap)
#endif
          ) {
            oldIndex = inb(agxIdxReg); 
            outb(agxIdxReg, IR_BORDER_CLR); 
            outb(agxByteData, overScan);
            outb(agxIdxReg, oldIndex);
         }
      }
   }
}


void
agxInstallColormap(pmap)
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

  agxStoreColors( pmap, entries, defs);

  WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
  
  DEALLOCATE_LOCAL(ppix);
  DEALLOCATE_LOCAL(prgb);
  DEALLOCATE_LOCAL(defs);
}


void
agxUninstallColormap(pmap)
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
agxRestoreColor0(pScreen)
     ScreenPtr pScreen;
{
  Pixel       pix = 0;
  xrgb        rgb;
  unsigned int  palDataReg;

  if (InstalledMaps[pScreen->myNum] == NOMAPYET)
    return;

  QueryColors(InstalledMaps[pScreen->myNum], 1, &pix, &rgb);

  if(XGA_PALETTE_CONTROL(agxChipId)) {
     outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
     outb(agxByteData, 0x00);
     outb(agxIdxReg, IR_PAL_DATA);
     palDataReg = agxByteData;
  }
  else {
     outb(agxIdxReg, 0);  /* make sure index is not 0x51 */
     outb(VGA_PAL_WRITE_INDEX, 0x00);
     palDataReg = VGA_PAL_DATA;
  }
  if (xf86Dac8Bit) {  /* XGA 8-bit or 6-bit */
    outb(palDataReg, rgb.red >> 8);
    outb(palDataReg, rgb.green >> 8);
    outb(palDataReg, rgb.blue >> 8);
  } else {
    outb(palDataReg, rgb.red >> 10);
    outb(palDataReg, rgb.green >> 10);
    outb(palDataReg, rgb.blue >> 10);
  }
}

void
agxClearColor0()
{
  Pixel       pix = 0;
  xrgb        rgb;
  unsigned int  palDataReg;

  if(XGA_PALETTE_CONTROL(agxChipId)) {
     outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
     outb(agxByteData, 0x00);
     outb(agxIdxReg, IR_PAL_DATA);
     palDataReg = agxByteData;
  }
  else {
     outb(agxIdxReg, 0);  /* make sure index is not 0x51 */
     outb(VGA_PAL_WRITE_INDEX, 0x00);
     palDataReg = VGA_PAL_DATA;
  }
  outb(palDataReg, 0);
  outb(palDataReg, 0);
  outb(palDataReg, 0);
}


