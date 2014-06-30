/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */
/*
 * The CyberPro2010 driver is based on sample code provided by IGS 
 * Technologies, Inc. http://www.integraphics.com.
 * IGS Technologies, Inc makes no representations about the suitability of 
 * this software for any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */
/*
** IGS Acceleration Routines - Modified from ct_accel.c for use with
** IGS CyberPro2010.
*/
#include "vga256.h"
#include "compiler.h"
#include "xf86.h"
#include "vga.h"
#include "xf86xaa.h"
#include "xf86local.h"
#include "igs_driver.h"
#include "igs_accel.h"

__inline__ void igsSync();
__inline__ void igsWaitHostBltAck();
__inline__ void igsWaitCpuFifo();
void igsSetupForScreenToScreenCopy();
void igsSubsequentScreenToScreenCopy();
void igs24SetupForScreenToScreenCopy();
void igs24SubsequentScreenToScreenCopy();
void igsSetupForFillRectSolid();
void igsSubsequentFillRectSolid();
void igs24SubsequentFillRectSolid();
void igsSetupForCPUToScreenColorExpand();
void igsSubsequentCPUToScreenColorExpand();
void igs24SetupForCPUToScreenColorExpand();
void igs24SubsequentCPUToScreenColorExpand();
void igsSetupForFill8x8Pattern();
void igs24SetupForFill8x8Pattern();
void igsSetupFor8x8PatternColorExpand();
void igs24SetupFor8x8PatternColorExpand();
void igsSubsequentFill8x8Pattern();
void igs24SubsequentFill8x8Pattern();
void igsSubsequent8x8PatternColorExpand();
void igs24Subsequent8x8PatternColorExpand();
extern void igsImageTextTE();
extern void igsImageTextNonTE();
extern void igsPolyTextTE();
extern void igsPolyTextNonTE();

RegionPtr igsCopyArea();
void igsDoPixWinBitBltCopy();

static unsigned int CommandFlags;

void igsAccelInit() 
{
    char miscCtrl;
    int scanlinebytes;

#ifdef ACCEL_DEBUG
    ErrorF("igsAccelInit\n");
#endif    

    /*
     ** Set up the main acceleration flags.
     */
    xf86AccelInfoRec.Flags =  BACKGROUND_OPERATIONS | PIXMAP_CACHE |
	HARDWARE_PATTERN_MONO_TRANSPARENCY | HARDWARE_PATTERN_TRANSPARENCY | 
	    HARDWARE_PATTERN_MOD_64_OFFSET | 
		HARDWARE_PATTERN_BIT_ORDER_MSBFIRST;
    
    /*
     * The following line installs a "Sync" function, that waits for
     * all coprocessor operations to complete.
     */
    xf86AccelInfoRec.Sync = igsSync;

    /* 
     * Setup a Screen to Screen copy (BitBLT) primitive
     */  
    xf86GCInfoRec.CopyAreaFlags = NO_PLANEMASK;
#ifdef notyet
    xf86GCInfoRec.CopyArea = igsCopyArea;
#endif
    if(vga256InfoRec.bitsPerPixel != 24)
    {
	xf86AccelInfoRec.SetupForScreenToScreenCopy =
	    igsSetupForScreenToScreenCopy;
	xf86AccelInfoRec.SubsequentScreenToScreenCopy =
	    igsSubsequentScreenToScreenCopy;
    }
#ifdef IGS_ACCEL24
    else
    {
	xf86AccelInfoRec.SetupForScreenToScreenCopy =
	    igs24SetupForScreenToScreenCopy;
	xf86AccelInfoRec.SubsequentScreenToScreenCopy =
	    igs24SubsequentScreenToScreenCopy;
    }
#endif

    /*
     * Install the low-level functions for drawing solid filled rectangles.
     */
    xf86GCInfoRec.PolyFillRectSolidFlags = NO_PLANEMASK;
    xf86AccelInfoRec.SetupForFillRectSolid = 
	igsSetupForFillRectSolid;
    if(vga256InfoRec.bitsPerPixel != 24) 
    {
	xf86AccelInfoRec.SubsequentFillRectSolid = 
	    igsSubsequentFillRectSolid;
    }
#ifdef IGS_ACCEL24
    else
    {
	xf86AccelInfoRec.SubsequentFillRectSolid = 
	    igs24SubsequentFillRectSolid;
    }
#endif
#ifdef __arm32__ 
    /*
     * Set up text acceleration.
     * So far only defined for arm32, bit order may vary for others.
     */
    xf86GCInfoRec.PolyGlyphBltTE = xf86PolyGlyphBltTE;
    xf86AccelInfoRec.PolyTextTE = igsPolyTextTE;
    xf86GCInfoRec.PolyGlyphBltNonTE = xf86PolyGlyphBltNonTE;
    xf86AccelInfoRec.PolyTextNonTE = igsPolyTextNonTE;
    xf86GCInfoRec.ImageGlyphBltTE = xf86ImageGlyphBltTE;
    xf86AccelInfoRec.ImageTextTE = igsImageTextTE;
    xf86GCInfoRec.ImageGlyphBltNonTE = xf86ImageGlyphBltNonTE;
    xf86AccelInfoRec.ImageTextNonTE = igsImageTextNonTE;
#endif
    
    /*
     * Setup the functions that perform monochrome colour expansion
     */
    xf86AccelInfoRec.ColorExpandFlags = SCANLINE_PAD_DWORD |
	CPU_TRANSFER_PAD_DWORD | VIDEO_SOURCE_GRANULARITY_DWORD | 
	    BIT_ORDER_IN_BYTE_MSBFIRST | NO_PLANEMASK | GXCOPY_ONLY;
#ifdef notyet
    /*
     * These routines would work if you could guarantee that the
     * destination was always aligned to an 8 bit boundary.
     */
    if (vga256InfoRec.bitsPerPixel != 24) 
    {	
	xf86AccelInfoRec.SetupForCPUToScreenColorExpand =
	    igsSetupForCPUToScreenColorExpand;
	xf86AccelInfoRec.SubsequentCPUToScreenColorExpand = 
	    igsSubsequentCPUToScreenColorExpand;
    }
#ifdef IGS_ACCEL24
    else
    {
	xf86AccelInfoRec.SetupForCPUToScreenColorExpand =
	    igs24SetupForCPUToScreenColorExpand;
	xf86AccelInfoRec.SubsequentCPUToScreenColorExpand = 
	    igs24SubsequentCPUToScreenColorExpand;
    }
#endif
#endif

    /* XAA has problems with 24bpp pattern tile. */
    if (vga256InfoRec.bitsPerPixel != 24) 
    {
#ifdef notyet /* This could work, but hasn't been tested */
        xf86AccelInfoRec.SetupForFill8x8Pattern =
	    igsSetupForFill8x8Pattern;
        xf86AccelInfoRec.SubsequentFill8x8Pattern =
            igsSubsequentFill8x8Pattern;
#endif
        xf86AccelInfoRec.SetupFor8x8PatternColorExpand =
	    igsSetupFor8x8PatternColorExpand;
        xf86AccelInfoRec.Subsequent8x8PatternColorExpand =
            igsSubsequent8x8PatternColorExpand;
    }

    xf86InitPixmapCache(&vga256InfoRec, (vga256InfoRec.virtualY + 1) *
        vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8,
        igsCacheEnd);    
}

/* Wait until the graphics engine is not busy and the CPU 
   fifo is empty */
__inline__ void igsSync() 
{
    unsigned long WaitCount = 0x00FFFFF0;

#ifdef IGS_SYNC_HACK
    *igsPixMapFormat = vgaBytesPerPixel - 1;
#endif

    while(WaitCount != 0)
    {
	if ((*igsCopControl & 0x82) == 0)
	{
	    return;
	}
	WaitCount --;
    }
    ErrorF("Warning: igsSync Forcing coprocessor reset\n");
    *igsCopControl = 0;
}

/* Wait for Host Blt acknowledge and Command Fifo empty */
__inline__ void igsWaitHostBltAck() 
{
    unsigned long WaitCount = 0x00FFFFF0;
    
#ifdef IGS_SYNC_HACK
    *igsPixMapFormat = vgaBytesPerPixel - 1;
#endif

    while(WaitCount!=0)
    {
	if ((*igsCopControl & 0x44) == 0)
	{
	    return;
	}
	WaitCount --;
    }
    ErrorF("Warning: igsWaitHostBltAck Forcing coprocessor reset\n");
    *igsCopControl = 0;
}

/* Wait until the CPU fifo is empty */
__inline__ void igsWaitCpuFifo() 
{
    unsigned long WaitCount = 0x00FFFFF0;

#ifdef IGS_SYNC_HACK
    *igsPixMapFormat = vgaBytesPerPixel - 1;
#endif

    while(WaitCount != 0)
    {
	if ((*igsCopControl & 0x02) == 0)
	{
	    return;
	}
	WaitCount --;
    }
    ErrorF("Warning: igsWaitCpuFifo Forcing coprocessor reset\n");
    *igsCopControl = 0;
}

                                                                  
/*
 * Screen-to-screen BitBLT.
 */
 
void igsSetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSetupForScreenToScreenCopy\n");
#endif   
    CommandFlags  = igsPxBlt | igsForeSrcPxMap;
    CommandFlags |= igsPatternForeground;

    /* Set up the blit direction. */
    if (ydir < 0)
    {
	if(xdir < 0)
	    CommandFlags |= igsRLUp;
	else
	    CommandFlags |= igsLRUp;
    }
    else
    {
	if(xdir < 0)
	    CommandFlags |= igsRLDown;
	else
	    CommandFlags |= igsLRDown;
    }
    
    igsSync();
    *igsCopControl = 0;

    if (transparency_color == -1)
    {
	*igsColorCompVal = igsDontCompare;
    }
    else
    {
	*igsBackMix = GXnoop;
	*igsColorCompVal = igsCompareNeq;
	*igsDestColorComp = transparency_color; 
    }
   
    *igsForeMix = (char)rop;
    *igsPxBitMask = planemask;
    
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;

    *igsPixMapFormat = vgaBytesPerPixel - 1;
}

void igs24SetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24SetupForScreenToScreenCopy\n");
#endif   
    CommandFlags  = igsPxBlt | igsForeSrcPxMap;
    CommandFlags |= igsPatternForeground;

    /* Set up the blit direction. */
    if (ydir < 0)
    {
	if(xdir < 0)
	    CommandFlags |= igsRLUp;
	else
	    CommandFlags |= igsLRUp;
    }
    else
    {
	if(xdir < 0)
	    CommandFlags |= igsRLDown;
	else
	    CommandFlags |= igsLRDown;
    }

    igsSync();
    *igsCopControl = 0;

    if (transparency_color == -1)
    {
	*igsColorCompVal = igsDontCompare;
    }
    else
    {
	*igsBackMix = GXnoop;
	*igsColorCompVal = igsCompareNeq;
	*igsDestColorComp = transparency_color; 
    }
   
    *igsForeMix = (char)rop;
    *igsPxBitMask = planemask;

    *igsPixMapWidth = vga256InfoRec.displayWidth * 3 - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth * 3 - 1;
    
    *igsPixMapFormat = vgaBytesPerPixel - 1;
}

void igsSubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSubsequentScreenToScreenCopy\n");
#endif   
    igsSync();

    /* Set up the coordinates */
    switch(CommandFlags & 0x06)
    {
	case igsLRDown:
	*igsDstStartPtr  = (unsigned long)(y2 * vga256InfoRec.displayWidth 
					   + x2);
	*igsSrcStartPtr  = (unsigned long)(y1 * vga256InfoRec.displayWidth 
					   + x1);
	break;
	
	case igsRLDown:
	*igsDstStartPtr  = (unsigned long)(y2 * vga256InfoRec.displayWidth 
					   + x2 + w - 1);
	*igsSrcStartPtr  = (unsigned long)(y1 * vga256InfoRec.displayWidth 
					   + x1 + w - 1);
	break;

	case igsLRUp:
	*igsDstStartPtr  = (unsigned long)((y2 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x2);
	*igsSrcStartPtr  = (unsigned long)((y1 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x1);
	break;

	case igsRLUp:
	*igsDstStartPtr  = (unsigned long)((y2 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x2 + w - 1);
	*igsSrcStartPtr  = (unsigned long)((y1 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x1 + w - 1);
	break;
    }

    *igsOpDimension1 = w - 1; /* source width in pixels */
    *igsOpDimension2 = h - 1; /* height */

    /* Write to high word starts operation */
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 
}

void igs24SubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24SubsequentScreenToScreenCopy\n");
#endif   
    igsSync();
    /* Set up the coordinates */
    switch(CommandFlags & 0x06)
    {
	case igsLRDown:
	*igsDstStartPtr  = (unsigned long)(y2 * vga256InfoRec.displayWidth 
					   + x2) * 3;
	*igsSrcStartPtr  = (unsigned long)(y1 * vga256InfoRec.displayWidth 
					   + x1) * 3;
	break;
	
	case igsRLDown:
	*igsDstStartPtr  = (unsigned long)(y2 * vga256InfoRec.displayWidth 
					   + x2 + w - 1) * 3;
	*igsSrcStartPtr  = (unsigned long)(y1 * vga256InfoRec.displayWidth 
					   + x1 + w - 1) * 3;
	break;

	case igsLRUp:
	*igsDstStartPtr  = (unsigned long)((y2 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x2) * 3;
	*igsSrcStartPtr  = (unsigned long)((y1 + h - 1)
					   * vga256InfoRec.displayWidth 
					   + x1) * 3;
	break;

	case igsRLUp:
	*igsDstStartPtr  = (unsigned long)((y2 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x2 + w - 1) * 3;
	*igsSrcStartPtr  = (unsigned long)((y1 + h - 1) 
					   * vga256InfoRec.displayWidth 
					   + x1 + w - 1) * 3;
	break;
    }

    *igsOpDimension1 = w - 1; /* source width in pixels */
    *igsOpDimension2 = h - 1; /* height */

    *igsDstXPhase    = (unsigned char)(y2 * vga256InfoRec.displayWidth 
				       + x2);
    /* Write to high word starts operation */
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 
}

/*
 * Solid rectangle fill.
 */

void igsSetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSetupForFillRectSolid\n");
#endif   
    CommandFlags  = igsPxBlt | igsForeSrcColor;
    CommandFlags |= igsPatternForeground;

    igsSync();
    *igsCopControl = 0;

    *igsForeColor = color;
    *igsForeMix = (char)rop;
    *igsPxBitMask = planemask;
    
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;

    *igsPixMapFormat = vgaBytesPerPixel - 1;
}

void igsSubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSubsequentFillRectSolid\n");
#endif   
    igsSync();
    /* Set up the coordinates */
    *igsOpDimension1 = w-1; /* source width in pixels */
    *igsOpDimension2 = h-1; /* height */
    
    *igsDstStartPtr  = (unsigned long)(y * vga256InfoRec.displayWidth + x);
 
    /* Write to high word starts operation */
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 
}

void igs24SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24SubsequentFillRectSolid\n");
#endif   
    igsSync();
    /* Set up the coordinates */
    *igsOpDimension1 = w-1; /* source width in pixels */
    *igsOpDimension2 = h-1; /* height */

    *igsDstStartPtr  = (unsigned long)(y * vga256InfoRec.displayWidth
				       + x) * 3;
    *igsDstXPhase    = (unsigned char)(y * vga256InfoRec.displayWidth
				       + x);
    

    /* Write to high word starts operation */
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 
}

/*
 * Colour expansion.
 */

void igsSetupForCPUToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSetupForCPUToScreenColorExpand\n");
#endif   

    CommandFlags = igsPxBltTern | igsForeSrcColor | igsBackSrcColor |
	 igsHostBltWrSrc2 | igsLRDown | igsFixFgPat;

    igsSync();
    *igsCopControl = 0;
    
    *igsPxBitMask = planemask;
    *igsForeMix = igsPatCopy;
    *igsForeColor = fg;
    
    if (bg == -1)
    {
	*igsBackMix = GXnoop;
	CommandFlags |= igsColexpTra;
    }
    else
    {
	*igsBackMix = rop;
	*igsBackColor = bg;
	CommandFlags |= igsColexpOpq;
    }

    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;  
    *igsPixMapWidth = vga256InfoRec.displayWidth - 1;
    *igsSrc2MapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = 0x00;
    *igsSrc2StartPtr = 0x00;
}

void igs24SetupForCPUToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24SetupForCPUToScreenColorExpand\n");
#endif   
    CommandFlags = igsPxBltTern | igsForeSrcColor | igsBackSrcColor |
	 igsHostBltWrSrc2 | igsLRDown | igsFixFgPat;

    igsSync();
    *igsCopControl = 0;
    
    *igsPxBitMask = planemask;
    *igsForeMix = igsPatCopy;
    *igsForeColor = fg;
    
    if (bg == -1)
    {
	*igsBackMix = GXnoop;
	CommandFlags |= igsColexpTra;
    }
    else
    {
	*igsBackMix = rop;
	*igsBackColor = bg;
	CommandFlags |= igsColexpOpq;
    }
    
    *igsDstMapWidth = vga256InfoRec.displayWidth * 3 - 1;
    *igsPixMapWidth = vga256InfoRec.displayWidth * 3 - 1;
    *igsSrc2MapWidth = vga256InfoRec.displayWidth * 3 - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;
  
    *igsSrcStartPtr = 0x00;
    *igsSrc2StartPtr = 0x00;
}

void igsSubsequentCPUToScreenColorExpand(x, y, w, h, skipleft)
    int x, y, w, h, skipleft;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSubsequentCPUToScreenColorExpand\n");
#endif   
    if(w < 0)
	FatalError("igsSubsequentCPUToScreenColorExpand: negative w value\n");
    
    igsSync();
    
    *igsOpDimension1 = w - 1;
    *igsOpDimension2 = h - 1;

    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth 
				      + x + skipleft);
    
    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 	

    igsWaitHostBltAck();
}

void igs24SubsequentCPUToScreenColorExpand(x, y, w, h, skipleft)
    int x, y, w, h, skipleft;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24SubsequentCPUToScreenColorExpand\n");
#endif   
    igsSync();
    
    *igsOpDimension1 = w - 1;
    *igsOpDimension2 = h - 1;
    
    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth 
				      + x + skipleft) * 3;

    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16); 	

    igsWaitHostBltAck();
}

/*
 * 8x8 Pattern Fill
 */
void igsSetupForFill8x8Pattern(patternx, patterny, rop, planemask,
transparency_color)
    int patternx, patterny, rop;
    unsigned planemask;
    int transparency_color;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSetupForFill8x8Pattern\n");
#endif   
    CommandFlags = igsPxBlt | igsForeSrcPxMap | igsLRDown;

    igsSync();
    *igsCopControl = 0;
    *igsForeMix = rop;
    *igsPxBitMask = planemask;

    if (transparency_color != -1)
    {
	CommandFlags |= igsSrcTilTra;
	*igsColorCompVal = igsCompareNeq;
	*igsDestColorComp = transparency_color; 
	*igsBackMix = GXnoop;
    }
    else
    {
	CommandFlags |= igsSrcTilOpq;
	*igsColorCompVal = igsDontCompare;
	*igsBackMix = rop;
    }

    *igsPixMapWidth = 8 - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;
}

void igs24SetupForFill8x8Pattern(patternx, patterny, rop, planemask,
transparency_color)
    int patternx, patterny, rop;
    unsigned planemask;
    int transparency_color;
{
    int yPhase;

#ifdef ACCEL_DEBUG
    ErrorF("igs24SetupForFill8x8Pattern\n");
#endif       
    CommandFlags = igsPxBlt | igsForeSrcPxMap | igsLRDown;

    igsSync();
    *igsCopControl = 0;
    *igsForeMix = rop;
    *igsPxBitMask = planemask;

    if (transparency_color != -1)
    {
	CommandFlags |= igsSrcTilTra;
	*igsColorCompVal = igsCompareNeq;
	*igsDestColorComp = transparency_color; 
	*igsBackMix = GXnoop;
    }
    else
    {
	CommandFlags |= igsSrcTilOpq;
	*igsColorCompVal = igsDontCompare;
	*igsBackMix = rop;
    }
    
    yPhase = (patternx & 0x3F) >> 3;

    *igsSrcStartPtr = (unsigned long)(patterny  * vga256InfoRec.displayWidth
				      + (patternx & 0x3F) + yPhase * 8) * 3;

    *igsDstYPhase = yPhase;

    *igsPixMapWidth =  8 * 3 - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth * 3 - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;
}

void igsSubsequentFill8x8Pattern(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSubsequentFill8x8Pattern\n");
#endif   
    igsSync();
    *igsOpDimension1 = w - 1;
    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth
				      + x);
    *igsSrcStartPtr = (unsigned long)(patterny * vga256InfoRec.displayWidth 
				      + (patternx & 0x3F)); /* XXX Check this */

    *igsDstXPhase = (y * vga256InfoRec.displayWidth + x);
    *igsOpDimension2 = h - 1;

    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16);     
}

void igs24SubsequentFill8x8Pattern(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{

#ifdef ACCEL_DEBUG
    ErrorF("igs24SubsequentFill8x8Pattern\n");
#endif   
    igsSync();
    *igsOpDimension1 = w - 1;
    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth
				      + x) * 3;

    *igsSrcBasePtr = (unsigned long)(patterny * vga256InfoRec.displayWidth 
				     + patternx) * 3;

    *igsDstXPhase = (y * vga256InfoRec.displayWidth + x);
    *igsOpDimension2 = h - 1;

    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16);     
}

/* 
 * 8x8 Pattern Colour Expansion
 */
void igsSetupFor8x8PatternColorExpand(patternx, patterny, bg, fg, rop,
planemask)
    int patternx, patterny, bg, fg, rop;
    unsigned int planemask;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSetupFor8x8PatternColorExpand\n");
#endif   
    CommandFlags = igsPxBlt | igsForeSrcColor | igsBackSrcColor | 
	igsLRDown;

    igsSync();
    *igsCopControl = 0;
    *igsForeMix = rop;
    *igsForeColor = fg;
    *igsPxBitMask = planemask;

    if ( bg == -1)
    {
	CommandFlags |= igsPattTilTra;
	*igsBackMix = GXnoop;
    }
    else
    {
	CommandFlags |= igsPattTilOpq;
	*igsBackColor = bg;
	*igsBackMix = rop;
    }

    *igsPixMapWidth = 8 - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = (unsigned long)(patterny * vga256InfoRec.displayWidth 
				      + patternx/xf86AccelInfoRec.BitsPerPixel); 
}

void igs24SetupFor8x8PatternColorExpand(patternx, patterny, bg, fg, rop,
planemask)
    int patternx, patterny, bg, fg, rop;
    unsigned int planemask;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24SetupFor8x8PatternColorExpand\n");
#endif   
    CommandFlags = igsPxBlt | igsForeSrcColor | igsBackSrcColor | 
	igsLRDown;

    igsSync();
    *igsCopControl = 0;
    *igsForeMix = rop;
    *igsForeColor = fg;
    *igsPxBitMask = planemask;

    if ( bg == -1)
    {
	CommandFlags |= igsPattTilTra;
	*igsBackMix = GXnoop;
    }
    else
    {
	CommandFlags |= igsPattTilOpq;
	*igsBackColor = bg;
	*igsBackMix = rop;
    }

    *igsPixMapWidth = 8 * 3 - 1;
    *igsDstMapWidth = vga256InfoRec.displayWidth * 3 - 1;
    *igsPixMapFormat = vgaBytesPerPixel - 1;

    *igsSrcStartPtr = (unsigned long)(patterny * vga256InfoRec.displayWidth 
				      + patternx/xf86AccelInfoRec.BitsPerPixel)
	* 3; 
}

void igsSubsequent8x8PatternColorExpand(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igsSubsequent8x8PatternColorExpand\n");
#endif   
    igsSync();
    *igsOpDimension1 = w - 1;
    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth
				      + x);
    *igsDstXPhase = (y * vga256InfoRec.displayWidth + x);
    *igsOpDimension2 = h - 1;

    *igsPatXPhase = 0;
    *igsPatYPhase = y & 0x7;    

    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16);     
}

void igs24Subsequent8x8PatternColorExpand(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
#ifdef ACCEL_DEBUG
    ErrorF("igs24Subsequent8x8PatternColorExpand\n");
#endif   
    igsSync();
    *igsOpDimension1 = w - 1;
    *igsDstStartPtr = (unsigned long)(y * vga256InfoRec.displayWidth
				      + x) * 3;
    *igsDstXPhase = (y * vga256InfoRec.displayWidth + x);
    *igsOpDimension2 = h - 1;

    *igsPatXPhase = 0;
    *igsPatYPhase = y & 0x7;    

    *igsPxOpL = (unsigned short)(CommandFlags & 0xFFFF);
    *igsPxOpH = (unsigned short)((CommandFlags & 0xFFFF0000) >> 16);     
}







