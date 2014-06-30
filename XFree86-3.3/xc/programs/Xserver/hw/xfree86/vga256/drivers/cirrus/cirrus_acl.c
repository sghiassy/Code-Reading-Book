/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cirrus_acl.c,v 3.6.2.7 1997/05/24 08:36:11 dawes Exp $ */

/*
 * New-style acceleration for chips with BitBLT engine:
 *
 *					DWORD
 *				Solid	Align	Left		Transp.	
 *			Auto	Fill /	Color	Edge	32bpp	Color   Pattern
 *		MMIO	Start	24bpp	Exp	Clip	Support	Compare Offset
 * CL-GD5426							Yes
 * CL-GD5428							Yes
 * CL-GD5429	Yes				Yes			Yes
 * CL-GD5430/40	Yes				Yes			Yes (3)
 * CL-GD5434	Yes					Yes (1)
 * CL-GD5436	Yes	Yes	Yes	Yes	Yes	Yes	?	Yes (3)
 * CL-GD5446	Yes	Yes	Yes	Yes	Yes	Yes (6)	Yes (2)	Yes
 * CL-GD5480	Yes	Yes	Yes	Yes	Yes	Yes	Yes (2)	Yes
 * CL-GD7543							Yes
 * CL-GD7548	Yes	Yes					Yes
 * CL-GD7555	Yes (4)	Yes	Yes	Yes	Yes	No	? (5)	?
 *
 * (1) 8x8 non-color expand pattern fill not supported at 32bpp.
 * (2) Not supported at 24/32bpp, and GXcopy only.
 * (3) Left-edge clipping is used for horizontal offset.
 * (4) MMIO space only at end of linear framebuffer.
 * (5) Mentioned in features in databook, but register description is missing.
 * (6) 5446 doesn't work at 32bpp with the current driver. Not sure whether
 *     it is really supported.
 *
 * In the case of the 7548, I wonder if it shares some of the other newer
 * BitBLT features such as clipping for color expansion. The databook doesn't
 * mention this.
 *
 * Also, it's unclear whether the CL-GD5436 has transparency color compare
 * (maybe it was a late feature).
 *
 * The CL-GD5446 and CL-GD5480 have "PCI-retry" support for MMIO with 
 * Auto-Start, which eliminates the need for BitBLT polling. It requires 
 * that a MMIO index below 0x10 is written first.
 *
 * I have made the assumption that it is OK on all chips with MMIO to write
 * a 32-bit word to the MMIO address 0x18:
 *
 * 0x18		BLT Mode
 * 0x19		Undefined
 * 0x1A		Raster OP
 * 0x1B         BLT Mode Extensions (GD5436/46 only)
 *
 * The driver is not yet ready for future increased XAA concurrency where
 * the SetUp functions might be called while a blit is still going on.
 * One issue is that the CL-GD5446 and CL-GD5480 (which don't need polling) 
 * require that a MMIO index below 0x10 is written first, which is not the 
 * case for the color-expansion SetUp functions.
 */

#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#include "compiler.h"

#include "xf86xaa.h"

#include "cir_driver.h"
#ifdef MMIO
#include "cirBlitMM.h"
#else
#include "cirBlitter.h"
#endif


#define DO_FILL_RECT   1
#define DO_COPY        1
#define DO_PATTERNS    0
#define DO_COLEXP      1

#define CHIPHAS(feature) (cirrusChipFeatures & feature)

#define BPPADJUST(x) \
    (vga256InfoRec.bitsPerPixel == 8 ? (x) : \
     vga256InfoRec.bitsPerPixel == 16 ? (x) * 2 : \
     vga256InfoRec.bitsPerPixel == 24 ? (x) * 3 : (x) * 4)

#define WAITEMPTY() { \
    if (!cirrusPCIRetrySupport) \
        CirrusBLTWaitEmpty(); }

/*
 * Must be careful not to confuse SOLIDCOLORFILL register bit flag
 * with SOLIDCOLORFILLMODE feature flag.
 */

enum {
    PACKED24FILL			= 0x1,
    AUTOSTART				= 0x2,
    PCIRETRYSUPPORT			= 0x4,
    SOLIDCOLORFILLFEATURE		= 0x8,
    TRANSPARENCYCOLORCOMPARE		= 0x10,
    ONLYGXCOPYTRANSPARENCYCOLORCOMPARE	= 0x20,
    TRANSPARENCYCOLORCOMPAREMASK	= 0x40,
    WRITEMASK				= 0x80,
    DWORDCOLOREXPANSIONSCANLINEPAD	= 0x100,
    CPUFRAMEBUFFERCONCURRENCY		= 0x200,
    FGCOLORREGISTERSIDEEFFECT		= 0x400,
    NEWSTYLEPATTERNOFFSET		= 0x800,
    OLDSTYLEPATTERNOFFSET		= 0x1000,
    BROKEN32BPPPATTERNFILL		= 0x2000,
    TRANSPARENCYNEEDSBGCOLOR		= 0x4000
};



#ifndef MMIO    

int cirrusUseAutoStart = FALSE;
int cirrusUseSolidColorFill = FALSE;
int cirrusPCIRetrySupport = FALSE;
int cirrusChipFeatures = 0;

#else

extern int cirrusUseAutoStart, cirrusUseSolidColorFill, cirrusPCIRetrySupport;
extern int cirrusChipFeatures;

#define MMIONAME(x) x##MMIO

#define CirrusAccelInit MMIONAME(CirrusAccelInit)
#define CirrusInitializeBitBLTEngine MMIONAME(CirrusInitializeBitBLTEngine)
#define CirrusSync MMIONAME(CirrusSync)
#define CirrusSetupForFillRectSolid MMIONAME(CirrusSetupForFillRectSolid)
#define Cirrus8SubsequentFillRectSolid MMIONAME(Cirrus8SubsequentFillRectSolid)
#define Cirrus16SubsequentFillRectSolid MMIONAME(Cirrus16SubsequentFillRectSolid)
#define Cirrus24SubsequentFillRectSolid MMIONAME(Cirrus24SubsequentFillRectSolid)
#define Cirrus32SubsequentFillRectSolid MMIONAME(Cirrus32SubsequentFillRectSolid)
#define CirrusSetupForScreenToScreenCopy MMIONAME(CirrusSetupForScreenToScreenCopy)
#define Cirrus8SubsequentScreenToScreenCopy MMIONAME(Cirrus8SubsequentScreenToScreenCopy)
#define Cirrus16SubsequentScreenToScreenCopy MMIONAME(Cirrus16SubsequentScreenToScreenCopy)
#define Cirrus24SubsequentScreenToScreenCopy MMIONAME(Cirrus24SubsequentScreenToScreenCopy)
#define Cirrus32SubsequentScreenToScreenCopy MMIONAME(Cirrus32SubsequentScreenToScreenCopy)
#define CirrusSetupForCPUToScreenColorExpand MMIONAME(CirrusSetupForCPUToScreenColorExpand)
#define CirrusSubsequentCPUToScreenColorExpand MMIONAME(CirrusSubsequentCPUToScreenColorExpand)
#define CirrusSetupForScreenToScreenColorExpand MMIONAME(CirrusSetupForScreenToScreenColorExpand)
#define CirrusSubsequentScreenToScreenColorExpand MMIONAME(CirrusSubsequentScreenToScreenColorExpand)
#define CirrusSetupForScanlineScreenToScreenColorExpand MMIONAME(CirrusSetupForScanlineScreenToScreenColorExpand)
#define CirrusSubsequentScanlineScreenToScreenColorExpand MMIONAME(CirrusSubsequentScanlineScreenToScreenColorExpand)
#define CirrusSetupFor8x8PatternFill MMIONAME(CirrusSetupFor8x8PatternFill)
#define CirrusSubsequent8x8PatternFill MMIONAME(CirrusSubsequent8x8PatternFill)
#define CirrusSetupFor8x8PatternColorExpand MMIONAME(CirrusSetupFor8x8PatternColorExpand)
#define CirrusSubsequent8x8PatternColorExpand MMIONAME(CirrusSubsequent8x8PatternColorExpand)

#endif

void CirrusSync();
void CirrusSetupForFillRectSolid();
void Cirrus8SubsequentFillRectSolid();
void Cirrus16SubsequentFillRectSolid();
void Cirrus24SubsequentFillRectSolid();
void Cirrus32SubsequentFillRectSolid();
void CirrusSetupForScreenToScreenCopy();
void Cirrus8SubsequentScreenToScreenCopy();
void Cirrus16SubsequentScreenToScreenCopy();
void Cirrus24SubsequentScreenToScreenCopy();
void Cirrus32SubsequentScreenToScreenCopy();
void CirrusSetupForCPUToScreenColorExpand();
void CirrusSubsequentCPUToScreenColorExpand();
void CirrusSetupForScreenToScreenColorExpand();
void CirrusSubsequentScreenToScreenColorExpand();
void CirrusSetupForScanlineScreenToScreenColorExpand();
void CirrusSubsequentScanlineScreenToScreenColorExpand();
void CirrusSetupFor8x8PatternFill();
void CirrusSubsequent8x8PatternFill();
void CirrusSetupFor8x8PatternColorExpand();
void CirrusSubsequent8x8PatternColorExpand();
  

/* Initialize the cirrusChipFeatures variable. */

static void InitializeChipFeatures() {
    cirrusChipFeatures = 0;

    if (cirrusChip == CLGD5436 || cirrusChip == CLGD5446 ||
	cirrusChip == CLGD5480 || cirrusChip == CLGD7555) {
        cirrusChipFeatures |= PACKED24FILL;
        cirrusChipFeatures |= DWORDCOLOREXPANSIONSCANLINEPAD;
#ifdef MMIO
        cirrusChipFeatures |= SOLIDCOLORFILLFEATURE;
#endif
    }

#ifdef MMIO
    if (cirrusChip == CLGD5436 || cirrusChip == CLGD5446 ||
	cirrusChip == CLGD5480 || cirrusChip == CLGD7548 || 
	cirrusChip == CLGD7555)
        cirrusChipFeatures |= AUTOSTART;
#endif

    if (cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
        /* Non conclusive data on 7555. */
        cirrusChipFeatures |= PCIRETRYSUPPORT;

    if (cirrusChip == CLGD5426 || cirrusChip == CLGD5428 ||
	cirrusChip == CLGD7543 || cirrusChip == CLGD7548 || 
	cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
	/* No conclusive data on 7555. */
        cirrusChipFeatures |= TRANSPARENCYCOLORCOMPARE;

    if (cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
        cirrusChipFeatures |= ONLYGXCOPYTRANSPARENCYCOLORCOMPARE;

    if (cirrusChip == CLGD5426 || cirrusChip == CLGD5428 ||
    cirrusChip == CLGD7543 || cirrusChip == CLGD7548)
        cirrusChipFeatures |= TRANSPARENCYCOLORCOMPAREMASK;

    if (cirrusChip == CLGD5426 || cirrusChip == CLGD5428 ||
    cirrusChip == CLGD7543 || cirrusChip == CLGD7548 || cirrusChip == CLGD5434)
        cirrusChipFeatures |= TRANSPARENCYNEEDSBGCOLOR;

    /* "Write Mask" is used for left-edge clipping. */
    if (cirrusChip == CLGD5430 || cirrusChip == CLGD5436 ||
	cirrusChip == CLGD5446 || cirrusChip == CLGD5480 ||
	cirrusChip == CLGD7555)
        cirrusChipFeatures |= WRITEMASK;

    if (cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
        cirrusChipFeatures |= CPUFRAMEBUFFERCONCURRENCY;

    if (cirrusChip <= CLGD5429 || HAVE75XX())
        /* At least the 5426/28 and 7543. */
        cirrusChipFeatures |= FGCOLORREGISTERSIDEEFFECT;

    if (cirrusChip == CLGD5430 || cirrusChip == CLGD5436)
    	/* No conclusive data on 7555. */
        cirrusChipFeatures |= OLDSTYLEPATTERNOFFSET;

    if (cirrusChip == CLGD5446 || cirrusChip == CLGD5480)
        cirrusChipFeatures |= NEWSTYLEPATTERNOFFSET;

    if (cirrusChip == CLGD5434)
        cirrusChipFeatures |= BROKEN32BPPPATTERNFILL;
}

void
CirrusAccelInit()
{

    /* Collect the chip features flags (nothing to do with XAA). */
    InitializeChipFeatures();

    /* Set up XAA flags. */
    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | PIXMAP_CACHE
        | ONLY_TWO_BITBLT_DIRECTIONS | HARDWARE_PATTERN_ALIGN_64;;
    if (CHIPHAS(CPUFRAMEBUFFERCONCURRENCY))
        xf86AccelInfoRec.Flags |= COP_FRAMEBUFFER_CONCURRENCY;	/* '46, '80 */
    xf86AccelInfoRec.Sync = CirrusSync;

#if DO_FILL_RECT
    xf86GCInfoRec.PolyFillRectSolidFlags |= NO_PLANEMASK;
    if (vga256InfoRec.bitsPerPixel == 24 && !CHIPHAS(PACKED24FILL))
          xf86GCInfoRec.PolyFillRectSolidFlags |= RGB_EQUAL;
  
    xf86AccelInfoRec.SetupForFillRectSolid = CirrusSetupForFillRectSolid;
    switch (vga256InfoRec.bitsPerPixel) {
      case 8 :
        xf86AccelInfoRec.SubsequentFillRectSolid = 
	  Cirrus8SubsequentFillRectSolid;
        break;
      case 16 :
        xf86AccelInfoRec.SubsequentFillRectSolid = 
	  Cirrus16SubsequentFillRectSolid;
        break;
      case 24 :
        xf86AccelInfoRec.SubsequentFillRectSolid = 
	  Cirrus24SubsequentFillRectSolid;
        break;
      case 32 :
        xf86AccelInfoRec.SubsequentFillRectSolid = 
	  Cirrus32SubsequentFillRectSolid;
        break;
      }
#endif /* DO_FILL_RECT */

#if DO_COPY
    xf86GCInfoRec.CopyAreaFlags |= NO_PLANEMASK;
    if (!CHIPHAS(TRANSPARENCYCOLORCOMPARE) || vga256InfoRec.bitsPerPixel > 16)
        /*
         * There are currently no chips supporting transparency color
         * compare at bpp > 16.
         */
        xf86GCInfoRec.CopyAreaFlags |= NO_TRANSPARENCY;
    if (CHIPHAS(ONLYGXCOPYTRANSPARENCYCOLORCOMPARE))
        xf86GCInfoRec.CopyAreaFlags |= TRANSPARENCY_GXCOPY;
    xf86AccelInfoRec.SetupForScreenToScreenCopy = CirrusSetupForScreenToScreenCopy;
    switch (vgaBitsPerPixel) {
    case 8 :
        xf86AccelInfoRec.SubsequentScreenToScreenCopy = Cirrus8SubsequentScreenToScreenCopy;
        break;
    case 16 :
        xf86AccelInfoRec.SubsequentScreenToScreenCopy = Cirrus16SubsequentScreenToScreenCopy;
        break;
    case 24 :
        xf86AccelInfoRec.SubsequentScreenToScreenCopy = Cirrus24SubsequentScreenToScreenCopy;
        break;
    case 32 :
        xf86AccelInfoRec.SubsequentScreenToScreenCopy = Cirrus32SubsequentScreenToScreenCopy;
        break;
    }
#endif /* DO_COPY */

#if DO_PATTERNS
    /* Pattern fills. */
    if (CHIPHAS(NEWSTYLEPATTERNOFFSET) || CHIPHAS(OLDSTYLEPATTERNOFFSET))
        xf86AccelInfoRec.Flags |= HARDWARE_PATTERN_PROGRAMMED_ORIGIN;
    if (vga256InfoRec.bitsPerPixel != 24 || CHIPHAS(PACKED24FILL)) {
#if 0
        if (vga256InfoRec.bitsPerPixel == 24)
            /* 8 "filler byte" at the end of each pattern scanline. */
            xf86AccelInfoRec.Flags |= HARDWARE_PATTERN_PAD_24BPP;
            /*
             * However, don't use the regular 8x8 pattern fill at 24bpp
             * at the moment because the 5446 needs special pattern offset
             * handling (offset in terms of 32bpp pixels) that hasn't
             * been implemented yet.
             */
#endif
        if (vga256InfoRec.bitsPerPixel != 24 &&
        (vga256InfoRec.bitsPerPixel != 32 ||
        !CHIPHAS(BROKEN32BPPPATTERNFILL))) {
            xf86AccelInfoRec.SetupForFill8x8Pattern =
                CirrusSetupFor8x8PatternFill;
            xf86AccelInfoRec.SubsequentFill8x8Pattern =
                CirrusSubsequent8x8PatternFill;

	    xf86AccelInfoRec.SetupFor8x8PatternColorExpand =
	      CirrusSetupFor8x8PatternColorExpand;
	    xf86AccelInfoRec.Subsequent8x8PatternColorExpand =
	      CirrusSubsequent8x8PatternColorExpand;
        }

    }
#endif /* DO_PATTERNS */

#if DO_COLEXP
    /* Color expansion. */
    if (vga256InfoRec.bitsPerPixel != 24 || CHIPHAS(PACKED24FILL)) {

        xf86AccelInfoRec.ColorExpandFlags =
            CPU_TRANSFER_PAD_DWORD |
            BIT_ORDER_IN_BYTE_MSBFIRST | VIDEO_SOURCE_GRANULARITY_DWORD;

        if (CHIPHAS(DWORDCOLOREXPANSIONSCANLINEPAD))
            xf86AccelInfoRec.ColorExpandFlags |= SCANLINE_PAD_DWORD;
        else
            xf86AccelInfoRec.ColorExpandFlags |= SCANLINE_PAD_BYTE;

        if (vga256InfoRec.bitsPerPixel == 24)
            /*
             * The 5436 and 5446, which support 24bpp color expansion,
             * only support it with transparency.
             */
            xf86AccelInfoRec.ColorExpandFlags |= ONLY_TRANSPARENCY_SUPPORTED;

        /*
         * The following change is now in place (post 3.2A):
         * No longer use the "legacy" color expansion text function
         * for chips that require BYTE padding. XAA supports BYTE
         * padding (not optimally though).
         *
         * The "indirect" color expansion was pretty useless anyway, because
         * of the DWORD source alignment requirement.
         */

	if (CHIPHAS(DWORDCOLOREXPANSIONSCANLINEPAD)) {
	    /*
	     * A modern chip with 32-bit scanline alignment is compatible
	     * with optimized XAA CPU-to-screen color expansion.
	     */
            if (CHIPHAS(WRITEMASK))
                xf86AccelInfoRec.ColorExpandFlags |=
                    LEFT_EDGE_CLIPPING |
                    LEFT_EDGE_CLIPPING_NEGATIVE_X;
            xf86AccelInfoRec.SetupForCPUToScreenColorExpand =
                CirrusSetupForCPUToScreenColorExpand;
            xf86AccelInfoRec.SubsequentCPUToScreenColorExpand =
                CirrusSubsequentCPUToScreenColorExpand;
        }
        else {
            /*
             * XAA CPU-to-screen color expansion with BYTE padding doesn't
             * work yet, so we have to use the indirect method (which
             * isn't particularly fast because there's no coprocessor/
             * CPU framebuffer concurrency).
             *
             * However, for "TE" text, we use the "legacy" cirrus
             * driver text color expansion that uses fast ASM routines
             * and correctly handles the BYTE padding.
             */
#ifdef MMIO
            xf86GCInfoRec.ImageGlyphBltTE = CirrusMMIOImageGlyphBlt;
	    if (vga256InfoRec.bitsPerPixel == 8)
                xf86GCInfoRec.PolyGlyphBltTE = CirrusMMIOPolyGlyphBlt;
#else
	    xf86GCInfoRec.ImageGlyphBltTE = CirrusImageGlyphBlt;
	    /* I think that the PolyGlyphBltTE function works only
	       in 8bpp modes.  --Corey 1/17/97 */
	    if (vga256InfoRec.bitsPerPixel == 8) {
	      xf86GCInfoRec.PolyGlyphBltTE = CirrusPolyGlyphBlt;
	    }
#endif
            xf86GCInfoRec.ImageGlyphBltTEFlags = NO_PLANEMASK;
            xf86GCInfoRec.PolyGlyphBltTEFlags = NO_PLANEMASK;
#if 0
            /*
             * Set up indirect color expansion primitives. These will
             * only be used when linear addressing is enabled.
             */
            xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand =
                CirrusSetupForScanlineScreenToScreenColorExpand;
            xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand =
                CirrusSubsequentScanlineScreenToScreenColorExpand;

	    xf86AccelInfoRec.ScratchBufferAddr = cirrusBufferSpaceAddr;
	    xf86AccelInfoRec.ScratchBufferSize = cirrusBufferSpaceSize;
#endif
        }

        xf86AccelInfoRec.SetupForScreenToScreenColorExpand =
            CirrusSetupForScreenToScreenColorExpand;
        xf86AccelInfoRec.SubsequentScreenToScreenColorExpand =
            CirrusSubsequentScreenToScreenColorExpand;
    }
    /* end of colexp functions */
#endif /* DO_COLEXP */


    if (CHIPHAS(AUTOSTART)) {
        cirrusUseAutoStart = TRUE;
        if (CHIPHAS(PCIRETRYSUPPORT))
            cirrusPCIRetrySupport = TRUE;
    }
    if (CHIPHAS(SOLIDCOLORFILLFEATURE))
        cirrusUseSolidColorFill = TRUE;

    /* Initialize the PixMap cache.  We're free to tell XAA the region
       of video memory (linearly) that's okay to use for PixMap caches.
       */
    xf86InitPixmapCache(&vga256InfoRec, vga256InfoRec.virtualY *
        vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8,
        vga256InfoRec.videoRam * 1024 - 1024
        /* - cirrusBufferSpaceSize */
        );
}

void CirrusInitializeBitBLTEngine() {
   int bytewidth;
   bytewidth = vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8;
   SETSRCPITCH(bytewidth);
   SETDESTPITCH(bytewidth);
   if (CHIPHAS(TRANSPARENCYCOLORCOMPAREMASK))
       SETTRANSPARENCYCOLORMASK16(0);
   if (CHIPHAS(DWORDCOLOREXPANSIONSCANLINEPAD))
       SETBLTMODEEXT(SOURCEDWORDGRANULARITY);
   if (cirrusUseAutoStart)
       SETBLTSTATUS(0x80);
}

void CirrusSync() {
    WAITUNTILFINISHED();
#ifndef MMIO
    if (CHIPHAS(FGCOLORREGISTERSIDEEFFECT))
        SETFOREGROUNDCOLOR(0);
#endif
#if 0
    /*
     * This is not necessary, as long as in every operation the
     * BLTMODEEXT is also written by using a SETBLTMODEANDROP command.
     */
    if (cirrusUseSolidColorFill)
        SETBLTMODEEXT(0);
#endif
}



void CirrusSetupForFillRectSolid(color, rop, planemask)
    int color, rop, planemask;
{
    int bltmode;

    if (vga256InfoRec.bitsPerPixel >= 24 &&
	(vga256InfoRec.bitsPerPixel != 24 || CHIPHAS(PACKED24FILL))) {
      if (!cirrusUseSolidColorFill) {
	SETBACKGROUNDCOLOR32(color);
      }
      SETFOREGROUNDCOLOR32(color);
    }
    else {
      if (!cirrusUseSolidColorFill) {
	SETBACKGROUNDCOLOR16(color);
      }
      SETFOREGROUNDCOLOR16(color);
    }


    
    SETSRCADDR(0);

    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        bltmode = COLOREXPAND | PATTERNCOPY;
        break;
    case 16 :
        bltmode = COLOREXPAND | PATTERNCOPY | PIXELWIDTH16;
        break;
    case 24 :
        if (CHIPHAS(PACKED24FILL)) {
            bltmode = COLOREXPAND | PATTERNCOPY | PIXELWIDTH24;
        }
        else {
            bltmode = COLOREXPAND | PATTERNCOPY;
        }
        break;
    default : /* 32 */
        bltmode = COLOREXPAND | PATTERNCOPY | PIXELWIDTH32;
        break;
    }

    if (cirrusUseSolidColorFill) {
        SETBLTMODEANDROP(bltmode, SOLIDCOLORFILL, cirrus_rop[rop]);
    }
    else {
        SETBLTMODEANDROP(bltmode, 0, cirrus_rop[rop]);
    }
}



void Cirrus8SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;
    destaddr = y * vga256InfoRec.displayWidth + x;
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w, h);			/* 8 */

#if 0
    if (!cirrusUseSolidColorFill)
        SETSRCADDR(0);	                        /* 20 */
#endif

    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}

void Cirrus16SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x) * 2;
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w * 2, h);		/* 10 */

#if 0
    if (!cirrusUseSolidColorFill)
        SETSRCADDR(0);	                        /* 20 */
#endif

    SETDESTADDR(destaddr);			/* 10 */
    if (!cirrusUseAutoStart)
        STARTBLT();
}

void Cirrus24SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x) * 3;
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w * 3, h);		/* 8 */

#if 0
    if (!cirrusUseSolidColorFill)
        SETSRCADDR(0);	                        /* 20 */
#endif

    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}

void Cirrus32SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x) * 4;
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w * 4, h);		/* 8 */

#if 0
    if (!cirrusUseSolidColorFill)
        SETSRCADDR(0);	                        /* 20 */
#endif

    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}

/*
 * Screen-to-screen BitBLT.
 *
 * Any raster-op is supported.
 * Planemask is supported in all modes except 24bpp.
 */

static int blitxdir, blitydir;
 
void CirrusSetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
    int bltmode;
    bltmode = 0;

    /* Set up optional transparency compare. */
    if (transparency_color != -1) {
        /*
         * As far as I'm aware, the only chips that support transparency
         * color compare only support it with 8/16bpp.
         */
        if (vga256InfoRec.bitsPerPixel == 8) {
            transparency_color &= 0xFF;
            transparency_color |= transparency_color << 8;
        }
        SETTRANSPARENCYCOLOR16(transparency_color);
        bltmode |= TRANSPARENCYCOMPARE;
    }
    /* Set up the blit direction. */
    blitxdir = xdir;
    blitydir = ydir;
    if (ydir < 0)
	bltmode |= BACKWARDS;
    SETBLTMODEANDROP(bltmode, 0, cirrus_rop[rop]);
}

/* This a bit ugly but I don't feel like duplicating the code this time. */

#define MAPSubsequentScreenToScreenCopy(bpp) \
void Cirrus##bpp##SubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h) \
    int x1, y1, x2, y2, w, h; \
{ \
    int srcaddr, destaddr; \
    /* \
     * We have set the flag indicating that xdir and ydir must be equal, \
     * so we can assume that here. \
     */ \
    if (blitydir == -1) { \
        srcaddr = (y1 + h - 1) * vga256InfoRec.displayWidth * (bpp / 8); \
	destaddr = (y2 + h - 1) * vga256InfoRec.displayWidth * (bpp / 8); \
    } else { \
        srcaddr = y1 * vga256InfoRec.displayWidth * (bpp / 8); \
        destaddr = y2 * vga256InfoRec.displayWidth * (bpp / 8); \
    } \
    if (blitxdir == -1) { \
	srcaddr += (x1 + w) * (bpp / 8) - 1; \
	destaddr += (x2 + w) * (bpp / 8) - 1; \
    } else { \
	srcaddr += x1 * (bpp / 8); \
	destaddr += x2 * (bpp / 8); \
    } \
    if (cirrusUseAutoStart) \
        WAITEMPTY() \
    else \
        WAITUNTILFINISHED(); \
    w *= (bpp / 8); \
    SETWIDTHANDHEIGHT(w, h); \
    SETSRCADDR(srcaddr); \
    SETDESTADDR(destaddr); \
    if (!cirrusUseAutoStart) \
        STARTBLT(); \
}

MAPSubsequentScreenToScreenCopy(8)
MAPSubsequentScreenToScreenCopy(16)
MAPSubsequentScreenToScreenCopy(24)
MAPSubsequentScreenToScreenCopy(32)

/*
 * CPU-to-screen color expansion.
 */
 
void CirrusSetupForCPUToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
    int bltmode, loadbg;
    bltmode = 0;

    if (bg == -1)
        bltmode = TRANSPARENCYCOMPARE;
    /*
     * There are three different types of chip regarding color expansion
     * with transparency:
     *
     * - The CL-GD5426/28/7543/7548, which have a transparency compare
     *   color. The background color and the 16-bit transparency color
     *   (doubled at 8bpp) must both be loaded with the same unique value.
     * - The CL-GD5434. It requires the 32-bit background color
     *   to be set to the logical inverse of the foreground color, with both
     *   loaded with 32 bits worth of pixels for 8bpp and 16bpp.
     * - The CL-GD5429/30/40/36/46, which do not require any additional
     *   colors to be loaded.
     */
    loadbg = (bg != -1);
    if (bg == -1 && CHIPHAS(TRANSPARENCYNEEDSBGCOLOR)) {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8 :
            bg = (~fg) & 0xFF;
            bg |= (bg << 8) | (bg << 16) | (bg << 24);
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 16 :
            bg = (~fg) & 0xFFFF;
            bg |= bg << 16;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
	case 24 :
            bg = (~fg) & 0xFFFFFF;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
	    break;
        case 32 :
            bg = ~fg;
            break;
        }
        if (CHIPHAS(TRANSPARENCYCOLORCOMPARE)) {
            SETTRANSPARENCYCOLOR16(bg);
        }
        loadbg = TRUE;
    }
    if (cirrusChip == CLGD5434 || vga256InfoRec.bitsPerPixel == 32 ||
    (vga256InfoRec.bitsPerPixel == 24 || CHIPHAS(PACKED24FILL))) {
        if (loadbg) {
            SETBACKGROUNDCOLOR32(bg);
        }
        SETFOREGROUNDCOLOR32(fg);
    }
    else {
        if (loadbg) {
            SETBACKGROUNDCOLOR16(bg);
        }
        SETFOREGROUNDCOLOR16(fg);
    }
    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        bltmode |= COLOREXPAND | SYSTEMSRC;
        break;
    case 16 :
        bltmode |= COLOREXPAND | SYSTEMSRC | PIXELWIDTH16;
        break;
    case 24 :
        if (CHIPHAS(PACKED24FILL)) {
            bltmode |= COLOREXPAND | SYSTEMSRC | PIXELWIDTH24;
        }
        else {
            bltmode |= COLOREXPAND | SYSTEMSRC;
        }
        break;
    default : /* 32 */
        bltmode |= COLOREXPAND | SYSTEMSRC | PIXELWIDTH32;
        break;
    }
    if (CHIPHAS(DWORDCOLOREXPANSIONSCANLINEPAD)) {
        SETBLTMODEANDROP(bltmode, SOURCEDWORDGRANULARITY, cirrus_rop[rop]);
    }
    else {
        SETBLTMODEANDROP(bltmode, 0, cirrus_rop[rop]);
    }
}

void CirrusSubsequentCPUToScreenColorExpand(x, y, w, h, skipleft)
    int x, y, w, h, skipleft;
{
    int destaddr;
    /*
     * I assume the higher level code has called Sync(), which is
     * a reasonable assumption since it transferred the CPU data.
     */
    w = BPPADJUST(w);
    SETWIDTHANDHEIGHT(w, h);
    if (CHIPHAS(WRITEMASK)) {
        /*
         * Apart from setting the write mask, this also satisfies the
         * requirement of writing to GR2C for the 5430/40.
         */
      /* Packed 24bpp mode is special.  The write mask is a _byte_ mask,
	 and can be up to 5 bits. */
      if (vga256InfoRec.bitsPerPixel == 24) {
        SETSRCADDR((skipleft*3) << 24);
      } else {
        SETSRCADDR(skipleft << 24);
      }
    }
    destaddr = y * vga256InfoRec.displayWidth + x;
    destaddr = BPPADJUST(destaddr);
    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}

/* Screen-to-screen color expansion. */

void CirrusSetupForScreenToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
    int bltmode, loadbg;

    bltmode = 0;
    if (bg == -1)
        bltmode = TRANSPARENCYCOMPARE;
    loadbg = (bg != -1);
    if (bg == -1 && CHIPHAS(TRANSPARENCYNEEDSBGCOLOR)) {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8 :
            bg = (~fg) & 0xFF;
            bg |= (bg << 8) | (bg << 16) | (bg << 24);
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 16 :
            bg = (~fg) & 0xFFFF;
            bg |= bg << 16;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 24 :
            bg = (~fg) & 0xFFFFFF;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 32 :
            bg = ~fg;
            break;
        }
        if (CHIPHAS(TRANSPARENCYCOMPARE)) {
            SETTRANSPARENCYCOLOR16(bg);
        }
        loadbg = TRUE;
    }

    if (cirrusChip == CLGD5434 || vga256InfoRec.bitsPerPixel == 32 ||
    (vga256InfoRec.bitsPerPixel == 24 || CHIPHAS(PACKED24FILL))) {
        if (loadbg) {
            SETBACKGROUNDCOLOR32(bg);
        }
        SETFOREGROUNDCOLOR32(fg);
    }
    else {
        if (loadbg) {
            SETBACKGROUNDCOLOR16(bg);
        }
        SETFOREGROUNDCOLOR16(fg);
    }

    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        bltmode |= COLOREXPAND;
        break;
    case 16 :
        bltmode |= COLOREXPAND | PIXELWIDTH16;
        break;
    case 24 :
        if (CHIPHAS(PACKED24FILL)) {
            bltmode |= COLOREXPAND | PIXELWIDTH24;
        }
        else {
            bltmode |= COLOREXPAND;
        }
        break;
    default : /* 32 */
        bltmode |= COLOREXPAND | PIXELWIDTH32;
        break;
    }
    SETBLTMODEANDROP(bltmode, 0, cirrus_rop[rop]);
}

void CirrusSubsequentScreenToScreenColorExpand(srcx, srcy, x, y, w, h)
    int srcx, srcy, x, y, w, h;
{
    int srcaddr, destaddr;
    w = BPPADJUST(w);
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w, h);
    srcaddr = srcy * vga256InfoRec.displayWidth;
    srcaddr = BPPADJUST(srcaddr);
    /*
     * srcx is in monochrome pixel units; actually we only support
     * units of 32 (DWORD alignmnent).
     */
    srcaddr += srcx / 8;
    SETSRCADDR(srcaddr);
    destaddr = y * vga256InfoRec.displayWidth + x;
    destaddr = BPPADJUST(destaddr);
    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}

/* Scanline-by-scanline screen-to-screen color expansion. */

/* These are currently unused. */

#if 0

static int scanline_destaddr;
static int scanline_width;

void CirrusSetupForScanlineScreenToScreenColorExpand(x, y, w, h, bg, fg, 
rop, planemask)
    int x, y, w, h;
    int bg, fg, rop;
    unsigned int planemask;
{
    int bltmode, loadbg;
    bltmode = 0;
    if (bg == -1)
        bltmode = TRANSPARENCYCOMPARE;
    loadbg = (bg != -1);
    if (bg == -1 && CHIPHAS(TRANSPARENCYNEEDSBGCOLOR)) {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8 :
            bg = (~fg) & 0xFF;
            bg |= (bg << 8) | (bg << 16) | (bg << 24);
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 16 :
            bg = (~fg) & 0xFFFF;
            bg |= bg << 16;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 24 :
            bg = (~fg) & 0xFFFFFF;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 32 :
            bg = ~fg;
            break;
        }
        if (CHIPHAS(TRANSPARENCYCOMPARE)) {
            SETTRANSPARENCYCOLOR16(bg);
        }
        loadbg = TRUE;
    }

    if (cirrusChip == CLGD5434 || vga256InfoRec.bitsPerPixel == 32 ||
    (vga256InfoRec.bitsPerPixel == 24 || CHIPHAS(PACKED24FILL))) {
        if (loadbg) {
            SETBACKGROUNDCOLOR32(bg);
        }
        SETFOREGROUNDCOLOR32(fg);
    }
    else {
        if (loadbg) {
            SETBACKGROUNDCOLOR16(bg);
        }
        SETFOREGROUNDCOLOR16(fg);
    }

    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        bltmode |= COLOREXPAND;
        break;
    case 16 :
        bltmode |= COLOREXPAND | PIXELWIDTH16;
        break;
    case 24 :
        if (CHIPHAS(PACKED24FILL)) {
            bltmode |= COLOREXPAND | PIXELWIDTH24;
        }
        else {
            bltmode |= COLOREXPAND;
        }
        break;
    default : /* 32 */
        bltmode |= COLOREXPAND | PIXELWIDTH32;
        break;
    }

    SETBLTMODEANDROP(bltmode, 0, cirrus_rop[rop]);
    scanline_width = BPPADJUST(w);
    scanline_destaddr = y * vga256InfoRec.displayWidth + x;
    scanline_destaddr = BPPADJUST(scanline_destaddr);
}

void CirrusSubsequentScanlineScreenToScreenColorExpand(srcaddr)
    int srcaddr;
{
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    srcaddr /= 8;
    SETWIDTHANDHEIGHT(scanline_width, 1);
    SETSRCADDR(srcaddr);
    SETDESTADDR(scanline_destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
    scanline_destaddr +=
        vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8;
}

#endif 	/* not used */

static int pattern_address;

void CirrusSetupFor8x8PatternFill(patternx, patterny, rop, planemask,
transparency_color)
    int patternx, patterny, rop, planemask, transparency_color;
{
    int bltmode;

    bltmode = 0;
    /* Set up optional transparency compare. */
    if (transparency_color != -1) {
        /*
         * As far as I'm aware, the only chips that support transparency
         * color compare only support it with 8/16bpp.
         */
        if (vga256InfoRec.bitsPerPixel == 8) {
            transparency_color &= 0xFF;
            transparency_color |= transparency_color << 8;
        }
        SETTRANSPARENCYCOLOR16(transparency_color);
        bltmode |= TRANSPARENCYCOMPARE;
    }
    SETBLTMODEANDROP(PATTERNCOPY | bltmode, 0, cirrus_rop[rop]);
    if (CHIPHAS(NEWSTYLEPATTERNOFFSET) || CHIPHAS(OLDSTYLEPATTERNOFFSET)) {
        pattern_address = patterny * vga256InfoRec.displayWidth + patternx;
    }
}

void CirrusSubsequent8x8PatternFill(patternx, patterny, x, y, w, h)
    int patternx, patterny, x, y, w, h;
{
    int srcaddr, destaddr;
    if (CHIPHAS(NEWSTYLEPATTERNOFFSET) || CHIPHAS(OLDSTYLEPATTERNOFFSET))
        if (CHIPHAS(NEWSTYLEPATTERNOFFSET))
            /* 5446, full pattern offset. */
            srcaddr = pattern_address + patterny * 8 + patternx;
        else {
            /* 5430/40, 5436 */
            srcaddr = pattern_address;
            /* Adjust destination area for left-edge clipping. */
            x -= patternx;
            w += patternx;
        }
    else
        srcaddr = patterny * vga256InfoRec.displayWidth + patternx;
    destaddr = y * vga256InfoRec.displayWidth + x;
    srcaddr = BPPADJUST(srcaddr);
    destaddr = BPPADJUST(destaddr);
    if (CHIPHAS(OLDSTYLEPATTERNOFFSET)) {
        /*
         * 5430/40, 5436 have vertical preset, and horizontal offset
         * can be programmed using left edge clipping.
         */
        srcaddr += patterny;
        if (vga256InfoRec.bitsPerPixel == 24)
            /* 24bpp is a special case, GR2F is in terms of bytes. */
            srcaddr |= (patternx * 3) << 24;
        else
            srcaddr |= patternx << 24;
    }
    w = BPPADJUST(w);
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w, h);
    SETSRCADDR(srcaddr);
    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}

void CirrusSetupFor8x8PatternColorExpand(patternx, patterny, bg, fg, rop,
planemask)
    int patternx, patterny, bg, fg, rop, planemask;
{
    int bltmode, loadbg;

    /* Ignore patternx and patterny. */
    bltmode = 0;
    if (bg == -1)
        bltmode = TRANSPARENCYCOMPARE;
    loadbg = (bg != -1);
    if (bg == -1 && CHIPHAS(TRANSPARENCYNEEDSBGCOLOR)) {
        switch (vga256InfoRec.bitsPerPixel) {
        case 8 :
            bg = (~fg) & 0xFF;
            bg |= (bg << 8) | (bg << 16) | (bg << 24);
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 16 :
            bg = (~fg) & 0xFFFF;
            bg |= bg << 16;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 24 :
            bg = (~fg) & 0xFFFFFF;
            if (cirrusChip == CLGD5434) {
                fg = ~bg;
            }
            break;
        case 32 :
            bg = ~fg;
            break;
        }
        if (CHIPHAS(TRANSPARENCYCOMPARE)) {
            SETTRANSPARENCYCOLOR16(bg);
        }
        loadbg = TRUE;
    }
    if (cirrusChip == CLGD5434 || vga256InfoRec.bitsPerPixel == 32 ||
    (vga256InfoRec.bitsPerPixel == 24 || CHIPHAS(PACKED24FILL))) {
        if (loadbg) {
            SETBACKGROUNDCOLOR32(bg);
        }
        SETFOREGROUNDCOLOR32(fg);
    }
    else {
        if (loadbg) {
            SETBACKGROUNDCOLOR16(bg);
        }
        SETFOREGROUNDCOLOR16(fg);
    }
    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        bltmode |= COLOREXPAND | PATTERNCOPY;
        break;
    case 16 :
        bltmode |= COLOREXPAND | PIXELWIDTH16 | PATTERNCOPY;
        break;
    case 24 :
        if (CHIPHAS(PACKED24FILL)) {
            bltmode |= COLOREXPAND | PIXELWIDTH24 | PATTERNCOPY;
        }
        else {
            bltmode |= COLOREXPAND | PATTERNCOPY;
        }
        break;
    default : /* 32 */
        bltmode |= COLOREXPAND | PIXELWIDTH32 | PATTERNCOPY;
        break;
    }
    SETBLTMODEANDROP(bltmode, 0, cirrus_rop[rop]);
    if (CHIPHAS(NEWSTYLEPATTERNOFFSET) || CHIPHAS(OLDSTYLEPATTERNOFFSET)) {
        pattern_address = BPPADJUST(patterny * vga256InfoRec.displayWidth)
            + patternx / 8;
    }
}

void CirrusSubsequent8x8PatternColorExpand(patternx, patterny, x, y, w, h)
    int patternx, patterny, x, y, w, h;
{
    int srcaddr, destaddr;
    if (CHIPHAS(NEWSTYLEPATTERNOFFSET) || CHIPHAS(OLDSTYLEPATTERNOFFSET)) {
        /*
         * Origin offset is passed in (patternx, patterny). Pattern storage
         * address was passed in Setup function and stored in
         * pattern_address.
         * The vertical offset is defined by bits 0-2 of the source address.
         * For the horizontal offset, left-edge clipping value is used,
         * which resides at the highest byte of the source address
         * DWORD.
         */
        srcaddr = pattern_address + patterny;
        if (vga256InfoRec.bitsPerPixel == 24)
            /* 24bpp is a special case, GR2F is in terms of bytes. */
            srcaddr |= (patternx * 3) << 24;
        else
            srcaddr |= patternx << 24;
        /* Adjust destination area for left-edge clipping. */
        x -= patternx;
        w += patternx;
    }
    else {
        srcaddr = patterny * vga256InfoRec.displayWidth;
        srcaddr = BPPADJUST(srcaddr);
        /* The pattern x-coordinate is in "bit" units. */
        srcaddr += patternx / 8;
    }
    destaddr = y * vga256InfoRec.displayWidth + x;
    destaddr = BPPADJUST(destaddr);
    w = BPPADJUST(w);
    if (cirrusUseAutoStart)
        WAITEMPTY()
    else
        WAITUNTILFINISHED();
    SETWIDTHANDHEIGHT(w, h);
    SETSRCADDR(srcaddr);
    SETDESTADDR(destaddr);
    if (!cirrusUseAutoStart)
        STARTBLT();
}
