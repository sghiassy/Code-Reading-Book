/*
 * $XConsortium: skyFill.c,v 1.3 92/01/27 18:03:17 eswu Exp $
 *
 * Copyright IBM Corporation 1991 
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
 * Author: Eng-Shien Wu
 */


/* Called by rendering routines to setup fillStyle on coprocessor.
 * Returns PixelOp bits that need to be set to handle the fillStyle.
 *
 *     ulong skySetTile   (scrnNum, pGC, &patWidth, &patHeight)
 *     ulong skySetStipple(scrnNum, pGC, &patWidth, &patHeight)
 *     ulong skySetSolid(scrnNum, pGC)
 *
 */

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "mergerop.h"

#include "ibmTrace.h"
#include "skyHdwr.h"
#include "skyReg.h"
#include "skyPriv.h"


/* Called by skyValidateGC when the tile or stipple has changed in the GC.
 * This clears the id currently being used and gives out a new id so that
 * when you go to render using the tile or stipple, it will be set into
 * the off-screen vram.
 */

void skyNewTileStipID(scrnNum, tileStipID)
    int             scrnNum;
    ulong          *tileStipID;
{
    ulong          *IDList = SKY_TILESTIP[scrnNum];

    /* Clear all entries of old tileStipID */

    if (*tileStipID)
    {
	int n;

	for (n=0; n<SKY_TILESTIP_AREAS; n++)
	{
	    if (IDList[n] == *tileStipID)
		IDList[n] = 0;
	}
    }

    /* Assign new tileStipID */
    *tileStipID = ++SKY_TILESTIPID_CTR[scrnNum];

    TRACE(("new tileStippleID = 0x%x\n", *tileStipID));
}


/* Called by skySetTile and skySetStipple to see if the tile/stipple
 * is already in the off-screen vram area and where to put it if it is not.
 *
 * Looks into table to see if the tile or stipple is already in off-screen
 * vram. If so, return true and set *base to the address of the tile/stipple
 * in vram. If not, return false and set *base to the address where the
 * tile/stipple can be put.
 *
 * The code is crufty.
 */

#define HashEntry(tileStipID)	(tileStipID % SKY_TILESTIP_AREAS)

Bool
skyFindTileStip(scrnNum, tileStipID, size, base)
    int             scrnNum;
    ulong           tileStipID;
    ulong           size;
    ulong          *base;
{
    ulong          *IDList = SKY_TILESTIP[scrnNum];
    ulong           id;
    uint             entry, n;
    Bool            foundSlot;

    TRACE(("skyFindTileStip(s=%d, tileStipID=0x%x, size=%d, base=0x%x)\n",
	scrnNum, tileStipID, size, base));


    /* Search for the id or a free slot. */

    entry = HashEntry(tileStipID);

    for (n=SKY_TILESTIP_AREAS; n--; entry=(entry+1) % SKY_TILESTIP_AREAS)
    {
	id = IDList[entry];

	TRACE(("\tIDList[%d]=0x%x\n", entry, id));

	foundSlot = (id==tileStipID || id == 0);

	if (foundSlot)
	    break;
    }

    /* If for some reason, we did not find the id or a free slot,
     * use the first slot we looked at.
     */

    if (! foundSlot)
	entry = HashEntry(tileStipID);		/* Use first entry searched */

    *base = SKY_INVIS_VRAM_BASE(scrnNum) + entry*SKY_MAX_TILESTIP_SIZE;

    TRACE(("\tentry=%d, base=0x%x, install=%d\n",
	entry, base, id==tileStipID && id != 0));

    IDList[entry] = tileStipID;

#ifndef NO_CACHING_TEST
    return (id==tileStipID && id != 0);
#else
    return (0);
#endif
}


/* Only handles regular tile case:
 *     tileIsPixel is handled by solid fill.
 *     two-color tiles handled by stipple fill.
 */

ulong
skySetTile(scrnNum, pGC, patWidth, patHeight)
    int             scrnNum;
    GCPtr	    pGC;
    int            *patWidth;
    int            *patHeight;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);

    int             w = *patWidth  = pGC->tile.pixmap->drawable.width;
    int             h = *patHeight = pGC->tile.pixmap->drawable.height;
    int             size = pGC->tile.pixmap->devKind * h;

    ulong           base;
    Bool            installed;


    TRACE(("skySetTile(scrnNum=%d, pGC=0x%x, &W=0x%x, &H=0x%x)\n",
		scrnNum, pGC, patWidth, patHeight));

    /* See if tile already installed. If installed, ``base'' points to
     * where the tile is installed. If not, ``base'' points to where
     * you should install the tile.
     */

    installed = skyFindTileStip(scrnNum, pSkyPriv->tileStipID, size, &base);
    SKYSetPixmap(scrnNum, PixMapA, base, w,h, PixSize8);

    if (! installed)
    {
	char *pdst = (char *) (SKY_SEGMENT[scrnNum] + base);
	char *psrc = (char *) pGC->tile.pixmap->devPrivate.ptr;

	memcpy(pdst, psrc, size);	/* copy tile to invisible VRAM */

	if (w & PIM)
	{
	    /* Pack tile to remove scanline pad. Skyway uses no pad. */

	    SKYSetPixmap(scrnNum, PixMapB, base,
		pGC->tile.pixmap->devKind, h, PixSize8);

	    SKYSetPlaneMask(scrnNum, ~0);
	    SKYSetALU(scrnNum, GXcopy);

	    SKYSetOpDim21(scrnNum,w,h);
	    SKYSetSrcXY_PatXY_DstXY_PixelOp(scrnNum,
		0,0, 0,0, 0,0,
		POStepBlt | POMaskDis | POOct0 |
		POForeSrc | POSrcB | PODestA | POPatFore);

	    SKYBusyWait(scrnNum);
	}
    }

    SKYSetPlaneMask(scrnNum, pGC->planemask);	/* Set planemask */
    SKYSetMix(scrnNum, pSkyPriv->BgFgMix);	/* Set ALU       */

    return (POForeSrc | POSrcA | POPatFore | PODestC | POMaskDis);
}


ulong
skySetStipple(scrnNum, pGC, patWidth, patHeight)
    int             scrnNum;
    GCPtr	    pGC;
    int            *patWidth;
    int            *patHeight;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    skyStipplePtr   stipple = &pSkyPriv->stipple;

    int             w = *patWidth  = stipple->width;
    int             h = *patHeight = stipple->height;
    ulong           base;

    ulong           PixelOp = 0;
    Bool            installed;


    TRACE(("skySetStipple(scrnNum=%d, pGC=0x%x, &W=0x%x, &H=0x%x)\n",
		scrnNum, pGC, patWidth, patHeight));

    installed = skyFindTileStip(scrnNum, pSkyPriv->tileStipID,
				stipple->size, &base);

    SKYSetPixmap(scrnNum, PixMapA, base, w,h, PixSize1);

    if (!installed)
    {
	char *pdst = (char *) (SKY_SEGMENT[scrnNum] + base);

	/* Set memory mode to Big-Endian, 1-bit per pixel mode */
	SKY_MEM_REG(scrnNum) = PixSize1;

	memcpy(pdst, stipple->bits, stipple->size);

	/* Set memory mode to Big-Endian, 8-bit per pixel mode */
	SKY_MEM_REG(scrnNum) = PixSize8;

	if (! stipple->packed)
	{
	    /* Pack stipple to remove scanline pad,
	     * because skyway uses no pad.
	     */

	    SKYSetPixmap(scrnNum, PixMapB, base,
		pGC->stipple->devKind << 3, h, PixSize1);

	    SKYSetPlaneMask(scrnNum, ~0);
	    SKYSetALU(scrnNum, GXcopy);

	    SKYSetOpDim21(scrnNum,w,h);
	    SKYSetSrcXY_PatXY_DstXY_PixelOp(scrnNum,
		0,0, 0,0, 0,0,
		POStepBlt | POMaskDis | POOct0 |
		POForeSrc | POSrcB | PODestA | POPatFore);

	    SKYBusyWait(scrnNum);
	}
    }

    SKYSetPlaneMask(scrnNum, pGC->planemask);	/* Set planemask */
    SKYSetMix(scrnNum, pSkyPriv->BgFgMix);	/* Set ALU       */

    if (pGC->fillStyle == FillStippled)
    {
	PixelOp = POForeReg | POPatA;
	SKYSetFgColor(scrnNum, stipple->fgPixel);
    }
    else /* FillOpaqueStippled || degenerate FillTiled */
    {
	PixelOp = POForeReg | POBackReg | POPatA;
	SKYSetFgBgColor(scrnNum, stipple->fgPixel, stipple->bgPixel);
    }

    return (PixelOp | PODestC | POMaskDis);
}


ulong
skySetSolid(scrnNum, pGC)
    int             scrnNum;
    GCPtr	    pGC;
{
    skyPrivGCPtr    pSkyPriv = skyGetGCPrivate(pGC);
    uint            fgPixel;

    TRACE(("skySetSolid(scrnNum=%d, pGC=0x%x)\n", scrnNum, pGC));

    SKYSetPlaneMask(scrnNum, pGC->planemask);	/* Set planemask */
    SKYSetMix(scrnNum, pSkyPriv->BgFgMix);	/* Set ALU       */

    fgPixel = (pGC->fillStyle == FillTiled) ? pGC->tile.pixel : pGC->fgPixel;
    SKYSetFgColor(scrnNum, fgPixel);
	
    return (POForeReg | POPatFore | PODestC | POMaskDis);
}

