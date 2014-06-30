/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach8/mach8.h,v 3.8 1996/12/23 06:39:42 dawes Exp $ */
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
 * KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, AND TIAGO GONS DISCLAIM
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD, OR TIAGO GONS BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Scott Laird (lair@kimbark.uchicago.edu)
 * and Tiago Gons (tiago@comosjn.hobby.nl)
 */
/* $XConsortium: mach8.h /main/8 1996/02/21 17:29:38 kaleb $ */

#ifndef MACH8_H
#define MACH8_H

#define MACH8_PATCHLEVEL "0"

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
#include "regmach8.h"

extern ScrnInfoRec mach8InfoRec;

extern short mach8alu[];

extern int mach8ValidTokens[];

extern void (*mach8ImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
extern void (*mach8ImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long 
#endif
);
extern void (*mach8RealImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
extern void (*mach8ImageStippleFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, Pixel, Pixel,
    short, unsigned long, int
#endif
);

/* Function Prototypes */

/* mach8.c */
Bool mach8Probe(
#if NeedFunctionPrototypes
    void
#endif
);
void mach8PrintIdent(
#if NeedFunctionPrototypes
    void
#endif
);
Bool mach8Initialize(
#if NeedFunctionPrototypes
    int,
    ScreenPtr,
    int,
    char **
#endif
);
void mach8EnterLeaveVT(
#if NeedFunctionPrototypes
    Bool,
    int 
#endif
);
Bool mach8CloseScreen(
#if NeedFunctionPrototypes
    int,
    ScreenPtr
#endif
);
Bool mach8SaveScreen(
#if NeedFunctionPrototypes
    ScreenPtr,
    Bool 
#endif
);
void mach8Adjust(
#if NeedFunctionPrototypes
    int,
    int 
#endif
);
Bool mach8SwitchMode(
#if NeedFunctionPrototypes
    DisplayModePtr 
#endif
);
Bool mach8ClockSelect(
#if NeedFunctionPrototypes
    int 
#endif
);
/* mach8cmap.c */
int mach8ListInstalledColormaps(
#if NeedFunctionPrototypes
    ScreenPtr,
    Colormap *
#endif
);
void mach8RestoreDACvalues(
#if NeedFunctionPrototypes
    void
#endif
);
void mach8StoreColors(
#if NeedFunctionPrototypes
    ColormapPtr,
    int,
    xColorItem *
#endif
);
void mach8InstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void mach8UninstallColormap(
#if NeedFunctionPrototypes
    ColormapPtr 
#endif
);
void mach8RestoreColor0(
#if NeedFunctionPrototypes
    ScreenPtr 
#endif
);
/* mach8gc.c */
Bool mach8CreateGC(
#if NeedFunctionPrototypes
    GCPtr 
#endif
);
/* mach8fs.c */
void mach8SolidFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void mach8TiledFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void mach8StipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
void mach8OStipFSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    DDXPointPtr,
    int *,
    int 
#endif
);
/* mach8ss.c */
void mach8SetSpans(
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
/* mach8gs.c */
void mach8GetSpans(
#if NeedFunctionPrototypes
    DrawablePtr,
    int,
    DDXPointPtr,
    int *,
    int,
    char *
#endif
);
/* mach8win.c */
void mach8CopyWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    DDXPointRec,
    RegionPtr 
#endif
);
/* mach8init.c */
void mach8calcvmode(
#if NeedFunctionPrototypes
    struct mach8vmodedef *,
    DisplayModePtr 
#endif
);
void mach8setcrtregs(
#if NeedFunctionPrototypes
    struct mach8vmodedef *
#endif
);
void mach8CleanUp(
#if NeedFunctionPrototypes
    void
#endif
);
void mach8Init(
#if NeedFunctionPrototypes
    struct mach8vmodedef *
#endif
);
void InitLUT(
#if NeedFunctionPrototypes
    void
#endif
);
void mach8InitEnvironment(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach8im.c */
void mach8ImageInit(
#if NeedFunctionPrototypes
    void
#endif
);
void mach8ImageFill(
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
/* mach8bstor.c */
void mach8SaveAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
void mach8RestoreAreas(
#if NeedFunctionPrototypes
    PixmapPtr,
    RegionPtr,
    int,
    int,
    WindowPtr 
#endif
);
/* mach8scrin.c */
Bool mach8ScreenInit(
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
/* mach8blt.c */
RegionPtr mach8CopyArea(
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
void mach8FindOrdering(
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
RegionPtr mach8CopyPlane(
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
/* mach8pcach.c */
/* mach8plypt.c */
void mach8PolyPoint(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    xPoint *
#endif
);
/* mach8line.c */
void mach8Line(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* mach8seg.c */
void mach8Segment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* mach8frect.c */
void mach8PolyFillRect(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xRectangle *
#endif
);
/* mach8text.c */
int mach8PolyText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *
#endif
);
int mach8PolyText16(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned short *
#endif
);
void mach8ImageText8(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    char *
#endif
);
void mach8ImageText16(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    int,
    unsigned short *
#endif
);
/* mach8font.c */
Bool mach8RealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr,
    FontPtr 
#endif
);
Bool mach8UnrealizeFont(
#if NeedFunctionPrototypes
    ScreenPtr,
    FontPtr 
#endif
);
/* mach8fcach.c */
void mach8FontCache8Init(
#if NeedFunctionPrototypes
    void
#endif
);
/* mach8dsln.c */
void mach8DashLine(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* mach8dssg.c */
void mach8DashSegment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* mach8ddln.c */
void mach8DoubleDashLine(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr 
#endif
);
/* mach8ddsg.c */
void mach8DoubleDashSegment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    xSegment *
#endif
);
/* mach8gtimg.c */
void mach8GetImage(
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

/* mach8*.s */

void mach8ImageRead(
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
void mach8ImageWrite(
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
void mach8ImageReadDram(
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
void mach8ImageWriteDram(
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
void mach8RealImageFillDram(
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
void mach8ImageStippleDram(
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
    unsigned long,
    int
#endif
);
void mach8ImageStipple(
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
    unsigned long,
    int
#endif
);
void mach8RealImageFill(
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
void mach8InitFrect(
#if NeedFunctionPrototypes
    int,
    int,
    int
#endif
);
#endif /* MACH8_H */
