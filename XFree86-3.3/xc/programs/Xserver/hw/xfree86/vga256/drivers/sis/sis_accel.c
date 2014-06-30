/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/sis/sis_accel.c,v 3.1.2.1 1997/05/09 07:15:48 hohndel Exp $ */


/*
 * This is a sample driver implementation template for the new acceleration
 * interface.
 */

#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#include "xf86xaa.h"

#include "sis_driver.h"
#include "sis_Blitter.h"
extern Bool sisUseXAAcolorExp ;
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

void SISSync();
void SISSetupForFillRectSolid();
void SISSubsequentFillRectSolid();
void SISSetupForScreenToScreenCopy();
void SISSubsequentScreenToScreenCopy();

void SISSetupForScreenToScreenColorExpand();
void SISSubsequentScreenToScreenColorExpand();
void SISSetupForScanlineScreenToScreenColorExpand();
void SISSubsequentScanlineScreenToScreenColorExpand();

void SISSetupFor8x8PatternColorExpand();
void SISSubsequent8x8PatternColorExpand();

static int sisPatternHeight=16 ;

/*
 * The following function sets up the supported acceleration. Call it
 * from the FbInit() function in the SVGA driver, or before ScreenInit
 * in a monolithic server.
 */
void SISAccelInit()
{
    int cacheStart, cacheEnd;
    int sisCursorSize = sisHWCursor ? 16384 : 0 ; 
    int offscreen_available ;
    int sisBLTPatternAddress ;
    int sisBLTPatternOffscreenSize ;
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

    /* Disable the PIXMAP CACHE in no linear because XAA high level does not
     * work with video in banked mode.
     * May be in the future we could restore the PIXMAP CACHE even in banked
     * mode
     */
       

    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | 
	(sisUseXAAcolorExp ? PIXMAP_CACHE : 0 ) |
	    HARDWARE_PATTERN_PROGRAMMED_BITS | 
		HARDWARE_PATTERN_PROGRAMMED_ORIGIN |
		    HARDWARE_PATTERN_BIT_ORDER_MSBFIRST |
			HARDWARE_PATTERN_MONO_TRANSPARENCY ;

    /*
     * The following line installs a "Sync" function, that waits for
     * all coprocessor operations to complete.
     */
    xf86AccelInfoRec.Sync = SISSync;

    /*
     * We want to set up the FillRectSolid primitive for filling a solid
     * rectangle. First we set up the flags for the graphics operation.
     * It may include GXCOPY_ONLY, NO_PLANEMASK, and RGB_EQUAL.
     */
    xf86GCInfoRec.PolyFillRectSolidFlags = NO_PLANEMASK ;

    /*
     * Install the low-level functions for drawing solid filled rectangles.
     */
    xf86AccelInfoRec.SetupForFillRectSolid = SISSetupForFillRectSolid;
    xf86AccelInfoRec.SubsequentFillRectSolid = SISSubsequentFillRectSolid;

    /*
     * We also want to set up the ScreenToScreenCopy (BitBLT) primitive for
     * copying a rectangular area from one location on the screen to
     * another. First we set up the restrictions. In this case, we
     * don't handle transparency color compare. Other allowed flags are
     * GXCOPY_ONLY and NO_PLANEMASK.
     */
    xf86GCInfoRec.CopyAreaFlags = NO_TRANSPARENCY | NO_PLANEMASK;
    
    /*
     * Install the low-level functions for screen-to-screen copy.
     */
    xf86AccelInfoRec.SetupForScreenToScreenCopy =
        SISSetupForScreenToScreenCopy;
    xf86AccelInfoRec.SubsequentScreenToScreenCopy =
        SISSubsequentScreenToScreenCopy;

    /* Color Expansion */
    if (vga256InfoRec.bitsPerPixel != 24) {
	/* the enhanced color expansion is not supported
	 * by the engine in 16M-color graphic mode.
	 */
	xf86AccelInfoRec.ColorExpandFlags = VIDEO_SOURCE_GRANULARITY_DWORD |
	                                BIT_ORDER_IN_BYTE_MSBFIRST |
					SCANLINE_PAD_DWORD |
					GXCOPY_ONLY |
					NO_PLANEMASK;
	if ( sisUseXAAcolorExp ) {
	    xf86AccelInfoRec.SetupForScreenToScreenColorExpand = 
		SISSetupForScreenToScreenColorExpand;
	    xf86AccelInfoRec.SubsequentScreenToScreenColorExpand = 
		SISSubsequentScreenToScreenColorExpand;
	    xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand =
		SISSetupForScanlineScreenToScreenColorExpand;
	    xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand =
		SISSubsequentScanlineScreenToScreenColorExpand;
	
	    offscreen_available = vga256InfoRec.videoRam * 1024 - 
		vga256InfoRec.displayWidth * vga256InfoRec.virtualY
		    * (vgaBitsPerPixel / 8);
	    sisBLTPatternOffscreenSize = 1024 ;
	
	    if (offscreen_available < sisBLTPatternOffscreenSize) {
		ErrorF("%s %s: Not enough off-screen video"
		       " memory for expand color.\n",
		       XCONFIG_PROBED, vga256InfoRec.name);
		sisBLTPatternOffscreenSize = 0 ;
	    }
	    else {
		sisBLTPatternAddress = vga256InfoRec.videoRam * 1024 
		    - sisCursorSize - sisBLTPatternOffscreenSize;
		xf86AccelInfoRec.ScratchBufferAddr=sisBLTPatternAddress;
		xf86AccelInfoRec.ScratchBufferSize=sisBLTPatternOffscreenSize;
	    }
	}	
	/*
	 * 8x8 color expand pattern fill
	 */

	xf86AccelInfoRec.SetupFor8x8PatternColorExpand =
	    SISSetupFor8x8PatternColorExpand;
	xf86AccelInfoRec.Subsequent8x8PatternColorExpand =
	    SISSubsequent8x8PatternColorExpand;

    }
     /*
     * Finally, we set up the video memory space available to the pixmap
     * cache. In this case, all memory from the end of the virtual screen
     * to the end of video memory minus 1K, can be used. If you haven't
     * enabled the PIXMAP_CACHE flag, then these lines can be omitted.
     */
    if (sisUseXAAcolorExp) {
	cacheStart =
	    vga256InfoRec.virtualY * vga256InfoRec.displayWidth
		* vga256InfoRec.bitsPerPixel / 8;
	cacheEnd =
	    vga256InfoRec.videoRam * 1024 - 1024 - sisBLTPatternOffscreenSize -
		sisCursorSize ;

	xf86InitPixmapCache(&vga256InfoRec, cacheStart, cacheEnd);
    }
    /* 
     * Now set variables often used
     *
     */
    
    sisPatternHeight = (SISchipset == SIS86C205) ? 16 : 8  ;
    
}

/*
 * This is the implementation of the Sync() function.
 */
void SISSync() {
    sisBLTWAIT;
}

static int sisALUConv[] =
{
    0x00,		       /* dest = 0; GXclear, 0 */
    0x88,		       /* dest &= src; GXand, 0x1 */
    0x44,		       /* dest = src & ~dest; GXandReverse, 0x2 */
    0xCC,		       /* dest = src; GXcopy, 0x3 */
    0x22,		       /* dest &= ~src; GXandInverted, 0x4 */
    0xAA,		       /* dest = dest; GXnoop, 0x5 */
    0x66,		       /* dest = ^src; GXxor, 0x6 */
    0xEE,		       /* dest |= src; GXor, 0x7 */
    0x11,		       /* dest = ~src & ~dest;GXnor, 0x8 */
    0x99,		       /*?? dest ^= ~src ;GXequiv, 0x9 */
    0x55,		       /* dest = ~dest; GXInvert, 0xA */
    0xDD,		       /* dest = src|~dest ;GXorReverse, 0xB */
    0x33,		       /* dest = ~src; GXcopyInverted, 0xC */
    0xBB,		       /* dest |= ~src; GXorInverted, 0xD */
    0x77,		       /*?? dest = ~src|~dest ;GXnand, 0xE */
    0xFF,		       /* dest = 0xFF; GXset, 0xF */
};

/*
 * This is the implementation of the SetupForFillRectSolid function
 * that sets up the coprocessor for a subsequent batch of solid
 * rectangle fills.
 */
void SISSetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{

    sisSETFGCOLOR(color);
    sisSETROP(sisALUConv[rop & 0xF]);
    sisSETPITCH(vga256InfoRec.displayWidth * vgaBytesPerPixel, 
		vga256InfoRec.displayWidth * vgaBytesPerPixel);
    /*
     * If you don't support a write planemask, and have set the
     * appropriate flag, then the planemask can be safely ignored.
     * The same goes for the raster-op if only GXcopy is supported.
     */
    /*SETWRITEPLANEMASK(planemask);*/
}

/*
 * This is the implementation of the SubsequentForFillRectSolid function
 * that sends commands to the coprocessor to fill a solid rectangle of
 * the specified location and size, with the parameters from the SetUp
 * call.
 */
void SISSubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr, op;

    destaddr = y * vga256InfoRec.displayWidth + x;
    op = sisCMDBLT | sisSRCFG | sisTOP2BOTTOM | sisLEFT2RIGHT;
    destaddr *= vgaBytesPerPixel ;
    /*
     * When BACKGROUND_OPERATIONS is enabled, on some chips (such as
     * Cirrus) you must wait here for the previous operation to finish.
     * On others (like ARK or Matrox), you don't, or you might wait for
     * a certain number of command FIFO slots to become free (the
     * latter is often unnecessary, and it does impact performance).
     */
    /* ChipSync(); */
    sisSETHEIGHTWIDTH(h-1, w * vgaBytesPerPixel-1);
    sisSETDSTADDR(destaddr);
    sisSETCMD(op);
    /* 
     * If you don't use BACKGROUND_OPERATIONS, this would be the
     * place to call SISSync().
     */
}

/*
 * This is the implementation of the SetupForScreenToScreenCopy function
 * that sets up the coprocessor for a subsequent batch of
 * screen-to-screen copies. Remember, we don't handle transparency,
 * so the transparency color is ignored.
 */
static int blitxdir, blitydir;
 
void SISSetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
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
    sisSETPITCH(vga256InfoRec.displayWidth * vgaBytesPerPixel, 
		vga256InfoRec.displayWidth * vgaBytesPerPixel);
    sisSETROP(sisALUConv[rop & 0xF]);
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
void SISSubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
    int srcaddr, destaddr;
    int op ;

    /*
     * If the direction is "decreasing", the chip wants the addresses
     * to be at the other end, so we must be aware of that in our
     * calculations.
     */
    op = sisCMDBLT | sisSRCVIDEO;
    if (blitydir == -1) {
	op |= sisBOTTOM2TOP;
        srcaddr = (y1 + h - 1) * vga256InfoRec.displayWidth;
	destaddr = (y2 + h - 1) * vga256InfoRec.displayWidth;
    } else {
	op |= sisTOP2BOTTOM;
        srcaddr = y1 * vga256InfoRec.displayWidth;
        destaddr = y2 * vga256InfoRec.displayWidth;
    }
    if (blitxdir == -1) {
	op |= sisRIGHT2LEFT;
	srcaddr += x1 + w - 1;
	destaddr += x2 + w - 1;
    } else {
	op |= sisLEFT2RIGHT;
	srcaddr += x1;
	destaddr += x2;
    }
    srcaddr *= vgaBytesPerPixel;
    destaddr *= vgaBytesPerPixel;
    if ( (vgaBytesPerPixel>1) && (blitxdir == -1) ) {
	srcaddr += vgaBytesPerPixel-1; 
	destaddr += vgaBytesPerPixel-1;
    }
    /*
     * Again, you may have to wait for the previous operation to
     * finish when using BACKGROUND_OPERATIONS.
     */
    /* SISSync(); */
    sisSETSRCADDR(srcaddr);
    sisSETDSTADDR(destaddr);
    sisSETHEIGHTWIDTH(h-1, w * vgaBytesPerPixel-1);
    sisSETCMD(op);
    /* 
     * If you don't use BACKGROUND_OPERATIONS, this would be the
     * place to call SISSync().
     */
}

/*
 * setup for screen-to-screen color expansion
 */
static int sisColExp_op ;
void SISSetupForScreenToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned planemask;
{
    int isTransparent = ( bg == -1 );
    int	op ;

    /*ErrorF("SISSetupScreenToScreenColorExpand()\n");*/

    op  = sisCMDCOLEXP | sisTOP2BOTTOM | sisLEFT2RIGHT ;

    /*
     * check transparency 
     */
    /* becareful with rop */
    if (isTransparent) {
	sisSETFGCOLOR(fg);
	sisSETROPFG(0xf0); 	/* pat copy */
	sisSETROPBG(0xAA); 	/* dst */
	op |= sisPATFG | sisSRCBG ; 
    } else {
	sisSETBGCOLOR(bg);
	sisSETFGCOLOR(fg);
	sisSETROPFG(0xf0);	/* pat copy */
	sisSETROPBG(0xcc); 	/* copy */
	op |= sisPATFG | sisSRCBG ;
    }
    op |= sisCMDENHCOLEXP ;
    sisColExp_op = op ;
}

/*
 * executing screen-to-screen color expansion
 */
void SISSubsequentScreenToScreenColorExpand(srcx, srcy, x, y, ww, h)
    int srcx, srcy, x, y, ww, h;
{
    int destpitch = vga256InfoRec.displayWidth * vgaBytesPerPixel ;
    int srcaddr = srcy * destpitch *  + srcx ;
    int destaddr = y * destpitch + x * vgaBytesPerPixel;
    int srcpitch ;
    int w ;
    int widthTodo ;

    /*ErrorF("SISSubsequentScreenToScreenColorExpand()\n");*/
#define maxWidth 144
    /* can't expand more than maxWidth in one time.
       it's a work around for scanline greater than maxWidth 
     */
    destpitch = vga256InfoRec.displayWidth * vgaBytesPerPixel ;
    srcpitch =  ((ww + 31)& ~31) /8 ;
    sisSETPITCH(srcpitch, destpitch);
    widthTodo = ww ;
    do { 
	w = widthTodo < maxWidth ? widthTodo : maxWidth ;
	sisSETDSTADDR(destaddr);
	sisSETSRCADDR(srcaddr);
	sisSETHEIGHTWIDTH(h-1, w*vgaBytesPerPixel-1);
	sisSETCMD(sisColExp_op);
	srcaddr += w ;
	destaddr += w*vgaBytesPerPixel ;
	widthTodo -= w ;
    } while ( widthTodo > 0 ) ;

}

static int sisDstAddr;
static int sisDstPitch;
static int sisWidth ;
void SISSetupForScanlineScreenToScreenColorExpand(x, y, w, h, bg, fg, rop,
planemask)
    int x, y, w, h, bg, fg, rop;
    unsigned int planemask;
{
    int isTransparent = ( bg == -1 );
    int	op ;
    int pitch = vga256InfoRec.displayWidth * vgaBytesPerPixel ;
    int destaddr = y * pitch + x * vgaBytesPerPixel;

    op  = sisCMDCOLEXP | sisTOP2BOTTOM | sisLEFT2RIGHT | 
	sisPATFG | sisSRCBG | sisCMDENHCOLEXP ;
    /*
     * check transparency 
     */
    /* becareful with rop */
    if (isTransparent) {
	sisSETFGCOLOR(fg);
	sisSETROPFG(0xf0); 	/* pat copy */
	sisSETROPBG(0xAA); 	/* dst */
    } else {
	sisSETBGCOLOR(bg);
	sisSETFGCOLOR(fg);
	sisSETROPFG(0xf0);	/* pat copy */
	sisSETROPBG(0xcc); 	/* copy */
    }
    sisColExp_op = op ;
    sisSETDSTADDR(destaddr);
    sisDstPitch = pitch ;
    sisDstAddr = destaddr ;
    sisWidth = w ;
}

void SISSubsequentScanlineScreenToScreenColorExpand(srcaddr)
    int srcaddr;
{
    int widthTodo ;
    int dstaddr, srcpitch ;
    int w ;

    /*ErrorF("SISSubsequentScanlineScreenToScreenColorExpand()\n");*/
#define maxWidth 144
    /* can't expand more than maxWidth in one time.
       it's a work around for scanline greater than maxWidth 
     */
    dstaddr = sisDstAddr ;
    widthTodo = sisWidth ;
    do { 
	w = widthTodo < maxWidth ? widthTodo : maxWidth ;
	srcpitch =  ((w + 31)& ~31) /8 ;
	sisSETPITCH(srcpitch, sisDstPitch);    
	sisSETHEIGHTWIDTH(0, w*vgaBytesPerPixel-1);
	sisSETSRCADDR(srcaddr/8);
	sisSETDSTADDR(dstaddr);
	sisSETCMD(sisColExp_op);
	srcaddr += w ;
	dstaddr += w*vgaBytesPerPixel ;
	widthTodo -= w ;
    } while ( widthTodo > 0 ) ;
    sisDstAddr += sisDstPitch ;


}

/*
 * setup for 8x8 color expand pattern fill
 *
 * HARDWARE_PATTERN_PROGRAMMED_BITS mean pattern in patternx,patterny
 * HARDWARE_PATTERN_PROGRAMMED_ORIGIN is not supported by the chip
 * the rotation is done on the fly during the load of the pattern into
 * the SiS registers.
 */
static unsigned int sisPatternReg[4]; 	/* plus 2 for rotation */
void SISSetupFor8x8PatternColorExpand(patternx, patterny, bg, fg,
                                            rop, planemask)
    unsigned patternx, patterny, planemask;
    int bg, fg, rop;
{
    unsigned int	*patternRegPtr ;
    int	       	i ;
    int 	dstpitch;
    int 	isTransparent = ( bg == -1 );
    int 	op  = sisCMDCOLEXP | sisTOP2BOTTOM | sisLEFT2RIGHT | 
	              sisPATFG | sisSRCBG ;

    dstpitch = vga256InfoRec.displayWidth * vgaBytesPerPixel ;
    /*
     * check transparency 
     */
    /* becareful with rop */
    if (isTransparent) {
	sisSETFGCOLOR(fg);
	sisSETROPFG(0xf0); 	/* pat copy */
	sisSETROPBG(0xAA); 	/* dst */
    } else {
	sisSETBGCOLOR(bg);
	sisSETFGCOLOR(fg);
	sisSETROPFG(0xf0);	/* pat copy */
	sisSETROPBG(0xcc); 	/* copy */
    }
    sisBLTWAIT;
    sisSETPITCH(0, dstpitch);    
    sisSETSRCADDR(0);
    sisColExp_op = op ;
    patternRegPtr =  (unsigned int *)sisSETPATREG();
    sisPatternReg[0] = sisPatternReg[2] = patternx ;
    sisPatternReg[1] = sisPatternReg[3] = patterny ;
    for ( i = 0 ; i < sisPatternHeight ;   ) {
	    patternRegPtr[i++] = patternx ;
	    patternRegPtr[i++] = patterny ;
	}
}

/*
 * executing 8x8 color expand pattern fill
 * reload the pattern in the SiS registers and do the rotation.
 */
void SISSubsequent8x8PatternColorExpand(patternx, patterny, x, y, w, h)
    unsigned patternx, patterny;
    int x, y, w, h;
{
    int 		dstaddr;
    register unsigned char 	*patternRegPtr ;
    register unsigned char 	*srcPatternRegPtr ;    
    register unsigned int 	*patternRegPtrL ;
    int			i, k ;
    unsigned short 	tmp;
    int			shift ;

    dstaddr = ( y * vga256InfoRec.displayWidth + x ) * vgaBytesPerPixel;

    /*ErrorF("SISSubsequent8x8PatternColorExpand(%d %d %d %d %d %d)\n",
	   patternx, patterny, x, y, w, h);*/
    sisBLTWAIT;
    patternRegPtr = sisSETPATREG();
    srcPatternRegPtr = (unsigned char *)sisPatternReg ;
    shift = 8-patternx ;
    for ( i = 0, k = patterny ; i < 8 ; i++, k++ ) {
	tmp = srcPatternRegPtr[k]<<8 | srcPatternRegPtr[k] ;
	tmp >>= shift ;
	patternRegPtr[i] = tmp & 0xff ;
    }
    patternRegPtrL = (unsigned int *)sisSETPATREG();
    for ( i = 2 ; i < sisPatternHeight ;   ) {
	patternRegPtrL[i++] = patternRegPtrL[0];
	patternRegPtrL[i++] = patternRegPtrL[1];
    }

    sisSETDSTADDR(dstaddr);
    sisSETHEIGHTWIDTH(h-1, w*vgaBytesPerPixel-1);
    sisSETCMD(sisColExp_op);
}






