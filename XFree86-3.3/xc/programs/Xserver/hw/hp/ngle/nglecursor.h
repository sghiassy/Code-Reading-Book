/* $XConsortium: nglecursor.h,v 1.2 95/01/24 02:07:13 dpw Exp $ */

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
 *	This file contains various macros, typedefs and extern declarations
 *	concerning cursors (both sprites and echoes).
 *
 ******************************************************************************/

#ifndef NGLECURSOR_H
#define NGLECURSOR_H


/* The following is the maximum width/height supported by the NGLE cursor
 * hardware:
 */
#define NGLE_MAX_SPRITE_SIZE	64


/*
 ******************************************************************************
 ** 
 ** NGLE DDX Driver's Sprite Structure:
 ** 
 ******************************************************************************
 */
typedef struct _NgleSpriteRec
{
    /* The sprite's state - visible or not visible */
    Int32	visible;

    /* Location of the sprite's hotspot relative to the screen's origin: */
    Int32	x;
    Int32	y;

    /* Pointer to currently installed cursor struct: */
    CursorPtr	pCursor;

    /* See the comment fairly early on in ngleDisplayCursor: */
    Int32	firstCallTo_DisplayCursor;

    /* Hardware values that affect cursor positioning.
     * Read in at X11 startup.
     */
    Card32	horizBackPorch;
    Card32	maxYLine;
    Card32	pipelineDelay;
    Card32	videoInterleave;

    /* Value of hardware register value written at cursor enable */
    Card32	enabledCursorXYValue;

    /* Save current cursor width and height.  When new cursor requested,
     * old cursor's structures (pointed to by pCursor above) may already
     * have been de-allocated. Used to minimize scanlines that must be written.
     */
    Int32	height;
    Int32	width;

    /* Cursor Y position adjustment */
    Int32	nTopPadLines;

    /* Value related to nTopPadLines: should cursor only be moved
     * while vertical blank is in progress?
     *
     * Set in ngleDisplayCursor() and used in ENABLE_CURSOR macro.
     */
    Bool	moveOnVBlank;

} NgleSpriteRec, *NgleSpritePtr;


/*
 ******************************************************************************
 ** 
 ** Macros to Access the NGLE DDX Driver's Sprite Structure:
 ** 
 ******************************************************************************
 */

#define NGLE_SPRITE_FROM_SCREEN(pScreen)\
    (NGLE_SCREEN_PRIV(pScreen)->pSprite)

#define NGLE_SPRITE_FROM_SCREEN_PRIV(pScreenPriv)\
    ((pScreenPriv)->pSprite)


/*
 ******************************************************************************
 ** 
 ** NGLE DDX Driver's Cursor Internally-Visible Extern Declarations:
 ** 
 ******************************************************************************
 */
extern Bool	ngleInitSprite();
extern Bool	hyperInitSprite();
extern Bool	ngleUninitSprite();

extern void	ngleDisableSprite();
extern void	ngleChangeScreen();

extern Bool	ngleDisplayCursor();
extern Bool     hyperDisplayCursor();
extern void	ngleRecolorCursor();

extern Bool	ngleSetCursorPosition();
extern void	ngleMoveSprite();

/* Shared HP routines for the cursor: */
extern void 	hpCursorLimits();
extern void 	hpConstrainCursor();
extern Bool 	hpSetCursorPosition();


/* The number of scanlines that must be left blank in order to end the
 * ghosting/tearing problem has been empirically determined to be 12.
 */
#define NTOPLINES_FOR_TIMBER 12

#endif /* NGLECURSOR_H */
