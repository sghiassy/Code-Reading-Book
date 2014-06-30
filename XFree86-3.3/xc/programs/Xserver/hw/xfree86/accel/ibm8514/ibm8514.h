/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/ibm8514.h,v 3.8 1996/12/23 06:37:53 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 */
/* $XConsortium: ibm8514.h /main/10 1996/02/21 17:24:42 kaleb $ */


#ifndef IBM8514_H
#define IBM8514_H

#define IBM8514_PATCHLEVEL "0"

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
#include "reg8514.h"

extern ScrnInfoRec ibm8514InfoRec;

extern short ibm8514alu[];

extern int ibm8514ValidTokens[];

/* function Prototypes */

/* ibm8514imrd.s */

void ibm8514ImageRead(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
void ibm8514ImageWrite(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
void ibm8514RealImageFill(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
void ibm8514ImageStipple(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, Pixel, Pixel, short,
    unsigned long, int 
#endif
);

/* ibm8514.c */
Bool ibm8514Probe(
#if NeedFunctionPrototypes
    void
#endif
);
void ibm8514PrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
Bool ibm8514Initialize(
#if NeedFunctionPrototypes
    int,
    ScreenPtr,
    int,
    char **
#endif
);
void ibm8514EnterLeaveVT(
#if NeedFunctionPrototypes
    Bool,
    int 
#endif
);
Bool ibm8514CloseScreen(
#if NeedFunctionPrototypes
    int,
    ScreenPtr
#endif
);
Bool ibm8514SaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr,
    Bool 
#endif
);
/* ibm8514cmap.c */
int ibm8514ListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr,
    Colormap *
#endif
);
void ibm8514RestoreDACvalues(
#if NeedFunctionPrototypes
    void
#endif
);
void ibm8514StoreColors(
#if NeedFunctionPrototypes
    ColormapPtr,
    int,
    xColorItem *
#endif
);
void ibm8514InstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void ibm8514UninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void ibm8514RestoreColor0(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
/* ibm8514gc.c */
Bool ibm8514CreateGC(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
/* ibm8514fs.c */
void ibm8514SolidFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ibm8514TiledFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ibm8514StipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void ibm8514OStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
/* ibm8514ss.c */
void ibm8514SetSpans(
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
/* ibm8514gs.c */
void ibm8514GetSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    int,
    DDXPointPtr,
    int *,
    int,
    char *
#endif
);
/* ibm8514win.c */
void ibm8514CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    DDXPointRec,
    RegionPtr 
#endif
);
/* ibm8514init.c */
void ibm8514CleanUp(
#if NeedFunctionPrototypes
    void
#endif
);
void ibm8514Init(
#if NeedFunctionPrototypes
    short 
#endif
);
void InitLUT(
#if NeedFunctionPrototypes
    void
#endif
);
void InitEnvironment(
#if NeedFunctionPrototypes
    void
#endif
);
/* ibm8514im.c */
void ibm8514ImageInit(
#if NeedFunctionPrototypes
    void
#endif
);
void ibm8514ImageFill(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long 
#endif
);
/* ibm8514bsto.c */
void ibm8514SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
void ibm8514RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
/* ibm8514scri.c */
Bool ibm8514ScreenInit(
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
/* ibm8514blt.c */
RegionPtr ibm8514CopyArea(
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
void ibm8514FindOrdering(
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
RegionPtr ibm8514CopyPlane(
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
/* ibm8514pcac.c */
/* ibm8514plyp.c */
void ibm8514PolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    xPoint *
#endif
);
/* ibm8514line.c */
void ibm8514Line(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* ibm8514seg.c */
void ibm8514Segment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* ibm8514frec.c */
void ibm8514PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xRectangle *
#endif
);
/* ibm8514text.c */
int ibm8514PolyText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *
#endif
);
int ibm8514PolyText16(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned short *
#endif
);
void ibm8514ImageText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *
#endif
);
void ibm8514ImageText16(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned short *
#endif
);
/* ibm8514font.c */
Bool ibm8514RealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr,
    FontPtr 
#endif
);
Bool ibm8514UnrealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr,
    FontPtr 
#endif
);
/* ibm8514fcac.c */
void ibm8514FontCache8Init(
#if NeedFunctionPrototypes
    void 
#endif
);
void ibm8514UnCacheFont8(
#if NeedFunctionPrototypes
    FontPtr 
#endif
);
int ibm8514CacheFont8(
#if NeedFunctionPrototypes
    FontPtr 
#endif
);
int ibm8514CPolyText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *,
    int 
#endif
);
void ibm8514CImageText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *,
    int 
#endif
);
/* ibm8514dsln.c */
void ibm8514DashLine(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* ibm8514dssg.c */
void ibm8514DashSegment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* ibm8514ddln.c */
void ibm8514DoubleDashLine(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* ibm8514ddsg.c */
void ibm8514DoubleDashSegment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* ibm8514gtim.c */
void ibm8514GetImage(
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
void ibm8514InitFrect(
#if NeedFunctionPrototypes
    int,
    int,
    int
#endif
);
#endif /* IBM8514_H */
