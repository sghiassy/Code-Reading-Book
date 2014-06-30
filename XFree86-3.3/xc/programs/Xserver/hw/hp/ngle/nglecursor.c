/* $XConsortium: nglecursor.c,v 1.3 95/01/24 02:05:50 dpw Exp $ */

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
 *	This file contains various procedures that implement cursor
 *	functionality (for both sprites and echoes).
 *
 ******************************************************************************/

#include "ngle.h"

extern WindowPtr    *WindowTable;
extern int	    hpActiveScreen;		/* input driver global */

/* Defined in nglecolormap.c: */
extern void ngleLoadCursorColormap(
    ScreenPtr	pScreen,
    CursorPtr	pCursor);



/******************************************************************************
 *
 * NGLE DDX Procedure:		hyperInitSprite()
 *
 * Description:
 *
 *	This routine is called at ScreenInit() time to initialize various
 *	cursor/sprite structures and the NGLE sprite code.
 *
 ******************************************************************************/

Bool hyperInitSprite(
    ScreenPtr		pScreen)
{
    NgleScreenPrivPtr	pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    hpPrivPtr		php 	    = (hpPrivPtr)pScreen->devPrivate;
    NgleHdwPtr          pDregs 	    = pScreenPriv->pDregs;
    Int32		nScanlinesToZero;
    Int32		curaddr;
    Int32		nFreeFifoSlots = 0;

    NGLE_LOCK(pScreenPriv);

    /* Initialize the pScreen Cursor Procedure Pointers: */
    pScreen->PointerNonInterestBox  = ngleNoop;
    pScreen->CursorLimits 	    = hpCursorLimits;
    pScreen->ConstrainCursor 	    = hpConstrainCursor;
    pScreen->DisplayCursor 	    = hyperDisplayCursor;
    pScreen->SetCursorPosition 	    = hpSetCursorPosition;
    pScreen->RealizeCursor 	    = ngleNoopTrue;
    pScreen->UnrealizeCursor 	    = ngleNoopTrue;
    pScreen->RecolorCursor 	    = ngleRecolorCursor;


    /* Initialize the Cursor Procedure Pointers: */
    php->MoveMouse 	= ngleMoveSprite;
    php->ChangeScreen	= ngleChangeScreen;
    php->CursorOff	= ngleDisableSprite;

    /* Initialize the pScreenPriv->sprite Structure: */
    pScreenPriv->sprite.visible = FALSE;
    pScreenPriv->sprite.x 	= (pScreenPriv->screenWidth >> 1);
    pScreenPriv->sprite.y 	= (pScreenPriv->screenHeight >> 1);
    pScreenPriv->sprite.width 	= 0;
    pScreenPriv->sprite.height 	= 0;
    pScreenPriv->sprite.pCursor = NULL;
    pScreenPriv->sprite.firstCallTo_DisplayCursor = TRUE;
    pScreenPriv->sprite.nTopPadLines = 0;
    pScreenPriv->sprite.moveOnVBlank = FALSE;

    /* Zero out cursor mask and data:
     * Each load of cursor data/mask will write minimum necessary
     * to overwrite previous cursor.
     */
    /* Zero out 64 bits (2 longs) in each mask scanline */
    nScanlinesToZero = NGLE_MAX_SPRITE_SIZE;

    /* to protect against VID bus data loss */
    SETUP_HW(pDregs);

    GET_FIFO_SLOTS(nFreeFifoSlots,2);
    HYPER_GET_VID_BUS_ACCESS;

    /* Write to mask area */
    HYPER_SET_CURSOR_ADDRESS(0);
    do
        {
	GET_FIFO_SLOTS(nFreeFifoSlots, 2);
	HYPER_WRITE_CURSOR_DATA(0);
	HYPER_WRITE_CURSOR_DATA(0);
        } while (--nScanlinesToZero);

    /* Zero out 64 bits (2 longs) in each data scanline */
    nScanlinesToZero = NGLE_MAX_SPRITE_SIZE;
    GET_FIFO_SLOTS(nFreeFifoSlots, 1);
    HYPER_SET_CURSOR_ADDRESS(HYPER_CURSOR_DATA_BIT);
    do
        {
	GET_FIFO_SLOTS(nFreeFifoSlots, 2);
	HYPER_WRITE_CURSOR_DATA(0);
	HYPER_WRITE_CURSOR_DATA(0);
        } while (--nScanlinesToZero);

    NGLE_UNLOCK(pScreenPriv);

    return(TRUE);

} /* hyperInitSprite() */


/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleInitSprite()
 *
 * Description:
 *
 *	This routine is called at ScreenInit() time to initialize various
 *	cursor/sprite structures and the NGLE sprite code.
 *
 ******************************************************************************/

Bool ngleInitSprite(pScreen)


    ScreenPtr		pScreen;
{
    NgleScreenPrivPtr	pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    hpPrivPtr		php 	    = (hpPrivPtr)pScreen->devPrivate;
    NgleHdwPtr          pDregs      = pScreenPriv->pDregs;
    NgleDevRomDataPtr   pDevRomData = pScreenPriv->pDevRomData;
    Int32		nScanlinesToZero;


    /* Initialize the pScreen Cursor Procedure Pointers: */
    pScreen->PointerNonInterestBox  = ngleNoop;
    pScreen->CursorLimits 	    = hpCursorLimits;
    pScreen->ConstrainCursor 	    = hpConstrainCursor;
    pScreen->SetCursorPosition 	    = hpSetCursorPosition;
    pScreen->DisplayCursor 	    = ngleDisplayCursor;
    pScreen->RealizeCursor 	    = ngleNoopTrue;
    pScreen->UnrealizeCursor 	    = ngleNoopTrue;
    pScreen->RecolorCursor 	    = ngleRecolorCursor;


    /* Initialize the HP private Cursor Procedure Pointers: */
    php->MoveMouse 	= ngleMoveSprite;
    php->ChangeScreen	= ngleChangeScreen;
    php->CursorOff	= ngleDisableSprite;

    /* Initialize the pScreenPriv->sprite Structure: */
    pScreenPriv->sprite.visible = FALSE;
    pScreenPriv->sprite.x 	= (pScreenPriv->screenWidth >> 1);
    pScreenPriv->sprite.y 	= (pScreenPriv->screenHeight >> 1);
    pScreenPriv->sprite.width 	= 0;
    pScreenPriv->sprite.height 	= 0;
    pScreenPriv->sprite.pCursor = NULL;
    pScreenPriv->sprite.firstCallTo_DisplayCursor = TRUE;
    pScreenPriv->sprite.nTopPadLines = 0;
    pScreenPriv->sprite.moveOnVBlank = FALSE;
    GET_CURSOR_SPECS(pDregs,pScreenPriv);
    pScreenPriv->sprite.pipelineDelay   = pDevRomData->cursor_pipeline_delay;
    pScreenPriv->sprite.videoInterleave = pDevRomData->video_interleaves;
    if (pScreenPriv->deviceID == S9000_ID_A1439A)
    {
	pScreenPriv->sprite.pipelineDelay--;
    }

    /* Zero out 64 bits (2 longs) in each scanline */
    START_CURSOR_MASK_ACCESS(pDregs);
    nScanlinesToZero = NGLE_MAX_SPRITE_SIZE;
    do
    {
	WRITE_CURSOR_BITS(pDregs,0,0);
    } while (--nScanlinesToZero);

    /* Zero out 64 bits (2 longs) in each scanline */
    START_CURSOR_DATA_ACCESS(pDregs);
    nScanlinesToZero = NGLE_MAX_SPRITE_SIZE;
    do
    {
	WRITE_CURSOR_BITS(pDregs,0,0);
    } while (--nScanlinesToZero);

    SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

    return(TRUE);

} /* ngleInitSprite() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleUninitSprite()
 *
 * Description:
 *
 *	This routine is called at CloseScreen() time to free various
 *	cursor/sprite structures and shutdown the NGLE sprite code.
 *
 ******************************************************************************/

Bool ngleUninitSprite(pScreen)

    ScreenPtr		pScreen;
{
    hpPrivPtr		php;

    php = (hpPrivPtr)pScreen->devPrivate;
    php->MoveMouse    = ngleNoop;
    php->CursorOff    = ngleNoop;
    php->ChangeScreen = ngleNoop;

    return(TRUE);

} /* ngleUninitSprite() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleDisableSprite()
 *
 * Description:
 *
 *	This routine is called from the input driver to disable
 *	a screen's sprite (make it invisible) when the cursor/sprite moves
 *	to another screen when the server is in a multi-headed configuration.
 *	The sprite is re-enabled for a screen by a call to DisplayCursor().
 *
 ******************************************************************************/

void ngleDisableSprite(pScreen)

    ScreenPtr	pScreen;
{
    NgleScreenPrivPtr	pScreenPriv = NGLE_SCREEN_PRIV(pScreen);


    if (pScreenPriv->sprite.visible)
    {
	pScreenPriv->sprite.visible == FALSE;
	DISABLE_CURSOR(pScreenPriv);
    }

} /* ngleDisableSprite() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleChangeScreen()
 *
 * Description:
 *
 *	This routine is intended to handle changing the current screen.
 *	pScreen points to new screen.
 *
 ******************************************************************************/

void ngleChangeScreen(pScreen)

    ScreenPtr		pScreen;
{
    NgleScreenPrivPtr   pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    WindowPtr           pRootWindow = WindowTable[pScreen->myNum];

    if (pScreenPriv->sprite.pCursor == NULL)
    {
	/* This should only occur once and only for screens 1 and above.
	 * Reason: server calls DisplayCursor during initialization,
	 * but before rootwindow (and default cursor) have been defined.
	 * So DisplayCursor exited at very top - before firstCall is set false.
	 */

	/* Do load and enable cursor. */
	(*pScreen->DisplayCursor)(pScreen, pRootWindow->optional->cursor);
    }

} /* ngleChangeScreen() */


/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleDisplayCursor()
 *
 * Description:
 *
 *	This routine is called by DIX (and the input driver)
 *	whenever the sprite/cursor is changed 1) from one "cursor" (i.e. image
 *	and mask bits, foreground and background color) to another 2) from one
 *	screen to another.  This routine downloads the cursor image/mask and
 *	foreground and background colors to the hardware cursor.
 *
 ******************************************************************************/

Bool ngleDisplayCursor(pScreen, pCursor)

    ScreenPtr		pScreen;
    CursorPtr		pCursor;
{
    NgleScreenPrivPtr	pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    hpPrivPtr		php = (hpPrivPtr)pScreen->devPrivate;
    WindowPtr		pRootWindow;
    NgleHdwPtr		pDregs;
    Card32		*pSrc;
    Int32		nScanlines; /* loop variable */
    Int32		heightPlus1;
    Int32		sourceIncrement;
    Int32		nScanlinesToZeroPlus1;
    Int32		nOffscreenScanlines = 0;
    Int32		x0, y0;	    /* Sprite origin (not hotspot) */
    Int32		xHi,	    /* For cursor positioning */
			xLo;
    Int32		i;
    Card32		srcMask;
    Card32		cursorXYValue = 0;  
    int 		nTopPadLines = 0;
    int 		nBotPadLines = 0;
    Int32		hbp_times_vi;



    /*
    ***************************************************************************
    ** We are called during initialization as a side effect of doing a change
    ** screens to insure screen 0 is on top.  At that time, there is no root
    ** window so we should just abort and wait for a call that has a cursor
    ** to display.
    **
    ** After the first time this routine is called (i.e. after the "side effect"
    ** call), if pCursor is NULL it means that the input driver is
    ** changing screens and wants this screen to use the same "cursor" (i.e.
    ** image and mask bits, etc.) as the last time that the sprite was on
    ** this screen.  However, the second time that pScreen->DisplayCursor is
    ** called, pCursor is NULL, but since there wasn't a previous cursor,
    ** the driver should use the Root Window's cursor (which is the side
    ** effect that the input driver and/or DIX wants to acheive).
    ***************************************************************************
    */
    if (!(pRootWindow = WindowTable[pScreen->myNum]))
	return(TRUE);

    if (pScreenPriv->sprite.firstCallTo_DisplayCursor)
    {
	pScreenPriv->sprite.firstCallTo_DisplayCursor = FALSE;

	if (pCursor == NULL)
	    pCursor = pRootWindow->optional->cursor;
    }

    /* Now if the cursor is still null, abort because we are using the
     * same cursor as before:
     */
    if (pCursor == NULL)
	return(FALSE);

    /* Check for a valid sized cursor: */
    if ((pCursor->bits->width  > NGLE_MAX_SPRITE_SIZE) ||
        (pCursor->bits->height > NGLE_MAX_SPRITE_SIZE))
        return(FALSE);  /* Cursor is TOO big */

    if (pScreenPriv->sprite.visible == TRUE)
    {
	/**** Before loading the new image, mask, and colors into the hardware
	 **** cursor, turn it off so we don't get flicker:
	 ****/
	 DISABLE_CURSOR(pScreenPriv);
    }

    if (    (pScreenPriv->deviceID == S9000_ID_TIMBER)
	||  (pScreenPriv->deviceID == S9000_ID_A1439A))
    {
	int nPadLines = NGLE_MAX_SPRITE_SIZE - pCursor->bits->height;
	if (nPadLines >= NTOPLINES_FOR_TIMBER)
	{
	    nPadLines       -= NTOPLINES_FOR_TIMBER;
	    /* Split extra lines between top and bottom */
	    nBotPadLines = nPadLines/2;
	    nTopPadLines = NTOPLINES_FOR_TIMBER + nPadLines - nBotPadLines;
	}
	else
	{
	    nTopPadLines = nPadLines;
	}
    }
    else
    {   /* ELK */
	nTopPadLines = 0;
	nBotPadLines = NGLE_MAX_SPRITE_SIZE - pCursor->bits->height;
    }

    /* Calculate cursor origin (not hotspot) and number of scanlines not
     * actually displayed, including any padding scanlines at top.
     */
    x0 = pScreenPriv->sprite.x - pCursor->bits->xhot;
    y0 	= pScreenPriv->sprite.y
	- (pCursor->bits->yhot + nTopPadLines);

    /* If y0 is less than zero, some scanlines of cursor won't be displayed.
     * This fudge factor will be used at cursor enable time.
     */
    if (y0 < 0)
    {
    	nOffscreenScanlines = - y0;
	y0 = 0;
    }

    pDregs = pScreenPriv->pDregs;

    /* If cursor has changed, write the image, mask, and cursor colormap
     * to the hardware cursor: 
     */
    if ((pCursor != pScreenPriv->sprite.pCursor)
      ||(pCursor->bits != pScreenPriv->sprite.pCursor->bits))
    {
	pScreenPriv->sprite.pCursor = pCursor;
	heightPlus1 = pCursor->bits->height + 1;

	if (pCursor->bits->width <= 32)
	{
	    /* Form left justified mask of meaningful bits in cursor
	     * data and mask.  Reason: old cursor may be larger and
	     * needs to be zeroed out, not left unchanged.
	     */
	    srcMask = 0xffffffffUL << (32 - pCursor->bits->width);

	    /* write cursor mask to HW */
	    START_CURSOR_MASK_ACCESS(pDregs);
	    pSrc = (Card32 *) pCursor->bits->mask;

	    nScanlines = nTopPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }
		
	    nScanlines = heightPlus1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,(*pSrc++ & srcMask),0);
	    }

	    nScanlines = nBotPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }

	    /* write cursor data to HW */
	    START_CURSOR_DATA_ACCESS(pDregs);
	    pSrc = (Card32 *) pCursor->bits->source;

	    nScanlines = nTopPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }
		
	    nScanlines = heightPlus1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,(*pSrc++ & srcMask),0);
	    }

	    nScanlines = nBotPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }
	}
	else
	{   /* Width == 64 */

	    /* Form left justified mask of meaningful bits
	     * second word in cursor data and mask.
	     */
	    srcMask = 0xffffffffUL << (64 - pCursor->bits->width);

	    /* write cursor mask to HW */
	    START_CURSOR_MASK_ACCESS(pDregs);
	    pSrc = (Card32 *) pCursor->bits->mask;

	    nScanlines = nTopPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }
		
	    nScanlines = heightPlus1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,(*pSrc++),(*pSrc++ & srcMask));
	    }

	    nScanlines = nBotPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }

	    /* write cursor data to HW */
	    START_CURSOR_DATA_ACCESS(pDregs);
	    pSrc = (Card32 *) pCursor->bits->source;

	    nScanlines = nTopPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }
		
	    nScanlines = heightPlus1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,(*pSrc++),(*pSrc++ & srcMask));
	    }

	    nScanlines = nBotPadLines + 1;
	    while (--nScanlines)
	    {
		WRITE_CURSOR_BITS(pDregs,0,0);
	    }
	}

	SETUP_FB(pDregs,pScreenPriv->deviceID,pScreenPriv->devDepth);

	/* Load the correct colors into the hardware cursor: */
	ngleLoadCursorColormap(pScreen, pCursor);
    }

    /* Load cursor position into NGLE:
     * X11 hotspot hasn't changed position, but NGLE stores
     * origin of cursor, and relative distance between hotspot
     * and origin may have changed.
     */

    /* Update cursor X/Y position register, which contains
     * all cursor position information except the number
     * of scanlines hidden.
     */
    if (pScreenPriv->deviceID != S9000_ID_ARTIST)
    {
	xHi = (x0 / pScreenPriv->sprite.videoInterleave)
	    + pScreenPriv->sprite.horizBackPorch
	    - pScreenPriv->sprite.pipelineDelay;
	xLo = x0 % pScreenPriv->sprite.videoInterleave;
    }
    else
    {
	hbp_times_vi = (pScreenPriv->sprite.horizBackPorch *
			pScreenPriv->sprite.videoInterleave);
	xHi = ((x0 + hbp_times_vi) / pScreenPriv->sprite.videoInterleave)
			- pScreenPriv->sprite.pipelineDelay;
	xLo = (x0 + hbp_times_vi) % pScreenPriv->sprite.videoInterleave;
    }
	    
    /* if y0 < 0, y0 has already been set to 0 above */
    cursorXYValue = 
	(   (xHi << 19)
	|   (xLo << 16)
	|   (pScreenPriv->sprite.maxYLine - y0)
	);

    /* Update values maintained in pScreenPriv->sprite */
    pScreenPriv->sprite.visible = TRUE;
    pScreenPriv->sprite.width 	= pCursor->bits->width;
    pScreenPriv->sprite.height 	= pCursor->bits->height;
    pScreenPriv->sprite.nTopPadLines = nTopPadLines;

    /* ENABLE_CURSOR macro uses value of sprite.moveOnVBlank
     * in deciding whether to wait for vertical blank before moving
     * cursor.
     */
    switch (pScreenPriv->moveCursorOnVBlank)
    {
    case CURSOR_AT_VBLANK_ALWAYS:
	pScreenPriv->sprite.moveOnVBlank    = TRUE;
	break;

    case CURSOR_AT_VBLANK_NEVER:
	pScreenPriv->sprite.moveOnVBlank    = FALSE;
	break;

    case CURSOR_AT_VBLANK_DRIVEROPTION:
	if ((   (pScreenPriv->deviceID == S9000_ID_TIMBER)
	    ||  (pScreenPriv->deviceID == S9000_ID_A1439A)
	    )
	&& (nTopPadLines < NTOPLINES_FOR_TIMBER))
	{
	    pScreenPriv->sprite.moveOnVBlank    = TRUE;
	}
	else
	{
	    pScreenPriv->sprite.moveOnVBlank    = FALSE;
	}
	break;
    }

    /**** Now, that we have loaded the correct bits into the hardware
     **** cursor, turn it back on:
     ****/

    /* If cursor is at top of screen, hotspot may be displayed
     * but origin is at (y<0).  NGLE hardware stores this value
     * in same register as cursor enable bit, so pass as parameter.
     */
    ENABLE_CURSOR(pScreenPriv, cursorXYValue, nOffscreenScanlines);

    return(TRUE);

} /* ngleDisplayCursor() */


/******************************************************************************
 *
 * NGLE DDX Procedure:		hyperDisplayCursor()
 *
 * Description:
 *
 *	This routine is called by DIX (and the input driver?)
 *	whenever the sprite/cursor is changed 1) from one "cursor" (i.e. image
 *	and mask bits, foreground and background color) to another 2) from one
 *	screen to another.  This routine downloads the cursor image/mask and
 *	foreground and background colors to the hardware cursor.
 *
 *      Cloned from ngleDisplayCursor to use new hyperdrive cursor model and
 *      to avoid ugly bug workarounds in ngle code.
 *
 ******************************************************************************/

Bool hyperDisplayCursor(
    ScreenPtr		pScreen,
    CursorPtr		pCursor)
{
    NgleScreenPrivPtr	pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    WindowPtr		pRootWindow;
    NgleHdwPtr		pDregs	    = pScreenPriv->pDregs;
    Card32		*pSrcStart;
    Card32		*pSrc;
    Int32		nScanlines; /* loop variable */
    Int32		heightPlus1;
    Int32		sourceIncrement;
    Int32		x0, y0;	    /* Sprite origin (not hotspot) */
    Int32		i;
    Int32               curaddr;
    Card32		srcMask;
    Int32		nFreeFifoSlots = 0; 
    Card32		cursorXYValue = 0;  
    int 		nBotPadLines = 0;


    /*
    ***************************************************************************
    ** We are called during initialization as a side effect of doing a change
    ** screens to insure screen 0 is on top.  At that time, there is no root
    ** window so we should just abort and wait for a call that has a cursor
    ** to display.
    **
    ** After the first time this routine is called (i.e. after the side effect
    ** call), if pCursor is NULL it means that the Corvallis input driver is
    ** changing screens and wants this screen to use the same "cursor" (i.e.
    ** image and mask bits, etc.) as the last time that the sprite was on
    ** this screen.  However, the second time that pScreen->DisplayCursor is
    ** called, pCursor is NULL, but since there wasn't a previous cursor,
    ** the driver should use the Root Window's cursor (which is the side
    ** effect that the input driver and/or DIX wants to acheive).
    ***************************************************************************
    */
    if (!(pRootWindow = WindowTable[pScreen->myNum]))
	return(TRUE);

    if (pScreenPriv->sprite.firstCallTo_DisplayCursor)
    {
	pScreenPriv->sprite.firstCallTo_DisplayCursor = FALSE;

	if (pCursor == NULL)
	    pCursor = pRootWindow->optional->cursor;
    }

    /* Now if the cursor is still null, abort because we are using the
     * same cursor as before:
     */

    if (pCursor == NULL)
	return(FALSE);

    /* Check for a valid sized cursor: (hyper same as ngle) */
    if ((pCursor->bits->width  > NGLE_MAX_SPRITE_SIZE) ||
        (pCursor->bits->height > NGLE_MAX_SPRITE_SIZE))
        return(FALSE);  /* Cursor is TOO big */

    /* Lock the device: */
    NGLE_LOCK(pScreenPriv);

    /* to protect against VID bus data loss */
    SETUP_HW(pDregs);

    GET_FIFO_SLOTS(nFreeFifoSlots,2);
    HYPER_GET_VID_BUS_ACCESS;
    if (pScreenPriv->sprite.visible == TRUE)
    {
	/**** Before loading the new image, mask, and colors into the hardware
	 **** cursor, turn it off so we don't get flicker:
	 ****/
	 /* Accessing asynchronous register: no need to check FIFO */
	 HYPER_DISABLE_CURSOR(pScreenPriv);

    }

    /* Calculate cursor origin (not hotspot) and number of scanlines not
     * actually displayed.
     */

    x0 = pScreenPriv->sprite.x - pCursor->bits->xhot;
    y0 	= pScreenPriv->sprite.y
	- (pCursor->bits->yhot);
    nBotPadLines = NGLE_MAX_SPRITE_SIZE - pCursor->bits->height;
    
    /* If cursor has changed, write the image, mask, and cursor colormap
     * to the hardware cursor: 
     */
    if ((pCursor != pScreenPriv->sprite.pCursor)
      ||(pCursor->bits != pScreenPriv->sprite.pCursor->bits))
    {

	pScreenPriv->sprite.pCursor = pCursor;

	heightPlus1 = pCursor->bits->height + 1;

	/* Start out writing the mask */
	pSrcStart = (Card32 *) pCursor->bits->mask;
	curaddr = 0; /* Mask Area */

	/**** Write the image and mask to the hardware cursor: ****/
	for (i = 0; i < 2; i++)
	{
	    /* Source is long-aligned: */
	    assert((Card32) pSrcStart == ((Card32)(pSrcStart) >> 2) << 2);

	    /* Like all pixmaps created by NGLE DDX, each scanline
	     * of the source and mask pixmaps are padded on the right
	     * to a long-word boundary.  For a NGLE sprite, this means
	     * the scanline is either 32 or 64 bits.
	     *
	     * Padding is NOT necessarily 0!
	     */

	    if (pCursor->bits->width <= 32)
	    {
		pSrc = pSrcStart;

		/* Form left justified mask of meaningful bits in cursor
		 * data and mask.  Alas, masking is performed on CPU
		 * on source data rather than using the ELK mask
		 * register.  Reason: old cursor may be larger and
		 * needs to be zeroed out, not left unchanged.
		 */
		srcMask = 0xffffffffUL << (32 - pCursor->bits->width);

		/* Ngle cares whether old cursor was small enough that */
		/* we dont have to zero all 64 bits.  Hyperdrive       */
		/* its a dont-care since you can either write the 0 or */
		/* write the new address.                              */

		nScanlines = heightPlus1;
		GET_FIFO_SLOTS(nFreeFifoSlots, 1);
		HYPER_SET_CURSOR_ADDRESS(curaddr);
		while (--nScanlines)
		    {
		        GET_FIFO_SLOTS(nFreeFifoSlots, 2);
		        HYPER_WRITE_CURSOR_DATA((*pSrc++ & srcMask));
		        HYPER_WRITE_CURSOR_DATA(0);
		    }
		nScanlines = nBotPadLines + 1;
		while (--nScanlines)
		    {
		        GET_FIFO_SLOTS(nFreeFifoSlots, 2);
		        HYPER_WRITE_CURSOR_DATA(0);
		        HYPER_WRITE_CURSOR_DATA(0);
		    }
	    }
	    else
	    {   /* Width == 64 */

		/* Form left justified mask of meaningful bits
		 * second word in cursor data and mask.
		 */
		srcMask = 0xffffffffUL << (64 - pCursor->bits->width);

		/* Destination bitmap address already set to (0,0) */
		pSrc = pSrcStart;

		nScanlines = heightPlus1;
		GET_FIFO_SLOTS(nFreeFifoSlots, 1);
		HYPER_SET_CURSOR_ADDRESS(curaddr);
		while (--nScanlines)
		{
		    GET_FIFO_SLOTS(nFreeFifoSlots, 2);
		    HYPER_WRITE_CURSOR_DATA(*pSrc++);
		    HYPER_WRITE_CURSOR_DATA((*pSrc++ & srcMask));
		}
		nScanlines = nBotPadLines + 1;
		while (--nScanlines)
		{
		    GET_FIFO_SLOTS(nFreeFifoSlots, 2);
		    HYPER_WRITE_CURSOR_DATA(0);
		    HYPER_WRITE_CURSOR_DATA(0);
		}
	    }
	/* Now do the data */
    	pSrcStart = (Card32 *) pCursor->bits->source;
	curaddr = HYPER_CURSOR_DATA_BIT;
	}

	/* Load the correct colors into the hardware cursor: */
	ngleLoadCursorColormap(pScreen, pCursor);
    }

    /* Load cursor position into NGLE:
     * X11 hotspot hasn't changed position, but NGLE stores
     * origin of cursor, and relative distance between hotspot
     * and origin may have changed.
     */

    /* if y0 < 0, y0 has already been set to 0 above */
    cursorXYValue = HYPER_CURSOR_XY(x0,y0);

    /* Update values maintained in pScreenPriv->sprite */
    pScreenPriv->sprite.visible = TRUE;
    pScreenPriv->sprite.width 	= pCursor->bits->width;
    pScreenPriv->sprite.height 	= pCursor->bits->height;

    /**** Now, that we have loaded the correct bits into the hardware
     **** cursor, turn it back on:
     ****/

    /* If cursor is at top of screen, hotspot may be displayed
     * but origin is at (y<0).  NGLE hardware stores this value
     * in same register as cursor enable bit, so pass as parameter.
     */
    HYPER_ENABLE_CURSOR(pScreenPriv, cursorXYValue);

    NGLE_UNLOCK(pScreenPriv);

    return(TRUE);

} /* hyperDisplayCursor() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleRecolorCursor()
 *
 * Description:
 *
 *	This routine is called from DIX whenever a color(s) for a "cursor"
 *	(i.e. image and mask bits, foreground and background color) change.
 *	Since this routine may be called for a "cursor" that isn't the
 *	current "sprite", care must be excercised before changing the colors
 *	in the hardware cursor.
 *
 ******************************************************************************/

void ngleRecolorCursor(pScreen, pCursor, displayed)

    ScreenPtr		pScreen;
    CursorPtr		pCursor;
    Bool		displayed;
{
    /* Since DisplayCursor updates the cursor foreground and background
     * colors before displaying it, we only need to re-load the
     * cursor colormap if the cursor changing colors is already being
     * displayed.
     */

    if (displayed)
	ngleLoadCursorColormap(pScreen, pCursor);

} /* ngleRecolorCursor() */



/******************************************************************************
 *
 * NGLE DDX Procedure:		ngleMoveSprite()
 *
 * Description:
 *
 *	This routine is called from the input driver whenever the
 *	pointer device (e.g. mouse) is moved.
 *
 ******************************************************************************/

void ngleMoveSprite(pScreen, xhot, yhot, forceit)

    ScreenPtr		pScreen;
    Int16		xhot,yhot;
    Bool		forceit;
{
    NgleScreenPrivPtr	pScreenPriv;
    CursorBitsPtr	pCursorBits;
    Int32 		x0, y0;
    Int32		nOffscreenScanlines = 0;
    Int32		hbp_times_vi,	    /* For cursor positioning */
			xHi,
			xLo;
    Card32		cursorXYValue = 0;  
    NgleHdwPtr		pDregs;
    Int32         	nFreeFifoSlots = 0;

#ifdef XTESTEXT1
    extern Int32	on_steal_input; /* Defined in xtestext1di.c */

    if (on_steal_input)
	check_for_motion_steal(xhot, yhot); /* XTM */
#endif /* XTESTEXT1 */

    pScreenPriv = NGLE_SCREEN_PRIV(pScreen);
    pDregs	= pScreenPriv->pDregs;

    /* Only move if cursor defined and position changed.
     *
     * (Cursor should be defined before this procedure called,
     * but one case may occur on multi-headed server when changing
     * cursor to new screen)
     */
    if ((pScreenPriv->sprite.pCursor != NULL)
    && ((xhot != pScreenPriv->sprite.x) || (yhot != pScreenPriv->sprite.y)))
    {
	/* Define local pointer to cursor xhot and yhot offsets */
	pCursorBits	= pScreenPriv->sprite.pCursor->bits;

	pScreenPriv->sprite.x = xhot;
	pScreenPriv->sprite.y = yhot;

	if (pScreenPriv->sprite.visible)
	{
	    /* Calculate cursor origin */
	    x0 = xhot - pCursorBits->xhot;
	    /* Not all cursors start at first scan line of cursor BINs
             * (to work around hardware problem w/ghosting and tearing
             * on Biff/Mojave-based graphics cards).  Number of padding
             * padding lines is set when cursor loaded.
             */
    	    y0 = yhot - pCursorBits->yhot - pScreenPriv->sprite.nTopPadLines;

	    if (pScreenPriv->deviceID == S9000_ID_HCRX)
            { /* ie NGLE_II_HW_CURSOR */
                cursorXYValue = HYPER_CURSOR_XY(x0,y0);
                assert(pScreenPriv->sprite.visible == TRUE);

                /* This test is an optimization for thunder */
                if (HYPER_ACCEL_BUSY_DODGER_IDLE)
                {
                    if (HYPER_ACCEL_BUSY_DODGER_IDLE)
                    {
                        /* FIFO slot not required for this write */
                        HYPER_CPC(pScreenPriv, cursorXYValue);
                    }
                    else
                    {
                        HYPER_FIFO_CP(pScreenPriv, cursorXYValue);
                    }
                }
                else
                {
                    HYPER_FIFO_CP(pScreenPriv, cursorXYValue);
                }
	    }
            else
            {
		/* Calculate number of scanlines not displayed
		 * because cursor is partly off the top of the screen.
		 * (X11 requires hotspot to remain on-screen,
		 * but not the sprite origin).
		 */
		if (y0 < 0)
		{
		    nOffscreenScanlines = - y0;
		    y0 = 0;
		}

		/* Update cursor X/Y position register, which contains
		 * all cursor position information except the number
		 * of scanlines hidden.
		 */
		hbp_times_vi = (pScreenPriv->sprite.horizBackPorch *
				pScreenPriv->sprite.videoInterleave);
		xHi = ((x0 + hbp_times_vi) / pScreenPriv->sprite.videoInterleave)
		    - pScreenPriv->sprite.pipelineDelay;
		if (pScreenPriv->deviceID != S9000_ID_ARTIST)
		    xLo = x0 % pScreenPriv->sprite.videoInterleave;
		else
		    xLo = (x0 + hbp_times_vi) % pScreenPriv->sprite.videoInterleave;

		/* if y0 < 0, y0 has already been set to 0 above */
		cursorXYValue = 
		    (   (xHi << 19)
		    |   (xLo << 16)
		    |   (pScreenPriv->sprite.maxYLine - y0)
		    );

		/* Now update number of cursor scan lines hidden. 
		 * It's in same register as the enable cursor bit, and this macro
		 * also enables cursor, but this position cursor routine is only 
		 * called if cursor is visible.
		 */
		ENABLE_CURSOR(pScreenPriv, cursorXYValue, nOffscreenScanlines);
	    }
	}
    }
} /* ngleMoveSprite() */
