/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vga256.h,v 3.6 1997/01/12 10:45:31 dawes Exp $ */





/* $XConsortium: vga256.h /main/5 1996/02/21 18:09:52 kaleb $ */

#ifndef _VGA256_H
#define VGA256_H

#include "X.h"
#include "Xmd.h"
#include "servermd.h"
#include "gcstruct.h"
#include "window.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "windowstr.h"
#include "region.h"
#include "mistruct.h"
#include "mibstore.h"
#include "fontstruct.h"
#include "dixfontstr.h"
#include "cfb.h"
#ifndef NO_CFBMSKBITS
#include "cfbmskbits.h"
#include "cfb8bit.h"
#endif
#include "vgaFasm.h"
#include "vgaBank.h"
#include "gcstruct.h"

extern GCOps vga256TEOps1Rect, vga256TEOps, vga256NonTEOps1Rect, vga256NonTEOps;

typedef struct _Cfbfunc{
    void (*vgaBitblt)(
#if NeedFunctionPrototypes
		unsigned char*,
		unsigned char*,
		int,
		int,
		int,
		int,
		int,
		int,
		int,
		int,
		int,
		int,
		int,
		unsigned long
#endif
);
    void (*doBitbltCopy)(
#if NeedFunctionPrototypes
		DrawablePtr,
		DrawablePtr,
		int,
		RegionPtr,
		DDXPointPtr,
		unsigned long,
		unsigned long
#endif
);
    void (*fillRectSolidCopy)(
#if NeedFunctionPrototypes
		DrawablePtr,
		GCPtr,
		int,
		BoxPtr
#endif
);
    void (*fillRectTransparentStippled32)();
    void (*fillRectOpaqueStippled32)();
    void (*segmentSS)(
#if NeedFunctionPrototypes
		DrawablePtr,
		GCPtr,
		int,
		xSegment *
#endif
);
    void (*lineSS)(
#if NeedFunctionPrototypes
		DrawablePtr,
		GCPtr,
		int,
		int,
		DDXPointPtr
#endif
);
    void (*fillBoxSolid)(
#if NeedFunctionPrototypes
		DrawablePtr,
		int,
		BoxPtr,
		unsigned long,
		unsigned long,
		int
#endif
);
    void (*teGlyphBlt8)(
#if NeedFunctionPrototypes
		DrawablePtr,
		GCPtr,
		int,
		int,
		unsigned int,
		CharInfoPtr *,
		pointer
#endif
);
    void (*copyPlane1to8)(
#if NeedFunctionPrototypes
		DrawablePtr,
		DrawablePtr,
		int,
		RegionPtr,
		DDXPointPtr,
		unsigned long,
		unsigned long
#endif
);
    void (*fillSolidSpans)(	/* Solid spans, any rop. */
#if NeedFunctionPrototypes	/* Must include clipping etc. */
		DrawablePtr,
		GCPtr,
		int,
		DDXPointPtr,
		int *,
		int
#endif
);
} CfbfuncRec, *CfbfuncPtr;

extern CfbfuncRec vga256LowlevFuncs;

/* BitBlt.s */

void WinWin(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int 
#endif
);

/* BitBlt2.s */

void PixWin(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int 
#endif
);

void WinPix(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int 
#endif
);

void PixPix(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int 
#endif
);

/* Box.s */

void SpeedUpBox(
#if NeedFunctionPrototypes
    unsigned char *,
    int ,
    int ,
    int ,
    int 
#endif
);

/* Line.s */

void SpeedUpBresS (
#if NeedFunctionPrototypes
    int,
    unsigned,
    unsigned,
    unsigned long *,
    int,
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

/* VHLine.s */

void SpeedUpHLine(
#if NeedFunctionPrototypes
    unsigned char *,
    int ,
    int ,
    int 
#endif
);

void SpeedUpVLine(
#if NeedFunctionPrototypes
    unsigned char *,
    int ,
    int ,
    int 
#endif
);

/* fBitBlt.s */

void fastBitBltCopy(
#if NeedFunctionPrototypes
    int ,
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int 
#endif
);

/* fFillAnd.s */

unsigned char *fastFillSolidGXand(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned long ,
    unsigned long ,
    int ,
    int ,
    int ,
    int
#endif
);

/* fFillOr.s */

unsigned char *fastFillSolidGXor(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned long ,
    unsigned long ,
    int ,
    int ,
    int ,
    int
#endif
);

/* fFillXor.s */

unsigned char *fastFillSolidGXxor(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned long ,
    unsigned long ,
    int ,
    int ,
    int ,
    int
#endif
);

/* fFillCopy.s */

unsigned char *fastFillSolidGXcopy(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned long ,
    unsigned long ,
    int ,
    int ,
    int ,
    int
#endif
); 

/* fFillSet.s */

unsigned char *fastFillSolidGXset(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned long ,
    unsigned long ,
    int ,
    int ,
    int ,
    int
#endif
);

/* vgabres.s */
void fastvga256BresS(
#if NeedFunctionPrototypes
    int ,
    unsigned long ,
    unsigned long ,
    unsigned long *,
    int ,
    register int ,
    int ,
    int ,
    int ,
    int ,
    register int ,
    register int ,
    int ,
    int 
#endif
);

/* vgalineH.s */
int fastvga256HorzS(
#if NeedFunctionPrototypes
    int ,
    unsigned long ,
    register unsigned long ,
    register unsigned long *,
    int ,
    int ,
    int ,
    int 
#endif
);

/* vgalineV.s */
void fastvga256VertS(
#if NeedFunctionPrototypes
    int ,
    unsigned long ,
    unsigned long ,
    unsigned long *,
    int ,
    int ,
    int ,
    register int 
#endif
);

/* vgaBitBlt.c */

void vgaBitBlt(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void OneBankvgaBitBlt(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vgaImageRead(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vgaImageWrite(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vgaPixBitBlt(
#if NeedFunctionPrototypes
    unsigned char *,
    unsigned char *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);

/* vgagc.c */
Bool vga256CreateGC(
#if NeedFunctionPrototypes
    register GCPtr 
#endif
);
/* vgawindow.c */
void vga256CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr ,
    DDXPointRec ,
    RegionPtr 
#endif
);
/* vgascrinit.c */
int vga256FinishScreenInit(
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
Bool vga256ScreenInit(
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
/* vgagetsp.c */
void vga256GetSpans(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    register DDXPointPtr ,
    int *,
    int ,
    char *
#endif
);
/* vgafillrct.c */
void vga256FillBoxTileOdd(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    PixmapPtr ,
    int ,
    int 
#endif
);
void vga256FillRectTileOdd(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr ,
    register GCPtr ,
    int ,
    xRectangle *
#endif
);
/* vgaimage.c */
void vga256GetImage(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    int ,
    int ,
    int ,
    unsigned int ,
    unsigned long ,
    char *
#endif
);
/* vgasolidC.c */
void vga256FillRectSolidCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256SolidSpansCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgasolidCS.c */
void speedupvga256FillRectSolidCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
/* vgasolidX.c */
void vga256FillRectSolidXor(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256SolidSpansXor(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgasolidO.c */
void vga256FillRectSolidOr(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256SolidSpansOr(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgasolidA.c */
void vga256FillRectSolidAnd(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256SolidSpansAnd(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgasolidG.c */
void vga256FillRectSolidGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256SolidSpansGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgatile32C.c */
void vga256FillRectTile32Copy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256Tile32FSCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgatile32G.c */
void vga256FillRectTile32General(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga256Tile32FSGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgatileoddC.c */
void vga256FillBoxTileOddCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    register BoxPtr ,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vga256FillSpanTileOddCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    DDXPointPtr ,
    int *,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vga256FillBoxTile32sCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vga256FillSpanTile32sCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    DDXPointPtr ,
    int *,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
/* vgatileoddG.c */
void vga256FillBoxTileOddGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    register BoxPtr ,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vga256FillSpanTileOddGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    DDXPointPtr ,
    int *,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vga256FillBoxTile32sGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
void vga256FillSpanTile32sGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    DDXPointPtr ,
    int *,
    PixmapPtr ,
    int ,
    int ,
    int ,
    unsigned long 
#endif
);
/* vgafillsp.c */
void vga256UnnaturalTileFS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GC *,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
void vga256UnnaturalStippleFS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GC *,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
void vga2568Stipple32FS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
void vga2568OpaqueStipple32FS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    DDXPointPtr ,
    int *,
    int 
#endif
);
/* vgasetsp.c */
int vga256SetScanline(
#if NeedFunctionPrototypes
    int ,
    int ,
    int ,
    int ,
    register unsigned int *,
    register int ,
    int *,
    int ,
    unsigned long 
#endif
);
void vga256SetSpans(
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
void vga256PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr ,
    RegionPtr ,
    int 
#endif
);
void vga256FillBoxSolid(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    unsigned long ,
    unsigned long ,
    int 
#endif
);
void vga256FillBoxTile32(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    PixmapPtr 
#endif
);
/* vgapntwinS.c */
void speedupvga256FillBoxSolid(
#if NeedFunctionPrototypes
    DrawablePtr ,
    int ,
    BoxPtr ,
    unsigned long ,
    unsigned long ,
    int 
#endif
);
/* vgazerarcC.c */
void vga256ZeroPolyArcSS8Copy(
#if NeedFunctionPrototypes
    register DrawablePtr ,
    GCPtr ,
    int ,
    xArc *
#endif
);
/* vgazerarcX.c */
void vga256ZeroPolyArcSS8Xor(
#if NeedFunctionPrototypes
    register DrawablePtr ,
    GCPtr ,
    int ,
    xArc *
#endif
);
/* vgazerarcG.c */
void vga256ZeroPolyArcSS8General(
#if NeedFunctionPrototypes
    register DrawablePtr ,
    GCPtr ,
    int ,
    xArc *
#endif
);
/* vgafillarcC.c */
void vga256PolyFillArcSolidCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    xArc *
#endif
);
/* vgafillarcG.c */
void vga256PolyFillArcSolidGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    xArc *
#endif
);
/* vgategblt.c */
void vga256TEGlyphBlt(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GC *,
    int ,
    int ,
    unsigned int ,
    CharInfoPtr *,
    pointer 
#endif
);
/* vgabstore.c */
void vga256SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr ,
    RegionPtr ,
    int ,
    int ,
    WindowPtr 
#endif
);
void vga256RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr ,
    RegionPtr ,
    int ,
    int ,
    WindowPtr 
#endif
);
/* vga8cppl.c */
void vga256CopyImagePlane(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long 
#endif
);
void vga256CopyPlane8to1(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
/* vgateblt8.c */
void vga256TEGlyphBlt8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GC *,
    int ,
    int ,
    unsigned int ,
    CharInfoPtr *,
    pointer 
#endif
);
/* vgateblt8S.c */
void speedupvga256TEGlyphBlt8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GC *,
    int ,
    int ,
    unsigned int ,
    CharInfoPtr *,
    pointer 
#endif
);
/* vgaglblt8.c */
void vga256PolyGlyphBlt8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    unsigned int ,
    CharInfoPtr *,
    pointer 
#endif
);
/* vgaglrop8.c */
void vga256PolyGlyphRop8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    unsigned int ,
    CharInfoPtr *,
    pointer 
#endif
);
/* vgarctstp8.c */
void vga2568FillRectOpaqueStippled32(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register BoxPtr 
#endif
);
void vga2568FillRectTransparentStippled32(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
/* vgarctstp8S.c */
void speedupvga2568FillRectOpaqueStippled32(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register BoxPtr 
#endif
);
void speedupvga2568FillRectTransparentStippled32(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    BoxPtr 
#endif
);
void vga2568FillRectStippledUnnatural(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register BoxPtr 
#endif
);
/* vgapolypnt.c */
void vga256PolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    xPoint *
#endif
);
/* vgaline.c */
void vga256LineSS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    DDXPointPtr 
#endif
);
void vga256LineSD(
#if NeedFunctionPrototypes
    DrawablePtr ,
    register GCPtr ,
    int ,
    int ,
    DDXPointPtr 
#endif
);
/* vgalineS.c */
void speedupvga256LineSS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    DDXPointPtr 
#endif
);
/* vgabresd.c */
void vga256BresD(
#if NeedFunctionPrototypes
    cfbRRopPtr ,
    int *,
    unsigned char *,
    int ,
    int *,
    int ,
    unsigned long *,
    int ,
    int ,
    int ,
    int ,
    int ,
    int ,
    register int ,
    register int ,
    int ,
    int 
#endif
);
/* vgaseg.c */
void vga256SegmentSS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register xSegment *
#endif
);
void vga256SegmentSD(
#if NeedFunctionPrototypes
    DrawablePtr ,
    register GCPtr ,
    int ,
    register xSegment *
#endif
);
/* vgasegS.c */
void speedupvga256SegmentSS(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    register xSegment *
#endif
);
/* vgabitblt.c */
void vga256DoBitblt(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long 
#endif
);
RegionPtr vga256CopyArea(
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
void vga256CopyPlane1to8(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
RegionPtr vga256CopyPlane(
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
/* vgabltC.c */
void vga256DoBitbltCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
/* vgabltCS.c */
void speedupvga256DoBitbltCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
/* vgabltX.c */
void vga256DoBitbltXor(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
/* vgabltO.c */
void vga256DoBitbltOr(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
/* vgabltG.c */
void vga256DoBitbltGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    DrawablePtr ,
    int ,
    RegionPtr ,
    DDXPointPtr ,
    unsigned long ,
    unsigned long 
#endif
);
/* vgapush8.c */
void vga256PushPixels8(
#if NeedFunctionPrototypes
    GCPtr ,
    PixmapPtr ,
    DrawablePtr ,
    int ,
    int ,
    int ,
    int 
#endif
);
/* vgaply1rctC.c */
void vga256FillPoly1RectCopy(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    DDXPointPtr 
#endif
);
/* vgaply1rctG.c */
void vga256FillPoly1RectGeneral(
#if NeedFunctionPrototypes
    DrawablePtr ,
    GCPtr ,
    int ,
    int ,
    int ,
    DDXPointPtr 
#endif
);
/* vgafuncs.c */

#endif /* _VGA256_H */
