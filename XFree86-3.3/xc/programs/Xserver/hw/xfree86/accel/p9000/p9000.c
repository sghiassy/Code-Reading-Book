/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000.c,v 3.44.2.5 1997/06/01 12:33:28 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1994 by Erik Nygren <nygren@mit.edu>
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
 * ERIK NYGREN, THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, SCOTT LAIRD,
 * DAVID MOEWS, AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL ERIK NYGREN, THOMAS ROELL, KEVIN E. MARTIN, 
 * RICKARD E. FAITH, DAVID MOEWS SCOTT LAIRD, OR TIAGO GONS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Further modifications by Scott Laird (lair@kimbark.uchicago.edu)
 * and Tiago Gons (tiago@comosjn.hobby.nl)
 * Modified for the P9000 by Erik Nygren (nygren@mit.edu)
 * Bank switching code for P9000 by David Moews (dmoews@xraysgi.ims.uconn.edu)
 *
 */
/* $XConsortium: p9000.c /main/18 1996/10/27 11:04:43 kaleb $ */

#define NEED_EVENTS
#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "pixmapstr.h"
#include "scrnintstr.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "opaque.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"
#include "p9000viper.h"
#include "p9000orchid.h"
#include "mi.h"
#include "cfb.h"

#ifdef XFreeXDGA
#include "Xproto.h"
#include "extnsionst.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#ifdef DPMSExtension
#include "extensions/dpms.h"
#endif

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

extern int p9000MaxClock;
extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;
extern char *xf86VisualNames[];

extern int defaultColorVisualClass;

static int p9000ValidMode(
#if NeedFunctionPrototypes 
   DisplayModePtr,
   Bool,
   int
#endif
);

ScrnInfoRec p9000InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    p9000Probe,      	/* Bool (* Probe)() */
    p9000Initialize,	/* Bool (* Init)() */
    p9000ValidMode,	/* int (* ValidMode)() */
    p9000EnterLeaveVT,	/* void (* EnterLeaveVT)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveMonitor)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveCursor)() */
    p9000AdjustFrame,   /* void (* AdjustFrame)() */
    p9000SwitchMode,	/* Bool (* SwitchMode)() */
    p9000DPMSSet,	/* void (* DPMSSet)() */
    p9000PrintIdent,	/* void (* PrintIdent)() */
    8,			/* int depth */
    {5, 6, 5},		/* xrgb weight */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1,			/* int displayWidth */
    -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },              /* OFlagSet options */
    {0, },              /* OFlagSet clockOptions */
    {0, },              /* OFlagSet xconfigFlag */
    NULL,	       	/* char *chipset */
    NULL,	       	/* char *ramdac */
    {0, 0, 0, 0},	/* int dacSpeeds[MAXDACSPEEDS] */
    0,			/* int dacSpeedBpp */
    0,			/* int clocks */
    {0, },		/* int clock[MAXCLOCKS] */
    0,			/* int maxClock */
    0,			/* int videoRam */
    0,                  /* int BIOSbase, 1.3 new */
    80000000L,          /* unsigned long MemBase, 2.1 new */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,	       	/* MonPtr monitor */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock, 1.3 new */
    FALSE,              /* Bool bankedMono */
    "P9000",            /* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    p9000ValidTokens,	/* int *validTokens */
    P9000_PATCHLEVEL,	/* char *patchlevel */
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
    NULL,               /* char* DCConfig */
    NULL,               /* char* DCOptions */
    0			/* int MemClk */
#ifdef XFreeXDGA
    /* Note that the double buffered support is 
     * a hack in the P9000 server.  See the README.P9000
     * file for more details.  */
    ,0,			/* int directMode */
    p9000SetVidPage,    /* Set Vid Page */
    0,			/* unsigned long physBase */
    0			/* int physSize */
#endif
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

ScreenPtr p9000savepScreen;

Bool p9000BlockCursor, p9000ReloadCursor;
int p9000hotX, p9000hotY;

static int AlreadyInited = FALSE;

p9000CRTCRegRec p9000CRTCRegs;

p9000MiscRegRec  p9000MiscReg;

p9000VendorRec *p9000VendorPtr = NULL;  /* The probed vendor */

/* A list of vendors to be probed */
static p9000VendorRec *p9000VendorList[] = {
#ifdef P9000_VPRPCI_SUP
  /* The Viper PCI must come before the Viper VLB */
  &(p9000ViperPciVendor),
#endif
  &(p9000ViperVlbVendor),
#ifdef P9000_ORCHID_SUP
  &(p9000OrchidVendor),
#endif
};

static int Num_p9000_Vendors = (sizeof(p9000VendorList)/
				sizeof(p9000VendorList[0]));

static ScreenPtr savepScreen = NULL;

/* P9000 control register base */
volatile unsigned long *CtlBase; 
/* P9000 linear mapped frame buffer base */
volatile unsigned long *VidBase; 

volatile pointer p9000VideoMem;

unsigned p9000BytesPerPixel;  /* The number of bytes per pixel */

/* Options that may be specified to XF86Config */
Bool p9000SWCursor = FALSE;         /* Use a software cursor */
Bool p9000DACSyncOnGreen = FALSE;   /* Enables syncing on green */
Bool p9000DAC8Bit = TRUE;           /* Use 8 bits for cmap entry (the
				     * alternative is not implemented
				     * and may never be */
Bool p9000NoAccel = FALSE;          /* Disables hardware acceleration */

unsigned char p9000SwapBits[256];

static unsigned p9000_IOPorts[] = {
  /* VGA Registers */
  SEQ_INDEX_REG,    SEQ_PORT,         MISC_OUT_REG,     MISC_IN_REG,
  GRA_I,            GRA_D,            CRT_IC,           CRT_DC,
  IS1_RC,           ATT_IW,           ATT_R,            VGA_BANK_REG,

  /* BT484/485 Register Defines */  
  BT_PIXEL_MASK,    BT_READ_ADDR,     BT_WRITE_ADDR,    BT_RAMDAC_DATA,
  BT_COMMAND_REG_0, BT_CURS_RD_ADDR,  BT_CURS_WR_ADDR,  BT_CURS_DATA ,
  BT_STATUS_REG,    BT_COMMAND_REG_3, BT_CURS_RAM_DATA, BT_COMMAND_REG_1,
  BT_COMMAND_REG_2, BT_CURS_Y_LOW,    BT_CURS_Y_HIGH,   BT_CURS_X_LOW, 
  BT_CURS_X_HIGH,
};

static int Num_p9000_IOPorts = (sizeof(p9000_IOPorts)/
				sizeof(p9000_IOPorts[0]));

/* p9000WeightMasks must match p9000weights[], below */
static short p9000WeightMasks[] = { RGB16_565, RGB16_555, RGB32_888 };
short p9000WeightMask;

#define p9000ReorderSwapBits(a,b)    b = \
        (a & 0x80) >> 7 | \
        (a & 0x40) >> 5 | \
        (a & 0x20) >> 3 | \
        (a & 0x10) >> 1 | \
        (a & 0x08) << 1 | \
        (a & 0x04) << 3 | \
        (a & 0x02) << 5 | \
        (a & 0x01) << 7;

/* Raster operation (alu) -> minterm mapping */
unsigned int p9000alu[16];	/* alu src = p9000 src        */
unsigned int p9000QuadAlu[16] ; /* alu src = p9000 foreground */
unsigned int p9000PixOpAlu[16] ; /* use for pix1 opaque operations */
unsigned int p9000PixAlu[16] ; /* use for pix1 transparent operations */

/*
 * p9000Probe --
 *     probe and initialize the hardware driver
 */
Bool
p9000Probe()
{
    int            memavail, rounding;
    DisplayModePtr pMode, pEnd = NULL;
    int            numValidModes = 0;
    int            maxX, maxY;
    int            curvendor;
    Bool           modeIsValid;
    OFlagSet       validOptions;
    xrgb           p9000weights[] = { { 5, 6, 5 }, { 5, 5, 5 }, /* 16bpp */
				      { 8, 8, 8 } };            /* 32bpp */
    int            i;
    unsigned long  tmpsysconfig;  /* dummy value */

    if (!xf86LinearVidMem())
      {
	ErrorF("%s %s: This operating system does not support memory mapping of linear regions.\nAs a result, it can not be used with this server\n",
	       XCONFIG_PROBED, p9000InfoRec.name);
	return(FALSE);
      }

    xf86ClearIOPortList(p9000InfoRec.scrnIndex);
    xf86AddIOPorts(p9000InfoRec.scrnIndex, Num_p9000_IOPorts, p9000_IOPorts);

    if (p9000InfoRec.chipset)
      {
	for (curvendor = 0; curvendor < Num_p9000_Vendors; curvendor++)
	  {
	    if (0 == strcmp(p9000VendorList[curvendor]->Vendor,
			    p9000InfoRec.chipset))
	      {
		p9000VendorPtr = p9000VendorList[curvendor];
		break;
	      }
	  }
	if (curvendor >= Num_p9000_Vendors)
	  {
	    ErrorF("%s %s: Chipset specified in XF86Config (%s) is not recognized!\n",
		   XCONFIG_GIVEN, p9000InfoRec.name, p9000InfoRec.chipset);
	    return(FALSE);
	  }
      }
    else
      {
	for (curvendor = 0; curvendor < Num_p9000_Vendors; curvendor++)
	  {
	    if (p9000VendorList[curvendor]->Probe())
	      {
		p9000VendorPtr = p9000VendorList[curvendor];
		break;
	      }
	  }
	if (curvendor >= Num_p9000_Vendors)
	  {
	    ErrorF("%s %s: Autodetection of chipset failed.  Specify explicitly in XF86Config.\n", XCONFIG_PROBED, p9000InfoRec.name);
	    return(FALSE);
	  }
      }
    if (xf86Verbose)
      ErrorF("%s %s: Vendor/chipset is %s (%s)\n",
	     (p9000InfoRec.chipset ? XCONFIG_GIVEN : XCONFIG_PROBED),
	     p9000InfoRec.name, p9000VendorPtr->Vendor, p9000VendorPtr->Desc);
    p9000InfoRec.chipset = p9000VendorPtr->Vendor;

    xf86EnableIOPorts(p9000InfoRec.scrnIndex);
    
    OFLG_ZERO(&validOptions);
    OFLG_SET(OPTION_SW_CURSOR, &validOptions);
    OFLG_SET(OPTION_NOACCEL, &validOptions);
    OFLG_SET(OPTION_SYNC_ON_GREEN, &validOptions);
    OFLG_SET(OPTION_POWER_SAVER, &validOptions);
    OFLG_SET(OPTION_VRAM_128, &validOptions);
    OFLG_SET(OPTION_VRAM_256, &validOptions);
    xf86VerifyOptions(&validOptions, &p9000InfoRec);
    
#if 0  /* We shouldn't need this any more */
    if (!p9000InfoRec.clocks)
      {
	ErrorF("%s %s: Autodetection of clocks is not supported.\n\tExplicitly specify in XF86Config file on a Clocks line.\n", XCONFIG_PROBED, p9000InfoRec.name);
	return(FALSE);
      }
#endif
    /* At the moment, all P9000-based boards use a icd2061a compatable
     * programmable clock so no more differentiation than this is needed */
    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &(p9000InfoRec.clockOptions));

    if (!p9000InfoRec.videoRam)
      {
	ErrorF("%s %s: Autodetection of video RAM is not yet supported.\n\tExplicitly specify VideoRAM in XF86Config file.\n", XCONFIG_PROBED, p9000InfoRec.name);
	return(FALSE);	
      }
    memavail = p9000InfoRec.videoRam*1024;

    /* Validate the vendor specific things like MemBase */
    if (!p9000VendorPtr->Validate())
      return(FALSE);
    /* The validate function may set p9000MaxClock.  Reflect this change. */
    p9000InfoRec.maxClock = p9000MaxClock; 

    if (xf86bpp < 0) {
        xf86bpp = p9000InfoRec.depth;
    }
    if (xf86weight.red == 0 || xf86weight.green == 0 || xf86weight.blue == 0) {
        xf86weight = p9000InfoRec.weight;
    }
    switch (xf86bpp)
      {
      case 8:
	break;
      case 16:
	p9000InfoRec.depth = 16;  /* if 555, set to 15 below */
	p9000InfoRec.bitsPerPixel = 16;
	if (p9000InfoRec.defaultVisual < 0)
	    p9000InfoRec.defaultVisual = TrueColor;
	if (defaultColorVisualClass < 0)
	    defaultColorVisualClass = p9000InfoRec.defaultVisual;
	break;
      case 24:
      case 32:
	xf86bpp = 32;
	p9000InfoRec.depth = 24;
	p9000InfoRec.bitsPerPixel = 32;     /* Use sparse 24 bpp (RGBX) */
	if (p9000InfoRec.defaultVisual < 0)
	    p9000InfoRec.defaultVisual = TrueColor;
	if (defaultColorVisualClass < 0)
	    defaultColorVisualClass = p9000InfoRec.defaultVisual;
	p9000MaxClock = P9000_MAX_32BPP_CLOCK;
	break;
      default:
	ErrorF("%s %s: Invalid value for bpp.  Valid values are 8, 16, and 32.\n", XCONFIG_GIVEN, p9000InfoRec.name);
	return(FALSE);
      }

    if (p9000InfoRec.bitsPerPixel > 8 &&
	defaultColorVisualClass >= 0 && defaultColorVisualClass != TrueColor) {
	ErrorF("Invalid default visual type: %d (%s)\n",
	       defaultColorVisualClass,
	       xf86VisualNames[defaultColorVisualClass]);
	return(FALSE);
    }

    if (xf86bpp == 16) 
      {
	for (i = 0; i < 2; i++) 
	  {
	    if (xf86weight.red == p9000weights[i].red
		&& xf86weight.green == p9000weights[i].green
		&& xf86weight.blue == p9000weights[i].blue)
	      break;
	  }
	if (i == 2) 
	  {
	    ErrorF("%s %s: Invalid color weighting\n", 
		   XCONFIG_GIVEN, p9000InfoRec.name);
	    return(FALSE);
	  }
	if (i == RGB16_555)
	  p9000InfoRec.depth = 15;
	p9000WeightMask = p9000WeightMasks[i];
      }
    else if (xf86bpp == 32)
      {
	xf86weight.red =  xf86weight.green = xf86weight.blue = 8;
	p9000WeightMask = p9000WeightMasks[RGB32_888];
      }

    xf86ProbeFailed = FALSE;

    if (xf86Verbose)
    {
        ErrorF("%s %s: (mem: %dk numclocks: %d vendor: %s)\n",
               OFLG_ISSET(XCONFIG_VIDEORAM,&p9000InfoRec.xconfigFlag) ?
               XCONFIG_GIVEN : XCONFIG_PROBED,
               p9000InfoRec.name,
               p9000InfoRec.videoRam,
               p9000InfoRec.clocks,
	       p9000VendorPtr->Vendor);
      }

    /* All modes must have the same width and height as the first valid mode.
     * Any invalid modes are removed from the mode ring.
     */
    pMode = p9000InfoRec.modes;
    if (pMode == NULL)
      {
	ErrorF("No modes supplied in XF86Config\n");
	return(FALSE);
      }
    do
      {
	DisplayModePtr pModeSv;
	modeIsValid = TRUE;
	/* xf86LookupMode returns FALSE if it ran into an invalid
	 * parameter */
	if(xf86LookupMode(pMode, &p9000InfoRec, LOOKUP_DEFAULT) == FALSE)
	  {
	    modeIsValid = FALSE;
	  }
	/* Now for a bunch of tests to see if the mode is possible and/or
	 * safe.  It it fails, the mode is removed.
	 */
        else if (pMode->HDisplay * pMode->VDisplay * (p9000InfoRec.bitsPerPixel / 8)
		 > memavail)
	  {
	    ErrorF("%s %s: Too little memory for mode %s.\n", XCONFIG_PROBED,
		   p9000InfoRec.name, pMode->name);
	    modeIsValid = FALSE;
	  }
	else if ((pEnd) && (pEnd->VDisplay != pMode->VDisplay)
		 && (pEnd->HDisplay != pMode->HDisplay))
	  {
	    ErrorF("%s %s: The dimensions of mode %s do not match those of\n\tthe first valid mode (%s).\n", XCONFIG_PROBED,
		   p9000InfoRec.name, pMode->name, pEnd->name);
	    modeIsValid = FALSE;
	  }
	/* We should do a better test than this...  *TO*DO* */
	/* These size limits are in effect until 1600x1200 is tested *TO*DO* */
	else if ((pMode->HDisplay > 1344) && (p9000InfoRec.bitsPerPixel==8)
		 || (pMode->HDisplay > 1152) && (p9000InfoRec.bitsPerPixel==16)
		 || (pMode->HDisplay > 832) && (p9000InfoRec.bitsPerPixel==32))
	  {
	    ErrorF("%s %s: The width of mode %s is too large for %dbpp.\n",
		   XCONFIG_PROBED, p9000InfoRec.name, 
		   pMode->name,p9000InfoRec.bitsPerPixel);
	    modeIsValid = FALSE;
	  }
        else if ((pMode->VDisplay > 1088) && (p9000InfoRec.bitsPerPixel==8)
		 || (pMode->VDisplay > 910) && (p9000InfoRec.bitsPerPixel==16)
		 || (pMode->VDisplay > 632) && (p9000InfoRec.bitsPerPixel==32))
	  {
	    ErrorF("%s %s: The height of mode %s is too large for %dbpp.\n",
		   XCONFIG_PROBED, 
		   p9000InfoRec.name, pMode->name,p9000InfoRec.bitsPerPixel);
	    modeIsValid = FALSE;
	  }
	else if (p9000InfoRec.clock[pMode->Clock] > p9000MaxClock)
	  {
	    ErrorF("%s %s: The clock speed of mode %s is too high (max %ld MHz).\n",
		   XCONFIG_PROBED, p9000InfoRec.name, 
		   pMode->name, p9000MaxClock/1000);	  
	    modeIsValid = FALSE;
	  }
	/* See if the horizontal resolution is valid.  This is constrained
	 * by possible values for sysconfig */
	else if (!p9000CalcSysconfigHres(pMode->HDisplay, 
					 p9000InfoRec.bitsPerPixel/8,
					 &tmpsysconfig))
	  {
	    ErrorF("%s %s: The width of mode %s is not valid.\n",
		   XCONFIG_PROBED, p9000InfoRec.name, pMode->name);
	    modeIsValid = FALSE;
	  }
	
	if (!modeIsValid)
	  {    /* The mode is not valid.  Delete it. */
	    if (pMode->next == pMode)
	      {  /* Then this is the last mode.  If we don't do this,
		  * xf86DeleteMode will have a fatal error */
		ErrorF("%s %s: Removing mode \"%s\" from list of valid modes.\n", XCONFIG_PROBED, 
		       p9000InfoRec.name, pMode->name);
		ErrorF("\n\t*** No valid modes found! ***\n"); 
		return(FALSE);
	      }
	    pModeSv=pMode->next;
	    xf86DeleteMode(&p9000InfoRec, pMode);
	    pMode = pModeSv; 
	  }
	else   /* The mode has passed and is valid */
	  {
	    /*** Print warnings here ***/
	    if (p9000InfoRec.clock[pMode->Clock] > 110000)
	      {
		ErrorF("**********************************************************\n");
		ErrorF("WARNING: The use of clocks over 110 MHz is not\n");
		ErrorF("  recommended or supported and may damage your video card!\n");
		ErrorF("**********************************************************\n");
	      }

	    /*
	     * Successfully looked up this mode.  If pEnd isn't 
	     * initialized, set it to this mode.
	     */
	    if (pEnd == (DisplayModePtr) NULL)
	      {
		pEnd = pMode;
		p9000InfoRec.virtualX = pEnd->HDisplay;
		p9000InfoRec.virtualY = pEnd->VDisplay;
	      }
	    /* Set the clock synthesizer frequency */
	    pMode->SynthClock = p9000InfoRec.clock[pMode->Clock];
	    pMode = pMode->next;
	  }
      } while (pMode != pEnd);
    
    if (xf86Verbose)
      ErrorF("%s %s: Virtual resolution set to %dx%d\n",
	     OFLG_ISSET(XCONFIG_VIRTUAL,&p9000InfoRec.xconfigFlag) ?
	     XCONFIG_GIVEN : XCONFIG_PROBED, p9000InfoRec.name,
	     p9000InfoRec.virtualX, p9000InfoRec.virtualY);

    if (OFLG_ISSET(OPTION_SYNC_ON_GREEN, &p9000InfoRec.options))
      {
	p9000DACSyncOnGreen = TRUE;
	if (xf86Verbose)
	  ErrorF("%s %s: Putting RAMDAC into sync-on-green mode\n",
		 XCONFIG_GIVEN, p9000InfoRec.name);
      }

#ifdef DPMSExtension
    if (DPMSEnabledSwitch ||
	(OFLG_ISSET(OPTION_POWER_SAVER, &p9000InfoRec.options) &&
	 !DPMSDisabledSwitch))
	defaultDPMSEnabled = DPMSEnabled = TRUE;
#endif

    if (OFLG_ISSET(OPTION_NOACCEL, &p9000InfoRec.options))
      {
	p9000NoAccel = TRUE;
	if (xf86Verbose)
	  ErrorF("%s %s: Disabling hardware acceleration\n",
		 XCONFIG_GIVEN, p9000InfoRec.name);
      }

    p9000SWCursor = OFLG_ISSET(OPTION_SW_CURSOR, &p9000InfoRec.options);
    if (xf86Verbose)
      ErrorF("%s %s: Using %s cursor\n", p9000SWCursor ? XCONFIG_GIVEN :
	     XCONFIG_PROBED, p9000InfoRec.name,
	     p9000SWCursor ? "software" : "hardware");

    if (xf86Verbose) 
      {
	if (p9000InfoRec.bitsPerPixel == 8)
	  ErrorF("%s %s: Using %d bits per RGB value\n",
		 XCONFIG_PROBED, p9000InfoRec.name,
		 p9000DAC8Bit ?  8 : 6);
	else if (p9000InfoRec.bitsPerPixel == 16)
	  ErrorF("%s %s: Using 16 bpp.  Color weight: %1d%1d%1d\n", 
		 XCONFIG_GIVEN, p9000InfoRec.name, xf86weight.red,
		 xf86weight.green, xf86weight.blue);
	else if (p9000InfoRec.bitsPerPixel == 32)
	  ErrorF("%s %s: Using sparse 32 bpp.  Color weight: %1d%1d%1d\n", 
		 XCONFIG_GIVEN, p9000InfoRec.name, xf86weight.red,
		 xf86weight.green, xf86weight.blue);
      }

    xf86DisableIOPorts(p9000InfoRec.scrnIndex);   
#ifdef DEBUG
    ErrorF("Reached end of p9000Probe...\n");
#endif

    return(TRUE);
}

void
p9000PrintIdent()
{
  int curvendor;

  ErrorF("  %s: accelerated server for ", p9000InfoRec.name);
  ErrorF("Weitek P9000 graphics adaptors (Patchlevel %s)\n", 
	 p9000InfoRec.patchLevel);
  ErrorF("  Supported vendors (specify on chipset line):\n");
  for (curvendor = 0; curvendor < Num_p9000_Vendors; curvendor++)
    {
      ErrorF("\t%s\t%s\n", p9000VendorList[curvendor]->Vendor,
	     p9000VendorList[curvendor]->Desc);
    }
}

/*
 * p9000Initialize --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
p9000Initialize (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
  int i;

  xf86EnableIOPorts(scr_index);
#ifdef DEBUG
  ErrorF("Entered p9000Initialize...\n");
  ErrorF("Entering Server Generation %d\n", serverGeneration);
#endif

#ifdef P9000_ACCEL
  /* Prepare GC stuff for use */
  p9000InitGC();
#endif

  if (serverGeneration == 1)  /* First time */
    {
      /* Maps P9000 linear address space into video memory */
      p9000InitAperture(scr_index);

      /* Do vendor specific initialization */
      p9000VendorPtr->Initialize(scr_index, pScreen, argc, argv);

      /* Set up swap bits table */
      for (i = 0; i < 256; i++)
	{
	  p9000ReorderSwapBits(i, p9000SwapBits[i]);
	}

      /* Save the VT's colormap and such before anyone messes with it */
      p9000SaveVT();
    }

  /* Calculates registers for this mode */
  p9000CalcCRTCRegs(&p9000CRTCRegs, p9000InfoRec.modes);

  /* Enables the P9000 */
  p9000VendorPtr->Enable(&p9000CRTCRegs);

  if (serverGeneration == 1)
  {
    /* Calculates registers specific to the P9000 and its memory size */
    p9000CalcMiscRegs();  /* Enable must be run before this! */
  }

#ifdef DEBUG
    ErrorF("About to enter p9000SetCRTCRegs\n");
#endif

  /* Load in the P9000 registers to set this resolution */
  p9000SetCRTCRegs(&p9000CRTCRegs); 

#ifdef DEBUG
    ErrorF("Done with p9000SetCRTCRegs.. Entering p9000InitEnvironment\n");
#endif

  p9000InitEnvironment();
  AlreadyInited = TRUE;

#ifdef DEBUG
    ErrorF("Done with p9000InitEnv... About to enter p9000ScreenInit\n");
#endif
  
  if (!p9000ScreenInit(pScreen,
		       (pointer) VidBase,
		       p9000InfoRec.virtualX, p9000InfoRec.virtualY,
		       75, 75,
		       p9000InfoRec.virtualX))
    return(FALSE);
  
  pScreen->CloseScreen = p9000CloseScreen;
  pScreen->SaveScreen = p9000SaveScreen;
  
#ifdef DEBUG
    ErrorF("pScreen being set up...\n");
#endif

#if 0
  mfbRegisterCopyPlaneProc (pScreen, miCopyPlane);
#endif

  switch (p9000InfoRec.bitsPerPixel) 
    {
    case 8:
      pScreen->InstallColormap = p9000InstallColormap;
      pScreen->UninstallColormap = p9000UninstallColormap;
      pScreen->ListInstalledColormaps = p9000ListInstalledColormaps;
      pScreen->StoreColors = p9000StoreColors;
      break;
    case 16:
    case 32:
      pScreen->InstallColormap = cfbInstallColormap;
      pScreen->UninstallColormap = cfbUninstallColormap;
      pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
      pScreen->StoreColors = (void (*)())NoopDDA;
    }

  if (p9000SWCursor)
    miDCInitialize (pScreen, &xf86PointerScreenFuncs);
  else
    {
      xf86PointerScreenFuncs.WarpCursor = p9000WarpCursor;
      (void)p9000CursorInit(0, pScreen); 
      pScreen->QueryBestSize = p9000QueryBestSize;
    }
  
#ifdef XFreeXDGA
  /*
   * DGA double buffering is supported if:
   *   1) virtualX*virtualY*bytesPerPixel < 1024K
   *   2) memconfig is 0x2
   * This is done by setting memconfig to 0x3 and the bank size to X*Y*BPP
   * and the bank select changes the read and write bank.  Adjust
   * frame then switches the bank being displayed.
   * Double buffering is enabled iff physSize!=videoRam*1024
   * NOTE: This is a HACK!
   */
  /* The standard values for single buffering */
  p9000InfoRec.displayWidth = p9000InfoRec.virtualX;
  p9000InfoRec.directMode = XF86DGADirectPresent;
  p9000InfoRec.physBase = p9000InfoRec.MemBase + 0x200000L;
  p9000InfoRec.physSize = p9000InfoRec.videoRam * 1024;
  /* Test to see if we can do double buffering */
  if ((p9000InfoRec.virtualX * p9000InfoRec.virtualY 
       * (p9000InfoRec.bitsPerPixel / 8) < p9000InfoRec.videoRam * 1024 / 2)
      && p9000MiscReg.memconfig == 0x2L)
    {
      p9000InfoRec.physSize = p9000InfoRec.virtualX * p9000InfoRec.virtualY 
	* (p9000InfoRec.bitsPerPixel / 8);
      ErrorF("%s %s: DGA double buffering enabled.\n",
	     XCONFIG_PROBED, p9000InfoRec.name);
    }
  else
    {
      ErrorF("%s %s: DGA double buffering not supported %s.\n",
	     XCONFIG_PROBED, p9000InfoRec.name,
	     p9000MiscReg.memconfig == 0x2L ? "at this resolution" 
	     : "on boards with only 1 MB");
    }
#ifdef DEBUG
  ErrorF("DGA: physBase 0x%x  physSize %d  width %d\n", p9000InfoRec.physBase,
	 p9000InfoRec.physSize, p9000InfoRec.displayWidth);
#endif
#endif

  savepScreen = pScreen;

  p9000savepScreen = pScreen;

#ifdef DEBUG
    ErrorF("Leaving p9000Initialize...\n");
#endif

#ifdef P9000_ACCEL
  p9000ImageInit();
#endif

  return (cfbCreateDefColormap(pScreen));
}

/*
 * p9000EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
p9000EnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
#ifdef DEBUG
  ErrorF("EnterLeave: %s with xf86Resetting=%s xf86Exiting=%s XF86DGADirectGraphics=%s\n",
	 enter?"Entering":"Leaving",
	 xf86Resetting?"TRUE":"FALSE",
	 xf86Exiting?"TRUE":"FALSE",
	 p9000InfoRec.directMode&XF86DGADirectGraphics?"TRUE":"FALSE");
#endif

#ifdef XFreeXDGA
  if (!xf86Exiting && (p9000InfoRec.directMode & XF86DGADirectGraphics))
    {
      if (enter)		/* Exit DGA Mode */
	{
	  unsigned long srtctl, sysconfig;

	  p9000BtCursorOn();
	  /* Restore memconfig and double buffering registers */
	  srtctl = p9000Fetch(SRTCTL, CtlBase);
	  sysconfig = p9000Fetch(SYSCONFIG, CtlBase);
	  p9000Store(MEM_CONFIG, CtlBase, p9000MiscReg.memconfig);
	  p9000Store(SRTCTL, CtlBase, srtctl & ~0x8L);
	  p9000Store(SYSCONFIG, CtlBase, sysconfig & ~0x600L);
	}
      else			/* Start DGA Mode */
	{
	  p9000BtCursorOff();
	  if (p9000MiscReg.memconfig == 0x2L && 
	      (p9000InfoRec.physSize != p9000InfoRec.videoRam*1024))
	    {
	      /* Enable double buffering */
	      p9000Store(MEM_CONFIG, CtlBase, 0x03);
	    }
	}
      /* Don't do anything else... */
      return;
    }
#endif
  
  if (enter)
    {
#ifdef DEBUG
      ErrorF("Entering\n");
#endif
      xf86EnableIOPorts(p9000InfoRec.scrnIndex);
      p9000SaveVT();
      p9000VendorPtr->Enable(&p9000CRTCRegs);
      p9000SetCRTCRegs(&p9000CRTCRegs); 
      p9000RestoreDACvalues();
      AlreadyInited = TRUE;
    } 
  else
    {
#ifdef DEBUG
      ErrorF("Leaving\n");
#endif
      p9000CleanUp();     /* This does p9000RestoreVT() */
      xf86DisableIOPorts(p9000InfoRec.scrnIndex);
      AlreadyInited = FALSE;
    }
}


/*
 * p9000SetVidPage -- 
 *      In DGA double buffered mode, switches which buffer is
 *      mapped into memory (0 or 1).
 */

void
p9000SetVidPage(bufnum)
     int bufnum;
{
#ifdef XFreeXDGA
  if ((p9000InfoRec.directMode & XF86DGADirectGraphics)
      && (p9000MiscReg.memconfig == 0x2L)
      && (p9000InfoRec.physSize != p9000InfoRec.videoRam*1024))
    {
      unsigned long sysconfig;
      sysconfig = p9000Fetch(SYSCONFIG, CtlBase);
#ifdef DEBUG
      ErrorF("P9000: SetVidPage %d was 0x%x\n",bufnum,sysconfig);
#endif
      p9000Store(SYSCONFIG, CtlBase, (sysconfig & ~0x600L) 
		 | ((0==bufnum) ? 0x000L : 0x600L));
    }
#endif /* XFreeXDGA */
}


/*
 * p9000AdjustFrame -- 
 *      Adjusts the screen frame offset.  Only works in
 *      DGA double buffer mode and then only switches between two banks.
 *      Displays bank 0 if yoff < virtualY else displays bank 1.
 */

void
p9000AdjustFrame(xoff, yoff)
     int xoff, yoff;
{
#ifdef XFreeXDGA
  if ((p9000InfoRec.directMode & XF86DGADirectGraphics)
      && (p9000MiscReg.memconfig == 0x2L)
      && (p9000InfoRec.physSize != p9000InfoRec.videoRam*1024))
    {
      unsigned long srtctl;
      srtctl = p9000Fetch(SRTCTL, CtlBase);
#ifdef DEBUG
      ErrorF("P9000: AdjustFrame %d,%d was 0x%x\n",xoff,yoff,srtctl);
#endif
      if (yoff < p9000InfoRec.virtualY)
	{			
	  /* Display Bank 0 */
	  p9000Store(SRTCTL, CtlBase, (srtctl & ~0x8L));
	}
      else
	{			
	  /* Display Bank 1 */
	  p9000Store(SRTCTL, CtlBase, (srtctl | 0x8L));
	}
    }
    /* Is a retrace wait required here? */
#endif /* XFreeXDGA */
}


/*
 * p9000CloseScreen --
 *      called to ensure video is enabled when server exits.
 */

Bool
p9000CloseScreen(screen_idx, pScreen)
     int screen_idx;
     ScreenPtr pScreen;
{
  /*
   * Hmm... The server may shut down even if it is not running on the
   * current vt. Let's catch this case here.
   */
  xf86Exiting = TRUE;
  if (xf86VTSema) 
    p9000EnterLeaveVT(LEAVE, screen_idx);
  return(TRUE);
}

/*
 * p9000SaveScreen --
 *      blank the screen.
 *      Only supported for 8bpp.  *TO*DO*  
 *      For now, use the screen saver extension.
 */

Bool
p9000SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
  if (on) 
    {
      SetTimeSinceLastInputEvent();
      /* Power the RAMDAC back up. */
      p9000OutBtReg(BT_COMMAND_REG_0, 0xFE, 0x0 /* ~BT_CR0_POWERDOWN */);
    }
  else
    {
      /* Power down the RAMDAC output to blank the screen.  No data
       * will be lost and MPU reads and writes should continue to work. */
      p9000OutBtReg(BT_COMMAND_REG_0, 0xFE, BT_CR0_POWERDOWN);
    }
  return(TRUE);
}

/*
 * p9000DPMSSet -- Sets VESA Display Power Management Signaling (DPMS) Mode
 */

#ifdef DPMSExtension
void
p9000DPMSSet(PowerManagementMode)
     int PowerManagementMode;
{
    if (!xf86VTSema) return;

    switch (PowerManagementMode) {
    case DPMSModeOn:
    case DPMSModeStandby:
    case DPMSModeSuspend:
	/* enable video (1e5, 1e4, or 1c4) */
	p9000Store(SRTCTL,CtlBase, p9000MiscReg.srtctl);
	/* Power the RAMDAC back up. */
	p9000OutBtReg(BT_COMMAND_REG_0, 0xFE, 0x0 /* ~BT_CR0_POWERDOWN */);
	break;
    case DPMSModeOff:
	/* Power down the RAMDAC output to blank the screen.  No data
	 * will be lost and MPU reads and writes should continue to work. */
	p9000OutBtReg(BT_COMMAND_REG_0, 0xFE, BT_CR0_POWERDOWN);
	usleep(10000);
	/* disable video in the video controller */
	p9000Store(SRTCTL,CtlBase,0x01C4L);
	break;
    }
}
#endif

/*
 * p9000SwitchMode --
 *     Set a new display mode
 */
Bool
p9000SwitchMode(mode)
     DisplayModePtr mode;
{
  p9000CalcCRTCRegs(&p9000CRTCRegs, mode);
  p9000VendorPtr->Enable(&p9000CRTCRegs);
  p9000SetCRTCRegs(&p9000CRTCRegs); 
  
  return(TRUE);
}

/*
 * p9000ValidMode --
 *
 */
static int
p9000ValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}

