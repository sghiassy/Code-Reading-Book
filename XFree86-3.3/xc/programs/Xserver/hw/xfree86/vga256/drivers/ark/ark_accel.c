/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/ark/ark_accel.c,v 3.2 1997/01/20 12:37:28 dawes Exp $ */

#include "vga256.h"
#include "compiler.h"
#include "xf86.h"
#include "vga.h"

#include "xf86xaa.h"

#include "ark_driver.h"
#include "ark_cop.h"

void ArkSync();
void Ark8SetupForFillRectSolid();
void Ark16SetupForFillRectSolid();
void Ark24SetupForFillRectSolid();
void Ark32SetupForFillRectSolid();
void ArkSubsequentFillRectSolid();
void Ark24SubsequentFillRectSolid();
void ArkSetupForScreenToScreenCopy();
void ArkSubsequentScreenToScreenCopy();
void ArkSetupForScanlineScreenToScreenColorExpand();
void ArkSubsequentScanlineScreenToScreenColorExpand();
void Ark24SetupForScanlineScreenToScreenColorExpand();
void Ark24SubsequentScanlineScreenToScreenColorExpand();
void ArkSetupForScreenToScreenColorExpand();
void ArkSubsequentScreenToScreenColorExpand();
void ArkSubsequentBresenhamLine();
void ArkSetupForFill8x8Pattern();
void ArkSubsequentFill8x8Pattern();
void ArkEmulateSubsequentFill8x8Pattern();
void ArkSetupFor8x8PatternColorExpand();
void ArkSubsequent8x8PatternColorExpand();

static int CommandFlags;


void ArkAccelInit() {
    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | PIXMAP_CACHE |
        COP_FRAMEBUFFER_CONCURRENCY | HARDWARE_PATTERN_TRANSPARENCY |
        HARDWARE_PATTERN_MOD_64_OFFSET;
    xf86AccelInfoRec.Sync = ArkSync;
        xf86GCInfoRec.PolyFillRectSolidFlags = 0;
        xf86AccelInfoRec.SetupForScreenToScreenCopy =
            ArkSetupForScreenToScreenCopy;
        xf86AccelInfoRec.SubsequentScreenToScreenCopy =
            ArkSubsequentScreenToScreenCopy;
        xf86GCInfoRec.CopyAreaFlags = 0;
        if (vga256InfoRec.bitsPerPixel == 24) {
            xf86GCInfoRec.CopyAreaFlags = NO_PLANEMASK | NO_TRANSPARENCY;
        }
    switch (vga256InfoRec.bitsPerPixel) {
    case 8 :
        xf86AccelInfoRec.SetupForFillRectSolid = Ark8SetupForFillRectSolid;
        xf86AccelInfoRec.SubsequentFillRectSolid = ArkSubsequentFillRectSolid;
        break;
    case 16 :
        xf86AccelInfoRec.SetupForFillRectSolid = Ark16SetupForFillRectSolid;
        xf86AccelInfoRec.SubsequentFillRectSolid = ArkSubsequentFillRectSolid;
        break;
    case 32 :
        xf86AccelInfoRec.SetupForFillRectSolid = Ark32SetupForFillRectSolid;
        xf86AccelInfoRec.SubsequentFillRectSolid = ArkSubsequentFillRectSolid;
        break;
    case 24 :
        xf86GCInfoRec.PolyFillRectSolidFlags = RGB_EQUAL | NO_PLANEMASK;
        xf86AccelInfoRec.SetupForFillRectSolid = Ark24SetupForFillRectSolid;
        xf86AccelInfoRec.SubsequentFillRectSolid = Ark24SubsequentFillRectSolid;
        break;
    }
    xf86AccelInfoRec.ColorExpandFlags =
        VIDEO_SOURCE_GRANULARITY_PIXEL | BIT_ORDER_IN_BYTE_LSBFIRST;
    if (vga256InfoRec.bitsPerPixel == 24) {
        /*
         * There is no 24bpp acceleration, but we can take advantage of
         * the "24bpp in 8bpp mode" color expansion support in XAA.
         *
         * However, this is not recommended for all purposes on a video
         * memory bandwidth-starved configuration, since due to the
         * indirect buffering, the video memory bandwidth used is
         * greater than plain cfb24 framebuffer code, and this shows
         * up with text.
         *
         * For stipples and bitmaps, things are different. So we
         * disable the use color expansion for text.
         */
        xf86AccelInfoRec.ColorExpandFlags |=
            TRIPLE_BITS_24BPP | RGB_EQUAL | NO_PLANEMASK;
        xf86AccelInfoRec.Flags |= NO_TEXT_COLOR_EXPANSION;
        xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand =
            Ark24SetupForScanlineScreenToScreenColorExpand;
        xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand =
            Ark24SubsequentScanlineScreenToScreenColorExpand;
    }
    else {
        xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand =
            ArkSetupForScanlineScreenToScreenColorExpand;
        xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand =
            ArkSubsequentScanlineScreenToScreenColorExpand;
    }
    xf86AccelInfoRec.ScratchBufferAddr = arkCOPBufferSpaceAddr;
    xf86AccelInfoRec.ScratchBufferSize = arkCOPBufferSpaceSize;
    if (vga256InfoRec.bitsPerPixel != 24) {
        xf86AccelInfoRec.SetupForScreenToScreenColorExpand =
            ArkSetupForScreenToScreenColorExpand;
        xf86AccelInfoRec.SubsequentScreenToScreenColorExpand =
            ArkSubsequentScreenToScreenColorExpand;
        xf86AccelInfoRec.SubsequentBresenhamLine =
            ArkSubsequentBresenhamLine;
        xf86GCInfoRec.PolyLineSolidZeroWidthFlags = 0;
        xf86GCInfoRec.PolySegmentSolidZeroWidthFlags = 0;
        xf86AccelInfoRec.ErrorTermBits = 14;
        xf86AccelInfoRec.SetupForFill8x8Pattern = ArkSetupForFill8x8Pattern;
        xf86AccelInfoRec.SubsequentFill8x8Pattern =
            ArkSubsequentFill8x8Pattern;
#if 0
        xf86AccelInfoRec.SetupFor8x8PatternColorExpand = ArkSetupFor8x8PatternColorExpand;
        xf86AccelInfoRec.Subsequent8x8PatternColorExpand =
            ArkSubsequent8x8PatternColorExpand;
#endif
    }
    xf86InitPixmapCache(&vga256InfoRec, vga256InfoRec.virtualY *
        vga256InfoRec.displayWidth * vga256InfoRec.bitsPerPixel / 8,
        vga256InfoRec.videoRam * 1024 - 256 - arkCOPBufferSpaceSize);
}

void ArkSync() {
    WAITUNTILFINISHED();
}

/*
 * Solid rectangle fill.
 *
 * Any raster-op is supported.
 * Planemask is supported in all modes except 24bpp.
 * 24bpp mode only accepts colors with red, green and blue bytes equal.
 * Because the destination address is set in pixel units,
 * SubsequentFillRectSolid is shared for 8, 16 and 32bpp.
 */

static int current_x, current_y;

void Ark8SetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    SETFOREGROUNDCOLOR(color);
    /* ARK color mix precisely matches X raster-ops. */
    SETCOLORMIXSELECT(rop | (rop << 8));
    CommandFlags = 0;
    if ((planemask & 0xFF) == 0xFF)
        CommandFlags = DISABLEPLANEMASK;
    else {
        SETWRITEPLANEMASK(planemask);
        CommandFlags = 0;
    }
    current_x = current_y = -1;
}

void Ark16SetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    SETFOREGROUNDCOLOR(color);
    SETCOLORMIXSELECT(rop | (rop << 8));
    if ((planemask & 0xFFFF) == 0xFFFF)
        CommandFlags = DISABLEPLANEMASK;
    else {
        SETWRITEPLANEMASK(planemask);
        CommandFlags = 0;
    }
    current_x = current_y = -1;
}

void Ark24SetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    SETFOREGROUNDCOLOR(color);
    SETCOLORMIXSELECT(rop | (rop << 8));
    /* No planemask supported. */
    current_x = current_y = -1;
}

void Ark32SetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    SETFOREGROUNDCOLOR32(color);
    SETCOLORMIXSELECT(rop | (rop << 8));
    if ((planemask & 0xFFFFFF) == 0xFFFFFF)
        CommandFlags = DISABLEPLANEMASK;
    else {
        SETWRITEPLANEMASK(planemask);
        CommandFlags = 0;
    }
    current_x = current_y = -1;
}

void ArkSubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr;
    SETWIDTHANDHEIGHT(w, h);
    if (x != current_x || y != current_y) {
        destaddr = y * vga256InfoRec.displayWidth + x;
        SETDESTADDR(destaddr);
        current_x = x;
        current_y = y;
    }
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR | STENCILONES |
        DISABLECLIPPING | BITBLT | CommandFlags);
    current_y += h;
}

void Ark24SubsequentFillRectSolid(x, y, w, h, color, rop)
    int x, y, w, h, color, rop;
{
    /* Uses 8bpp COP mode. */
    int destaddr;
    destaddr = (y * vga256InfoRec.displayWidth + x) * 3;
    SETWIDTHANDHEIGHT(w * 3, h);
    SETDESTADDR(destaddr);
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR | STENCILONES |
        DISABLEPLANEMASK | DISABLECLIPPING | BITBLT);
}

/*
 * Screen-to-screen BitBLT.
 *
 * Any raster-op is supported.
 * Planemask is supported in all modes except 24bpp.
 */
 
void ArkSetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
transparency_color)
    int xdir, ydir;
    int rop;
    unsigned planemask;
    int transparency_color;
{
    CommandFlags = 0;
    /* Set up optional transparency compare. */
    if (transparency_color != -1) {
        if (vga256InfoRec.bitsPerPixel <= 16) {
            SETTRANSPARENCYCOLOR(transparency_color);
        }
        else {
            SETTRANSPARENCYCOLOR32(transparency_color);
        }
        CommandFlags = STENCILGENERATED;
        /* For the foreground (transparent), use DEST. */
        SETCOLORMIXSELECT(rop | 0x0500);
    }
    else {
        SETCOLORMIXSELECT(rop | (rop << 8));
    }
    /* Set up the blit direction. */
    if (ydir < 0)
	CommandFlags |= UP;
    if (xdir < 0)
	CommandFlags |= LEFT;
    /* Set up the planemask. */
    if ((vga256InfoRec.bitsPerPixel == 8 && (planemask & 0xFF) == 0xFF)
    || (vga256InfoRec.bitsPerPixel == 16 && (planemask & 0xFFFF) == 0xFFFF)
    || (vga256InfoRec.bitsPerPixel == 32 && (planemask & 0xFFFFFF) == 0xFFFFFF)
    || vga256InfoRec.bitsPerPixel == 24)
        CommandFlags |= DISABLEPLANEMASK;
    else
        SETWRITEPLANEMASK(planemask);
}

void ArkSubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
    int srcaddr, destaddr;
    if (CommandFlags & UP) {
        srcaddr = (y1 + h - 1) * vga256InfoRec.displayWidth;
	destaddr = (y2 + h - 1) * vga256InfoRec.displayWidth;
    } else {
        srcaddr = y1 * vga256InfoRec.displayWidth;
        destaddr = y2 * vga256InfoRec.displayWidth;
    }
    if (CommandFlags & LEFT) {
	srcaddr += x1 + w - 1;
	destaddr += x2 + w - 1;
    } else {
	srcaddr += x1;
	destaddr += x2;
    }
    if (vga256InfoRec.bitsPerPixel == 24) {
	/* Special case; the COP is in 8bpp pixel mode. */
	if (CommandFlags & LEFT) {
            SETSOURCEADDR(srcaddr * 3 + 2);
	    SETDESTADDR(destaddr * 3 + 2);
	}
	else {
	    SETSOURCEADDR(srcaddr * 3);
            SETDESTADDR(destaddr * 3);
	}
	SETWIDTHANDHEIGHT(w * 3, h);
    }
    else {
	SETSOURCEADDR(srcaddr);
	SETDESTADDR(destaddr);
	SETWIDTHANDHEIGHT(w, h);
    }
    SETCOMMAND(BACKGROUNDBITMAP | FOREGROUNDBITMAP |
        DISABLECLIPPING | BITBLT | CommandFlags);
}


void ArkSetupForScanlineScreenToScreenColorExpand(x, y, w, h, bg, fg, rop,
planemask)
    int x, y, w, h, bg, fg, rop;
    unsigned int planemask;
{
    int destaddr;
    unsigned int mask;
    destaddr = y * vga256InfoRec.displayWidth + x;
    SETDESTADDR(destaddr);
    SETWIDTHANDHEIGHT(w, 1);
    SETSTENCILPITCH(w);
#if 0
    SETBITMAPCONFIG(LINEARSTENCILADDR | LINEARSOURCEADDR | 
        LINEARDESTADDR);
#endif

    if (bg == -1) {
        SETCOLORMIXSELECT((rop << 8) | 0x05);	/* Background = Destination */
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETFOREGROUNDCOLOR32(fg);
        }
        else {
            SETFOREGROUNDCOLOR(fg);
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 32:
	    SETBGANDFGCOLOR((bg & 0xFFFF) + (fg << 16));
	    SETBGANDFGCOLORHIGH((bg >> 16) + (fg & 0xFFFF0000));
	    break;
        default:
    	    SETBGANDFGCOLOR(bg + (fg << 16));
	    break;
        }
        SETCOLORMIXSELECT(rop | (rop << 8));
    }
    mask = (1 << vga256InfoRec.depth) - 1;
    if ((planemask & mask) == mask)
        CommandFlags = DISABLEPLANEMASK;
    else {
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETWRITEPLANEMASK32(planemask);
        }
        else {
            SETWRITEPLANEMASK(planemask);
        }
        CommandFlags = 0;
    }
}

void ArkSubsequentScanlineScreenToScreenColorExpand(srcaddr)
    int srcaddr;
{
    SETSTENCILADDR(srcaddr);
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR | STENCILBITMAP
        | DISABLECLIPPING | BITBLT | CommandFlags);
}

void Ark24SetupForScanlineScreenToScreenColorExpand(x, y, w, h, bg, fg, rop,
planemask)
    int x, y, w, h, bg, fg, rop;
    unsigned int planemask;
{
    int destaddr;
    unsigned int mask;
    destaddr = (y * vga256InfoRec.displayWidth + x) * 3;
    SETDESTADDR(destaddr);
    SETWIDTHANDHEIGHT(w * 3, 1);
    SETSTENCILPITCH(w * 3);
#if 0
    SETBITMAPCONFIG(LINEARSTENCILADDR | LINEARSOURCEADDR | 
        LINEARDESTADDR);
#endif

    if (bg == -1) {
        SETCOLORMIXSELECT((rop << 8) | 0x05);	/* Background = Destination */
        SETFOREGROUNDCOLOR(fg);
    }
    else {
    	SETBGANDFGCOLOR((bg & 0xFF) | (fg << 16));
        SETCOLORMIXSELECT(rop | (rop << 8));
    }
    /* No planemask supported at 24bpp. */
}

void Ark24SubsequentScanlineScreenToScreenColorExpand(srcaddr)
    int srcaddr;
{
    SETSTENCILADDR(srcaddr);
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR | STENCILBITMAP
        | DISABLECLIPPING | BITBLT | DISABLEPLANEMASK);
}

void ArkSetupForScreenToScreenColorExpand(bg, fg, rop, planemask)
    int bg, fg, rop;
    unsigned int planemask;
{
    unsigned int mask;
    if (bg == -1) {
        SETCOLORMIXSELECT((rop << 8) | 0x05);	/* Background = Destination */
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETFOREGROUNDCOLOR32(fg);
        }
        else {
            SETFOREGROUNDCOLOR(fg);
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 32:
	    SETBGANDFGCOLOR((bg & 0xFFFF) + (fg << 16));
	    SETBGANDFGCOLORHIGH((bg >> 16) + (fg & 0xFFFF0000));
	    break;
        default:
    	    SETBGANDFGCOLOR(bg + (fg << 16));
	    break;
        }
        SETCOLORMIXSELECT(rop | (rop << 8));
    }
    mask = (1 << vga256InfoRec.depth) - 1;
    if ((planemask & mask) == mask)
        CommandFlags = DISABLEPLANEMASK;
    else {
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETWRITEPLANEMASK32(planemask);
        }
        else {
            SETWRITEPLANEMASK(planemask);
        }
        CommandFlags = 0;
    }
}

void ArkSubsequentScreenToScreenColorExpand(srcx, srcy, x, y, w, h)
    int srcx, srcy, x, y, w, h;
{
    int srcaddr, destaddr;
    /* source address is in linear stencil pixel units. */
    srcaddr = srcy * vga256InfoRec.displayWidth * 8 + srcx;
    destaddr = y * vga256InfoRec.displayWidth + x;
    SETSTENCILPITCH((w + 31) / 32);
    SETSTENCILADDR(srcaddr);
    SETDESTADDR(destaddr);
    SETWIDTHANDHEIGHT(w, h);
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR | STENCILBITMAP
        | DISABLECLIPPING | BITBLT | CommandFlags);
}

void ArkSubsequentBresenhamLine(x1, y1, octant, err, e1, e2, length)
    int x1, y1, octant, err, e1, e2, length;
{
    int destaddr;
    SETERRORTERM(err);
    SETERRORINCREMENT(e1, e2);
    destaddr = y1 * vga256InfoRec.displayWidth + x1;
    SETDESTADDR(destaddr);
    SETWIDTH(length);
    current_x = current_y = -1;
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR | STENCILONES
        | DISABLECLIPPING | LINEDRAW | CommandFlags | octant);
}

void ArkSetupForFill8x8Pattern(patternx, patterny, rop, planemask,
transparency_color)
    int patternx, patterny, rop;
    unsigned planemask;
    int transparency_color;
{
    unsigned int mask;
    CommandFlags = 0;
    /* Set up optional transparency compare. */
    if (transparency_color != -1) {
        if (vga256InfoRec.bitsPerPixel <= 16) {
            SETTRANSPARENCYCOLOR(transparency_color);
        }
        else {
            SETTRANSPARENCYCOLOR32(transparency_color);
        }
        CommandFlags = STENCILGENERATED;
        /* For the foreground (transparent), use DEST. */
        SETCOLORMIXSELECT(rop | 0x0500);
    }
    else {
        SETCOLORMIXSELECT(rop | (rop << 8));
    }
    mask = (1 << vga256InfoRec.depth) - 1;
    if ((planemask & mask) == mask)
        CommandFlags |= DISABLEPLANEMASK;
    else {
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETWRITEPLANEMASK32(planemask);
        }
        else {
            SETWRITEPLANEMASK(planemask);
        }
    }
}

void ArkSubsequentFill8x8Pattern(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
    int destaddr, patternaddr;
    patternaddr = patterny * vga256InfoRec.displayWidth + patternx;
    SETSOURCEADDR(patternaddr);
    SETSOURCEPITCH(8);
    destaddr = y * vga256InfoRec.displayWidth + x;
    SETDESTADDR(destaddr);
    SETWIDTHANDHEIGHT(w, h);
    SETCOMMAND(BACKGROUNDBITMAP | FOREGROUNDBITMAP |
        DISABLECLIPPING | PATTERN8X8 | BITBLT | CommandFlags);
    SETSOURCEPITCH(vga256InfoRec.displayWidth);
}

/* This doesn't work. */

void ArkSetupFor8x8PatternColorExpand(patternx, patterny, bg, fg, rop,
planemask)
    int patternx, patterny, bg, fg, rop;
    unsigned int planemask;
{
    unsigned int mask;
    CommandFlags = 0;
    if (bg == -1) {
        SETCOLORMIXSELECT((rop << 8) | 0x05);	/* Background = Destination */
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETFOREGROUNDCOLOR32(fg);
        }
        else {
            SETFOREGROUNDCOLOR(fg);
        }
    }
    else {
        switch (vga256InfoRec.bitsPerPixel) {
        case 32:
	    SETBGANDFGCOLOR((bg & 0xFFFF) + (fg << 16));
	    SETBGANDFGCOLORHIGH((bg >> 16) + (fg & 0xFFFF0000));
	    break;
        default:
    	    SETBGANDFGCOLOR(bg + (fg << 16));
	    break;
        }
        SETCOLORMIXSELECT(rop | (rop << 8));
    }
    mask = (1 << vga256InfoRec.depth) - 1;
    if ((planemask & mask) == mask)
        CommandFlags |= DISABLEPLANEMASK;
    else {
        if (vga256InfoRec.bitsPerPixel == 32) {
            SETWRITEPLANEMASK32(planemask);
        }
        else {
            SETWRITEPLANEMASK(planemask);
        }
    }
}

void ArkSubsequent8x8PatternColorExpand(patternx, patterny, x, y, w, h)
    int patternx, patterny;
    int x, y, w, h;
{
    int destaddr, patternaddr;
    patternaddr = patterny * vga256InfoRec.displayWidth * 8 + patternx;
    SETSTENCILADDR(patternaddr);
    destaddr = y * vga256InfoRec.displayWidth + x;
    SETDESTADDR(destaddr);
    SETSTENCILPITCH(8);
    SETWIDTHANDHEIGHT(w, h);
    SETCOMMAND(SELECTBGCOLOR | SELECTFGCOLOR |
        DISABLECLIPPING | PATTERN8X8 | BITBLT | CommandFlags);
    SETSOURCEPITCH(vga256InfoRec.displayWidth);
}

