/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000.h,v 3.16.2.1 1997/05/10 11:34:03 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Erik Nygren <nygren@mit.edu>.
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
 * ERIK NYGREN, CHRIS MASON, AND KEVIN MARTIN DISCLAIM ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK NYGREN, CHRIS
 * MASON, OR KEVIN MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for P9000 by Erik Nygren <nygren@mit.edu>
 * Additional changes by Chris Mason <mason@mail.csh.rit.edu>
 * 
 */
/* $XConsortium: p9000.h /main/9 1996/05/07 17:11:20 kaleb $ */

#ifndef P9000_H
#define P9000_H

/****************************/
/* Compile Time Options     */
/* WARNING: THESE ARE ALPHA */
/* CODE AND MAY NOT BE      */
/* FINISHED                 */
#define P9000_ACCEL        /* Use acceleration */
#define P9000_IM_ACCEL     /* Use image read from p9000im.c */
#define P9000_ORCHID_SUP   /* Support the Orchid P9000 (may not work) */
#define P9000_VPRPCI_SUP   /* Support the Viper PCI (may not work yet) */
/* #define DEBUG            */
/****************************/

#define P9000_PATCHLEVEL "0"

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
#include "cursorstr.h"
#include "compiler.h"
#include "misc.h"
#include "xf86.h"
#include "regionstr.h"
#include "xf86_OSlib.h"
#include "xf86Procs.h"

#include "p9000reg.h"
#include "p9000curs.h"

typedef Bool (* ProbeVendorProcPtr)(
#if NeedNestedPrototypes
   void
#endif
);

typedef void (* SetClockVendorProcPtr)(
#if NeedNestedPrototypes
   long, long
#endif
);

typedef void (* EnableVendorProcPtr)(
#if NeedNestedPrototypes
   p9000CRTCRegPtr
#endif
);

typedef void (* DisableVendorProcPtr)(
#if NeedNestedPrototypes
   void
#endif
);

typedef Bool (* ValidateVendorProcPtr)(
#if NeedNestedPrototypes
   void
#endif
);

typedef void (* InitializeVendorProcPtr)(
#if NeedNestedPrototypes
   int,         /* The index of pScreen in the ScreenInfo */
   ScreenPtr,   /* The Screen to initialize */
   int,         /* The number of the Server's arguments. */
   char **      /* The arguments themselves. Don't change! */
#endif
);

/* Vendor Labels */
#define P9000_VENDOR_VIPERVLB 1   /* Diamond Viper VLB */
#define P9000_VENDOR_VIPERPCI 2   /* Diamond Viper PCI */
#define P9000_VENDOR_ORCHID   3   /* Orchid P9000 and Weitek compatables */

typedef struct {
  char *Desc;                     /* Description of the vendor */
  char *Vendor;                   /* The name of the vendor */
  ProbeVendorProcPtr Probe;       /* Probes for a vendor's hardware and returns
			           * true if it is present. */
  SetClockVendorProcPtr SetClock; /* Takes the Dot Clock and Memory Clock in 
			           * Hz and sets things to that. */
  EnableVendorProcPtr Enable;     /* Disables VGA and enables the P9000 */
  DisableVendorProcPtr Disable;   /* Disables the P9000 and enables VGA */
  ValidateVendorProcPtr Validate; /* Validates XF86Config parameters like
				   * memory base and returns FALSE
				   * if failed (causes p9000Probe to fail) */
  InitializeVendorProcPtr Initialize; /* Does first-time only initialization
				   * (like memory mapping for RAMDAC) */
  int Label;                      /* A numerical label for quick compares. */
} p9000VendorRec;

extern p9000MiscRegRec  p9000MiscReg;
extern p9000VendorRec  *p9000VendorPtr;

/* p9000.c */
extern Bool p9000Probe(
#if NeedFunctionPrototypes
   void
#endif
);

extern Bool p9000Initialize(
#if NeedFunctionPrototypes
   int, ScreenPtr, int, char **
#endif
);

extern void p9000EnterLeaveVT(
#if NeedFunctionPrototypes
   Bool, int
#endif
);

extern void p9000AdjustFrame(
#if NeedFunctionPrototypes
   int, int
#endif
);

extern void p9000SetVidPage(
#if NeedFunctionPrototypes
   int
#endif
);

extern void p9000PrintIdent(
#if NeedFunctionPrototypes
   void
#endif
);

extern Bool p9000SwitchMode(
#if NeedFunctionPrototypes
   DisplayModePtr
#endif
);

extern Bool p9000SaveScreen(
#if NeedFunctionPrototypes
   ScreenPtr, Bool
#endif
);

#ifdef DPMSExtension
extern void p9000DPMSSet(
#if NeedFunctionPrototypes
   int PowerManagementMode
#endif
);
#endif

extern Bool p9000CloseScreen(
#if NeedFunctionPrototypes
   int, ScreenPtr
#endif
);

/****************** p9000init.c *******************/

extern Bool p9000CalcSysconfigHres(
#if NeedFunctionPrototypes
   int, unsigned long, unsigned long *
#endif
);

extern void p9000ClearScreen(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000CalcMiscRegs(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000CalcCRTCRegs(
#if NeedFunctionPrototypes
   p9000CRTCRegPtr, DisplayModePtr
#endif
);

extern void p9000SetCRTCRegs(
#if NeedFunctionPrototypes
   p9000CRTCRegPtr
#endif
);

extern void p9000InitAperture(
#if NeedFunctionPrototypes
   int
#endif
);

extern void p9000InitEnvironment(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000CleanUp(
#if NeedFunctionPrototypes
   void
#endif
);

extern Bool p9000SysconfigHres(
#if NeedFunctionPrototypes
   int, int, unsigned long *
#endif
);

/****************** p9000vga.c *****************/

extern void p9000SaveVT(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000RestoreVT(
#if NeedFunctionPrototypes
   void
#endif
);


/********************** p9000cmap.c **************************/

extern void p9000InitLUT(
#if NeedFunctionPrototypes
   void
#endif
);

extern void p9000ReadLUT(
#if NeedFunctionPrototypes
   LUTENTRY *
#endif
);

extern void p9000WriteLUT(
#if NeedFunctionPrototypes
   LUTENTRY *
#endif
);

extern void p9000RestoreDACvalues(
#if NeedFunctionPrototypes
   void
#endif
);

extern int p9000ListInstalledColormaps(
#if NeedFunctionPrototypes
   ScreenPtr, Colormap *
#endif
);

extern void p9000StoreColors(
#if NeedFunctionPrototypes
   ColormapPtr, int, xColorItem	*
#endif
);

extern void p9000InstallColormap(
#if NeedFunctionPrototypes
   ColormapPtr
#endif
);

extern void p9000UninstallColormap(
#if NeedFunctionPrototypes
   ColormapPtr
#endif
);

/*********************** p9000scrin.c **************************/

extern Bool p9000ScreenInit(
#if NeedFunctionPrototypes
   register ScreenPtr, pointer, int, int, int, int, int
#endif
);

#ifdef P9000_ACCEL

/*********************** p9000gc.c **************************/

extern void
p9000InitGC(
#if NeedFunctionPrototypes
    void  
#endif
);

extern Bool
p9000CreateGC(
#if NeedFunctionPrototypes
    register GCPtr
#endif
);

extern Bool
p9000CreateGC16(
#if NeedFunctionPrototypes
    register GCPtr
#endif
);

extern Bool
p9000CreateGC32(
#if NeedFunctionPrototypes
    register GCPtr
#endif
);

extern void
p9000ValidateGC(
#if NeedFunctionPrototypes
    register GCPtr,
    unsigned long,
    DrawablePtr
#endif
);

extern void
p9000ValidateGC16(
#if NeedFunctionPrototypes
    register GCPtr,
    unsigned long,
    DrawablePtr
#endif
);

extern void
p9000ValidateGC32(
#if NeedFunctionPrototypes
    register GCPtr,
    unsigned long,
    DrawablePtr
#endif
);

/*********************** p9000im.c **************************/

extern void p9000ImageInit(
#if NeedFunctionPrototypes
void
#endif
);

extern void (*p9000ImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);

extern void (*p9000ImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);

extern void p9000ImageOpStipple(
#if NeedFunctionPrototypes
    int x, int y, int w, int h, char *psrc, int pwidth, int pw, int ph, 
    int pox, int poy, int fgPixel, int bgPixel, int alu, int planemask
#endif
) ;

extern void p9000ImageStipple(
#if NeedFunctionPrototypes
    int x, int y, int w, int h, char *psrc, int pwidth, int pw, int ph, 
    int pox, int poy, int fgPixel, int alu, int planemask
#endif
) ;

/* don't use this one yet! */
extern void p9000ImageFill(
#if NeedFunctionPrototypes
    int x, int y, int w, int h, char *psrc, int pwidth, int pw, int ph, 
    int pox, int poy, int alu, int planemask
#endif
) ;


/*********************** p9000blt.c **************************/

extern RegionPtr
p9000CopyArea(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GC *,
    int, int,
    int, int,
    int, int
#endif
);

extern RegionPtr
p9000CopyArea16(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GC *,
    int, int,
    int, int,
    int, int
#endif
);

extern RegionPtr
p9000CopyArea32(
#if NeedFunctionPrototypes
    DrawablePtr,
    DrawablePtr,
    GC *,
    int, int,
    int, int,
    int, int
#endif
);

extern RegionPtr p9000CopyPlane(
#if NeedFunctionPrototypes
    DrawablePtr /*pSrcDrawable*/,
    DrawablePtr /*pDstDrawable*/,
    GCPtr /*pGC*/,
    int /*srcx*/,
    int /*srcy*/,
    int /*width*/,
    int /*height*/,
    int /*dstx*/,
    int /*dsty*/,
    unsigned long /*bitPlane*/
#endif
);

extern void p9000CopyWindow() ;
 
/***********/

#endif /* P9000_ACCEL */

/*********************** p9000curs.c ****************************/

void p9000WarpCursor(
#if NeedFunctionPrototypes
   ScreenPtr, int, int
#endif
);

void p9000QueryBestSize(
#if NeedFunctionPrototypes
  int, unsigned short *, unsigned short *, ScreenPtr
#endif
);

Bool p9000CursorInit(
#if NeedFunctionPrototypes
   char *, ScreenPtr
#endif
);

/*********************** p9000BtCurs.c ****************************/

void p9000BtRecolorCursor(
#if NeedFunctionPrototypes
   ScreenPtr, CursorPtr, Bool
#endif
);

/*********************** p9000Line.c *****************************/

void p9000Line(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr
#endif
);

void p9000Line1Rect(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    int,
    DDXPointPtr
#endif
);

/********************** p9000seg.c ******************************/

void p9000Segment(
#if NeedFunctionPrototypes
    DrawablePtr,
    GCPtr,
    int,
    register xSegment *
#endif
);

/********************** p9000frect.c ******************************/

void
p9000PolyFillRect(
#if NeedFunctionPrototypes
     DrawablePtr,
     GCPtr,
     int,
     xRectangle*
#endif
);
 
void
p900016PolyFillRect(
#if NeedFunctionPrototypes
     DrawablePtr,
     GCPtr,
     int,
     xRectangle*
#endif
);

void
p900032PolyFillRect(
#if NeedFunctionPrototypes
     DrawablePtr,
     GCPtr,
     int,
     xRectangle*
#endif
);


/********************** p9000pntwin.c ******************************/

void
p9000PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    RegionPtr,
    int
#endif
);

void
p900016PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    RegionPtr,
    int
#endif
);

void
p900032PaintWindow(
#if NeedFunctionPrototypes
    WindowPtr,
    RegionPtr,
    int
#endif
);

/*********************** p9000text.c *********************************/

int
p9000PolyText8(
#if NeedFunctionPrototypes
     DrawablePtr pDraw,
     GCPtr pGC,
     int   x,
     int   y,
     int   count,
     char *chars
#endif
) ;

int
p9000PolyText16(
#if NeedFunctionPrototypes
     DrawablePtr pDraw,
     GCPtr pGC,
     int   x,
     int   y,
     int   count,
     unsigned short *chars
#endif
);

/* these two are slower than the software */
int
p9000ImageText8() ;

int
p9000ImageText16() ;
 
/****************************************************************/


extern ScrnInfoRec p9000InfoRec;

extern int p9000ValidTokens[];

/* P9000 control linear mapped memory base */
extern volatile unsigned long *CtlBase;
/* P9000 linear mapped frame buffer base */
extern volatile unsigned long *VidBase;  

extern volatile pointer p9000VideoMem;

extern Bool p9000SWCursor;         /* Use a software cursor */
extern Bool p9000DACSyncOnGreen;   /* Enables syncing on green */
extern Bool p9000DAC8Bit;          /* Only 8 bit is supported for now
				    * (as opposed to 6 bit) */
extern Bool p9000NoAccel;          /* Disables hardware acceleration */

extern ScreenPtr p9000savepScreen;

extern unsigned p9000BytesPerPixel;

/* Raster operation (alu) -> minterm mapping */
extern unsigned int p9000alu[];		/* alu src = p9000 src        */
extern unsigned int p9000QuadAlu[] ;	/* alu src = p9000 foreground */
extern unsigned int p9000PixOpAlu[] ;	/* use for opaque pix1 ops */
extern unsigned int p9000PixAlu[] ;	/* use for transparent pix1 ops */

/* Retrieve a long word from memory */
#ifndef p9000Fetch
#define p9000Fetch(Off,Base) (*(volatile unsigned long *)(Base + ((Off)/4L)))
#endif

/* Store a long word into memory */
#ifndef p9000Store
#define p9000Store(Off,Base,Data)  *(volatile unsigned long *)(Base + ((Off)/4L)) = (unsigned long)Data
#endif

/* Retrieve a byte from memory */
#ifndef p9000Fetch8
#define p9000Fetch8(Off,Base) (*(volatile unsigned char *)(Base + ((Off)/4L)))
#endif

/* Store a byte into memory */
#ifndef p9000Store8
#define p9000Store8(Off,Base,Data)  *(volatile unsigned char *)(Base + ((Off)/4L)) = (unsigned char)Data
#endif

/* Wait for Drawing Engine to be free */
#ifndef p9000NotBusy
#define p9000NotBusy()  while(p9000Fetch(STATUS,CtlBase) & SR_BUSY)
#endif

/* Wait for Blit/Quad Engine to be free */
#ifndef p9000QBNotBusy
#define p9000QBNotBusy() while(p9000Fetch(STATUS,CtlBase) & SR_ISSUE_QBN)
#endif

/* 
** offscreen memory looks something like this:
**     fontcache: 78 rows
**     stipple:  everything else
** these are very experimental and subject to change -- CLM 5/12/95
*/

#ifndef _OFFSCREENCONST_
#define _OFFSCREENCONST_
/* address for the first byte of offscreen memory */
#define OFFSCREENBYTE (p9000InfoRec.virtualX * (p9000InfoRec.virtualY) * \
		       p9000BytesPerPixel) 

/* x value for first offscreen byte */
#define OFFSCREENX    0

/* y value for first offscreen byte */
#define OFFSCREENY    (p9000InfoRec.virtualY)

/* font chache start and height */
#define FONTCACHEY (OFFSCREENY)
#define FONTCACHEBYTE (OFFSCREENBYTE)
#define FONTCACHEHEIGHT 0

/* where the offscreen area for p9000Stipple starts */
#define STIPPLEY (OFFSCREENY + FONTCACHEHEIGHT)
#define STIPPLEBYTE (p9000InfoRec.virtualX * STIPPLEY * p9000BytesPerPixel)
#endif

#endif /* P9000_H */
