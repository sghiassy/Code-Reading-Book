/* $XConsortium: ngleblt.c,v 1.1 93/08/08 12:56:35 rws Exp $ */

/*
****************************************************************************
** DDX WINDOW ROUTINES
**
** (c) Copyright Hewlett-Packard Company, 1992.

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
**
****************************************************************************
*/

#include "ngle.h"


/******************************************************************************
 *
 * NGLE DDX Procedure:          ngleBitBlt
 *
 * Description:
 *
 *	This is HP's version of the cfbDoBitBltCopy() routine.  This enables
 *	fast scrolling.
 *
 ******************************************************************************/
void ngleBitBlt (

    DrawablePtr		pSrcDraw,
    DrawablePtr		pDstDraw,
    int			alu,
    RegionPtr		prgnDst,
    DDXPointPtr		pptSrc,
    unsigned long	planeMask)
{
    BoxPtr		pbox;
    int			nbox;
    NgleScreenPrivPtr	pScreenPriv;
    NgleHdwPtr		pDregs;

    /* get pointers so HP's private structures */
    pScreenPriv = NGLE_SCREEN_PRIV(pDstDraw->pScreen);
    pDregs = (NgleHdwPtr) pScreenPriv->pDregs;


    /* get a pointer and count of the boxes */
    pbox = REGION_RECTS(prgnDst);
    nbox = REGION_NUM_RECTS(prgnDst);

    /* call the hardware blt routine for each box, but first set up the
     * hardware
     */
    SETUP_COPYAREA(pDregs);

    if (pScreenPriv->devDepth == 24)
    /* do the if statement at this level for performance reasons */
    {
	while (nbox--)
	{
	    ngleDepth24_CopyAreaFromToScreen(pSrcDraw, pDstDraw,
		pptSrc->x, pptSrc->y, pbox, alu, planeMask);

	    pbox++;
	    pptSrc++;
	}
    }
    else
    {
	while (nbox--)
	{
	    ngleDepth8_CopyAreaFromToScreen(pSrcDraw, pDstDraw,
		pptSrc->x, pptSrc->y, pbox, alu, planeMask);

	    pbox++;
	    pptSrc++;
	}
    }

    /* Now restore the hardware state so the CFB driver can use it */
    SETUP_FB(pDregs, pScreenPriv->deviceID,pScreenPriv->devDepth);
}
