/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/vga16.h,v 3.9 1996/12/28 08:15:49 dawes Exp $ */





/* $XConsortium: vga16.h /main/5 1996/02/21 17:58:47 kaleb $ */
/*
 * some header files need to be included here to make
 * the prototypes happy.
 */
#include <windowstr.h>
#include <gcstruct.h>
#include <colormapst.h>
#include <fontstruct.h>
#ifndef MFB_OPEQ_WHITE
#include <mfb.h>	/* GJA -- for PixelType */
#endif

/* ppcArea.c */
void ppcAreaFill(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    BoxPtr,
    GCPtr 
#endif
);
/* ppcBitmap.c */
void ppcQuickBlt(
#if NeedFunctionPrototypes
    int *,
    int *,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int 
#endif
);
void ppcRotBitmapRight(
#if NeedFunctionPrototypes
    PixmapPtr,
    int 
#endif
);
void ppcRotBitmapDown(
#if NeedFunctionPrototypes
    PixmapPtr,
    int 
#endif
);
/* ppcBStore.c */
void ppcSaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
void ppcRestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
/* ppcClip.c */
void ppcDestroyClip(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
void ppcChangeClip(
#if NeedFunctionPrototypes
    GCPtr,
    int,
    pointer,
    int 
#endif
);
void ppcCopyClip(
#if NeedFunctionPrototypes
    GCPtr,
    GCPtr 
#endif
);
/* ppcCpArea.c */
RegionPtr vga16CopyArea(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GC *,
    int,
    int,
    int,
    int,
    int,
    int 
#endif
);
/* ppcCReduce.c */
void ppcReduceGeneral(
#if NeedFunctionPrototypes
    int,
    unsigned long,
    unsigned long,
    unsigned long,
    int,
    int,
    ppcReducedRrop *
#endif
);
void ppcReduceColorRrop(
#if NeedFunctionPrototypes
    ppcReducedRrop *,
    int,
    ppcReducedRrop *
#endif
);
void ppcGetReducedColorRrop(
#if NeedFunctionPrototypes
    GC *,
    int,
    ppcReducedRrop *
#endif
);
/* ppcCurs.c */
Bool ppcRealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    CursorPtr 
#endif
);
Bool ppcUnrealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    CursorPtr 
#endif
);
/* ppcDepth.c */
Bool ppcDepthOK(
#if NeedFunctionPrototypes
    DrawablePtr,
    int 
#endif
);
/* ppcFillRct.c */
void ppcPolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xRectangle *
#endif
);
/* ppcWindowFS.c */
void ppcSolidWindowFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ppcStippleWindowFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ppcOpStippleWindowFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ppcTileWindowFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
/* ppcPixmapFS.c */
int mod(
#if NeedFunctionPrototypes
    int,
    int 
#endif
);
void ppcSolidPixmapFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ppcStipplePixmapFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ppcOpStipplePixmapFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ppcTilePixmapFS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
/* ppcGC.c */
Bool ppcCreateGC(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
void ppcDestroyGC(
#if NeedFunctionPrototypes
    GC *
#endif
);
Mask ppcChangePixmapGC(
#if NeedFunctionPrototypes
    GC *,
    Mask 
#endif
);
void ppcValidateGC(
#if NeedFunctionPrototypes
    GCPtr,
    Mask,
    DrawablePtr 
#endif
);
/* ppcGetSp.c */
void ppcGetSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    int,
    DDXPointPtr,
    int *,
    int,
    char *
#endif
);
/* ppcImg.c */
void ppcGetImage(
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
/* ppcLine.c */
void ppcScrnZeroLine(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
void ppcScrnZeroDash(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
void ppcScrnZeroSegs(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* ppcPixmap.c */
PixmapPtr ppcCreatePixmap(
#if NeedFunctionPrototypes
    ScreenPtr,
    int,
    int,
    int 
#endif
);
PixmapPtr ppcCopyPixmap(
#if NeedFunctionPrototypes
    PixmapPtr 
#endif
);
void ppcPadPixmap(
#if NeedFunctionPrototypes
    PixmapPtr 
#endif
);
void ppcRotatePixmap(
#if NeedFunctionPrototypes
    PixmapPtr,
    int 
#endif
);
/* ppcPntWin.c */
void ppcPaintWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    RegionPtr,
    int 
#endif
);
void ppcPaintWindowSolid(
#if NeedFunctionPrototypes
    WindowPtr,
    RegionPtr,
    int 
#endif
);
void ppcPaintWindowTile(
#if NeedFunctionPrototypes
    WindowPtr,
    RegionPtr,
    int 
#endif
);
/* ppcPolyPnt.c */
void ppcPolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    xPoint *
#endif
);
/* ppcPolyRec.c */
void ppcPolyRectangle(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xRectangle *
#endif
);
/* ppcPushPxl.c */
void ppcPushPixels(
#if NeedFunctionPrototypes
    GCPtr,
    PixmapPtr,
    DrawablePtr,
    int,
    int,
    int,
    int 
#endif
);
/* ppcQuery.c */
void ppcQueryBestSize(
#if NeedFunctionPrototypes
    int,
    unsigned short *,
    unsigned short *,
    ScreenPtr
#endif
);
/* ppcRot.c */
void ppcRotZ8mapUp(
#if NeedFunctionPrototypes
    PixmapPtr,
    PixmapPtr,
    int 
#endif
);
void ppcRotZ8mapLeft(
#if NeedFunctionPrototypes
    PixmapPtr,
    PixmapPtr,
    int 
#endif
);
void ppcClipZ8Pixmap(
#if NeedFunctionPrototypes
    PixmapPtr,
    PixmapPtr 
#endif
);
PixmapPtr ppcClipBitmap(
#if NeedFunctionPrototypes
    PixmapPtr,
    int,
    int 
#endif
);
void ppcRotBitmapUp(
#if NeedFunctionPrototypes
    PixmapPtr,
    PixmapPtr,
    int 
#endif
);
void ppcRotBitmapLeft(
#if NeedFunctionPrototypes
    PixmapPtr,
    PixmapPtr,
    int 
#endif
);
void ppcClipZ1Pixmap(
#if NeedFunctionPrototypes
    PixmapPtr,
    PixmapPtr 
#endif
);
void ppcEndWorldHunger(
#if NeedFunctionPrototypes
    void
#endif
);
void ppcRefinanceNationalDebt(
#if NeedFunctionPrototypes
    void
#endif
);
void ppcEndWarForever(
#if NeedFunctionPrototypes
    void
#endif
);
void ppcPayServerHackersWhatTheyreWorth(
#if NeedFunctionPrototypes
    void
#endif
);
/* ppcRslvC.c */
void vga16ResolveColor(
#if NeedFunctionPrototypes
    unsigned short *,
    unsigned short *,
    unsigned short *,
    VisualPtr 
#endif
);
Bool vga16InitializeColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
Bool vga16CreateDefColormap(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
/* ppcSetSp.c */
void ppcSetSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    char *,
    DDXPointPtr,
    int *,
    int,
    int 
#endif
);
/* ppcWindow.c */
void ppcCopyWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    DDXPointRec,
    RegionPtr 
#endif
);
void ppcCopyWindowForXYhardware(
#if NeedFunctionPrototypes
    WindowPtr,
    DDXPointRec,
    RegionPtr 
#endif
);
Bool ppcPositionWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int 
#endif
);
Bool ppcUnrealizeWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int 
#endif
);
Bool ppcRealizeWindow(
#if NeedFunctionPrototypes
    WindowPtr 
#endif
);
Bool ppcDestroyWindow(
#if NeedFunctionPrototypes
    WindowPtr 
#endif
);
Bool ppcCreateWindow(
#if NeedFunctionPrototypes
    WindowPtr 
#endif
);
Bool ppcCreateWindowForXYhardware(
#if NeedFunctionPrototypes
    WindowPtr 
#endif
);
/* emulOpStip.c */
void ppcOpaqueStipple(
#if NeedFunctionPrototypes
    WindowPtr,
    PixmapPtr,
    unsigned long int,
    unsigned long int,
    int,
    unsigned long int,
    int,
    int,
    int,
    int,
    int,
    int 
#endif
);
/* emulRepAre.c */
void ppcReplicateArea(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    ScreenPtr 
#endif
);
/* emulTile.c */
void ppcTileRect(
#if NeedFunctionPrototypes
    WindowPtr,
    PixmapPtr,
    const int,
    const unsigned long int,
    int,
    int,
    int,
    int,
    int,
    int 
#endif
);
/* vgaGC.c */
Mask vgaChangeGCtype(
#if NeedFunctionPrototypes
    GC *,
    ppcPrivGCPtr 
#endif
);
Mask vgaChangeWindowGC(
#if NeedFunctionPrototypes
    GC *,
    Mask 
#endif
);
/* vgaBitBlt.c */
void vgaBitBlt(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int 
#endif
);
/* vgaImages.c */
void vgaDrawColorImage(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int,
    int,
    int,
    unsigned char *,
    int,
    const int,
    const unsigned long int 
#endif
);
void vgaReadColorImage(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int,
    int,
    int,
    unsigned char *,
    int 
#endif
);
/* vgaLine.c */
void vgaHorzLine(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned long int,
    int,
    unsigned long int,
    int,
    int,
    int 
#endif
);
void vgaVertLine(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned long int,
    int,
    unsigned long int,
    int,
    int,
    int 
#endif
);
void vgaBresLine(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned long int,
    int,
    unsigned long int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    unsigned long int 
#endif
);
/* vgaStipple.c */
void vgaDrawMonoImage(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned char *,
    int,
    int,
    int,
    int,
    unsigned long int,
    int,
    unsigned long int 
#endif
);
void vgaFillStipple(
#if NeedFunctionPrototypes
    WindowPtr,
    const PixmapPtr,
    unsigned long int,
    const int,
    unsigned long int,
    int,
    int,
    int,
    int,
    const int,
    const int 
#endif
);
/* vgaSolid.c */
void vgaFillSolid(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned long int,
    const int,
    unsigned long int,
    int,
    const int,
    int,
    const int 
#endif
);
/* offscreen.c */
int vgaSaveScreenPix(
#if NeedFunctionPrototypes
    ScreenPtr,
    PixmapPtr 
#endif
);
int vgaRestoreScreenPix(
#if NeedFunctionPrototypes
    ScreenPtr,
    PixmapPtr 
#endif
);
int do_rop(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    const unsigned long 
#endif
);
void offBitBlt(
#if NeedFunctionPrototypes
    WindowPtr,
    const int,
    const int,
    const int,
    int,
    int,
    int,
    int,
    int,
    int 
#endif
);
void offDrawColorImage(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int,
    int,
    int,
    unsigned char *,
    int,
    const int,
    const unsigned long int 
#endif
);
void offReadColorImage(
#if NeedFunctionPrototypes
    WindowPtr,
    int,
    int,
    int,
    int,
    unsigned char *,
    int 
#endif
);
void offBresLine(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned long int,
    int,
    unsigned long int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    int,
    unsigned long int 
#endif
);
void offFillSolid(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned long int,
    const int,
    unsigned long int,
    int,
    const int,
    int,
    const int 
#endif
);
void offDrawMonoImage(
#if NeedFunctionPrototypes
    WindowPtr,
    unsigned char *,
    int,
    int,
    int,
    int,
    unsigned long int,
    int,
    unsigned long int 
#endif
);
void offFillStipple(
#if NeedFunctionPrototypes
    WindowPtr,
    const PixmapPtr,
    unsigned long int,
    const int,
    unsigned long int,
    int,
    int,
    int,
    int,
    const int,
    const int 
#endif
);
/* mfbimggblt.c */
void v16ImageGlyphBlt(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    int,
    unsigned int,
    CharInfoPtr *,
    unsigned char *
#endif
);
void doImageGlyphBlt(
#if NeedFunctionPrototypes
    DrawablePtr,
    GC *,
    int,
    int,
    unsigned int,
    CharInfoPtr *,
    unsigned char *,
    ExtentInfoRec *
#endif
);
/* wm3.c */
int wm3_set_regs(
#if NeedFunctionPrototypes
    GC *
#endif
);
/* ppcIO.c */
int NeverCalled(
#if NeedFunctionPrototypes
    void
#endif
);
Bool vgaScreenClose(
#if NeedFunctionPrototypes
    int,
    ScreenPtr 
#endif
);
void Init16Output(
#if NeedFunctionPrototypes
    ScreenPtr,
    pointer,
    int,
    int,
    int,
    int,
    int
#endif
);
/* mfbfillarc.c */
void v16PolyFillArcSolid(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xArc *
#endif
);
void v16PolyFillArc(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xArc *
#endif
);

/* mfbzerarc.c */
void v16ZeroPolyArcSS(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xArc *
#endif
);
void v16ZeroPolyArc(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xArc *
#endif
);

extern Bool xf86VTSema;

/* mfbline.c */
/* internal */ void DoV16SegmentSS (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    register xSegment *
#endif
);
/* internal */ void DoV16LineSS (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int	,
    int	,
    DDXPointPtr
#endif
);
/* internal */ void DoV16SegmentSD (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    register xSegment *
#endif
);
/* internal */ void DoV16LineSD (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr
#endif
);
void v16SegmentSS (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    register xSegment *
#endif
);
void v16LineSS (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int	,
    int	,
    DDXPointPtr
#endif
);
void v16SegmentSD (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    register xSegment *
#endif
);
void v16LineSD (
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr
#endif
);

/* mfbbres.c */
void v16BresS(
#if NeedFunctionPrototypes
	PixelType *,
	int,
	int,
	int,
	int,
	int,
	int,
	register int,
	register int,
	int,
	int
#endif
);

/* mfbbresd.c */
void v16BresD(
#if NeedFunctionPrototypes
	int, int,
	int *,
	unsigned char *,
	int,
	int *,
	int,
	PixelType *,
	int, int, int, int, int, int,
	int, int,
	int, int
#endif
);

/* mfbhrzvert.c */
int v16HorzS(
#if NeedFunctionPrototypes
	register PixelType *,
	register int,
	int,
	int,
	int
#endif
);
int v16VertS(
#if NeedFunctionPrototypes
	register PixelType *,
	register int,
	int,
	int,
	register int
#endif
);
