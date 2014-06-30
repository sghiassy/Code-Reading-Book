#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "colormapst.h"
#include "windowstr.h"
#include "compiler.h"

#include "r1.h"

#define NOMAPYET        (ColormapPtr) 0

static ColormapPtr InstalledMaps[MAXSCREENS];


int
r1ListInstalledColormaps(pScreen, pmaps)
ScreenPtr pScreen;
Colormap *pmaps;
{
    /*
     * By the time we are processing requests, we can guarantee that there is
     * always a colormap installed
     */
    *pmaps = InstalledMaps[pScreen->myNum]->mid;
    return(1);
}


int
r1GetInstalledColormaps(pScreen, pmap)
ScreenPtr        pScreen;
ColormapPtr      *pmap;
{
    *pmap = InstalledMaps[pScreen->myNum];
    return(1);
}


void
r1StoreColors(pmap, ndef, pdefs)
ColormapPtr pmap;
int   ndef;
xColorItem *pdefs;
{
   int   i;
   xColorItem directDefs[256];

   if (pmap != InstalledMaps[pmap->pScreen->myNum])
       return;

   if ((pmap->pVisual->class | DynamicClass) == DirectColor) {
       ndef = cfbExpandDirectColors(pmap, ndef, pdefs, directDefs);
       pdefs = directDefs;
   }

   /* XXX -- R1-specific code goes here... */


}


void
r1InstallColormap(pmap)
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

   r1StoreColors(pmap, entries, defs);

   WalkTree(pmap->pScreen, TellGainedMap, &pmap->mid);
   DEALLOCATE_LOCAL(ppix);
   DEALLOCATE_LOCAL(prgb);
   DEALLOCATE_LOCAL(defs);
}


void
r1UninstallColormap(pmap)
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
