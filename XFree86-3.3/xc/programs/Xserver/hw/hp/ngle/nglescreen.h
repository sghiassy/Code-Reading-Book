/* $XConsortium: nglescreen.h,v 1.2 95/01/24 02:10:27 dpw Exp $ */

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
 *	concerning the Screen structure.
 *
 ******************************************************************************/

#ifndef NGLESCREEN
#define NGLESCREEN


/*
 ******************************************************************************
 **
 ** NGLE DDX Driver's Screen Private Structure:
 **
 ******************************************************************************
 */

typedef struct _NgleScreenPrivRec
{
    /**** High-Level information about the screen/device: ****/
    ScreenPtr			pScreen;	/* Pointer to DIX structure */
    Int32			myNum;		/* The number of this screen */
    Int32			fildes;		/* Unix fildes of device file */
    dev_t			dev_sc;		/* Device file's minor number */

    Card32			deviceID;	/* Is this an Elk or Rattler */
    NgleHdwPtr			pDregs;		/* Pointer to the hardware */
    pointer			fbaddr;		/* Pointer to the framebuffer */
    char                        *crt_region[CRT_MAX_REGIONS]; /* Other regions
                                                 * associated with frame buffer
                                                 * that might be mapped in.
                                                 * Obtained from GCDESCRIBE.
                                                 */
    NgleDevRomDataPtr           pDevRomData;	/* Pointer to the ROM */
    Bool			isGrayScale;	/* GRX (Not color device) */

    /**** Device sizes: ****/
    Int32			devWidth;	/* Raw width, for addr calc's */
    Int32			devDepth;	/* Depth of device # of planes */
    Int32			screenWidth;	/* Visible resolution: width */
    Int32			screenHeight;	/* Visible resolution: height */

    /**** X11 sprite information: ****/
    NgleSpriteRec		sprite;

    /**** Installed-in-Hardware Colormap Information: ****/
    ColormapPtr 		installedMap;	/* ptr to DIX colormap 
						 * currently installed */
    Card32			hwColors[256];	/* Copy of installed map */

    /**** Miscellaneous information: ****/
    /* Allow user to require that cursor updates wait for vertical blanking
     */
    Int32			moveCursorOnVBlank;

    /* Added for X server controlling both heads of Tomcat (2-headed ELK):
     * on server exit, only unmap the framebuffer and control space
     * if the "last" head on the Tomcat for this server.
     */
    Bool			lastDeviceOnThisSgcToClose;

    /* Hyperdrive (and probably other future devices) has configuration bits */
    /* to tell frame buffer depth (8:88 or 8:24) and accelerator present
    */
    unsigned Int32              deviceSpecificConfig;

    /* 
     * Pointers to various functions returned from cfbScreenInit(),
     * used for wrapper routines.
     */
    Bool (* CreateGC)();

} NgleScreenPrivRec, *NgleScreenPrivPtr;


/*
 ******************************************************************************
 **
 ** Macros to Access the NGLE DDX Driver's Screen Private Structure:
 **
 ******************************************************************************
 */

extern Int32	ngleScreenPrivIndex;
#define NGLE_SCREEN_PRIV(pScreen)\
    ((NgleScreenPrivPtr) ((pScreen)->devPrivates[ngleScreenPrivIndex].ptr))


/*
 ******************************************************************************
 **
 ** Values for pScreenPriv->moveCursorOnVBlank:
 ** Should cursor be moved only during vertical blank?
 **
 ******************************************************************************
 */
#define CURSOR_AT_VBLANK_ALWAYS		1
#define CURSOR_AT_VBLANK_DRIVEROPTION	0
#define CURSOR_AT_VBLANK_NEVER		-1

/* Server state values used by hyperResetPlanes() */
#define SERVER_INIT             1
#define SERVER_EXIT             2
#define SERVER_RECOVERY         3


#endif /* NGLESCREEN */
