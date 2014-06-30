/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agx.h,v 3.19 1996/12/23 06:32:25 dawes Exp $ */
/*
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry A. Worth not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Henry A. Worth makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * HENRY A. WORTH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS, IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY 
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER 
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, 
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF 
 * THIS SOFTWARE.
 *
 */
/* $XConsortium: agx.h /main/10 1996/10/19 17:49:46 kaleb $ */

#ifndef AGX_H
#define AGX_H

#define AGX_PATCHLEVEL "0"

#define AGX_CURSBYTES		1024
#define AGX_CURSMAX		64

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
#include "regagx.h"

_XFUNCPROTOBEGIN

extern void (*agxImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
extern void (*agxImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
extern void (*agxImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, 
    short, unsigned long
#endif
);
extern void agxMemToVid(
#if NeedFunctionPrototypes
   int  dst,
   int  dstWidth,
   char *src,
   int  srcWidth,
   int  h
#endif
);
extern void agxPartMemToVid(
#if NeedFunctionPrototypes
   int  dst,
   int  dstWidth,
   char *src,
   int  srcWidth,
   int  w,
   int  h
#endif
);
extern void agxImageTileScrPad(
#if NeedFunctionPrototypes
    int                 x,
    int                 y,
    int                 w,
    int                 h,
    int                 pwidth,
    int                 pw,
    int                 ph,
    int                 xrot,
    int                 yrot
#endif
);
extern void agxImageStipScrPad(
#if NeedFunctionPrototypes
    int                 x,
    int                 y,
    int                 w,
    int                 h,
    int                 pwidth,
    int                 pw,
    int                 ph,
    int                 xrot,
    int                 yrot
#endif
);

extern ScreenPtr savepScreen;

/* agx.c */
Bool agxProbe(
#if NeedFunctionPrototypes
    void
#endif
);
void agxPrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
int agxGetMemSize(
#if NeedFunctionPrototypes
    void
#endif
);
Bool agxInit(
#if NeedFunctionPrototypes
    int ,
    ScreenPtr ,
    int ,
    char **
#endif
);
void agxEnterLeaveVT(
#if NeedFunctionPrototypes
    Bool ,
    int 
#endif
);
Bool agxCloseScreen(
#if NeedFunctionPrototypes
    int screen_idx,
    ScreenPtr pScreen
#endif
);
Bool agxSaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr ,
    Bool 
#endif
);
void agxAdjustFrame(
#if NeedFunctionPrototypes
    int ,
    int 
#endif
);
/* agxCmap.c */
int agxListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr ,
    Colormap *
#endif
);
void agxStoreColors(
#if NeedFunctionPrototypes
    ColormapPtr ,
    int ,
    xColorItem *
#endif
);
void agxInstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void agxUninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void agxRestoreColor0(
#if NeedFunctionPrototypes
    ScreenPtr pScreen
#endif
);
void agxClearColor0(
#if NeedFunctionPrototypes
   void
#endif
);

/* agxCursor.c */
Bool agxCursorInit(
#if NeedFunctionPrototypes
    char *,
    ScreenPtr 
#endif
);
void agxRepositionCursor(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
void agxRestoreCursor(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
void agxRenewCursorColor(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
void agxWarpCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    int ,
    int 
#endif
);
void agxQueryBestSize(
#if NeedFunctionPrototypes
    int ,
    unsigned short *,
    unsigned short *,
    ScreenPtr 
#endif
);
void agxCursorOff(
#if NeedFunctionPrototypes
    void
#endif
);
void agxClearSavedCursor(
#if NeedFunctionPrototypes
    int 
#endif
);
/* agxInit.c */
void vgarestore(
#if NeedFunctionPrototypes
    agxSaveBlock *
#endif
);
agxSaveBlock *agxHWSave(
#if NeedFunctionPrototypes
    agxSaveBlock *,
    int 
#endif
);
void agxHWRestore(
#if NeedFunctionPrototypes
    agxSaveBlock *
#endif
);
void agxInitDisplay(
#if NeedFunctionPrototypes
    int ,
    agxCRTCRegPtr 
#endif
);
void agxCalcCRTCRegs(
#if NeedFunctionPrototypes
    agxCRTCRegPtr ,
    DisplayModePtr 
#endif
);
void agxInitGE(
#if NeedFunctionPrototypes
    void
#endif
);
void agxSetCRTCRegs(
#if NeedFunctionPrototypes
    agxCRTCRegPtr 
#endif
);
Bool agxSwitchMode(
#if NeedFunctionPrototypes
    DisplayModePtr 
#endif
);
Bool agxGetClocks(
#if NeedFunctionPrototypes
    int ,
    int ,
    int ,
    int ,
    ScrnInfoRec *
#endif
);
void agxResetCRTC(
#if NeedFunctionPrototypes
    int 
#endif
);
Bool agxClockSelect(
#if NeedFunctionPrototypes
    int ,
    int 
#endif
);
Bool xgaNiClockSelect(
#if NeedFunctionPrototypes
    int ,
    int 
#endif
);
Bool agxValidateClocks(
#if NeedFunctionPrototypes
    int 
#endif
);
Bool agxSetUpProbeCRTC(
#if NeedFunctionPrototypes
    agxCRTCRegPtr 
#endif
);
Bool agxProbeClocks(
#if NeedFunctionPrototypes
    int 
#endif
);
void agxSaveLUT(
#if NeedFunctionPrototypes
    LUTENTRY *
#endif
);
void agxRestoreLUT(
#if NeedFunctionPrototypes
    LUTENTRY *
#endif
);
void agxInitLUT(
#if NeedFunctionPrototypes
    void
#endif
);
void agxCleanUp(
#if NeedFunctionPrototypes
    void
#endif
);
void agxImageClear(
#if NeedFunctionPrototypes
    unsigned int
#endif
);

/* agxIm.c */
void agxSetVidPage(
#if NeedFunctionPrototypes
    int 
#endif
);

void agxImageInit(
#if NeedFunctionPrototypes
    void
#endif
);
int agxBytePadScratchMapPow2(
#if NeedFunctionPrototypes
    int ,
    int 
#endif
);
void agxImageStipple(
#if NeedFunctionPrototypes
    int ,
    int ,
    int ,
    int ,
    char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    Pixel ,
    Pixel ,
    short ,
    short ,
    unsigned long 
#endif
);
/* agxBStor.c */
void agxSaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr
#endif
);
void agxRestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr
#endif
);
/* agxScrIn.c */
Bool agxScreenInit(
#if NeedFunctionPrototypes
    register ScreenPtr ,
    pointer ,
    int ,
    int ,
    int ,
    int ,
    int 
#endif
);
/* agxGC.c */
Bool agxCreateGC(
#if NeedFunctionPrototypes
    register GCPtr 
#endif
);
/* agxGC.c */
void agxChangeGC(
#if NeedFunctionPrototypes
    GC * ,
    BITS32
#endif
);
/* agxGC.c */
void agxDestroyGC(
#if NeedFunctionPrototypes
    GC *
#endif
);
/* agxGC.c */
GCOps * 
agxCreateOps(
#if NeedFunctionPrototypes
    GCOps *
#endif
);
/* agxGC.c */
void agxDestroyOps(
#if NeedFunctionPrototypes
    GCOps *
#endif
);
/* agxGC.c */
void agxDestroyClip(
#if NeedFunctionPrototypes
    GCPtr
#endif
);
/* agxGC.c */
void agxChangeClip(
#if NeedFunctionPrototypes
    GCPtr,
    int ,
    pointer ,
    int
#endif
);
/* agxGC.c */
void agxCopyClip(
#if NeedFunctionPrototypes
    GCPtr,
    GCPtr
#endif
);
/* agxGC.c */
void agxCopyGC(
#if NeedFunctionPrototypes
    GC * ,
    Mask ,
    GC *
#endif
);
/* agxGC16.c */
Bool agxCreateGC16(
#if NeedFunctionPrototypes
    register GCPtr
#endif
);
/* agxGC32.c */
Bool agxCreateGC32(
#if NeedFunctionPrototypes
    register GCPtr
#endif
);
/* agxBlt.c */
RegionPtr agxCopyArea(
#if NeedFunctionPrototypes
    register DrawablePtr ,
    register DrawablePtr ,
    GC *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int 
#endif
);
void agxFindOrdering(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    GC *,
    int ,
    BoxPtr ,
    int ,
    int ,
    int ,
    int ,
    unsigned int *
#endif
);
RegionPtr agxCopyPlane(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
/* agxLine.c */
void agxLine(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    DDXPointPtr 
#endif
);
/* agxSeg.c */
void agxSegment(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register xSegment *
#endif
);
/* agxDLine.c */
void agxDLine(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    DDXPointPtr
#endif
);
/* agxDSeg.c */
void agxDSegment(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register xSegment *
#endif
);
/* agxMap.c */
void agxMapSetSrc(
#if NeedFunctionPrototypes
    int 
#endif
);
void agxMapSetDst(
#if NeedFunctionPrototypes
    int 
#endif
);
void agxMapSetSrcDst(
#if NeedFunctionPrototypes
    int 
#endif
);
/* agxBCach.c */
void agxBitCache8Init(
#if NeedFunctionPrototypes
    int x,
    int y
#endif
);
/* agxFCach.c */
void agxUnCacheFont8(
#if NeedFunctionPrototypes
    FontPtr 
#endif
);
/* agxFont.c */
Bool agxRealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr ,
    FontPtr 
#endif
);
Bool agxUnrealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr ,
    FontPtr 
#endif
);
/* agxText.c */
int agxPolyText8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    char *
#endif
);
void agxImageText8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    char *
#endif
);
int agxPolyText16(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    unsigned short *
#endif
);
void agxImageText16(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    unsigned short *
#endif
);
/* agxFRect.c */
void agxPolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr ,
    register GCPtr ,
    int ,
    xRectangle *
#endif
);
/* agxFS.c */
void agxSolidFSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
void agxTiledFSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
void agxStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
void agxOStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr,
    int *,
    int 
#endif
);
/* agxIm.c */
void agxFSpansTile(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    DDXPointPtr,
    int *,
    PixmapPtr,
    int,
    int,
    unsigned int,
    unsigned int
#endif
);
void agxFSpansStipple(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    DDXPointPtr,
    int *,
    PixmapPtr,
    int,
    int,
    Pixel,
    Pixel,
    short,
    short,
    unsigned long
#endif
);
/* agxGtImg.c */
void agxGetImage(
#if NeedFunctionPrototypes
    DrawablePtr,
    int,
    int,
    int,
    int,
    unsigned int,
    unsigned long,
    char * 
#endif
);
/* agxGS.c */
void agxGetSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    register DDXPointPtr ,
    int *,
    int ,
    char *
#endif
);
/* agxSS.c */
void agxSetSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    char *,
    register DDXPointPtr ,
    int *,
    int ,
    int 
#endif
);
/* vgapntwin.c */
void agxPaintWindow(
#if NeedFunctionPrototypes
    WindowPtr ,
    RegionPtr ,
    int
#endif
);
/* agxIm.c */
void agxFillBoxTile(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    PixmapPtr,
    int,
    int,
    short,
    unsigned long
#endif
);
void agxFillBoxStipple(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    PixmapPtr,
    int,
    int,
    Pixel,
    Pixel,
    short,
    short,
    unsigned long
#endif
);
/* agxWin.c */
void agxCopyWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    DDXPointRec,
    RegionPtr
#endif
);
/* agxFillArc.c */
void
agxPolyFillArc(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xArc *
#endif
);
/* agxFillPoly.c */
void
agxFillPolygon(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    DDXPointPtr
#endif
);
void
agxFillConvexPoly(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr
#endif
);
void
agxFillGeneralPoly(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr
#endif
);
/* xf86RamDac.c */
void xf86ProbeRamDac(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86SetUpRamDac(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86RamDacHWNoop(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86RamDacNoopInit(
#if NeedFunctionPrototypes
    void
#endif
);
/* xf86Bt481.c */
void xf86Bt481HWSave(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86Bt481HWRestore(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86Bt481Init(
#if NeedFunctionPrototypes
    void
#endif
);
Bool xf86Bt482RealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    CursorPtr 
#endif
);
void xf86Bt482CursorOn(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86Bt482CursorOff(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86Bt482MoveCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    int ,
    int 
#endif
);
void xf86Bt482RecolorCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    CursorPtr 
#endif
);
void xf86Bt482LoadCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    CursorPtr ,
    int ,
    int ,
    int 
#endif
);
/* xf86Bt485.c */
unsigned char xf86InBt485StatReg(
#if NeedFunctionPrototypes
    void
#endif
);
Bool xf86Bt485RealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    CursorPtr 
#endif
);
void xf86Bt485CursorOn(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86Bt485CursorOff(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86Bt485MoveCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    int ,
    int 
#endif
);
void xf86Bt485RecolorCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    CursorPtr 
#endif
);
void xf86Bt485LoadCursor(
#if NeedFunctionPrototypes
    ScreenPtr ,
    CursorPtr ,
    int ,
    int ,
    int 
#endif
);
void xf86Bt485HWSave(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86Bt485HWRestore(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86Bt485Init(
#if NeedFunctionPrototypes
    void
#endif
);
/* xf861502x.c */
void xf86OutSc1502xCmd(
#if NeedFunctionPrototypes
    int ,
    int 
#endif
);
unsigned char xf86InSc1502xCmd(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86OutSc1502xIndReg(
#if NeedFunctionPrototypes
    int ,
    int ,
    int 
#endif
);
unsigned char xf86InSc1502xIndReg(
#if NeedFunctionPrototypes
    int 
#endif
);
void xf86Sc1502xHWSave(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86Sc1502xHWRestore(
#if NeedFunctionPrototypes
    union xf86RamDacSave *
#endif
);
void xf86Sc1502xInit(
#if NeedFunctionPrototypes
    void
#endif
);
void xf86Sc1502xPrintId(
#if NeedFunctionPrototypes
    void
#endif
);

_XFUNCPROTOEND

extern pointer agxVideoMem;
extern pointer agxPhysVidMem;
extern unsigned long agxMemBase;
extern unsigned long agxPOSMemBase;
extern unsigned long agxFontCacheOffset;
extern unsigned long agxFontCacheSize;
extern unsigned long agxFontRowLength;
extern unsigned int  agxFontRowNumLines;
extern unsigned long agxScratchOffset;
extern unsigned long agxScratchSize;
extern unsigned long agxHWCursorOffset;
extern Bool     agxHWCursor;

extern pointer vgaBase;
extern pointer vgaVirtBase;
extern pointer vgaPhysBase;
extern int     vgaIOBASE;
extern int     vgaCRIndex;
extern int     vgaCRReg;
extern Bool    vgaUse2Banks;
extern Bool    useSpeedUp;

extern Bool xf86VTSema;
extern int   agxMaxX, agxMaxY;
extern int   agxVirtX, agxVirtY;
extern int   agxDisplayWidth; 
extern int   agxAdjustedVirtX;
extern Bool  agx128WidthAdjust;
extern Bool  agx256WidthAdjust;
extern Bool  agx288WidthAdjust;
extern Bool agxUse4MbAperture;
extern Bool (*agxClockSelectFunc)();


extern Bool agxSaveVGA;
extern Bool agxInited;

extern pointer      agxGEBase;
extern pointer      agxGEPhysBase;
extern unsigned int agxIdxReg;
extern unsigned int agxDAReg;
extern unsigned int agxApIdxReg;
extern unsigned int agxByteData;
extern unsigned int agxChipId;
extern unsigned int xf86RamDacBase;
extern unsigned long vgaBankSize;
extern unsigned long agxBankSize;

extern int agxBpp;
extern int agxWeight;
extern int defaultColorVisualClass;
extern unsigned char agxVideoMapFormat;
extern unsigned long agxPixMask;
extern int BytesPerPixelShift;

extern agxCRTCRegRec agxCRTCRegs;
extern agxSaveBlock  *agxSavedState;

extern ScrnInfoRec agxInfoRec;
extern int agxValidTokens[];

extern short agxalu[];

#endif
