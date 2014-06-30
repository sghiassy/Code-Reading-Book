/* $XFree86: xc/programs/Xserver/hw/xfree86/xaa/sampledrv.c,v 3.3 1997/01/14 22:21:59 dawes Exp $ */


/*
 * This is a sample driver implementation template for the new acceleration
 * interface.
 */

#include "vga256.h"
#include "xf86.h"
#include "vga.h"

#include "../../../xaa/xf86xaa.h"

/* 
 * Include any definitions for communicating with the coprocessor here.
 * In this sample driver, the following macros are defined:
 *
 *	SETFOREGROUNDCOLOR(color)
 *	SETRASTEROP(rop)
 *	SETWRITEPLANEMASK(planemask)
 *	SETSOURCEADDR(srcaddr)
 *	SETDESTADDR(destaddr)
 *	SETWIDTH(width)
 *	SETHEIGHT(height)
 *	SETBLTXDIR(xdir)
 *	SETBLTYDIR(yrdir)
 *	SETCOMMAND(command)
 *      WAITUNTILFINISHED()
 *
 * The interface for accelerator chips varies widely, and this may not
 * be a realistic scenario. In this sample implemention, the chip requires
 * the source and destation location to be specified with addresses, but
 * it might just as well use coordinates. When implementing the primitives,
 * you will often find the need to store some settings in a variable.
 */
/* #include "coprocessor.h" */

void ChipSync();
void ChipSetupForFillRectSolid();
void ChipSubsequentFillRectSolid();
void ChipSetupForScreenToScreenCopy();
void ChipSubsequentScreenToScreenCopy();


/*
 * The following function sets up the supported acceleration. Call it
 * from the FbInit() function in the SVGA driver, or before ScreenInit
 * in a monolithic server.
 */
void ChipAccelInit() {
    /*
     * If you want to disable acceleration, just don't modify anything
     * in the AccelInfoRec.
     */

    /*
     * Set up the main acceleration flags.
     * Usually, you will want to use BACKGROUND_OPERATIONS,
     * and if you have ScreenToScreenCopy, use the PIXMAP_CACHE.
     *
     * If the chip is restricted in the screen-to-screen BitBLT
     * directions it supports, you can indicate that here:
     *
     * ONLY_TWO_BITBLT_DIRECTIONS indicates that xdir must be equal to ydir.
     * ONLY_LEFT_TO_RIGHT_BITBLT indicates that the xdir must be 1.
     */
    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | PIXMAP_CACHE;

    /*
     * The following line installs a "Sync" function, that waits for
     * all coprocessor operations to complete.
     */
    xf86AccelInfoRec.Sync = ChipSync;

    /*
     * We want to set up the FillRectSolid primitive for filling a solid
     * rectangle. First we set up the flags for the graphics operation.
     * It may include GXCOPY_ONLY, NO_PLANEMASK, and RGB_EQUAL.
     */
    xf86GCInfoRec.PolyFillRectSolidFlags = 0;

    /*
     * Install the low-level functions for drawing solid filled rectangles.
     */
    xf86AccelInfoRec.SetupForFillRectSolid = ChipSetupForFillRectSolid;
    xf86AccelInfoRec.SubsequentFillRectSolid = ChipSubsequentFillRectSolid;

    /*
     * We also want to set up the ScreenToScreenCopy (BitBLT) primitive for
     * copying a rectangular area from one location on the screen to
     * another. First we set up the restrictions. In this case, we
     * don't handle transparency color compare. Other allowed flags are
     * GXCOPY_ONLY, NO_PLANEMASK and TRANSPARENCY_GXCOPY.
     */
    xf86GCInfoRec.CopyAreaFlags = NO_TRANSPARENCY;
    
    /*
     * Install the low-level functions for screen-to-screen copy.
     */
    xf86AccelInfoRec.SetupForScreenToScreenCopy =
        ChipSetupForScreenToScreenCopy;
    xf86AccelInfoRec.SubsequentScreenToScreenCopy =
        ChipSubsequentScreenToScreenCopy;

    /*
     * Set a pointer to the server InfoRec so that XAA knows how to
     * format its start-up messages. If you are not using the SVGA
     * server, you would provide a pointer to the ScrnInfoRec defined
     * for the server. The SVGA server correctly initializes this
     * field itself, so this line can be omitted when using the SVGA
     * server.
     */
/*    xf86AccelInfoRec.ServerInfoRec = &vga256InfoRec; */

    /*
     * Finally, we set up the video memory space available to the pixmap
     * cache. In this case, all memory from the end of the virtual screen
     * to the end of video memory minus 1K, can be used. If you haven't
     * enabled the PIXMAP_CACHE flag, then these lines can be omitted.
     */
     xf86AccelInfoRec.PixmapCacheMemoryStart =
         vga256InfoRec.virtualY * vga256InfoRec.displayWidth
         * vga256InfoRec.bitsPerPixel / 8;
     xf86AccelInfoRec.PixmapCacheMemoryEnd =
        vga256InfoRec.videoRam * 1024 - 1024;
}

/*
 * This is the implementation of the Sync() function.
 */
void ChipSync() {
    WAITUNTILFINISHED();
}

/*
 * This is the implementation of the SetupForFillRectSolid function
 * that sets up the coprocessor for a subsequent batch of solid
 * rectangle fills.
 */
void ChipSetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    SETFOREGROUNDCOLOR(color);
    SETRASTEROP(rop);
    /*
     * If you don't support a write planemask, and have set the
     * appropriate flag, then the planemask can be safely ignored.
     * The same goes for the raster-op if only GXcopy is supported.
     */
    SETWRITEPLANEMASK(planemask);
}

/*
 * This is the implementation of the SubsequentForFillRectSolid function
 * that sends commands to the coprocessor to fill a solid rectangle of
 * the specified location and size, with the parameters from the SetUp
 * call.
 */
void ChipSubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;
    destaddr = y * vga256InfoRec.displayWidth + x;
    /*
     * When BACKGROUND_OPERATIONS is enabled, on some chips (such as
     * Cirrus) you must wait here for the previous operation to finish.
     * On others (like ARK or Matrox), you don't, or you might wait for
     * a certain number of command FIFO slots to become free (the
     * latter is often unnecessary, and it does impact performance).
     */
    /* ChipSync(); */
    SETWIDTH(w);
    SETHEIGHT(h);
    SETDESTADDR(destaddr);
    SETCOMMAND(FILLRECTANGLE);
    /* 
     * If you don't use BACKGROUND_OPERATIONS, this would be the
     * place to call ChipSync().
     */
}

/*
 * This is the implementation of the SetupForScreenToScreenCopy function
 * that sets up the coprocessor for a subsequent batch of
 * screen-to-screen copies. Remember, we don't handle transparency,
 * so the transparency color is ignored.
 */
static int blitxdir, blitydir;
 
void ChipSetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
    /*
     * xdir can be either 1 (left-to-right) or -1 (right-to-left).
     * ydir can be either 1 (top-to-bottom) or -1 (bottom-to-top).
     */
    SETBLTXDIR(xdir);
    SETBLTYDIR(ydir);
    SETRASTEROP(rop);
    SETPLANEMASK(planemask);
    blitxdir = xdir;
    blitydir = ydir;
}

/*
 * This is the implementation of the SubsequentForScreenToScreenCopy
 * that sends commands to the coprocessor to perform a screen-to-screen
 * copy of the specified areas, with the parameters from the SetUp call.
 * In this sample implementation, the direction must be taken into
 * account when calculating the addresses (with coordinates, it might be
 * a little easier).
 */
void ChipSubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
    int srcaddr, destaddr;
    /*
     * If the direction is "decreasing", the chip wants the addresses
     * to be at the other end, so we must be aware of that in our
     * calculations.
     */
    if (blitydir == -1) {
        srcaddr = (y1 + h - 1) * vga256InfoRec.displayWidth;
	destaddr = (y2 + h - 1) * vga256InfoRec.displayWidth;
    } else {
        srcaddr = y1 * vga256InfoRec.displayWidth;
        destaddr = y2 * vga256InfoRec.displayWidth;
    }
    if (blitxdir == -1) {
	srcaddr += x1 + w - 1;
	destaddr += x2 + w - 1;
    } else {
	srcaddr += x1;
	destaddr += x2;
    }
    /*
     * Again, you may have to wait for the previous operation to
     * finish when using BACKGROUND_OPERATIONS.
     */
    /* ChipSync(); */
    SETSOURCEADDR(srcaddr);
    SETDESTADDR(destaddr);
    SETWIDTH(w);
    SETHEIGHT(h);
    SETCOMMAND(SCREENTOSCREENCOPY);
    /* 
     * If you don't use BACKGROUND_OPERATIONS, this would be the
     * place to call ChipSync().
     */
}
