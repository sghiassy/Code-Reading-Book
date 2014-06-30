
/*
 * ET4/6K acceleration interface.
 *
 * Uses Harm Hanemaayer's generic acceleration interface (XAA).
 *
 * Author: Koen Gadeyne
 *
 * Much of the acceleration code is based on the XF86_W32 server code from
 * Glenn Lai.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/et4_accel.c,v 3.11.2.5 1997/05/21 15:02:47 dawes Exp $ */


/*
 * if NO_OPTIMIZE is set, some optimizations are disabled.
 *
 * What it basically tries to do is minimize the amounts of writes to
 * accelerator registers, since these are the ones that slow down small
 * operations a lot.
 */

#undef NO_OPTIMIZE

#include "vga256.h"
#include "xf86.h"
#include "vga.h"
#include "tseng.h"
#include "tseng_acl.h"
#include "compiler.h"

#include "xf86xaa.h"

#include "miline.h"

void TsengSync();
void TsengSetupForFillRectSolid();
void Tseng4SubsequentFillRectSolid();
void Tseng6SubsequentFillRectSolid();

void TsengSubsequentFillTrapezoidSolid();

void TsengSetupForScreenToScreenCopy();
void TsengSubsequentScreenToScreenCopy();

void TsengSetupForScanlineScreenToScreenColorExpand();
void TsengSubsequentScanlineScreenToScreenColorExpand();

void TsengSubsequentBresenhamLine();
void TsengSubsequentTwoPointLine();

void TsengSetupForCPUToScreenColorExpand();
void TsengSubsequentCPUToScreenColorExpand();

void TsengSetupForScreenToScreenColorExpand();
void TsengSubsequentScreenToScreenColorExpand();

void TsengSetupForFill8x8Pattern();
void TsengSubsequentFill8x8Pattern();


static int bytesperpixel, powerPerPixel;
static int tseng_line_width;

/* These will hold the ping-pong registers.
 * Note that ping-pong registers might not be needed when using
 * BACKGROUND_OPERATIONS (because of the WAIT()-ing involved)
 */

static LongP MemFg;
static long Fg;

static LongP MemBg;
static long Bg;

static LongP MemPat;
static long Pat;

/* Do we use PCI-retry or busy-waiting */
extern Bool Use_Pci_Retry;


/*
 * The following function sets up the supported acceleration. Call it from
 * the FbInit() function in the SVGA driver. Do NOT initialize any hardware
 * in here. That belongs in tseng_init_acl().
 */
void TsengAccelInit() {
    /*
     * If you want to disable acceleration, just don't modify anything in
     * the AccelInfoRec.
     */

    /*
     * Set up the main acceleration flags.
     */
    xf86AccelInfoRec.Flags = BACKGROUND_OPERATIONS | PIXMAP_CACHE
      | HARDWARE_PATTERN_ALIGN_64 | HARDWARE_PATTERN_PROGRAMMED_ORIGIN;

    /* we'll disable COP_FRAMEBUFFER_CONCURRENCY for the public beta
     * release, because it causes font corruption. But THIS NEEDS TO BE
     * INVESTIGATED.
     */
/*      | COP_FRAMEBUFFER_CONCURRENCY;*/
      
    if (et4000_type >= TYPE_ET6000)
    {
      xf86AccelInfoRec.Flags |= HARDWARE_PATTERN_TRANSPARENCY;
    }

    /*
     * The following line installs a "Sync" function, that waits for
     * all coprocessor operations to complete.
     */
    xf86AccelInfoRec.Sync = TsengSync;

    /*
     * We want to set up the FillRectSolid primitive for filling a solid
     * rectangle.
     */
    xf86GCInfoRec.PolyFillRectSolidFlags = NO_PLANEMASK;

    xf86AccelInfoRec.SetupForFillRectSolid = TsengSetupForFillRectSolid;
    if (et4000_type >= TYPE_ET6000) {
      xf86AccelInfoRec.SubsequentFillRectSolid = Tseng6SubsequentFillRectSolid;
#if TRAPEZOIDS_FIXED
      /* disabled for now: not fully compliant yet */
      xf86AccelInfoRec.SubsequentFillTrapezoidSolid = TsengSubsequentFillTrapezoidSolid;
#endif
    }
    else
      xf86AccelInfoRec.SubsequentFillRectSolid = Tseng4SubsequentFillRectSolid;

    /*
     * We also want to set up the ScreenToScreenCopy (BitBLT) primitive for
     * copying a rectangular area from one location on the screen to
     * another. First we set up the restrictions. In this case, we
     * don't handle transparency color compare nor planemasks.
     */
    xf86GCInfoRec.CopyAreaFlags = NO_PLANEMASK;

    if (et4000_type < TYPE_ET6000)
      xf86GCInfoRec.CopyAreaFlags |= NO_TRANSPARENCY;
    
    xf86AccelInfoRec.SetupForScreenToScreenCopy =
        TsengSetupForScreenToScreenCopy;
    xf86AccelInfoRec.SubsequentScreenToScreenCopy =
        TsengSubsequentScreenToScreenCopy;

    /*
     * 8x8 pattern tiling not possible on W32 chips in 24bpp mode.
     * Currently, 24bpp pattern tiling doesn't work at all.
     */

#ifdef BPP24_BUG
    if ( !((et4000_type < TYPE_ET6000) && (vgaBitsPerPixel == 24)) )
#else
    if ( !(vgaBitsPerPixel == 24) )
#endif
    {
      xf86AccelInfoRec.SetupForFill8x8Pattern =
          TsengSetupForFill8x8Pattern;
      xf86AccelInfoRec.SubsequentFill8x8Pattern =
          TsengSubsequentFill8x8Pattern;
    }

    /*
     * Setup hardware-line-drawing code.
     */

    if (et4000_type >= TYPE_ET4000W32P)
    {
#if 0
    /* -- currently disabled because of major bugs... */
    xf86AccelInfoRec.SubsequentBresenhamLine =
        TsengSubsequentBresenhamLine;
    xf86AccelInfoRec.ErrorTermBits = 11;
#endif

    xf86AccelInfoRec.SubsequentTwoPointLine =
        TsengSubsequentTwoPointLine;

    xf86GCInfoRec.PolyLineSolidZeroWidthFlags =
         NO_TRANSPARENCY | NO_PLANEMASK | TWO_POINT_LINE_ERROR_TERM;
    xf86GCInfoRec.PolySegmentSolidZeroWidthFlags =
         NO_TRANSPARENCY | NO_PLANEMASK | TWO_POINT_LINE_ERROR_TERM;
    }

    /*
     * Color expansion primitives.
     * Since the ET6000 doesn't support CPU-to-screen color expansion,
     * we revert to scanline-screen-to-screen color expansion instead.
     * This is a less performant solution.
     */

    /*
     * ScanlineScreenToScreenColorExpand needs a linear ScratchBufferAddr,
     * so there's no point in providing this function when banked adressing
     * is used, although it is feasible (through an MMU aperture). XAA needs
     * to be modified to accomodate this. Currently it tries to write to
     * FrameBufferBase+ScratchBufferAddr, which is only valid in linear mode.
     *
     */

    if ( (et4000_type >= TYPE_ET6000) || (vgaBitsPerPixel == 8) )
    {
      xf86AccelInfoRec.ColorExpandFlags =
          BIT_ORDER_IN_BYTE_LSBFIRST | VIDEO_SOURCE_GRANULARITY_PIXEL | NO_PLANEMASK;

#if 1
      /* new and untested (not used by XAA yet -- needs testing) */

      xf86AccelInfoRec.SetupForScreenToScreenColorExpand =
          TsengSetupForScreenToScreenColorExpand;
      xf86AccelInfoRec.SubsequentScreenToScreenColorExpand =
          TsengSubsequentScreenToScreenColorExpand;
#endif

      xf86AccelInfoRec.SetupForScanlineScreenToScreenColorExpand =
          TsengSetupForScanlineScreenToScreenColorExpand;
      xf86AccelInfoRec.SubsequentScanlineScreenToScreenColorExpand =
          TsengSubsequentScanlineScreenToScreenColorExpand;
          
      /* triple-buffering is needed to account for double-buffering of Tseng
       * acceleration registers. Increasing this number doesn't help solve the
       * problems with both ET4000 and ET6000 with text rendering.
       */
      xf86AccelInfoRec.PingPongBuffers = 3;

      xf86AccelInfoRec.ScratchBufferSize = scratchVidBase + 1024 - (long) W32Mix;
      xf86AccelInfoRec.ScratchBufferAddr = W32Mix;

      if (!OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
      {
        /* in banked mode, use aperture #0 */
        xf86AccelInfoRec.ScratchBufferBase =
           (unsigned char *)
             ( ((int)vgaBase) + 0x18000L + 1024 - xf86AccelInfoRec.ScratchBufferSize );
      }
#if 0
      ErrorF("ColorExpand ScratchBuf: Addr = %d (0x%x); Size = %d (0x%x); Base = %d (0x%x)\n",
             xf86AccelInfoRec.ScratchBufferAddr, xf86AccelInfoRec.ScratchBufferAddr,
             xf86AccelInfoRec.ScratchBufferSize, xf86AccelInfoRec.ScratchBufferSize,
             xf86AccelInfoRec.ScratchBufferBase, xf86AccelInfoRec.ScratchBufferBase);
#endif
    }

#if 0
    /*
     * CPU-to-screen color expansion doesn't seem to be reliable yet. The
     * W32 needs the correct amount of data sent to it in this mode, or it
     * hangs the machine until is does (?). Currently, the init code in this
     * file or the XAA code that uses this does something wrong, so that
     * occasionally we get accelerator timeouts, and after a few, complete
     * system hangs.
     *
     * What works is this: tell XAA that we have SCANLINE_PAD_DWORD, and then
     * add the following code in TsengSubsequentCPUToScreenColorExpand():
     *     w = (w + 31) & ~31; this code rounds the width up to the nearest
     * multiple of 32, and together with SCANLINE_PAD_DWORD, this makes
     * CPU-to-screen color expansion work. Of course, the display isn't
     * correct (4 chars are "blanked out" when only one is written, for
     * example). But this shows that the principle works. But the code
     * doesn't...
     */
    if (et4000_type < TYPE_ET6000)
    {
      /*
       * CPU_TRANSFER_PAD_DWORD is implied by XAA, and I'm not sure this is
       * OK, because the W32 might be trying to expand the padding data.
       */
      xf86AccelInfoRec.ColorExpandFlags |=
          SCANLINE_NO_PAD | CPU_TRANSFER_BASE_FIXED;
        /* "| CPU_TRANSFER_PAD_DWORD" is implied, but should not be needed/allowed */
   
      xf86AccelInfoRec.SetupForCPUToScreenColorExpand =
          TsengSetupForCPUToScreenColorExpand;
      xf86AccelInfoRec.SubsequentCPUToScreenColorExpand =
          TsengSubsequentCPUToScreenColorExpand;
      
      /* we'll be using MMU aperture 2 */
      xf86AccelInfoRec.CPUToScreenColorExpandBase = CPU2ACLBase;
      /* ErrorF("CPU2ACLBase = 0x%x\n", CPU2ACLBase);*/
      /* aperture size is 8kb in banked mode. Larger in linear mode, but 8kb is enough */
      xf86AccelInfoRec.CPUToScreenColorExpandRange = 8192;
    }
#endif

    /*
     * Finally, we set up the video memory space available to the pixmap
     * cache. In this case, all memory from the end of the virtual screen to
     * the end of video memory minus 1K (which we already reserved), can be
     * used.
     */
    xf86InitPixmapCache(
        &vga256InfoRec,
        vga256InfoRec.virtualY * vga256InfoRec.displayWidth * vgaBitsPerPixel / 8,
        vga256InfoRec.videoRam * 1024
    );
    
    /*
     * For Tseng, we set up some often-used values
     */
     bytesperpixel = vgaBitsPerPixel / 8;
     switch (bytesperpixel)   /* for MULBPP optimization */
     {
       case 1: powerPerPixel = 0;
               break;
       case 2:
       case 3: powerPerPixel = 1;
               break;
       case 4: powerPerPixel = 2;
     }
     
     tseng_line_width = vga256InfoRec.displayWidth * bytesperpixel;

     /*
      * Init ping-pong registers.
      * This might be obsoleted by the BACKGROUND_OPERATIONS flag.
      */
     MemFg = MemW32ForegroundPing;
     Fg = W32ForegroundPing;
     MemBg = MemW32BackgroundPing;
     Bg = W32BackgroundPing;
     MemPat = MemW32PatternPing;
     Pat = W32PatternPing;
}


/*
 * Some commonly used macro's / inlines
 */

static __inline__ int COLOR_REPLICATE_DWORD(int color)
{
    switch (bytesperpixel)
    {
      case 1:
        color &= 0xFF;
        color = (color <<  8) | color;
        color = (color << 16) | color;
        break;
      case 2:
        color &= 0xFFFF;
        color = (color << 16) | color;
        break;
    }
    return color;
}

/*
 * Optimizing note: increasing the wrap size for fixed-color source/pattern
 * tiles from 4x1 (as below) to anything bigger doesn't seem to affect
 * performance (it might have been better for larger wraps, but it isn't).
 */

static __inline__ void SET_FG_COLOR(int color)
{
    *ACL_SOURCE_ADDRESS  = Fg;
    *ACL_SOURCE_Y_OFFSET = 3;
    color = COLOR_REPLICATE_DWORD(color);
    if (et4000_type >= TYPE_ET4000W32P)
    {
      *ACL_SOURCE_WRAP   = 0x02;
      *MemFg             = color;
    }
    else
    {
      *ACL_SOURCE_WRAP    = 0x12;
      *MemFg             = color;
      *(MemFg + 1)       = color;
    }
}

static __inline__ void SET_BG_COLOR(int color)
{
    *ACL_PATTERN_ADDRESS  = Pat;
    *ACL_PATTERN_Y_OFFSET = 3;
    color = COLOR_REPLICATE_DWORD(color);
    if (et4000_type >= TYPE_ET4000W32P)
    {
      *ACL_PATTERN_WRAP   = 0x02;
      *MemPat             = color;
    }
    else
    {
      *ACL_PATTERN_WRAP   = 0x12;
      *MemPat             = color;
      *(MemPat + 1)       = color;
    }
}

/*
 * this does the same as SET_FG_COLOR and SET_BG_COLOR together, but is
 * faster, because it allows the PCI chipset to chain the requests into a
 * burst sequence. The order of the commands is partly linear.
 * So far for the theory...
 */
static __inline__ void SET_FG_BG_COLOR(int fgcolor, int bgcolor)
{
    *ACL_PATTERN_ADDRESS  = Pat;
    *ACL_SOURCE_ADDRESS   = Fg;
    *((LongP) ACL_PATTERN_Y_OFFSET) = 0x00030003;
    fgcolor = COLOR_REPLICATE_DWORD(fgcolor);
    bgcolor = COLOR_REPLICATE_DWORD(bgcolor);
    if (et4000_type >= TYPE_ET4000W32P)
    {
      *ACL_SOURCE_WRAP    = 0x02;
      *ACL_PATTERN_WRAP   = 0x02;
      *MemFg              = fgcolor;
      *MemPat             = bgcolor;
    }
    else
    {
      *ACL_SOURCE_WRAP    = 0x12;
      *ACL_PATTERN_WRAP   = 0x12;
      *MemFg              = fgcolor;
      *(MemFg+1)          = fgcolor;
      *MemPat             = bgcolor;
      *(MemPat+1)         = bgcolor;
    }
}

#define SET_FUNCTION_BLT \
    if (et4000_type>=TYPE_ET6000) \
        *ACL_MIX_CONTROL     = 0x33; \
    else \
        *ACL_ROUTING_CONTROL = 0x00;

#define SET_FUNCTION_BLT_TR \
        *ACL_MIX_CONTROL     = 0x13;

#define SET_FUNCTION_COLOREXPAND \
    if (et4000_type >= TYPE_ET6000) \
      *ACL_MIX_CONTROL     = 0x32; \
    else \
      *ACL_ROUTING_CONTROL = 0x08;

#define SET_FUNCTION_COLOREXPAND_CPU \
    *ACL_ROUTING_CONTROL = 0x02;

/*
 * Real 32-bit multiplications are horribly slow compared to 16-bit (on i386).
 *
 * FBADDR() could be implemented completely in assembler on i386.
 */
#ifdef NO_OPTIMIZE
#define MULBPP(x) ((x) * bytesperpixel)
#else
static __inline__ int MULBPP(int x)
{
    int result = x << powerPerPixel;
    if (bytesperpixel != 3)
      return result;
    else
      return result + x;
}
#endif

#define FBADDR(x,y) ( (y) * tseng_line_width + MULBPP(x) )

#define SET_FG_ROP(rop) \
    *ACL_FOREGROUND_RASTER_OPERATION = W32OpTable[rop];

#define SET_BG_ROP(rop) \
    *ACL_BACKGROUND_RASTER_OPERATION = W32PatternOpTable[rop];

/* faster than separate functions */
#define SET_FG_BG_ROP(fgrop, bgrop) \
    *((WordP) ACL_BACKGROUND_RASTER_OPERATION) = \
        W32PatternOpTable[bgrop] | W32OpTable[fgrop];

#define SET_BG_ROP_TR(rop, bg_color) \
  if ((bg_color) == -1)    /* transparent color expansion */ \
    *ACL_BACKGROUND_RASTER_OPERATION = 0xaa; \
  else \
    *ACL_BACKGROUND_RASTER_OPERATION = W32PatternOpTable[rop];


static int old_x = 0, old_y = 0;

/* generic SET_XY */
static __inline__ void SET_XY(int x, int y)
{
  int new_x;
  if (et4000_type >= TYPE_ET6000)
    new_x = MULBPP(x)-1;
  else
    new_x = MULBPP(x-1);
  *((LongP) ACL_X_COUNT) = ((y - 1) << 16) + new_x;
  old_x = x; old_y = y;
}

/*
 * This is plain and simple "benchmark rigging".
 * (no real application does lots of subsequent same-size blits)
 *
 * The effect of this is amazingly good on e.g large blits: 400x400 rectangle fill
 * in 24 and 32 bpp jumps from 276 MB/sec to up to 490 MB/sec... But not always.
 * There must be a good reason why this gives such a boost, but I don't know it.
 */

static __inline__ void SET_XY_4(int x, int y)
{
    int new_xy;

    if ( (old_y != y) || (old_x != x) )
    {
      new_xy = ((y - 1) << 16) + MULBPP(x-1);
      *((LongP) ACL_X_COUNT) = new_xy;
      old_x = x; old_y = y;
    }
}

static __inline__ void SET_XY_6(int x, int y)
{
    int new_xy; /* using this intermediate variable is faster */

    if ( (old_y != y) || (old_x != x) )
    {
      new_xy = ((y - 1) << 16) + MULBPP(x)-1;
      *((LongP) ACL_X_COUNT) = new_xy;
      old_x = x; old_y = y;
    }
}


/* generic SET_XY_RAW */
static __inline__ void SET_XY_RAW(int x, int y)
{
  *((LongP) ACL_X_COUNT) = (y << 16) + x;
  old_x = x; old_y = y;
}

#define SET_DELTA(Min, Maj) \
    *((LongP) ACL_DELTA_MINOR) = ((Maj) << 16) + (Min)

#define SET_SECONDARY_DELTA(Min, Maj) \
    *((LongP) ACL_SECONDARY_DELTA_MINOR) = ((Maj) << 16) + (Min)


/* Must do 0x09 (in one operation) for the W32 */
#define START_ACL(dst) \
    *(ACL_DESTINATION_ADDRESS) = dst; \
    if (et4000_type < TYPE_ET4000W32P) *ACL_OPERATION_STATE = 0x09;

/* START_ACL for the ET6000 */
#define START_ACL_6(dst) \
    *(ACL_DESTINATION_ADDRESS) = dst;

#define START_ACL_CPU(dst) \
    *(ACL_DESTINATION_ADDRESS) = dst;

static __inline__ void PINGPONG()
{
    if (Fg == W32ForegroundPing)
    {
      MemFg  = MemW32ForegroundPong; Fg = W32ForegroundPong; 
      MemBg  = MemW32BackgroundPong; Bg = W32BackgroundPong; 
      MemPat = MemW32PatternPong;   Pat = W32PatternPong; 
    }
    else
    {
      MemFg  = MemW32ForegroundPing; Fg = W32ForegroundPing;
      MemBg  = MemW32BackgroundPing; Bg = W32BackgroundPing; 
      MemPat = MemW32PatternPing;   Pat = W32PatternPing;
    }
}

static int old_dir=-1;

#ifdef NO_OPTIMIZE
#define SET_XYDIR(dir) \
      *ACL_XY_DIRECTION = (dir);
#else
/*
 * only changing ACL_XY_DIRECTION when it needs to be changed avoids
 * unnecessary PCI bus writes, which are slow. This shows up very well
 * on consecutive small fills.
 */
#define SET_XYDIR(dir) \
    if ((dir) != old_dir) \
      *ACL_XY_DIRECTION = old_dir = (dir);
#endif

#define SET_SECONDARY_XYDIR(dir) \
      *ACL_SECONDARY_EDGE = (dir);

/*
 * This is the implementation of the Sync() function.
 *
 * To avoid pipeline/cache/buffer flushing in the PCI subsystem and the VGA
 * controller, we might replace this read-intensive code with a dummy
 * accelerator operation that causes a hardware-blocking (wait-states) until
 * the running operation is done.
 */
void TsengSync() {
    WAIT_ACL;
}

/*
 * This is the implementation of the SetupForFillRectSolid function
 * that sets up the coprocessor for a subsequent batch for solid
 * rectangle fills.
 */
void TsengSetupForFillRectSolid(color, rop, planemask)
    int color, rop;
    unsigned planemask;
{
    /*
     * all registers are queued in the Tseng chips, except of course for the
     * stuff we want to store in off-screen memory. So we have to use a
     * ping-pong method for those if we want to avoid having to wait for the
     * accelerator when we want to write to these.
     */
     
/*    ErrorF("S");*/

#ifdef DEBUG_PLANEMASK
    if (planemask != -1)
      ErrorF("SetupForFillRectSolid: BUG: planemask = 0x%x\n", planemask);
#endif

    PINGPONG();

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

    SET_FG_ROP(rop);
    SET_FG_COLOR(color);
    
    SET_FUNCTION_BLT;
}

/*
 * This is the implementation of the SubsequentForFillRectSolid function
 * that sends commands to the coprocessor to fill a solid rectangle of
 * the specified location and size, with the parameters from the SetUp
 * call.
 *
 * Splitting it up between ET4000 and ET6000 avoids lots of "if (et4000_type
 * >= TYPE_ET6000)" -style comparisons.
 */
void Tseng4SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr = FBADDR(x,y);

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

    SET_XYDIR(0);

    SET_XY_4(w, h);
    START_ACL(destaddr);
}

void Tseng6SubsequentFillRectSolid(x, y, w, h)
    int x, y, w, h;
{
    int destaddr = FBADDR(x,y);

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

   /* if XYDIR is not reset here, drawing a hardware line in between
    * blitting, with the same ROP, color, etc will not cause a call to
    * SetupFor... (because linedrawing uses SetupForSolidFill() as its
    * Setup() function), and thus the direction register will have been
    * changed by the last LineDraw operation.
    */
    SET_XYDIR(0);

    SET_XY_6(w, h);
    START_ACL_6(destaddr);
}


/*
 * This is the implementation of the SetupForScreenToScreenCopy function
 * that sets up the coprocessor for a subsequent batch of
 * screen-to-screen copies. Remember, we don't handle transparency,
 * so the transparency color is ignored.
 */
static int blitxdir, blitydir;
 
void TsengSetupForScreenToScreenCopy(xdir, ydir, rop, planemask,
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

    int blit_dir=0;

/*    ErrorF("C%d ", transparency_color);*/

    blitxdir = xdir;
    blitydir = ydir;
    
    if (xdir == -1) blit_dir |= 0x1;
    if (ydir == -1) blit_dir |= 0x2;

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

    SET_XYDIR(blit_dir);
    
    if ((et4000_type >= TYPE_ET6000) && (transparency_color != -1))
    {
      SET_BG_COLOR(transparency_color);
      SET_FUNCTION_BLT_TR;
    }
    else
      SET_FUNCTION_BLT;

    SET_FG_ROP(rop);

    *ACL_SOURCE_WRAP = 0x77; /* no wrap */
    *ACL_SOURCE_Y_OFFSET = tseng_line_width-1;
}

/*
 * This is the implementation of the SubsequentForScreenToScreenCopy
 * that sends commands to the coprocessor to perform a screen-to-screen
 * copy of the specified areas, with the parameters from the SetUp call.
 * In this sample implementation, the direction must be taken into
 * account when calculating the addresses (with coordinates, it might be
 * a little easier).
 *
 * Splitting up the SubsequentScreenToScreenCopy between ET4000 and ET6000
 * doesn't seem to improve speed for small blits (as it did with
 * FillRectSolid).
 */


void TsengSubsequentScreenToScreenCopy(x1, y1, x2, y2, w, h)
    int x1, y1, x2, y2, w, h;
{
    int srcaddr, destaddr;

    /*
     * Optimizing note: the pre-calc code below (i.e. until the first
     * register write) doesn't significantly affect performance. Removing it
     * all boosts small blits from 24.22 to 25.47 MB/sec. Don't waste time
     * on that. One less PCI bus write would boost us to 30.00 MB/sec, up
     * from 24.22. Waste time on _that_...
     */

    /* tseng chips want x-sizes in bytes, not pixels */
    x1 = MULBPP(x1);
    x2 = MULBPP(x2);
    
    /*
     * If the direction is "decreasing", the chip wants the addresses
     * to be at the other end, so we must be aware of that in our
     * calculations.
     */
    if (blitydir == -1) {
        srcaddr = (y1 + h - 1) * tseng_line_width;
        destaddr = (y2 + h - 1) * tseng_line_width;
    } else {
        srcaddr = y1 * tseng_line_width;
        destaddr = y2 * tseng_line_width;
    }
    if (blitxdir == -1) {
        /* Accelerator start address must point to first byte to be processed.
         * Depending on the direction, this is the first or the last byte
         * in the multi-byte pixel.
         */
        int eol = MULBPP(w);
        srcaddr += x1 + eol - 1;
        destaddr += x2 + eol - 1;
    } else {
        srcaddr += x1;
        destaddr += x2;
    }

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

    SET_XY(w, h);
    *ACL_SOURCE_ADDRESS = srcaddr;
    START_ACL(destaddr);
}

static int pat_src_addr;

void TsengSetupForFill8x8Pattern(patternx, patterny, rop, planemask, transparency_color)
   int patternx, patterny;
   int rop;
   unsigned int planemask;
   int transparency_color;
{
  pat_src_addr = FBADDR(patternx, patterny);
  
/*  ErrorF("P");*/

  /* Avoid PCI-Retry's */
  if (!Use_Pci_Retry) WAIT_QUEUE;

  SET_FG_ROP(rop);

  if ((et4000_type >= TYPE_ET6000) && (transparency_color != -1))
  {
    SET_BG_COLOR(transparency_color);
    SET_FUNCTION_BLT_TR;
  }
  else
    SET_FUNCTION_BLT;

  switch(bytesperpixel)
  {
    case 1: *ACL_SOURCE_WRAP      = 0x33;   /* 8x8 wrap */
            *ACL_SOURCE_Y_OFFSET  = 8 - 1;
            break;
    case 2: *ACL_SOURCE_WRAP      = 0x34;   /* 16x8 wrap */
            *ACL_SOURCE_Y_OFFSET  = 16 - 1;
            break;
    case 3: *ACL_SOURCE_WRAP      = 0x3D;   /* 24x8 wrap --- only for ET6000 !!! */
            *ACL_SOURCE_Y_OFFSET  = 32 - 1; /* this is no error -- see databook */
            break;
    case 4: *ACL_SOURCE_WRAP      = 0x35;   /* 32x8 wrap */
            *ACL_SOURCE_Y_OFFSET  = 32 - 1;
  }
  
  SET_XYDIR(0);
}

void TsengSubsequentFill8x8Pattern(patternx, patterny, x, y, w, h)
   int patternx, patterny;
   int x, y;
   int w, h;
{
  int destaddr = FBADDR(x,y);
  int srcaddr = pat_src_addr + MULBPP(patterny * 8 + patternx);

  /* Avoid PCI-Retry's */
  if (!Use_Pci_Retry) WAIT_QUEUE;

  *ACL_SOURCE_ADDRESS = srcaddr;

  SET_XY(w, h);
  START_ACL(destaddr);
}


static int ColorExpandDst;

void TsengSetupForScanlineScreenToScreenColorExpand(x, y, w, h, bg, fg, rop, planemask)
    int x, y;
    int w, h;
    int bg, fg;
    int rop;
    unsigned int planemask;
{

/*    ErrorF("X");*/

    PINGPONG();

    ColorExpandDst = FBADDR(x,y);

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

    SET_FG_ROP(rop);
    SET_BG_ROP_TR(rop, bg);
      
    SET_FG_BG_COLOR(fg, bg);

    SET_FUNCTION_COLOREXPAND;

    *ACL_MIX_Y_OFFSET = 0x0FFF; /* see remark below */

    SET_XYDIR(0);

    SET_XY(w, 1);
}

void TsengSubsequentScanlineScreenToScreenColorExpand(srcaddr)
    int srcaddr;
{
    /* COP_FRAMEBUFFER_CONCURRENCY can cause text corruption !!!
     *
     * Looks like some scanline data DWORDS are not written to the ping-pong
     * framebuffers, so that old data is rendered in some places. Is this
     * caused by PCI host bridge queueing? Or is data lost when written
     * while the accelerator is accessing the framebuffer (which would be
     * the real reason NOT to use COP_FRAMEBUFFER_CONCURRENCY)?
     *
     * Even with ping-ponging, parts of scanline three (which are supposed
     * to be written to the old, already rendered scanline 1 buffer) have
     * not yet arrived in the framebuffer, and thus some parts of the new
     * scanline are rendered with data from two lines above it.
     *
     * Extra problem: ET6000 queueing really needs triple buffering for
     * this, because XAA can still overwrite scanline 1 when writing data
     * for scanline three. Right _after_ that, the accelerator blocks on
     * queueing in TsengSubsequentScanlineScreenToScreenColorExpand(), but
     * then it's too late: the scanline data is already overwritten. That's
     * why we use 3 ping-pong buffers.
     *
     * "x11perf -fitext" is about 530k chars/sec now, but with
     * COP_FRAMEBUFFER_CONCURRENCY, this goes up to >700k (which is similar
     * to what Xinside can do).
     *
     * Needs to be investigated!
     *
     * Update: this seems to depend upon ACL_MIX_Y_OFFSET, although that
     * register should not do anything at all here (only one line done at a
     * time, so no Y_OFFSET needed). Setting the offset to 0x0FFF seems to
     * remedy this situation most of the time (still an occasional error
     * here and there). This _could_ be a bug, but then it would have to be
     * in both in the ET6000 _and_ the ET4000W32p.
     *
     * The more delay added after starting a color-expansion operation, the
     * less font corruption we get. But nothing really solves it.
     */
    
    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;

    *ACL_MIX_ADDRESS = srcaddr;
    START_ACL(ColorExpandDst);
    ColorExpandDst += tseng_line_width;
    
    /*
     * If not using triple-buffering, we need to wait for the queued
     * register set to be transferred to the working register set here,
     * because otherwise an e.g. double-buffering mechanism could overwrite
     * the buffer that's currently being worked with with new data too soon.
     *
     * WAIT_QUEUE; // not needed with triple-buffering
     */
}


/*
 * CPU-to-Screen color expansion.
 *   This is for ET4000 only (The ET6000 cannot do this)
 */

void TsengSetupForCPUToScreenColorExpand(bg, fg, rop, planemask)
   int bg, fg;
   int rop;
   unsigned int planemask;
{
/*  ErrorF("X");*/

  PINGPONG();

  /* Avoid PCI-Retry's */
  if (!Use_Pci_Retry) WAIT_QUEUE;

  SET_FG_ROP(rop);
  SET_BG_ROP_TR(rop, bg);

  SET_XYDIR(0);

  SET_FG_BG_COLOR(fg,bg);

  SET_FUNCTION_COLOREXPAND_CPU;

  *ACL_MIX_ADDRESS  = 0;
}


/*
 * TsengSubsequentCPUToScreenColorExpand() is potentially dangerous:
 *   Not writing enough data to the MMU aperture for CPU-to-screen color
 *   expansion will eventually cause a system deadlock!
 */

void TsengSubsequentCPUToScreenColorExpand(x, y, w, h, skipleft)
   int x, y;
   int w, h;
   int skipleft;
{
  int destaddr = FBADDR(x,y);

  /* ErrorF("\n %dx%d",w,h); */
  
  /* Avoid PCI-Retry's */
  if (!Use_Pci_Retry) WAIT_QUEUE;

  *ACL_MIX_Y_OFFSET = w-1;
  SET_XY(w, h);
  START_ACL_CPU(destaddr);
}


void TsengSetupForScreenToScreenColorExpand(bg, fg, rop, planemask)
   int bg, fg;
   int rop;
   unsigned int planemask;
{
/*  ErrorF("SSC ");*/

  PINGPONG();

  /* Avoid PCI-Retry's */
  if (!Use_Pci_Retry) WAIT_QUEUE;

  SET_FG_ROP(rop);
  SET_BG_ROP_TR(rop, bg);
      
  SET_FUNCTION_COLOREXPAND;

  SET_FG_BG_COLOR(fg, bg);

  SET_XYDIR(0);
}

void TsengSubsequentScreenToScreenColorExpand(srcx, srcy, x, y, w, h)
   int srcx, srcy;
   int x, y;
   int w, h;
{
  int destaddr = FBADDR(x,y);

  int mixaddr = FBADDR(srcx, srcy * 8);
  
  /* Avoid PCI-Retry's */
  if (!Use_Pci_Retry) WAIT_QUEUE;

  SET_XY(w, h);
  *ACL_MIX_ADDRESS = mixaddr;
  *ACL_MIX_Y_OFFSET = w-1;

  START_ACL(destaddr);
}


/*
 * W32p/ET6000 hardware linedraw code. 
 *
 * Actually, the Tseng engines are rather slow line-drawing machines.
 * On 350-pixel long lines, the _raw_ accelerator performance is about
 * 24400 lines per second. And that is "only" about 8.5 Million pixels
 * per second.
 * The break-even point is a Pentium-133, which does line-drawing as fast as
 * the ET6000, including X-overhead.
 * Of course, there's the issue of hardware parallellism that makes a difference.
 *
 * TsengSetupForFillRectSolid() is used as a setup function
 */

void TsengSubsequentBresenhamLine(x1, y1, octant, err, e1, e2, length)
   int x1, y1;
   int octant;
   int err, e1, e2;
   int length;
{
   int algrthm=0, direction=0;
   int destaddr = FBADDR(x1,y1);
   
   direction = W32BresTable[octant];

   if (octant & XDECREASING)
     destaddr += bytesperpixel-1;

   if (!(octant & YDECREASING))
     algrthm = 16;

   /* Avoid PCI-Retry's */
   if (!Use_Pci_Retry) WAIT_QUEUE;
   
   if (!(octant & YMAJOR))
   {
       SET_XY_RAW(length * bytesperpixel - 1, 0xFFF);
   }
   else
   {
       SET_XY_RAW(0xFFF, length -1);
   }

   SET_DELTA(e1>>1, length);
   
   SET_XYDIR(0x80 | algrthm | direction);
   
   START_ACL(destaddr);
}

/*
 * Two-point lines.
 *
 * Three major problems that needed to be solved here:
 *
 * 1. The "bias" value must be translated into the "line draw algorithm"
 *    parameter in the Tseng accelerators. This parameter, although not
 *    documented as such, needs to be set to the _inverse_ of the
 *    appropriate bias bit (i.e. for the appropriate octant).
 *
 * 2. In >8bpp modes, the accelerator will render BYTES in the same order as
 *    it is drawing the line. This means it will render the colors in the
 *    same order as well, reversing the byte-order in pixels that are drawn
 *    right-to-left. This causes wrong colors to be rendered.
 *
 * 3. The Tseng data book says that the ACL Y count register needs to be
 *    programmed with "dy-1". A similar thing is said about ACL X count. But
 *    this assumes (x2,y2) is NOT drawn (although that is not mentionned in
 *    the data book). X assumes the endpoint _is_ drawn. If "dy-1" is used,
 *    this sometimes results in a negative value (if dx==dy==0),
 *    causing a complete accelerator hang.
 */

void TsengSubsequentTwoPointLine(x1, y1, x2, y2, bias)
   int x1, y1;
   int x2, y2; /* excl. */
   int bias;
{
   int dx, dy, temp, destaddr, algrthm;
   int dir_reg = 0x80;
   int octant=0;
 
/*   ErrorF("L"); */

   /*
    * Fix drawing "bug" when drawing right-to-left (dx<0). This could also be
    * fixed by changing the offset in the color "pattern" instead if dx < 0
    */
   if (bytesperpixel > 1)
   {
     if (x2 < x1)
     {
       temp = x1; x1 = x2; x2 = temp; 
       temp = y1; y1 = y2; y2 = temp; 
     }
   }

   destaddr = FBADDR(x1, y1);
   
   /* modified from CalcLineDeltas() macro */

   /* compute X direction, and take abs(delta-X) */
   dx = x2 - x1;
   if (dx<0)
     {
       dir_reg |= 1;
       octant |= XDECREASING;
       dx = -dx;
       /* destaddr must point to highest addressed byte in the pixel when drawing
        * right-to-left
        */
       destaddr += bytesperpixel-1;
     }

   /* compute delta-Y */
   dy = y2-y1;

   /* compute Y direction, and take abs(delta-Y) */
   if (dy<0)
     {
       dir_reg |= 2;
       octant |= YDECREASING;
       dy = -dy;
     }

   /* Avoid PCI-Retry's */
   if (!Use_Pci_Retry) WAIT_QUEUE;

   /* compute axial direction and load registers */
   if (dx >= dy)  /* X is major axis */
   {
     dir_reg |= 4;
     SET_XY_RAW(MULBPP(dx), 0xFFF);
     SET_DELTA(dy, dx);
   }
   else           /* Y is major axis */
   {
     SetYMajorOctant(octant);
     SET_XY_RAW(0xFFF, dy);
     SET_DELTA(dx, dy);
   }

   /* select "linedraw algorithm" (=bias) and load direction register */
   algrthm = ((bias >> octant) & 1) ^ 1;

   dir_reg |= algrthm << 4;
   SET_XYDIR(dir_reg);

   START_ACL(destaddr);
}

/*
 * Trapezoid filling code.
 *
 * TsengSetupForFillRectSolid() is used as a setup function
 */

#undef USE_ERROR_TERM
#undef DEBUG_TRAP

void TsengSubsequentFillTrapezoidSolid(ytop, height, left, dxL, dyL, eL, right, dxR, dyR, eR)
        int ytop;
        int height;
        int left;
        int dxL, dyL;
        int eL;
        int right;
        int dxR, dyR;
        int eR;
{
    int destaddr, algrthm;
    int xcount = right - left + 1;
#ifdef USE_ERROR_TERM
    int dir_reg = 0x60;
    int sec_dir_reg = 0x20;
#else
    int dir_reg = 0x40;
    int sec_dir_reg = 0x00;
#endif
    int octant=0;
    int bias = 0x00; /* FIXME !!! */

/*    ErrorF("#");*/

#ifdef DEBUG_TRAP
    ErrorF("ytop=%d, height=%d, left=%d, dxL=%d, dyL=%d, eL=%d, right=%d, dxR=%d, dyR=%d, eR=%d ",
            ytop, height, left, dxL, dyL, eL, right, dxR, dyR, eR);
#endif

    if ((dyL < 0) || (dyR < 0)) ErrorF("Tseng Trapezoids: Wrong assumption: dyL/R < 0\n");

    destaddr = FBADDR(left,ytop);

    /* left edge */
    if (dxL<0)
      {
        dir_reg |= 1;
        octant |= XDECREASING;
        dxL = -dxL;
      }

    /* Y direction is always positive (top-to-bottom drawing) */

    /* Avoid PCI-Retry's */
    if (!Use_Pci_Retry) WAIT_QUEUE;


    /* left edge */
    /* compute axial direction and load registers */
    if (dxL >= dyL)  /* X is major axis */
    {
      dir_reg |= 4;
      SET_DELTA(dyL, dxL);
      if (dir_reg & 1) {      /* edge coherency: draw left edge */
         destaddr += bytesperpixel;
         sec_dir_reg |= 0x80;
         xcount--;
      }
    }
    else           /* Y is major axis */
    {
      SetYMajorOctant(octant);
      SET_DELTA(dxL, dyL);
    }
#ifdef USE_ERROR_TERM
    *ACL_ERROR_TERM = eL-1;
#endif

    /* select "linedraw algorithm" (=bias) and load direction register */
    /* ErrorF(" o=%d ", octant);*/
    algrthm = ((bias >> octant) & 1) ^ 1;
    dir_reg |= algrthm << 4;
    SET_XYDIR(dir_reg);


    /* right edge */
    if (dxR<0)
      {
        sec_dir_reg |= 1;
        dxR = -dxR;
      }

    /* compute axial direction and load registers */
    if (dxR >= dyR)  /* X is major axis */
    {
      sec_dir_reg |= 4;
      SET_SECONDARY_DELTA(dyR, dxR);
      if (dir_reg & 1) {      /* edge coherency: do not draw right edge */
        sec_dir_reg |= 0x40;
        xcount++;
      }
    }
    else           /* Y is major axis */
    {
      SET_SECONDARY_DELTA(dxR, dyR);
    }
#ifdef USE_ERROR_TERM
    *ACL_SECONDARY_ERROR_TERM = eR;
#endif

    /* ErrorF("%02x", sec_dir_reg);*/
    SET_SECONDARY_XYDIR(sec_dir_reg);

    SET_XY_6(xcount, height);

#ifdef DEBUG_TRAP
    ErrorF("-> %d,%d\n", xcount, height);
#endif

    START_ACL_6(destaddr);
}

