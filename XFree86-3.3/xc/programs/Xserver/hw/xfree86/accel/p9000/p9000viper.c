/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000viper.c,v 3.11.2.2 1997/05/11 01:54:43 dawes Exp $ */
/*
 * Copyright 1994, Erik Nygren (nygren@mit.edu)
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Harry Langenbacher (harry@brain.jpl.nasa.gov)
 * Additions for Viper PCI by Matt Thomas <thomas@lkg.dec.com>
 *
 * ERIK NYGREN, MATT THOMAS, AND HARRY LANGENBACHER DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK
 * NYGREN, MATT THOMAS, OR HARRY LANGENBACHER BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * 
 */
/* $XConsortium: p9000viper.c /main/6 1996/03/11 10:44:06 kaleb $ */

#include "X.h"
#include "input.h"

#include "xf86.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"
#include "xf86_PCI.h"
#include "ICD2061A.h"
#include "p9000viper.h"

#include <string.h>

static unsigned p9000ViperSaveMisc;       /* Stored value of misc register  */
static unsigned p9000ViperNewClockReg;    /* misc reg setting used by server */
static unsigned p9000ViperInited = FALSE; /* Has the Viper been initialized  */
static unsigned MemBaseFlags = VPR_VLB_OCR_BASE_80000000;

extern int xf86Verbose;

/* Prototypes */
Bool p9000ViperVlbProbe(
#if NeedFunctionPrototypes
   void
#endif
);

void p9000ViperVlbEnable(
#if NeedFunctionPrototypes
   p9000CRTCRegPtr
#endif
);

void p9000ViperVlbDisable(
#if NeedFunctionPrototypes
   void
#endif
);

Bool p9000ViperVlbValidate(
#if NeedFunctionPrototypes
   void
#endif
);

void p9000ViperVlbInitialize(
#if NeedFunctionPrototypes
   int,         /* The index of pScreen in the ScreenInfo */
   ScreenPtr,   /* The Screen to initialize */
   int,         /* The number of the Server's arguments. */
   char **      /* The arguments themselves. Don't change! */
#endif
);

Bool p9000ViperPciProbe(
#if NeedFunctionPrototypes
   void
#endif
);

void p9000ViperPciEnable(
#if NeedFunctionPrototypes
   p9000CRTCRegPtr
#endif
);

void p9000ViperPciDisable(
#if NeedFunctionPrototypes
   void
#endif
);

Bool p9000ViperPciValidate(
#if NeedFunctionPrototypes
   void
#endif
);

void p9000ViperPciInitialize(
#if NeedFunctionPrototypes
   int,         /* The index of pScreen in the ScreenInfo */
   ScreenPtr,   /* The Screen to initialize */
   int,         /* The number of the Server's arguments. */
   char **      /* The arguments themselves. Don't change! */
#endif
);

void p9000ViperSetClock(
#if NeedFunctionPrototypes
    long, long
#endif
);

p9000VendorRec p9000ViperVlbVendor = {
  "Diamond Viper VLB",    /* char *Desc */
  "vipervlb",             /* char *Vendor */
  p9000ViperVlbProbe,     /* Bool (* Probe)() */
  p9000ViperSetClock,     /* void (* SetClock)() */
  p9000ViperVlbEnable,    /* void (* Enable)() */
  p9000ViperVlbDisable,   /* void (* Disable)() */
  p9000ViperVlbValidate,  /* Bool (* Validate)() */
  p9000ViperVlbInitialize,/* void (* Initialize)() */
  P9000_VENDOR_VIPERVLB,  /* int Label */
};


p9000VendorRec p9000ViperPciVendor = {
  "Diamond Viper PCI",    /* char *Desc */
  "viperpci",             /* char *Vendor */
  p9000ViperPciProbe,     /* Bool (* Probe)() */
  p9000ViperSetClock,     /* void (* SetClock)() */
  p9000ViperPciEnable,    /* void (* Enable)() */
  p9000ViperPciDisable,   /* void (* Disable)() */
  p9000ViperPciValidate,  /* Bool (* Validate)() */
  p9000ViperPciInitialize,/* void (* Initialize)() */
  P9000_VENDOR_VIPERPCI,  /* int Label */
};

/*************************************************/
/***************** VIPER VLB *********************/
/*************************************************/

/*
 * p9000ViperVlbProbe --
 *    Determines whether a Dimaond Viper is available.
 */
Bool
p9000ViperVlbProbe()
{
  char bios_sig[100];
  
  if (-1 == xf86ReadBIOS(BIOS_BASE, VPR_VLB_BIOS_OFFSET,
			 (unsigned char *)bios_sig, VPR_VLB_BIOS_LENGTH))
    return FALSE; /* This OS can't probe the BIOS */
  bios_sig[VPR_VLB_BIOS_LENGTH] = '\0';
  if (0 == strncmp(bios_sig, VPR_VLB_BIOS_SIGNATURE,
		   strlen(VPR_VLB_BIOS_SIGNATURE)))
    {
      if (xf86Verbose)
	ErrorF("%s BIOS signature for Diamond Viper VLB found:\n\t<%s>\n",
	       XCONFIG_PROBED, bios_sig);
      return TRUE;
    }
  else
    return FALSE;
}

/*
 * p9000ViperVlbValidate --
 *    Determines whether parameters are valid for the Viper VLB.
 */
Bool
p9000ViperVlbValidate()
{
  if ((p9000InfoRec.MemBase != 0xA0000000) &&
      (p9000InfoRec.MemBase != 0x20000000) &&
      (p9000InfoRec.MemBase != 0x80000000))
    {
      p9000InfoRec.MemBase = 0x80000000;
      ErrorF("%s: MemBase not specified.  Using 0x%lx as a default.\n",
	     p9000InfoRec.name, p9000InfoRec.MemBase);
    }
  return(TRUE);
}

/*
 * p9000ViperVlbInitialize --
 *    Does one-time initialization for the Viper VLB.
 */
void
p9000ViperVlbInitialize(scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
}

/*
 * p9000ViperVlbEnable --
 *    Initializes a Diamond Viper for use. 
 */
void
p9000ViperVlbEnable(crtcRegs)
     p9000CRTCRegPtr crtcRegs;
{
  unsigned char OutputCtlBits;  /* Bits to output to output control register */
  extern unsigned MemBaseFlags;

  if (!p9000ViperInited)       /* If not saved yet */
   {
     p9000ViperSaveMisc = inb((short)MISC_IN_REG);       /* Save VGA Clocks */
#ifdef DEBUG
     ErrorF ("p9000ViperVlbEnable:debug info:p9000ViperSaveMisc=inb(MISC_IN_REG)=0x%02X\n",
	     (int)p9000ViperSaveMisc);
#endif
     /* p9000ViperNewClockReg: 0 becomes 1, 1 becomes 2, 2 becomes 0,
      * 3 (means 2) becomes 1 */
     p9000ViperNewClockReg = 
       (((p9000ViperSaveMisc>>2)&3) + 1) 
	 % (NUM_PROGRAMMABLE_CLOCKS);
   }

  if (p9000InfoRec.MemBase == 0xA0000000)
    MemBaseFlags = VPR_VLB_OCR_BASE_A0000000;
  else if (p9000InfoRec.MemBase == 0x20000000)
    MemBaseFlags = VPR_VLB_OCR_BASE_20000000;
  else if (p9000InfoRec.MemBase == 0x80000000)
    MemBaseFlags = VPR_VLB_OCR_BASE_80000000;

  p9000BtEnable(crtcRegs);

  /* The Diamond Viper uses the w5[12]86's Output Control Register to select
   * which device is enabled, etc.  It is located at 3c5 index 12.
   *
   * Note that when the W5186 is disabled, it is held in RESET.
   * As a result, its DRAM doesn't get refreshed and the fonts in the
   * VC's get corrupted.  Enabling both solves the problem but may be a
   * bad thing - it may cause too high of a clock rate to clock the w5186.
   */

  p9000UnlockVGAExtRegs();

  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);
  OutputCtlBits = (  MemBaseFlags      /* Select memory base */
		   | (crtcRegs->hp)<<5 /* Horiz Sync Polarity */
		   | (crtcRegs->vp)<<6 /* Vert Sync Polarity */
		   | VPR_VLB_OCR_ENABLE_P9000
		   /* These bits are reserved.  Lets not change them.
		    * Should be 0x08. */
		   | (inb(SEQ_PORT) & VPR_VLB_OCR_RESERVED_MASK)
		   );

  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);
  outb(SEQ_PORT, OutputCtlBits);

  p9000LockVGAExtRegs();

  outb(BT_PIXEL_MASK, 0xff);
  if (!p9000SWCursor)
    p9000BtCursorOn();
  p9000ViperInited = TRUE;
}


/*
 * p9000ViperVlbDisable --
 *    Turns off the P9000 and reenables VGA
 */
void p9000ViperVlbDisable()
{
  unsigned char OutputCtlBits;  /* Bits to output to output control register */
  unsigned char tmp;

  p9000BtCursorOff();

  /* Enable the Viper's video clock but disable the cursor */
  p9000OutBtReg(BT_COMMAND_REG_2, 0x0, BT_CR2_PORTSEL_NONMASK |
		BT_CR2_PCLK1 | BT_CR2_CURSOR_DISABLE);

  /* Turn video screen off so we don't see noise and trash */
  outb(SEQ_INDEX_REG, SEQ_CLKMODE_INDEX);
  tmp = inb(SEQ_PORT);
  outb(SEQ_PORT, tmp | 0x20);

  p9000UnlockVGAExtRegs();

  /* Disable the P9000 and enable VGA.  I assume that we're not running
     at 640x480 or 640x350 so the sync polarity doesn't matter here?
     this should be fixed for 640x480 or 640x350.
   */

  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);

  OutputCtlBits = ( /* Select memory base... */  
		   MemBaseFlags
		   | SP_POSITIVE<<5  /* Horiz Sync Polarity */
		   | SP_POSITIVE<<6  /* Vert Sync Polarity */
		   | VPR_VLB_OCR_ENABLE_W5186
		   /* These bits are reserved.  Lets not change them.  
		    * Should be 0x08. */
		   | (inb(SEQ_PORT) & VPR_VLB_OCR_RESERVED_MASK)
		   );
  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);
  outb(SEQ_PORT, OutputCtlBits);

  p9000LockVGAExtRegs();

  outb(MISC_OUT_REG, p9000ViperSaveMisc);  /* Restore VGA Clock select bits */
#ifdef DEBUG
  ErrorF ( "p9000ViperVlbDisable - debug info:MISC_OUT set to 0x%02X\n",
          (int) p9000ViperSaveMisc ) ;
#endif
  
  usleep(30000);   /* Wait at least 10 msecs (ICD2061 timeout) for the
		    * clock to change */

  p9000BtRestore();

  /* Turn video screen back on */
  outb(SEQ_INDEX_REG, SEQ_CLKMODE_INDEX);
  tmp = inb(SEQ_PORT);
  outb(SEQ_PORT, tmp & ~0x20);
}

/*************************************************/
/****************** VIPER PCI ********************/
/*************************************************/


/*
 * p9000ViperPciProbe --
 *    Determines whether a Diamond Viper is available.
 */
Bool
p9000ViperPciProbe()
{
  pciConfigPtr pcrp, *pcrpp;
  int i = 0;

  pcrpp = xf86scanpci(p9000InfoRec.scrnIndex);

  if (!pcrpp)
    return FALSE;

  while ((pcrp = pcrpp[i++]) != (pciConfigPtr)NULL)
    if ((pcrp->_vendor == PCI_WEITEK_VENDOR_ID) &&
        (pcrp->_device == PCI_P9000_DEVICE_ID))
      break;

  if (!pcrp)
    return FALSE;
  
  p9000InfoRec.MemBase = 0;
  p9000InfoRec.IObase = 0;

  if (pcrp->_base0) {
    if (pcrp->_base0 & 1)
      p9000InfoRec.IObase = pcrp->_base0 & 0xFFFFFFFC;
    else
      p9000InfoRec.MemBase = pcrp->_base0 & 0xFFFFFFF0;
  }
  if (pcrp->_base1) {
    if (pcrp->_base1 & 1) {
      if (!p9000InfoRec.IObase)
	p9000InfoRec.IObase = pcrp->_base1 & 0xFFFFFFFC;
    } else
      if (!p9000InfoRec.MemBase)
	p9000InfoRec.MemBase = pcrp->_base1 & 0xFFFFFFF0;
  }
  if (pcrp->_base2) {
    if (pcrp->_base2 & 1) {
      if (!p9000InfoRec.IObase)
	p9000InfoRec.IObase = pcrp->_base2 & 0xFFFFFFFC;
    } else
      if (!p9000InfoRec.MemBase)
	p9000InfoRec.MemBase = pcrp->_base2 & 0xFFFFFFF0;
  }
  if (pcrp->_base3) {
    if (pcrp->_base3 & 1) {
      if (!p9000InfoRec.IObase)
	p9000InfoRec.IObase = pcrp->_base3 & 0xFFFFFFFC;
    } else
      if (!p9000InfoRec.MemBase)
	p9000InfoRec.MemBase = pcrp->_base3 & 0xFFFFFFF0;
  }
  if (pcrp->_base4) {
    if (pcrp->_base4 & 1) {
      if (!p9000InfoRec.IObase)
	p9000InfoRec.IObase = pcrp->_base4 & 0xFFFFFFFC;
    } else
      if (!p9000InfoRec.MemBase)
	p9000InfoRec.MemBase = pcrp->_base4 & 0xFFFFFFF0;
  }
  if (pcrp->_base5) {
    if (pcrp->_base5 & 1) {
      if (!p9000InfoRec.IObase)
	p9000InfoRec.IObase = pcrp->_base5 & 0xFFFFFFFC;
    } else
      if (!p9000InfoRec.MemBase)
	p9000InfoRec.MemBase = pcrp->_base5 & 0xFFFFFFF0;
  }

  if (xf86Verbose) {
    ErrorF("%s %s: PCI: %s rev %x", XCONFIG_PROBED, p9000InfoRec.name, 
	   "Weitek P9000", pcrp->_rev_id);
    if (p9000InfoRec. MemBase)
      ErrorF(", Memory @ 0x%08x\n", p9000InfoRec.MemBase);
    if (p9000InfoRec.IObase)
      ErrorF(", I/O @ 0x%04x\n", p9000InfoRec.IObase);
    ErrorF("\n");
  }

  xf86cleanpci();

  return TRUE;
}

/*
 * p9000ViperPciValidate --
 *    Determines whether parameters are valid for the Viper PCI.
 */
Bool
p9000ViperPciValidate()
{
  if (!p9000InfoRec.MemBase)
    {
      ErrorF("%s: MemBase must be specified for Viper PCI\n");
      return(FALSE);
    }
  if (!p9000InfoRec.IObase)
    {
      ErrorF("%s: IObase must be specified for Viper PCI for extended IO\n");
      return(FALSE);
    }
  return(TRUE);
}

/*
 * p9000ViperPciInitialize --
 *    Does one-time initialization for the Viper PCI.
 */
void
p9000ViperPciInitialize(scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
}

/*
 * p9000ViperPciEnable --
 *    Initializes a Diamond Viper for use. 
 */
void
p9000ViperPciEnable(crtcRegs)
     p9000CRTCRegPtr crtcRegs;
{
  unsigned char OutputCtlBits;  /* Bits to output to output control register */
  extern unsigned MemBaseFlags;

  if (!p9000ViperInited)       /* If not saved yet */
   {
    p9000ViperSaveMisc = inb((short)MISC_IN_REG);  /* Save VGA Clock 
						    * select bits */
#ifdef DEBUG
    ErrorF ( "p9000ViperPciEnable:debug info:p9000ViperSaveMisc=inb(MISC_IN_REG)=0x%02X\n", (int)p9000ViperSaveMisc);
#endif
    p9000ViperNewClockReg = 
      (((p9000ViperSaveMisc>>2)&3) + 1) 
	% (NUM_PROGRAMMABLE_CLOCKS);
   }

  p9000BtEnable(crtcRegs);

  /* The Diamond Viper uses the w5[12]86's Output Control Register to select
   * which device is enabled, etc.  It is located at 3c5 index 12.
   */

  p9000UnlockVGAExtRegs();

  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);
  OutputCtlBits = ( VPR_PCI_OCR_ENABLE_P9000
		   /* These bits are reserved.  Lets not change them.
		    * Should be 0x?B. */
		   | (inb(SEQ_PORT) & VPR_PCI_OCR_RESERVED_MASK)
		   );

  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);
  outb(SEQ_PORT, OutputCtlBits);

  outb((short)MISC_OUT_REG,
       (0x3F & p9000ViperSaveMisc)
       | (crtcRegs->hp)<<6 /* Horiz Sync Polarity */
       | (crtcRegs->vp)<<7 /* Vert Sync Polarity */
       );
#ifdef DEBUG
  ErrorF ( "p9000ViperPciEnable - debug info:MISC_OUT set to 0x%02X\n",
          (int)( (0x3F & p9000ViperSaveMisc)
       | (crtcRegs->hp)<<6 /* Horiz Sync Polarity */
       | (crtcRegs->vp)<<7 )) ;
#endif

  p9000LockVGAExtRegs();

  outb(BT_PIXEL_MASK, 0xff);
  if (!p9000SWCursor)
    p9000BtCursorOn();
  p9000ViperInited = TRUE;
}


/*
 * p9000ViperPciDisable --
 *    Turns off the P9000 and reenables VGA
 */
void p9000ViperPciDisable()
{
  unsigned char OutputCtlBits;  /* Bits to output to output control register */
  unsigned char tmp;

  p9000BtCursorOff();

  /* Enable the Viper's video clock but disable the cursor */
  p9000OutBtReg(BT_COMMAND_REG_2, 0x0, BT_CR2_PORTSEL_NONMASK |
		BT_CR2_PCLK1 | BT_CR2_CURSOR_DISABLE);

  /* Turn video screen off so we don't see noise and trash */
  outb(SEQ_INDEX_REG, SEQ_CLKMODE_INDEX);
  tmp = inb(SEQ_PORT);
  outb(SEQ_PORT, tmp | 0x20);

  p9000UnlockVGAExtRegs();

  /* Disable the P9000 and enable VGA.  I assume that we're not running
     at 640x480 or 640x350 so the sync polarity doesn't matter here?
     this should be fixed for 640x480 or 640x350.
   */

  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);

  OutputCtlBits = (
		   VPR_PCI_OCR_ENABLE_W5186  /* Really 0x0 */
		   /* These bits are reserved.  Lets not change them. */
		   | (inb(SEQ_PORT) & VPR_PCI_OCR_RESERVED_MASK)
		   );
  outb(SEQ_INDEX_REG, SEQ_OUTPUT_CTL_INDEX);
  outb(SEQ_PORT, OutputCtlBits);

  p9000LockVGAExtRegs();

  outb(MISC_OUT_REG, p9000ViperSaveMisc);  /* Restore VGA Clock select bits */
#ifdef DEBUG
  ErrorF ( "p9000ViperPciDisable - debug info:MISC_OUT set to 0x%02X\n",
          (int) p9000ViperSaveMisc ) ;
#endif
  
  usleep(30000);  /* Wait at least 10 msecs (ICD2061 timeout)
		   * for the clock freq to change */
  
  p9000BtRestore();

  /* Turn video screen back on */
  outb(SEQ_INDEX_REG, SEQ_CLKMODE_INDEX);
  tmp = inb(SEQ_PORT);
  outb(SEQ_PORT, tmp & ~0x20);
}

/*************************************************/
/****************** COMMON ***********************/
/*************************************************/

/* 
 * p9000UnlockVGAExtRegs --
 *    Unlock VGA extended registers
 */
void p9000UnlockVGAExtRegs()
{
  unsigned char temp;

  outb(SEQ_INDEX_REG, SEQ_MISC_INDEX);
  temp = inb(SEQ_PORT);                     /* Read misc register */
  outb(SEQ_PORT,temp); outb(SEQ_PORT,temp); /* Write back twice */
  temp = inb(SEQ_PORT);                     /* Read misc register again */
  outb(SEQ_PORT, temp & ~0x20);             /* Clear bit 5 */
}

/* 
 * p9000LockVGAExtRegs --
 *    Relock VGA extended registers
 */
void p9000LockVGAExtRegs(void)
{
  unsigned char temp;

  /* Relock extended registers */
  outb(SEQ_INDEX_REG, SEQ_MISC_INDEX);
  temp = inb(SEQ_PORT);                /* Read misc register */
  outb(SEQ_PORT, temp | 0x20);         /* Set bit 5 */
}


/* 
 * p9000ViperSetClock --
 *    Sets the clock to the specified dot
 *    and memory clocks
 */
void p9000ViperSetClock(dotclock, memclock)
     long dotclock, memclock;  /* In HZ */
{
  unsigned int clock_ctrl_word , ActualHertz;

  if ((labs(dotclock - memclock) <= CLK_JITTER_TOL)
      || (labs(dotclock - 2*memclock) <= CLK_JITTER_TOL)
      || (labs(2*dotclock - memclock) <= CLK_JITTER_TOL))
    {
      /* If dotclock and memclock are integer multiples, we'll have jitter
       * This method of fixing the problem is just temporary */
      if (memclock == MEMSPEED)	memclock = MEMSPEED_ALT;
      else                      memclock = MEMSPEED;
    }

  /* The register used (0) should be different than the register
   * used for (vga) text mode */
  clock_ctrl_word =  ICD2061ACalcClock (dotclock, p9000ViperNewClockReg );  
  ICD2061ASetClock (clock_ctrl_word);
  ActualHertz = ICD2061AGetClock (clock_ctrl_word);
  
#ifdef DEBUG
  ErrorF("Dot-clock actually set to %d Hz.  Wanted %ld\n",ActualHertz,
	 dotclock);
#endif
  
  clock_ctrl_word = ICD2061ACalcClock (memclock, 3); /* memclock uses reg 3 */
  ICD2061ASetClock (clock_ctrl_word);
  ActualHertz = ICD2061AGetClock (clock_ctrl_word);
  /* Wait at least 10 msecs (ICD2061 timeout) for the clock to change */
  usleep(30000); 
  
#ifdef DEBUG
  ErrorF("Mem clock actually set to %d Hz.  Wanted %ld\n",ActualHertz,
	   memclock);
#endif
}

