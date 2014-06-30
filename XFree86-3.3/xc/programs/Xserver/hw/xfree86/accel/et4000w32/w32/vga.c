/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/vga.c,v 3.39.2.4 1997/05/16 11:35:12 hohndel Exp $ */ 
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
 * Author:  Thomas Roell, roell@@informatik.tu-muenchen.de
 *
 */
/* $XConsortium: vga.c /main/22 1996/10/25 21:19:22 kaleb $ */

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
#include "cfb.h"
#include "cfbmskbits.h"
#include "w32version.h"
#include "w32box.h"

#ifdef XFreeXDGA
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

extern vgaVideoChipRec ET4000;
extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;
extern Bool miDCInitialize();
extern Bool W32SaveScreen();

ScrnInfoRec vga256InfoRec = {
  FALSE,		/* Bool configured */
  -1,			/* int tmpIndex */
  -1,			/* int scrnIndex */
  vgaProbe,		/* Bool (* Probe)() */
  vgaScreenInit,	/* Bool (* Init)() */
  vgaValidMode,		/* int (* ValidMode)() */
  vgaEnterLeaveVT,	/* void (* EnterLeaveVT)() */
  (void (*)())NoopDDA,	/* void (* EnterLeaveMonitor)() */
  (void (*)())NoopDDA,	/* void (* EnterLeaveCursor)() */
  (void (*)())NoopDDA,	/* void (* AdjustFrame)() */
  vgaSwitchMode,	/* Bool (* SwitchMode)() */
  vgaDPMSSet,		/* void (* DPMSSet)() */
  vgaPrintIdent,        /* void (* PrintIdent)() */
  8,			/* int depth */
  {0, 0, 0},            /* xrgb weight */
  8,			/* int bitsPerPixel */
  PseudoColor,		/* int defaultVisual */
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
  MAX_W32_CLOCK,	/* int maxClock */
  0,			/* int videoRam */
  0xC0000,		/* int BIOSbase */
  0,			/* unsigned long MemBase, unused for this driver */
  240, 180,		/* int width, height */
  0,			/* unsigned long speedup */
  NULL,			/* DisplayModePtr modes */
  NULL,			/* MonPtr monitor */
  NULL,			/* char *clockprog */
  -1,                   /* int textclock */
  FALSE,		/* Bool bankedMono */
  "ET4000W32",		/* char *name */
  {0, 0, 0},		/* RgbRec blackColour */ 
  {0xFF, 0xFF, 0xFF},	/* RgbRec whiteColour */ 
  vga256ValidTokens,	/* int *validTokens */
  ET4000W32_PATCH_LEVEL,/* char *patchLevel */
  0,			/* int IObase */
  0,			/* int PALbase */
  0,			/* int COPbase */
  0,			/* int POSbase */
  0,			/* int instance */
  0,			/* int s3Madjust */
  0,			/* int s3Nadjust */
  0,			/* int s3MClk */
  0,			/* int chipId */
  0,			/* int chipRev */
  0,			/* unsigned long VGAbase */
  0,			/* int s3RefClk */
  -1,			/* int s3BlankDelay */
  0,			/* int textClockFreq */
  NULL,                 /* char* DCConfig */
  NULL,                 /* char* DCOptions */
  0	                /* int MemClk */
#ifdef XFreeXDGA
  ,0,			/* int directMode */
  NULL,			/* Set Vid Page */
  0,			/* unsigned long physBase */
  0			/* int physSize */
#endif
};

pointer vgaOrigVideoState = NULL;
pointer vgaNewVideoState = NULL;
pointer vgaBase = NULL;
pointer vgaVirtBase = NULL;
vgaPCIInformation *vgaPCIInfo = NULL;

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
Bool (* vgaValidModeFunc)(
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
int vgaMapSize;
void *vgaWriteBottom;
void *vgaWriteTop; 

int  vgaInterlaceType;
OFlagSet vgaOptionFlags;

int vgaIOBase;

static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

vgaHWCursorRec vgaHWCursor;

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

#define Drivers W32Drivers

extern vgaVideoChipPtr Drivers[];

/*
 *	Wrap the chip-level restore function in a protect/unprotect.
 */
static w32_mode = FALSE;
static void
vgaRestore(mode)
     pointer mode;
{
    vgaProtect(TRUE);
    (vgaRestoreFunc)(mode);
    vgaProtect(FALSE);
    if (w32_mode)
    {
	(*vgaSaveScreenFunc)(SS_FINISH);
    }
}


/*
 *     Print out identifying strings for drivers included in the server
 */
void
vgaPrintIdent()
{
  int            i, j, n, c;
  char		 *id;

  ErrorF("  %s: accelerated server for ET4000W32 graphics adaptors (Patchlevel %s):\n      ",
         vga256InfoRec.name, vga256InfoRec.patchLevel);

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
}


/*
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
  
  /* pixel multiplexing variables */
  Bool pixMuxPossible = FALSE;
  int nonMuxMaxClock = 0;
  int pixMuxMinClock = 0;
  int pixMuxNeeded = FALSE;
  int pixMuxMinWidth = 1024;
             

  /* Only supports 8bpp.  Check if someone is trying a different depth */
  if (vga256InfoRec.depth != 8)
    {
      ErrorF("\n%s %s: Unsupported depth (%d)\n",
	     XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.depth);
      return(FALSE);
    }

  /* First do a general PCI probe (unless disabled) */
  if (!OFLG_ISSET(OPTION_NO_PCI_PROBE, &vga256InfoRec.options)) {
    vgaPCIInfo = vgaGetPCIInfo();
  }

  for (i=0; Drivers[i]; i++)
  {
    vgaSaveScreenFunc = Drivers[i]->ChipSaveScreen;
    if ((Drivers[i]->ChipProbe)())
      {
        xf86ProbeFailed = FALSE;

	needmem = vga256InfoRec.videoRam * 1024;
	rounding = Drivers[i]->ChipRounding;

        if (xf86Verbose)
        {
	  ErrorF("%s %s: chipset:  %s\n",
             OFLG_ISSET(XCONFIG_CHIPSET,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
             vga256InfoRec.name,
             vga256InfoRec.chipset);
	  ErrorF("%s %s: videoram: %dk",
		 OFLG_ISSET(XCONFIG_VIDEORAM,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
                 vga256InfoRec.name,
	         vga256InfoRec.videoRam
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

        /* dacSpeeds option processing */
        
        if (vga256InfoRec.dacSpeeds[0] <= 0) {
          switch(W32RamdacType) {
            case NORMAL_DAC:
            case ATT20C47xA_DAC:
            case Sierra1502X_DAC:
            case ATT20C497_DAC:
            case ATT20C490_DAC:
            case ATT20C493_DAC:
            case ATT20C491_DAC:
            case ATT20C492_DAC:
                                vga256InfoRec.dacSpeeds[0] = MAX_W32_CLOCK;
                                break;
            case GENDAC_DAC:
            case ICS5341_DAC:
	    case STG1702_DAC:
            case STG1703_DAC:
            case ET6000_DAC:
                                vga256InfoRec.dacSpeeds[0] = 135000;
                                break;
            default:
               vga256InfoRec.dacSpeeds[0] = MAX_W32_CLOCK;
          }
        }

        if (xf86Verbose) {
          ErrorF("%s %s: Ramdac speed: %d\n",
                 OFLG_ISSET(XCONFIG_DACSPEED, &vga256InfoRec.xconfigFlag) ?
                 XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name,
                 vga256InfoRec.dacSpeeds[0] / 1000);
        }

        /* Check that maxClock is not higher than dacSpeeds */
        if (vga256InfoRec.maxClock > vga256InfoRec.dacSpeeds[0])
          vga256InfoRec.maxClock = vga256InfoRec.dacSpeeds[0];

        /* maxClock  = dacSpeeds , unless we use pixel multiplexing (?) */
          vga256InfoRec.maxClock = vga256InfoRec.dacSpeeds[0];

        if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
          if (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
          {
            ErrorF("%s %s: Using W32p programmable clock chip ICS5341\n",
                   OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions) ?
                   XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
            pixMuxPossible = TRUE;
            nonMuxMaxClock = MAX_W32_CLOCK;  /* or 75000 ? */ 
            pixMuxMinClock = 67500;
            pixMuxMinWidth = 1024;   /* seems to be this way: 1024x768 is wrong with pixmux -- something to do with byte/word/dword modes */
          } 
          else if (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
          {
            ErrorF("%s %s: Using W32p programmable clock chip STG1703\n",
                   OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions) ?
                   XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
            pixMuxPossible = TRUE;
            nonMuxMaxClock = MAX_W32_CLOCK;  /* or 75000 ? */ 
            pixMuxMinClock = 67500;
            pixMuxMinWidth = 1024;   /* seems to be this way: 1024x768 is wrong with pixmux -- something to do with byte/word/dword modes */
          }
          else if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))
          {
            ErrorF("%s %s: Using W32 programmable clock chip ICD2061a\n",
                   OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions) ?
                   XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
          }
          else if (OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions))
          {
            ErrorF("%s %s: Using ET6000 built-in programmable clock\n",
                   OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions) ?
                   XCONFIG_GIVEN : XCONFIG_PROBED, vga256InfoRec.name);
          }
          else
          {
            ErrorF("%s %s: Unsupported clock chip given for ET4000 W32\n",
                   XCONFIG_GIVEN, vga256InfoRec.name);
            return(FALSE);
          }
        }

	vgaEnterLeaveFunc = Drivers[i]->ChipEnterLeave;
	vgaInitFunc = Drivers[i]->ChipInit;
	vgaValidModeFunc = Drivers[i]->ChipValidMode;
	vgaSaveFunc = Drivers[i]->ChipSave;
	vgaRestoreFunc = Drivers[i]->ChipRestore;
	vgaWriteBottom = (pointer)Drivers[i]->ChipWriteBottom;
        vgaWriteTop = (pointer)Drivers[i]->ChipWriteTop;

	vga256InfoRec.AdjustFrame = vgaAdjustFunc = Drivers[i]->ChipAdjust;

	vgaMapSize = Drivers[i]->ChipMapSize;
	vgaInterlaceType = Drivers[i]->ChipInterlaceType;
	vgaOptionFlags = Drivers[i]->ChipOptionFlags;
	OFLG_SET(OPTION_POWER_SAVER, &vgaOptionFlags);
	OFLG_SET(OPTION_CLKDIV2, &vgaOptionFlags);
	OFLG_SET(OPTION_NO_PCI_PROBE, &vgaOptionFlags);

	xf86VerifyOptions(&vgaOptionFlags, &vga256InfoRec);

#ifdef DPMSExtension
	if (OFLG_ISSET(OPTION_POWER_SAVER, &vga256InfoRec.options))
	    DPMSEnabled = TRUE;
#endif

	/* if Virtual given: is the virtual size too big? */
	if (vga256InfoRec.virtualX > 0 &&
	    vga256InfoRec.virtualX * vga256InfoRec.virtualY > needmem)
	  {
	    ErrorF("%s: Too little memory for virtual resolution %d %d\n",
                   vga256InfoRec.name, vga256InfoRec.virtualX,
                   vga256InfoRec.virtualY);
            vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	  }

        maxX = maxY = -1;
	pMode = pEnd = vga256InfoRec.modes;
	if (pMode == NULL)
	  {
	    ErrorF("No modes supplied in XF86Config\n");
            vgaEnterLeaveFunc(LEAVE);
	    return(FALSE);
	  }
	do {
	  if (!xf86LookupMode(pMode, &vga256InfoRec, LOOKUP_DEFAULT))
	    {
              vgaEnterLeaveFunc(LEAVE);
	      return(FALSE);
	    }
	  if (pMode->HDisplay * pMode->VDisplay > needmem)
	    {
	      ErrorF("%s: Too little memory for mode %s\n", vga256InfoRec.name,
                     pMode->name);
              vgaEnterLeaveFunc(LEAVE);
	      return(FALSE);
	    }
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
            
#ifdef USE_PIXMUX
         /*
          * Check what impact each mode has on pixel multiplexing,
          * and mark those modes for which pixmux must be used.
          */
           
         if (pixMuxPossible) {
           if ((vga256InfoRec.clock[pMode->Clock] / 1000) > (nonMuxMaxClock / 1000)) {
               if (pMode->HDisplay >= pixMuxMinWidth)
                  /* pixmux SHOULD work in interlaced mode, but I don't know how to do it :-( */
                  if (!(pMode->Flags & V_INTERLACE))
                    {
                      pMode->Flags |= V_PIXMUX;
                      pixMuxNeeded = TRUE;
                      ErrorF("%s %s: Mode \"%s\" will use pixel multiplexing.\n",
                              XCONFIG_PROBED, vga256InfoRec.name, pMode->name);
                    }
            }
          }
#endif
	  pMode = pMode->next;
	}
	while (pMode != pEnd);

	tx = vga256InfoRec.virtualX;
	ty = vga256InfoRec.virtualY;

        vga256InfoRec.virtualX = max(maxX, vga256InfoRec.virtualX);
        vga256InfoRec.virtualY = max(maxY, vga256InfoRec.virtualY);

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
	if ((tx != vga256InfoRec.virtualX) || (ty != vga256InfoRec.virtualY))
            OFLG_CLR(XCONFIG_VIRTUAL,&vga256InfoRec.xconfigFlag);

#ifdef USE_PIXMUX
        if (pixMuxNeeded) {
           if (xf86Verbose)
           ErrorF("%s %s: Operating RAMDAC in pixel multiplex mode\n",
                  XCONFIG_PROBED, vga256InfoRec.name);
        }
#endif
        /*
         * For programmable clocks, fill in the SynthClock value
         * and set V_DBLCLK as required for each mode
         */

        if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
           /* First just copy the pixel values */
           pEnd = pMode = vga256InfoRec.modes;
           do {
              pMode->SynthClock = vga256InfoRec.clock[pMode->Clock];
              pMode = pMode->next;
           } while (pMode != pEnd);

#ifdef USE_PIXMUX
          /* Now make adjustments */
          pEnd = pMode = vga256InfoRec.modes;
          do {
             switch(W32RamdacType) {
             case ICS5341_DAC:
	     case STG1702_DAC:
             case STG1703_DAC:
                /*
                 * This one depend on pixel multiplexing for 8bpp.
                 */
                if (pMode->SynthClock > nonMuxMaxClock) {
                   pMode->SynthClock /= 2;
                   pMode->Flags |= V_DBLCLK;
                }
                break;
             default:
                /* Do nothing */
                break;
             }
             pMode = pMode->next;
          } while (pMode != pEnd);
#endif
        }      

        vga256InfoRec.displayWidth = vga256InfoRec.virtualX;
        if (xf86Verbose)
          ErrorF("%s %s: Virtual resolution set to %dx%d\n",
                 OFLG_ISSET(XCONFIG_VIRTUAL,&vga256InfoRec.xconfigFlag) ? 
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
                 vga256InfoRec.name,
                 vga256InfoRec.virtualX, vga256InfoRec.virtualY);

        vgaHWCursor.Initialized = FALSE;

#ifdef XFreeXDGA
        {
            vga256InfoRec.physBase = 0xA0000 + ET4000.ChipWriteBottom;
            vga256InfoRec.physSize = ET4000.ChipSegmentSize;
            vga256InfoRec.setBank = ET4000.ChipSetReadWrite;
        }
#endif

	/* Free PCI information */
	xf86cleanpci();
	if (vgaPCIInfo) {
	  vgaPCIInfo->ThisCard = (pciConfigPtr)NULL;
	  vgaPCIInfo->AllCards = (pciConfigPtr *)NULL;
	}

	return TRUE;
      }
  }

  vgaSaveScreenFunc = vgaHWSaveScreen;
  
  if (vga256InfoRec.chipset)
    ErrorF("%s: '%s' is an invalid chipset", vga256InfoRec.name,
	       vga256InfoRec.chipset);
  return FALSE;
}


/*
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
    vgaBase = xf86MapVidMem(scr_index, VGA_REGION, (pointer)0xA0000,
			    vgaMapSize);
    vgaVirtBase = (pointer)VGABASE;
    vgaWriteBottom = (void *)((unsigned int)vgaWriteBottom +
		     (unsigned int)vgaBase); 
    vgaWriteTop    = (void *)((unsigned int)vgaWriteTop +
		     (unsigned int)vgaBase);
  }

  if (!(vgaInitFunc)(vga256InfoRec.modes))
    FatalError("%s: hardware initialization failed\n", vga256InfoRec.name);

  /*
   * This function gets called while in graphics mode during a server
   * reset, and this causes the original video state to be corrupted.
   * So, only initialise vgaOrigVideoState if it hasn't previously been done
   * DHD Dec 1991.
   */
  if (!vgaOrigVideoState)
    vgaOrigVideoState = (pointer)(vgaSaveFunc)(vgaOrigVideoState);
  vgaRestore(vgaNewVideoState);

#ifndef DIRTY_STARTUP
  W32SaveScreen(NULL, FALSE); /* blank the screen */
#endif
  (vgaAdjustFunc)(vga256InfoRec.frameX0, vga256InfoRec.frameY0);

  /*
   * Take display resolution from the -dpi flag if specified
   */

  if (monitorResolution)
    displayResolution = monitorResolution;

  /*
   * Inititalize the dragon to color display
   */
  if (!cfbScreenInit(pScreen,
		     (pointer) vgaVirtBase,
		     vga256InfoRec.virtualX,
		     vga256InfoRec.virtualY,
		     displayResolution, displayResolution,
		     vga256InfoRec.displayWidth))
    return(FALSE);

  pScreen->CloseScreen = vgaCloseScreen;
  pScreen->SaveScreen = W32SaveScreen;
  pScreen->InstallColormap = vgaInstallColormap;
  pScreen->UninstallColormap = vgaUninstallColormap;
  pScreen->ListInstalledColormaps = vgaListInstalledColormaps;
  pScreen->StoreColors = vgaStoreColors;
  pScreen->whitePixel = 1;
  pScreen->blackPixel = 0;
  XF86FLIP_PIXELS();
  
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

  if (!cfbCreateDefColormap(pScreen))
    return(FALSE);


#ifndef DIRTY_STARTUP
    W32SaveScreen(NULL, TRUE); /* unblank the screen */

  /* Fill the screen with black pixels */
    if (serverGeneration == 1)
    {
	/* Depends on pixel size--GGLGGL*/
	W32_INIT_BOX(GXcopy, ~0, PFILL(pScreen->blackPixel), vga256InfoRec.virtualX - 1)
	W32_BOX(0, vga256InfoRec.virtualX, vga256InfoRec.virtualY)
	WAIT_XY
    }
#endif /* ! DIRTY_STARTUP */

  savepScreen = pScreen;

  return(TRUE);
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
      pspix = (PixmapPtr)pScreen->devPrivate;
    }

  if (enter)
    {
      w32_mode = TRUE;
      xf86MapDisplay(screen_idx, VGA_REGION);

      (vgaEnterLeaveFunc)(ENTER);

#ifdef XFreeXDGA
      if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
        /* Should we do something here or not ? */
      } else
#endif
      {
        vgaOrigVideoState = (pointer)(vgaSaveFunc)(vgaOrigVideoState);
      }

      XRamdac();
      vgaRestore(vgaNewVideoState);

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
	  pspix->devPrivate.ptr = (pointer)vgaVirtBase;
	  cfbDoBitbltCopy(&ppix->drawable, &pspix->drawable, GXcopy, &pixReg,
			  &pixPt, 0xFF);

        }
      }
      if (ppix) {
        (pScreen->DestroyPixmap)(ppix);
        ppix = NULL;
      }
    }
  else
    {
      w32_mode = FALSE;
      /* Make sure IO isn't disabled (by other drivers) */    
      xf86MapDisplay(screen_idx, VGA_REGION);

      (vgaEnterLeaveFunc)(ENTER);

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
	  cfbDoBitblt(&pspix->drawable, &ppix->drawable, GXcopy, &pixReg,
                      &pixPt, 0xFF);
	  pspix->devPrivate.ptr = ppix->devPrivate.ptr;
        }
        (vgaSaveFunc)(vgaNewVideoState);
      }
      /*
       * We come here in many cases, but one is special: When the server aborts
       * abnormaly. Therefore there MUST be a check whether vgaOrigVideoState
       * is valid or not.
       */
#ifdef XFreeXDGA
      if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
         (vgaEnterLeaveFunc)(LEAVE);
      } else
#endif
      { 
         if (vgaOrigVideoState)
  	    vgaRestore(vgaOrigVideoState);
         VGARamdac();

         (vgaEnterLeaveFunc)(LEAVE);

         xf86UnMapDisplay(screen_idx, VGA_REGION);
      }
    }
}


/*
 *  Ensure video is enabled when the server exits
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
 *  Set a new display mode
 */
Bool
vgaSwitchMode(mode)
     DisplayModePtr mode;
{
  if ((vgaInitFunc)(mode))
  {
    vgaRestore(vgaNewVideoState);
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
{
  return (vgaValidModeFunc)(mode, verbose, flag);
}
