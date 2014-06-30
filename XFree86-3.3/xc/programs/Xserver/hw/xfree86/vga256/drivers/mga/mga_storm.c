/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/mga/mga_storm.c,v 1.1.2.2 1997/05/25 05:06:51 dawes Exp $ */

/*
 * This is a sample driver implementation template for the new acceleration
 * interface.
 */

#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#include "vgaPCI.h"

#include "miline.h"

#include "xf86xaa.h"

#include "mga.h"
#include "mga_reg.h"
#include "mga_map.h"

/*
 * forward definitions for once only compiled functions.
 */
void MGAStormAccelInit();
void MGAStormSync();
void MGAStormEngineInit();

/*
 * forward definitions for the functions in this file.
 * MGANAME macro gives correct name for current depth
 */
static void MGANAME(SetupForFillRectSolid)();
static void MGANAME(SubsequentFillRectSolid)();
static void MGANAME(SubsequentFillTrapezoidSolid)();
static void MGANAME(SetupForScreenToScreenCopy)();
static void MGANAME(SubsequentScreenToScreenCopy)();
static void MGANAME(SetupForCPUToScreenColorExpand)();
static void MGANAME(SubsequentCPUToScreenColorExpand)();
static void MGANAME(SetupForScreenToScreenColorExpand)();
static void MGANAME(SubsequentScreenToScreenColorExpand)();
static void MGANAME(SetupFor8x8PatternColorExpand)();
static void MGANAME(Subsequent8x8PatternColorExpand)();
static void MGANAME(SubsequentTwoPointLine)();
static void MGANAME(SetupForDashedLine)();
static void MGANAME(SubsequentDashedBresenhamLine)();
static void MGANAME(SetClippingRectangle)();
  
static int mga_cmd, mga_lastcmd, mga_linecmd, mga_rop;
static int mga_sgn, mga_lastsgn, mga_lastcxright, mga_lastshift;
static int mgablitxdir, mgablitydir;
static int mga_ClipRect;
static int mga_useBLKopaqueExpand;

static CARD32 mgaDashedPatternBuf[4];	/* allocate 128 bits */
static CARD32 mgaStylelen;

/*
 * The following function sets up the supported acceleration. Call it
 * from the FbInit() function in the SVGA driver.
 */
void MGANAME(AccelInit)() 
{
    int tmp;
    
    /*
     * If you to disable acceleration, just don't modify anything
     * in the AccelInfoRec.
     */

    /*
     * Set up the main flags acceleration.
     * Usually, you will want to use BACKGROUND_OPERATIONS,
     * and if you have ScreenToScreenCopy, use the PIXMAP_CACHE.
     */
    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | 
                             PIXMAP_CACHE | 
                             HARDWARE_CLIP_LINE |
                             USE_TWO_POINT_LINE |
                             TWO_POINT_LINE_NOT_LAST |
			     /* LINE_PATTERN_MSBFIRST_DECREASING | */
                             NO_SYNC_AFTER_CPU_COLOR_EXPAND;

    /* pattern flags */
    xf86AccelInfoRec.Flags |= HARDWARE_PATTERN_PROGRAMMED_BITS |
                             HARDWARE_PATTERN_PROGRAMMED_ORIGIN |
                             HARDWARE_PATTERN_SCREEN_ORIGIN |
                             HARDWARE_PATTERN_BIT_ORDER_MSBFIRST |
                             HARDWARE_PATTERN_MONO_TRANSPARENCY;
    


    /*
     * install hardware lines and clipping
     */

    xf86AccelInfoRec.SubsequentTwoPointLine = MGANAME(SubsequentTwoPointLine);
    xf86AccelInfoRec.SetClippingRectangle = MGANAME(SetClippingRectangle);

#if 0 /* wait for XAA suport */
    xf86AccelInfoRec.SetupForDashedLine = MGANAME(SetupForDashedLine);
    xf86AccelInfoRec.SubsequentDashedBresenhamLine = MGANAME(SubsequentDashedBresenhamLine);
    xf86AccelInfoRec.LinePatternBuffer = (void *) &mgaDashedPatternBuf[0];
    xf86AccelInfoRec.LinePatternMaxLength = 128;
    xf86AccelInfoRec.ErrorTermBits = 15;
#endif

    /*
     * The following line installs a "Sync" function, that waits for
     * all coprocessor operations to complete.
     */
    xf86AccelInfoRec.Sync = MGAStormSync;

    /*
     * We want to set up the FillRectSolid primitive for filling a solid
     * rectangle. First we set up the flags for the graphics operation.
     * It may include GXCOPY_ONLY, NO_PLANEMASK, and RGB_EQUAL.
     */
    xf86GCInfoRec.PolyFillRectSolidFlags = 0;

#if PSZ == 24
    /*
     * In 24 bpp, all three colors must have the same mask
     */
    xf86GCInfoRec.PolyFillRectSolidFlags |= NO_PLANEMASK;
#endif

    /*
     * Install the low-level functions for drawing solid filled rectangles.
     */
    xf86AccelInfoRec.SetupForFillRectSolid   = 
    				MGANAME(SetupForFillRectSolid);
    xf86AccelInfoRec.SubsequentFillRectSolid = 
    				MGANAME(SubsequentFillRectSolid);
#if 0  /* wait for XAA support */
    xf86AccelInfoRec.SubsequentFillTrapezoidSolid =
    				MGANAME(SubsequentFillTrapezoidSolid);
#endif

    /*
     * We also want to set up the ScreenToScreenCopy (BitBLT) primitive for
     * copying a rectangular area from one location on the screen to
     * another. First we set up the restrictions. In this case, we
     * don't handle transparency color compare. Other allowed flags are
     * GXCOPY_ONLY and NO_PLANEMASK.
     */
    xf86GCInfoRec.CopyAreaFlags = NO_TRANSPARENCY;
#if PSZ == 24
    xf86GCInfoRec.CopyAreaFlags |= NO_PLANEMASK;
#endif
    
    /*
     * Install the low-level functions for screen-to-screen copy.
     */
    xf86AccelInfoRec.SetupForScreenToScreenCopy =
        			MGANAME(SetupForScreenToScreenCopy);
    xf86AccelInfoRec.SubsequentScreenToScreenCopy =
        			MGANAME(SubsequentScreenToScreenCopy);

    /*
     * color expansion
     */
    xf86AccelInfoRec.ColorExpandFlags = SCANLINE_PAD_DWORD |
                                        CPU_TRANSFER_PAD_DWORD |
                                        BIT_ORDER_IN_BYTE_LSBFIRST |
                                        LEFT_EDGE_CLIPPING |
                                        LEFT_EDGE_CLIPPING_NEGATIVE_X |
                                        VIDEO_SOURCE_GRANULARITY_PIXEL; 
#if PSZ == 24
    xf86AccelInfoRec.ColorExpandFlags |= NO_PLANEMASK;
#endif

    /* Mystique can't do opaque color expansion in BLOCK access type */
    mga_useBLKopaqueExpand = (MGAchipset == PCI_CHIP_MGA2064);
    
    xf86AccelInfoRec.SetupForScreenToScreenColorExpand =
    			MGANAME(SetupForScreenToScreenColorExpand);
    xf86AccelInfoRec.SubsequentScreenToScreenColorExpand =
    			MGANAME(SubsequentScreenToScreenColorExpand);
    					
    xf86AccelInfoRec.SetupForCPUToScreenColorExpand = 
				MGANAME(SetupForCPUToScreenColorExpand);
    xf86AccelInfoRec.SubsequentCPUToScreenColorExpand = 
				MGANAME(SubsequentCPUToScreenColorExpand);
    
#ifdef __alpha__
    xf86AccelInfoRec.CPUToScreenColorExpandBase = (unsigned*)MGAMMIOBaseDENSE;
#else /* __alpha__ */
    xf86AccelInfoRec.CPUToScreenColorExpandBase = (unsigned*)MGAMMIOBase;
#endif /* __alpha__ */
    xf86AccelInfoRec.CPUToScreenColorExpandRange = 0x1C00;

    /*
     * 8x8 color expand pattern fill
     */
    xf86AccelInfoRec.SetupFor8x8PatternColorExpand =
    			MGANAME(SetupFor8x8PatternColorExpand);
    xf86AccelInfoRec.Subsequent8x8PatternColorExpand =
    			MGANAME(Subsequent8x8PatternColorExpand);
     
    /*
     * Finally, we set up the video memory space available to the pixmap
     * cache. In this case, all memory from the end of the virtual screen
     * to the end of video memory minus 1K, can be used.
     */
    {
        int cacheStart = (vga256InfoRec.virtualY * vga256InfoRec.displayWidth
                            + MGAydstorg) * vga256InfoRec.bitsPerPixel / 8;
        int cacheEnd = vga256InfoRec.videoRam * 1024 - 1024;
        /*
         * we can't fast blit between first 4MB and second 4MB for interleave
         * and between first 2MB and other memory for non-interleave
         */
        int max_fastbitblt_mem = (MGAinterleave ? 4096 : 2048) * 1024;
     
        if( cacheStart > max_fastbitblt_mem - 4096 )
            MGAusefbitblt = 0;
        else
            if( cacheEnd > max_fastbitblt_mem - 1024 )
                cacheEnd = max_fastbitblt_mem - 1024;
        
        xf86InitPixmapCache(&vga256InfoRec, cacheStart, cacheEnd);
    }
}


#if PSZ == 8

/* functions specific to this chipset, to be compiled just the once */

/*
 * The following function sets up the supported acceleration. Call it
 * from the FbInit() function in the SVGA driver.
 */
void MGAStormAccelInit() {
    switch( vgaBitsPerPixel )
    {
    case 8:
    	Mga8AccelInit();
    	break;
    case 16:
    	Mga16AccelInit();
    	break;
    case 24:
    	Mga24AccelInit();
    	break;
    case 32:
    	Mga32AccelInit();
    	break;
    }
}

/*
 * This is the implementation of the Sync() function.
 */
void MGAStormSync() 
{
    int i, j;
    
    /*
     * Flush the read cache (SDK 5-2)
     * It doesn't matter which VGA register we write,
     * so we pick one that's not used in "Power" mode.
     */
    OUTREG8(MGAREG_CRTC_INDEX, 0);
     
    if (!MGAISBUSY())
        return;
    
    for (j = 8; j > 0; j--) {    
        for (i = 10000000; i > 0; i--)
            if (!MGAISBUSY())
                return;
        OUTREG8(MGAREG_OPMODE, 0); /* terminate DMA sequence */
        ErrorF("MGA: Drawing Engine time-out.\n");
    }
    FatalError("MGA: Drawing Engine time-out.\n");
}

/*
 * Global initialization of drawing engine
 */
void MGAStormEngineInit()
{
    long maccess;
    
    switch( vgaBitsPerPixel )
    {
    case 8:
        maccess = 0;
        break;
    case 16:
	/* set 16 bpp, turn off dithering, turn on 5:5:5 pixels */
        maccess = 1 + (1 << 30) + (1 << 31);
        break;
    case 24:
        maccess = 3;
        break;
    case 32:
        maccess = 2;
        break;
    }
    
    OUTREG(MGAREG_PITCH, vga256InfoRec.displayWidth);
    OUTREG(MGAREG_YDSTORG, MGAydstorg);
    OUTREG(MGAREG_MACCESS, maccess);
    OUTREG(MGAREG_PLNWT, ~0);
    OUTREG(MGAREG_OPMODE, 0);
}

#endif /* PSZ == 8 */ /* once only compilation */

  
/*
 * Replicate colors and planemasks
 */ 
#if PSZ == 8 
#define REPLICATE(pixel)                              \
      pixel &= 0xff; pixel |= (pixel << 24) | (pixel << 16) | (pixel << 8); 
#elif PSZ == 16 
#define REPLICATE(pixel)                              \
      pixel &= 0xffff; pixel |= (pixel << 16);
#else 
#define REPLICATE(pixel) ; 
#endif    

#define REPLICATE24(pixel) \
      pixel &= 0xffffff; pixel |= (pixel & 0xff) << 24;

/*
 * Disable clipping
 */
#define DISABLECLIPPING() \
    OUTREG(MGAREG_CXBNDRY, 0xFFFF0000);  /* (maxX << 16) | minX */ \
    OUTREG(MGAREG_YTOP, 0x00000000);  /* minPixelPointer */ \
    OUTREG(MGAREG_YBOT, 0x007FFFFF);  /* maxPixelPointer */ \
    mga_ClipRect = 0; /* one time line clipping is off */

/*
 * Use faster access type for certain rop's
 *
 * SETACCESSNOGXCOPY when rop isn't GXcopy
 * SETACCESS1 for any rop and foreground only used
 * SETACCESS2 for any rop and both foreground and background used
 */
/* 
 * when rop != GXcopy we can use RPL or RSTR access type 
 * (mga2064w pp 5-19, 5-23, 5-24, 5-29, 5-31, 5-33, 5-34, 5-37, 5-39)
 */  
#define SETACCESSNOGXCOPY(cmd,rop) \
    if( (rop == GXclear) || (rop == GXcopyInverted) || (rop == GXset) ) \
    	cmd |= MGADWG_RPL; \
    else \
    	cmd |= MGADWG_RSTR;

#if PSZ != 24

/*
 * when rop == GXcopy and bpp != 24 we can use BLOCK access type 
 * (mga2064w pp 5-23, 5-24, 5-33, 5-39)
 */
#define SETACCESS1(cmd,rop,fg) \
    if( rop == GXcopy ) { \
    	cmd |= MGADWG_BLK; \
    } else { \
        SETACCESSNOGXCOPY(cmd,rop); \
    }

#define SETACCESS2(cmd,rop,bg,fg,transc) \
    if( rop == GXcopy ) { \
        if( transc || mga_useBLKopaqueExpand ) \
    	    cmd |= MGADWG_BLK; \
    	else \
    	    cmd |= MGADWG_RPL; \
    } else { \
        SETACCESSNOGXCOPY(cmd,rop); \
    }

#else /* PSZ != 24 */

/*
 * in 24 bpp, when rop == GXcopy, we can use BLOCK atype
 * only if color is gray (R=G=B=A)
 */
#define RGBEQUAL(c) ( !(((c >> 16) ^ c) & 0xFF) && !(((c >> 8) ^ c) & 0xFF) )

/* 
 * SETACCESS1 checks only foreground color
 * (mga2064w pp 5-23) 
 */
#define SETACCESS1(cmd,rop,fg) \
    if( rop == GXcopy ) { \
        if( RGBEQUAL(fg) ) \
        { \
    	    REPLICATE24(fg); \
    	    mga_cmd |= MGADWG_BLK; \
    	} \
    	else \
    	    mga_cmd |= MGADWG_RPL; \
    } else { \
        SETACCESSNOGXCOPY(cmd,rop); \
    }

/*
 * SETACCESS2 checks both foreground and background colors.
 * The background color is checked only if it isn't transparent (transc flag).
 * (mga2064w pp 5-24, 5-33, 5-39)
 */
#define SETACCESS2(cmd,rop,bg,fg,transc) \
    if( rop == GXcopy ) { \
        if( (transc || mga_useBLKopaqueExpand) && \
            (transc || RGBEQUAL(bg)) && RGBEQUAL(fg) ) \
        { \
    	    REPLICATE24(bg); \
    	    REPLICATE24(fg); \
    	    cmd |= MGADWG_BLK; \
    	} \
    	else \
    	    cmd |= MGADWG_RPL; \
    } else { \
        SETACCESSNOGXCOPY(cmd,rop); \
    }

#endif /* PSZ != 24 */
             
/*    
 * This is the implementation of the SetupForFillRectSolid function
 * that sets up the coprocessor for a subsequent batch for solid
 * rectangle fills.
 */
void MGANAME(SetupForFillRectSolid)(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    mga_cmd = MGADWG_TRAP | MGADWG_NOZCMP | MGADWG_SOLID | MGADWG_ARZERO | 
    	       MGADWG_SGNZERO | MGADWG_SHIFTZERO | MGADWG_BMONOLEF;

    /* set atype - foreground color only used (mga2064w pp 5-23) */
    SETACCESS1(mga_cmd,rop,color);

    REPLICATE(color);
    SETFOREGROUNDCOLOR(color);
#if PSZ != 24
    REPLICATE(planemask);
    SETWRITEPLANEMASK(planemask);
#endif
    SETRASTEROP(rop);
    /* mga_lastcmd is used by TwoPointLine() to restore the FillRect state */
    OUTREG(MGAREG_DWGCTL, mga_lastcmd = mga_cmd);
    DISABLECLIPPING();

    /* now, we construct mga_linecmd by masking the opcod and optimising */
    /* the use of gxcopy rop. opcod is clear so we can draw lines quickly */

    mga_linecmd = MGADWG_NOZCMP | MGADWG_SOLID | MGADWG_SHIFTZERO | 
                   MGADWG_BFCOL;
    if ( rop == GXcopy )
	mga_linecmd |= mga_lastcmd & 0x000F0000; /* same bop, RPL atype */
    else
	mga_linecmd |= mga_lastcmd & 0x000F0070; /* same bop and atype */
}

/*
 * This is the implementation of the SubsequentFillRectSolid function
 * that sends commands to the coprocessor to fill a solid rectangle of
 * the specified location and size, with the parameters from the SetUp
 * call.
 */

void MGANAME(SubsequentFillRectSolid)(x, y, w, h)
    int x, y, w, h;
{
    OUTREG(MGAREG_FXBNDRY, ((x + w) << 16) | x);
    OUTREG(MGAREG_YDSTLEN + MGAREG_EXEC, (y << 16) | h);
}

/*
 * This is the implementation of the SubsequentFillTrapezoidSolid
 */
void MGANAME(SubsequentFillTrapezoidSolid)(y, h, left, dxL, dyL, eL, 
						right, dxR, dyR, eR)
    int y, h, left, dxL, dyL, eL, right, dxR, dyR, eR;
{
    int sdxl = (dxL < 0);
    int sdxr = (dxR < 0);
    
    /* change command because we are after SetupForFillRectSolid */
    OUTREG(MGAREG_DWGCTL, mga_cmd & ~(MGADWG_ARZERO | MGADWG_SGNZERO));

    OUTREG(MGAREG_AR0, dyL);
    OUTREG(MGAREG_AR1, sdxl? (dxL + dyL - 1) : -dxL);
    OUTREG(MGAREG_AR2, sdxl? dxL : -dxL);
    OUTREG(MGAREG_AR4, sdxr? (dxR + dyR - 1) : -dxR);
    OUTREG(MGAREG_AR5, sdxr? dxR : -dxR);
    OUTREG(MGAREG_AR6, dyR);
    OUTREG(MGAREG_SGN, (sdxl << 1) | (sdxr << 5));
    OUTREG(MGAREG_FXBNDRY, ((right + 1) << 16) | left);
    OUTREG(MGAREG_YDSTLEN + MGAREG_EXEC, (y << 16) | h);

    /* restore FillRect state for future rects */
    OUTREG(MGAREG_DWGCTL, mga_cmd);
}

/*
 * This is the implementation of the SetupForScreenToScreenCopy function
 * that sets up the coprocessor for a subsequent batch for solid
 * screen-to-screen copies. Remember, we don't handle transparency,
 * so the transparency color is ignored.
 */

void MGANAME(SetupForScreenToScreenCopy)(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
    int srcpitch;
    
    mga_rop = rop;
    mga_cmd = MGADWG_BITBLT | MGADWG_NOZCMP | MGADWG_SHIFTZERO | MGADWG_BFCOL;
    /*
     * now use faster access type for certain rop's
     * we can't use BLOCK atype for GXcopy (mga2064w 5-29)
     */
    if( rop == GXcopy )
        mga_cmd |= MGADWG_RPL;
    else {
        SETACCESSNOGXCOPY(mga_cmd,rop);
    }

    mgablitxdir = xdir;
    mgablitydir = ydir;
    if( ydir == 1 )     /* top - down */
    {
        mga_sgn = 0;
        srcpitch = xf86AccelInfoRec.FramebufferWidth;
    }
    else                /* bottom - up */
    {
        mga_sgn = 4;
        srcpitch = -xf86AccelInfoRec.FramebufferWidth;
    }

#if PSZ != 24
    REPLICATE(planemask);
    SETWRITEPLANEMASK(planemask);
#endif
    SETRASTEROP(rop);
    OUTREG(MGAREG_DWGCTL, mga_lastcmd = mga_cmd);
    OUTREG(MGAREG_AR5, srcpitch);
    DISABLECLIPPING();
    mga_lastsgn = -1;
    mga_lastcxright = 0xFFFF;  /* maxX */
}

/*
 * This is the implementation of the SubsequentForScreenToScreenCopy
 * that sends commands to the coprocessor to perform a screen-to-screen
 * copy of the specified areas, with the parameters from the SetUp call.
 * In this sample implementation, the direction must be taken into
 * account when calculating the addresses (with coordinates, it might be
 * a little easier).
 */
void MGANAME(SubsequentScreenToScreenCopy)(xsrc, ysrc, xdst, ydst, w, h)
    int xsrc, ysrc, xdst, ydst, w, h;
{
    long srcStart, srcStop;
    int cmd;
    int fxright = xdst + --w;
    int cxright = 0xFFFF;  /* maxX */
    /* check whether this blit can be converted to left-to-right blit */
    int left_to_right = ((mgablitxdir == 1) || (ysrc != ydst));
    
    /*
     * Try to use fast bitblt
     */
        /* alignment constraints (SDK 5-30)
        */
    if(
#if PSZ == 32
        !((xsrc ^ xdst) & 31)
#elif PSZ == 16
        !((xsrc ^ xdst) & 63)
#else
        !((xsrc ^ xdst) & 127)
#endif
        /* fast bitblt works only with GXcopy and from left to right
        */ 
        && (mga_rop == GXcopy) && left_to_right && MGAusefbitblt)
    {
        /* undocumented constraints
        */
#if PSZ == 8
        if( (xdst & (1 << 6)) && (((fxright >> 6) - (xdst >> 6)) & 7) == 7 )
            cxright = fxright, fxright |= 1 << 6;
#elif PSZ == 16
        if( (xdst & (1 << 5)) && (((fxright >> 5) - (xdst >> 5)) & 7) == 7 )
            cxright = fxright, fxright |= 1 << 5;
#elif PSZ == 24
        if( ((xdst * 3) & (1 << 6)) && 
                 ((((fxright * 3 + 2) >> 6) - ((xdst * 3) >> 6)) & 7) == 7 )
            cxright = fxright, fxright = ((fxright * 3 + 2) | (1 << 6)) / 3;
#elif PSZ == 32
        if( (xdst & (1 << 4)) && (((fxright >> 4) - (xdst >> 4)) & 7) == 7 )
            cxright = fxright, fxright |= 1 << 4;
#endif
        /* command for fast blit (mga2064w pp 5-30) */
        cmd = MGADWG_FBITBLT | MGADWG_RPL | MGADWG_NOZCMP | 
              MGADWG_SHIFTZERO | 0x000A0000 | MGADWG_BFCOL;
    }
    else
        /* normal blit */
        cmd = mga_cmd;
        
    if(mgablitydir != 1)    /* bottom to top */
    {
        ysrc += h - 1;
        ydst += h - 1;
    }
    if(left_to_right)    /* left to right */
    {
        srcStart = ysrc * xf86AccelInfoRec.FramebufferWidth + xsrc + MGAydstorg;
        srcStop  = srcStart + w;
        mga_sgn &= ~1;
    }
    else             /* right to left */
    {
        srcStop  = ysrc * xf86AccelInfoRec.FramebufferWidth + xsrc + MGAydstorg;;
        srcStart = srcStop + w;
        mga_sgn |= 1;
    }
 
    /* cmd, mga_sgn and cxright are constant in most cases
    */
    if(cmd != mga_lastcmd)
        OUTREG(MGAREG_DWGCTL, mga_lastcmd = cmd);
    if(mga_sgn != mga_lastsgn)
        OUTREG(MGAREG_SGN, mga_lastsgn = mga_sgn);
    if(cxright != mga_lastcxright)
        OUTREG(MGAREG_CXRIGHT, mga_lastcxright = cxright);

    OUTREG(MGAREG_FXBNDRY, (fxright << 16) | xdst);
    OUTREG(MGAREG_YDSTLEN, (ydst << 16) | h);
    OUTREG(MGAREG_AR3, srcStart);
    OUTREG(MGAREG_AR0 + MGAREG_EXEC, srcStop);
}

/*
 * setup for screen-to-screen color expansion
 */
void MGANAME(SetupForScreenToScreenColorExpand)(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned planemask;
{
    /* handle transparent background */
    int transc = ( bg == -1 );
    
    /* bitblt with expansion (mga2064w pp 5-33) */
    mga_cmd = MGADWG_BITBLT | MGADWG_NOZCMP | MGADWG_SGNZERO |
    	MGADWG_SHIFTZERO | MGADWG_BMONOLEF;
    
    /* set atype, both foreground and background used */
    SETACCESS2(mga_cmd,rop,bg,fg,transc)
    
    /*
     * check transparency 
     */
    if( transc )
        mga_cmd |= MGADWG_TRANSC;
    else
    {
        REPLICATE(bg);
        SETBACKGROUNDCOLOR(bg);
    }

    REPLICATE(fg);
    SETFOREGROUNDCOLOR(fg);
#if PSZ != 24
    REPLICATE(planemask);
    SETWRITEPLANEMASK(planemask);
#endif
    SETRASTEROP(rop);
    OUTREG(MGAREG_DWGCTL, mga_cmd);
    DISABLECLIPPING();
}

/*
 * executing screen-to-screen color expansion
 */
void MGANAME(SubsequentScreenToScreenColorExpand)(srcx, srcy, x, y, w, h)
    int srcx, srcy, x, y, w, h;
{
    int srcStart = srcy * xf86AccelInfoRec.FramebufferWidth * 8 + srcx
								+ MGAydstorg;
    int srcStop = srcStart + w - 1;

    OUTREG(MGAREG_AR3, srcStart);
    OUTREG(MGAREG_AR0, srcStop);
    OUTREG(MGAREG_AR5, (w + 31) & ~31);   /* source is DWORD-padded */
    OUTREG(MGAREG_FXBNDRY, ((x + w - 1) << 16) | x);
    OUTREG(MGAREG_YDSTLEN + MGAREG_EXEC, (y << 16) | h);
}

/*
 * setup for CPU-to-screen color expansion
 */
void MGANAME(SetupForCPUToScreenColorExpand)(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned planemask;
{
    int transc = ( bg == -1 );
    
    /* ILOAD with expansion (mga2064w pp 5-39) */
    mga_cmd = MGADWG_ILOAD | MGADWG_LINEAR | MGADWG_NOZCMP | MGADWG_SGNZERO |
    	MGADWG_SHIFTZERO | MGADWG_BMONOLEF;

    /* set atype, both foreground and background color used */
    SETACCESS2(mga_cmd,rop,bg,fg,transc)
    
    /*
     * check transparency 
     */
    if( transc )
        mga_cmd |= MGADWG_TRANSC;
    else
    {
        REPLICATE(bg);
        SETBACKGROUNDCOLOR(bg);
    }

    REPLICATE(fg);
    SETFOREGROUNDCOLOR(fg);
#if PSZ != 24
    REPLICATE(planemask);
    SETWRITEPLANEMASK(planemask);
#endif
    SETRASTEROP(rop);
    OUTREG(MGAREG_DWGCTL, mga_cmd);
    OUTREG16(MGAREG_OPMODE, MGAOPM_DMA_BLIT);
    OUTREG(MGAREG_YTOP, 0x00000000);  /* minPixelPointer */
    OUTREG(MGAREG_YBOT, 0x007FFFFF);  /* maxPixelPointer */
}

/*
 * executing CPU-to-screen color expansion
 */
void MGANAME(SubsequentCPUToScreenColorExpand)(x, y, w, h, skipleft)
    int x, y, w, h, skipleft;
{
    OUTREG(MGAREG_CXBNDRY, ((x + w - 1) << 16) | ((x + skipleft) & 0xffff));
    w = (w + 31) & ~31;     /* source is dword padded */
    OUTREG(MGAREG_AR0, (w * h) - 1);
    OUTREG(MGAREG_AR3, 0);            /* we need it here for stability */
    OUTREG(MGAREG_FXBNDRY, ((x + w - 1) << 16) | (x & 0xffff));
    OUTREG(MGAREG_YDSTLEN + MGAREG_EXEC, (y << 16) | h);
}

/*
 * setup for 8x8 color expand pattern fill
 */
void MGANAME(SetupFor8x8PatternColorExpand)(patternx, patterny, bg, fg,
                                            rop, planemask)
    unsigned patternx, patterny, planemask;
    int bg, fg, rop;
{
    int transc = ( bg == -1 );
    
    /* patterned rectangle fill (mga2064w pp 5-24) */
    mga_cmd = MGADWG_TRAP | MGADWG_NOZCMP | MGADWG_ARZERO | MGADWG_SGNZERO |
    	      MGADWG_BMONOLEF;

    /* set atype, both foreground and background used */
    SETACCESS2(mga_cmd,rop,bg,fg,transc)
    
    /*
     * check transparency 
     */
    if( transc )
        mga_cmd |= MGADWG_TRANSC;
    else
    {
        REPLICATE(bg);
        SETBACKGROUNDCOLOR(bg);
    }

    REPLICATE(fg);
    SETFOREGROUNDCOLOR(fg);
#if PSZ != 24
    REPLICATE(planemask);
    SETWRITEPLANEMASK(planemask);
#endif
    SETRASTEROP(rop);
    OUTREG(MGAREG_DWGCTL, mga_cmd);
    OUTREG(MGAREG_PAT0, patternx);      /* just store pattern */
    OUTREG(MGAREG_PAT1, patterny);      /* in chip registers */
    DISABLECLIPPING();
    mga_lastshift = -1;
}

/*
 * executing 8x8 color expand pattern fill
 */
void MGANAME(Subsequent8x8PatternColorExpand)(patternx, patterny, x, y, w, h)
    unsigned patternx, patterny;
    int x, y, w, h;
{
    int shift = (patterny << 4) | patternx;
    if( shift != mga_lastshift )
        OUTREG(MGAREG_SHIFT, mga_lastshift = shift);
    OUTREG(MGAREG_FXBNDRY, ((x + w) << 16) | x);
    OUTREG(MGAREG_YDSTLEN + MGAREG_EXEC, (y << 16) | h);
}

/*
 * MgaSubsequentTwoPointLine ()
 * by ajv 961116
 * 
 * changelog:
 *  961116 - started
 *  961118 - merged it with 3.2a, added capstyle, added compatibility with
 *           SetupForFillRectSolid (which does the setup for lines and
 * 	     rectangles)
 *  961120 - modified it so that fewer instructions are executed per line
 *           segment, and moved some code into SetupForFillRect so that
 * 	     that code is not constantly being (unnecessarily) reevaluated
 * 	     all the time.
 *  961121 - added one time only clipping as per Harm's notes. Also worked
 *           to introduce concurrency by doing more work behind the EXEC. 
 *           Reordered code for register starved CPU's (Intel x86) plus
 * 	     it achieves better locality of code for other processors.
 */

void
MGANAME(SubsequentTwoPointLine)(x1, y1, x2, y2, bias)
        int x1, y1, x2, y2, bias;
{
    register int mga_localcmd = mga_linecmd;

    /* draw the last pixel? */
    if ( bias & 0x0100 )
        mga_localcmd |= MGADWG_AUTOLINE_OPEN; /* no */
    else
        mga_localcmd |= MGADWG_AUTOLINE_CLOSE; /* yep */

    OUTREG(MGAREG_DWGCTL, mga_localcmd);
    OUTREG(MGAREG_XYSTRT, ( y1 << 16 ) | (x1 & 0xffff));
    OUTREG(MGAREG_XYEND + MGAREG_EXEC, ( y2 << 16 ) | (x2 & 0xffff));

    /* do some work whilst the chipset is busy */

    /* if clipping is on, disable it */
    if ( mga_ClipRect )
    {
	DISABLECLIPPING();
    }

    /* restore FillRect state for future rects */
    OUTREG(MGAREG_DWGCTL, mga_lastcmd);
}

void
MGANAME(SetupForDashedLine)(int fg, int bg, int rop, unsigned int planemask,
				 int size)
{
    /* handle transparent background */
    int transc = ( bg == -1 );

    MGAStormSync();

    /* load the style length part into the SHIFT register */

    mgaStylelen = ( (size - 1) << 16 ) & 0x007f0000;
    OUTREG(MGAREG_SHIFT, mgaStylelen);
    mgaStylelen = size;

    /* load the pattern */
    
    switch ( ((size + 31) >> 5) )
    {
 	case 4: OUTREG(MGAREG_SRC3, mgaDashedPatternBuf[3]); 
 	case 3: OUTREG(MGAREG_SRC2, mgaDashedPatternBuf[2]); 
 	case 2: OUTREG(MGAREG_SRC1, mgaDashedPatternBuf[1]); 
 	default: OUTREG(MGAREG_SRC0, mgaDashedPatternBuf[0]); 
    }

    /* closed Bresenham lines with a linestyle, p5-19 or p5-30 */
    mga_cmd = MGADWG_LINE_CLOSE | MGADWG_BFCOL;
    
    /*
     * check transparency and set bg/fg colors
     */

    if ( transc )
        mga_cmd |= MGADWG_TRANSC;
    else
    {
        REPLICATE(bg);
        SETBACKGROUNDCOLOR(bg);
    }

    REPLICATE(fg);
    SETFOREGROUNDCOLOR(fg);

    /* set planemask (if appropiate) */

#if PSZ != 24
    REPLICATE(planemask);
    SETWRITEPLANEMASK(planemask);
#endif

    /* set atype, based upon fastest rules */

    if ( rop == GXcopy ) 
    {
        mga_cmd |= MGADWG_RPL;
    }
    else
    {
        SETACCESSNOGXCOPY(mga_cmd, rop);
    }

    /* set rop (bop) */

    SETRASTEROP(rop);

    /* set up power drawing mode */

    OUTREG(MGAREG_DWGCTL, mga_cmd);

    /* we have to do this to allow the hw clipping to work */

    DISABLECLIPPING();
}

void 
MGANAME(SubsequentDashedBresenhamLine)(int x1, int y1, int octant, int err, 
                                       int e1, int e2, int length, int start)
{
    unsigned int oct = 0;
    unsigned int startPos = mgaStylelen - start;

#ifdef DEBUG
    if ( start < 0 )
	ErrorF("mga dashed lines: -ve start (%d, %d)\n", mgaStylelen, start);

    if ( startPos > mgaStylelen )
 	ErrorF("mga dashed lines: startpos > mgaStylelen (%d, %d)\n", mgaStylelen, start);

    if ( startPos > 127 )
        ErrorF("mga dashed lines: startPos > 127 (%d, %d)\n", mgaStylelen, start);
#endif

    OUTREG16(MGAREG_SHIFT, startPos & 0x7f);

    /* load the xy position. The Mill documentation has an error, xdst not ydst */

    OUTREG16(MGAREG_XDST, x1);
    OUTREG(MGAREG_YDSTLEN, (y1 << 16) | length);

    if ( octant & YDECREASING )
	oct |= 4;

    if ( octant & XDECREASING )
	oct |= 2;

    if ( !((octant & YMAJOR) == YMAJOR) )
	oct |= 1;

    OUTREG16(MGAREG_SGN, oct);
    OUTREG(MGAREG_AR0, e1 & 0x3ffff);
    OUTREG(MGAREG_AR1, err & 0x0fffffff);
    OUTREG(MGAREG_AR2 + MGAREG_EXEC, e2 & 0x3ffff);

    /* disable one-time clipping (see XAA notes) */
    if ( mga_ClipRect )
    {
	DISABLECLIPPING();
    }
}

void
MGANAME(SetClippingRectangle)(x1, y1, x2, y2)
        int x1, y1, x2, y2;
{
        int tmp;

#if 0  /* [rdk] I think we don't need it, see xaa/NOTES */
        if ( x2 < x1 )
        {
                tmp = x2;
                x2 = x1;
                x1 = tmp;
        }
        if ( y2 < y1 )
        {
                tmp = y2;
                y2 = y1;
                y1 = tmp;
        }
#endif

        OUTREG(MGAREG_CXBNDRY, (x2 << 16) | x1);
        OUTREG(MGAREG_YTOP,
	       y1 * xf86AccelInfoRec.FramebufferWidth + MGAydstorg);
        OUTREG(MGAREG_YBOT,
	       y2 * xf86AccelInfoRec.FramebufferWidth + MGAydstorg);

	/* indicate to TwoPoint Line that one time only clipping is on */
	mga_ClipRect = 1;
}
