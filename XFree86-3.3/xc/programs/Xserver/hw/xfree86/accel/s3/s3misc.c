/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3misc.c,v 3.68.2.2 1997/05/19 08:06:54 dawes Exp $ */
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
/* $XConsortium: s3misc.c /main/36 1996/12/28 14:42:12 kaleb $ */


#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "s3.h"
#include "regs3.h"
#include "xf86_HWlib.h"
#include "ICD2061A.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "s3linear.h"

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
 
extern char s3Mbanks;
extern Bool s3Mmio928;
extern Bool s3NewMmio;
extern unsigned long s3MemBase;

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

static Bool s3TryAddress(
#if NeedFunctionPrototypes
     long *	/* addr */,
     long	/* value */,
     long	/* physaddr */,
     int	/* stage */
#endif
);

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


static void s3WaitRetrace()
{
      while (inb(vgaIOBase + 0xA) & 0x08);
      while (!(inb(vgaIOBase + 0xA) & 0x08));
}


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
   int i,j,k,ok;
   unsigned char *pat;
   unsigned short dash_test_pattern = 0xac00;
   Pixel blackPixel;

   if (xf86FlipPixels && s3Bpp == 1)
      blackPixel = (Pixel) 1;
   else
      blackPixel = (Pixel) 0;

   s3Unlock();	  /* for restarts */
   
   /* for clips */
   s3ScissB = ((s3InfoRec.videoRam * 1024) / s3BppDisplayWidth) - 1;
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

      if ((OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	   OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) &&
	  !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options) &&
	  s3Mmio928) {
	vgaBaseLow = xf86MapVidMem(scr_index, VGA_REGION,
				   (pointer)0xA0000, s3BankSize);
	vgaBaseHigh = xf86MapVidMem(scr_index, EXTENDED_REGION,
				    (pointer)0x7C0A0000, s3BankSize);
	vgaBase = vgaBaseLow;
      } else {
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
      }

      /* s3Port59/s3Port5A need to be checked/initialized
	 before s3Init() is called the first time */

      if (S3_801_928_SERIES (s3ChipId)
	  && s3Localbus
	  && xf86LinearVidMem()
	  && !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options)
	  && !OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
	 if (S3_x64_SERIES(s3ChipId)) 
	    if (s3InfoRec.MemBase != 0) {
	       if ((s3InfoRec.MemBase & 0x3ffffff) && s3NewMmio) {
		  ErrorF("%s %s: base address not correctly aligned to 64MB\n",
			 XCONFIG_PROBED, s3InfoRec.name);
		  ErrorF("\t\tbase address changed from 0x%08lx to 0x%08lx\n",
			 s3InfoRec.MemBase, s3InfoRec.MemBase & ~0x3ffffff);
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
		     so use a pretty random base address as last resort,
		     0x04000000 seems to be a necessary  for some VLB boards
		     but won't work with 128MB though :-( */
		  base0 = 0x04000000;
		  ErrorF("%s %s: PCI: base address not correctly aligned\n",
			 XCONFIG_PROBED, s3InfoRec.name);
		  ErrorF("\t      base address changed from 0x%08lx to 0x%08lx\n",
			 orig_base0, base0);
	       }
	    }
	    else 
	       base0 = 0xf3000000;  /* old default, not good for newmmio */
	 else {
#ifdef PC98_PWLB
	    if (pc98BoardType == PWLB)
	       base0 = 0x0;
	    else
#endif
	    base0 = 0x03000000;
	 }
      }

      s3Port59 = base0 >> 24;
      s3Port5A = base0 >> 16;

      s3Init(s3InfoRec.modes);

      if (xf86LinearVidMem() &&
	  !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options)) {
	 /* Now, see if we can map a high buffer */
	 if (s3Localbus && !S3_911_SERIES(s3ChipId) &&
	     !OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
	    long i2;
	    long *poker;
	    unsigned long pVal;
	    Bool CachedFrameBuffer = FALSE;

	    pVal = 0x12345678;

	    s3InitEnvironment();
	    s3ImageWriteNoMem(0, 0, s3Bpp==3 ? 2 : 4 / s3Bpp, 1, (char *) &pVal, 
			      s3Bpp==3 ? 2 : 4 / s3Bpp,
			      0, 0, (short) s3alu[GXcopy], ~0);	 

	    if (S3_801_928_SERIES (s3ChipId)) {
	       outb(vgaCRIndex, 0x59);
	       outb(vgaCRReg, s3Port59);
	       outb(vgaCRIndex, 0x5a);
	       outb(vgaCRReg, s3Port5A);

	       if (s3InfoRec.videoRam <= 1024) {
		  s3LinApOpt=0x15;
	       } else if (s3InfoRec.videoRam <= 2048) {
		  s3LinApOpt=0x16;
	       } else {
		  s3LinApOpt=0x17;
	       }
       	       s3BankSize = s3InfoRec.videoRam * 1024;
	       s3EnableLinear();
	    }
	 
	    /*
	     * XXXX This is for debugging only.  It attempts to find
	     * which values of LAW are decoded (see s3TryAddress() below).
	     */
	    if (OFLG_ISSET(OPTION_FB_DEBUG, &s3InfoRec.options)
		&& !s3NewMmio) {  /* don't poke around for newmmio */
	       for (i2 = 0xff; i2 >= 3; i2--) { /* 4080Mb..48Mb stepsize 16Mb */
		  addr = (i2 << 24);

	          s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
				          (pointer)addr, 4096);
	          poker = (long *) s3VideoMem; 
	          s3TryAddress(poker, pVal, addr, 0);
	          xf86UnMapVidMem(scr_index, LINEAR_REGION, s3VideoMem, 4096);
	       }
	    }
		  
	    /*
	     * If a MemBase value was given in the XF86Config, skip the LAW
	     * probe and use the high 10 bits for the hw part of LAW.
	     * Normally only 6 bits are set in hw, but the Diamond Stealth
	     * Pro is different.
	     */
	    if (s3InfoRec.MemBase != 0) {
	       addr = (s3InfoRec.MemBase & 0xffc00000);
	       s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
					  (pointer)addr, s3BankSize);
	       if (s3NewMmio)
		  s3MmioMem  = xf86MapVidMem(scr_index, MMIO_REGION,
					     (pointer)(addr+S3_NEWMMIO_REGBASE), S3_NEWMMIO_REGSIZE);
	       s3DisableLinear();
	       outb(vgaCRIndex, 0x5a);
	       if (S3_x64_SERIES(s3ChipId)) {
		  outb(vgaCRReg, (addr >> 16) & 0xff);
		  outb(vgaCRIndex, 0x59);
		  outb(vgaCRReg, (addr >> 24) & 0xff);
	       }
	       else 
		  outb(vgaCRReg, 0xc0);
	       s3EnableLinear();

	       s3LinearAperture = TRUE;
	       ErrorF("%s %s: Local bus LAW is 0x%03lXxxxxx\n", 
		      XCONFIG_GIVEN, s3InfoRec.name, (addr >> 20));
	    } else {
	       if (S3_x64_SERIES(s3ChipId) && 
		   !(S3_TRIO64_SERIES(s3ChipId) &&
		     /*s3InfoRec.videoRam > 1024 &&*/ s3VLB)) {
		  /* So far, only tested for the PCI ELSA W2000Pro */
		  s3DisableLinear();
	          outb(vgaCRIndex, 0x59);
	          addr = inb(vgaCRReg) << 8;
	          outb(vgaCRIndex, 0x5a);
	          addr |= inb(vgaCRReg);
	          addr <<= 16;
		  s3EnableLinear();

                  if (OFLG_ISSET(OPTION_FB_DEBUG, &s3InfoRec.options)) {
		     ErrorF("Read LAW as 0x%08X \n", addr);
	          }
		  s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
                                             (pointer)addr, s3BankSize);
		  if (s3NewMmio)
		     s3MmioMem  = xf86MapVidMem(scr_index, MMIO_REGION,
						(pointer)(addr+S3_NEWMMIO_REGBASE), S3_NEWMMIO_REGSIZE);
		  poker = (long *) s3VideoMem;
		  if (s3TryAddress(poker, pVal, addr, 1)) {
		     s3LinearAperture = TRUE;
		     if (xf86Verbose) {
			ErrorF("%s %s: Local bus LAW is 0x%08lX\n", 
			       XCONFIG_PROBED, s3InfoRec.name, addr);
		     }
		  } else {
		     s3LinearAperture = FALSE;
		     CachedFrameBuffer = TRUE;
		     if (xf86Verbose) {
			ErrorF("%s %s: Local bus LAW is 0x%08lX %s\n", 
			       XCONFIG_PROBED, s3InfoRec.name, addr,
			       "but linear fb not usable");
		     }
		     xf86UnMapVidMem(scr_index, LINEAR_REGION, s3VideoMem,
				     s3BankSize);
		     if (s3NewMmio)
			xf86UnMapVidMem(scr_index, MMIO_REGION, s3MmioMem,
					S3_NEWMMIO_REGSIZE);
		  }
	       } else {
#ifdef PC98_PWLB
		 if (pc98BoardType == PWLB) {
		     unsigned long addr = (PWLB_WinAdd << 16) + 0x400000;

		     s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
						(pointer)addr, s3BankSize);
		     if (s3NewMmio)
			s3MmioMem = xf86MapVidMem(scr_index, MMIO_REGION,
						  (pointer)(addr+S3_NEWMMIO_REGBASE), S3_NEWMMIO_REGSIZE);
		     poker = (long *) s3VideoMem; 

		     if (s3TryAddress(poker, pVal, addr, 1)) {
			ErrorF("%s %s: Local bus LAW is 0x%08X\n", 
			     XCONFIG_PROBED, s3InfoRec.name, addr);
			s3LinearAperture = TRUE;
		     }
		 } else {
#endif
	          for (i2 = 0xff; i2 >= 3; i2--) { /* 4080Mb..48Mb stepsize 16Mb */
		     addr = (i2 << 24);

		     s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
						(pointer)addr, s3BankSize);
		     if (s3NewMmio)
			s3MmioMem = xf86MapVidMem(scr_index, MMIO_REGION,
						  (pointer)(addr+S3_NEWMMIO_REGBASE), S3_NEWMMIO_REGSIZE);
		     poker = (long *) s3VideoMem; 

		     if (s3TryAddress(poker, pVal, addr, 1)) {
			/* We found some ram, but is it ours? */
	       
			s3DisableLinear();
			/* move it up by 12MB */
			outb(vgaCRIndex, 0x5a);
			outb(vgaCRReg, 0xC0);
			s3EnableLinear();

			if (!s3TryAddress(poker, pVal, addr, 2)) {
			   addr += (0x0C<<20);
			   xf86UnMapVidMem(scr_index, LINEAR_REGION,
					   s3VideoMem, s3BankSize);
			   s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
						      (pointer)addr,
						      s3BankSize);
			   if (s3NewMmio) {
			      xf86UnMapVidMem(scr_index, MMIO_REGION,
					      s3MmioMem, S3_NEWMMIO_REGSIZE);
			      s3MmioMem = xf86MapVidMem(scr_index, MMIO_REGION,
							(pointer)(addr+S3_NEWMMIO_REGBASE),
							S3_NEWMMIO_REGSIZE);
			   }
		     
			   if (s3TryAddress((long *)s3VideoMem, pVal,
					    addr, 3)) {
			      ErrorF("%s %s: Local bus LAW is 0x%03Xxxxxx\n", 
				     XCONFIG_PROBED, s3InfoRec.name,
				     (i2 << 4 | 0x0c));
			      s3LinearAperture = TRUE;
			      break;
			   }
			   /*
			    * Special test for Diamond Stealth Pro.
			    * On this card, bits 6 and 7 of register 0x5a
			    * control address bits A29 and A26 rather than
			    * A22 and A23. Don't ask we why.
			    */
			   addr = (i2 << 24) | 0x24000000;
			   xf86UnMapVidMem(scr_index, LINEAR_REGION,
					   s3VideoMem, s3BankSize);
			   s3VideoMem = xf86MapVidMem(scr_index, LINEAR_REGION,
						      (pointer)addr,
						      s3BankSize);
			   if (s3NewMmio) {
			      xf86UnMapVidMem(scr_index, MMIO_REGION,
					      s3MmioMem, S3_NEWMMIO_REGSIZE);
			      s3MmioMem = xf86MapVidMem(scr_index, MMIO_REGION,
							(pointer)(addr+S3_NEWMMIO_REGBASE),
							S3_NEWMMIO_REGSIZE);
			   }
			   if (s3TryAddress((long *)s3VideoMem, pVal,
					    addr, 4)) {
			      ErrorF("%s %s: Local bus LAW is 0x%03Xxxxxx\n", 
				     XCONFIG_PROBED, s3InfoRec.name,
				     (i2 | 0x24) << 4);
			      s3LinearAperture = TRUE;
			      break;
			   }
			} else {
			   ErrorF("%s %s: linear framebuffer found, but",
				  XCONFIG_PROBED, s3InfoRec.name);
			   ErrorF(" it appears to be in a cachable\n");
			   ErrorF("\t address range.\n");
			   CachedFrameBuffer = TRUE;
			}
			s3DisableLinear();
			outb(vgaCRIndex, 0x5a);
			outb(vgaCRReg, 0x00);	/* reset for next probe */
			s3EnableLinear();
		     }
		     xf86UnMapVidMem(scr_index, LINEAR_REGION, s3VideoMem,
				     s3BankSize);
		     if (s3NewMmio)
			xf86UnMapVidMem(scr_index, MMIO_REGION, s3MmioMem,
					S3_NEWMMIO_REGSIZE);
		  }
#ifdef PC98_PWLB
		 }
#endif
	       }
	    }

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
            s3DisableLinear();
         }
      }

      /* No linear mapping */
      if (!s3VideoMem) {
	 s3VideoMem = vgaBase;
	 addr = 0xA0000;
	 if (s3NewMmio) {  /* doesn't work without linear mapping (yet?) */
	    ErrorF("%s %s: Chipset \"newmmio\" needs linear framebuffer\n",
		       XCONFIG_GIVEN, s3InfoRec.name);
	    ErrorF("\t\tplease specify Chipset \"mmio_928\"\n");
	    /* disable mmio and LB, try to avoid crashing the PC */
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
      outb (vgaCRReg, tmp | 0x18);	/* old and new mmio enabled */
     
      outb (vgaCRIndex, 0x58);
      outb (vgaCRReg, (s3LinApOpt & ~0x4) | s3SAM256);  /* window size for linear mode */

      *(volatile int *)s3VideoMem = 0x12345678;
      if ((ltmp = *(volatile int *)s3VideoMem) != 0x12345678)
	ErrorF("new mmio linear mode doesn't work ltmp %x\n",ltmp);
   }
   /* end BL */

   s3InitEnvironment();
   AlreadyInited = TRUE;

   s3ImageInit();


   /* avoid bogus bug messages for old chips */
   if (serverGeneration == 1 && S3_x64_SERIES(s3ChipId)) {

      /* now testing Trio32 BITBLT bug 
       * using a 2*2 pixel pattern BLT from (0,0) to (0,2) */

      pat = (unsigned char*) xcalloc(s3Bpp, 2 * 2);
   
      /* init source pattern:   2*2 pixel checker pattern (glyph source) */
      pat[0 * s3Bpp] = pat[3 * s3Bpp] = 0;
      pat[1 * s3Bpp] = pat[2 * s3Bpp] = 1;
      (*s3ImageWriteFunc)(0, 0, 2, 2, (char*)pat, 2 * s3Bpp, 
			  0, 0, (short) s3alu[GXcopy], ~0);
   
      /* first test original BITBLT; then workaround */
      for (s3Trio32FCBug = 0; s3Trio32FCBug < 2; s3Trio32FCBug++) {

	 /* init destination (all bits set) */
	 for(i = 0; i < s3Bpp * 2 * 2; i++) 
	    pat[i] = (char)0xff;
	 s3ImageWriteNoMem(0, 2, 2, 2, (char*)pat, 2 * s3Bpp,
			   0, 0, (short) s3alu[GXcopy], ~0);

	 BLOCK_CURSOR;

	 if (s3Trio32FCBug) {
	    WaitQueue16_32(2, 3);
	    SET_BKGD_MIX(BSS_BKGDCOL | MIX_OR);
	    SET_BKGD_COLOR(0);
	 } else {
	    WaitQueue(1);
	    SET_BKGD_MIX(BSS_BKGDCOL | MIX_DST);
	 }

	 WaitQueue16_32(3, 6);
	 SET_FRGD_COLOR(1);
	 SET_RD_MASK(1);
	 SET_WRT_MASK(~0);

	 WaitQueue(5);
	 SET_PIX_CNTL(MIXSEL_EXPBLT | COLCMPOP_F);
	 SET_FRGD_MIX(FSS_FRGDCOL | MIX_SRC);
	 SET_CURPT(0,0);
	 
	 WaitQueue(5);
	 SET_DESTSTP(0,2);
	 SET_AXIS_PCNT(1, 1);

	 SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

	 /* read back the destination */
	 WaitIdle();
	 (*s3ImageReadFunc)(0, 2, 2, 2, (char*)pat, 2 * s3Bpp, 0, 0, ~0);

#ifdef DEBUG_TRI32_BITBLT_TEST
	 for(i = 0; i < s3Bpp * 2 * 2; i++)
	    ErrorF("%s %s: BITBLT %2d %02x\n",
		   XCONFIG_PROBED, s3InfoRec.name, i, pat[i]&0xff);
	 ErrorF("\n");

	 ErrorF("%s %s:            ",
		XCONFIG_PROBED, s3InfoRec.name);
	 for(i=k=0; i<8; i++)
	    for(j=0; j<s3Bpp; j++,k++)
	       ErrorF(" %02x", (k ^ ((dash_test_pattern & (1<<(15-i))) ? 0xff : 0)));
	 ErrorF("\n\n");
#endif
	 if (pat[0] == 0xff && pat[3 * s3Bpp] == 0xff)
	    break;		/* BITBLT worked */
      }
   
      /* clear source and destination patterns on the screen */
      for(i = 0; i < s3Bpp * 2 * 2; i++)
	 pat[i] = blackPixel;
      (*s3ImageWriteFunc)(0, 0, 2, 2, (char*)pat, 2 * s3Bpp, 
			  0, 0, (short) s3alu[GXcopy], ~0);
      (*s3ImageWriteFunc)(0, 2, 2, 2, (char*)pat, 2 * s3Bpp, 
			  0, 0, (short) s3alu[GXcopy], ~0);

      if (s3Trio32FCBug > 1) {
	 ErrorF("%s %s: WARNING: BITBLT malfunction --\n"
		"\tplease report to XFree86@XFree86.Org\n",
		XCONFIG_PROBED, s3InfoRec.name);
      }
      else if (s3Trio32FCBug) {
	 ErrorF("%s %s: BITBLT malfunction, using workaround code\n",
		XCONFIG_PROBED, s3InfoRec.name);
      }
      xfree(pat);

      /* restore read mask */
      WaitQueue16_32(1, 2);
      SET_RD_MASK(0);



      /* now testing S3 968 dashed line bug :-(
       * using an 8 pixel line (0,0) to (7,0) */

      pat = (unsigned char*) xcalloc(s3Bpp, 8);

      /* first test original dashed line code (MIX_DST); 
       * then workaround ( MIX_OR zero) */
   
      for (s3_968_DashBug = 0; s3_968_DashBug < 2; s3_968_DashBug++) {
      
	 /* init region with backgound pattern */
	 for(i=0; i<s3Bpp*8; i++) 
	    pat[i] = i;
	 (*s3ImageWriteFunc)(0, 0, 8, 1, (char*)pat, 8 * s3Bpp,
			     0, 0, (short) s3alu[GXcopy], ~0);
      
	 /* read back the destination */
	 WaitIdle();
	 (*s3ImageReadFunc)(0, 0, 8, 1, (char*)pat, 8 * s3Bpp, 0, 0, ~0);

	 BLOCK_CURSOR;
	 WaitQueue16_32(3,4);
	 SET_FRGD_MIX(FSS_FRGDCOL | MIX_XOR);
	 if (s3_968_DashBug) {
	    SET_BKGD_COLOR(0);
	    SET_BKGD_MIX(BSS_BKGDCOL | MIX_OR);
	 }
	 else {
	    SET_BKGD_MIX(BSS_BKGDCOL | MIX_DST);
	 }      

	 WaitQueue16_32(4,6);
	 SET_WRT_MASK(~0);
	 SET_FRGD_COLOR(~0);
	 SET_PIX_CNTL(MIXSEL_EXPPC | COLCMPOP_F);
      
	 WaitQueue(7);
	 SET_CURPT(0,0);
	 SET_MAJ_AXIS_PCNT(8-1);
	 SET_DESTSTP(2*(0-8), 2*0);
	 SET_ERR_TERM(2*0 - 8 -1);
	 SET_CMD(CMD_LINE | DRAW | LASTPIX | PLANAR | INC_X | INC_Y  |
		 PCDATA | _16BIT | WRTDATA);
	 SET_PIX_TRANS_W(dash_test_pattern);
      
	 /* read back the destination */
	 WaitIdle();
	 (*s3ImageReadFunc)(0, 0, 8, 1, (char*)pat, 8 * s3Bpp, 0, 0, ~0);

#ifdef DEBUG_968_DASH_TEST
	 ErrorF("%s %s: DASH968 %d  ",
		XCONFIG_PROBED, s3InfoRec.name,s3_968_DashBug);
	 for(i = 0; i < s3Bpp * 8; i++)
	    ErrorF(" %02x", pat[i]&0xff);
	 ErrorF("\n");

	 ErrorF("%s %s:            ",
		XCONFIG_PROBED, s3InfoRec.name);
	 for(i=k=0; i<8; i++)
	    for(j=0; j<s3Bpp; j++,k++)
	       ErrorF(" %02x", (k ^ ((dash_test_pattern & (1<<(15-i))) ? 0xff : 0)));
	 ErrorF("\n\n");
#endif

	 ok = 1;
	 for(i=k=0; i<8; i++)
	    for(j=0; j<s3Bpp; j++,k++)
	       if (pat[k] != (k ^ ((dash_test_pattern & (1<<(15-i))) ? 0xff : 0)))
		  ok = 0;
	 if (ok)
	    break;		/* dashed line worked */
      }

      /* clear the test region */
      for(i=0; i<s3Bpp*8; i++) 
	 pat[i] = blackPixel;
      (*s3ImageWriteFunc)(0, 0, 8, 1, (char*)pat, 8 * s3Bpp,
			  0, 0, (short) s3alu[GXcopy], ~0);

      if (s3_968_DashBug > 1) {
	 ErrorF("%s %s: WARNING: S3 968 dashed line malfunction --\n"
		"\tplease report to XFree86@XFree86.Org\n",
		XCONFIG_PROBED, s3InfoRec.name);
      }
      else if (s3_968_DashBug) {
	 ErrorF("%s %s: S3 968 dashed line malfunction, using workaround code\n",
		XCONFIG_PROBED, s3InfoRec.name);
      }
      xfree(pat);
   }

   WaitQueue16_32(3,4);
   SET_FRGD_COLOR(1);
   SET_PIX_CNTL(MIXSEL_EXPBLT | COLCMPOP_F);

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
   PixmapPtr pspix = 0;
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
	s3EnableLinear();
	s3Unlock();
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

#ifdef DPMSExtension
void
s3DPMSSet(PowerManagementMode)
    int PowerManagementMode;
{
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
}
#endif

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

   if (OFLG_ISSET(OPTION_SHOWCACHE, &s3InfoRec.options)) {
      if ( debugcache & 1)
         y += 512;
   }
      
   if (x > s3DisplayWidth - s3HDisplay)
      x  = s3DisplayWidth - s3HDisplay;

   /* so may S3 cards have problems with some odd base addresses, 
    * to catch them all only even base values will be used.
    */

   origBase = (y * s3DisplayWidth + x) * s3Bpp;
   Base = (origBase >> 2) & ~1;

   if (S3_964_SERIES(s3ChipId) && DAC_IS_BT485_SERIES) {
      if ((Base & 0x3f) >= 0x3c) 
	 Base = (Base & ~0x3f) | 0x3b;
      else if (s3Bpp>1 && (Base & 0x3f) == 0x3a) 
	 Base = (Base & ~0x3f) | 0x39;
      else if (s3Bpp>2 && (Base & 0x1f) == 0x1a) 
	 Base = (Base & ~0x1f) | 0x19;
   }
   if (S3_964_SERIES(s3ChipId)
       && (DAC_IS_TI3025 || DAC_IS_TI3026 || DAC_IS_TI3030 || DAC_IS_IBMRGB)) {
      int px, py, a;
      miPointerPosition(&px, &py);
      if (s3Bpp == 3) {
	 if (DAC_IS_TI3030 || DAC_IS_IBMRGB528)
	    a = 12;
	 else 
	    a = 6;
	 if (px-x > s3HDisplay/2)
	    Base = ((origBase + (a-1)*4) >> 2) & ~1;
	 Base -= Base % a;
      }
      else {
	 if (s3Bpp==1 && !DAC_IS_TI3030 && !DAC_IS_IBMRGB528)
	    a = 4-1;
	 else 
	    a = 8-1;
	 if (px-x > s3HDisplay/2)
	    Base = ((origBase + a*4) >> 2) & ~1;
	 Base &= ~a;
      }
   }

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

#ifdef XFreeXDGA
   if (!(s3InfoRec.directMode & XF86DGADirectMouse)) {
#endif
      s3AdjustCursorXPos = (origBase - (Base << 2)) / s3Bpp;

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
      s3WaitRetrace();
   }
#endif
}


/*
 * Force optimising compilers to read *addr
 */
static Bool
s3TryAddress(addr, value, physaddr, stage)
     long *addr;
     long value;
     long physaddr;
     int stage;
{
   long tmp;

   tmp = *addr;

   if (OFLG_ISSET(OPTION_FB_DEBUG, &s3InfoRec.options)) {
      if (stage == 0 && tmp != 0xFFFFFFFF) {
	 ErrorF("Memory found at 0x%08X (read 0x%08X)\n", physaddr, tmp);
      } else {
	 ErrorF("Stage %d: address = 0x%08X, read 0x%08X\n", stage, physaddr,
		tmp);
      }
   }
   if (tmp == value)
      return TRUE;
   else
      return FALSE;
}


void s3SetVidPage(vPage)
   int vPage;
{
  s3BankSelect(vPage);
}
