/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3misc.c,v 3.14.2.4 1997/05/28 13:12:51 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Thomas Roell not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 *
 * THOMAS ROELL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL THOMAS ROELL OR KEVIN E. MARTIN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 *
 * Header: /home/src/xfree86/mit/server/ddx/xf86/accel/s3/RCS/s3.c,v 2.0
 * 1993/02/22 05:58:13 jon Exp
 *
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3misc.c /main/8 1996/10/27 11:47:15 kaleb $ */


#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "s3v.h"
#include "xf86_HWlib.h"
#include "ICD2061A.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"


#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#ifdef PC98
#include "s3pc98.h"
#endif

extern Bool s3NewMmio;
extern unsigned long s3MemBase;

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

extern ScreenPtr s3savepScreen;
static PixmapPtr ppix = NULL;
extern Bool  s3Localbus;
extern Bool  s3VLB;
extern Bool  s3LinearAperture;
extern int s3BankSize;
extern int s3DisplayWidth;
extern pointer vgaBase;
extern pointer vgaBaseLow;
extern pointer vgaBaseHigh;
extern pointer s3VideoMem;
extern pointer s3MmioMem;
extern unsigned char s3Port59;
extern unsigned char s3Port5A;
extern unsigned char s3Port31;

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;
extern int xf86Verbose;

static Bool AlreadyInited = FALSE;
static Bool s3ModeSwitched = FALSE;

#ifdef PC98
extern int pc98BoardType;
#endif

/*
 * s3Initialize -- Attempt to find and initialize a VGA framebuffer Most of
 * the elements of the ScreenRec are filled in.  The video is enabled for the
 * frame buffer...
 */

Bool
s3Initialize(scr_index, pScreen, argc, argv)
     int   scr_index;		/* The index of pScreen in the ScreenInfo */
     ScreenPtr pScreen;		/* The Screen to initialize */
     int   argc;		/* The number of the Server's arguments. */
     char **argv;		/* The arguments themselves. Don't change! */
{
   int displayResolution = 75;	/* default to 75dpi */
   extern int monitorResolution;
   int tmp;
   char *given;

   s3Unlock();	  /* for restarts */

   /* for clips */
   s3ScissB = ((s3InfoRec.videoRam * 1024) / s3BppDisplayWidth) - 1;
   if (s3ScissB > 2047) s3ScissB = 2047;
   s3ScissR = s3DisplayWidth - 1;

 /*
  * Initialize the screen, saving the original state for Save/Restore
  * function
  */

   if (serverGeneration == 1) {
      unsigned long addr = 0;
      unsigned long base0 = 0xa0000;   /* old default for s3Port59/s3Port5A */
      s3BankSize = 0x10000;
      s3LinApOpt = 0x14;

      /* First, map the vga window -- it is always required */
#ifndef PC98
      vgaBase = xf86MapVidMem(scr_index, VGA_REGION, (pointer)0xA0000,
			      s3BankSize);
#else
      switch(pc98BoardType & 0xf0 ){
      case PW:
      case PW805I:
            vgaBase = xf86MapVidMem(scr_index, VGA_REGION,
                                      (pointer)(PW_WinAdd << 16), s3BankSize);
            ErrorF("   PC98: PW mem-access\n");
	    break;
      case PCSKB:
      case PCHKB:
            vgaBase = xf86MapVidMem(scr_index, VGA_REGION,
                                      (pointer)(XKB_WinAdd << 16), s3BankSize);
            ErrorF("   PC98: SKB,HKB mem-access\n");
	    break;
      case PCSKB4:
            vgaBase = xf86MapVidMem(scr_index, VGA_REGION,
                                      (pointer)(SKB4_WinAdd << 16), s3BankSize);
            ErrorF("   PC98: SKB4 mem-access(not work,yet)\n");
	    break;
      case NECWAB:
            vgaBase = xf86MapVidMem(scr_index, VGA_REGION,
                                      (pointer)(NEC_WinAdd << 16), s3BankSize);
            ErrorF("   PC98: NEC-WAB(C Bus) mem-access\n");
	    break;
      case PWLB:
            vgaBase = xf86MapVidMem(scr_index, LINEAR_REGION,
                                      (pointer)((PWLB_WinAdd << 16) + 0xA0000),
							s3BankSize);
            ErrorF("   PC98: PW localbus mem-access\n");
	    break;
      default:
            vgaBase = xf86MapVidMem(scr_index, VGA_REGION,
				      (pointer)0xA0000,s3BankSize);
      }
#endif /* PC98 */

      /* s3Port59/s3Port5A need to be checked/initialized
	 before s3Init() is called the first time */

      if (s3Localbus
	  && xf86LinearVidMem()
	  && !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options)
	  && !OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
	 if (s3InfoRec.MemBase != 0) {
	    if (s3InfoRec.MemBase & 0x3ffffff) {
	       ErrorF("%s %s: base address not correctly aligned to 64MB\n",
		      XCONFIG_PROBED, s3InfoRec.name);
	       ErrorF("\t\tbase address changed from 0x%08lx to 0x%08lx\n",
		      s3InfoRec.MemBase, s3InfoRec.MemBase & 0x3ffffff);
	       s3InfoRec.MemBase &= ~0x3ffffff;
	    }
	    base0 = s3InfoRec.MemBase;
	 }
	 else if (s3MemBase != 0)  /* checked in s3.c */
	    base0 = s3MemBase;
	 else if (s3NewMmio) {
	    unsigned long orig_base0;
	    outb(vgaCRIndex, 0x59);
	    base0 = inb(vgaCRReg) << 24;
	    outb(vgaCRIndex, 0x5a);
	    base0 |= inb(vgaCRReg) << 16;
	    orig_base0 = base0;
	    base0 &= 0xfc000000;
	    if (base0 == 0 || base0 != orig_base0) {
	       /* the aligned address may clash with other devices,
		  so use a pretty random base address... */
	       base0 = 0xb4000000;  /* last resort */
	       ErrorF("%s %s: PCI: base address not correctly aligned\n",
		      XCONFIG_PROBED, s3InfoRec.name);
	       ErrorF("\t      base address changed from 0x%08lx to 0x%08lx\n",
		      orig_base0, base0);
	    }
	 }
	 else
	    base0 = 0xf3000000;  /* old default, not good for newmmio */
      }

      s3Port59 = base0 >> 24;
      s3Port5A = base0 >> 16;

      s3Init(s3InfoRec.modes);

      if (xf86LinearVidMem() &&
	  !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options)) {
	 /* Now, see if we can map a high buffer */
	 if (s3Localbus &&
	     !OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
	    Bool CachedFrameBuffer = FALSE;

	    s3InitEnvironment();

	    if (s3InfoRec.videoRam <= 1024) {
	       s3LinApOpt=0x15;
	    } else if (s3InfoRec.videoRam <= 2048) {
	       s3LinApOpt=0x16;
	    } else {
	       s3LinApOpt=0x17;
	    }
	    s3BankSize = s3InfoRec.videoRam * 1024;

	    if (s3InfoRec.MemBase != 0) {
	       addr = s3InfoRec.MemBase;
	       given = XCONFIG_GIVEN;
	    }
	    else {
	       outb(vgaCRIndex, 0x59);
	       addr = inb(vgaCRReg) << 8;
	       outb(vgaCRIndex, 0x5a);
	       addr |= inb(vgaCRReg);
	       addr <<= 16;
	       given = XCONFIG_PROBED;
	    }
	    addr &= 0xfc000000;

	    if (addr == 0)
	       addr = 0xb4000000;  /* last resort */

	    s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
				       (pointer)addr, s3BankSize);
	    s3MmioMem  = xf86MapVidMem(scr_index, MMIO_REGION,
				       (pointer)(addr+S3_NEWMMIO_REGBASE), S3_NEWMMIO_REGSIZE);

	    outb(vgaCRIndex, 0x53);	/* disable new mmio mode */
	    tmp = inb(vgaCRReg) & ~0x18;
	    outb(vgaCRReg, tmp);
	    outb (vgaCRIndex, 0x58);	/* disable linear mode */
	    outb (vgaCRReg, s3SAM256);
	    
	    outb(vgaCRIndex, 0x5a);
	    outb(vgaCRReg, (addr >> 16) & 0xff);
	    outb(vgaCRIndex, 0x59);
	    outb(vgaCRReg, (addr >> 24) & 0xff);
	    
	    outb(vgaCRIndex, 0x53);	/* enable new mmio mode */
	    tmp = inb(vgaCRReg) & ~0x18;
	    outb(vgaCRReg, tmp | 0x08);
	    outb (vgaCRIndex, 0x58);	/* enable linear mode */
	    outb (vgaCRReg, (s3LinApOpt & ~0x04) | s3SAM256);
	    
	    s3LinearAperture = TRUE;
	    ErrorF("%s %s: Local bus LAW is 0x%03lXxxxxx\n",
		   given, s3InfoRec.name, (addr >> 20));

            /* The UnMap can unmap all mapped regions, so make sure vgaBase is
	       still mapped */
	    xf86MapDisplay(scr_index, VGA_REGION);

	    if (!s3LinearAperture) {
	       ErrorF("%s %s: %s local bus framebuffer -",
		      XCONFIG_PROBED, s3InfoRec.name,
	              (CachedFrameBuffer? "Can't use": "Couldn't find"));
	       ErrorF(" linear access disabled\n");
	       s3BankSize = 0x10000;
	       s3VideoMem = NULL;
	       addr = 0xA0000;
	    }
         }
      }

      /* No linear mapping */
      if (!s3VideoMem) {
	 s3VideoMem = vgaBase;
	 addr = 0xA0000;
	 if (s3NewMmio) { /* doesn't work without linear mapping (yet?) */
	    ErrorF("%s %s: Chipset \"newmmio\" needs linear framebuffer\n",
		   XCONFIG_GIVEN, s3InfoRec.name);
	    outb(vgaCRIndex, 0x53);
            outb(vgaCRReg, 0x00);
            outb(vgaCRIndex, 0x58);
            outb(vgaCRReg, 0x00);
	 }
	 /* If using VGA aperture, set it up */
	 if (s3BankSize == 0x10000) {
#ifndef PC98
	    outb(vgaCRIndex, 0x59);
	    outb(vgaCRReg, 0x00);
	    outb(vgaCRIndex, 0x5a);
	    outb(vgaCRReg, 0x0a);
#else
	    switch( pc98BoardType & 0xf0 ){
		case PCSKB4:
		    outb(vgaCRIndex, 0x59);
		    outb(vgaCRReg, SKB4_WinAdd >> 8);
		    outb(vgaCRIndex, 0x5a);
		    outb(vgaCRReg, SKB4_WinAdd & 0xff);
		    addr = SKB4_WinAdd << 16;
		    break;
		case NECWAB:
		    outb(vgaCRIndex, 0x59);
		    outb(vgaCRReg, 0x00);
		    outb(vgaCRIndex, 0x5a);
		    outb(vgaCRReg, NEC_WinAdd );
		    addr = NEC_WinAdd << 16;
		    break;
		default:
		    outb(vgaCRIndex, 0x59);
		    outb(vgaCRReg, 0x00);
		    outb(vgaCRIndex, 0x5a);
		    outb(vgaCRReg, 0x0a);
	    }
#endif
	    s3LinApOpt = 0x14;
	 }
      }

#ifdef XFreeXDGA
      s3InfoRec.physBase = addr;
      s3InfoRec.physSize = s3BankSize;
#endif


      /* Save CR59, CR5A for future calls to s3Init() */
      outb(vgaCRIndex, 0x59);
      s3Port59 = inb(vgaCRReg);
      outb(vgaCRIndex, 0x5A);
      s3Port5A = inb(vgaCRReg);
   } else
      s3Init(s3InfoRec.modes);

   /* BL: for new mmio we enable linear mode */
   if (s3NewMmio) {
      unsigned char tmp;
      unsigned long ltmp;
      WaitIdle();
      outb(vgaCRIndex, 0x39);
      outb(vgaCRReg, 0xa5);
      outb (vgaCRIndex, 0x53);
      tmp = inb(vgaCRReg);
      outb (vgaCRReg, tmp | 0x08);	/* new mmio enabled */

      outb (vgaCRIndex, 0x58);
      outb (vgaCRReg, (s3LinApOpt & ~0x4) | s3SAM256);  /* window size for linear mode */

      *(volatile int *)s3VideoMem = 0x12345678;
      if ((ltmp = *(volatile int *)s3VideoMem) != 0x12345678)
	ErrorF("new mmio linear mode doesn't work ltmp %x\n",ltmp);
   }
   /* end BL */
   				    
   			/* new - KJB */
   s3InitSTREAMS( s3InfoRec.modes );
   s3InitEnvironment();
   AlreadyInited = TRUE;

   s3ImageInit();

   xf86InitCache(s3CacheMoveBlock);
   s3FontCache8Init();

   /*
    * Take display resolution from the -dpi flag if specified
    */

   if (monitorResolution)
      displayResolution = monitorResolution;

   if (!s3ScreenInit(pScreen,
		     (pointer) s3VideoMem,
		     s3InfoRec.virtualX, s3InfoRec.virtualY,
		     displayResolution, displayResolution,
		     s3DisplayWidth))
      return (FALSE);

   switch (s3InfoRec.bitsPerPixel) {
   case 8:
      pScreen->InstallColormap = s3InstallColormap;
      pScreen->UninstallColormap = s3UninstallColormap;
      pScreen->ListInstalledColormaps = s3ListInstalledColormaps;
      pScreen->StoreColors = s3StoreColors;
      break;
   case 16:
   case 24:
   case 32:
      pScreen->InstallColormap = cfbInstallColormap;
      pScreen->UninstallColormap = cfbUninstallColormap;
      pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
      pScreen->StoreColors = (void (*)())NoopDDA;
   }
   pScreen->QueryBestSize = s3QueryBestSize;
   xf86PointerScreenFuncs.WarpCursor = s3WarpCursor;
   (void)s3CursorInit(0, pScreen);

   pScreen->CloseScreen = s3CloseScreen;
   pScreen->SaveScreen = s3SaveScreen;

   s3savepScreen = pScreen;
   return (cfbCreateDefColormap(pScreen));

}


/*
 * s3EnterLeaveVT -- grab/ungrab the current VT completely.
 */

void
s3EnterLeaveVT(enter, screen_idx)
     Bool  enter;
     int screen_idx;
{
   PixmapPtr pspix=NULL;
   ScreenPtr pScreen = s3savepScreen;

   if (!xf86Exiting && !xf86Resetting) {
      /* cfbGetScreenPixmap(pScreen) */
        switch (s3InfoRec.bitsPerPixel) {
        case 8:
            pspix = (PixmapPtr)pScreen->devPrivate;
            break;
        case 16:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr;
            break;
        case 24:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb24ScreenPrivateIndex].ptr;
            break;
        case 32:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
            break;
	}
   }

   if (enter) {
#ifdef PC98
      s3EnterLeaveMachdep(S3PC98SERVER_ENTER);
#endif
      xf86MapDisplay(screen_idx, VGA_REGION);
      if (s3VideoMem != vgaBase)
	 xf86MapDisplay(screen_idx, LINEAR_REGION);

      if (!xf86Resetting) {
	 ScrnInfoPtr pScr = XF86SCRNINFO(pScreen);

         s3Unlock();
	 s3Init(s3InfoRec.modes);
         s3InitSTREAMS(s3InfoRec.modes);
         s3InitEnvironment();
	 AlreadyInited = TRUE;
	 s3RestoreDACvalues();
	 s3ImageInit();
   	 s3FontCache8Init();
	 s3RestoreColor0(pScreen);
         (void)s3CursorInit(0, pScreen);
	 s3RestoreCursor(pScreen);
	 s3AdjustFrame(pScr->frameX0, pScr->frameY0);


	 if ((pointer)pspix->devPrivate.ptr != s3VideoMem && ppix) {
	    pspix->devPrivate.ptr = s3VideoMem;
	    (*s3ImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
			        ppix->devPrivate.ptr,
			        PixmapBytePad(s3DisplayWidth,
					      pScreen->rootDepth),
			        0, 0, s3alu[GXcopy], ~0);
	 }
      }
      if (ppix) {
	 (pScreen->DestroyPixmap)(ppix);
	 ppix = NULL;
      }
   } else {
      /* Make sure that another driver hasn't disabled IO */
      xf86MapDisplay(screen_idx, VGA_REGION);
      if (s3VideoMem != vgaBase)
	 xf86MapDisplay(screen_idx, LINEAR_REGION);

      if (!xf86Exiting) {
	 ppix = (pScreen->CreatePixmap)(pScreen, s3DisplayWidth,
					pScreen->height, pScreen->rootDepth);
	 if (ppix) {
	    (*s3ImageReadFunc)(0, 0, pScreen->width, pScreen->height,
			       ppix->devPrivate.ptr,
			       PixmapBytePad(s3DisplayWidth,
					     pScreen->rootDepth),
			       0, 0, ~0);
	    pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	 }
      }
#ifdef XFreeXDGA
      if (s3InfoRec.directMode & XF86DGADirectGraphics) {
	s3HideCursor();
      } else
#endif
        if (AlreadyInited) {
	  s3CleanUp();
          AlreadyInited = FALSE;
        }

      xf86UnMapDisplay(screen_idx, VGA_REGION);
      if (s3VideoMem != vgaBase)
         xf86UnMapDisplay(screen_idx, LINEAR_REGION);
#ifdef XFreeXDGA
      if (s3InfoRec.directMode & XF86DGADirectGraphics) {
        /* make sure we are in linear mode */
      }
#endif
#ifdef PC98
   s3EnterLeaveMachdep(S3PC98SERVER_LEAVE);
#endif
   }
}

/*
 * s3CloseScreen -- called to ensure video is enabled when server exits.
 */

Bool
s3CloseScreen(screen_idx, pScreen)
     int screen_idx;
     ScreenPtr pScreen;
{

 /*
  * Hmm... The server may shut down even if it is not running on the current
  * vt. Let's catch this case here.
  */
   xf86Exiting = TRUE;
   if (xf86VTSema)
      s3EnterLeaveVT(LEAVE, screen_idx);
   else if (ppix) {
      /*
       * 7-Jan-94 CEG: The server is not running on the current vt.
       * Free the screen snapshot taken when the server vt was left.
       */
      (s3savepScreen->DestroyPixmap)(ppix);
      ppix = NULL;
   }
   return (TRUE);
}

/*
 * s3SaveScreen -- blank the screen.
 */

Bool
s3SaveScreen(pScreen, on)
     ScreenPtr pScreen;
     Bool  on;
{
   unsigned char scrn;

   if (on)
      SetTimeSinceLastInputEvent();

   if (xf86VTSema) {
      /* the server is running on the current vt */
      /* so just go for it */

      outb(0x3C4,1);
      scrn = inb(0x3C5);

      if (on) {
	 scrn &= 0xDF;			/* enable screen */
      } else {
	 scrn |= 0x20;			/* blank screen */
      }

      outw(0x3C4, 0x0100);              /* syncronous reset */
      outw(0x3C4, (scrn << 8) | 0x01); /* change mode */
      outw(0x3C4, 0x0300);              /* end reset */
   }
   return (TRUE);
}

/*
 * s3DPMSSet -- Sets VESA Display Power Management Signaling (DPMS) Mode
 */

void
s3DPMSSet(PowerManagementMode)
    int PowerManagementMode;
{
#ifdef DPMSExtension
  unsigned char seq1, extsync;
  if (!xf86VTSema) return;
  switch (PowerManagementMode)
    {
    case DPMSModeOn:
      /* Screen: On; HSync: On, VSync: On */
      seq1 = 0x00;
      extsync = 0x00;
      break;
    case DPMSModeStandby:
      /* Screen: Off; HSync: Off, VSync: On */
      seq1 = 0x20;
      extsync = 0x10;
      break;
    case DPMSModeSuspend:
      /* Screen: Off; HSync: On, VSync: Off */
      seq1 = 0x20;
      extsync = 0x40;
      break;
    case DPMSModeOff:
      /* Screen: Off; HSync: Off, VSync: Off */
      seq1 = 0x20;
      extsync = 0x50;
      break;
    }
  outw(0x3C4, 0x0608);	/* Unlock SRD */
  outw(0x3C4, 0x0100);	/* Synchronous Reset */
  outb(0x3C4, 0x01);	/* Select SEQ1 */
  seq1 |= inb(0x3C5) & ~0x20;
  outb(0x3C5, seq1);
  outb(0x3C4, 0x0D);	/* Select SEQD */
  extsync |= inb(0x3C5) & ~0xF0;
  usleep(10000);
  outb(0x3C5, extsync);
  outw(0x3C4, 0x0300);	/* End Reset */
  outw(0x3C4, 0x0008);	/* Lock SRD */
#endif
}

static debugcache = 0;

Bool
s3SwitchMode(mode)
     DisplayModePtr mode;
{
   WaitIdle();  /* wait till idle */

   if (OFLG_ISSET(OPTION_SHOWCACHE, &s3InfoRec.options)) {
      debugcache++;
      if (debugcache & 1) {
        mode = mode->prev;
        s3InfoRec.modes=mode;
      }
   }

   if (s3Init(mode)) {	  
   			/* new - KJB */
      s3InitSTREAMS(mode);
      s3ModeSwitched = TRUE;
#if notyet
      s3Restore(vgaNewVideoState);
#endif
      s3ImageInit(); /* mode switching changes the memory bank */
      s3RestoreCursor(s3savepScreen);
      return (TRUE);
   } else {
      ErrorF("Mode switch failed because of hardware initialisation error\n");
      s3ImageInit();
      return (FALSE);
   }
}

void
s3AdjustFrame(int x, int y)
{
   int   Base, origBase;
   unsigned char tmp;
   extern int s3AdjustCursorXPos;  /* for s3Cursor.c */
   extern int s3HDisplay;
   extern unsigned char s3Port51;

   if (OFLG_ISSET(OPTION_SHOWCACHE, &s3InfoRec.options)) {
      if ( debugcache & 1)
         y += 512;
   }

   if (x > s3DisplayWidth - s3HDisplay)
      x  = s3DisplayWidth - s3HDisplay;

   /* so may S3 cards have problems with some odd base addresses,
    * to catch them all only even base values will be used.
    */

   origBase = (y * s3DisplayWidth + x) * realS3Bpp /*s3Bpp*/;
   Base = (origBase >> 2) & ~1;

    /* STREAMS handler */
    if ( s3InfoRec.bitsPerPixel == 32 || 
         s3InfoRec.bitsPerPixel == 24 ) { 
      if ( s3MmioMem != NULL ) {
         #if 0
				/* Wait for VSYNC */
         /* while ((inb(vgaIOBase + 0x0A) & 0x08) == 0x00) ; */
 				/* X and Y start coords + 1. */
  	   #if 1
         ((mmtr)s3MmioMem)->streams_regs.regs.prim_start_coord = /* 0x00010001; */
           ( (x + 1) << 16 ) | (y + 1) & 0xf800f800;
	   #else
         ((mmtr)s3MmioMem)->streams_regs.regs.second_start_coord = 
           ( (0x7ff - x) << 16 ) | (0x7ff - y) & 0xf800f800;
	   #endif
	 #else
	 if (S3_ViRGE_VX_SERIES(s3ChipId))
	   ((mmtr)s3MmioMem)->streams_regs.regs.prim_fbaddr0 = 
	     ((y * s3DisplayWidth + (x & ~7)) * realS3Bpp/*s3Bpp*/);
	 else
	   ((mmtr)s3MmioMem)->streams_regs.regs.prim_fbaddr0 = 
	     ((y * s3DisplayWidth + (x & ~3)) * realS3Bpp/*s3Bpp*/) /* & ~3 */;
         #endif
         }
    } else {
	   outb(vgaCRIndex, 0x31);
	   outb(vgaCRReg, ((Base & 0x030000) >> 12) | s3Port31);
	   s3Port51 &= ~0x03;
	   s3Port51 |= ((Base & 0x0c0000) >> 18);
	   outb(vgaCRIndex, 0x51);
	   /* Don't override current bank selection */
	   tmp = (inb(vgaCRReg) & ~0x03) | (s3Port51 & 0x03);
	   outb(vgaCRReg, tmp);
	
	   outw(vgaCRIndex, (Base & 0x00FF00) | 0x0C);
	   outw(vgaCRIndex, ((Base & 0x00FF) << 8) | 0x0D);
    } /* if!(24bpp) */

#ifdef XFreeXDGA
   if (!(s3InfoRec.directMode & XF86DGADirectMouse)) {
#endif
      s3AdjustCursorXPos = (origBase - (Base << 2)) / realS3Bpp/*s3Bpp*/;

      if (s3ModeSwitched) {
         s3ModeSwitched = FALSE;
         s3RestoreCursor(s3savepScreen);
      } else {
         s3RepositionCursor(s3savepScreen);
      }
#ifdef XFreeXDGA
   }
#endif

#ifdef XFreeXDGA
   if (s3InfoRec.directMode & XF86DGADirectGraphics) {
      /* Wait until vertical retrace is in progress. */
      while (inb(vgaIOBase + 0xA) & 0x08);
      while (!(inb(vgaIOBase + 0xA) & 0x08));
   }
#endif
}


void s3SetVidPage(vPage)
   int vPage;
{
}
