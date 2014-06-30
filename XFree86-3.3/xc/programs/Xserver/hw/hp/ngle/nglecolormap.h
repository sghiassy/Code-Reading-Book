/* $XConsortium: nglecolormap.h,v 1.2 95/01/24 02:02:01 dpw Exp $ */

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
 *  Defines shared structure NgleDisplayRec as well as
 *  DDX-specific entry points relating to color for NGLE driver.
 *
 ******************************************************************************/

#ifndef NGLECOLORMAP_H
#define NGLECOLORMAP_H

#define HYPER_CMAP_TYPE				0
#define NGLE_CMAP_INDEXED0_TYPE			0
#define NGLE_CMAP_OVERLAY_TYPE			3

/* Typedef of LUT (Colormap) BLT Control Register */
typedef union
{   /* Note assumption that fields are packed left-to-right */
    unsigned long   all;
    struct
    {
        unsigned    enable              :  1;
        unsigned    waitBlank           :  1;
        unsigned    reserved1           :  4;
        unsigned    lutOffset           : 10;   /* Within destination LUT */
        unsigned    lutType             :  2;   /* Cursor, image, overlay */
        unsigned    reserved2           :  4;
        unsigned    length              : 10;
    } fields;
} NgleLutBltCtl;

extern void ngleInstallColormap(
    ColormapPtr pColormap);

extern void ngleUninstallColormap(
    ColormapPtr pColormap);

extern int ngleListInstalledColormaps(
    ScreenPtr   pScreen,
    Colormap    *pCmapList);

extern void ngleStoreColors(
    ColormapPtr pCmap,
    int         ndef,
    xColorItem  *pdefs);

extern Bool ngleCreateColormap(
    ColormapPtr pCmap);

extern void ngleDestroyColormap(
    ColormapPtr pCmap);

extern void ngleResolvePseudoColor(
    Card16      *pRed,
    Card16      *pGreen,
    Card16      *pBlue,
    VisualPtr   pVisual);


#endif /* NGLECOLORMAP_H */
