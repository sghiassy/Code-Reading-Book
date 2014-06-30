/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vga.c,v 3.71.2.10 1997/06/01 12:33:41 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 */
/* $XConsortium: vga.c /main/29 1996/10/25 10:34:12 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "cursor.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "gcstruct.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "vga.h"
#include "vgaPCI.h"

#ifdef PC98
#include "pc98_vers.h"
#endif

#ifdef XFreeXDGA
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#if !defined(MONOVGA) && !defined(XF86VGA16)
#include "vga256.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "xf86scrin.h"
#include "xf86xaa.h"
#else
#include "windowstr.h"
#endif

#ifdef PC98_EGC
#define EGC_PLANE	0x4a0
#define EGC_MODE	0x4a4
#define EGC_FGC		0x4a6
#define EGC_COPY_MODE	0x2cac
#endif

#if defined(PC98_TGUI)
extern pointer pc98PvramBase;
#endif


#ifndef XF86VGA16
#ifdef MONOVGA
extern void mfbDoBitbltCopy();
extern void mfbDoBitbltCopyInverted();
#ifdef BANKEDMONOVGA
extern void mfbDoBitbltTwoBanksCopy();
extern void mfbDoBitbltTwoBanksCopyInverted();
void (*ourmfbDoBitbltCopy)();
void (*ourmfbDoBitbltCopyInverted)();
#endif
#else
unsigned long useSpeedUp = 0;
#if !defined(__alpha__) && !defined(__arm32__)
extern void speedupvga256TEGlyphBlt8();
extern void speedupvga2568FillRectOpaqueStippled32();
extern void speedupvga2568FillRectTransparentStippled32();
#else /* !__alpha__ && !__arm32__ */
extern void vga256TEGlyphBlt8();
extern void vga2568FillRectOpaqueStippled32();
extern void vga2568FillRectTransparentStippled32();
#endif /* !__alpha__ && !__arm32__ */
extern void OneBankvgaBitBlt();
#endif /* MONOVGA */
#endif /* !XF86VGA16 */

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;
extern char *xf86VisualNames[];
extern Bool miDCInitialize();

ScrnInfoRec vga256InfoRec = {
  FALSE,		/* Bool configured */
  -1,			/* int tmpIndex */
  -1,			/* int scrnIndex */
  vgaProbe,		/* Bool (* Probe)() */
  vgaScreenInit,	/* Bool (* Init)() */
  vgaValidMode,		/* Bool (* ValidMode)() */
  vgaEnterLeaveVT,	/* void (* EnterLeaveVT)() */
  (void (*)())NoopDDA,	/* void (* EnterLeaveMonitor)() */
  (void (*)())NoopDDA,	/* void (* EnterLeaveCursor)() */
  vgaAdjustFrame,	/* void (* AdjustFrame)() */
  vgaSwitchMode,	/* Bool (* SwitchMode)() */
  vgaDPMSSet,		/* void (* DPMSSet)() */
  vgaPrintIdent,        /* void (* PrintIdent)() */
#ifdef MONOVGA
  1,			/* int depth */
#else
#ifdef XF86VGA16
  4,			/* int depth */
#else
  8,			/* int depth */
#endif
#endif
  {0, 0, 0},            /* xrgb weight */
#ifdef MONOVGA
  1,			/* int bitsPerPixel */
#else
  8,			/* int bitsPerPixel */
#endif
  -1,			/* int defaultVisual */
  -1, -1,		/* int virtualX,virtualY */
  -1,                   /* int displayWidth */
  -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
  {0, },		/* OFlagSet options */
  {0, },		/* OFlagSet clockOptions */
  {0, },                /* OFlagSet xconfigFlag */
  NULL,			/* char *chipset */
  NULL,			/* char *ramdac */
  {0, 0, 0, 0},		/* int dacSpeeds[MAXDACSPEEDS] */
  0,			/* int dacSpeedBpp */
  0,			/* int clocks */
  {0, },		/* int clock[MAXCLOCKS] */
  DEFAULT_MAX_CLOCK,	/* int maxClock */
  0,			/* int videoRam */
#ifndef	PC98_EGC
  0xC0000,		/* int BIOSbase */
#else
  0xE8000,		/* int BIOSbase */
#endif
  0,			/* unsigned long MemBase, unused for this driver */
  240, 180,		/* int width, height */
  0,			/* unsigned long speedup */
  NULL,			/* DisplayModePtr modes */
  NULL,			/* MonPtr monitor */
  NULL,			/* char *clockprog */
  -1,                   /* int textclock */
  FALSE,		/* Bool bankedMono */
#ifdef MONOVGA
  "VGA2",		/* char *name */
#else
#ifdef XF86VGA16
  "VGA16",		/* char *name */
#else
  "SVGA",		/* char *name */
#endif
#endif
  {0, 0, 0},		/* xrgb blackColour */ 
  {0x3F, 0x3F, 0x3F},	/* xrgb whiteColour */ 
#ifdef MONOVGA
  vga2ValidTokens,	/* int *validTokens */
  VGA2_PATCHLEVEL,	/* char *patchLevel */
#else
#ifdef XF86VGA16
  vga16ValidTokens,	/* int *validTokens */
  VGA16_PATCHLEVEL,	/* char *patchLevel */
#else
  vga256ValidTokens,	/* int *validTokens */
  SVGA_PATCHLEVEL,	/* char *patchLevel */
#endif
#endif
  0,			/* int IObase */
  0,			/* int PALbase */
  0,			/* int COPbase */
  0,			/* int POSbase */
  0,			/* int instance */
  0,			/* int s3Madjust */
  0,			/* int s3Nadjust */
  0,			/* int s3MClk */
  0,			/* int chipID */
  0,			/* int chipRev */
  0,			/* unsigned long VGAbase */
  0,			/* int s3RefClk */
  -1,			/* int s3BlankDelay */
  0,			/* int textClockFreq */
  NULL,			/* char* DCConfig */
  NULL,			/* char* DCOptions */
  0			/* int MemClk */
#ifdef XFreeXDGA
  ,0,                   /* int directMode */
  NULL,                 /* Set Vid Page */
  0,                    /* unsigned long physBase */
  0                     /* int physSize */
#endif
};

pointer vgaOrigVideoState = NULL;
pointer vgaNewVideoState = NULL;
pointer vgaBase = NULL;
pointer vgaVirtBase = NULL;
pointer vgaLinearBase = NULL;
pointer vgaLinearOrig = NULL;
vgaPCIInformation *vgaPCIInfo = NULL;
Bool vgaDAC8BitComponents = FALSE;

void (* vgaEnterLeaveFunc)(
#if NeedFunctionPrototypes
    Bool
#endif
) = (void (*)())NoopDDA;
Bool (* vgaInitFunc)(
#if NeedFunctionPrototypes
    DisplayModePtr
#endif
) = (Bool (*)())NoopDDA;
int (* vgaValidModeFunc)(
#if NeedFunctionPrototypes
    DisplayModePtr,
    Bool,
    int
#endif
) = (Bool (*)())NoopDDA;
void * (* vgaSaveFunc)(
#if NeedFunctionPrototypes
    void *
#endif
) = (void *(*)())NoopDDA;
void (* vgaRestoreFunc)(
#if NeedFunctionPrototypes
    void *
#endif
) = (void (*)())NoopDDA;
void (* vgaAdjustFunc)(
#if NeedFunctionPrototypes
    int,
    int
#endif
) = (void (*)())NoopDDA;
void (* vgaSaveScreenFunc)() = vgaHWSaveScreen;
void (* vgaFbInitFunc)() = (void (*)())NoopDDA;
Bool (* vgaScrInitFunc)() = (Bool (*)())NoopDDA;
int (* vgaPitchAdjustFunc)() = (int (*)())NoopDDA;
int (* vgaLinearOffsetFunc)() = (int (*)())NoopDDA;
void (* vgaSetReadFunc)() = (void (*)())NoopDDA;
void (* vgaSetWriteFunc)() = (void (*)())NoopDDA;
void (* vgaSetReadWriteFunc)() = (void (*)())NoopDDA;
int vgaMapSize;
int vgaSegmentSize;
int vgaSegmentShift;
int vgaSegmentMask;
void *vgaReadBottom;
void *vgaReadTop;
void *vgaWriteBottom;
void *vgaWriteTop =    (pointer)&writeseg; /* dummy for linking */
Bool vgaReadFlag;
Bool vgaWriteFlag;
Bool vgaUse2Banks;
int  vgaInterlaceType;
OFlagSet vgaOptionFlags;
extern Bool clgd6225Lcd;
Bool vgaUseLinearAddressing;
int vgaPhysLinearBase;
int vgaLinearSize;
int vgaBitsPerPixel = 0;
int vgaBytesPerPixel = 0;

#ifdef MONOVGA
int vgaReadseg=0;
int vgaWriteseg=0;
int vgaReadWriteseg=0;
#endif

int vgaIOBase;

static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;
static Bool (* saveInitFunc)();
static void * (* saveSaveFunc)();
static void (* saveRestoreFunc)();
static void (* saveAdjustFunc)();
static void (* saveSaveScreenFunc)();
static void (* saveSetReadFunc)();
static void (* saveSetWriteFunc)();
static void (* saveSetReadWriteFunc)();

vgaHWCursorRec vgaHWCursor;

#ifdef MONOVGA
static int validDepth = 1;
#endif
#ifdef XF86VGA16
static int validDepth = 4;
#endif

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern int defaultColorVisualClass;

#define Drivers vgaDrivers

extern vgaVideoChipPtr Drivers[];

/*
 * vgaPrintIdent --
 *     Prints out identifying strings for drivers included in the server
 */
void
vgaPrintIdent()
{
  int            i, j, n, c;
  char		 *id;

#ifdef MONOVGA
  ErrorF("  %s: server for monochrome VGA (Patchlevel %s):\n      ",
         vga256InfoRec.name, vga256InfoRec.patchLevel);
#else
#ifdef XF86VGA16
  ErrorF("  %s: server for 4-bit colour VGA (Patchlevel %s):\n      ",
         vga256InfoRec.name, vga256InfoRec.patchLevel);
#else
  ErrorF("  %s: server for SVGA graphics adaptors (Patchlevel %s):\n      ",
         vga256InfoRec.name, vga256InfoRec.patchLevel);
#endif
#endif
  n = 0;
  c = 0;
  for (i=0; Drivers[i]; i++)
    for (j = 0; id = (Drivers[i]->ChipIdent)(j); j++, n++)
    {
      if (n)
      {
        ErrorF(",");
        c++;
        if (c + 1 + strlen(id) < 70)
        {
          ErrorF(" ");
          c++;
        }
        else
        {
          ErrorF("\n      ");
          c = 0;
        }
      }
      ErrorF("%s", id);
      c += strlen(id);
    }
  ErrorF("\n");
#ifdef PC98
  ErrorF("  PC98: Supported Video Boards:\n\t");
#endif
#ifdef PC98_EGC
  ErrorF("%s\n",PC98_VGA16_BOARDS);
#endif
#ifdef PC98_NEC480
  ErrorF("%s\n",PC98_VGA256_BOARDS);
#endif
#if defined(PC98_WAB)||defined(PC98_WABEP)||defined(PC98_GANB_WAP)||\
	defined(PC98_NKVNEC)||defined(PC98_WSNA)
  ErrorF("%s\n",PC98_CIRRUS_BOARDS);
#endif
#ifdef PC98_TGUI
  ErrorF("%s\n",PC98_TGUI_BOARDS);
#endif
}


/*
 * vgaProbe --
 *     probe and initialize the hardware driver
 */
Bool
vgaProbe()
{
  int            i, j, k;
  DisplayModePtr pMode, pEnd, pmaxX = NULL, pmaxY = NULL;
  int            maxX, maxY;
  int            needmem, rounding;
  int            tx,ty;
  Bool           defaultVisualGiven = FALSE;

  /*
   * At this point xf86bpp is what's given as -bpp on the command
   * line, or what is specified as DefaultColorDepth in the XF86Config
   * file.  The true depth/bits-per-pixel are yet to be determined.
   * At this point, the weights are uninitialized unless specifically
   * given by the user. Here we determine good defaults if weights haven't
   * been explicitly given.
   */

  if(vga256InfoRec.defaultVisual >= 0)
    defaultVisualGiven = TRUE;

  if (xf86bpp <= 0)
#ifdef MONOVGA
    xf86bpp = 1;
#else
#ifdef XF86VGA16
    xf86bpp = 4;
#else
    xf86bpp = 8;
#endif
#endif

  switch (xf86bpp) {
#ifdef MONOVGA
  case 1:
    vga256InfoRec.depth = 1;
    vga256InfoRec.bitsPerPixel = 1;
    if (!defaultVisualGiven)
      vga256InfoRec.defaultVisual = StaticGray;
    xf86weight.red = 1;
    xf86weight.green = 1;
    xf86weight.blue = 1;
    break;
#else
#ifdef XF86VGA16
  case 4:
    vga256InfoRec.depth = 4;
    vga256InfoRec.bitsPerPixel = 8;	/* This is a special case though */
    if (!defaultVisualGiven)
      vga256InfoRec.defaultVisual = StaticColor;
    xf86weight.red = 6;
    xf86weight.green = 6;
    xf86weight.blue = 6;
    break;
#else
  case 8:
    vga256InfoRec.depth = 8;
    vga256InfoRec.bitsPerPixel = 8;
    xf86weight.red = 6;
    xf86weight.green = 6;
    xf86weight.blue = 6;
    break;
  case 15:
    vga256InfoRec.depth = 15;
    vga256InfoRec.bitsPerPixel = 16;
    if (!defaultVisualGiven)
      vga256InfoRec.defaultVisual = TrueColor;
    xf86weight.red = 5;
    xf86weight.green = 5;
    xf86weight.blue = 5;
    break;
  case 16:
    vga256InfoRec.bitsPerPixel = 16;
    if (!defaultVisualGiven)
      vga256InfoRec.defaultVisual = TrueColor;
    if (xf86weight.red && xf86weight.green && xf86weight.blue) {
      vga256InfoRec.depth = xf86weight.red + xf86weight.green +
                            xf86weight.blue;
      if (vga256InfoRec.depth != 15 && vga256InfoRec.depth != 16) {
	ErrorF("\nError: Invalid weight specified for bpp 16: %d%d%d\n",
		xf86weight.red, xf86weight.green, xf86weight.blue);
	return FALSE;
      }
    } else {
      vga256InfoRec.depth = 16;
      xf86weight.red = 5;
      xf86weight.green = 6;
      xf86weight.blue = 5;
    }
    break;
  case 24:
    vga256InfoRec.depth = 24;
    vga256InfoRec.bitsPerPixel = 24;
    if (!defaultVisualGiven)
      vga256InfoRec.defaultVisual = TrueColor;
    xf86weight.red = 8;
    xf86weight.green = 8;
    xf86weight.blue = 8;
    break;
  case 32:
    vga256InfoRec.depth = 24;
    vga256InfoRec.bitsPerPixel = 32;
    if (!defaultVisualGiven)
      vga256InfoRec.defaultVisual = TrueColor;
    xf86weight.red = 8;
    xf86weight.green = 8;
    xf86weight.blue = 8;
    break;
#endif
#endif
  default:
    ErrorF("\nError: Unsupported bpp \"%i\"\n", xf86bpp);
    return FALSE;
  }
    
#if !defined(MONOVGA) && !defined(XF86VGA16)
  vgaBitsPerPixel = vga256InfoRec.bitsPerPixel;
  vgaBytesPerPixel = vga256InfoRec.bitsPerPixel >> 3;
#endif

  if (defaultColorVisualClass < 0)
    defaultColorVisualClass = vga256InfoRec.defaultVisual;

  switch (vga256InfoRec.depth) {
  case 1:
    if (defaultColorVisualClass != StaticGray) {
      ErrorF("\nError: Visual \"%s\" isn't supported at depth %d\n",
	     xf86VisualNames[defaultColorVisualClass], vga256InfoRec.depth);
      return FALSE;
    }
  case 4:
    switch (defaultColorVisualClass) {
    case StaticGray:
    case GrayScale:
    case StaticColor:
    case PseudoColor:
      break; /* these are OK */
    default:
      ErrorF("\nError: Visual \"%s\" isn't supported at depth %d\n",
	     xf86VisualNames[defaultColorVisualClass],
	     vga256InfoRec.depth);
      return FALSE;
    }
    break;
  case 8:
    break; /* all visuals supported at 8bpp */
  default:
    if (defaultColorVisualClass != TrueColor) {
      ErrorF("\nError: Visual \"%s\" isn't supported at depth %d\n",
	      xf86VisualNames[defaultColorVisualClass],
	      vga256InfoRec.depth);
      return FALSE;
    }
  }
      
#if !defined(PC98) || defined(PC98_TGUI)
  /* First do a general PCI probe (unless disabled) */
  if (!OFLG_ISSET(OPTION_NO_PCI_PROBE, &vga256InfoRec.options)) {
    vgaPCIInfo = vgaGetPCIInfo();
  }
#endif

  for (i=0; Drivers[i]; i++)
  {
    vgaSaveScreenFunc = Drivers[i]->ChipSaveScreen;
    if ((Drivers[i]->ChipProbe)())
      {
        xf86ProbeFailed = FALSE;
#ifdef MONOVGA
#ifdef BANKEDMONOVGA
        /*
         * In the mono mode we use, the memory is divided into 4 planes
         * so we can only effectively use 1/4 of the total.  For cards
         * with < 256K there should be fewer planes.
         */
        if (vga256InfoRec.videoRam <= 256 || !vga256InfoRec.bankedMono)
            needmem = Drivers[i]->ChipSegmentSize * 8;
        else
	    needmem = vga256InfoRec.videoRam / 4 * 1024 * 8;
#else /* BANKEDMONOVGA */
	needmem = Drivers[i]->ChipSegmentSize * 8;
#endif /* BANKEDMONOVGA */
	rounding = 32;
#else /* MONOVGA */
#ifdef XF86VGA16
#ifdef PC98_EGC
	needmem = Drivers[i]->ChipSegmentSize * 8;
#else
	needmem = vga256InfoRec.videoRam / 4 * 1024 * 8;
#endif
	rounding = 32;
#else
	needmem = vga256InfoRec.videoRam * 1024;
	rounding = Drivers[i]->ChipRounding;
	/*
	 * Correct 'needmem' for 16/32bpp.
	 * For 8/16/32bpp, needmem is defined as the max. number of pixels.
	 */
	needmem /= (vgaBitsPerPixel / 8);

#endif /* XF86VGA16 */
#endif /* MONOVGA */

        if (xf86Verbose)
        {
	  ErrorF("%s %s: chipset:  %s\n",
             OFLG_ISSET(XCONFIG_CHIPSET,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
             vga256InfoRec.name,
             vga256InfoRec.chipset);
#ifdef MONOVGA
	  ErrorF("%s %s: videoram: %dk (using %dk)",
#else
#ifdef XF86VGA16
	  ErrorF("%s %s: videoram: %dk (using %dk)",
#else
	  ErrorF("%s %s: videoram: %dk",
#endif
#endif
		 OFLG_ISSET(XCONFIG_VIDEORAM,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
                 vga256InfoRec.name,
	         vga256InfoRec.videoRam
#ifdef MONOVGA
                 , needmem / 8 / 1024
#endif
#ifdef XF86VGA16
                 , needmem / 2 / 1024
#endif
	         );
	

	  for (j=0; j < vga256InfoRec.clocks; j++)
	  {
	    if ((j % 8) == 0)
	      ErrorF("\n%s %s: clocks:", 
                OFLG_ISSET(XCONFIG_CLOCKS,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
                vga256InfoRec.name);
	    ErrorF(" %6.2f", (double)vga256InfoRec.clock[j]/1000.0);
	  }
	  ErrorF("\n");
        }

	/* Scale raw clocks to give pixel clocks if driver requires it */
	if ((Drivers[i]->ChipClockMulFactor > 1)
	 || (Drivers[i]->ChipClockDivFactor > 1)) {
	  for (j = 0; j < vga256InfoRec.clocks; j++) {
	    vga256InfoRec.clock[j] *= Drivers[i]->ChipClockDivFactor;
	    vga256InfoRec.clock[j] /= Drivers[i]->ChipClockMulFactor;
	  }
	  vga256InfoRec.maxClock *= Drivers[i]->ChipClockDivFactor;
	  vga256InfoRec.maxClock /= Drivers[i]->ChipClockMulFactor;
	  if (xf86Verbose) {
	    ErrorF("%s %s: Effective pixel clocks available:\n",
		   XCONFIG_PROBED, vga256InfoRec.name);
	    for (j=0; j < vga256InfoRec.clocks; j++)
	    {
	      if ((j % 8) == 0)
	      {
		if (j != 0)
		  ErrorF("\n");
	        ErrorF("%s %s: pixel clocks:", XCONFIG_PROBED,
		       vga256InfoRec.name);
	      }
	      ErrorF(" %6.2f", (double)vga256InfoRec.clock[j] / 1000.0);
	    }
	    ErrorF("\n");
	  }
	}

#if !defined(MONOVGA) && !defined(XF86VGA16)
        /*
         * If bpp is > 8, make sure the driver supports it.
         */
        if ((vgaBitsPerPixel == 16 && !Drivers[i]->ChipHas16bpp)
        || (vgaBitsPerPixel == 24 && !Drivers[i]->ChipHas24bpp)
        || (vgaBitsPerPixel == 32 && !Drivers[i]->ChipHas32bpp)) {
            ErrorF("\n%s %s: %dbpp not supported for this chipset\n",
                XCONFIG_GIVEN, vga256InfoRec.name, vgaBitsPerPixel);
	    Drivers[i]->ChipEnterLeave(LEAVE);
	    return(FALSE);
        }
#endif

	vgaEnterLeaveFunc = Drivers[i]->ChipEnterLeave;
	vgaInitFunc = Drivers[i]->ChipInit;
	vgaValidModeFunc = Drivers[i]->ChipValidMode;
	vgaSaveFunc = Drivers[i]->ChipSave;
	vgaRestoreFunc = Drivers[i]->ChipRestore;
	vgaAdjustFunc = Drivers[i]->ChipAdjust;
	vgaFbInitFunc = Drivers[i]->ChipFbInit;
#ifndef PC98_EGC
	vgaSetReadFunc = Drivers[i]->ChipSetRead;
	vgaSetWriteFunc = Drivers[i]->ChipSetWrite;
	vgaSetReadWriteFunc = Drivers[i]->ChipSetReadWrite;
#endif		 
	vgaMapSize = Drivers[i]->ChipMapSize;
	vgaSegmentSize = Drivers[i]->ChipSegmentSize;
	vgaSegmentShift = Drivers[i]->ChipSegmentShift;
	vgaSegmentMask = Drivers[i]->ChipSegmentMask;
	vgaReadBottom = (pointer)((unsigned long)Drivers[i]->ChipReadBottom);
	vgaReadTop = (pointer)((unsigned long)Drivers[i]->ChipReadTop);
	vgaWriteBottom = (pointer)((unsigned long)Drivers[i]->ChipWriteBottom);
	vgaWriteTop = (pointer)((unsigned long)Drivers[i]->ChipWriteTop);
	vgaUse2Banks = Drivers[i]->ChipUse2Banks;
	vgaInterlaceType = Drivers[i]->ChipInterlaceType;
	vgaOptionFlags = Drivers[i]->ChipOptionFlags;
	OFLG_SET(OPTION_POWER_SAVER, &vgaOptionFlags);
	OFLG_SET(OPTION_CLGD6225_LCD, &vgaOptionFlags);
	OFLG_SET(OPTION_NO_PCI_PROBE, &vgaOptionFlags);
	OFLG_SET(OPTION_CLKDIV2, &vgaOptionFlags);
	/* For XAA */
	OFLG_SET(OPTION_XAA_BENCHMARK, &vgaOptionFlags);
	OFLG_SET(OPTION_XAA_NO_COL_EXP, &vgaOptionFlags);
	OFLG_SET(OPTION_NO_PIXMAP_CACHE, &vgaOptionFlags);

	xf86VerifyOptions(&vgaOptionFlags, &vga256InfoRec);

#if !defined(MONOVGA) && !defined(XF86VGA16)
	/* We do this after the option has been verified. */
	if (vgaBitsPerPixel == 8
	&& OFLG_ISSET(OPTION_DAC_8_BIT, &vga256InfoRec.options)) {
            ErrorF("%s %s: Using 8 bits per color component\n",
                XCONFIG_GIVEN, vga256InfoRec.name);
	    vgaDAC8BitComponents = TRUE;
	    xf86weight.red = xf86weight.green = xf86weight.blue = 8;
	}
	vga256InfoRec.blackColour.red = 0;
	vga256InfoRec.blackColour.green = 0;
	vga256InfoRec.blackColour.blue = 0;

	vga256InfoRec.weight.red = xf86weight.red;
	vga256InfoRec.weight.green = xf86weight.green;
	vga256InfoRec.weight.blue = xf86weight.blue;

	vga256InfoRec.whiteColour.red =  (0x0001 << xf86weight.red) - 1;
	vga256InfoRec.whiteColour.green = (0x0001 << xf86weight.green) - 1;
	vga256InfoRec.whiteColour.blue = (0x0001 << xf86weight.blue) - 1;

	ErrorF("%s %s: Using %d bpp, Depth %d, Color weight: %d%d%d\n",
		XCONFIG_GIVEN, vga256InfoRec.name, vgaBitsPerPixel,
		vga256InfoRec.depth, vga256InfoRec.weight.red,
		vga256InfoRec.weight.green, vga256InfoRec.weight.blue);
#endif

#ifdef DPMSExtension
	if (DPMSEnabledSwitch ||
	    (OFLG_ISSET(OPTION_POWER_SAVER, &vga256InfoRec.options) &&
	     !DPMSDisabledSwitch))
	    defaultDPMSEnabled = DPMSEnabled = TRUE;
#endif

	if (OFLG_ISSET(OPTION_CLGD6225_LCD, &vga256InfoRec.options))
	    clgd6225Lcd = TRUE;


	/* if Virtual given: is the virtual size too big? */
#ifdef BANKEDMONOVGA
	if (vga256InfoRec.virtualX > (2048)) {
 		ErrorF("%s: Virtual width %i exceeds max. virtual width %i\n",
		       vga256InfoRec.name, vga256InfoRec.virtualX, (2048));
		vgaEnterLeaveFunc(LEAVE);
		return(FALSE);
	}
	vga256InfoRec.displayWidth = ((vga256InfoRec.virtualX + rounding - 1) /
		rounding) * rounding;
	if (vga256InfoRec.displayWidth*vga256InfoRec.virtualY <= 8*vgaSegmentSize)
	{	/* may be unbanked */
		if (vga256InfoRec.displayWidth > vga256InfoRec.virtualX)
			ErrorF("%s %s: Display width set to %d (a multiple of"
			       " %d)\n", XCONFIG_PROBED, vga256InfoRec.name,
			       vga256InfoRec.displayWidth, rounding);
	}
	else if (vga256InfoRec.virtualX > (1024))
		vga256InfoRec.displayWidth=2048;
	     else vga256InfoRec.displayWidth=1024;

	if (vga256InfoRec.virtualX > 0 &&
	    vga256InfoRec.displayWidth * vga256InfoRec.virtualY > needmem)
	  {
	    ErrorF("%s: Too little memory for virtual resolution\n"
                   "      %d (display width %d) x %d\n",
                   vga256InfoRec.name, vga256InfoRec.virtualX,
                   vga256InfoRec.displayWidth, vga256InfoRec.virtualY);
            vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	  }
#else
#ifndef USE_OLD_ROUNDING
	if (vga256InfoRec.virtualX > 0) {
	  /* Let the driver have a chance to use a larger screen pitch if
	     it needs.  This ability is particularly useful for the Cirrus
	     Laguna family, which uses tiled memory.  Each scanline must be
	     an integer number of tiles wide, where tiles are 128 or 256
	     bytes wide.  Furthermore, there are only a few tile pitches
	     allowed.  It just so happens that none of these tile pitches
	     yield a screen byte pitch that is divisible by 3.  The upshot:
	     you can't have 24bpp if the screen pitch is not a multiple of 
	     three -- i.e., if the screen pitch is not an integer number
	     of _pixels_. */
	  if (vgaPitchAdjustFunc != (int (*)())NoopDDA)
	    vga256InfoRec.displayWidth = (*vgaPitchAdjustFunc)();
          else {
	    if (vga256InfoRec.virtualX % rounding) {
	      vga256InfoRec.displayWidth = vga256InfoRec.virtualX +
					   (rounding -
				           (vga256InfoRec.virtualX % rounding));
	      ErrorF("%s %s: Display width set to %d (a multiple of %d)\n",
	             XCONFIG_PROBED, vga256InfoRec.name,
		     vga256InfoRec.displayWidth, rounding);
	    } else {
	      vga256InfoRec.displayWidth = vga256InfoRec.virtualX;
	    }
	  }
	  if (vga256InfoRec.displayWidth * vga256InfoRec.virtualY > needmem)
	  {
	    ErrorF("%s: Too little memory for virtual resolution %d %d\n",
                   vga256InfoRec.name, vga256InfoRec.virtualX,
                   vga256InfoRec.virtualY);
            vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	  }
	}
#else
	if (vga256InfoRec.virtualX > 0 &&
	    vga256InfoRec.virtualX * vga256InfoRec.virtualY > needmem)
	  {
	    ErrorF("%s: Too little memory for virtual resolution %d %d\n",
                   vga256InfoRec.name, vga256InfoRec.virtualX,
                   vga256InfoRec.virtualY);
            vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	  }
#endif
#endif

        maxX = maxY = -1;
	tx = vga256InfoRec.virtualX;
	ty = vga256InfoRec.virtualY;

        if (Drivers[i]->ChipBuiltinModes) {
          pEnd = pMode = vga256InfoRec.modes = Drivers[i]->ChipBuiltinModes;
          ErrorF("%s %s: Using builtin driver modes\n", XCONFIG_PROBED,
                 vga256InfoRec.name);
	  do {
            ErrorF("%s %s: Builtin Mode: %s\n", XCONFIG_PROBED,
                   vga256InfoRec.name, pMode->name);
	    if (pMode->HDisplay > maxX)
	    {
	      maxX = pMode->HDisplay;
	      pmaxX = pMode;
	    }
	    if (pMode->VDisplay > maxY)
	    {
	      maxY = pMode->VDisplay;
	      pmaxY = pMode;
	    }
            pMode = pMode->next;
          } while (pMode != pEnd);
        } else if (vga256InfoRec.modes == NULL) {
          ErrorF("No modes supplied in XF86Config\n");
	  vgaEnterLeaveFunc(LEAVE);
	  return(FALSE);
        } else {
	  pMode = vga256InfoRec.modes;
	  pEnd = (DisplayModePtr) NULL;
	  do {
	    DisplayModePtr pModeSv;

	    /*
	     * xf86LookupMode returns FALSE if it ran into an invalid
	     * parameter
	     */
	    if(xf86LookupMode(pMode, &vga256InfoRec, LOOKUP_DEFAULT) == FALSE) {
	      pModeSv = pMode->next;
	      xf86DeleteMode(&vga256InfoRec, pMode);
	      pMode = pModeSv; 
	    } else if (pMode->HDisplay * pMode->VDisplay > needmem) {
	      pModeSv=pMode->next;
	      ErrorF("%s %s: Insufficient video memory for all resolutions\n",
		     XCONFIG_PROBED, vga256InfoRec.name);
	      xf86DeleteMode(&vga256InfoRec, pMode);
	      pMode = pModeSv;
	    } else if (((tx > 0) && (pMode->HDisplay > tx)) || 
		       ((ty > 0) && (pMode->VDisplay > ty))) {
	      pModeSv=pMode->next;
	      ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
		     XCONFIG_PROBED, vga256InfoRec.name,
		     pMode->HDisplay, pMode->VDisplay, tx, ty);
	      xf86DeleteMode(&vga256InfoRec, pMode);
	      pMode = pModeSv;
	    } else {
	      /*
	       * Successfully looked up this mode.  If pEnd isn't 
	       * initialized, set it to this mode.
	       */
	      if (pEnd == (DisplayModePtr) NULL)
		pEnd = pMode;

	      if (pMode->HDisplay > maxX)
	      {
		maxX = pMode->HDisplay;
		pmaxX = pMode;
	      }
	      if (pMode->VDisplay > maxY)
	      {
		maxY = pMode->VDisplay;
		pmaxY = pMode;
	      }

	      pMode = pMode->next;
	    }
	  } while (pMode != pEnd);
        }

        vga256InfoRec.virtualX = max(maxX, vga256InfoRec.virtualX);
        vga256InfoRec.virtualY = max(maxY, vga256InfoRec.virtualY);
#ifdef BANKEDMONOVGA
	if (vga256InfoRec.virtualX > (2048)) {
		ErrorF("%s: Max. width %i exceeds max. virtual width %i\n",
			vga256InfoRec.name, vga256InfoRec.virtualX, (2048));
		vgaEnterLeaveFunc(LEAVE);
		return(FALSE);
	}
	/* Now that modes are resolved and max. extents are found,
	 * test size again */
	vga256InfoRec.displayWidth = ((vga256InfoRec.virtualX + rounding - 1) /
		rounding) * rounding;
	if (vga256InfoRec.displayWidth*vga256InfoRec.virtualY > 8*vgaSegmentSize)
	  {
             if (vga256InfoRec.virtualX > (1024))
		vga256InfoRec.displayWidth=2048;
	     else vga256InfoRec.displayWidth=1024;
          }
	if ((xf86Verbose) &&
	    (vga256InfoRec.displayWidth != vga256InfoRec.virtualX))
		ErrorF("%s %s: Display width set to %d (a multiple of %d)\n",
			XCONFIG_PROBED, vga256InfoRec.name,
		        vga256InfoRec.displayWidth, rounding);
#endif

#ifndef BANKEDMONOVGA
#ifndef USE_OLD_ROUNDING
	if (vga256InfoRec.displayWidth < 0) {
	  /* Let the driver have a chance to use a larger screen pitch if
	     it needs.  This ability is particularly useful for the Cirrus
	     Laguna family, which uses tiled memory.  Each scanline must be
	     an integer number of tiles wide, where tiles are 128 or 256
	     bytes wide.  Furthermore, there are only a few tile pitches
	     allowed.  It just so happens that none of these tile pitches
	     yield a screen byte pitch that is divisible by 3.  The upshot:
	     you can't have 24bpp if the screen pitch is not a multiple of 
	     three -- i.e., if the screen pitch is not an integer number
	     of _pixels_. */
	  if (vgaPitchAdjustFunc != (int (*)())NoopDDA)
	    vga256InfoRec.displayWidth = (*vgaPitchAdjustFunc)();
          else {
	    if (vga256InfoRec.virtualX % rounding) {
	      vga256InfoRec.displayWidth = vga256InfoRec.virtualX + (rounding -
				           (vga256InfoRec.virtualX % rounding));
	      ErrorF("%s %s: Display width set to %d (a multiple of %d)\n",
	             XCONFIG_PROBED, vga256InfoRec.name,
		     vga256InfoRec.displayWidth, rounding);
	    } else {
	      vga256InfoRec.displayWidth = vga256InfoRec.virtualX;
	    }
	  }
	  if (vga256InfoRec.displayWidth * vga256InfoRec.virtualY > needmem)
	  {
	    ErrorF("%s: Too little memory for virtual resolution %d %d\n",
                   vga256InfoRec.name, vga256InfoRec.virtualX,
                   vga256InfoRec.virtualY);
            vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	  }
	}
#else
	if (vga256InfoRec.virtualX % rounding)
	  {
	    vga256InfoRec.virtualX -= vga256InfoRec.virtualX % rounding;
	    ErrorF(
	     "%s %s: Virtual width rounded down to a multiple of %d (%d)\n",
	     XCONFIG_PROBED, vga256InfoRec.name, rounding,
	     vga256InfoRec.virtualX);
            if (vga256InfoRec.virtualX < maxX)
            {
              ErrorF(
               "%s: Rounded down virtual width (%d) is too small for mode %s",
	       vga256InfoRec.name, vga256InfoRec.virtualX, pmaxX->name);
              vgaEnterLeaveFunc(LEAVE);
              return(FALSE);
            }
	  }

	if ( vga256InfoRec.virtualX * vga256InfoRec.virtualY > needmem)
	{
          if (vga256InfoRec.virtualX != maxX ||
              vga256InfoRec.virtualY != maxY)
	    ErrorF(
              "%s: Too little memory to accomodate virtual size and mode %s\n",
               vga256InfoRec.name,
               (vga256InfoRec.virtualX == maxX) ? pmaxX->name : pmaxY->name);
          else
	    ErrorF("%s: Too little memory to accomodate modes %s and %s\n",
                   vga256InfoRec.name, pmaxX->name, pmaxY->name);
          vgaEnterLeaveFunc(LEAVE);
	  return(FALSE);
	}
#endif
#else
	if ( vga256InfoRec.displayWidth * vga256InfoRec.virtualY > needmem) {
		ErrorF("%s: Too little memory to accomodate display width %i"
			" and virtual height %i\n",
			vga256InfoRec.name,
			vga256InfoRec.displayWidth, vga256InfoRec.virtualY);
		vgaEnterLeaveFunc(LEAVE);
		return(FALSE);
	}
#endif
	if ((tx != vga256InfoRec.virtualX) || (ty != vga256InfoRec.virtualY))
            OFLG_CLR(XCONFIG_VIRTUAL,&vga256InfoRec.xconfigFlag);

#ifdef USE_OLD_ROUNDING
#ifndef BANKEDMONOVGA
	/* Let the driver have a chance to use a larger screen pitch if
	   it needs.  This ability is particularly useful for the Cirrus
	   Laguna family, which uses tiled memory.  Each scanline must be
	   an integer number of tiles wide, where tiles are 128 or 256
	   bytes wide.  Furthermore, there are only a few tile pitches
	   allowed.  It just so happens that none of these tile pitches
	   yield a screen byte pitch that is divisible by 3.  The upshot:
	   you can't have 24bpp if the screen pitch is not a multiple of 
	   three -- i.e., if the screen pitch is not an integer number
	   of _pixels_. */

	if (vgaPitchAdjustFunc != (int (*)())NoopDDA)
	  vga256InfoRec.displayWidth = (*vgaPitchAdjustFunc)();
        else
          vga256InfoRec.displayWidth = vga256InfoRec.virtualX;
#endif
#endif
        if (xf86Verbose)
          ErrorF("%s %s: Virtual resolution set to %dx%d\n",
                 OFLG_ISSET(XCONFIG_VIRTUAL,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
                 vga256InfoRec.name,
                 vga256InfoRec.virtualX, vga256InfoRec.virtualY);

#if !defined(XF86VGA16)
#if !defined(MONOVGA)
	if ((vga256InfoRec.speedup & ~SPEEDUP_ANYWIDTH) &&
            vga256InfoRec.displayWidth != 1024)
	  {
	    ErrorF(
              "%s %s: SpeedUp code selection modified because virtualX != 1024\n",
              XCONFIG_PROBED, vga256InfoRec.name);
	    vga256InfoRec.speedup &= SPEEDUP_ANYWIDTH;
	    OFLG_CLR(XCONFIG_SPEEDUP,&vga256InfoRec.xconfigFlag);
	  }

        /*
         * Currently the 16bpp and 32bpp modes use stock cfb with linear
         * addressing, so avoid the SpeedUp code for these depths.
         */
      if (vgaBitsPerPixel == 8) {

        useSpeedUp = vga256InfoRec.speedup & SPEEDUP_ANYCHIPSET;
        if (useSpeedUp && xf86Verbose)
          ErrorF("%s %s: Generic SpeedUps selected (Flags=0x%x)\n",
               OFLG_ISSET(XCONFIG_SPEEDUP,&vga256InfoRec.xconfigFlag) ?
                    XCONFIG_GIVEN : XCONFIG_PROBED , 
               vga256InfoRec.name, useSpeedUp);

        /* We deal with the generic speedups here */
	if (useSpeedUp & SPEEDUP_TEGBLT8)
	{
#if !defined(__alpha__) && !defined(__arm32__)
	  vga256LowlevFuncs.teGlyphBlt8 = speedupvga256TEGlyphBlt8;
	  vga256TEOps1Rect.ImageGlyphBlt = speedupvga256TEGlyphBlt8;
	  vga256TEOps.ImageGlyphBlt = speedupvga256TEGlyphBlt8;
#else /* !__alpha__ && !__arm32__ */
	  vga256LowlevFuncs.teGlyphBlt8 = vga256TEGlyphBlt8;
	  vga256TEOps1Rect.ImageGlyphBlt = vga256TEGlyphBlt8;
	  vga256TEOps.ImageGlyphBlt = vga256TEGlyphBlt8;
#endif /* !__alpha__ && !__arm32__ */
	}

	if (useSpeedUp & SPEEDUP_RECTSTIP)
	{
#if !defined(__alpha__) && !defined(__arm32__)
	  vga256LowlevFuncs.fillRectOpaqueStippled32 = 
	    speedupvga2568FillRectOpaqueStippled32;
	  vga256LowlevFuncs.fillRectTransparentStippled32 = 
	    speedupvga2568FillRectTransparentStippled32;
#else /* !__alpha__ && !__arm32__ */
	  vga256LowlevFuncs.fillRectOpaqueStippled32 = 
	    vga2568FillRectOpaqueStippled32;
	  vga256LowlevFuncs.fillRectTransparentStippled32 = 
	    vga2568FillRectTransparentStippled32;
#endif /* !__alpha__ && !__arm32__ */
	}

	if (!vgaUse2Banks)
	{
	  vga256LowlevFuncs.vgaBitblt = OneBankvgaBitBlt;
	}

      } /* endif vgaBitsPerPixel == 8 */

	/* Initialise chip-specific enhanced fb functions */
	vgaHWCursor.Initialized = FALSE;
	(*vgaFbInitFunc)();

	/* The driver should now have determined whether linear */
	/* addressing is possible */
	vgaUseLinearAddressing = Drivers[i]->ChipUseLinearAddressing;
	vgaPhysLinearBase = Drivers[i]->ChipLinearBase;
	vgaLinearSize = Drivers[i]->ChipLinearSize;

#ifdef XFreeXDGA
	if (vgaUseLinearAddressing) {
	    vga256InfoRec.physBase = vgaPhysLinearBase;
	    vga256InfoRec.physSize = vga256InfoRec.videoRam * 1024;
	} else {
	    vga256InfoRec.physBase = 0xA0000 + Drivers[i]->ChipWriteBottom;
	    vga256InfoRec.physSize = Drivers[i]->ChipSegmentSize;
	    vga256InfoRec.setBank = vgaSetVidPage;
	}
#endif

	/* Currently linear addressing is required for 16/32bpp. */
	/* Bail out if it is not enabled. */
	if (vgaBitsPerPixel > 8 && !vgaUseLinearAddressing) {
 	    ErrorF("%s: Linear addressing is required for %dbpp\n",
 	    vga256InfoRec.name, vgaBitsPerPixel);
	    vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	}

#else
#ifdef BANKEDMONOVGA
	if (vgaUse2Banks)
	{
	  ourmfbDoBitbltCopy = mfbDoBitbltTwoBanksCopy;
	  ourmfbDoBitbltCopyInverted = mfbDoBitbltTwoBanksCopyInverted;
	}
	else
	{
	  ourmfbDoBitbltCopy = mfbDoBitbltCopy;
	  ourmfbDoBitbltCopyInverted = mfbDoBitbltCopyInverted;
	}
#else
	ourmfbDoBitbltCopy = mfbDoBitbltTwoBanksCopy;
	ourmfbDoBitbltCopyInverted = mfbDoBitbltTwoBanksCopyInverted;
#endif /* BANKEDMONOVGA */
#endif /* !MONOVGA */
#endif /* !XF86VGA16 */

#if !defined(PC98) || defined(PC98_TGUI)
	if (!OFLG_ISSET(OPTION_NO_PCI_PROBE, &vga256InfoRec.options)) {
	  /* Free PCI information */
	  xf86cleanpci();
	  if (vgaPCIInfo) {
	    vgaPCIInfo->ThisCard = (pciConfigPtr)NULL;
	    vgaPCIInfo->AllCards = (pciConfigPtr *)NULL;
	  }
	}
#endif

	return TRUE;
      }
  }

  vgaSaveScreenFunc = vgaHWSaveScreen;

  /* Free PCI information */
  xf86cleanpci();
  
  if (vga256InfoRec.chipset)
    ErrorF("%s: '%s' is an invalid chipset", vga256InfoRec.name,
	       vga256InfoRec.chipset);
  return FALSE;
}


/* Allow each driver to optionally set a display pitch other than the
   (default) virtualX size.  This functionality was added by Corin
   Anderson while he was bringing up support for 24bpp and the Laguna 
   family of Cirrus Logic chips. */
void vgaSetPitchAdjustHook(int (* ChipPitchAdjust)())
{
  vgaPitchAdjustFunc = ChipPitchAdjust;
}


/* Allow each driver to optionally set an offset of the
   frame buffer data from the linear base address. */
void vgaSetLinearOffsetHook(int (* ChipLinearOffset)())
{
  vgaLinearOffsetFunc = ChipLinearOffset;
}


/* Allow each driver to hook the ScreenInit function */
void vgaSetScreenInitHook(Bool (* ChipScrInit)())
{
  vgaScrInitFunc = ChipScrInit;
}


/*
 * vgaScreenInit --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
vgaScreenInit (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
  int displayResolution = 75;    /* default to 75dpi */
  extern int monitorResolution;

  if (serverGeneration == 1) {
#if defined(PC98_WAB) || defined(PC98_WABEP)
    vgaBase = xf86MapVidMem(scr_index, VGA_REGION, (pointer)0xE0000,
			    vgaMapSize);
#else
#if defined(PC98_GANB_WAP) || defined(PC98_WSNA) || defined(PC98_NKVNEC)
    vgaBase = xf86MapVidMem(scr_index, VGA_REGION, (pointer)0xF00000,
			    vgaMapSize);
#else
#if defined(PC98_TGUI)
    if(!vgaUseLinearAddressing && pc98PvramBase != NULL)
      vgaBase = xf86MapVidMem(scr_index, VGA_REGION,
			      pc98PvramBase, vgaMapSize);
#else
#if defined(PC98_EGC) || defined(PC98_NEC480)
    vgaBase = xf86MapVidMem(scr_index, VGA_REGION, (pointer)0xA8000,
			    vgaMapSize);
#else    
    vgaBase = xf86MapVidMem(scr_index, VGA_REGION, (pointer)0xA0000,
			    vgaMapSize);
#endif /* PC98_EGC || PC98_NE480 */
#endif /* PC98_TGUI */
#endif /* PC98_GANB_WAP || PC98_WSNA || PC98_NKVNEC */
#endif /* PC98_WAB || PC98_WABEP */
    if (vgaUseLinearAddressing) {
        vgaLinearBase = xf86MapVidMem(scr_index, LINEAR_REGION,
        			      (pointer)
				       ((unsigned long)vgaPhysLinearBase),
        			      vgaLinearSize);

        vgaLinearOrig = vgaLinearBase; /* save copy of original base */
    }
#ifdef MONOVGA
    if (vga256InfoRec.displayWidth * vga256InfoRec.virtualY >= vgaSegmentSize * 8)
    {                                                     /* ^ mfb bug */
      ErrorF("%s %s: Using banked mono vga mode\n", 
          XCONFIG_PROBED, vga256InfoRec.name);
      vgaVirtBase = (pointer)VGABASE;
    }
    else
    {
      ErrorF("%s %s: Using non-banked mono vga mode\n", 
          XCONFIG_PROBED, vga256InfoRec.name);
      vgaVirtBase = vgaBase;
    }
#else
#ifdef XF86VGA16
    vgaVirtBase = vgaBase;
#else
    vgaVirtBase = (pointer)VGABASE;
#endif
#endif

    /* This doesn't mean anything yet for SVGA 16/32bpp (no banking). */
    if (vgaUseLinearAddressing)
    {
      if (vgaLinearOffsetFunc != (int (*)())NoopDDA)
	vgaLinearBase = (void *)((unsigned long)vgaLinearBase
				 + (*vgaLinearOffsetFunc)());
      vgaReadBottom = vgaLinearBase;
      vgaReadTop = (void *)((unsigned int)vgaLinearSize
      			    + (unsigned long)vgaLinearBase);
      vgaWriteBottom = vgaLinearBase;
      vgaWriteTop = (void *)((unsigned int)vgaLinearSize
      			    + (unsigned long)vgaLinearBase);
      vgaSegmentSize = vgaLinearSize;	/* override */
      vgaSegmentMask = vgaLinearSize - 1;
      vgaSetReadFunc = (void (*)())NoopDDA;
      vgaSetWriteFunc = (void (*)())NoopDDA;
      vgaSetReadWriteFunc = (void (*)())NoopDDA;
    }
    else
    {
      vgaReadBottom  = (void *)((unsigned long)vgaReadBottom
			        + (unsigned long)vgaBase); 
      vgaReadTop     = (void *)((unsigned long)vgaReadTop
			        + (unsigned long)vgaBase); 
      vgaWriteBottom = (void *)((unsigned long)vgaWriteBottom
			        + (unsigned long)vgaBase); 
      vgaWriteTop    = (void *)((unsigned long)vgaWriteTop
			        + (unsigned long)vgaBase); 
    }
  }

  if (!(*vgaInitFunc)(vga256InfoRec.modes))
    FatalError("%s: hardware initialisation failed\n", vga256InfoRec.name);


  /*
   * This function gets called while in graphics mode during a server
   * reset, and this causes the original video state to be corrupted.
   * So, only initialise vgaOrigVideoState if it hasn't previously been done
   * DHD Dec 1991.
   */
  if (!vgaOrigVideoState)
    vgaOrigVideoState = (pointer)(*vgaSaveFunc)(vgaOrigVideoState);
  (*vgaRestoreFunc)(vgaNewVideoState);
#ifndef DIRTY_STARTUP
  vgaSaveScreen(NULL, FALSE); /* blank the screen */
#endif
  (*vgaAdjustFunc)(vga256InfoRec.frameX0, vga256InfoRec.frameY0);

  /*
   * Take display resolution from the -dpi flag if specified
   */

  if (monitorResolution)
    displayResolution = monitorResolution;

  /*
   * Inititalize the dragon to color display
   */
#ifndef XF86VGA16
#ifdef MONOVGA
  if (!mfbScreenInit(pScreen,
		     (pointer) vgaVirtBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth))
#else
  xf86AccelInfoRec.ServerInfoRec = &vga256InfoRec;
  if (vgaBitsPerPixel == 8)
      if (!xf86XAAScreenInitvga256(pScreen,
		     (pointer) vgaVirtBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth))
          return(FALSE);
  if (vgaBitsPerPixel == 16)
      if (!xf86XAAScreenInit16bpp(pScreen,
		     vgaLinearBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth))
          return(FALSE);
  if (vgaBitsPerPixel == 24)
      if (!xf86XAAScreenInit24bpp(pScreen,
		     vgaLinearBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth))
          return(FALSE);
  if (vgaBitsPerPixel == 32)
      if (!xf86XAAScreenInit32bpp(pScreen,
		     vgaLinearBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth))
#endif
    return(FALSE);
#else /* XF86VGA16 */
  Init16Output(pScreen,
		     (pointer) vgaVirtBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth);
#endif /* XF86VGA16 */

  pScreen->CloseScreen = vgaCloseScreen;
  pScreen->SaveScreen = vgaSaveScreen;
  pScreen->whitePixel = 1;
  pScreen->blackPixel = 0;
  XF86FLIP_PIXELS();
#ifndef MONOVGA
  /* For 16/32bpp, the cfb defaults are OK. */
  if (vgaBitsPerPixel <= 8) { /* For 8bpp SVGA and VGA16 */
      pScreen->InstallColormap = vgaInstallColormap;
      pScreen->UninstallColormap = vgaUninstallColormap;
      pScreen->ListInstalledColormaps = vgaListInstalledColormaps;
      pScreen->StoreColors = vgaStoreColors;
  }

  /* Let each chip driver have a whack at hooking functions, etc. */
  if (!(*vgaScrInitFunc)(pScreen,
			 vgaLinearBase,
			 vga256InfoRec.virtualX,
			 vga256InfoRec.virtualY,
			 displayResolution, displayResolution,
			 vga256InfoRec.displayWidth))
    return FALSE;

#endif
  
  if (vgaHWCursor.Initialized)
  {
    xf86PointerScreenFuncs.WarpCursor = vgaHWCursor.Warp;
    pScreen->QueryBestSize = vgaHWCursor.QueryBestSize;
    vgaHWCursor.Init(0, pScreen);
  }
  else
  {
    miDCInitialize (pScreen, &xf86PointerScreenFuncs);
  }

#ifndef XF86VGA16
#ifdef MONOVGA
  if (!mfbCreateDefColormap(pScreen))
#else
  if (!cfbCreateDefColormap(pScreen))
#endif
    return(FALSE);
#else /* XF86VGA16 */
  vga16CreateDefColormap(pScreen);
#endif /* XF86VGA16 */

#ifndef DIRTY_STARTUP
  /* Fill the screen with black */
  if (serverGeneration == 1)
  {
#ifdef PC98_NEC480
	{ int i;
		extern short *vramwindow;

		outb(0xa8, 0xff);
		outb(0xaa, 0x00);
		outb(0xac, 0x00);
		outb(0xae, 0x00);
		for(i=0;i<10;i++) {
			*(vramwindow+2) = i;
			memset(vgaBase, 0xff, 0x8000);
		}
		*(vramwindow+2) = 0;
	}
#else /* PC98_NEC480 */
#if (defined(MONOVGA) && !defined (BANKEDMONOVGA)) /* || defined(XF86VGA16) */
    memset(vgaBase,pScreen->blackPixel,vgaSegmentSize);
#else
#ifdef PC98_EGC
#if 1
    {
      outw(EGC_PLANE, 0);
      outw(EGC_MODE, EGC_COPY_MODE);
      outw(EGC_FGC, pScreen->blackPixel);
      memset(vgaBase, 0xff, 0x8000);
    }
#else
    vgaFillSolid( pScreen->blackPixel, GXcopy, 0x0F /* planes */, 0, 0,
		 pScreen->width, pScreen->height );
#endif
#else
    pointer    vgaVirtPtr;
    pointer    vgaPhysPtr;

#if !defined(BANKEDMONOVGA)
    if (vgaUseLinearAddressing) {
        /* use original linear base from MapVidMem() */
        memset(vgaLinearOrig, pScreen->blackPixel,
	       vga256InfoRec.videoRam * 1024);
      }
    else /* banked: */
#endif

    for (vgaVirtPtr = vgaVirtBase;
#if defined(MONOVGA) || defined(XF86VGA16)
        vgaVirtPtr<(pointer)((char *)vgaVirtBase+(vga256InfoRec.videoRam*256));
#else
        vgaVirtPtr<(pointer)((char *)vgaVirtBase+(vga256InfoRec.videoRam*1024));
#endif
        vgaVirtPtr = (pointer)((char *)vgaVirtPtr + vgaSegmentSize))
        {
#if defined(MONOVGA)
	    if (vgaVirtBase == vgaBase)
            {
                /* Not using banking mode */
                vgaPhysPtr = vgaBase;
            }
            else
#endif
            {
                /* Set the bank, then clear it */
                vgaPhysPtr=vgaSetWrite(vgaVirtPtr);
            }
            memset(vgaPhysPtr,pScreen->blackPixel,vgaSegmentSize);
        }
#endif /* !PC98_EGC */
#endif /* MONOVGA */
#endif /* !PC98_NEC480 */
  }
  vgaSaveScreen(NULL, TRUE); /* unblank the screen */
#endif /* ! DIRTY_STARTUP */

  savepScreen = pScreen;

  return(TRUE);
}

static void saveDummy() {}

/*
 *      Assign a new serial number to the window.
 *      Used to force GC validation on VT switch.
 */

/*ARGSUSED*/
static int
vgaNewSerialNumber(pWin, data)
    WindowPtr pWin;
    pointer data;
{
    pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    return WT_WALKCHILDREN;
}


/*
 * vgaEnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
vgaEnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
  BoxRec  pixBox;
  RegionRec pixReg;
  DDXPointRec pixPt;
  PixmapPtr   pspix;
  ScreenPtr   pScreen = savepScreen;

  if (!xf86Resetting && !xf86Exiting)
    {
      pixBox.x1 = 0; pixBox.x2 = pScreen->width;
      pixBox.y1 = 0; pixBox.y2 = pScreen->height;
      pixPt.x = 0; pixPt.y = 0;
      (pScreen->RegionInit)(&pixReg, &pixBox, 1);
#if !defined(MONOVGA) && !defined(XF86VGA16)
      if (vgaBitsPerPixel == 8)
          pspix = (PixmapPtr)pScreen->devPrivate;
      if (vgaBitsPerPixel == 16)
          pspix = (PixmapPtr)pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr;
      if (vgaBitsPerPixel == 24)
          pspix = (PixmapPtr)pScreen->devPrivates[cfb24ScreenPrivateIndex].ptr;
      if (vgaBitsPerPixel == 32)
          pspix = (PixmapPtr)pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
#else
      pspix = (PixmapPtr)pScreen->devPrivate;
#endif
    }

  /* Force every GC writing to the screen to be validated.  */
  if (pScreen && !xf86Exiting && !xf86Resetting)
      WalkTree(pScreen, vgaNewSerialNumber, 0);

  if (enter)
    {
      vgaInitFunc = saveInitFunc;
      vgaSaveFunc = saveSaveFunc;
      vgaRestoreFunc = saveRestoreFunc;
      vgaAdjustFunc = saveAdjustFunc;
      vgaSaveScreenFunc = saveSaveScreenFunc;
      vgaSetReadFunc = saveSetReadFunc;
      vgaSetWriteFunc = saveSetWriteFunc;
      vgaSetReadWriteFunc = saveSetReadWriteFunc;
      
      xf86MapDisplay(screen_idx, VGA_REGION);
      if (vgaUseLinearAddressing)
        xf86MapDisplay(screen_idx, LINEAR_REGION);

      (*vgaEnterLeaveFunc)(ENTER);
#ifdef XFreeXDGA
      if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
	/* Should we do something here or not ? */
      } else
#endif
      {
        vgaOrigVideoState = (pointer)(*vgaSaveFunc)(vgaOrigVideoState);
      }
      (*vgaRestoreFunc)(vgaNewVideoState);
#ifdef SCO
      /*
       * This is a temporary fix for et4000's, it shouldn't affect the other
       * drivers and the user doesn't notice it so, here it is.  What does it
       * fix, et4000 driver leaves the screen blank when you switch back to it.
       * A mode toggle, even on the same mode, fixes it.
       */
      vgaSwitchMode(vga256InfoRec.modes);
#endif

      /*
       * point pspix back to vgaVirtBase, and copy the dummy buffer to the
       * real screen.
       */
      if (!xf86Resetting)
      {
	ScrnInfoPtr pScr = XF86SCRNINFO(pScreen);

	if (vgaHWCursor.Initialized)
	{
	  vgaHWCursor.Init(0, pScreen);
	  vgaHWCursor.Restore(pScreen);
	  vgaAdjustFunc(pScr->frameX0, pScr->frameY0);
	}

        if ((pointer)pspix->devPrivate.ptr != (pointer)vgaVirtBase && ppix)
        {
#if !defined(MONOVGA) && !defined(XF86VGA16)
          if (vgaBitsPerPixel == 8)
	      pspix->devPrivate.ptr = (pointer)vgaVirtBase;
	  else
	      pspix->devPrivate.ptr = vgaLinearBase;
#else
	  pspix->devPrivate.ptr = (pointer)vgaVirtBase;
#endif
#ifndef XF86VGA16
#ifdef MONOVGA
	  mfbDoBitblt(&ppix->drawable, &pspix->drawable, GXcopy, &pixReg,
                      &pixPt);
#else
          if (vgaBitsPerPixel == 8)
	      vga256DoBitblt(&ppix->drawable, &pspix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFF);
          if (vgaBitsPerPixel == 16)
	      cfb16DoBitblt(&ppix->drawable, &pspix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFFFF);
          if (vgaBitsPerPixel == 24)
	      cfb24DoBitblt(&ppix->drawable, &pspix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFFFFFF);
          if (vgaBitsPerPixel == 32)
	      cfb32DoBitblt(&ppix->drawable, &pspix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFFFFFFFF);
#endif
#else /* XF86VGA16 */
	  vgaRestoreScreenPix(pScreen,ppix);
#endif /* XF86VGA16 */
        }
      }
      if (ppix) {
        (pScreen->DestroyPixmap)(ppix);
        ppix = NULL;
      }
    }
  else
    {

      /* Make sure that another driver hasn't disabeled IO */    
      xf86MapDisplay(screen_idx, VGA_REGION);
      if (vgaUseLinearAddressing)
        xf86MapDisplay(screen_idx, LINEAR_REGION);

      (*vgaEnterLeaveFunc)(ENTER);

      /*
       * Create a dummy pixmap to write to while VT is switched out.
       * Copy the screen to that pixmap
       */
      if (!xf86Exiting)
      {
        ppix = (pScreen->CreatePixmap)(pScreen, vga256InfoRec.displayWidth,
				       pScreen->height, pScreen->rootDepth);
        if (ppix)
        {
#ifndef XF86VGA16
#ifdef MONOVGA
	  mfbDoBitblt(&pspix->drawable, &ppix->drawable, GXcopy, &pixReg,
                      &pixPt, 0xFF);
#else
          if (vgaBitsPerPixel == 8)
	      vga256DoBitblt(&pspix->drawable, &ppix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFF);
          if (vgaBitsPerPixel == 16)
	      cfb16DoBitblt(&pspix->drawable, &ppix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFFFF);
          if (vgaBitsPerPixel == 24){
	      cfb24DoBitblt(&pspix->drawable, &ppix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFFFFFF);
	  }
          if (vgaBitsPerPixel == 32)
	      cfb32DoBitblt(&pspix->drawable, &ppix->drawable, GXcopy,
	          &pixReg, &pixPt, 0xFFFFFFFF);
#endif
#else /* XF86VGA16 */
	  vgaSaveScreenPix(pScreen,ppix);
#endif /* XF86VGA16 */
	  pspix->devPrivate.ptr = ppix->devPrivate.ptr;
        }
        (*vgaSaveFunc)(vgaNewVideoState);

#if !defined(MONOVGA) && !defined(XF86VGA16)
	xf86InvalidatePixmapCache();
#endif
      }
      /*
       * We come here in many cases, but one is special: When the server aborts
       * abnormaly. Therefore there MUST be a check whether vgaOrigVideoState
       * is valid or not.
       */
#ifdef XFreeXDGA
      if (vga256InfoRec.directMode & XF86DGADirectGraphics) {      
         /* make sure we are in linear mode */
         /* hide any harware cursors */
         (*vgaEnterLeaveFunc)(LEAVE);
      } else
#endif
      {
         if (vgaOrigVideoState)
            (*vgaRestoreFunc)(vgaOrigVideoState);

            (*vgaEnterLeaveFunc)(LEAVE);

            xf86UnMapDisplay(screen_idx, VGA_REGION);
            if (vgaUseLinearAddressing)
            {
               xf86UnMapDisplay(screen_idx, LINEAR_REGION);
            }
      }
  
      saveInitFunc = vgaInitFunc;
      saveSaveFunc = vgaSaveFunc;
      saveRestoreFunc = vgaRestoreFunc;
      saveAdjustFunc = vgaAdjustFunc;
      saveSaveScreenFunc = vgaSaveScreenFunc;
      saveSetReadFunc = vgaSetReadFunc;
      saveSetWriteFunc = vgaSetWriteFunc;
      saveSetReadWriteFunc = vgaSetReadWriteFunc;

#ifdef XFreeXDGA      
      if (!(vga256InfoRec.directMode & XF86DGADirectGraphics)) {
#endif
      vgaInitFunc = (Bool (*)())saveDummy;
      vgaSaveFunc = (void * (*)())saveDummy;
      vgaRestoreFunc = (void (*)())saveDummy;
      vgaAdjustFunc = (void (*)())saveDummy;
      vgaSetReadFunc = (void (*)())saveDummy;
      vgaSetWriteFunc = (void (*)())saveDummy;
      vgaSetReadWriteFunc = (void (*)())saveDummy;
#ifdef XFreeXDGA
      }
#endif
       vgaSaveScreenFunc = (void (*)())saveDummy;
 
      
    }
}

/*
 * vgaCloseScreen --
 *      called to ensure video is enabled when server exits.
 */
Bool
vgaCloseScreen(screen_idx, pScreen)
    int		screen_idx;
    ScreenPtr	pScreen;
{
  /*
   * Hmm... The server may shut down even if it is not running on the
   * current vt. Let's catch this case here.
   */
  xf86Exiting = TRUE;
#ifdef	PC98_EGC
  /* clear screen */
  {
    outw(EGC_PLANE, 0);
    outw(EGC_MODE, EGC_COPY_MODE);
    outw(EGC_FGC, 0);
    memset(vgaBase, 0xff, 0x8000);
  }
  outb(0x6a, 0x07);
  outb(0x6a, 0x04);
  outb(0x6a, 0x06);
/* outb((hiresoflg)?0xa4:0x7c, 0x00); */
  outb(0x7c, 0x00);
#endif
#ifdef PC98_NEC480
	{ int i;
		extern short *vramwindow;

		outb(0xa8, 0xff);
		outb(0xaa, 0x00);
		outb(0xac, 0x00);
		outb(0xae, 0x00);
		for(i=0;i<10;i++) {
			*(vramwindow+2) = i;
			memset(vgaBase, 0xff, 0x8000);
		}
		*(vramwindow+2) = 0;
	}
	outb(0x6a, 0x07);
	outb(0x6a, 0x04);
	outb(0x6a, 0x06);
	outb(0x7c, 0x00);
#endif /* PC98_NEC480 */
  if (xf86VTSema)
    vgaEnterLeaveVT(LEAVE, screen_idx);
  else if (ppix) {
    /*
     * 7-Jan-94 CEG: The server is not running on the current vt.
     * Free the screen snapshot taken when the server vt was left.
     */
    (savepScreen->DestroyPixmap)(ppix);
    ppix = NULL;
  }
  return(TRUE);
}

/*
 * vgaAdjustFrame --
 *      Set a new viewport
 */
void
vgaAdjustFrame(x, y)
     int x, y;
{
  (*vgaAdjustFunc)(x, y);
}

/*
 * vgaSwitchMode --
 *     Set a new display mode
 */
Bool
vgaSwitchMode(mode)
     DisplayModePtr mode;
{
  if ((*vgaInitFunc)(mode))
  {
    (*vgaRestoreFunc)(vgaNewVideoState);
    if (vgaHWCursor.Initialized)
    {
      vgaHWCursor.Restore(savepScreen);
    }
    return(TRUE);
  }
  else
  {
    ErrorF("Mode switch failed because of hardware initialisation error\n");
    return(FALSE);
  }
}

/*
 * vgaValidMode --
 *     Validate a mode for VGA architecture. Also checks the chip driver
 *     to see if the mode can be supported.
 */
int
vgaValidMode(mode, verbose, flag)
     DisplayModePtr mode;
     Bool verbose;
     int flag;
{
  return (*vgaValidModeFunc)(mode, verbose, flag);
}
