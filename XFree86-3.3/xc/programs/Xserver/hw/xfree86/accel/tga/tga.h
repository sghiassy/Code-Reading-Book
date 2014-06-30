/* $XConsortium: tga.h /main/4 1996/10/27 11:47:29 kaleb $ */
/*
 * Copyright 1995,96 by Alan Hourihane <alanh@fairlite.demon.co.uk>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Alan Hourihane not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Alan Hourihane makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * ALAN HOURIHANE DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ALAN HOURIHANE BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/tga/tga.h,v 3.4 1997/01/18 06:55:24 dawes Exp $ */

#ifndef _TGA_H_
#define _TGA_H_

#define TGA_PATCHLEVEL "0"

#ifndef LINKKIT

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "pixmap.h"
#include "region.h"
#include "gc.h"
#include "gcstruct.h"
#include "colormap.h"
#include "colormapst.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "windowstr.h"
#include "compiler.h"
#include "misc.h"
#include "xf86.h"
#include "regionstr.h"
#include "xf86_OSlib.h"
#include "xf86bcache.h"
#include "xf86fcache.h"
#include "xf86Procs.h"

#include "tgacurs.h"
#include "tga_regs.h"

extern volatile unsigned long *VidBase;

#include <X11/Xfuncproto.h>

#else /* !LINKKIT */
#include "X.h"
#include "input.h"
#include "misc.h"
#include "xf86.h"
#include "xf86_OSlib.h"
#endif /* !LINKKIT */

#if !defined(__GNUC__) || defined(NO_INLINE)
#define __inline__ /**/
#endif

extern ScrnInfoRec tgaInfoRec;
extern Bool tgaBt485PixMux;

#ifndef LINKKIT
_XFUNCPROTOBEGIN

extern void (*tgaImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
extern void (*tgaImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
extern void (*tgaImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);

extern short tgaalu[];
extern pointer tgaVideoMem;
extern ScreenPtr savepScreen;

extern int tgaValidTokens[];

extern Bool tgaDAC8Bit;

/* Function Prototypes */

/* tgaConf.c */
/* tga.c */
void tgaPrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
Bool tgaProbe(
#if NeedFunctionPrototypes
    void
#endif
);
/* tgamisc.c */
Bool tgaInitialize(
#if NeedFunctionPrototypes
    int,
    ScreenPtr,
    int,
    char **
#endif
);
void tgaEnterLeaveVT(
#if NeedFunctionPrototypes
    Bool,
    int 
#endif
);
Bool tgaCloseScreen(
#if NeedFunctionPrototypes
    int,
    ScreenPtr
#endif
);
Bool tgaSaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr,
    Bool 
#endif
);
Bool tgaSwitchMode(
#if NeedFunctionPrototypes
    DisplayModePtr 
#endif
);
void tgaDPMSSet(
#if NeedFunctionPrototypes
    int PowerManagementMode
#endif
);
void tgaAdjustFrame(
#if NeedFunctionPrototypes
    int,
    int 
#endif
);
/* tgacmap.c */
int tgaListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr,
    Colormap *
#endif
);
void tgaRestoreDACvalues(
#if NeedFunctionPrototypes
    void
#endif
);
int tgaGetInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr,
    ColormapPtr *
#endif
);
void tgaStoreColors(
#if NeedFunctionPrototypes
    ColormapPtr,
    int,
    xColorItem *
#endif
);
void tgaInstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void tgaUninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void tgaRestoreColor0(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
/* tgagc.c */
Bool tgaCreateGC(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
/* tgagc16.c */
Bool tgaCreateGC16(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
/* tgagc32.c */
Bool tgaCreateGC32(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
/* tgafs.c */
void tgaSolidFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void tgaTiledFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void tgaStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void tgaOStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
/* tgass.c */
void tgaSetSpans(
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
/* tgags.c */
void tgaGetSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    int,
    DDXPointPtr,
    int *,
    int,
    char *
#endif
);
/* tgawin.c */
void tgaCopyWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    DDXPointRec,
    RegionPtr 
#endif
);
/* tgainit.c */
void tgaCalcCRTCRegs(
#if NeedFunctionPrototypes
    tgaCRTCRegPtr, 
    DisplayModePtr
#endif
);
void tgaSetCRTCRegs(
#if NeedFunctionPrototypes
    tgaCRTCRegPtr
#endif
);
void tgaCleanUp(
#if NeedFunctionPrototypes
    void
#endif
);
Bool tgaInit(
#if NeedFunctionPrototypes
    DisplayModePtr 
#endif
);
void tgaInitEnvironment(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaUnlock(
#if NeedFunctionPrototypes
    void
#endif
);
/* tgaim.c */
void tgaImageInit(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaImageWriteNoMem(
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
void tgaImageStipple(
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
    Pixel,
    short,
    unsigned long
#endif
);
void tgaImageOpStipple(
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
    Pixel,
    Pixel,
    short,
    unsigned long 
#endif
);
/* tgabstor.c */
void tgaSaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
void tgaRestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
/* tgascrin.c */
Bool tgaScreenInit(
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
/* tgablt.c */
RegionPtr tgaCopyArea(
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
void tgaFindOrdering(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GC *,
    int,
    BoxPtr,
    int,
    int,
    int,
    int,
    unsigned int *
#endif
);
RegionPtr tgaCopyPlane(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    int,
    int,
    int,
    unsigned long 
#endif
);
/* tgaplypt.c */
void tgaPolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    xPoint *
#endif
);
/* tgaline.c */
void tgaLine(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* tgaseg.c */
void tgaSegment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* tgafrect.c */
void tgaPolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xRectangle *
#endif
);
void tgaInitFrect(
#if NeedFunctionPrototypes
    int,
    int,
    int
#endif
);
/* tgatext.c */
int tgaNoCPolyText(
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

void tgaFontStipple(
#if NeedFunctionPrototypes
    int,
    int,
    int,
    int,
    unsigned char *,
    int,
    Pixel
#endif
);


int tgaNoCImageText(
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
/* tgafont.c */
Bool tgaRealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr,
    FontPtr 
#endif
);
Bool tgaUnrealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr,
    FontPtr 
#endif
);
/* tgafcach.c */
void tgaFontCache8Init(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaGlyphWrite(
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
/* tgabcach.c */
void tgaCacheMoveBlock(
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
/* tgacurs.c */
Bool tgaCursorInit(
#if NeedFunctionPrototypes
    char *,
    ScreenPtr 
#endif
);
void tgaShowCursor(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaHideCursor(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaRestoreCursor(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
void tgaRepositionCursor(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
void tgaRenewCursorColor(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
void tgaWarpCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    int,
    int 
#endif
);
void tgaQueryBestSize(
#if NeedFunctionPrototypes
    int,
    unsigned short *,
    unsigned short *,
    ScreenPtr 
#endif
);
/* tgaBtCurs.c */
void tgaBtRecolorCursor(
#if NeedFunctionPrototypes
    ScreenPtr, CursorPtr, Bool
#endif
);
/* tgadline.c */
void tgaDline(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* tgadseg.c */
void tgaDsegment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* tgagtimg.c */
void tgaGetImage(
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
/* tgaTiCursor.c */
void tgaOutTiIndReg(
#if NeedFunctionPrototypes
    unsigned char,
    unsigned char,
    unsigned char 
#endif
);
unsigned char tgaInTiIndReg(
#if NeedFunctionPrototypes
    unsigned char 
#endif
);
Bool tgaTiRealizeCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    CursorPtr 
#endif
);
void tgaTiCursorOn(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaTiCursorOff(
#if NeedFunctionPrototypes
    void
#endif
);
void tgaTiMoveCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    int,
    int 
#endif
);
void tgaTiRecolorCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    CursorPtr 
#endif
);
void tgaTiLoadCursor(
#if NeedFunctionPrototypes
    ScreenPtr,
    CursorPtr,
    int,
    int 
#endif
);

_XFUNCPROTOEND

#endif /* !LINKKIT */
#endif /* _I128_H_ */

