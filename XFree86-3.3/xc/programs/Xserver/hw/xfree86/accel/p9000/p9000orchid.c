/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000orchid.c,v 3.8 1996/12/23 06:40:48 dawes Exp $ */
/*
 * Copyright 1994, Erik Nygren (nygren@mit.edu)
 *
 * This code may be freely incorporated in any program without royalty, as
 * long as the copyright notice stays intact.
 *
 * Additions by Harry Langenbacher (harry@brain.jpl.nasa.gov)
 *
 * ERIK NYGREN AND HARRY LANGENBACHER DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ERIK NYGREN OR HARRY
 * LANGENBACHER BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 * */
/* $XConsortium: p9000orchid.c /main/7 1996/03/11 10:43:55 kaleb $ */

/* NOTE: This is actually based mostly on information contained in
 * Weitek's Power 9000 VL Bus Combination Board Application Note and
 * secondly on information from the Orchid P9000 owner's manual */

#include "X.h"
#include "input.h"

#include "xf86.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#include "p9000.h"
#include "p9000reg.h"
#include "p9000Bt485.h"
#include "ICD2061A.h"
#include "p9000orchid.h"
#include "p9000viper.h"

#include <string.h>

static unsigned int p9000OrchidSaveMisc;  /* Stored value of misc register  */
static unsigned int p9000OrchidSaveVidSelect; /* Stored value of Video Select
					 * register (Output Command Register)*/
static unsigned p9000OrchidInited = FALSE;/* Has the Orchid been initialized */

extern int xf86Verbose;
extern int p9000MaxClock;

/* Prototypes */
Bool p9000OrchidProbe(
#if NeedFunctionPrototypes
   void
#endif
);

void p9000OrchidEnable(
#if NeedFunctionPrototypes
   p9000CRTCRegPtr
#endif
);

void p9000OrchidDisable(
#if NeedFunctionPrototypes
   void
#endif
);

Bool p9000OrchidValidate(
#if NeedFunctionPrototypes
   void
#endif
);

void p9000OrchidInitialize(
#if NeedFunctionPrototypes
   int,         /* The index of pScreen in the ScreenInfo */
   ScreenPtr,   /* The Screen to initialize */
   int,         /* The number of the Server's arguments. */
   char **      /* The arguments themselves. Don't change! */
#endif
);

void p9000OrchidSetClock(
#if NeedFunctionPrototypes
    long, long
#endif
);

p9000VendorRec p9000OrchidVendor = {
  "Orchid P9000",       /* char *Desc */
  "orchid_p9000",       /* char *Vendor */
  p9000OrchidProbe,     /* Bool (* Probe)() */
  p9000OrchidSetClock,     /* void (* SetClock)() */
  p9000OrchidEnable,    /* void (* Enable)() */
  p9000OrchidDisable,   /* void (* Disable)() */
  p9000OrchidValidate,  /* Bool (* Validate)() */
  p9000OrchidInitialize,/* void (* Initialize)() */
  P9000_VENDOR_ORCHID,  /* int Label */
};

/*************************************************/
/***************** ORCHID P9000 ******************/
/*************************************************/

/*
 * p9000OrchidProbe --
 *    Determines whether a Dimaond Viper is available.
 */
Bool
p9000OrchidProbe()
{
  char bios_sig[100];
  
  if (-1 == xf86ReadBIOS(BIOS_BASE, VPR_VLB_BIOS_OFFSET,
			 (unsigned char *)bios_sig, VPR_VLB_BIOS_LENGTH))
    return FALSE; /* This OS can't probe the BIOS */
  bios_sig[ORCHID_BIOS_LENGTH] = '\0';
  if (0 == strncmp(bios_sig, VPR_VLB_BIOS_SIGNATURE,
		   strlen(VPR_VLB_BIOS_SIGNATURE)))
    {
      if (xf86Verbose)
	ErrorF("%s BIOS signature for Orchid P9000 found:\n\t<%s>\n",
	       XCONFIG_PROBED, bios_sig);
      return TRUE;
    }
  else
    return FALSE;
}

/*
 * p9000OrchidValidate --
 *    Determines whether parameters are valid for the Orchid P9000.
 */
Bool
p9000OrchidValidate()
{
  /* Lets be safe... */
  p9000MaxClock = P9000_MAX_ORCHID_CLOCK;

#if 0
  if ((p9000InfoRec.MemBase != 0xC0000000) &&
      (p9000InfoRec.MemBase != 0xD0000000) &&
      (p9000InfoRec.MemBase != 0xE0000000))
#else
  if (p9000InfoRec.MemBase == 0)
#endif
    {
      ErrorF("%s: MemBase must be specified with Orchid P9000!\n",
	     p9000InfoRec.name);
      ErrorF("\tIf using a genuine Orchid card:\n");
      ErrorF("\t\tCheck your dip switches 6 and 7.\n");
      ErrorF("\t\tValid values are 0xC0000000, 0xD0000000, and 0xE0000000.\n");
      return(FALSE);
    }
  return(TRUE);
}

/*
 * p9000OrchidInitialize --
 *    Does one-time initialization for the Orchid P9000.
 */
void
p9000OrchidInitialize(scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
}

/*
 * p9000OrchidEnable --
 *    Initializes a Orchid P9000 for use. 
 */
void
p9000OrchidEnable(crtcRegs)
     p9000CRTCRegPtr crtcRegs;
{
  unsigned char VidSelectBits; /* Bits to output to output to Video Select register */

  if ( crtcRegs->hp != crtcRegs->vp )
    ErrorF("\a p9000OrchidEnable: ERROR, sync polarities not equal\n" ) ;

  if (!p9000OrchidInited)
    {   
      p9000OrchidSaveMisc = inb ((short) MISC_IN_REG);
  /* Save VGA Clock select bits, etc., if 1st time */
#ifdef DEBUG
  ErrorF ( "p9000OrchidEnable:debug info:p9000OrchidSaveMisc=inb(MISC_IN_REG)=0x%02X\n",
                           (int)p9000OrchidSaveMisc) ;
#endif
    }
  
  p9000BtEnable(crtcRegs);

  /* The Weitek P9000 VLB board uses a register mapped to the w5[12]86's
     "Output Control Register" address to select
     which device is enabled, etc.  It is located at 3c5 index 12.
   */

  p9000UnlockVGAExtRegs();

  outb((short)SEQ_INDEX_REG, (char)SEQ_OUTPUT_CTL_INDEX); 
  /* select the "video select" reg
   * this reg indicates vga memory size, and controls p9000/w5286
   * selection, and records extended clock modes */

  if (p9000OrchidInited)
    p9000OrchidSaveVidSelect = inb(SEQ_PORT) ; /* save value if 1st time */

  VidSelectBits = (
		   (((crtcRegs->hp == SP_POSITIVE)
		     && (crtcRegs->vp == SP_POSITIVE))
		    ? ORCHID_OCR_SYNC_POSITIVE : ORCHID_OCR_SYNC_NEGATIVE)
		   | ORCHID_OCR_ENABLE_P9000 /* this is a single bit */
		   /* These bits are not to be changed. */
		   | ( p9000OrchidSaveVidSelect & ORCHID_OCR_RESERVED_MASK )
		   );

  /* outb((short)SEQ_INDEX_REG, (char)SEQ_OUTPUT_CTL_INDEX) ; */
  outb((short) SEQ_PORT, (char)VidSelectBits ) ;

  p9000LockVGAExtRegs();

  outb((short)BT_PIXEL_MASK, (char)0xff);
  if (!p9000SWCursor)
    p9000BtCursorOn();
  p9000OrchidInited = TRUE;
}


/*
 * p9000OrchidDisable --
 *    Turns off the P9000 and reenables VGA
 */
void p9000OrchidDisable()
{
  unsigned char tmp;
  unsigned char VidSelectBits; /* Bits to output to output to */
                               /* Video Select register */
  p9000BtCursorOff();

  /* Enable the Orchid's video clock but disable the cursor */
  p9000OutBtReg(BT_COMMAND_REG_2, 0x0, BT_CR2_PORTSEL_NONMASK |
		BT_CR2_PCLK1 | BT_CR2_CURSOR_DISABLE);

  /* Turn video screen off so we don't see noise and trash */
  outb((short)SEQ_INDEX_REG, (char)SEQ_CLKMODE_INDEX);
  tmp = inb(SEQ_PORT);
  outb((short)SEQ_PORT, (char)(tmp | 0x20));

  p9000UnlockVGAExtRegs();

  /* Disable the P9000 and enable VGA. */

  /* select OCR = video select reg. */
  outb((short)SEQ_INDEX_REG, (char)SEQ_OUTPUT_CTL_INDEX);
  /* restore the previous state */
  outb((short) SEQ_PORT, (char)p9000OrchidSaveVidSelect);

  p9000LockVGAExtRegs();

  /* The freq. that should be in the ICD2061A's reg #2 is indicated by
   * bits 3 2 1 0 of the Video Select Reg. - so lets look it up in a
   * table and re-program the frequency. See Figure 12, page 51 in the
   * Weitek Power 9000 VL Bus Combination Board Application Note */
  /* Restore VGA Clock select bits, etc. */
  outb ((short)MISC_OUT_REG, (char)p9000OrchidSaveMisc); 
#ifdef DEBUG
  ErrorF ( "p9000OrchidDisable - debug:MISC_OUT set to 0x%02X\n", /* DEBUG */
          (int)( ( p9000OrchidSaveMisc ) | ORCHID_CLKSELBITS_P9000 )) ;
#endif

  usleep(30000);  /* Wait at least 10 msecs (ICD2061 timeout) for 
		   * the clock chip output freq to change */

  p9000BtRestore();

  /* Turn video screen back on */
  outb((short)SEQ_INDEX_REG, (char)SEQ_CLKMODE_INDEX);
  tmp = inb(SEQ_PORT);
  outb((short)SEQ_PORT, (char)(tmp & ~0x20));
}


/* 
 * p9000OrchidSetClock --
 *    Sets the clock to the specified dot
 *    and memory clocks
 */
void p9000OrchidSetClock(dotclock, memclock)
     long dotclock, memclock;  /* In HZ */
{
  unsigned int clock_ctrl_word , ActualHertz;
  int savemisc ;

  if ((labs(dotclock - memclock) <= CLK_JITTER_TOL)
      || (labs(dotclock - 2*memclock) <= CLK_JITTER_TOL)
      || (labs(2*dotclock - memclock) <= CLK_JITTER_TOL))
    {
      /* If dotclock and memclock are integer multiples, we'll have jitter
       * This method of fixing the problem is just temporary */
      if (memclock == MEMSPEED)	memclock = MEMSPEED_ALT;
      else                      memclock = MEMSPEED;
    }

  /* The register used should be different than the register
   * used for (vga) text mode so we can go back to it, but the weitek
     p9000-vlb app. note sez we should use reg 2 and clock select bits=3*/
  clock_ctrl_word = ICD2061ACalcClock (dotclock, 2);  
  ICD2061ASetClock (clock_ctrl_word);
  ActualHertz = ICD2061AGetClock (clock_ctrl_word);
  
#ifdef DEBUG
  ErrorF("Dot-clock actually set to %d Hz.  Wanted %ld\n",ActualHertz,
	 dotclock);
#endif
  
  clock_ctrl_word = ICD2061ACalcClock (memclock, 3); /* memclock uses reg 3 */
  ICD2061ASetClock (clock_ctrl_word);
  if (!p9000OrchidInited) /* check to see if it was ever 
			   * saved (for debugging only?)*/
   {
     ErrorF("p9000OrchidSetClock: ERROR: p9000OrchidSaveMisc uninitialized\n");
     p9000OrchidSaveMisc = 0x67 ; /* a wild guess as to what it should be ! */
   }

  /* set clock select bits for p9000 mode */
  outb ((short) MISC_OUT_REG,
	(char)((p9000OrchidSaveMisc & ORCHID_CLKSELBITS_MASK) 
	       | ORCHID_CLKSELBITS_P9000)); 
#ifdef DEBUG
  ErrorF ( "p9000OrchidSetClock - debug:MISC_OUT set to 0x%02X\n", /* DEBUG */
          (int)((p9000OrchidSaveMisc & ORCHID_CLKSELBITS_MASK)
		| ORCHID_CLKSELBITS_P9000));
#endif

  ActualHertz = ICD2061AGetClock (clock_ctrl_word);
  /* Wait at least 10 msecs (ICD2061 timeout) for the clock chip 
   * to change output */
  usleep(30000); 
  
#ifdef DEBUG
  ErrorF("Mem clock actually set to %d Hz.  Wanted %ld\n",ActualHertz,
	   memclock);
#endif
}


