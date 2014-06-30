/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32.h,v 3.15 1996/12/23 06:38:21 dawes Exp $ */
/*
 * Copyright 1992,1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND RICKARD E. FAITH DISCLAIM ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: mach32.h /main/8 1996/10/19 17:52:47 kaleb $ */


#ifndef MACH32_H
#define MACH32_H

#define MACH32_PATCHLEVEL "0"

#define MACH32_CURSBYTES	1024
#define MACH32_CURSMAX		64

#define NEW_MEM_DETECT

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
#include "xf86_OSproc.h"
#include "xf86bcache.h"
#include "xf86fcache.h"
#include "xf86Procs.h"

#include "regionstr.h"
#include "regmach32.h"

extern unsigned char *mach32VideoMem;
extern pointer vgaBase;
extern Bool xf86VTSema;
extern short mach32MaxX, mach32MaxY;
extern short mach32VirtX, mach32VirtY;
extern short mach32DisplayWidth;
extern Bool mach32Use4MbAperture;
extern Bool mach32DAC8Bit;

extern int mach32Ramdac;
extern int mach32RamdacSubType;
extern int mach32BusType;

/* == 0 --> no boundary crossing on this line */
/*  > 0 --> x value of first pixel in video aperture */
extern int mach32VideoPageBoundary[MACH32_MAX_Y+1];

extern ScrnInfoRec mach32InfoRec;
extern int mach32ValidTokens[];

extern short mach32alu[];

extern Bool checkCursorColor;

extern void (*mach32ImageWriteFunc)(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    int,
    char *,
    int,
    int,
    int,
    short,
    unsigned long
#endif
);
extern void (*mach32ImageReadFunc)(
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
extern void (*mach32ImageFillFunc)(
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
    int,
    short,
    unsigned long
#endif
);

/* Function Prototypes */

/* mach32.c */
Bool mach32Probe(
#if NeedFunctionPrototypes
    void
#endif
);
void mach32PrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
Bool mach32Initialize(
#if NeedFunctionPrototypes
    int scr_index,
    ScreenPtr pScreen,
    int argc,
    char **argv
#endif
);
void mach32EnterLeaveVT(
#if NeedFunctionPrototypes
    Bool enter,
    int screen_idx
#endif
);
Bool mach32CloseScreen(
#if NeedFunctionPrototypes
    int screen_idx,
    ScreenPtr pScreen
#endif
);
Bool mach32SaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    Bool on
#endif
);
void mach32AdjustFrame(
#if NeedFunctionPrototypes
    int x,
    int y
#endif
);
Bool mach32SwitchMode(
#if NeedFunctionPrototypes
    DisplayModePtr mode
#endif
);
Bool mach32ClockSelect(
#if NeedFunctionPrototypes
    int no
#endif
);
/* mach32cmap.c */
int mach32ListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    Colormap *pmaps
#endif
);
int mach32GetInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    ColormapPtr *pmap
#endif
);
void mach32StoreColors(
#if NeedFunctionPrototypes
    ColormapPtr pmap,
    int ndef,
    xColorItem *pdefs
#endif
);
void mach32InstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr pmap
#endif
);
void mach32UninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr pmap
#endif
);
void mach32RestoreColor0(
#if NeedFunctionPrototypes
    ScreenPtr pScreen
#endif
);
/* mach32gc.c */
void mach32InitGC(
#if NeedFunctionPrototypes
    void
#endif
);
Bool mach32CreateGC(
#if NeedFunctionPrototypes
    register GCPtr pGC
#endif
);
/* mach32fs.c */
void mach32SolidFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
void mach32TiledFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
void mach32StipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
void mach32OStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nInit,
    DDXPointPtr pptInit,
    int *pwidthInit,
    int fSorted
#endif
);
/* mach32ss.c */
void mach32SetSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    char *psrc,
    register DDXPointPtr ppt,
    int *pwidth,
    int nspans,
    int fSorted
#endif
);
/* mach32gs.c */
void mach32GetSpans(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    int wMax,
    register DDXPointPtr ppt,
    int *pwidth,
    int nspans,
    char *pdstStart
#endif
);
/* mach32win.c */
void mach32CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr pWin,
    DDXPointRec ptOldOrg,
    RegionPtr prgnSrc
#endif
);
/* mach32init.c */
void mach32CalcCRTCRegs(
#if NeedFunctionPrototypes
    mach32CRTCRegPtr crtcRegs,
    DisplayModePtr mode
#endif
);
void mach32SetCRTCRegs(
#if NeedFunctionPrototypes
    mach32CRTCRegPtr crtcRegs
#endif
);
void mach32SaveLUT(
#if NeedFunctionPrototypes
    LUTENTRY *lut
#endif
);
void mach32RestoreLUT(
#if NeedFunctionPrototypes
    LUTENTRY *lut
#endif
);
void mach32InitLUT(
#if NeedFunctionPrototypes
    void
#endif
);
void mach32InitEnvironment(
#if NeedFunctionPrototypes
    void
#endif
);
void mach32InitAperture(
#if NeedFunctionPrototypes
    int screen_idx
#endif
);
void mach32SetRamdac(
#if NeedFunctionPrototypes
    unsigned short *clock
#endif
);
void mach32InitDisplay(
#if NeedFunctionPrototypes
    int screen_idx
#endif
);
void mach32CleanUp(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach32im.c */
void mach32SetVGAPage(
#if NeedFunctionPrototypes
    int page
#endif
);
void mach32ImageInit(
#if NeedFunctionPrototypes
    void
#endif
);
void mach32ImageStipple(
#if NeedFunctionPrototypes
    int x,
    int y,
    int w,
    int h,
    char *psrc,
    int pwidth,
    int pw,
    int ph,
    int pox,
    int poy,
    Pixel fgPixel,
    Pixel bgPixel,
    short alu,
    unsigned long planemask,
    int   opaque
#endif
);
void mach32ImageOpStipple(
#if NeedFunctionPrototypes
    int x,
    int y,
    int w,
    int h,
    char *psrc,
    int pwidth,
    int pw,
    int ph,
    int pox,
    int poy,
    Pixel fgPixel,
    Pixel bgPixel,
    short alu,
    unsigned long planemask
#endif
);
void mach32FontOpStipple(
#if NeedFunctionPrototypes
    int x,
    int y,
    int w,
    int h,
    unsigned char *psrc,
    int pwidth,
    Pixel id
#endif
);
/* mach32bstor.c */
void mach32SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr pPixmap,
    RegionPtr prgnSave,
    int xorg,
    int yorg,
    WindowPtr pWin
#endif
);
void mach32RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr pPixmap,
    RegionPtr prgnRestore,
    int xorg,
    int yorg,
    WindowPtr pWin
#endif
);
/* mach32scrin.c */
Bool mach32ScreenInit(
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
/* mach32blt.c */
RegionPtr mach32CopyArea(
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
void mach32FindOrdering(
#if NeedFunctionPrototypes
    DrawablePtr pSrcDrawable,
    DrawablePtr pDstDrawable,
    GC *pGC,
    int numRects,
    BoxPtr boxes,
    int srcx,
    int srcy,
    int dstx,
    int dsty,
    unsigned int *ordering
#endif
);
RegionPtr mach32CopyPlane(
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
/* mach32pcach.c */
/* mach32plypt.c */
void mach32PolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int mode,
    int npt,
    xPoint *pptInit
#endif
);
/* mach32line.c */
void mach32Line(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int mode,
    int npt,
    DDXPointPtr pptInit
#endif
);
/* mach32line1r.c */
void mach32Line1Rect(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int mode,
    int npt,
    DDXPointPtr pptInit
#endif
);
/* mach32seg.c */
void mach32Segment(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nseg,
    register xSegment *pSeg
#endif
);
/* mach32frect.c */
void mach32PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    register GCPtr pGC,
    int nrectFill,
    xRectangle *prectInit
#endif
);
/* mach32text.c */
int mach32NoCPolyText(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *,
    Bool
#endif
);
int mach32NoCImageText(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *,
    Bool
#endif
);
/* mach32font.c */
Bool mach32RealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    FontPtr font
#endif
);
Bool mach32UnrealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr pScreen,
    FontPtr font
#endif
);
/* mach32fcach.c */
void mach32FontCache8Init(
#if NeedFunctionPrototypes
    void
#endif
);
void mach32GlyphWrite(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    unsigned char *,
    CacheFont8Ptr,
    GCPtr,
    BoxPtr,
    int
#endif
);
void mach32GlyphWrite(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    unsigned char *,
    CacheFont8Ptr,
    GCPtr,
    BoxPtr,
    int
#endif
);
/* mach32bc.c */

void mach32CacheMoveBlock(
#if NeedFunctionPrototypes
    int /*srcx*/,
    int /*srcy*/,
    int /*dstx*/,
    int /*dsty*/,
    int /*h*/,
    int /*len*/,
    unsigned int /*bitplane*/
#endif
);


/* mach32bcach.c */
void mach32CacheMoveBlock(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    int,
    int,
    int,
    unsigned int
#endif
);
/* mach32pntwn.c */
void mach32PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr pWin,
    RegionPtr pRegion,
    int what
#endif
);
void mach32FillBoxSolid(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    int nBox,
    BoxPtr pBox,
    unsigned long pixel
#endif
);
/* ativga.c */
void mach32SaveVGAInfo(
#if NeedFunctionPrototypes
    int screen_idx
#endif
);
void mach32RestoreVGAInfo(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach32curs.c */
Bool mach32CursorInit(
#if NeedFunctionPrototypes
    char *pm,
    ScreenPtr pScr
#endif
);
Bool mach32RealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs
#endif
);
Bool mach32UnrealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs
#endif
);
void mach32SetCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs,
    int x,
    int y
#endif
);
void mach32RepositionCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr
#endif
);
void mach32RestoreCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr
#endif
);
void mach32MoveCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    int x,
    int y
#endif
);
void mach32RenewCursorColor(
#if NeedFunctionPrototypes
    ScreenPtr pScr
#endif
);
void mach32RecolorCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    CursorPtr pCurs,
    Bool displayed
#endif
);
void mach32WarpCursor(
#if NeedFunctionPrototypes
    ScreenPtr pScr,
    int x,
    int y
#endif
);
void mach32QueryBestSize(
#if NeedFunctionPrototypes
    int class,
    unsigned short *pwidth,
    unsigned short *pheight,
    ScreenPtr pScr
#endif
);
void mach32CursorOff(
#if NeedFunctionPrototypes
    void
#endif
);
void mach32ClearSavedCursor(
#if NeedFunctionPrototypes
    int scr_index
#endif
);
extern void mach32BlockHandler(
#if NeedFunctionPrototypes
    int i,
    pointer blockData,
    struct timeval ** pTimeout,
    pointer pReadmask
#endif
);
/* mach32gtimg.c */
void mach32GetImage(
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
/* mach32mem.c */
int mach32GetMemSize(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach32dline.c */
void mach32Dline(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int mode,
    int npt,
    DDXPointPtr pptInit
#endif
);
/* mach32dseg.c */
void mach32Dsegment(
#if NeedFunctionPrototypes
    DrawablePtr pDrawable,
    GCPtr pGC,
    int nseg,
    register xSegment *pSeg
#endif
);
#endif
