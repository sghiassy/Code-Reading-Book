/*
 * $XConsortium: skyCmap.c,v 1.3 91/12/11 21:23:39 eswu Exp $ 
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991 
 *
 * All Rights Reserved 
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of IBM not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission. 
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND NONINFRINGEMENT OF
 * THIRD PARTY RIGHTS, IN NO EVENT SHALL IBM BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE. 
 *
 */

/*
 * skyCmap.c - colormap routines 
 */

#define NEED_EVENTS

#include "X.h"
#include "Xproto.h"
#include "resource.h"
#include "misc.h"
#include "scrnintstr.h"
#include "screenint.h"
#include "colormapst.h"
#include "colormap.h"
#include "pixmapstr.h"

#include "OScompiler.h"
#include "ibmTrace.h"

#include "skyHdwr.h"
#include "skyProcs.h"


static ColormapPtr InstalledMaps[MAXSCREENS];


void
skyStoreColors(pColormap, ndef, pdefs)
    ColormapPtr     pColormap;
    int             ndef;
    xColorItem     *pdefs;
{
    int             scrnNum = pColormap->pScreen->myNum;

    TRACE(("skyStoreColors(pColormap=x%x)\n", pColormap));

    if (pColormap != InstalledMaps[scrnNum])
	return;

    while (ndef--)
    {
	SKYSetColor(scrnNum, pdefs->pixel,
		pdefs->red, pdefs->green, pdefs->blue);
	pdefs++;
    }
}


void
skyInstallColormap(pColormap)
    ColormapPtr     pColormap;
{
    ScreenPtr       pScreen = pColormap->pScreen;
    VisualPtr       pVisual = pColormap->pVisual;
    int             scrnNum = pScreen->myNum;

    int             i;
    Entry          *pent;
    unsigned int    red, green, blue;
    ColormapPtr     pOldMap;


    TRACE(("skyInstallColormap(pColormap=x%x)\n", pColormap));

    if ((pVisual->class | DynamicClass) == DirectColor)
    {
	for (i = 0; i < 256; i++)
	{
	    pent = &pColormap->red[(i & pVisual->redMask) >>
				   pVisual->offsetRed];
	    red = pent->co.local.red;
	    pent = &pColormap->green[(i & pVisual->greenMask) >>
				     pVisual->offsetGreen];
	    green = pent->co.local.green;
	    pent = &pColormap->blue[(i & pVisual->blueMask) >>
				    pVisual->offsetBlue];
	    blue = pent->co.local.blue;

	    SKYSetColor(scrnNum, i, red, green, blue);
	}
    }
    else
    {
	for (i = 0, pent = pColormap->red;
	     i < pVisual->ColormapEntries;
	     i++, pent++)
	{
	    if (pent->fShared)
	    {
		red = pent->co.shco.red->color;
		green = pent->co.shco.green->color;
		blue = pent->co.shco.blue->color;
	    }
	    else
	    {
		red = pent->co.local.red;
		green = pent->co.local.green;
		blue = pent->co.local.blue;
	    }

	    SKYSetColor(scrnNum, i, red, green, blue);
	}
    }


    pOldMap = InstalledMaps[scrnNum];
    if (pColormap != pOldMap)
    {
	if (pOldMap != (ColormapPtr)None)
	    WalkTree(pScreen, TellLostMap, (char *)&pOldMap->mid);

	InstalledMaps[scrnNum] = pColormap;
	WalkTree(pScreen, TellGainedMap, (char *)&pColormap->mid);

    }
    return;
}


void
skyUninstallColormap(pColormap)
    ColormapPtr     pColormap;
{
    ColormapPtr     pDefCmap;
    ScreenPtr       pScreen = pColormap->pScreen;
    int             scrnNum = pScreen->myNum;

    TRACE(("skyUninstallColormap(pColormap=x%x)\n", pColormap));

    if (pColormap != InstalledMaps[scrnNum])
	return;

    pDefCmap = (ColormapPtr) LookupIDByType(pScreen->defColormap, RT_COLORMAP);

    if (pDefCmap != pColormap)	/* never uninstall the default map */
	(* pScreen->InstallColormap) (pDefCmap);

    return;
}


int
skyListInstalledColormaps(pScreen, pCmapList)
    ScreenPtr       pScreen;
    Colormap       *pCmapList;
{
    int             scrnNum = pScreen->myNum;

    TRACE(("skyListInstalledMapss()\n"));

    *pCmapList = InstalledMaps[scrnNum]->mid;
    return 1;
}


void
skyRefreshColormaps(pScreen)
    ScreenPtr       pScreen;
{
    int             scrnNum = pScreen->myNum;

    TRACE(("skyRefreshColormaps(pColormap=x%x)\n", InstalledMaps[scrnNum]));
    skyInstallColormap(InstalledMaps[scrnNum]);
}
