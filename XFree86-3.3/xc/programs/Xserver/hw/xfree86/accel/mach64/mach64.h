/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64.h,v 3.14 1997/01/18 06:54:36 dawes Exp $ */
/*
 * Copyright 1992,1993,1994,1995,1996 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Kevin E. Martin not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Kevin E. Martin
 * makes no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: mach64.h /main/9 1996/10/27 18:06:13 kaleb $ */

#ifndef MACH64
#define MACH64_H

#define MACH64_PATCHLEVEL "0"

#define MACH64_CURSBYTES	1024
#define MACH64_CURSMAX		64

#include "X.h"
#include "input.h"
#include "pixmap.h"
#include "region.h"
#include "gc.h"
#include "gcstruct.h"
#include "colormap.h"
#include "colormapst.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "servermd.h"
#include "windowstr.h"
#include "misc.h"
#include "xf86.h"
#include "regionstr.h"
#include "xf86_OSlib.h"
#include "xf86Procs.h"

#include "regionstr.h"
#include "regmach64.h"

extern pointer mach64VideoMem;
extern pointer vgaBase;
extern Bool xf86VTSema;
extern int mach64MaxX, mach64MaxY;
extern int mach64VirtX, mach64VirtY;
extern Bool mach64DAC8Bit;

#ifdef PIXPRIV
extern int mach64PixmapIndex;
#endif

extern int mach64Ramdac;
extern int mach64RamdacSubType;
extern int mach64BusType;
extern int mach64MemType;
extern int mach64ChipType;
extern int mach64ChipRev;
extern int mach64ClockType;
extern int mach64Clocks[MACH64_NUM_CLOCKS];
extern int mach64MinFreq;
extern int mach64MaxFreq;
extern int mach64RefFreq;
extern int mach64RefDivider;
extern int mach64NAdj;
extern int mach64CXClk;
extern int mach64MemClk;
extern int mach64DRAMMemClk;
extern int mach64VRAMMemClk;
extern int mach64MemCycle;

extern Bool mach64IntegratedController;

extern unsigned int mach64MemorySize;
extern unsigned int mach64ApertureSize;
extern unsigned long mach64ApertureAddr;

extern short mach64WeightMask;

extern ScrnInfoRec mach64InfoRec;
extern int mach64ValidTokens[];

extern int mach64alu[];

extern Bool checkCursorColor;

extern void (*mach64ImageReadFunc)(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    int,
    char *,
    int,
    int,
    int,
    unsigned long
#endif 
);

extern void (*mach64ImageWriteFunc)(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    int,
    char *,
    int,
    int,
    int,
    int,
    unsigned long
#endif 
);

/* Function Prototypes */

/* mach64.c */
Bool mach64Probe(
#if NeedFunctionPrototypes
    void
#endif
);
void mach64PrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
Bool mach64Initialize(
#if NeedFunctionPrototypes
    int scr_index,
    ScreenPtr pScreen,
    int argc,
    char **argv
#endif
);
void mach64EnterLeaveVT(
#if NeedFunctionPrototypes
    Bool enter,
    int screen_idx
#endif
);
Bool mach64CloseScreen(
#if NeedFunctionPrototypes
    int screen_idx,
    ScreenPtr pScreen
#endif
);
Bool mach64SaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    Bool on
#endif
);
void mach64AdjustFrame(
#if NeedFunctionPrototypes
    int x,
    int y
#endif
);
Bool mach64SwitchMode(
#if NeedFunctionPrototypes
    DisplayModePtr mode
#endif
);
void mach64DPMSSet(
#if NeedFunctionPrototypes
    int PowerManagementMode
#endif
);
/* mach64cmap.c */
int mach64ListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    Colormap *pmaps
#endif
);
int mach64GetInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    ColormapPtr *pmap
#endif
);
void mach64StoreColors(
#if NeedFunctionPrototypes
    ColormapPtr pmap,
    int ndef,
    xColorItem *pdefs
#endif
);
void mach64InstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr pmap
#endif
);
void mach64UninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr pmap
#endif
);
void mach64RestoreColor0(
#if NeedFunctionPrototypes
    ScreenPtr pScreen
#endif
);
/* mach64gc.c */
void mach64InitGC(
#if NeedFunctionPrototypes
    void
#endif
);
Bool mach64CreateGC(
#if NeedFunctionPrototypes
    register GCPtr pGC
#endif
);
/* mach64fs.c */
void mach64SolidFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
void mach64TiledFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
void mach64StipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
void mach64OStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
/* mach64win.c */
void mach64CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr pWin,
    DDXPointRec ptOldOrg,
    RegionPtr prgnSrc
#endif
);
/* mach64init.c */
void mach64CalcCRTCRegs(
#if NeedFunctionPrototypes
    mach64CRTCRegPtr crtcRegs,
    DisplayModePtr mode
#endif
);
void mach64SetCRTCRegs(
#if NeedFunctionPrototypes
    mach64CRTCRegPtr crtcRegs
#endif
);
void mach64SaveLUT(
#if NeedFunctionPrototypes
    LUTENTRY *lut
#endif
);
void mach64RestoreLUT(
#if NeedFunctionPrototypes
    LUTENTRY *lut
#endif
);
void mach64InitLUT(
#if NeedFunctionPrototypes
    void
#endif
);
void mach64InitEnvironment(
#if NeedFunctionPrototypes
    void
#endif
);
void mach64InitAperture(
#if NeedFunctionPrototypes
    int screen_idx
#endif
);
void mach64P_RGB514Index(
#if NeedFunctionPrototypes
    int index,
    int data
#endif
);
unsigned char mach64R_RGB514Index(
#if NeedFunctionPrototypes
    int index
#endif
);
void mach64SetRamdac(
#if NeedFunctionPrototypes
    int colorDepth,
    int AccelMode,
    int dotClock
#endif
);
void mach64InitDisplay(
#if NeedFunctionPrototypes
    int screen_idx
#endif
);
void mach64CleanUp(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach64im.c */
void mach64ImageInit(
#if NeedFunctionPrototypes
    void
#endif
);
void mach64ImageStippleFunc(
#if NeedFunctionPrototypes
    int x,
    int y,
    int w,
    int h,
    char *psrc,
    int pwidth,
    int px,
    int py,
    Pixel fgPixel,
    Pixel bgPixel,
    int alu,
    unsigned long planemask,
    int opaque
#endif
);
/* mach64bstor.c */
void mach64SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr pPixmap,
    RegionPtr prgnSave,
    int xorg,
    int yorg,
    WindowPtr pWin
#endif
);
void mach64RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr pPixmap,
    RegionPtr prgnRestore,
    int xorg,
    int yorg,
    WindowPtr pWin
#endif
);
/* mach64scrin.c */
Bool mach64ScreenInit(
#if NeedFunctionPrototypes
    register ScreenPtr pScreen,
    pointer pbits,
    int xsize,
    int ysize,
    int dpix,
    int dpiy,
    int width
#endif
);
/* mach64blt.c */
RegionPtr mach64CopyArea(
#if NeedFunctionPrototypes
    register DrawablePtr pSrcDrawable,
    register DrawablePtr pDstDrawable,
    GC *pGC,
    int srcx,
    int srcy,
    int width,
    int height,
    int dstx,
    int dsty
#endif
);
void mach64DoBitBlt(
#if NeedFunctionPrototypes
    DrawablePtr pSrc,
    DrawablePtr pDst,
    GC *pGC,
    RegionPtr rgnDst,
    DDXPointPtr pptSrc,
    unsigned long bitPlane
#endif
);
RegionPtr mach64CopyPlane(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    GCPtr pGC,
    int srcx,
    int srcy,
    int width,
    int height,
    int dstx,
    int dsty,
    unsigned long bitPlane
#endif
);
/* mach64pcach.c */
void mach64CacheInit(
#if NeedFunctionPrototypes
    int w,
    int h 
#endif
);
void mach64CacheFreeSlot(
#if NeedFunctionPrototypes
    PixmapPtr pix
#endif
);
void mach64IncrementCacheLRU(
#if NeedFunctionPrototypes
    int slot
#endif
);
int mach64CacheTile(
#if NeedFunctionPrototypes
    PixmapPtr pix
#endif
);
int mach64CacheStipple(
#if NeedFunctionPrototypes
    PixmapPtr pix,
    int fg
#endif
);
int mach64CacheOpStipple(
#if NeedFunctionPrototypes
    PixmapPtr pix,
    int fg,
    int bg
#endif
);
/* mach64line.c */
void mach64Line(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int mode,
    int npt,
    DDXPointPtr pptInit
#endif
);
/* mach64seg.c */
void mach64Segment(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nseg,
    register xSegment *pSeg
#endif
);
/* mach64orect.c */
void mach64PolyRectangle(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    register GCPtr pGC,
    int nRectsInit,
    xRectangle *pRectsInit
#endif
);
/* mach64frect.c */
void mach64PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    register GCPtr pGC,
    int nrectFill,
    xRectangle *prectInit
#endif
);
/* mach64text.c */
int mach64PolyText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *
#endif
);
int mach64PolyText16(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned short *
#endif
);
void mach64ImageText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *
#endif
);
void mach64ImageText16(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned short *
#endif
);
/* mach64font.c */
Bool mach64RealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    FontPtr font
#endif
);
Bool mach64UnrealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    FontPtr font
#endif
);
/* mach64fcache.c */
void mach64FontCache8Init(
#if NeedFunctionPrototypes
    int x,
    int y
#endif
);
int mach64CacheFont(
#if NeedFunctionPrototypes
    FontPtr font
#endif
);
void mach64UnCacheFont(
#if NeedFunctionPrototypes
    FontPtr font
#endif
);
int mach64DrawText(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned char *,
    int,
    int
#endif
);
/* mach64pntwn.c */
void mach64PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr pWin,
    RegionPtr pRegion,
    int what
#endif
);
/* ativga.c */
void mach64SaveVGAInfo(
#if NeedFunctionPrototypes
    int screen_idx
#endif
);
void mach64RestoreVGAInfo(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach64curs.c */
Bool mach64CursorInit(
#if NeedFunctionPrototypes
    char *pm,
    ScreenPtr pScr
#endif
);
Bool mach64RealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs
#endif
);
Bool mach64UnrealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs
#endif
);
void mach64SetCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs,
    int x,
    int y
#endif
);
void mach64RepositionCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr
#endif
);
void mach64RestoreCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr
#endif
);
void mach64MoveCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    int x,
    int y
#endif
);
void mach64RenewCursorColor(
#if NeedFunctionPrototypes
    ScreenPtr pScr
#endif
);
void mach64RecolorCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs,
    Bool displayed
#endif
);
void mach64WarpCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    int x,
    int y
#endif
);
void mach64QueryBestSize(
#if NeedFunctionPrototypes
    int class,
    unsigned short *pwidth,
    unsigned short *pheight,
    ScreenPtr pScr
#endif
);
void mach64CursorOff(
#if NeedFunctionPrototypes
    void
#endif
);
void mach64ClearSavedCursor(
#if NeedFunctionPrototypes
    int scr_index
#endif
);
extern void mach64BlockHandler(
#if NeedFunctionPrototypes
    int i,
    pointer blockData,
    struct timeval ** pTimeout,
    pointer pReadmask
#endif
);
/* mach64gtimg.c */
void mach64GetImage(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    int sx,
    int sy,
    int w,
    int h,
    unsigned int format,
    unsigned long planeMask,
    char * pdstLine
#endif
);
/* mach64dline.c  --  NOT IMPLEMENTED */
void mach64Dline(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int mode,
    int npt,
    DDXPointPtr pptInit
#endif
);
/* mach64dseg.c   --  NOT IMPLEMENTED */
void mach64Dsegment(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nseg,
    register xSegment *pSeg
#endif
);
#endif
