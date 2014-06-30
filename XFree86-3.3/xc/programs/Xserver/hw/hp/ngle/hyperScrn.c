/* $XConsortium: hyperScrn.c,v 1.1 95/01/25 16:14:36 gildea Exp $ */
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
 *	This file contains various global variables and routines concerning
 *	the Screen structure.  This includes the ngleScreenInit routine.
 *
 ******************************************************************************/


#include "ngle.h"

Bool hyperResetPlanes(
    NgleScreenPrivPtr           pScreenPriv,
    Card32                      serverState);

static void hyperUndoITE(
    NgleScreenPrivPtr       pScreenPriv);


/******************************************************************************
 *
 * NGLE DDX Utility Procedure:	hyperResetPlanes
 *
 * Description:
 *
 *	This routine implements pNgleScreenInit->InitPlanes and
 *	pScreenPriv->ngleResetPlanes.  It resets the image, overlay 
 *	and attribute planes to a known state.  This includes doing 
 *	what is necessary to counteract what the ITE has done as well 
 *	as supporting friendly boot operation.
 *
 * Assumptions:
 *
 *	- default colormap has already been initialized and installed
 *	- fast-locking has been initialized.
 *
 *	Does not assume a lock is in effect.
 *
 ******************************************************************************/

Bool hyperResetPlanes(
    NgleScreenPrivPtr	pScreenPriv,
    Card32		serverState)
{
    NgleHdwPtr		pDregs 		= (NgleHdwPtr) pScreenPriv->pDregs;
    Card32		controlPlaneReg;
    Card32		planeEnableMask;


    NGLE_LOCK(pScreenPriv);

    if (IS_24_DEVICE(pScreenPriv))
	if (pScreenPriv->devDepth == 24)
	    controlPlaneReg = 0x04000F00;
	else
	    controlPlaneReg = 0x00000F00;   /* 0x00000800 should be enought, but lets clear all 4 bits */
    else
	controlPlaneReg = 0x00000F00;	    /* 0x00000100 should be enought, but lets clear all 4 bits */

    switch(serverState)
    {
    case SERVER_INIT:
	/**************************************************
	 ** Need to clear screen 
	 **************************************************/
	if (IS_24_DEVICE(pScreenPriv))
	    ngleDepth24_ClearImagePlanes(pScreenPriv);
	else
	    ngleDepth8_ClearImagePlanes(pScreenPriv);

	/* Paint attribute planes for default case.
	 * On Hyperdrive, this means all windows using overlay cmap 0.
	 */
	ngleResetAttrPlanes(pScreenPriv, controlPlaneReg);

    	/* Clear overlay planes:
	 *
	 * If on Hyperdrive and doing Friendly Boot, we want a smooth 
	 * transition when VUE starts up.  There are 3 cases to worry
	 * about: 1) If only the friendly boot slate is visible on the 
	 * screen and ITE console messages are not, then we need to clear 
	 * the text planes so that the ITE messages don't flash on screen.  
	 * This will create a smooth transition when the vuelogin window 
	 * appears.  2) If only ITE console messages are visible and the
	 * friendly boot slate is not, then we need to clear the planes
	 * containing the slate.  3) If both the friendly boot slate and
	 * ITE console messages are visible, then we don't need to bother
	 * clearing any of the overlay planes.
	 *
	 * Side note:  The above is correct if the default visual is in
	 * the overlays.  If the default visual is in the image planes,
	 * then clear all of the overlay planes to prevent color flashing
	 * of slate which will happen when smooth cmap is installed in
	 * overlays.
	 *
	 * STI uses the upper 5 overlay planes for the friendly boot slate 
	 * and the lower 3 overlay planes for ITE console messages.  These
	 * planes are enabled/disabled via the FDR register on Marathon.
	 */

	if ( pScreenPriv->myNum == 0 && pScreenPriv->devDepth == 8 )
	{   /* isDefaultVisualInOverlays == TRUE */
	    SETUP_HW(pDregs); /* Wait for hw to be ready */
	    planeEnableMask = NGLE_READ32(pDregs->reg32);

	    if ((planeEnableMask & 0xffff0000) == 0xf8f80000)
	    {
		/* only slate is visible so need to clear text planes */
		ngleClearOverlayPlanes(pScreenPriv, 0x7, 0);
	    }
	    else if ((planeEnableMask & 0xffff0000) == 0x07070000)
	    {
		/* only text is visible so need to clear slate planes */
		ngleClearOverlayPlanes(pScreenPriv, 0xf8, 0);
	    }
	    /* else if ((planeEnableMask & 0xffff0000) == 0xffff0000) */
		/* both slate and text are visible so clear isn't necessary */
	}
	else
	{
	    /* Not in friendly boot mode and/or default visual is 
	     * in image planes so clear all overlay planes 
	     */
	    ngleClearOverlayPlanes(pScreenPriv, 0xff, 255);
	}

	/**************************************************
	 ** Also need to counteract ITE settings 
	 **************************************************/
	hyperUndoITE(pScreenPriv);
	break;

    case SERVER_EXIT:
	/**************************************************
	 ** Need to clear screen 
	 **************************************************/
	if (IS_24_DEVICE(pScreenPriv))
	    ngleDepth24_ClearImagePlanes(pScreenPriv);
	else
	    ngleDepth8_ClearImagePlanes(pScreenPriv);
	ngleResetAttrPlanes(pScreenPriv, controlPlaneReg);
	ngleClearOverlayPlanes(pScreenPriv, 0xff, 0);
	break;

    case SERVER_RECOVERY:
	/**************************************************
	 ** Need to counteract ITE settings 
	 **************************************************/
	hyperUndoITE(pScreenPriv);

	/**************************************************
	 * Reset attribute planes to known state 
	 **************************************************/
	ngleResetAttrPlanes(pScreenPriv, controlPlaneReg);
	break;
    }

    NGLE_UNLOCK(pScreenPriv);

    return(TRUE);

} /* hyperResetPlanes */


/******************************************************************************
 *
 * NGLE DDX Utility Procedure:	hyperUndoITE
 *
 * Description:
 *
 *	This local routine counteracts what the ITE has done on
 *	Hyperdrive.
 *
 * 	STI uses the upper 5 overlay planes for the friendly boot slate 
 * 	and the lower 3 overlay planes for ITE console messages.  These
 * 	planes are enabled/disabled via the FDR register on Marathon.
 *
 * 	ITE has set transparency enable mask to a non-zero
 * 	value.  Make sure that it's set to zero.  Method is per
 * 	Curtis McAllister.
 *
 * 	Before clearing the transparency, write zeroes to the overlay
 * 	planes (to avoid flashing display of colors that are no longer
 * 	transparent).
 *
 * Assumptions:
 *
 *	Assumes fast-locking has been initialized.
 *	Does not assume a lock is in effect.
 *
 ******************************************************************************/

static void hyperUndoITE(
    NgleScreenPrivPtr       pScreenPriv)
{
    NgleHdwPtr              pDregs;
    Int32		    nFreeFifoSlots = 0;
    Card32		    fbAddr;

    pDregs 	= (NgleHdwPtr) pScreenPriv->pDregs;

    NGLE_LOCK(pScreenPriv);


    /**********************************************
     * Display enable all overlay planes
     **********************************************/

    /*
     * On Hyperdrive, plane enable done via FDR register on Marathon.
     * Enable all planes (image and overlay).
     */
    GET_FIFO_SLOTS(nFreeFifoSlots,1);
    NGLE_WRITE32(pDregs->reg32,0xffffffff);


    /**********************************************
     * Write overlay transparency mask so only entry 255 is transparent
     **********************************************/

    /* Hardware setup for full-depth write to "magic" location */
    GET_FIFO_SLOTS(nFreeFifoSlots, 7);
    NGLE_QUICK_SET_DST_BM_ACCESS(
	BA(IndexedDcd, Otc04, Ots08, AddrLong,
	   BAJustPoint(0), BINovly, BAIndexBase(0)));
    NGLE_QUICK_SET_IMAGE_BITMAP_OP(
	IBOvals(RopSrc, MaskAddrOffset(0),
		BitmapExtent08, StaticReg(FALSE),
		DataDynamic, MaskOtc, BGx(FALSE), FGx(FALSE)));

    /* Now prepare to write to the "magic" location */
    fbAddr = (Card32) NGLE_LONG_FB_ADDRESS(0, 1532, 0);
    NGLE_BINC_SET_DSTADDR(fbAddr);
    NGLE_REALLY_SET_IMAGE_PLANEMASK(0xffffff);
    NGLE_BINC_SET_DSTMASK(~0UL);

    /* Finally, write a zero to clear the mask */
    NGLE_BINC_WRITE32(0);

    NGLE_UNLOCK(pScreenPriv);

} /* hyperUndoITE */
