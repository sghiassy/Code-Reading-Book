/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/pvga1/pvg_driver.c,v 3.31.2.4 1997/05/27 06:22:26 dawes Exp $
 *
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
 */
/* $XConsortium: pvg_driver.c /main/14 1996/10/27 11:08:28 kaleb $ */

/*
 * Accelerated support for 90C31 added by Mike Tierney <floyd@eng.umd.edu>
 * Accelerated support for 90C33 added by Bill Conn <conn@bnr.ca>
 */

/*
 * 16 (17) clock support added by Anders Bostrom <erabosa@lmera.ericsson.se>
 */

/*
 * Support for 90C33 added by Bill Morgart <wsm@morticia.ssw.com>
 */

/*
 * Support for 90C24a added by Brad Bosch <brad@lachman.com> 6/18/95
 * Primary testing and many ideas provided by Darin Ernst <dernst@pppl.gov>
 */

#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"

#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#ifdef XF86VGA16
#define MONOVGA
#endif

#ifndef MONOVGA
#include "vga256.h"

extern void pvgacfbFillRectSolidCopy();
extern void pvgacfbDoBitbltCopy();
extern void pvgacfbFillBoxSolid();
extern void pvgaBitBlt();

extern void C33cfbFillRectSolidCopy();
extern void C33cfbDoBitbltCopy();
extern void C33cfbFillBoxSolid();
extern void C33BitBlt();
#endif
#include "paradise.h"

typedef struct {
  vgaHWRec      std;          /* std IBM VGA register */
  unsigned char PR0A;           /* PVGA1A, WD90Cxx */
  unsigned char PR0B;
  unsigned char MemorySize;
  unsigned char VideoSelect;
  unsigned char CRTCCtrl;
  unsigned char VideoCtrl;
  unsigned char InterlaceStart; /* WD90Cxx */
  unsigned char InterlaceEnd;
  unsigned char MiscCtrl1;
  unsigned char MiscCtrl2;
  unsigned char MiscCtrl4;
  unsigned char InterfaceCtrl;  /* WD90C1x */
  unsigned char MemoryInterface; /* WD90C1x, WD90C3x */
  unsigned char FlatPanelCtrl; /* WD90C2x */
  unsigned short CursorCntl;   /* WD90C3x, 23C0<-2, 23C2, 0 */
} vgaPVGA1Rec, *vgaPVGA1Ptr;

static Bool  PVGA1Probe();
static char *PVGA1Ident();
static Bool  PVGA1ClockSelect();
static void  PVGA1EnterLeave();
static Bool  PVGA1Init();
static int   PVGA1ValidMode();
static void *PVGA1Save();
static void  PVGA1Restore();
static void  PVGA1Adjust();
static void  PVGA1FbInit();
extern void  PVGA1SetRead();
extern void  PVGA1SetWrite();
extern void  PVGA1SetReadWrite();
extern void  WD90C33SetRead();
extern void  WD90C33SetWrite();
extern void  WD90C33SetReadWrite();

vgaVideoChipRec PVGA1 = {
  PVGA1Probe,
  PVGA1Ident,
  PVGA1EnterLeave,
  PVGA1Init,
  PVGA1ValidMode,
  PVGA1Save,
  PVGA1Restore,
  PVGA1Adjust,
  vgaHWSaveScreen,
  (void (*)())NoopDDA,
  PVGA1FbInit,
  PVGA1SetRead,
  PVGA1SetWrite,
  PVGA1SetReadWrite,
  0x10000,
  0x08000,
  15,
  0x7FFF,
  0x08000, 0x10000,
  0x00000, 0x08000,
  TRUE,                                    /* Uses 2 banks */
  VGA_DIVIDE_VERT,
  {0,},
  8,
  FALSE,
  0,
  0,
  FALSE,
  FALSE,
  FALSE,
  NULL,
  1,                         /* ChipClockMulFactor */
  1                          /* ChipClockDivFactor */
};

#define new ((vgaPVGA1Ptr)vgaNewVideoState)

int WDchipset;
static int MClk = 45000;
static int MClkIndex = 8;
static unsigned char save_cs2 = 0;
static Bool use_cs2 = TRUE;
static Bool wd90c24_use_1meg = FALSE;

#undef DO_WD90C20

static SymTabRec chipsets[] = {
  { C_PVGA1,	"pvga1" },
  { WD90C00,	"wd90c00" },
  { WD90C10,	"wd90c10" },
  { WD90C30,	"wd90c30" },
  { WD90C24,	"wd90c24" },
  { WD90C31,	"wd90c31" },
  { WD90C33,	"wd90c33" },
#ifdef DO_WD90C20
  { WD90C20,	"wd90c20" },
#endif
  { -1,		"" },
};

static unsigned PVGA1_ExtPorts[] = {            /* extra ports for WD90C31 */
             0x23C0, 0x23C1, 0x23C2, 0x23C3, 0x23C4, 0x23C5 };

static int NumPVGA1_ExtPorts =
             ( sizeof(PVGA1_ExtPorts) / sizeof(PVGA1_ExtPorts[0]) );

static unsigned PVGA1_ExtPorts_WD90C33[] =    /* extra ports for WD90C33 */
{ 
  EXT_REG_ACCESS_PORT,
  EXT_REG_IO_PORT,  
  HBITBLT_PORT0,  
  HBITBLT_PORT1,  
  LINE_DRAW_K1,
  LINE_DRAW_K2,
  LINE_DRAW_ERROR_TERM,
  COMMAND_BUFFER
};

static int NumPVGA1_ExtPorts_WD90C33 =
             ( sizeof(PVGA1_ExtPorts_WD90C33)
            / sizeof(PVGA1_ExtPorts_WD90C33[0]) );

/*
 * PVGA1lcd24power(int)
 * 
 * Sets VESA Display Power Management Signaling (DPMS) Mode for monitor.
 * Also Enable or disable LCD power for WD90c24 controlled LCD.
 */
#ifdef DPMSExtension
static void
PVGA1lcd24power(int PowerManagementMode)
{
  static unsigned char lcd_off = 0;
  unsigned char lock_val, reg_val, seq1, sync2;
  int crt_on; /* allow sync to turn off? */

  if (!xf86VTSema) return;

  /* Turn LCD on or off and blank/unblank screen */
  switch (PowerManagementMode)
    {
    case DPMSModeOn:
      /* Turn LCD on */
      if (lcd_off) {
	/* this is a wd90c24 LCD which we have turned off */
	lcd_off = 0; /* Don't allow nested calls */

	/* Unlock extended PR registers */
	wrinx(0x3C4, 0x06, 0x48);

	reg_val = rdinx(0x3C4, 0x19); /* PR57 */
	/* PNLOFF low */
	wrinx(0x3C4, 0x19, reg_val | 0x04); /* set bit 2 */

	usleep(100000);
	lock_val = rdinx(0x3CE, 0x0F);
	/* Unlock PR0-PR4 */
	wrinx(0x3CE, 0x0F, (0xF8 & lock_val) | 0x05);
	reg_val = rdinx(0x3CE, 0x0E); /* PR4 */
	/* Drive FP interface pins active */
	wrinx(0x3CE, 0x0E, 0xDF & reg_val); /* clear bit 5 */
	wrinx(0x3CE, 0x0F, lock_val); /* restore lock */
      
	usleep(100000);
	lock_val = rdinx(vgaIOBase+0x04, 0x34);
	/* Unlock FP Reg */
	wrinx(vgaIOBase+0x04, 0x34, (0x0F & lock_val) | 0xA0);
	reg_val = rdinx(vgaIOBase+0x04, 0x032);
	/* Turn on LCD */
	wrinx(vgaIOBase+0x04, 0x32, 0x10 | reg_val); /* set PR19, bit 4 */
	wrinx(vgaIOBase+0x04, 0x34, lock_val); /* restore locks*/
      }

      /* Unblank Screen */
      outw(0x3C4, 0x0100);	/* Synchronous Reset */
      outb(0x3C4, 0x01);	/* Select SEQ1 */
      seq1 = inb(0x3C5) & ~0x20;
      outb(0x3C5, seq1);
      outw(0x3C4, 0x0300);	/* End Reset */

      usleep(10000);
      break;

    case DPMSModeStandby:
    case DPMSModeSuspend:
    case DPMSModeOff:
      /* Turn LCD off */
      lock_val = rdinx(vgaIOBase+0x04, 0x34);
      /* Unlock FP Reg */
      wrinx(vgaIOBase+0x04, 0x34, (0x0F & lock_val) | 0xA0);
      reg_val = rdinx(vgaIOBase+0x04, 0x032); /* PR19 */
      crt_on = ((reg_val & 0x20) == 0x20); /* CRT on? */
      if (reg_val & 0x10) {

	/* this is a wd90c24 with LCD on */
	lcd_off ++;
	/* Turn off LCD */
	wrinx(vgaIOBase+0x04, 0x32, 0xEF & reg_val); /* clear PR19, bit 4 */
	wrinx(vgaIOBase+0x04, 0x34, lock_val); /* restore locks*/

	lock_val = rdinx(0x3CE, 0x0F);
	/* Unlock PR0-PR4 */
	wrinx(0x3CE, 0x0F, (0xF8 & lock_val) | 0x05);
	reg_val = rdinx(0x3CE, 0x0E); /* PR4 */
	/* Tri-state FP interfacee pins */
	wrinx(0x3CE, 0x0E, reg_val | 0x20); /* set bit 5 */
	wrinx(0x3CE, 0x0F, lock_val); /* restore lock */

	lock_val = rdinx(0x3C4, 0x06);
	/* Unlock extended PR registers */
	wrinx(0x3C4, 0x06, 0x48);
	reg_val = rdinx(0x3C4, 0x19); /* PR57 */
	/* PNLOFF high */
	wrinx(0x3C4, 0x19, 0xFB & reg_val); /* clear bit 2 */
      }
      else {
	wrinx(vgaIOBase+0x04, 0x34, lock_val); /* restore locks*/
      }

      /* Blank screen */
      if (crt_on) {	/* blank only if monitor in use */
	outw(0x3C4, 0x0100);	/* Synchronous Reset */
	outb(0x3C4, 0x01);	/* Select SEQ1 */
	seq1 = inb(0x3C5) | 0x20;
	outb(0x3C5, seq1);
	outw(0x3C4, 0x0300);	/* End Reset */

	usleep(10000);
      }

      break;
    }

  /* Manipulate HSync and VSync */
  switch (PowerManagementMode)
    {
    case DPMSModeOn:
      /* Screen: On; HSync: On, VSync: On */
      outb(vgaIOBase + 4, 0x17);
      sync2 = inb(vgaIOBase + 5);
      sync2 |= 0x80;			/* enable sync   */
      usleep(10000);
      outb(vgaIOBase + 5, sync2);
      break;

    case DPMSModeStandby:
      /* Screen: Off; HSync: Off, VSync: On */
      /* This may be supported later */
      break;
    case DPMSModeSuspend:
      /* Screen: Off; HSync: On, VSync: Off */
      /* This may be supported later */
      break;
    case DPMSModeOff:
      /* Screen: Off; HSync: Off, VSync: Off */
      if (crt_on) {	/* disable sync only if monitor in use */
	outb(vgaIOBase + 4, 0x17);
	sync2 = inb(vgaIOBase + 5);
	sync2 &= ~0x80;			/* disable sync */
	usleep(10000);
	outb(vgaIOBase + 5, sync2);
      }
      break;
    }
}
#endif

/*
 * PVGA1Ident
 */

static char *
PVGA1Ident(n)
     int n;
{
  if (chipsets[n].token < 0)
    return(NULL);
  else
    return(chipsets[n].name);
}


/*
 * PVGA1ClockSelect --
 *      select one of the possible clocks ...
 */

static Bool
PVGA1ClockSelect(no)
     int no;
{
  static unsigned char save1, save2, save3, save4, save5;
  unsigned char temp;

  switch(no)
  {
    case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      if (WDchipset == WD90C24) {
	  outb(0x3C4, 0x06); /* unlock paradise extended registers */
	  save4 = inb (0x3C5);
	  outb(0x3C5, (save4 & 0xEF) | 0x48);

	  /* save feed Mclk state (miscctrl1) */
	  outb(vgaIOBase + 4, 0x2E); save3 = inb(vgaIOBase + 5);

	  wrinx(0x3C4, 0x35, 0x50);	/* unlock pr68 */
	  save5 = rdinx(0x3C4, 0x32);
	  wrinx(0x3C4, 0x32, 139);	/* set programable clock to ~62.2MHz */
	  save2 = rdinx(0x3C4, 0x31);
	  ErrorF("%s %s: WD: Current clock is 0x%x\n",
		 XCONFIG_PROBED, vga256InfoRec.name,
		 (save2>>1 & 0xc0) ^ 0x4 | (save1>>2 & 0x3));
	  break;
      }
      if (use_cs2)
      {
        outb(0x3CE, 0x0C); save2 = inb(0x3CF);
      }
      if (WDchipset != C_PVGA1)
      {
        outb(vgaIOBase + 4, 0x2E); save3 = inb(vgaIOBase + 5);
      }
      if (IS_WD90C3X(WDchipset) &&
	  !OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
      {
        outb(0x3C4, 0x12); save4 = inb(0x3C5);
      }
      break;
    case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      if (WDchipset == WD90C24) {
	  outb(0x3C4, 0x06); /* unlock paradise extended registers */
	  temp = inb (0x3C5);
	  outb(0x3C5, (temp & 0xEF) | 0x48);

 	  outw(vgaIOBase + 4, 0x2E | (save3 << 8)); /* disable feed Mclk */

	  wrinx(0x3C4, 0x35, 0x50);	/* unlock pr68 */
	  wrinx(0x3C4, 0x31, save2);

	  outb(0x3C4, 0x06); /* restore paradise extended registers lock */
	  outb(0x3C5, save4);
	  break;
      }
      if (use_cs2)
      {
        outw(0x3CE, 0x0C | (save2 << 8));
      }
      if (WDchipset != C_PVGA1)
      {
        outw(vgaIOBase + 4, 0x2E | (save3 << 8));
      }
      if (IS_WD90C3X(WDchipset) &&
	  !OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
      {
        outw(0x3C4, 0x12 | (save4 << 8));
      }
      break;
    default:
      if (no == MClkIndex)
      {
        /*
         * On all of the chipsets after PVGA1, you can feed MCLK as VCLK.
         */
        outb(vgaIOBase+4, 0x2E);
        temp = inb(vgaIOBase+5);
        outb(vgaIOBase+5, temp | 0x10);
      }
      else
      {
        /*
         * Disable feeding MCLK to VCLK
         */
        if (WDchipset != C_PVGA1)
        {
            outb(vgaIOBase+4, 0x2E);
            temp = inb(vgaIOBase+5);
            outb(vgaIOBase+5, temp & 0xEF);
        }
        temp = inb(0x3CC);
        outb(0x3C2, ( temp & 0xf3) | ((no << 2) & 0x0C));
	if (WDchipset == WD90C24) {
	    outb(0x3C4, 0x06); /* unlock paradise extended registers */
	    temp = inb (0x3C5);
	    outb(0x3C5, (temp & 0xEF) | 0x48);

	    wrinx(0x3C4, 0x35, 0x50);	/* unlock pr68 */
	    wrinx(0x3C4, 0x31, (rdinx(0x3C4, 0x31) & 0xE7) | (((no ^ 0x4) << 1) & 0x18));
	    break;
	}
        if (use_cs2)
        {
          outw(0x3CE, 0x0C | ((((no & 0x04) >> 1) ^ save_cs2) << 8));
        }
        if (IS_WD90C3X(WDchipset) &&
	    !OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
        {
          outb(0x3C4, 0x12); temp = inb(0x3C5);
          outb(0x3C5, temp & 0xfb | ((no & 0x8) >> 1) ^ 0x4);
        }
      }
  }
  return(TRUE);
}



/*
 * PVGA1Probe --
 *      check up whether a PVGA1 based board is installed
 */

static Bool
PVGA1Probe()
{
    int numclocks = 17;

    /*
     * Set up I/O ports to be used by this card
     */
    xf86ClearIOPortList(vga256InfoRec.scrnIndex);
    xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);

    if (vga256InfoRec.chipset) {
	WDchipset = xf86StringToToken(chipsets, vga256InfoRec.chipset);
        if (WDchipset < 0)
	    return (FALSE);
        PVGA1EnterLeave(ENTER);
    }
    else {
	char ident[4];
	unsigned char tmp, tmp1;
	unsigned char sig[2];

#ifdef USE_BIOS_PROBE
	/* This test fails for some on-the-motherboard WD video */
	if (xf86ReadBIOS(vga256InfoRec.BIOSbase, 0x7D, (unsigned char *)ident,
			 4) != 4)
	    return(FALSE);
        if (strncmp(ident, "VGA=",4)) 
	    return(FALSE);
#else
	xf86EnableIOPorts(vga256InfoRec.scrnIndex);
	tmp = rdinx(0x3CE, 0x0F);
	wrinx(0x3CE, 0x0F, 0x17 | tmp);	/* Lock registers */
	if (testinx2(0x3CE, 0x09, 0x7F)) {
	    wrinx(0x3CE, 0x0F, tmp);
	    xf86DisableIOPorts(vga256InfoRec.scrnIndex);
	    return(FALSE);
	}
	wrinx(0x3CE, 0x0F, 0x05);	/* Unlock them again */
	if (!testinx2(0x3CE, 0x09, 0x7F)) {
	    wrinx(0x3CE, 0x0F, tmp);
	    xf86DisableIOPorts(vga256InfoRec.scrnIndex);
	    return(FALSE);
	}
#endif

        /*
         * OK.  We know it's a Paradise/WD chip.  Now to figure out which one.
         */
        PVGA1EnterLeave(ENTER);
        if (!testinx(vgaIOBase+0x04, 0x2B))
	    WDchipset = C_PVGA1;
        else {
	    tmp = rdinx(0x3C4, 0x06);
	    wrinx(0x3C4, 0x06, 0x48);
	    if (!testinx2(0x3C4, 0x07, 0xF0))
	        WDchipset = WD90C00;
	    else if (!testinx(0x3C4, 0x10)) {
		/* WD90C2x */
	        WDchipset = WD90C20;	
		wrinx(vgaIOBase+0x04, 0x34, 0xA6);
		if ((rdinx(vgaIOBase+0x04,0x32) & 0x20) != 0)
		    wrinx(vgaIOBase+0x04, 0x34, 0x00);
#ifndef DO_WD90C20
        	PVGA1EnterLeave(LEAVE);
		return(FALSE);
#endif
	    }
	    else if (testinx2(0x3C4, 0x14, 0x0F)) {
		/* 
		 * WD90C24, 26, 30, 31, 33.
		 */
		tmp1 = rdinx(vgaIOBase+0x04, 0x34);
		wrinx(vgaIOBase+0x04, 0x34, 0x00);
		sig[0] = rdinx(vgaIOBase+0x04, 0x36);
		sig[1] = rdinx(vgaIOBase+0x04, 0x37);
    		ErrorF("%s %s: WD: Signature for WD90C[23]X=[0x%02x 0x%02x]\n", 
           	       XCONFIG_PROBED, vga256InfoRec.name, sig[0], sig[1]);
		if ((sig[0] == '3') && (sig[1] == '1'))
		    WDchipset = WD90C31;
		else if ((sig[0] == '3') && (sig[1] == '0'))
		    WDchipset = WD90C30;
		else if ((sig[0] == '3') && (sig[1] == '3'))
		    WDchipset = WD90C33;
		else if ((sig[0] == '2') && (sig[1] == '4')) {
		    WDchipset = WD90C24;
		    ErrorF("%s %s: WD: Detected 90C24.  Try 90C30 or 90C31 if this doesn't work.\n",
			   XCONFIG_PROBED, vga256InfoRec.name);
		}
		else if ((sig[0] == '2') && (sig[1] == '6')) {
		    ErrorF("%s %s: WD: Detected 90C26.  This MAY work as a\n"
			   XCONFIG_PROBED, vga256InfoRec.name);
		    ErrorF("%s %s:     90C30, but we don't know for sure.\n",
			   XCONFIG_PROBED, vga256InfoRec.name);
		    wrinx(vgaIOBase+0x04, 0x34, tmp1);
        	    PVGA1EnterLeave(LEAVE);
		    return(FALSE);
		}
		else {
		    ErrorF("%s %s: WD: Unsupported chipset detected.\n",
			   XCONFIG_PROBED, vga256InfoRec.name);
		    wrinx(vgaIOBase+0x04, 0x34, tmp1);
        	    PVGA1EnterLeave(LEAVE);
		    return(FALSE);
		}		    
		wrinx(vgaIOBase+0x04, 0x34, tmp1);
	    }
	    else {
		/* WD90C1x */
		WDchipset = WD90C10;
	    }
	    wrinx(0x3C4, 0x06, tmp);
	}
    }
    vga256InfoRec.chipset = xf86TokenToString(chipsets, WDchipset);

    if (WDchipset == WD90C31 || WDchipset == WD90C24)
	/* enable extra hardware accel registers */
    {
        xf86AddIOPorts(vga256InfoRec.scrnIndex,
                       NumPVGA1_ExtPorts, PVGA1_ExtPorts);
      	PVGA1EnterLeave(LEAVE);   /* force update of IO ports enable */
      	PVGA1EnterLeave(ENTER);
    }

    if (WDchipset == WD90C33)  /* enable extra hardware accel registers */
    {
	/* Enable the WD90C33 features */
        xf86AddIOPorts(vga256InfoRec.scrnIndex,
                       NumPVGA1_ExtPorts_WD90C33, PVGA1_ExtPorts_WD90C33);
      	PVGA1EnterLeave(LEAVE);   /* force update of IO ports enable */
      	PVGA1EnterLeave(ENTER);
    }


    /*
     * Detect how much memory is installed
     */
    if (!vga256InfoRec.videoRam) {
      unsigned char config, temp;

      outb(0x3CE, 0x0B); config = inb(0x3CF);

      switch(config & 0xC0) {
      case 0x00:
      case 0x40:
	vga256InfoRec.videoRam = 256;
	break;
      case 0x80:
	vga256InfoRec.videoRam = 512;

	if (WDchipset == WD90C24) {

	    ErrorF("%s %s: WD: FP_Lock = 0x%0x:\n",
		   XCONFIG_PROBED, vga256InfoRec.name,
		   rdinx(vgaIOBase+0x04, 0x34));
	    wrinx(vgaIOBase+0x04, 0x34, 0xA0); /* Unlock FP Reg, lock shadows*/

	    /* dual scan ==  (PR18[bit:3,1,0] = 0). */
	    /* flat panel disable == (PR19[bit:4] = 0). */
	    wd90c24_use_1meg = !((rdinx(vgaIOBase+0x04, 0x031) & 0x0B) ||
				 (rdinx(vgaIOBase+0x04, 0x032) & 0x10));
	    if (wd90c24_use_1meg) {
		wrinx(vgaIOBase+0x04, 0x34, 0xA6); /* unlock shadows */
		ErrorF("%s %s: WD: dual scan 90C24, external monitor: assuming 1 Meg VRAM\n",
		       XCONFIG_PROBED, vga256InfoRec.name);
		vga256InfoRec.videoRam = 1024;
	    }
	    else {
		if (vga256InfoRec.virtualX*vga256InfoRec.virtualY > 512*1024) {
		    vga256InfoRec.virtualX = 640;
		    vga256InfoRec.virtualY = 480;
		}
#ifdef DPMSExtension
		vga256InfoRec.DPMSSet = PVGA1lcd24power;
#endif
	    }
	}
	break;
      case 0xC0:
	vga256InfoRec.videoRam = 1024;
	if (WDchipset == WD90C33){ 
	  outb(vgaIOBase+4,0x3e); 
	  if (inb(vgaIOBase + 5) & 0x80) {
	    vga256InfoRec.videoRam = 2048; 
	  }
	}
	break;
      }
    }
    if (WDchipset == WD90C33 && vga256InfoRec.videoRam > 1024) {
      PVGA1.ChipSetWrite = WD90C33SetWrite;
      PVGA1.ChipSetRead = WD90C33SetRead;
      PVGA1.ChipSetReadWrite = WD90C33SetReadWrite;
    }

    /*
     * The 90C1x, 90C2x, and 90C3x seem to have this bit set for the first 
     * 4 clocks (perhaps only with ICS90C6x clock chip?), so save the initial 
     * state to get the correct clock ordering.  If the user requested that 
     * the bit be inverted, do it.
     */
    if ((WDchipset == WD90C10) || (WDchipset == WD90C20) || 
	(IS_WD90C3X(WDchipset))) {
        if (OFLG_ISSET(OPTION_SWAP_HIBIT, &vga256InfoRec.options))
	    save_cs2 = 0;
	else
	    save_cs2 = 2;
    }
    else {
        if (OFLG_ISSET(OPTION_SWAP_HIBIT, &vga256InfoRec.options))
	    save_cs2 = 2;
	else
	    save_cs2 = 0;
    }

    /*
     * All of the chips later than the PVGA1 can feed MCLK as VCLK, yielding
     * an extra clock select.
     */
    if (WDchipset == C_PVGA1)
	if (OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
	    numclocks = 8;
	else
	    numclocks = 4;
    else if (IS_WD90C3X(WDchipset) &&
	     !OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
	numclocks = 17;
    else
	numclocks = 9;

    if (numclocks > 8)
        MClkIndex = numclocks - 1;

    if ((WDchipset == C_PVGA1 || WDchipset == WD90C00) &&
	!OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options)) {
        use_cs2 = FALSE;
    }
    if (!vga256InfoRec.clocks) {
	vgaGetClocks(numclocks, PVGA1ClockSelect);
    }
    if (WDchipset != C_PVGA1) {
	if (vga256InfoRec.clocks != MClkIndex + 1) {
	    ErrorF("%s %s: %s: No MCLK specified in 'Clocks' line.  %s\n",
		   XCONFIG_GIVEN, vga256InfoRec.name, 
		   vga256InfoRec.chipset, "Using default 45");
	    MClk = 45000;
	}
	else
	    MClk = vga256InfoRec.clock[MClkIndex];
    }

    vga256InfoRec.bankedMono = TRUE;

    /* Initialize allowed option flags based on chipset */
    OFLG_SET(OPTION_SWAP_HIBIT, &PVGA1.ChipOptionFlags);
    if (WDchipset == C_PVGA1)
	OFLG_SET(OPTION_8CLKS, &PVGA1.ChipOptionFlags);
    if (WDchipset == WD90C20) {
	OFLG_SET(OPTION_INTERN_DISP, &PVGA1.ChipOptionFlags);
	OFLG_SET(OPTION_EXTERN_DISP, &PVGA1.ChipOptionFlags);
    }
    if (WDchipset == WD90C31 || WDchipset == WD90C33 || WDchipset == WD90C24)
	OFLG_SET(OPTION_NOACCEL, &PVGA1.ChipOptionFlags);
    if (WDchipset == WD90C24) {
	OFLG_SET(OPTION_SLOW_DRAM, &PVGA1.ChipOptionFlags);
	OFLG_SET(OPTION_MED_DRAM, &PVGA1.ChipOptionFlags);
	OFLG_SET(OPTION_FAST_DRAM, &PVGA1.ChipOptionFlags);
    }
    
#ifndef MONOVGA
#ifdef XFreeXDGA
    vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

    return(TRUE);
}


/*
 * PVGA1FbInit --
 *      initialize the cfb SpeedUp functions
 */
/* block of current settings for speedup registers */
unsigned int pvga1_blt_cntrl2 = (unsigned int)-1;
unsigned int pvga1_blt_src_low = (unsigned int)-1;
unsigned int pvga1_blt_src_hgh = (unsigned int)-1;
unsigned int pvga1_blt_dim_x = (unsigned int)-1;
unsigned int pvga1_blt_dim_y = (unsigned int)-1;
unsigned int pvga1_blt_row_pitch = (unsigned int)-1;
unsigned int pvga1_blt_rop = (unsigned int)-1;
unsigned int pvga1_blt_for_color = (unsigned int)-1;
unsigned int pvga1_blt_bck_color = (unsigned int)-1;
unsigned int pvga1_blt_trns_color = (unsigned int)-1;
unsigned int pvga1_blt_trns_mask = (unsigned int)-1;
unsigned int pvga1_blt_planemask = (unsigned int)-1;

void (*pvga1_stdcfbFillRectSolidCopy)();
void (*pvga1_stdcfbDoBitbltCopy)();
void (*pvga1_stdcfbBitblt)();
void (*pvga1_stdcfbFillBoxSolid)();


static void
PVGA1FbInit()
{
#ifndef MONOVGA
  int useSpeedUp;

  if (((WDchipset != WD90C31) && (WDchipset != WD90C33)
       && (WDchipset != WD90C24)) ||
      OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
     return;

  useSpeedUp = vga256InfoRec.speedup & SPEEDUP_ANYWIDTH;
  if (useSpeedUp && xf86Verbose)
  {
    ErrorF("%s %s: PVGA1: SpeedUps selected (Flags=0x%x)\n", 
           OFLG_ISSET(XCONFIG_SPEEDUP,&vga256InfoRec.xconfigFlag) ?
           XCONFIG_GIVEN : XCONFIG_PROBED,
           vga256InfoRec.name, useSpeedUp);
  }
  if (useSpeedUp & SPEEDUP_FILLRECT)
  {
   /** must save default because if not screen to screen must use default **/
    pvga1_stdcfbFillRectSolidCopy = vga256LowlevFuncs.fillRectSolidCopy;
    vga256LowlevFuncs.fillRectSolidCopy = pvgacfbFillRectSolidCopy;
  }
  if (useSpeedUp & SPEEDUP_BITBLT)
  {
    pvga1_stdcfbDoBitbltCopy = vga256LowlevFuncs.doBitbltCopy;
    vga256LowlevFuncs.doBitbltCopy = pvgacfbDoBitbltCopy;
    pvga1_stdcfbBitblt = vga256LowlevFuncs.vgaBitblt;
    vga256LowlevFuncs.vgaBitblt = pvgaBitBlt;
  }
  if (useSpeedUp & SPEEDUP_LINE)
  {
    /* 90C33 has this stuff */
    ;
  }
  if (useSpeedUp & SPEEDUP_FILLBOX)
  {
    pvga1_stdcfbFillBoxSolid = vga256LowlevFuncs.fillBoxSolid;
    vga256LowlevFuncs.fillBoxSolid = pvgacfbFillBoxSolid;
  }
#endif /* MONOVGA */
}



/*
 * PVGA1EnterLeave --
 *      enable/disable io-mapping
 */

static void 
PVGA1EnterLeave(enter)
     Bool enter;
{
  unsigned char temp;

#ifndef MONOVGA
#ifdef XFreeXDGA 
  if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter)
    return;
#endif
#endif

  if (enter)
    {
      xf86EnableIOPorts(vga256InfoRec.scrnIndex);

      vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
      outw(0x3CE, 0x050F);           /* unlock PVGA1 Register Bank 1 */
      outw(vgaIOBase + 4, 0x8529);   /* unlock PVGA1 Register Bank 2 */
      outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, temp & 0x7F);
      /* Unlock sequencer extended registers */
      outb(0x3C4, 0x06);
      if (WDchipset == WD90C33) {
	outb(0x3C5, 0x48);
      } else {
	temp = inb (0x3C5);
	outb(0x3C5, (temp & 0xEF) | 0x48);
      }
    }
  else
    {
      /* the brain-dead Speedstar 24X BIOS brings up the board with a
	 programmable clock used, but to a default value of 28.3 MHz.
	 If now only the clock select lines were restored the timing has a 
	 good chance of breaking -- why can't they just use clock 1 ? */
      if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions) &&
	  vga256InfoRec.clock[2] > 0) {	/* restore only if clock2 probed */
	unsigned long freq = vga256InfoRec.clock[2];
#ifdef DEBUG
	ErrorF("Leave: Restoring F = %d kHz \n", freq);
#endif
	if (freq < 25000 || freq > 100000)
	  freq = 28300;
	AltICD2061SetClock(50500*1000, 3);	/* restore MCLK default */
	AltICD2061SetClock(freq*1000, 2);
	temp = inb(0x3cc);
	outb(0x3c2, temp | 0xC);
      }
      xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    }
}



/*
 * PVGA1Restore --
 *      restore a video mode
 */

static void
PVGA1Restore(restore)
     vgaPVGA1Ptr restore;
{
  unsigned char temp;

  vgaProtect(TRUE);

  /*
   * First unlock all these special registers ...
   * NOTE: Locking will not be fully renabled !!!
   */
  outb(0x3CE, 0x0D); temp = inb(0x3CF); outb(0x3CF, temp & 0x1C);
  outb(0x3CE, 0x0E); temp = inb(0x3CF); outb(0x3CF, temp & 0xFB);

  if (WDchipset == WD90C24) {
      temp = rdinx(vgaIOBase+0x04, 0x34);
      /* Unlock FP Reg */
      wrinx(vgaIOBase+0x04, 0x34, (0x0F & temp) | 0xA0);
      if (!(rdinx(vgaIOBase+0x04, 0x032) & 0x10)) {
	  /* this is a wd90c24 in CRT only mode */
	  /* unlock clock select, some graphics/sequencer bits, 8/9 char */
	  temp = 0xF8;	/* mask */
      } else {
	  /* this is a wd90c24 with LCD on */
	  wrinx(vgaIOBase+0x04, 0x34, 0xA0); /* Unlock FP Reg, lock shadows*/
	  /* unlock nothing*/
	  temp = 0xFF;	/* mask */
      }
  } else {
      /* unlock clock select, some graphics/sequencer bits, 8/9 char clock */
      temp = 0xF8;	/* mask */
  }
      
  outb(vgaIOBase + 4, 0x2A); temp &= inb(vgaIOBase + 5);
  outb(vgaIOBase + 5, temp);

  /* Unlock sequencer extended registers */
  outb(0x3C4, 0x06);
  if (WDchipset == WD90C33) {
    outb(0x3C5, 0x48);
  } else {
    temp = inb (0x3C5);
    outb(0x3C5, (temp & 0xEF) | 0x48);
  }

  outw(0x3CE, 0x0009);   /* segment select A */
  outw(0x3CE, 0x000A);   /* segment select B */

  outw(0x3CE, (restore->PR0A << 8) | 0x09);
  outw(0x3CE, (restore->PR0B << 8) | 0x0A);

  outb(0x3CE, 0x0B); temp = inb(0x3CF);          /* banking mode ... */
  outb(0x3CF, (temp & 0x37) | (restore->MemorySize & 0xC8));
       
  if (new->std.NoClock >= 0)
  {
      if (WDchipset == WD90C24) {
	  outb(0x3C4, 0x06); /* unlock paradise extended registers */
	  temp = inb (0x3C5);
	  outb(0x3C5, (temp & 0xEF) | 0x48);
	  
	  wrinx(0x3C4, 0x35, 0x50);	/* unlock pr68 */
#if 0
fprintf(stderr, "pr68 was = 0x%x\n", rdinx(0x3C4, 0x31));
fprintf(stderr, "pr69 was = 0x%x\n", rdinx(0x3C4, 0x32));
#endif
	  wrinx(0x3C4, 0x31, (rdinx(0x3C4, 0x31) & 0xE0) | ((restore->VideoSelect) & 0x1F));
	  /* set programable clock */
	  if ((restore->std.NoClock & 0x14) == 0x04)
	      wrinx(0x3C4, 0x32, restore->MiscCtrl4);
#if 0
fprintf(stderr, "restored pr68 = 0x%x\n", restore->VideoSelect);
fprintf(stderr, "pr68 now = 0x%x\n", rdinx(0x3C4, 0x31));
fprintf(stderr, "pr69 now = 0x%x\n", rdinx(0x3C4, 0x32));
#endif
      }
      else {
	  outw(0x3CE, (restore->VideoSelect << 8) | 0x0C);
	  if (IS_WD90C3X(WDchipset) &&
	      !OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
	      outw(0x3C4, (restore->MiscCtrl4 << 8) | 0x12);
      }
  }
#ifndef MONOVGA
  outw(0x3CE, (restore->CRTCCtrl << 8)    | 0x0D);
  outw(0x3CE, (restore->VideoCtrl << 8)   | 0x0E);
#endif
  
  /*
   * Now the WD90Cxx specials (Register Bank 2)
   */
  outw(vgaIOBase + 4, (restore->InterlaceStart << 8) | 0x2C);
  outw(vgaIOBase + 4, (restore->InterlaceEnd << 8)   | 0x2D);
  outw(vgaIOBase + 4, (restore->MiscCtrl2 << 8)      | 0x2F);

  /*
   * For the WD90C10 & WD90C11 we have to select segment mapping.
   */
  outb(0x3C4, 0x11); temp = inb(0x3C5);
  outb(0x3C5, (temp & 0x03) | (restore->InterfaceCtrl & 0xFC));

  outb(vgaIOBase + 4, 0x2E);

  /* Note, only selected bits of MiscCtrl1 are restored */
  temp = inb(vgaIOBase+5);
  if (WDchipset != C_PVGA1)
    {
      /*
       * Deal with 9th clock select if not PVGA1.
       */
      if (new->std.NoClock >= 0)
        temp = (temp & 0xEF) | (restore->MiscCtrl1 & 0x10);
      if (IS_WD90C3X(WDchipset))
        temp = (temp & 0xBF) | (restore->MiscCtrl1 & 0x40);
      outb(vgaIOBase + 5, temp);
    }
  if ((IS_WD90C3X(WDchipset)) || (WDchipset == WD90C10))
    {
      unsigned char mask = (IS_WD90C3X(WDchipset) ? 0xCF : 0x0F);

      if (wd90c24_use_1meg)
	  mask = 0xEF;	/* also restore VRAM data path width */

      outb(0x3C4, 0x10);
      temp = inb(0x3C5);
      outb(0x3C5, (restore->MemoryInterface & mask) | (temp & ~mask));
    }

  /* XXX ICD2061A should be re-programmed here, but to what freq ?
     this requires a major rewrite of the whole XFree86 code because
     that value may be part of a "standard" video state */

  /* This must be done AFTER the Memory width (MemoryInterface) is restored */
  vgaHWRestore((vgaHWPtr)restore);

  if (WDchipset == WD90C24) {
      if (restore->std.NoClock & 0x4) {
	  /* Use programable clock */
	  temp = inb(0x3CC);
	  outb(0x3C2, ( temp & 0xf3) | 0x8);
      }
  }
  if (WDchipset == WD90C20)
    {
      /* Note, only selected bits of FlatPanelCtrl are restored */
      outb(vgaIOBase + 4, 0x32);
      temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, (temp & 0xCF) | (restore->FlatPanelCtrl & 0x30));
    }

  if (WDchipset == WD90C31 || WDchipset == WD90C24 ||
      WDchipset == WD90C33)    /* set hardware cursor register */
  {
     outw (EXT_REG_ACCESS_PORT, CURSOR_BLOCK);
     outw (EXT_REG_IO_PORT, restore->CursorCntl);
  }

  vgaProtect(FALSE);
}



/*
 * PVGA1Save --
 *      save the current video mode
 */

static void *
PVGA1Save(save)
     vgaPVGA1Ptr save;
{
  unsigned char PR0A, PR0B, temp;

  vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
  outb(0x3CE, 0x0D); temp = inb(0x3CF); outb(0x3CF, temp & 0x1C);
  outb(0x3CE, 0x0E); temp = inb(0x3CF); outb(0x3CF, temp & 0xFD);

  if (WDchipset != WD90C24 || wd90c24_use_1meg)
      /* unlock clock select, some graphics/sequencer bits, 8/9 char clock */
      temp = 0xF8;
  else
      /* unlock nothing */
      temp = 0xFF;
  outb(vgaIOBase + 4, 0x2A); temp &= inb(vgaIOBase + 5);
  outb(vgaIOBase + 5, temp);

  /* Unlock sequencer extended registers */
  outb(0x3C4, 0x06);
  if (WDchipset == WD90C33) {
    outb(0x3C5, 0x48);
  } else {
    temp = inb (0x3C5);
    outb(0x3C5, (temp & 0xEF) | 0x48);
  }

  outb(0X3CE, 0x09); PR0A = inb(0x3CF); outb(0x3CF, 0x00);
  outb(0X3CE, 0x0A); PR0B = inb(0x3CF); outb(0x3CF, 0x00);

  save = (vgaPVGA1Ptr)vgaHWSave((vgaHWPtr)save, sizeof(vgaPVGA1Rec));

  save->PR0A = PR0A;
  save->PR0B = PR0B;
  outb(0x3CE, 0x0B); save->MemorySize  = inb(0x3CF);
  if (WDchipset == WD90C24) {
      wrinx(0x3C4, 0x35, 0x50);	/* unlock pr68 */
      outb(0x3C4, 0x31); save->VideoSelect = inb(0x3C5);
      outb(0x3C4, 0x32); save->MiscCtrl4 = inb(0x3C5);
  } else {
      outb(0x3CE, 0x0C); save->VideoSelect = inb(0x3CF);
      if (IS_WD90C3X(WDchipset) &&
	  !OFLG_ISSET(OPTION_8CLKS, &vga256InfoRec.options))
      {
	  outb(0x3C4, 0x12); save->MiscCtrl4 = inb(0x3C5);
      }
  }
#ifndef MONOVGA
  outb(0x3CE, 0x0D); save->CRTCCtrl    = inb(0x3CF);
  outb(0x3CE, 0x0E); save->VideoCtrl   = inb(0x3CF);
#endif

  /* WD90Cxx */
  outb(vgaIOBase + 4, 0x2C); save->InterlaceStart = inb(vgaIOBase+5);
  outb(vgaIOBase + 4, 0x2D); save->InterlaceEnd   = inb(vgaIOBase+5);
  outb(vgaIOBase + 4, 0x2E); save->MiscCtrl1      = inb(vgaIOBase+5);
  outb(vgaIOBase + 4, 0x2F); save->MiscCtrl2      = inb(vgaIOBase+5);

  /* WD90C1x */
  outb(0x3C4, 0x11); save->InterfaceCtrl = inb(0x3C5);

  /* WD90C3x */
  if (IS_WD90C3X(WDchipset) || WDchipset == WD90C10)
    {
      outb(0x3C4, 0x10); save->MemoryInterface = inb(0x3C5);
    }
  if (WDchipset == WD90C20)
    {
      outb(vgaIOBase + 4, 0x32); save->FlatPanelCtrl = inb(vgaIOBase + 5);
    }

  if (WDchipset == WD90C31 || WDchipset == WD90C24 ||
      WDchipset == WD90C33)   /* save hardware cursor register */
  {
     outw (EXT_REG_ACCESS_PORT, CURSOR_BLOCK);
     save->CursorCntl = inw(EXT_REG_IO_PORT);
  }

  return ((void *) save);
}



/*
 * PVGA1Init --
 *      Handle the initialization, etc. of a screen.
 */

static Bool
PVGA1Init(mode)
     DisplayModePtr mode;
{

/*  new->SeqClkMode = 0; */

  if (!vgaHWInit(mode,sizeof(vgaPVGA1Rec)))
    return(FALSE);

  /* disable c33 command buffer fifo ... */
  /* for now                             */
  if (WDchipset == WD90C33)
    outw(COMMAND_BUFFER, 0);

#ifndef MONOVGA
  new->std.CRTC[19] = vga256InfoRec.displayWidth >> 3; /* we are in byte-mode */
  if (WDchipset == WD90C33)
    new->std.CRTC[20] = 0x00;	/* this needs to be 00 for wd90C33 */
				/* should also be true for C31 too */
  else
    new->std.CRTC[20] = 0x40;
  new->std.CRTC[23] = 0xE3; /* thats what the man says */
#endif

  new->PR0A = 0x00;
  new->PR0B = 0x00;
  /*************************************
   * Try setting this to 0x0C to play with 8bit vs 16 bit stuff
   *************************************
   */
  outb(0x3CE, 0x0B); new->MemorySize  = inb(0x3CF) | 0x08;
  if (new->std.NoClock >= 0)
  {
      if (WDchipset == WD90C24) {
	  if (new->std.NoClock & 0x4) {
	      /* Use programable clock */
	      new->VideoSelect = 0;
	      new->MiscCtrl4=vga256InfoRec.clock[new->std.NoClock]/447.443+.5;
	      if (new->MiscCtrl4 < 0x38) new->MiscCtrl4 = 0x38;
	      if (new->MiscCtrl4 > 0xBE) new->MiscCtrl4 = 0xBE;
	      if (abs(new->MiscCtrl4 * 447.443 - vga256InfoRec.clock[new->std.NoClock]) > 20) {
		  ErrorF("%s %s: WD: Clock %6.3f adjusted to %6.3f by clock generator\n",
			 XCONFIG_PROBED, vga256InfoRec.name,
			 vga256InfoRec.clock[new->std.NoClock]/1000.,
			 new->MiscCtrl4 * .447443);
	      }
	  } else {
	      /* Use preprogrammed clocks */
	      new->VideoSelect = ((new->std.NoClock ^ 0x4) << 1) & 0x18;
	      new->MiscCtrl4 = 139;	/* Just a sane value */
	  }
	  if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options) ||
	      OFLG_ISSET(OPTION_MED_DRAM, &vga256InfoRec.options) ||
	      OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options)) {
	      ErrorF("%s %s: %s: Memory clock overridden by option\n",
		     XCONFIG_GIVEN, vga256InfoRec.name, vga256InfoRec.chipset);
	      /* Set Mclk */
	      /* I know this seems strange, but it allows what I wanted */
	      if (OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options)) {
		  new->VideoSelect ^= 0x6;
		  if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
		      new->VideoSelect ^= 0x2;
	      }
	      if (OFLG_ISSET(OPTION_MED_DRAM, &vga256InfoRec.options)) {
		  new->VideoSelect ^= 0x7;
		  if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options) &&
		      !OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options))
		      new->VideoSelect ^= 0x2;
	      }
	  } else {	/* Keep current Mclk */
	      unsigned char tmp;
	      outb(0x3C4, 0x06); /* unlock paradise extended registers */
	      tmp = inb (0x3C5);
	      outb(0x3C5, (tmp & 0xEF) | 0x48);
	      wrinx(0x3C4, 0x35, 0x50);	/* unlock pr68 */
	      
	      new->VideoSelect |= rdinx(0x3C4, 0x31) & 0x7; /* Fetch Mclk */
	  }
      } else {
	  if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions)) {
		unsigned long freq = vga256InfoRec.clock[mode->Clock];
		if (freq > 16000) 
		  {
		    AltICD2061SetClock(62000*1000, 3); /* increase MCLK */
		    AltICD2061SetClock(freq*1000, 2);
		    new->std.MiscOutReg = (new->std.MiscOutReg & 0xF3) | 8;
		    outb(0x3c2, new->std.MiscOutReg);
		    new->std.NoClock = 2;
#ifdef DEBUG
		    ErrorF("Freq = %d kHz selected\n", freq);
#endif
		  }
	  }
	  new->VideoSelect = ((new->std.NoClock & 0x4) >> 1) ^ save_cs2;
	  new->MiscCtrl4 = ((new->std.NoClock & 0x8) >> 1) ^ 0x4;
      }
  }
#ifndef MONOVGA
  new->CRTCCtrl = 0x00;
  new->VideoCtrl = 0x01;
#endif

  /* WD90Cxx */
  if (mode->Flags & V_INTERLACE) 
    {
      new->InterlaceStart = (mode->CrtcHSyncStart >> 3) -
			    (mode->CrtcHTotal >> 4);
      new->InterlaceEnd = 0x20 |
                          ((mode->CrtcHSyncEnd >> 3) -
			   (mode->CrtcHTotal >> 4)) & 0x1F;
    }
  else
    {
      new->InterlaceStart = 0x00;
      new->InterlaceEnd = 0x00;
    }
  /* MCLK clock select */
  new->MiscCtrl1 = 0x00;
  if ((new->std.NoClock == MClkIndex) && (WDchipset != C_PVGA1))
    new->MiscCtrl1 |= 0x10;
  new->MiscCtrl2 = 0x00;

  /* WD90C1x */
  if ((WDchipset != C_PVGA1)&&(WDchipset != WD90C00)&&(WDchipset != WD90C20))
  {
    if (WDchipset == WD90C33)
      new->InterfaceCtrl = 0x5F;
    else
      new->InterfaceCtrl = 0x5C;
  }
  else
    new->InterfaceCtrl = 0x00;

  if (WDchipset == WD90C10)
#ifdef MONOVGA
    new->MemoryInterface = 0x0B;
#else
    new->MemoryInterface = 0x09;
#endif

  if (IS_WD90C3X(WDchipset))
    {
      new->MemoryInterface = 0xC1;
      if (wd90c24_use_1meg)
        {
	  new->MemorySize = 0xc8;
	  if (vga256InfoRec.clock[new->std.NoClock] > MClk) {
	      new->MemoryInterface = 0x41;
	  }
        }
      else
        {
	  if (vga256InfoRec.clock[new->std.NoClock] > MClk) {
	      new->MemoryInterface = 0x41;
	      new->MiscCtrl1 |= 0x40;
	  }
        }
      if (WDchipset == WD90C31 || WDchipset == WD90C33 || WDchipset == WD90C24)
	{
/*** MJT ***/
/**** not yet
          new->CursorCntl = 
	    (0 << 12) | (0 << 11) | (1 << 9) | (0 << 8) | (1 << 5);
****/
        }
    }
  
  if (WDchipset == WD90C20)
    {
      /*
       * Select internal display, external display or both, depending
       * on user option-flag settings.  If neither option flag is set,
       * default to the internal display for monochrome, and external
       * display for color.
       */
      new->FlatPanelCtrl = 0x00;
      if (OFLG_ISSET(OPTION_INTERN_DISP, &vga256InfoRec.options))
	new->FlatPanelCtrl |= 0x10;
      if (OFLG_ISSET(OPTION_EXTERN_DISP, &vga256InfoRec.options))
	new->FlatPanelCtrl |= 0x20;
      if (new->FlatPanelCtrl == 0x00)
#ifdef MONOVGA
	new->FlatPanelCtrl = 0x10;
#else
	new->FlatPanelCtrl = 0x20;
#endif
    }

  return(TRUE);
}
	


/*
 * PVGA1Adjust --
 *      adjust the current video frame to display the mousecursor
 */

static void 
PVGA1Adjust(x, y)
     int x, y;
{
#ifdef MONOVGA
  int           Base = (y * vga256InfoRec.displayWidth + x + 3) >> 3;
#else
  int           Base = (y * vga256InfoRec.displayWidth + x + 1) >> 2;
#endif
  unsigned char temp;

  outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);
  outb(0x3CE, 0x0D); temp=inb(0x3CF); 
  outb(0x3CF, ((Base & 0x030000) >> 13) | (temp & 0xE7));

#ifdef XFreeXDGA
  if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
    /* Wait until vertical retrace is in progress. */
    while (inb(vgaIOBase + 0xA) & 0x08);
    while (!(inb(vgaIOBase + 0xA) & 0x08));
  }
#endif
}

/*
 * PVGA1ValidMode --
 *
 */
static int
PVGA1ValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}
