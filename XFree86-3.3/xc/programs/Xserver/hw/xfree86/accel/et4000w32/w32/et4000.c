/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/et4000.c,v 1.1.2.4 1997/05/25 05:06:46 dawes Exp $ 
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
 *          ET6000 support by Koen Gadeyne
 *          split away from XFree 3.2A code (originally linked to SVGA et4000 driver)
 *            for XFree 3.2Xa
 */
/* $XConsortium: et4_driver.c /main/27 1996/10/28 04:48:15 kaleb $ */

#include "X.h"
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "vga.h"
#include "vgaPCI.h"

/* W32_ACCEL_SUPPORT is set for the XF86_W32 server, not for the SVGA server */
#ifdef W32_ACCEL_SUPPORT
#include "w32.h"
#endif

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

#ifndef W32_ACCEL_SUPPORT
#ifndef MONOVGA
#define USE_XAA  /* not XF86_W32 and not MONO and not VGA16 == XF86_SVGA */
#include "tseng_acl.h"
#endif
#endif

#ifndef MONOVGA
#include "vga256.h"
#endif

#ifdef W32_SUPPORT
typedef struct {
  unsigned char cmd_reg;
  unsigned char PLL_f2_M;
  unsigned char PLL_f2_N;
  unsigned char PLL_ctrl;
  unsigned char PLL_w_idx;
  unsigned char PLL_r_idx;
  } GenDACstate;
#endif

typedef struct {
  vgaHWRec std;               /* good old IBM VGA */
  unsigned char ExtStart;     /* Tseng ET4000 specials   CRTC 0x33/0x34/0x35 */
  unsigned char Compatibility;
  unsigned char OverflowHigh;
  unsigned char StateControl;    /* TS 6 & 7 */
  unsigned char AuxillaryMode;
  unsigned char Misc;           /* ATC 0x16 */
  unsigned char SegSel1, SegSel2;
  unsigned char HorOverflow;
  unsigned char ET6KMMAPCtrl;    /* ET6000 -- used for linear memory mapping */
  unsigned char ET6KVidCtrl1;    /* ET6000 -- used for 15/16 bpp modes */
  unsigned char ET6KMemBase;     /* ET6000 -- linear memory mapped address */
  unsigned char ET6KPerfContr;   /* ET6000 -- system performance control */
  unsigned char ET6KMclkM, ET6KMclkN; /* memory clock values */
#ifdef W32_SUPPORT
  unsigned char SegMapComp;     /* CRTC 0x30 */
  unsigned char VSConf1;        /* CRTC 0x36 */
  unsigned char VSConf2;        /* CRTC 0x37 */
  unsigned char IMAPortCtrl;    /* IMA port control register (0x217B index 0xF7) */
  GenDACstate gendac;
  Bool Gr_Mode;           /* kludge: true if we're dealing with a graphics mode */
#endif
#ifndef MONOVGA
  unsigned char RCConf;       /* CRTC 0x32 */
#endif
  } vgaET4000Rec, *vgaET4000Ptr;


#define TYPE_ET4000		0
#define TYPE_ET4000W32		1
#define TYPE_ET4000W32I		2
#define TYPE_ET4000W32P		3
#define TYPE_ET4000W32Pc	4
#define TYPE_ET6000		5

static Bool     ET4000Probe();
static char *   ET4000Ident();
static Bool     ET4000ClockSelect();
static Bool     ET6000ClockSelect();
static Bool     LegendClockSelect();
#ifdef W32_ACCEL_SUPPORT
static Bool     ICS5341ClockSelect();
static Bool     STG1703ClockSelect();
static Bool     ICD2061AClockSelect();
#endif
static void     ET4000EnterLeave();
static Bool     ET4000Init();
static int      ET4000ValidMode();
static void *   ET4000Save();
static void     ET4000Restore();
static void     ET4000Adjust();
static void     ET4000FbInit();
extern void     ET4000SetRead();
extern void     ET4000SetWrite();
extern void     ET4000SetReadWrite();
extern void     ET4000W32SetRead();
extern void     ET4000W32SetWrite();
extern void     ET4000W32SetReadWrite();
extern void	TsengAccelInit();
extern void     ET4000HWSaveScreen();

static unsigned char 	save_divide = 0;
#ifndef MONOVGA
static unsigned char    initialRCConf = 0x70;
#ifdef W32_SUPPORT
/* these should be taken from the "Saved" register set instead of this way */
static unsigned char    initialCompatibility = 0x18;
static unsigned char    initialVSConf1 = 0x03;
static unsigned char    initialVSConf2 = 0x0b;
static unsigned char    initialIMAPortCtrl = 0x20;
static unsigned char    initialET6KMemBase = 0xF0;
#endif
#endif
static unsigned char    initialET6KMclkM = 0x56, initialET6KMclkN = 0x25;
static unsigned char    initialET6KPerfContr = 0x3a;

static unsigned char    save_VSConf1=0x03;

int et4000_type;

unsigned long ET6Kbase;  /* PCI config space base address for ET6000 */

vgaVideoChipRec ET4000 = {
  ET4000Probe,
  ET4000Ident,
  ET4000EnterLeave,
  ET4000Init,
  ET4000ValidMode,
  ET4000Save,
  ET4000Restore,
  ET4000Adjust,
  ET4000HWSaveScreen,
  (void (*)())NoopDDA,
  ET4000FbInit,
  ET4000SetRead,
  ET4000SetWrite,
  ET4000SetReadWrite,
  0x20000,			/* ChipMapSize (0x10000 for normal vga,
                                                0x20000 for accelerator in banked mode */
  0x10000,			/* ChipSegmentSize, 16k*/
  16,				/* ChipSegmentShift */
  0xFFFF,			/* ChipSegmentMask */
  0x00000, 0x10000,		/* ChipReadBottom, ChipReadTop  */
  0x00000, 0x10000,		/* ChipWriteBottom,ChipWriteTop */
  TRUE,				/* ChipUse2Banks, Uses 2 bank */
  VGA_NO_DIVIDE_VERT,		/* ChipInterlaceType -- don't divide verts */
  {0,},				/* ChipOptionFlags */
  8,				/* ChipRounding */
  FALSE,			/* ChipUseLinearAddressing */
  0,				/* ChipLinearBase */
  0,				/* ChipLinearSize */
  FALSE,			/* ChipHas16bpp */
  FALSE,			/* ChipHas24bpp */
  FALSE,			/* ChipHas32bpp */
  NULL,				/* ChipBuiltinModes */
  1,				/* ChipClockScaleFactor */
};

#define new ((vgaET4000Ptr)vgaNewVideoState)

static SymTabRec chipsets[] = {
  { TYPE_ET4000,	"et4000" },
#ifdef W32_SUPPORT
  { TYPE_ET4000W32,	"et4000w32" },
  { TYPE_ET4000W32I,	"et4000w32i" },
  { TYPE_ET4000W32P,	"et4000w32p" },
  { TYPE_ET4000W32Pc,	"et4000w32p" },
  { TYPE_ET6000,	"et6000" },
#endif
  { -1,			"" },
};

Bool (*ClockSelect)();

static unsigned ET4000_ExtPorts[] = {0x3B8, 0x3BF, 0x3CD, 0x3D8,
#ifdef W32_SUPPORT
	0x217a, 0x217b,		/* These last two are W32 specific */
#endif
};

static int Num_ET4000_ExtPorts = 
	(sizeof(ET4000_ExtPorts)/sizeof(ET4000_ExtPorts[0]));

/* ET6000 PCI-config space ports
 */
#define Num_ET6000_PCIPorts 0x88
static unsigned int ET6000_PCIPorts[Num_ET6000_PCIPorts];


/*
 * ET4000Ident
 */

static char *
ET4000Ident(n)
     int n;
{
  if (chipsets[n].token < 0)
    return(NULL);
  else
    return(chipsets[n].name);
}


/*
 * ET4000ClockSelect --
 *      select one of the possible clocks ...
 */

static Bool
ET4000ClockSelect(no)
     int no;
{
  static unsigned char save1, save2, save3, save4;
  unsigned char temp;

  switch(no)
  {
    case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaIOBase + 4, 0x34); save2 = inb(vgaIOBase + 5);
      outb(0x3C4, 7); save3 = inb(0x3C5);
      if( et4000_type > TYPE_ET4000 )
      {
         outb(vgaIOBase + 4, 0x31); save4 = inb(vgaIOBase + 5);
      }
      break;
    case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outw(vgaIOBase + 4, 0x34 | (save2 << 8));
      outw(0x3C4, 7 | (save3 << 8));
      if( et4000_type > TYPE_ET4000 )
      {
         outw(vgaIOBase + 4, 0x31 | (save4 << 8));
      }
      break;
    default:
      temp = inb(0x3CC);
      outb(0x3C2, ( temp & 0xf3) | ((no << 2) & 0x0C));
      outb(vgaIOBase + 4, 0x34);	/* don't nuke the other bits in CR34 */
      temp = inb(vgaIOBase + 5);
      outw(vgaIOBase + 4, 0x34 | ((temp & 0xFD) << 8) | ((no & 0x04) << 7));

#ifndef OLD_CLOCK_SCHEME
      {
         outb(vgaIOBase + 4, 0x31);
         temp = inb(vgaIOBase + 5);
         outb(vgaIOBase + 5, (temp & 0x3f) | ((no & 0x10) << 2));
         outb(0x3C4, 7); temp = inb(0x3C5);
         outb(0x3C5, (save_divide ^ ((no & 0x8) << 3)) | (temp & 0xBF));
      }
#else
      {
         outb(0x3C4, 7); temp = inb(0x3C5);
         outb(0x3C5, (save_divide ^ ((no & 0x10) << 2)) | (temp & 0xBF));
      }
#endif
  }
  return(TRUE);
}

/*
 * LegendClockSelect --
 *      select one of the possible clocks ...
 */

static Bool
LegendClockSelect(no)
     int no;
{
  /*
   * Sigma Legend special handling
   *
   * The Legend uses an ICS 1394-046 clock generator.  This can generate 32
   * different frequencies.  The Legend can use all 32.  Here's how:
   *
   * There are two flip/flops used to latch two inputs into the ICS clock
   * generator.  The five inputs to the ICS are then
   *
   * ICS     ET-4000
   * ---     ---
   * FS0     CS0
   * FS1     CS1
   * FS2     ff0     flip/flop 0 output
   * FS3     CS2
   * FS4     ff1     flip/flop 1 output
   *
   * The flip/flops are loaded from CS0 and CS1.  The flip/flops are
   * latched by CS2, on the rising edge. After CS2 is set low, and then high,
   * it is then set to its final value.
   *
   */
  static unsigned char save1, save2;
  unsigned char temp;

  switch(no)
  {
    case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaIOBase + 4, 0x34); save2 = inb(vgaIOBase + 5);
      break;
    case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outw(vgaIOBase + 4, 0x34 | (save2 << 8));
      break;
    default:
      temp = inb(0x3CC);
      outb(0x3C2, (temp & 0xF3) | ((no & 0x10) >> 1) | (no & 0x04));
      outw(vgaIOBase + 4, 0x0034);
      outw(vgaIOBase + 4, 0x0234);
      outw(vgaIOBase + 4, ((no & 0x08) << 6) | 0x34);
      outb(0x3C2, (temp & 0xF3) | ((no << 2) & 0x0C));
  }
  return(TRUE);
}

/*
 * ET6000ClockSelect --
 *      programmable clock chip
 */

static Bool
ET6000ClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = ET4000ClockSelect(freq);
      break;
   default:
      {
        ET6000SetClock(freq, 2);
        result = ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}


#ifdef W32_ACCEL_SUPPORT
/*
 * ICS5341ClockSelect --
 *      programmable clock chip
 */

static Bool
ICS5341ClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = ET4000ClockSelect(freq);
      break;
   default:
      {
        /*
	 * right now this is never called
	 * the code programs the clocks directly :-(
	 */
        ET4000gendacSetClock(freq, 2); /* can't fail */
        result = ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}

/*
 * STG1703ClockSelect --
 *      programmable clock chip
 */

static Bool
STG1703ClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = ET4000ClockSelect(freq);
      break;
   default:
      {
        /*
	 * right now this is never called
	 * the code programs the clocks directly :-(
	 */
        ET4000stg1703SetClock(freq, 2); /* can't fail */
        result = ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}

/*
 * ICD2061AClockSelect --
 *      programmable clock chip
 */

static Bool
ICD2061AClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = ET4000ClockSelect(freq);
      break;
   default:
      {
        Et4000AltICD2061SetClock((long)freq*1000, 2); /* can't fail */
        result = ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}
#endif

#ifdef USE_XAA
/*
 * ET4000LinMem --
 *      handle linear memory mode stuff
 */

static Bool
ET4000LinMem(Bool autodetect)
{
 /* W32p cards can give us their Lin. memory address through the PCI
  * configuration. For W32i, this is not possible (VL-bus, MCA or ISA). W32i
  * cards have three extra external "address" lines, SEG2..SEG0 which _can_
  * be connected to any set of address lines in addition to the already
  * connected A23..A0. SEG2..SEG0 are either for three extra address lines
  * or to connect an external address decoder (mostly an 74F27). It is NOT
  * possible to know how SEG2..SEG0 are connected. We _assume_ they are
  * connected to A26..A24 (most likely case). This means linear memory can
  * be decoded into any 4MB block in the address range 0..128MB.
  */

 /* This code will probably only work for PCI and VLB bus cards. MCA is weird,
  * ISA is out of the question (I think).
  */
  
#define DEFAULT_LINEAR_BASE 0x80000000

  long temp=0;
  
  if (vga256InfoRec.MemBase != 0)   /* MemBase given from XF86Config */
  {
    switch(et4000_type)
    {
      case TYPE_ET4000W32I:   /* A26..A22 are decoded */
        if (vga256InfoRec.MemBase & ~0x07C00000)
          ErrorF("%s %s: MemBase out of range. Must be <= 0x07C00000 on 4MB boundary.\n",
                 XCONFIG_PROBED, vga256InfoRec.name);
        vga256InfoRec.MemBase &= ~0x07C00000;
        break;
      case TYPE_ET4000W32Pc: /* A31,A30 decoded from PCI config space */
        temp |= vgaPCIInfo->MemBase & 0xC0000000;  /* get A31,30 from PCI config */
      case TYPE_ET4000W32P: /* A31,A30 are decoded as 00 (=always mapped below 512 MB) */
        temp |= 0x3FC00000; /* A29..A22 */
        if (vga256InfoRec.MemBase & ~temp )
          ErrorF("%s %s: MemBase out of range. Must be <= 0x%x on 16MB boundary.\n",
                 XCONFIG_PROBED, vga256InfoRec.name, temp);
        vga256InfoRec.MemBase &= ~temp;
        break;
      case TYPE_ET6000:
        if (vga256InfoRec.MemBase & 0xFF000000)
          ErrorF("%s %s: MemBase out of range. Must be <= 0xFF000000 on 16MB boundary.\n",
                 XCONFIG_PROBED, vga256InfoRec.name, temp);
        vga256InfoRec.MemBase &= 0xFF000000;
        break;
      default:
        ErrorF("%s %s: This chipset does not support linear memory.\n",
               XCONFIG_PROBED, vga256InfoRec.name);
        return (FALSE); /* no can do */
    }
  }
  else     /* MemBase not given: find it */
  {
    switch(et4000_type)
    {
      case TYPE_ET4000W32I:   /* A26..A22 are decoded */
        outb(vgaIOBase+0x04, 0x30);
        vga256InfoRec.MemBase = (inb(vgaIOBase+0x05) & 0x1F) << 22;
        break;
      case TYPE_ET4000W32Pc: /* A31,A30 decoded from PCI config space */
      case TYPE_ET4000W32P:  /* A31,A30 are decoded as 00 (=always mapped below 512 MB) */
        vga256InfoRec.MemBase = vgaPCIInfo->MemBase;
        break;
      case TYPE_ET6000:
        if ( (vgaPCIInfo->MemBase !=0) && (autodetect) ) /* don't trust PCI when not autodetecting */
          vga256InfoRec.MemBase = vgaPCIInfo->MemBase;
        else if (inb(ET6Kbase+0x13) != 0)
        {
          vga256InfoRec.MemBase = inb(ET6Kbase+0x13) << 24;
          ErrorF("%s %s: ET6000: port-probed linear memory base = 0x%x\n",
                  XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.MemBase);
        }
        else /* Argghh... nobody set up the linear address base yet. Guess */
        {
          ErrorF("%s %s: ET6000: Could not determine linear memory base address."
                 " Setting it to 0x%X\n",
                 XCONFIG_PROBED, vga256InfoRec.name, DEFAULT_LINEAR_BASE);
          vga256InfoRec.MemBase = DEFAULT_LINEAR_BASE;
        }
        break;
      default:
        ErrorF("%s %s: This chipset does not support linear memory.\n",
               XCONFIG_PROBED, vga256InfoRec.name);
        return (FALSE); /* no can do */
    }
  }
  /*
   * And now for some really ugly hacking. Memory mapping the MMIO registers
   * (which is required for accelerator support) uses the system's mmap()
   * call. This call requires the base address to be on a page boundary on
   * some systems. Therefor, we will _not_ ask for memory-mapped IO at the
   * correct address (=ET4000.ChipLinearBase+0x003FFF00), and let the system
   * also map out memory for the video frame buffer at ET4000.ChipLinearBase
   * for ET4000.ChipLinearSize bytes. Instead, we will ask for the entire 4
   * MB of memory mapped all at once in one block. This also saves us from
   * having to add a hook in vga.c to do that extra step of mapping (MMIO).
   *
   * NOTE however that this means the memory mapped IO registers OVERLAP
   * with the last 8kb of memory in the frame buffer!. Thus only 4MB-8kb of
   * video memory can be used for the display. What a mess...
   */

  /* 16 Meg is _actually_ mapped out by the ET6000, but we won't tell the
   * mmap() code about that. Since all mmap()-ed space is also reflected in
   * the "ps" listing, too many users would be worried by a server that
   * always eats 16MB of memory, even if it's not "real" memory, just
   * address space.
   */

  ET4000.ChipLinearSize = 4096 * 1024;

  ET4000.ChipLinearBase = vga256InfoRec.MemBase;

  ET4000.ChipUseLinearAddressing = TRUE;
  if (ET4000.ChipLinearBase==0L)
  {
    ErrorF("%s %s: Linear memory address == 0x0. KABOOM! Going back to banked mode.\n",
            XCONFIG_PROBED, vga256InfoRec.name);
    ET4000.ChipUseLinearAddressing = FALSE;
    return(FALSE);
  }
  return(TRUE);
}
#endif


static Bool
ET6000InitVars(Bool autodetect)
{
   int i;

   /* check the PCI config for ET6000 data. This assumes ET6000 cards are
    * _always_ on the PCI bus. They _can_ be on the VL-bus, but I don't
    * think they'll be making any VLB cards with this chip anyway.
    *
    * If chipset was forced (=no autodetect via PCI), don't use
    * PCI-scanner-provided data, because specifying the chipset might have
    * been done to overcome a misdetected card (e.g. if two VGA cards are
    * in the system, XFree's PCI scanner might give us the wrong one).
    */

   /* define used IO ports... Is this really necessary for PCI config space IO? */
   for (i=0; i<Num_ET6000_PCIPorts; i++)
     ET6000_PCIPorts[i] = ET6Kbase+i;
   xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_ET6000_PCIPorts, ET6000_PCIPorts);
   
   ET4000EnterLeave(ENTER);

   if (autodetect)
   {
     ET6Kbase = vgaPCIInfo->IOBase & ~0xFF;
   }
   else
   {
     /* get PCI IOBase from CRTC registers instead of from XFree PCI structure */
     outb(vgaIOBase + 4, 0x21); ET6Kbase  = (inb(vgaIOBase + 5) << 8);
     outb(vgaIOBase + 4, 0x22); ET6Kbase += (inb(vgaIOBase + 5) << 16);
     outb(vgaIOBase + 4, 0x23); ET6Kbase += (inb(vgaIOBase + 5) << 24); /* keep this split up */
     ErrorF("%s %s: ET6000: port-probed I/O base = 0x%x\n",
             XCONFIG_PROBED, vga256InfoRec.name, ET6Kbase);
   }

  /*
   * clock related stuff
   */
  
  ClockSelect = ET6000ClockSelect;
  OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions);
  OFLG_SET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions);

  outb(ET6Kbase+0x67, 0x0f);
  ErrorF("%s %s: ET6000: CLKDAC ID: 0x%X\n",
      XCONFIG_PROBED, vga256InfoRec.name, inb(ET6Kbase+0x69));

#ifdef DO_WE_NEED_THIS
  vga256InfoRec.clocks = 3;
  vga256InfoRec.clock[0] = 25175;
  vga256InfoRec.clock[1] = 28322;
  vga256InfoRec.clock[2] = 31500; /* this one will be reprogrammed */
#endif
  if (vga256InfoRec.dacSpeeds[0] <= 0)
  {
    vga256InfoRec.dacSpeeds[0] = 135000;
    vga256InfoRec.maxClock = 135000;
  }

  ErrorF("%s %s: ET6000: Using built-in 135 MHz programmable Clock Chip/RAMDAC\n",
      XCONFIG_PROBED, vga256InfoRec.name);

  return(TRUE);
}


/*
 * ET4000AutoDetect -- Old-style autodetection code (by register probing)
 *
 * This code is only called when the chipset is not given beforehand,
 * and if the PCI code hasn't detected one previously.
 */

Bool ET4000AutoDetect()
{
  unsigned char temp, origVal, newVal;

  ET4000EnterLeave(ENTER);
  /*
   * Check first that there is a ATC[16] register and then look at
   * CRTC[33]. If both are R/W correctly it's a ET4000 !
   */
  temp = inb(vgaIOBase+0x0A); 
  outb(0x3C0, 0x16 | 0x20); origVal = inb(0x3C1);
  outb(0x3C0, origVal ^ 0x10);
  outb(0x3C0, 0x16 | 0x20); newVal = inb(0x3C1);
  outb(0x3C0, origVal);
  if (newVal != (origVal ^ 0x10))
  {
    ET4000EnterLeave(LEAVE);
    return(FALSE);
  }

  outb(vgaIOBase+0x04, 0x33);          origVal = inb(vgaIOBase+0x05);
  outb(vgaIOBase+0x05, origVal ^ 0x0F); newVal = inb(vgaIOBase+0x05);
  outb(vgaIOBase+0x05, origVal);
  if (newVal != (origVal ^ 0x0F))
  {
    ET4000EnterLeave(LEAVE);
    return(FALSE);
  }

  et4000_type = TYPE_ET4000;

#ifdef W32_SUPPORT
 /*
  * Now check for an ET4000/W32.
  * Test for writability of 0x3cb.
  */
  origVal = inb(0x3cb);
  outb(0x3cb, 0x33);	/* Arbitrary value */
  newVal = inb(0x3cb);
  outb(0x3cb, origVal);
  if (newVal == 0x33)
  {
    /* We have an ET4000/W32. Now determine the type. */
    outb(0x217a, 0xec);
    temp = inb(0x217b) >> 4;
    switch (temp) {
      case 0 : /* ET4000/W32 */
          et4000_type = TYPE_ET4000W32;
          break;
      case 1 : /* ET4000/W32i */
      case 3 : /* ET4000/W32i rev b */ 
      case 11: /* ET4000/W32i rev c */
          et4000_type = TYPE_ET4000W32I;
          break;
      case 2 : /* ET4000/W32p rev a */
      case 5 : /* ET4000/W32p rev b */
          et4000_type = TYPE_ET4000W32P;
      case 6 : /* ET4000/W32p rev d */
      case 7 : /* ET4000/W32p rev c */
          et4000_type = TYPE_ET4000W32Pc;
          break;
      default :
          ErrorF("%s %s: ET4000W32: Unknown type. Try chipset override.\n",
              XCONFIG_PROBED, vga256InfoRec.name);
          ET4000EnterLeave(LEAVE);
          return(FALSE);
    }
  }
#endif

  vga256InfoRec.chipset = xf86TokenToString(chipsets, et4000_type);      

  return(TRUE);
}


/*
 * TsengDetectMem --
 *      try to find amount of video memory intalled.
 */

static Bool
TsengDetectMem()
{
  unsigned char config;
  int ramtype=0;
  
  if (et4000_type == TYPE_ET6000)
  {
    ramtype = inb(0x3C2) & 0x03;
    switch (ramtype)
    {
      case 0x03:  /* MDRAM */
        ramtype=0;
        vga256InfoRec.videoRam = ((inb(ET6Kbase+0x47) & 0x07) + 1) * 8*32; /* number of 8 32kb banks  */
        if (inb(ET6Kbase+0x45) & 0x04)
        {
          vga256InfoRec.videoRam <<= 1;
        }
        ErrorF("%s %s: ET6000: Detected %d kb of multi-bank DRAM\n",
            XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.videoRam);
        break;
      case 0x00:  /* DRAM */
        ramtype=1;
        vga256InfoRec.videoRam = 1024 << (inb(ET6Kbase+0x45) & 0x03);
        ErrorF("%s %s: ET6000: Detected %d Mb of standard DRAM\n",
            XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.videoRam);
        break;
      default:    /* unknown RAM type */
        ErrorF("%s %s: ET6000: Unknown video memory type %d -- assuming 1 MB (unless specified)\n",
            XCONFIG_PROBED, vga256InfoRec.name, ramtype);
        vga256InfoRec.videoRam = 1024;
    }
  }
  else /* pre-ET6000 devices */
  {  
     outb(vgaIOBase+0x04, 0x37); config = inb(vgaIOBase+0x05);
     
     switch(config & 0x03) {
     case 1: vga256InfoRec.videoRam = 256; break;
     case 2: vga256InfoRec.videoRam = 512; break;
     case 3: vga256InfoRec.videoRam = 1024; break;
     }

     if (config & 0x80) vga256InfoRec.videoRam <<= 1;

#ifdef W32_SUPPORT
     /* Check for interleaving on W32i/p. */
     if (et4000_type >= TYPE_ET4000W32I)
     {
         outb(vgaIOBase+0x04, 0x32);
         config = inb(vgaIOBase+0x05);
         if (config & 0x80)
         {
           vga256InfoRec.videoRam <<= 1;
           ErrorF("%s %s: ET4000W32: Interleaved DRAM detected.\n",
             XCONFIG_PROBED, vga256InfoRec.name);
         }
     }
#endif
  }
}


/*
 * ET4000Probe --
 *      check whether a Et4000 based board is installed
 */

static Bool
ET4000Probe()
{
  int numClocks;
  Bool autodetect = TRUE;

  /*
   * Set up I/O ports to be used by this card
   */
  xf86ClearIOPortList(vga256InfoRec.scrnIndex);
  xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
  xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_ET4000_ExtPorts, ET4000_ExtPorts);
 
  /* Try to detect a Tseng video card */

  if (vga256InfoRec.chipset)   /* no auto-detect: chipset is given */
  {
    et4000_type = xf86StringToToken(chipsets, vga256InfoRec.chipset);
    if (et4000_type < 0)
        return FALSE;
    autodetect = FALSE;
  }
  else if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_TSENG)
  {
    switch(vgaPCIInfo->ChipType)
      {
        case PCI_CHIP_ET6000:
          et4000_type = TYPE_ET6000;
          break;
        case PCI_CHIP_ET4000_W32P_A:
        case PCI_CHIP_ET4000_W32P_B:
          et4000_type = TYPE_ET4000W32P;
          break;
        case PCI_CHIP_ET4000_W32P_C:
        case PCI_CHIP_ET4000_W32P_D:
          et4000_type = TYPE_ET4000W32Pc;
          break;
        default: 
          ErrorF("%s %s: Unknown Tseng Labs PCI device 0x%x -- please report.\n",
                 XCONFIG_PROBED, vga256InfoRec.name, vgaPCIInfo->ChipType);
      }
    vga256InfoRec.chipset = xf86TokenToString(chipsets, et4000_type);      
  }
  else  /* all else failed -- try old-style autodetect */
    if (ET4000AutoDetect()==FALSE) return(FALSE);

  if (et4000_type == TYPE_ET6000)
      ET6000InitVars(autodetect);

  ET4000EnterLeave(ENTER);

 
  /*
   * Detect how much memory is installed
   */
  if (!vga256InfoRec.videoRam)
     TsengDetectMem();

#ifdef W32_SUPPORT
  /*
   * If more than 1MB of RAM is available on the W32i/p, use the
   * W32-specific banking function that can address 4MB.
   */ 
  if (et4000_type > TYPE_ET4000 && vga256InfoRec.videoRam > 1024) {
      ET4000.ChipSetRead = ET4000W32SetRead;
      ET4000.ChipSetWrite= ET4000W32SetWrite;
      ET4000.ChipSetReadWrite = ET4000W32SetReadWrite;
  }
#endif


#ifdef USE_XAA
  /*
   * Linear mode and >8bpp mode handling
   */

  if (et4000_type >= TYPE_ET4000W32Pc)
  {
    OFLG_SET(OPTION_LINEAR, &ET4000.ChipOptionFlags);
  }

  /* currently only W32p rev C and up support linear memory */
  if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
  {
    if (et4000_type < TYPE_ET4000W32Pc)
    {
      ErrorF("%s %s: Linear memory mode disabled (not yet supported on this device).\n",
             XCONFIG_PROBED, vga256InfoRec.name);
      OFLG_CLR(OPTION_LINEAR, &vga256InfoRec.options);
    }
  }

  /* Use banked addressing by default. */
  if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
  {
    if (!ET4000LinMem(autodetect))
      ErrorF("%s %s: Linear memory mode not supported on this device.\n",
             XCONFIG_PROBED, vga256InfoRec.name);
  }

  if (et4000_type >= TYPE_ET6000)  /* currently only ET6000 has >8bpp */
  {
    ET4000.ChipHas16bpp = TRUE;
    ET4000.ChipHas24bpp = TRUE;
    ET4000.ChipHas32bpp = TRUE;
  }

  /*
   * acceleration-related stuff
   *
   * Currently only works on W32p or newer chips. W32i doesn't work yet.
   */  

  if (et4000_type > TYPE_ET4000W32I) 
  {
    if (et4000_type < TYPE_ET6000)
    {
      if (vga256InfoRec.videoRam > (4096-516))
      {
         ErrorF("%s %s: Only 4096-516 kb of memory can be used in accelerated mode.\n",
                XCONFIG_PROBED, vga256InfoRec.name);
         vga256InfoRec.videoRam = 4096-516;
      }
    }
    else
    {
      if (vga256InfoRec.videoRam > (4096-8))
      {
         ErrorF("%s %s: Only 4096-8 kb of memory can be used in accelerated mode.\n",
                XCONFIG_PROBED, vga256InfoRec.name);
         vga256InfoRec.videoRam = 4096-8;
      }
    }

    if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
    {
      ErrorF("%s %s: Reserving 1kb of video memory for accelerator.\n",
             XCONFIG_PROBED, vga256InfoRec.name);
      vga256InfoRec.videoRam -= 1; /* 1kb reserved for accelerator code */
    }
    
    if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)
        && OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
    {
      if (et4000_type < TYPE_ET6000)
      {
        ErrorF("%s %s: Acceleration disabled (not yet supported in linear mode).\n",
               XCONFIG_PROBED, vga256InfoRec.name);
        OFLG_SET(OPTION_NOACCEL, &vga256InfoRec.options);
      }
    }
  }
  else   /* no acceleration supported */
  {
    /* this makes life easier in the rest of the server code */
    OFLG_SET(OPTION_NOACCEL, &vga256InfoRec.options);
  }

  OFLG_SET(OPTION_NOACCEL, &ET4000.ChipOptionFlags);

 /* currently, the power-saving method used by the XFree86 SVGA servers
  * causes the DRAM refresh on both ET4000 and ET6000 boards to be disabled,
  * resulting in video memory corruption. Until a better DPMS implementation
  * is added, disable this option.
  */
  if (OFLG_ISSET(OPTION_POWER_SAVER, &vga256InfoRec.options)) {
    ErrorF("%s %s: option \"power_saver\" not working in this release (disabled).\n",
           XCONFIG_GIVEN, vga256InfoRec.name);
    OFLG_CLR(OPTION_POWER_SAVER, &vga256InfoRec.options);
  }

#endif

  if (et4000_type < TYPE_ET6000)
  {
    /* Initialize option flags allowed for this driver */
    OFLG_SET(OPTION_LEGEND, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_HIBIT_HIGH, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_HIBIT_LOW, &ET4000.ChipOptionFlags);
#ifndef MONOVGA
    OFLG_SET(OPTION_PCI_BURST_ON, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_PCI_BURST_OFF, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_W32_INTERLEAVE_ON, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_W32_INTERLEAVE_OFF, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_SLOW_DRAM, &ET4000.ChipOptionFlags);
    OFLG_SET(OPTION_FAST_DRAM, &ET4000.ChipOptionFlags);
#endif

/*
 * because of some problems with W32 cards, SLOW_DRAM is _always_ enabled
 * for those cards
 */
 if (et4000_type <= TYPE_ET4000W32)
 {
   ErrorF("%s %s: option \"slow_dram\" is enabled by default on this card.\n",
         XCONFIG_PROBED, vga256InfoRec.name);
   OFLG_SET(OPTION_SLOW_DRAM, &vga256InfoRec.options);
 }

#ifdef W32_ACCEL_SUPPORT
    if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
    {
      if (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
      {
        ClockSelect = ICS5341ClockSelect;
        numClocks = 3;
      }
      else if (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
      {
        ClockSelect = STG1703ClockSelect;
        numClocks = 3;
      }
      else if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))
      {
        ClockSelect = ICD2061AClockSelect;
        numClocks = 3;
      }
      else
      {
        ErrorF("Unsuported programmable Clock chip.\n");
        ET4000EnterLeave(LEAVE);
        return(FALSE);
      }
    }
    else
#endif
    {
      if (OFLG_ISSET(OPTION_LEGEND, &vga256InfoRec.options))
        {
          ClockSelect = LegendClockSelect;
          numClocks   = 32;
        }
      else
        {
          ClockSelect = ET4000ClockSelect;
          if( et4000_type > TYPE_ET4000 )
             numClocks = 32;
  	else
             numClocks = 16;
        }
    }   
    
    if (OFLG_ISSET(OPTION_HIBIT_HIGH, &vga256InfoRec.options))
      {
        if (OFLG_ISSET(OPTION_HIBIT_LOW, &vga256InfoRec.options))
          {
            ET4000EnterLeave(LEAVE);
            FatalError(
               "\nOptions \"hibit_high\" and \"hibit_low\" are incompatible\n");
          }
        save_divide = 0x40;
      }
    else if (OFLG_ISSET(OPTION_HIBIT_LOW, &vga256InfoRec.options))
      save_divide = 0;
    else
      {
        /* Check for initial state of divide flag */
        outb(0x3C4, 7);
        save_divide = inb(0x3C5) & 0x40;
        ErrorF("%s %s: ET4000: Initial hibit state: %s\n", XCONFIG_PROBED,
               vga256InfoRec.name, save_divide & 0x40 ? "high" : "low");
      }
  } /* et4000_type < ET6000 */

#ifndef MONOVGA
    /* Save initial RCConf value */
    outb(vgaIOBase + 4, 0x32); initialRCConf = inb(vgaIOBase + 5);
#ifdef W32_SUPPORT
    if (et4000_type > TYPE_ET4000) {
      /* Save initial Auxctrl (CRTC 0x34) value */
      outb(vgaIOBase + 4, 0x34); initialCompatibility = inb(vgaIOBase + 5);
      /* Save initial VSConf1 (CRTC 0x36) value */
      outb(vgaIOBase + 4, 0x36); initialVSConf1 = inb(vgaIOBase + 5);
      /* Save initial VSConf2 (CRTC 0x37) value */
      outb(vgaIOBase + 4, 0x37); initialVSConf2 = inb(vgaIOBase + 5);
      if (et4000_type < TYPE_ET6000)
      {
        /* Save initial IMAPortCtrl value */
        outb(0x217a, 0xF7); initialIMAPortCtrl = inb(0x217b);
      }
    }
#endif
    if (et4000_type == TYPE_ET6000)
    {
      int tmp = inb(ET6Kbase+0x67);
      initialET6KMemBase   = inb(ET6Kbase+0x13);
      initialET6KPerfContr = inb(ET6Kbase+0x41);
      outb(ET6Kbase+0x67, 10);
      initialET6KMclkM = inb(ET6Kbase+0x69);
      initialET6KMclkN = inb(ET6Kbase+0x69);
      outb(ET6Kbase+0x67, tmp);
      ErrorF("%s %s: ET6000: MClk: %3.2f MHz\n", XCONFIG_PROBED,
             vga256InfoRec.name, gendacMNToClock(initialET6KMclkM, initialET6KMclkN)/1000.0);
    }
#endif

  if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
    if (!vga256InfoRec.clocks) vgaGetClocks(numClocks, ClockSelect);
  }
  
  vga256InfoRec.bankedMono = TRUE;
#ifndef MONOVGA
#ifdef XFreeXDGA
  vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  return(TRUE);
}


/*
 * ET4000FbInit --
 *	initialise the cfb SpeedUp functions
 */

static void
ET4000FbInit()
{
#ifndef MONOVGA
  int useSpeedUp;

  if (xf86Verbose && ET4000.ChipUseLinearAddressing)
          ErrorF("%s %s: %s: Using linear framebuffer at 0x%08X.\n",
                  XCONFIG_PROBED, vga256InfoRec.name,
                  vga256InfoRec.chipset, ET4000.ChipLinearBase);

  if (vga256InfoRec.videoRam > 1024)
    useSpeedUp = vga256InfoRec.speedup & SPEEDUP_ANYCHIPSET;
  else
    useSpeedUp = vga256InfoRec.speedup & SPEEDUP_ANYWIDTH;
  if (useSpeedUp && xf86Verbose)
  {
    ErrorF("%s %s: ET4000: SpeedUps selected (Flags=0x%x)\n", 
	   OFLG_ISSET(XCONFIG_SPEEDUP,&vga256InfoRec.xconfigFlag) ?
           XCONFIG_GIVEN : XCONFIG_PROBED,
           vga256InfoRec.name, useSpeedUp);
  }
  if (useSpeedUp & SPEEDUP_FILLRECT)
  {
    vga256LowlevFuncs.fillRectSolidCopy = speedupvga256FillRectSolidCopy;
  }
  if (useSpeedUp & SPEEDUP_BITBLT)
  {
    vga256LowlevFuncs.doBitbltCopy = speedupvga256DoBitbltCopy;
  }
  if (useSpeedUp & SPEEDUP_LINE)
  {
    vga256LowlevFuncs.lineSS = speedupvga256LineSS;
    vga256LowlevFuncs.segmentSS = speedupvga256SegmentSS;
    vga256TEOps1Rect.Polylines = speedupvga256LineSS;
    vga256TEOps1Rect.PolySegment = speedupvga256SegmentSS;
    vga256TEOps.Polylines = speedupvga256LineSS;
    vga256TEOps.PolySegment = speedupvga256SegmentSS;
    vga256NonTEOps1Rect.Polylines = speedupvga256LineSS;
    vga256NonTEOps1Rect.PolySegment = speedupvga256SegmentSS;
    vga256NonTEOps.Polylines = speedupvga256LineSS;
    vga256NonTEOps.PolySegment = speedupvga256SegmentSS;
  }
  if (useSpeedUp & SPEEDUP_FILLBOX)
  {
    vga256LowlevFuncs.fillBoxSolid = speedupvga256FillBoxSolid;
  }
#endif /* MONOVGA */
}


/*
 * ET4000EnterLeave --
 *      enable/disable io-mapping
 */

static void 
ET4000EnterLeave(enter)
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
      outb(0x3BF, 0x03);                           /* unlock ET4000 special */
      outb(vgaIOBase + 8, 0xA0);
      outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, temp & 0x7F);
    }
  else
    {
      outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, temp | 0x80);
      outb(vgaIOBase + 8, 0x00);
      outb(0x3D8, 0x29);
      outb(0x3BF, 0x01);                           /* relock ET4000 special */

      xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    }
}



/*
 * ET4000Restore --
 *      restore a video mode
 */

static void 
ET4000Restore(restore)
  vgaET4000Ptr restore;
{
  unsigned char i;

#ifndef W32_ACCEL_SUPPORT
  vgaProtect(TRUE);
#endif

  outb(0x3CD, 0x00); /* segment select bits 0..3 */
  if (et4000_type > TYPE_ET4000)
    outb(0x3CB, 0x00); /* segment select bits 4,5 */

#ifdef W32_ACCEL_SUPPORT
  if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
  {
    if (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
    {
       /* Restore ICS 5341 GenDAC Command and PLL registers */
       outb(vgaIOBase + 4, 0x31);
       i = inb(vgaIOBase + 5);
       outb(vgaIOBase + 4, 0x31);
       outb(vgaIOBase + 5, i | 0x40);

       outb(0x3c6, restore->gendac.cmd_reg);      /* Enhanced command register*/
       outb(0x3c8, 2);                            /* index to f2 reg */
       outb(0x3c9, restore->gendac.PLL_f2_M);     /* f2 PLL M divider */
       outb(0x3c9, restore->gendac.PLL_f2_N);     /* f2 PLL N1/N2 divider */

       outb(0x3c8, 0x0e);                         /* index to PLL control */
       outb(0x3c9, restore->gendac.PLL_ctrl);     /* PLL control */
       outb(0x3c8, restore->gendac.PLL_w_idx);    /* PLL write index */
       outb(0x3c7, restore->gendac.PLL_r_idx);    /* PLL read index */

       outb(vgaIOBase + 4, 0x31);
       outb(vgaIOBase + 5, i & ~0x40);
    }
    if (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
    {
       /* Restore STG 1703 GenDAC Command and PLL registers 
        * we share one data structure with the gendac code, so the names
	* are not too good.
	*/

       STG1703setIndex(0x24,restore->gendac.PLL_f2_M);
       outb(0x3c6,restore->gendac.PLL_f2_N);      /* use autoincrement */
       STG1703setIndex(0x03,restore->gendac.PLL_ctrl);/* write same value to */
       outb(0x3c6,restore->gendac.PLL_ctrl);	  /* primary and secondary 
       						   * pixel mode select register 
						   */
       STG1703magic(0);
       xf86dactopel();
       xf86setdaccomm(restore->gendac.cmd_reg);   /* write enh command reg */
    }
    if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))
    {
       /* Alas... The ICD clock registers cannot be restored */
    }
  }
#endif
  if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
  {
    if (OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions))
    {
       /* Restore ET6000 CLKDAC PLL registers */
       i = inb(ET6Kbase+0x67); /* remember old CLKDAC index register pointer */
       outb(ET6Kbase+0x67, 2);
       outb(ET6Kbase+0x69, restore->gendac.PLL_f2_M);
       outb(ET6Kbase+0x69, restore->gendac.PLL_f2_N);
       /* set MClk values if needed, but don't touch them if not needed */
#ifdef ALLOW_ET6K_FAST_DRAM
       if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
       {
         outb(ET6Kbase+0x67, 10);
         outb(ET6Kbase+0x69, restore->ET6KMclkM);
         outb(ET6Kbase+0x69, restore->ET6KMclkN);
       }
#endif
       /* restore old index register */
       outb(ET6Kbase+0x67, i);
    }
  }

  if (et4000_type == TYPE_ET6000)
  {
    outb(ET6Kbase+0x13, restore->ET6KMemBase);
    outb(ET6Kbase+0x40, restore->ET6KMMAPCtrl);
    outb(ET6Kbase+0x58, restore->ET6KVidCtrl1);
    outb(ET6Kbase+0x41, restore->ET6KPerfContr);
  }
  
  outw(vgaIOBase + 4, (restore->HorOverflow << 8)  | 0x3F);
  outw(vgaIOBase + 4, (restore->SegMapComp << 8)  | 0x30);
 
  vgaHWRestore((vgaHWPtr)restore);

  outw(0x3C4, (restore->StateControl << 8)  | 0x06);
  outw(0x3C4, (restore->AuxillaryMode << 8) | 0x07);
  i = inb(vgaIOBase + 0x0A); /* reset flip-flop */
  outb(0x3C0, 0x36); outb(0x3C0, restore->Misc);
  outw(vgaIOBase + 4, (restore->ExtStart << 8)      | 0x33);
  if (restore->std.NoClock >= 0)
    outw(vgaIOBase + 4, (restore->Compatibility << 8) | 0x34);
  outw(vgaIOBase + 4, (restore->OverflowHigh << 8)  | 0x35);
#ifndef MONOVGA
#ifdef W32_SUPPORT  
  if (et4000_type > TYPE_ET4000)
  {
    outw(vgaIOBase + 4, (restore->VSConf1 << 8)  | 0x36);
    /* 
     * We must also save VSConf1 in save_VSConf1, because we are at
     * this moment in the middle of a sync reset, and we will have
     * saved the OLD value, which we want to change now.
     */
    save_VSConf1 = restore->VSConf1;
    outw(vgaIOBase + 4, (restore->VSConf2 << 8)  | 0x37);
    if (et4000_type < TYPE_ET6000)
    {
      outw(0x217a, (restore->IMAPortCtrl << 8)  | 0xF7);
    }
  }
#endif
#ifdef WHY_WOULD_YOU_RESTRICT_THAT_TO_THIS_OPTION
  if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
#endif
    outw(vgaIOBase + 4, (restore->RCConf << 8)  | 0x32);
#endif
  outb(0x3CD, restore->SegSel1);
  if (et4000_type > TYPE_ET4000)
    outb(0x3CB, restore->SegSel2);

  /*
   * This might be required for the Legend clock setting method, but
   * should not be used for the "normal" case because the high order
   * bits are not set in NoClock when returning to text mode.
   */
  if (OFLG_ISSET(OPTION_LEGEND, &vga256InfoRec.options))
    if (restore->std.NoClock >= 0)
      {
	vgaProtect(TRUE);
        (ClockSelect)(restore->std.NoClock);
#ifdef W32_ACCEL_SUPPORT
	vgaProtect(FALSE);
#endif
      }

#ifdef USE_XAA
  /* MEGA-kludge: we need to know if we're "restoring" a graphics mode or not.
   * If so, init the ACL, else, don't touch it...  Yuck.
   */
  if (restore->Gr_Mode && (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)))
  {
    tseng_init_acl(); /* initialize the accelerator registers for XAA interface */
  }
#endif
#ifndef W32_ACCEL_SUPPORT
  vgaProtect(FALSE);
#endif
}



/*
 * ET4000Save --
 *      save the current video mode
 */

static void *
ET4000Save(save)
     vgaET4000Ptr save;
{
  unsigned char             i, temp, saveseg1=0, saveseg2=0;

  /*
   * we need this here , cause we MUST disable the ROM SYNC feature
   * this bit changed with W32p_rev_c...
   */
  outb(vgaIOBase + 4, 0x34); temp = inb(vgaIOBase + 5);
  if( et4000_type < TYPE_ET4000W32Pc) {
     outb(vgaIOBase + 5, temp & 0x1F);
  }

  saveseg1 = inb(0x3CD); outb(0x3CD, 0x00); /* segment select 1 */
  if (et4000_type > TYPE_ET4000) {
    saveseg2 = inb(0x3CB);
    outb(0x3CB, 0x00); /* segment select 2 */
  }

  save = (vgaET4000Ptr)vgaHWSave((vgaHWPtr)save, sizeof(vgaET4000Rec));
  save->Compatibility = temp;
  save->SegSel1 = saveseg1;
  save->SegSel2 = saveseg2;

  outb(vgaIOBase + 4, 0x33); save->ExtStart     = inb(vgaIOBase + 5);
  outb(vgaIOBase + 4, 0x35); save->OverflowHigh = inb(vgaIOBase + 5);
#ifdef W32_SUPPORT  
  if (et4000_type > TYPE_ET4000)
  {
    outb(vgaIOBase + 4, 0x36); save->VSConf1 = inb(vgaIOBase + 5);
    outb(vgaIOBase + 4, 0x37); save->VSConf2 = inb(vgaIOBase + 5);
    if (et4000_type < TYPE_ET6000)
    {
      outb(0x217a, 0xF7); save->IMAPortCtrl = inb(0x217b);
    }
  }
#endif
#ifndef MONOVGA
#ifdef WHY_WOULD_YOU_RESTRICT_THAT_TO_THIS_OPTION
  if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
#endif
    outb(vgaIOBase + 4, 0x32); save->RCConf = inb(vgaIOBase + 5);
#endif
  outb(0x3C4, 6); save->StateControl  = inb(0x3C5);
  outb(0x3C4, 7); save->AuxillaryMode = inb(0x3C5);
  save->AuxillaryMode |= 0x14;
  i = inb(vgaIOBase + 0x0A); /* reset flip-flop */
  outb(0x3C0,0x36); save->Misc = inb(0x3C1); outb(0x3C0, save->Misc);
#ifdef W32_ACCEL_SUPPORT
  if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
  {
    if (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
    {
      /* Restore ICS 5341 GenDAC Command and PLL registers */
      outb(vgaIOBase + 4, 0x31);
      i = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, i | 0x40);

      save->gendac.cmd_reg = inb(0x3c6);      /* Enhanced command register */
      save->gendac.PLL_w_idx = inb(0x3c8);    /* PLL write index */
      save->gendac.PLL_r_idx = inb(0x3c7);    /* PLL read index */
      outb(0x3c7, 2);                         /* index to f2 reg */
      save->gendac.PLL_f2_M = inb(0x3c9);     /* f2 PLL M divider */
      save->gendac.PLL_f2_N = inb(0x3c9);     /* f2 PLL N1/N2 divider */
      outb(0x3c7, 0x0e);                      /* index to PLL control */
      save->gendac.PLL_ctrl = inb(0x3c9);     /* PLL control */

      outb(vgaIOBase + 5, i & ~0x40);
    }
    if (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
    {
      /* Restore STG 1703 GenDAC Command and PLL registers 
       * unfortunately we reuse the gendac data structure, so the 
       * field names are not really good.
       */

      xf86dactopel();
      save->gendac.cmd_reg = xf86getdaccomm();/* Enhanced command register */
      save->gendac.PLL_f2_M = STG1703getIndex(0x24);    
                                              /* f2 PLL M divider */
      save->gendac.PLL_f2_N = inb(0x3c6);     /* f2 PLL N1/N2 divider */
      save->gendac.PLL_ctrl = STG1703getIndex(0x03);    
                                              /* pixel mode select control */
    }
    if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))
    {
      /* ICD2061A clock registers cannot be saved, 'cause they cannot be read */ 
    }
  }
#endif

  if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
  {
    if (OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions))
    {
       /* Save ET6000 CLKDAC PLL registers */
       i = inb(ET6Kbase+0x67); /* remember old CLKDAC index register pointer */
       outb(ET6Kbase+0x67, 2);
       save->gendac.PLL_f2_M = inb(ET6Kbase+0x69);
       save->gendac.PLL_f2_N = inb(ET6Kbase+0x69);
       /* save MClk values */
       outb(ET6Kbase+0x67, 10);
       save->ET6KMclkM = inb(ET6Kbase+0x69);
       save->ET6KMclkN = inb(ET6Kbase+0x69);
       /* restore old index register */
       outb(ET6Kbase+0x67, i);
    }
  }

  if (et4000_type == TYPE_ET6000)
  {
    save->ET6KMemBase   = inb(ET6Kbase+0x13);
    save->ET6KMMAPCtrl  = inb(ET6Kbase+0x40);
    save->ET6KVidCtrl1  = inb(ET6Kbase+0x58);
    save->ET6KPerfContr = inb(ET6Kbase+0x41);
  }
  
  outb(vgaIOBase + 4, 0x30); save->SegMapComp = inb(vgaIOBase + 5);
  outb(vgaIOBase + 4, 0x3F); save->HorOverflow = inb(vgaIOBase + 5);

  save->Gr_Mode=FALSE;

  return ((void *) save);
}



/*
 * ET4000Init --
 *      Handle the initialization of the VGAs registers
 */

static Bool
ET4000Init(mode)
     DisplayModePtr mode;
{
  int row_offset;
  int BytesPerPix = vga256InfoRec.bitsPerPixel>>3;

#ifdef W32_ACCEL_SUPPORT
  int pixMuxShift = 0;

  /* define pixMuxShift depending on mode, pixel multiplexing and ramdac type */

  if (mode->Flags & V_PIXMUX)
  {
     if ((OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions)) 
          && (W32RamdacType==ICS5341_DAC))
     {
        pixMuxShift =  mode->Flags & V_DBLCLK ? 1 : 0;
     }  
     if ((OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions)) 
          && (W32RamdacType==STG1703_DAC))
     {
        pixMuxShift =  mode->Flags & V_DBLCLK ? 1 : 0;
     }  
  }

  if (!mode->CrtcHAdjusted) {
     if (pixMuxShift > 0) {
        /* now divide the horizontal timing parameters as required */
        mode->CrtcHTotal     >>= pixMuxShift;
        mode->CrtcHDisplay   >>= pixMuxShift;
        mode->CrtcHSyncStart >>= pixMuxShift;
        mode->CrtcHSyncEnd   >>= pixMuxShift;
	mode->CrtcHSkew      >>= pixMuxShift;
     }
     else if (pixMuxShift < 0) {
        /* now multiply the horizontal timing parameters as required */
        mode->CrtcHTotal     <<= -pixMuxShift;
        mode->CrtcHDisplay   <<= -pixMuxShift;
        mode->CrtcHSyncStart <<= -pixMuxShift;
        mode->CrtcHSyncEnd   <<= -pixMuxShift;
	mode->CrtcHSkew      <<= -pixMuxShift;
     }
     mode->CrtcHAdjusted = TRUE;
  }
#endif


  if (!vgaHWInit(mode,sizeof(vgaET4000Rec)))
    return(FALSE);



#ifdef MONOVGA
  /* Don't ask me why this is needed on the ET6000 and not on the others */
  if (et4000_type >= TYPE_ET6000) new->std.Sequencer[1] |= 0x04;
  row_offset = new->std.CRTC[19];
#else
  new->std.Attribute[16] = 0x01;  /* use the FAST 256 Color Mode */
  row_offset = vga256InfoRec.displayWidth >> 3; /* overruled by 16/24/32 bpp code */
#endif
  new->std.CRTC[20] = 0x60;
  new->std.CRTC[23] = 0xAB;
  new->StateControl = 0x00; 
  new->AuxillaryMode = 0xBC;
  new->ExtStart = 0x00;

  new->OverflowHigh = (mode->Flags & V_INTERLACE ? 0x80 : 0x00)
    | 0x10
      | ((mode->CrtcVSyncStart & 0x400) >> 7 )
	| (((mode->CrtcVDisplay -1) & 0x400) >> 8 )
	  | (((mode->CrtcVTotal -2) & 0x400) >> 9 )
	    | (((mode->CrtcVSyncStart) & 0x400) >> 10 );

#ifdef MONOVGA
  new->Misc = 0x00;
#else
  new->Misc = 0x80;
#endif

#ifndef MONOVGA
  new->RCConf = initialRCConf;
  if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
  {
    /*
     *  make sure Trsp is no more than 75ns
     *            Tcsw is 25ns
     *            Tcsp is 25ns
     *            Trcd is no more than 50ns
     * Timings assume SCLK = 40MHz
     *
     * Note, this is experimental, but works for me (DHD)
     */
    /* Tcsw, Tcsp, Trsp */
    new->RCConf &= ~0x1F;
    if (initialRCConf & 0x18)
      new->RCConf |= 0x08;
    /* Trcd */
    new->RCConf &= ~0x20;
  }
#ifdef W32_SUPPORT
  /*
   * Here we make sure that CRTC regs 0x34 and 0x37 are untouched, except for 
   * some bits we want to change. 
   * Notably bit 7 of CRTC 0x34, which changes RAS setup time from 4 to 0 ns 
   * (performance),
   * and bit 7 of CRTC 0x37, which changes the CRTC FIFO low treshold control.
   * At really high pixel clocks, this will avoid lots of garble on the screen 
   * when something is being drawn. This only happens WAY beyond 80 MHz 
   * (those 135 MHz ramdac's...)
   */
   if (et4000_type > TYPE_ET4000) {
     if (! OFLG_ISSET(OPTION_SLOW_DRAM, &vga256InfoRec.options))
       new->Compatibility = (initialCompatibility & 0x7F) | 0x80;
     new->VSConf1 = initialVSConf1;
     new->VSConf2 = initialVSConf2;
     new->IMAPortCtrl = initialIMAPortCtrl;
     if (vga256InfoRec.clock[mode->Clock] > 80000)
       new->VSConf2 = (new->VSConf2 & 0x7f) | 0x80;
   }

   if (et4000_type >= TYPE_ET4000W32P)
   {
      /*
       * next, we check the PCI Burst option and turn that on or of
       * which is done with bit 4 in CR34
       */
      if (OFLG_ISSET(OPTION_PCI_BURST_OFF, &vga256InfoRec.options))
         new->Compatibility &= 0xEF;
      if (OFLG_ISSET(OPTION_PCI_BURST_ON, &vga256InfoRec.options))
         new->Compatibility |= 0x10;
   }
   if (et4000_type >= TYPE_ET4000W32I)
   {
     /*
      * now on to the memory interleave setting (CR32 bit 7)
      */
      if (OFLG_ISSET(OPTION_W32_INTERLEAVE_OFF, &vga256InfoRec.options))
         new->RCConf &= 0x7F;
      if (OFLG_ISSET(OPTION_W32_INTERLEAVE_ON, &vga256InfoRec.options))
         new->RCConf |= 0x80;
   }
#endif

#endif
    
  /* Set clock-related registers when not Legend
   * cannot really SET the clock here yet, since the ET4000Save()
   * is called LATER, so it would save the wrong state...
   * and since they use the ET4000Restore() to actually SET vga regs,
   * we can only set the clock there (that's why we copy Synthclock into
   * the other struct.
   */

#ifdef W32_ACCEL_SUPPORT
    if (    (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
         && (    (W32RamdacType == STG1703_DAC)
              || (W32RamdacType == ICS5341_DAC)) )
    { 
      /* for pixmux: must use post-div of 4 on ICS GenDAC clock generator!
       */

       if (mode->Flags & V_PIXMUX)
       {
         commonCalcClock(mode->SynthClock,1,1,31,2,3,100000,vga256InfoRec.dacSpeeds[0]*2+1, 
         		 &(new->gendac.PLL_f2_M), &(new->gendac.PLL_f2_N));
         if( W32RamdacType == STG1703_DAC ) {
            new->gendac.cmd_reg |= 8;
            new->gendac.PLL_ctrl = 0x05;              /* set DAC to 2*8 mode */
         } else if( W32RamdacType == ICS5341_DAC ) {
            new->gendac.cmd_reg = 0x10;               /* set DAC to 2*8 mode */
            new->gendac.PLL_ctrl = 0;
         }
         new->Misc = (new->Misc & 0xCF) | 0x20;   /* bits 5 and 4 set 8/16 bit 
	 					   * DAC mode, at the W32 side 
						   * (DAC needs to be set, too)
                                                   * here we set it to 16-bit 
						   * mode 
						   */

#if THIS_SHOULD_BE_NECESSARY_BUT_FAILS
         /* set doubleword adressing -- seems to be needed for <1280 modes 
	  * to get correct screen 
	  */
         new->std.CRTC[0x14] = (new->std.CRTC[0x14] & 0x9F) | 0x40;
#endif
         new->std.CRTC[0x17] = (new->std.CRTC[0x17] & 0xFB);
         
         /* to avoid blurred vertical line during flyback, disable H-blanking 
	  * (better solution needed !!!) 
	  */
         new->std.CRTC[0x02] = 0xff;
       }
       else
       {
         commonCalcClock(mode->SynthClock,1,1,31,0,3,100000,vga256InfoRec.dacSpeeds[0]*2+1, 
         		 &(new->gendac.PLL_f2_M), &(new->gendac.PLL_f2_N));
         if( W32RamdacType == STG1703_DAC ) {
            new->gendac.cmd_reg |= 8;
            new->gendac.PLL_ctrl = 0x00;              /* set DAC to 1*8 mode */
         } else if( W32RamdacType == ICS5341_DAC ) {
            new->gendac.cmd_reg = 0x00;               /* set DAC to 1*8 mode */
            new->gendac.PLL_ctrl = 0;
         }
         new->Misc = (new->Misc & 0xCF);   /* 8 bit DAC mode */
       }
       new->gendac.PLL_w_idx = 0;
       new->gendac.PLL_r_idx = 0;
       
       /* the programmed clock will be on clock index 2 */
       /* disable MCLK/2 and MCLK/4 */
       new->AuxillaryMode = (new->AuxillaryMode & 0xBE);   
       /* clear CS2: we need clock #2 */
       new->Compatibility = (new->Compatibility & 0xFD);   
       new->std.MiscOutReg = (new->std.MiscOutReg & 0xF3) | 0x08; 
       new->std.NoClock = 2;
    }
    else
    if (    (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
              && (    (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))))
    {
       /* the programmed clock will be on clock index 2 */
       /* disable MCLK/2 and MCLK/4 */
       new->AuxillaryMode = (new->AuxillaryMode & 0xBE);   
       /* clear CS2: we need clock #2 */
       new->Compatibility = (new->Compatibility & 0xFD);   
       new->std.MiscOutReg = (new->std.MiscOutReg & 0xF3) | 0x08; 
       new->std.NoClock = 2;
       ICD2061AClockSelect(mode->SynthClock);
    }
    else
#endif

    if (et4000_type>=TYPE_ET6000)
    {
       /* setting min_n2 to "1" will ensure a more stable clock ("0" is allowed though) */
       commonCalcClock(vga256InfoRec.clock[new->std.NoClock],1,1,31,1,3,
                 100000,vga256InfoRec.dacSpeeds[0]*2, 
       		 &(new->gendac.PLL_f2_M), &(new->gendac.PLL_f2_N));

       /* ErrorF("M=0x%x ; N=0x%x\n",new->gendac.PLL_f2_M, new->gendac.PLL_f2_N);*/
       /* above 100MB/sec, we enable the "LOW FIFO threshold" */
       if (vga256InfoRec.clock[new->std.NoClock] * BytesPerPix > 100000)
       {
         new->ET6KPerfContr = initialET6KPerfContr | 0x10;
       }
       else
       {
         new->ET6KPerfContr = initialET6KPerfContr & ~0x10;
       }

#ifdef ALLOW_ET6K_FAST_DRAM
       if (OFLG_ISSET(OPTION_FAST_DRAM, &vga256InfoRec.options))
       {
         /*
          * FAST_DRAM sets the memory clock to 100 MHz instead of the
          * standard 90 MHz. The lowest speed-grade of the MDRAMs is 100 MHz
          * anyway, and the ET6000 core is designed to run at 135 MHz.
          *
          * For the tweakers: every step increment of ET6KMclkM with 1
          * increases the memory clock with about 2.5 MHz: Memory clock =
          * (ET6KMclkM+2)*14.31818/(2*3).
          *
          * 0x28 = 100 MHz ; 0x2C = 110 MHz; 0x30 = 120 MHz.
          *   120 MHz causes permanent mayhem on my system 
          *    -- don't try this at home!
          *
          * Currently disabled -- causes trouble [KMG].
          */
         new->ET6KMclkM = 0x28;
         new->ET6KMclkN = 0x21;
       }
       else
       {
         new->ET6KMclkM = initialET6KMclkM;
         new->ET6KMclkN = initialET6KMclkN;
       }
#endif

       /* force clock #2 */
       new->Compatibility = (new->Compatibility & 0xFD);   
       new->std.MiscOutReg = (new->std.MiscOutReg & 0xF3) | 0x08; 
       new->std.NoClock = 2;
       /* ET6000ClockSelect(vga256InfoRec.clock[new->std.NoClock]); */
    }
    else
    if (new->std.NoClock >= 0)
    {
      new->AuxillaryMode = (save_divide ^ ((new->std.NoClock & 8) << 3)) |
                           (new->AuxillaryMode & 0xBF);
      new->Compatibility = (new->Compatibility & 0xFD) | 
      				((new->std.NoClock & 0x04) >> 1);
    }
  
  /*
   * linear mode handling
   */

   if (et4000_type==TYPE_ET6000)
   {
      if (ET4000.ChipUseLinearAddressing)
      {
         new->ET6KMemBase = vga256InfoRec.MemBase >> 24;
         new->ET6KMMAPCtrl |= 0x09;
      }
      else
      {
         new->ET6KMMAPCtrl &= ~0x09;
         new->ET6KMemBase = initialET6KMemBase;
      }
   }
   else  /* et4000 style linear memory */
   {
      if (ET4000.ChipUseLinearAddressing)
      {
         new->VSConf1 |= 0x10;
         new->SegMapComp = (vga256InfoRec.MemBase >> 22) & 0xFF;
         new->std.Graphics[6] &= ~0x0C;
         new->IMAPortCtrl &= ~0x01; /* disable IMA port (to get >1MB lin mem) */
      }
      else
      {
         new->VSConf1 &= ~0x10;
         if (et4000_type < TYPE_ET4000W32P)
           new->SegMapComp = 0x1C;  /* default value */
         else
           new->SegMapComp = 0x00;
      }
    }
  
  /*
   * 16/24/32 bpp handling -- currently only for ET6000
   */

   if ((et4000_type==TYPE_ET6000) && (vga256InfoRec.bitsPerPixel>=8))
   {
     new->Misc &= 0xCF; /* clear BPP bits -- This needs to be modified for pixel multiplexing */
     new->Misc |= (BytesPerPix-1) << 4; /* set BPP bits for desired mode */
     row_offset *= BytesPerPix;
     
     if (vga256InfoRec.bitsPerPixel == 16)
     {
        if (xf86weight.red == 5 && xf86weight.green == 5 && xf86weight.blue == 5)
            new->ET6KVidCtrl1 &= ~0x02; /* 5-5-5 RGB mode */
        if (xf86weight.red == 5 && xf86weight.green == 6 && xf86weight.blue == 5)
            new->ET6KVidCtrl1 |=  0x02; /* 5-6-5 RGB mode */
     }
   }

  /*
   * Horizontal overflow settings: for modes with > 2048 pixels per line
   */

   new->std.CRTC[19] = row_offset;
   new->std.Attribute[17] = 0x00; /* overscan color is set to 0xFF by main VGA code. Why? */
   new->HorOverflow = ((((mode->CrtcHTotal>>3)-5) & 0x100) >> 8)
     | ((((mode->CrtcHDisplay>>3)-1) & 0x100) >> 7)
       | ((((mode->CrtcHSyncStart>>3)-1) & 0x100) >> 6)
         | (((mode->CrtcHSyncStart>>3) & 0x100) >> 4)
           | ((row_offset & 0x200) >> 3)
             | ((row_offset & 0x100) >> 1);

  new->Gr_Mode=TRUE;

  /*
   * Enable memory mapped IO registers when acceleration is needed.
   */

#ifdef USE_XAA
  if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
  {
    if (et4000_type==TYPE_ET6000)
    {
      if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
        new->ET6KMMAPCtrl |= 0x02; /* MMU can't be used here (causes system hang...) */
      else
        new->ET6KMMAPCtrl |= 0x06; /* MMU is needed in banked accelerated mode */
    }
    else
    {
      new->VSConf1 |= 0x28;
    }
  }
#endif

  return(TRUE);
}



/*
 * ET4000Adjust --
 *      adjust the current video frame to display the mousecursor.
 */

static void 
ET4000Adjust(x, y)
     int x, y;
{

#ifdef MONOVGA
  int Base = (y * vga256InfoRec.displayWidth + x + 3) >> 3;
#else
  int BytesPerPix = vga256InfoRec.bitsPerPixel>>3;
  int Base = ((y * vga256InfoRec.displayWidth + x + 1)*BytesPerPix) >> 2;
  /* adjust Base address so it is a non-fractional multiple of BytesPerPix */
  Base -= (Base % BytesPerPix);
#endif

  outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);
  outw(vgaIOBase + 4, ((Base & 0x0F0000) >> 8) | 0x33);

#ifdef XFreeXDGA
  if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
    /* Wait until vertical retrace is in progress. */
    while (inb(vgaIOBase + 0xA) & 0x08);
    while (!(inb(vgaIOBase + 0xA) & 0x08));
  }
#endif
}

/*
 * ET4000HWSaveScreen --
 *
 *   perform a sequencer reset.
 *
 * The ET4000 "Video System Configuration 1" register (CRTC index 0x36),
 * which is used to set linear memory mode and MMU-related stuff, is
 * partially reset to "0" when TS register index 1 bit 1 is set (synchronous
 * reset): bits 3..5 are reset during a sync. reset. The problem is that
 * sync reset is active during the register setup (ET4000Restore()), and
 * thus VSConf1 never gets written...).
 *
 * We hook this function so that we can remember/restore VSConf1.
 */

void
ET4000HWSaveScreen(start)
Bool start;
{
#ifndef PC98_NEC480
#ifndef PC98_EGC
  if (start == SS_START)
  {
    if (et4000_type > TYPE_ET4000)
    {
      outb(vgaIOBase + 4, 0x36);
      save_VSConf1 = inb(vgaIOBase + 5);
    }
    vgaHWSaveScreen(start);
  }
  else
  {
    vgaHWSaveScreen(start);
    if (et4000_type > TYPE_ET4000)
    {
      outw(vgaIOBase + 4, (save_VSConf1 << 8) | 0x36);
    }
  }
#endif
#endif
}


/*
 * ET4000ValidMode --
 *
 */
static int 
ET4000ValidMode(mode, verbose)
DisplayModePtr mode;
Bool verbose;
{
return MODE_OK;
}
