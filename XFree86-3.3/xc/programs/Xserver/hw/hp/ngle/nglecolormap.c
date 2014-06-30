/* $XConsortium: nglecolormap.c,v 1.2 95/01/24 02:00:06 dpw Exp $ */

/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.
 *
 *************************************************************************/

/******************************************************************************
 *
 *  DDX entry points relating to color for NGLE driver:
 *
 *  	ngleLoadCursorColormap()	
 * 	ngleCreateDefColormap()
 * 	ngleCreateColormap()
 * 	ngleInstallColormap()
 * 	ngleStoreColors()
 * 	ngleListInstalledColormaps()
 * 	ngleUninstallColormap()
 * 	ngleDestroyColormap()
 *
 ******************************************************************************/

#include "ngle.h"

extern NgleLutBltCtl setNgleLutBltCtl(
    Card32              deviceID,
    Int32		devDepth,
    Card32              offsetWithinLut,
    Card32              length);            /* #entries to update */

extern NgleLutBltCtl setHyperLutBltCtl(
    Card32              deviceID,
    Int32		devDepth,
    Card32              offsetWithinLut,
    Card32              length);            /* #entries to update */

/* Gray Scale support: calculate intensity from rgb using NTSC weights */
#define CALCULATE_GRAYSCALE_INTENSITY(intensity, red8, grn8, blu8)	\
{									\
    intensity   = (Card8)						\
		( (float) ((red8) & 0xff) * 0.30			\
		+ (float) ((grn8) & 0xff) * 0.59			\
		+ (float) ((blu8) & 0xff) * 0.11			\
		);							\
    intensity   &= 0xff;						\
}


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleLoadCursorColormap()
 *
 * Description:
 *
 *      This routine is called by ngleDisplayCursor and ngleRecolorCursor
 *	to load into the NGLE hardware the 2-entry colormap for cursors.
 *
 *	This procedure is internal to the NGLE driver: it is not
 *	called from outside the driver, neither directly nor indirectly
 *	through a procedure table.
 *
 ******************************************************************************/

void ngleLoadCursorColormap(
    ScreenPtr   pScreen,
    CursorPtr   pCursor)
{
    NgleScreenPrivPtr       pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    NgleHdwPtr              pDregs = pScreenPriv->pDregs;
    Card32                  color;
    Card8		    intensity;


    /* Set up for drawing to the off-screen colormap table */
    START_CURSOR_COLORMAP_ACCESS(pDregs);

    /* X11 stores each R/G/B of a color in unsigned shorts (16 bits).
     * Scale to 8 bits and place in lower 24 bits of color.
     */
    if (pScreenPriv->isGrayScale)
    {
	CALCULATE_GRAYSCALE_INTENSITY(intensity,
		    (pCursor->backRed >> 8),
		    (pCursor->backGreen >> 8),
		    (pCursor->backBlue >> 8));
	/* Replicate intensity to red, green, and blue */
	color 	    = (Card32)
		    (	(intensity << 16) 
		    |	(intensity << 8)
		    |	intensity
		    );
	WRITE_CURSOR_COLOR(pDregs,color);

	CALCULATE_GRAYSCALE_INTENSITY(intensity,
		    (pCursor->foreRed >> 8),
		    (pCursor->foreGreen >> 8),
		    (pCursor->foreBlue >> 8));
	/* Replicate intensity to red, green, and blue */
	color 	    = (Card32)
		    (	(intensity << 16) 
		    |	(intensity << 8)
		    |	intensity
		    );
	WRITE_CURSOR_COLOR(pDregs,color);
    }
    else
    {
	color       = ( ((Card32) (pCursor->backRed  & 0xff00) << 8)
		    |    (Card32) (pCursor->backGreen & 0xff00)
		    |    (Card32) (pCursor->backBlue >> 8)
		    );
	WRITE_CURSOR_COLOR(pDregs,color);

	color       = ( ((Card32) (pCursor->foreRed  & 0xff00) << 8)
		    |    (Card32) (pCursor->foreGreen & 0xff00)
		    |    (Card32) (pCursor->foreBlue >> 8));
	WRITE_CURSOR_COLOR(pDregs,color);
    }

    FINISH_CURSOR_COLORMAP_ACCESS(pDregs,pScreenPriv->deviceID,
				    pScreenPriv->devDepth);

}   /* ngleLoadCursorColormap() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleCreateDefColormap
 *
 * Description: Create a default colormap for the given screen's root window.
 *
 * Called by: DDX procedure ngleScreenInit
 *
 * Creates default colormap, initializes all of its entries,
 * installs it into hardware, then points pScreen->defColormap to it.
 *
 ******************************************************************************/

Bool
ngleCreateDefColormap(
    ScreenPtr		pScreen)
{
    VisualPtr		pVisual;
    ColormapPtr		pColormap;
    unsigned short      red_zero = 0, red_ones = ~0;
    unsigned short      green_zero = 0, green_ones = ~0;
    unsigned short      blue_zero = 0, blue_ones = ~0;
 
    /* Scan the table of visuals for this screen, looking for the 
     * rootVisual.
     */
    for (pVisual = pScreen->visuals;
	 pVisual->vid != pScreen->rootVisual;
	 pVisual++)
	;

    /* Create a colormap, not allocating (reserving) any entries. */
    if (CreateColormap (pScreen->defColormap, pScreen, pVisual,
			&pColormap, AllocNone, 0) != Success)
    {
	return FALSE;
    }

    /* Reserve entries for black and white */
    if (AllocColor (pColormap, &red_zero, &green_zero, &blue_zero,
	&(pScreen->blackPixel), 0)
	    || AllocColor(pColormap, &red_ones, &green_ones, &blue_ones,
		    &(pScreen->whitePixel), 0)) 
    {
	FatalError("Unable to reserve black and white pixels.\n");
    }

    /* Install initialized colormap as default into hardware.  */
    ngleInstallColormap(pColormap);

    return(TRUE);
}   /* ngleCreateDefColormap() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleCreateColormap()
 *
 * Description: Perform device-specific initialization of new colormap.
 *
 * Entry:  pmap points to the colormap in question
 * 
 * Return Value: None
 *
 * Called by: DIX CreateColormap
 *
 ******************************************************************************/

Bool 
ngleCreateColormap(
    ColormapPtr pmap)
{
    NgleScreenPrivPtr 	pScreenPriv = NGLE_SCREEN_PRIV(pmap->pScreen);
    xColorItem		localColor;
    int 	        i;
    VisualPtr		pVisual = pmap->pVisual;

    localColor.flags = DoRed | DoGreen | DoBlue;

    if (pVisual->class == DirectColor )
    {
	for (i = 0; i < pVisual->ColormapEntries; i++)
	{
	    localColor.pixel = i << pVisual->offsetBlue
		             | i << pVisual->offsetGreen
		             | i << pVisual->offsetRed;
	    
	    localColor.red   = pmap->red[i].co.local.red     = i << 8;
	    localColor.green = pmap->green[i].co.local.green = i << 8;
	    localColor.blue  = pmap->blue[i].co.local.blue   = i << 8;
	    
	    ngleStoreColors(pmap, 1, &localColor);
	}
    }
    else if (pVisual->class == PseudoColor )
    {
        if (pScreenPriv->isGrayScale)
        {
	    for(i = 0; i < pVisual->ColormapEntries; i++) 
	    {
	        localColor.pixel = i;
	        localColor.red = pmap->red[i].co.local.red = i << 8;
	        localColor.green = pmap->red[i].co.local.green = i << 8;
	        localColor.blue = pmap->red[i].co.local.blue = i << 8;

	        ngleStoreColors(pmap, 1, &localColor);
	    }
        }
        else
        {
	    for(i = 0; i < pVisual->ColormapEntries; i++) 
	    {
	        localColor.pixel = i;
	        localColor.red = pmap->red[i].co.local.red = (i & 0x7) << 13;
	        localColor.green = pmap->red[i].co.local.green = (i & 0x38) << 10;
	        localColor.blue = pmap->red[i].co.local.blue = (i & 0xc0) << 8;

	        ngleStoreColors(pmap, 1, &localColor);
	    }
        }
    }
    else
    {
	ErrorF("ngleCreateColormap: Unsupported visual type\n");
    }

        return(TRUE);
}   /* ngleCreateColormap() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleInstallColormap()
 *
 * Description: Install given colormap into hardware.
 *
 * Results:
 *      None
 *
 * Side Effects:
 *      All clients requesting ColormapNotify are notified
 *	Colormap is installed into NGLE hardware
 *
 ******************************************************************************/
 
void ngleInstallColormap(
    ColormapPtr cmap)
{
    NgleScreenPrivPtr 	pScreenPriv = NGLE_SCREEN_PRIV(cmap->pScreen);
    NgleHdwPtr          pDregs = pScreenPriv->pDregs;
    unsigned int 	i;
    Card32		color;
    int			nColormapEntries = cmap->pVisual->ColormapEntries;
    Card8		intensity;
    unsigned short	red, green, blue;


    /* We do not need to install if this is the current map.
     * it's already installed.
     */
    if (cmap == pScreenPriv->installedMap)
        return;


    /* If uninstalling an explicitly installed map, tell everybody */
    if (pScreenPriv->installedMap)
    {
	WalkTree(pScreenPriv->installedMap->pScreen, TellLostMap,
		 (char *) &(pScreenPriv->installedMap->mid));
    }

    /* setup colormap hardware */
    START_IMAGE_COLORMAP_ACCESS(pDregs);
    
    /* Load each colormap entry into shadow RAM */

    if (cmap->pVisual->class == DirectColor)
    {
	for (i=0; i < nColormapEntries; i++)
	{
	    if (cmap->red[i].fShared)
		red = cmap->red[i].co.shco.red->color;
	    else
		red = cmap->red[i].co.local.red;

	    if (cmap->green[i].fShared)
		green = cmap->green[i].co.shco.green->color;
	    else
		green = cmap->green[i].co.local.green;

	    if (cmap->blue[i].fShared)
		blue = cmap->blue[i].co.shco.blue->color;
	    else
		blue = cmap->blue[i].co.local.blue;

	    if (!pScreenPriv->isGrayScale)
	    {
		color   = ( ((Card32) (red & 0xff00) << 8)
			|    (Card32) (green & 0xff00)
			|   ((Card32) (blue & 0xff00) >> 8)
			);
	    }
	    else
	    {	/* Gray Scale */
		CALCULATE_GRAYSCALE_INTENSITY(intensity,
		    (red >> 8),
		    (green >> 8),
		    (blue >> 8));
		color = intensity | (intensity << 8) | (intensity << 16);
	    }

	    pScreenPriv->hwColors[i] = color;

	    WRITE_IMAGE_COLOR(pDregs,i,color);
	}
    }
    else		/* PseudoColor */
    {
        for (i=0; i < nColormapEntries; i++)
        {
	    if (cmap->red[i].fShared)
	    {
	        red   = cmap->red[i].co.shco.red->color;
	        green = cmap->red[i].co.shco.green->color;
	        blue  = cmap->red[i].co.shco.blue->color;
	    }
	    else
	    {
	        red   = cmap->red[i].co.local.red;
	        green = cmap->red[i].co.local.green;
	        blue  = cmap->red[i].co.local.blue;
	    }

	    if (!pScreenPriv->isGrayScale)
	    {
	        color   = ( ((Card32) (red & 0xff00) << 8)
		        |    (Card32) (green & 0xff00)
		        |   ((Card32) (blue & 0xff00) >> 8)
		        );
	    }
	    else
	    {	/* Gray Scale */
	        CALCULATE_GRAYSCALE_INTENSITY(intensity,
		    (red >> 8),
		    (green >> 8),
		    (blue >> 8));
	        color = intensity | (intensity << 8) | (intensity << 16);
	    }

	    pScreenPriv->hwColors[i] = color;

	    WRITE_IMAGE_COLOR(pDregs,i,color);
        }
    }

    if (pScreenPriv->deviceID == S9000_ID_HCRX)
    {
	NgleLutBltCtl	lutBltCtl;

	lutBltCtl   = setHyperLutBltCtl(pScreenPriv->deviceID,
			pScreenPriv->devDepth,
			0,      /* Offset w/i LUT */
			256);   /* Load entire LUT */
	NGLE_BINC_SET_SRCADDR((Card32)
		NGLE_LONG_FB_ADDRESS(0, 0x100, 0)); /* 0x100 is same as used in
						     * WRITE_IMAGE_COLOR() */
	START_COLORMAPLOAD(lutBltCtl.all);
	SETUP_FB(pDregs, pScreenPriv->deviceID, pScreenPriv->devDepth);
    }
    else
    {
	/* cleanup colormap hardware */
	FINISH_IMAGE_COLORMAP_ACCESS(pDregs,pScreenPriv->deviceID,
				   pScreenPriv->devDepth);
    }

    pScreenPriv->installedMap = cmap;
    WalkTree(cmap->pScreen, TellGainedMap, (char *) &(cmap->mid));

}   /* ngleInstallColormap() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleStoreColors()
 *
 * Description: Sets the pixels in pdefs into the specified map.
 *
 *	If the colormap being updated has been loaded into the
 *	NGLE hardware, update there as well.
 *
 * Results:
 *      None
 *
 * Parameters:
 *	Colors in xColorItem *pDefs are Card16's (not Card8s);
 *	Must be shifted right 8 before loading into 8 bit DAC.
 *
 * Side Effects:
 *      Hardware updated if colormap loaded.
 *
 * Note on Gray Scale:
 *	Intensity calculation of using a RGB weighting of (.3,.59,.11)
 *	is the NTSC weighting formula.
 *
 ******************************************************************************/
 
void 
ngleStoreColors(
    ColormapPtr		pmap,
    int			ndef,
    xColorItem		*pdefs)
{
    Card32	 	index;
    NgleScreenPrivPtr 	pScreenPriv = NGLE_SCREEN_PRIV(pmap->pScreen);
    short		intensity;  /* gray scale */
    NgleHdwPtr          pDregs = pScreenPriv->pDregs;
    Card32		color;
    VisualPtr		pVisual = pmap->pVisual;
    int			i;


    /* We do not need to update HW if this is not the current map.
     */
    if (pmap != pScreenPriv->installedMap)
        return;

    /* setup colormap hardware */
    START_IMAGE_COLORMAP_ACCESS(pDregs);

    if (pVisual->class == DirectColor)
    {
	for (i = 0; i < ndef; i++)
	{
	    if ((pdefs+i)->flags & DoRed)
	    {
		index = ((pdefs+i)->pixel & pVisual->redMask)
					>> pVisual->offsetRed;
		color = pScreenPriv->hwColors[index];
		color = ((((pdefs+i)->red) << 8) & 0xff0000) | (color & 0xffff);
		pScreenPriv->hwColors[index] = color;

		WRITE_IMAGE_COLOR(pDregs, index, color);
	    }
	    if ((pdefs+i)->flags & DoGreen)
	    {
		index = ((pdefs+i)->pixel & pVisual->greenMask)
					>> pVisual->offsetGreen;
		color = pScreenPriv->hwColors[index];
		color = (((pdefs+i)->green) & 0xff00) | (color & 0xff00ff);
		pScreenPriv->hwColors[index] = color;

		WRITE_IMAGE_COLOR(pDregs, index, color);
	    }
	    if ((pdefs+i)->flags & DoBlue)
	    {
		index = ((pdefs+i)->pixel & pVisual->blueMask)
					>> pVisual->offsetBlue;
		color = pScreenPriv->hwColors[index];
		color = ((((pdefs+i)->blue) >> 8) & 0xff) | (color & 0xffff00);
		pScreenPriv->hwColors[index] = color;

		WRITE_IMAGE_COLOR(pDregs, index, color);
	    }
	}
    }
    else		/* PseudoColor */
    {
        while (ndef--)
        {
	    index = pdefs->pixel&0xff;

	    if (!pScreenPriv->isGrayScale)
	    {
	        color = pScreenPriv->hwColors[index];

	        if( pdefs->flags & DoRed) 
	        {
		    color = (((pdefs->red) << 8) & 0xff0000) | (color & 0xffff);
	        }
	        if( pdefs->flags & DoGreen) 
	        {
		    color = ((pdefs->green) & 0xff00) | (color & 0xff00ff);
	        }
	        if( pdefs->flags & DoBlue) 
	        {
		    color = (((pdefs->blue) >> 8) & 0xff) | (color & 0xffff00);
	        }
	    }
	    else
	    {	/* Gray Scale */
	        if ((pdefs->flags & DoRed)
	         ||  (pdefs->flags & DoGreen)
	         ||  (pdefs->flags & DoBlue))
	         {
		    CALCULATE_GRAYSCALE_INTENSITY(intensity,
		        ((pdefs->red) >> 8),
		        ((pdefs->green) >> 8),
		        ((pdefs->blue) >> 8));

		    color = intensity | (intensity << 8) | (intensity << 16);
	         }
	    }

	    pScreenPriv->hwColors[index] = color;

	    WRITE_IMAGE_COLOR(pDregs,index,color);

	    pdefs++;
        }
    }

    if (pScreenPriv->deviceID == S9000_ID_HCRX)
    {
	NgleLutBltCtl	lutBltCtl;

	lutBltCtl   = setHyperLutBltCtl(pScreenPriv->deviceID,
			pScreenPriv->devDepth,
			0,      /* Offset w/i LUT */
			256);   /* Load entire LUT */
	NGLE_BINC_SET_SRCADDR((Card32)
		NGLE_LONG_FB_ADDRESS(0, 0x100, 0)); /* 0x100 is same as used in
						     * WRITE_IMAGE_COLOR() */
	START_COLORMAPLOAD(lutBltCtl.all);
	SETUP_FB(pDregs, pScreenPriv->deviceID, pScreenPriv->devDepth);
    }
    else
    {
	/* cleanup colormap hardware */
	FINISH_IMAGE_COLORMAP_ACCESS(pDregs,pScreenPriv->deviceID,
				   pScreenPriv->devDepth);
    }

}   /* ngleStoreColors() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleListInstalledColormaps()
 *
 * Description: Fills in the list with the IDs of the installed maps.
 *
 * Results:
 *      Returns the number of IDs in the list
 *
 * Side Effects:
 *      None
 *
 ******************************************************************************/
 
int 
ngleListInstalledColormaps(
    ScreenPtr   pScreen,
    Colormap    *pCmapList)
{
    NgleScreenPrivPtr 	pScreenPriv = NGLE_SCREEN_PRIV(pScreen);

    *pCmapList = pScreenPriv->installedMap->mid;
    return (1);
}   /* ngleListInstalledColormaps() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleUninstallColormap()
 *
 * Description: If given colormap installed, uninstall it.
 *		If the colormap's visual is the default (root) visual,
 *		take care that _some_ colormap is installed:
 *		if cmap is default colormap, leave installed,
 *		else install default colormap.
 * Caveat:
 *	It is possible to receive a request to uninstall a colormap that 
 *	is not installed.  In this case, ngleUninstallColormap will mark
 *	the colormap as uninstalled and return correctly.
 *
 * Results:
 *      None
 *
 * Side Effects:
 *      All clients requesting ColormapNotify are notified
 *
 ******************************************************************************/

void 
ngleUninstallColormap( ColormapPtr cmap)
{
    NgleScreenPrivPtr 	pScreenPriv = NGLE_SCREEN_PRIV(cmap->pScreen);

    if (cmap == pScreenPriv->installedMap)
    {
	Colormap defMapID = cmap->pScreen->defColormap;

	/* Don't uninstall default colormap */
	if (cmap->mid != defMapID)
	{
	    ColormapPtr defMap = 
		(ColormapPtr) LookupIDByType(defMapID, RT_COLORMAP);
	    
	    if (defMap)
		ngleInstallColormap(defMap);
	    else
		ErrorF("ngleUninstallColormap: ",
		    "Can't find default colormap\n");
	}
    }
}   /* ngleUninstallColormap() */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleDestroyColormap()
 *
 * Description: Perform device-specific operations to destroy a colormap.
 *
 * Entry:  pmap points to the colormap of interest.
 *
 * Return Value: none
 *
 * Called by: DIX DestroyColormap routine
 *
 ******************************************************************************/

void 
ngleDestroyColormap(
    ColormapPtr pmap
)
{
    return;
}   /* ngleDestroyColormap */


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleResolvePseudoColor()
 *
 * Description: Normalize colors to device.
 *
 ******************************************************************************/

void 
ngleResolvePseudoColor(
    Card16      *pRed,
    Card16      *pGreen,
    Card16      *pBlue,
    VisualPtr   pVisual)
{
  unsigned short mask;
  unsigned short bits = pVisual->bitsPerRGBValue;
  unsigned short mult = 0xffff / ((1 << bits) - 1);
  unsigned short round = (1 << ( 16 - pVisual->bitsPerRGBValue))/ 2;


  /* 
   * Now we need to figure out how many bits/rgb this visual
   * (hardware) has and then normalize the request value to that.
   */

  mask = ((1 << bits) - 1) << (MAX_BITS_PER_RGB - bits);
  *pRed = ((*pRed) - (*pRed >> pVisual->bitsPerRGBValue)) + round;
  *pGreen = ((*pGreen) - (*pGreen >> pVisual->bitsPerRGBValue)) + round;
  *pBlue = ((*pBlue) - (*pBlue >> pVisual->bitsPerRGBValue)) + round;

  *pRed = mult * ((((*pRed) >> MAX_BITS_PER_RGB ) & mask) >> 
					(MAX_BITS_PER_RGB - bits));
  *pGreen = mult * ((((*pGreen) >> MAX_BITS_PER_RGB ) & mask) >> 
					(MAX_BITS_PER_RGB - bits));
  *pBlue = mult * ((((*pBlue) >> MAX_BITS_PER_RGB ) & mask) >> 
					(MAX_BITS_PER_RGB - bits));

  return;

} /* ngleResolvePseudoColor() */


NgleLutBltCtl setNgleLutBltCtl(
    Card32              deviceID,
    Int32		devDepth,
    Card32              offsetWithinLut,
    Card32              length)             /* #entries to update */
{
    NgleLutBltCtl       lutBltCtl;

    /* set enable, zero reserved fields */
    lutBltCtl.all           = 0x80000000;

    lutBltCtl.fields.length = length;

    if (deviceID == S9000_ID_A1439A) /* CRX24 */
    {
        if (devDepth == 8)
        {
            lutBltCtl.fields.lutType    = NGLE_CMAP_OVERLAY_TYPE;
            lutBltCtl.fields.lutOffset  = 0;
        }
        else
        {
            lutBltCtl.fields.lutType    = NGLE_CMAP_INDEXED0_TYPE;
            lutBltCtl.fields.lutOffset  = 0 * 256;
        }
    }
    else if (deviceID == S9000_ID_ARTIST)
    {
        lutBltCtl.fields.lutType    = NGLE_CMAP_INDEXED0_TYPE;
        lutBltCtl.fields.lutOffset  = 0 * 256;
    }
    else
    {
        lutBltCtl.fields.lutType   = NGLE_CMAP_INDEXED0_TYPE;
        lutBltCtl.fields.lutOffset = 0;
    }

    /* Offset points to start of LUT.  Adjust for within LUT */
    lutBltCtl.fields.lutOffset += offsetWithinLut;

    return(lutBltCtl);

}   /* setNgleLutBltCtl() */

NgleLutBltCtl setHyperLutBltCtl(
    Card32              deviceID,
    Int32		devDepth,
    Card32              offsetWithinLut,
    Card32              length)             /* #entries to update */
{
    NgleLutBltCtl       lutBltCtl;

    /* set enable, zero reserved fields */
    lutBltCtl.all           = 0x80000000;

    lutBltCtl.fields.length = length;
    lutBltCtl.fields.lutType    = HYPER_CMAP_TYPE;

    /* Expect lutIndex to be 0 or 1 for image cmaps, 2 or 3 for overlay cmaps*/
    if (devDepth == 8)
	lutBltCtl.fields.lutOffset  = 2 * 256;
    else
	lutBltCtl.fields.lutOffset  = 0 * 256;

    /* Offset points to start of LUT.  Adjust for within LUT */
    lutBltCtl.fields.lutOffset += offsetWithinLut;

    return(lutBltCtl);

}   /* setHyperLutBltCtl() */
