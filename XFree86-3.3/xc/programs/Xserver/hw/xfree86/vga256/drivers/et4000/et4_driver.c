/*
 * $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/et4_driver.c,v 3.45.2.14 1997/05/30 12:59:07 dawes Exp $ 
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
 *          ET6000 and ET4000W32 16/24/32 bpp support by Koen Gadeyne
 *          DPMS support by Harald Nordgård Hansen
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

#include "tseng.h"

#ifndef MONOVGA
#include "vga256.h"
#endif


static Bool     ET4000Probe();
static char *   ET4000Ident();
static void     ET4000EnterLeave();
static Bool     ET4000Init();
static int      ET4000ValidMode();
static void *   ET4000Save();
static void     ET4000Restore();
static void     ET4000Adjust();
static void     ET4000FbInit();
#ifdef DPMSExtension
static void     TsengCrtcDPMSSet();
static void     TsengHVSyncDPMSSet();
#endif
extern void     ET4000SetRead();
extern void     ET4000SetWrite();
extern void     ET4000SetReadWrite();
extern void     ET4000W32SetRead();
extern void     ET4000W32SetWrite();
extern void     ET4000W32SetReadWrite();
extern void	TsengAccelInit();
extern void     ET4000HWSaveScreen();

unsigned char 	tseng_save_divide = 0;
#ifdef USE_XAA
/* Do we use PCI-retry or busy-waiting */
Bool Use_Pci_Retry = 0;

/* Do we use the XAA acceleration architecture */
static Bool Use_ACL = FALSE;
#endif

#ifndef MONOVGA
#include "tseng_cursor.h"
extern vgaHWCursorRec vgaHWCursor;

static unsigned char    initialRCConf = 0x70;
#ifdef W32_SUPPORT
/* these should be taken from the "Saved" register set instead of this way */
static unsigned char    initialCompatibility = 0x18;
static unsigned char    initialVSConf1 = 0x03;
static unsigned char    initialVSConf2 = 0x0b;
static unsigned char    initialIMAPortCtrl = 0x20;
#endif
#endif
static unsigned char    initialET6KMemBase = 0xF0;
static unsigned char    initialET6KMclkM = 0x56, initialET6KMclkN = 0x25;
static unsigned char    initialET6KPerfContr = 0x3a;

static unsigned char    save_VSConf1=0x03;

static int bustype=0;    /* W32 bus type (currently used for lin mem on W32i) */

/* some exported variables */
t_tseng_type et4000_type = TYPE_UNKNOWN;

unsigned long ET6Kbase;  /* PCI config space base address for ET6000 */

static pciConfigPtr tseng_pcr = NULL;


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
  0x10000,			/* ChipSegmentSize, 64k */
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
  /*
   * This is TRUE if the driver has support for the given depth for 
   * the detected configuration. It must be set in the Probe function.
   * It most cases it should be FALSE.
   */
  FALSE,			/* ChipHas16bpp */
  FALSE,			/* ChipHas24bpp */
  FALSE,			/* ChipHas32bpp */
  NULL,				/* ChipBuiltinModes */
  1,				/* ChipClockMulFactor */
  1 				/* ChipClockDivFactor */
};

#define new ((vgaET4000Ptr)vgaNewVideoState)

static SymTabRec chipsets[] = {
  { TYPE_ET4000,	"ET4000" },
#ifdef W32_SUPPORT
  { TYPE_ET4000W32,	"ET4000W32" },
  { TYPE_ET4000W32I,	"ET4000W32i" },
  { TYPE_ET4000W32Ib,	"ET4000W32i_rev_b" },
  { TYPE_ET4000W32Ic,	"ET4000W32i_rev_c" },
  { TYPE_ET4000W32P,	"ET4000W32p" }, /* treated as a W32p_rev_a, used for backwards-compatibility */
  { TYPE_ET4000W32Pa,	"ET4000W32p_rev_a" },
  { TYPE_ET4000W32Pb,	"ET4000W32p_rev_b" },
  { TYPE_ET4000W32Pc,	"ET4000W32p_rev_c" },
  { TYPE_ET4000W32Pd,	"ET4000W32p_rev_d" },
#endif
  { TYPE_ET6000,	"ET6000" },
  { -1,			"" },
};

Bool (*ClockSelect)();

static unsigned ET4000_ExtPorts[] = {0x3B8, 0x3BF, 0x3CD, 0x3CB, 0x3D8,
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

 /*
  * For non-PCI cards (especially VLB), most motherboards don't decode all
  * 32 address bits. The weird default memory base below will always end up
  * at the end of the decoded address space -- independent of the number of
  * address bits that are decoded.
  */
#define DEFAULT_LIN_MEMBASE ( (256 + 128 + 64 + 32 + 16 + 8 + 4) * 1024*1024 )
#define DEFAULT_LIN_MEMBASE_PCI (DEFAULT_LIN_MEMBASE & 0xFF000000)

 /* This code will probably only work for PCI and VLB bus cards. MCA is weird,
  * ISA is out of the question (I think).
  */
  
  unsigned long mask = -1L;
  unsigned char bus;

  if (vgaBitsPerPixel < 8) return FALSE;

  if (et4000_type < TYPE_ET4000W32I) {
    ErrorF("%s %s: This chipset does not support linear memory.\n",
           XCONFIG_PROBED, vga256InfoRec.name);
    return (FALSE); /* no can do */
  }

  switch(et4000_type)
  {
    case TYPE_ET4000W32I:
    case TYPE_ET4000W32Ib:
    case TYPE_ET4000W32Ic:
        /*
         * Notation: SMx = bit x of Segment Map Comparator (CRTC index 0x30)
         *
         * We assume the driver code disables the image port (which it does)
         *
         * ISA:      [ A23, A22, A21, A20 ] ==      [ SM1, SM0, 0, 0 ]
         * MCA: [ A24, A23, A22, A21, A20 ] == [ SM2, SM1, SM0, 0, 0 ]
         * VLB: [ /A26, /A25, /A24, A23, A22, A21, A20 ] ==   ("/" means inverted!)
         *       [ SM4,  SM3,  SM2, SM1, SM0, 0  , 0   ]
         */
        outb(0x217A, 0xEF); bus = inb(0x217B) & 0x60;   /* Determine bus type */
        ErrorF("%s %s: Detected W32i bus type: ", XCONFIG_PROBED, vga256InfoRec.name);
        switch (bus) {
            case 0x40:
                ErrorF("MCA.\n");
                bustype = BUS_MCA;
                mask = 0x01C00000; /* MADE24, A23 and A22 are decoded */
                break;
            case 0x60:
                ErrorF("Local Bus.\n");
                bustype = BUS_VLB;
                mask = 0x07C00000; /* A26..A22 are decoded */
                break;
            case 0x00:
            case 0x20:
            default:
                ErrorF("ISA.\n");
                bustype = BUS_ISA;
                mask = 0x00C00000; /* SEGE and A22 are decoded */
                break;
        }
        break;
    case TYPE_ET4000W32P:
    case TYPE_ET4000W32Pa:
    case TYPE_ET4000W32Pb:
    case TYPE_ET4000W32Pc:
    case TYPE_ET4000W32Pd:
        outb(0x217A, 0xEF); bus = inb(0x217B) >> 3;   /* Determine bus type */
        ErrorF("%s %s: Detected W32p bus type (0x%02x): ",
               XCONFIG_PROBED, vga256InfoRec.name, bus);
        switch (bus) {
            case 0x1C:
                ErrorF("Local Buffered Bus or PCI.\n");
                bustype = BUS_PCI;
                mask = 0x3FC00000; /* A29..A22 */
                break;
            case 0x13:
                ErrorF("Local Bus option 1a.\n");
                bustype = BUS_VLB;
                mask = 0x1FC00000; /* SEGI,A27..A22 */
                break;
            case 0x11:
                ErrorF("Local Bus option 1b.\n");
                bustype = BUS_VLB;
                mask = 0x00C00000; /* SEGI,A22 */
                break;
            case 0x08:
            case 0x0B:
            default:
                ErrorF("Local Bus option 2.\n");
                bustype = BUS_VLB;
                mask = 0x3FC00000; /* A29..A22 */
                break;
        }
        if ( (et4000_type >= TYPE_ET4000W32Pc) && (mask = 0x3FC00000) )
           mask |= 0xC0000000; /* A31,A30 decoded from PCI config space */
        break;
    case TYPE_ET6000:
        mask = 0xFF000000;
        break;
  }

  if (vga256InfoRec.MemBase != 0)   /* MemBase given from XF86Config */
  {
    /* check for possible errors in given linear base address */
    if ((vga256InfoRec.MemBase & (~mask)) != 0) {
        ErrorF("%s %s: MemBase out of range. Must be <= 0x%x on 0x%x boundary.\n",
               XCONFIG_PROBED, vga256InfoRec.name, mask, ~(mask | 0xFF000000) + 1);
        vga256InfoRec.MemBase &= ~mask;
    }
  }
  else     /* MemBase not given: find it */
  {
    switch(et4000_type)
    {
      case TYPE_ET4000W32I:
      case TYPE_ET4000W32Ib:
      case TYPE_ET4000W32Ic:
        vga256InfoRec.MemBase = DEFAULT_LIN_MEMBASE;
        break;
      case TYPE_ET4000W32P:  /* A31,A30 are decoded as 00 (=always mapped below 512 MB) */
      case TYPE_ET4000W32Pa:
      case TYPE_ET4000W32Pb:
        if (tseng_pcr) vga256InfoRec.MemBase = tseng_pcr->_base0;
        else vga256InfoRec.MemBase = DEFAULT_LIN_MEMBASE;
        if (vga256InfoRec.MemBase > mask)           /* ... except if we can't decode that much */
          vga256InfoRec.MemBase = mask - 4*1024*1024; /* top of decodable memory */
        break;
      case TYPE_ET4000W32Pc: /* A31,A30 decoded from PCI config space, but in PCI mode only */
      case TYPE_ET4000W32Pd:
        if (tseng_pcr) vga256InfoRec.MemBase = tseng_pcr->_base0;
        else vga256InfoRec.MemBase = 0xC0000000 | DEFAULT_LIN_MEMBASE_PCI;
        if (vga256InfoRec.MemBase > mask)           /* ... except if we can't decode that much */
          vga256InfoRec.MemBase = mask - 4*1024*1024; /* top of decodable memory */
        break;
      case TYPE_ET6000:
        if (tseng_pcr && autodetect) /* don't trust PCI when not autodetecting */
          vga256InfoRec.MemBase = tseng_pcr->_base0;
        else if (inb(ET6Kbase+0x13) != 0)
        {
          vga256InfoRec.MemBase = inb(ET6Kbase+0x13) << 24;
          ErrorF("%s %s: ET6000: port-probed linear memory base = 0x%x\n",
                  XCONFIG_PROBED, vga256InfoRec.name, vga256InfoRec.MemBase);
        }
        else vga256InfoRec.MemBase = 0xF0000000; /* map memory near top of memory by default */
        break;
    }
    vga256InfoRec.MemBase &= mask;
  }

  /* One final check for a valid MemBase */
  if (vga256InfoRec.MemBase < 4096 * 1024) {
      FatalError("%s %s: Invalid MemBase: please define a non-zero MemBase in XF86Config. "
                 "See README.tseng or tseng.sgml for more information.",
                 XCONFIG_PROBED, vga256InfoRec.name);
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

   ET4000EnterLeave(ENTER);

   if (autodetect && tseng_pcr)
   {
     ET6Kbase = tseng_pcr->_base1 & ~0xFF;
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

   /* define used IO ports... Is this really necessary for PCI config space IO? */
   for (i=0; i<Num_ET6000_PCIPorts; i++)
     ET6000_PCIPorts[i] = ET6Kbase+i;
   xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_ET6000_PCIPorts, ET6000_PCIPorts);
   
  /*
   * clock related stuff
   */
  
  ClockSelect = Tseng_ET6000ClockSelect;

  outb(ET6Kbase+0x67, 0x0f);
  ErrorF("%s %s: ET6000: CLKDAC ID: 0x%X\n",
      XCONFIG_PROBED, vga256InfoRec.name, inb(ET6Kbase+0x69));

#ifdef DO_WE_NEED_THIS
  vga256InfoRec.clocks = 3;
  vga256InfoRec.clock[0] = 25175;
  vga256InfoRec.clock[1] = 28322;
  vga256InfoRec.clock[2] = 31500; /* this one will be reprogrammed */
#endif

  /* avoid autoprobing for what we already know */
  vga256InfoRec.ramdac = xf86TokenToString(TsengDacTable, ET6000_DAC);

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
          et4000_type = TYPE_ET4000W32I;
          break;
      case 3 : /* ET4000/W32i rev b */ 
          et4000_type = TYPE_ET4000W32Ib;
          break;
      case 11: /* ET4000/W32i rev c */
          et4000_type = TYPE_ET4000W32Ic;
          break;
      case 2 : /* ET4000/W32p rev a */
          et4000_type = TYPE_ET4000W32Pa;
          break;
      case 5 : /* ET4000/W32p rev b */
          et4000_type = TYPE_ET4000W32Pb;
          break;
      case 6 : /* ET4000/W32p rev d */
          et4000_type = TYPE_ET4000W32Pd;
          break;
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
 * The 8*32kb ET6000 MDRAM granularity causes the more general probe to
 * detect too much memory in some configurations, because that code has a
 * 8-bank (=256k) granularity. E.g. it fails to recognize 2.25 MB of memory
 * (detects 2.5 instead). This function goes to check if the RAM is actually
 * there. MDRAM comes in multiples of 4 banks (16, 24, 32, 36, 40, 64, 72,
 * 80, ...), so checking each 64k block should be enough granularity.
 *
 * The exact same code could be used on other Tseng chips, or even on ANY
 * VGA board, but probably only in case of trouble.
 *
 */
#define VIDMEM ((volatile unsigned int*)check_vgabase)
#define SEGSIZE (ET4000.ChipSegmentSize / 1024)

/* vgaSetVidPage() doesn't seem to work -- dunno why */
static void Tseng_set_segment(int seg)
{
  int seg1, seg2;
  seg1 = seg & 0x0F;
  seg2 = (seg & 0x30) >> 4;
  outb(0x3CB, seg2 | (seg2 << 4));
  outb(0x3CD, seg1 | (seg1 << 4));
}

static int et6000_check_videoram(int ram)
{
  unsigned char oldSegSel1, oldSegSel2, oldGR5, oldGR6, oldSEQ2, oldSEQ4;
  int segment, i;
  int real_ram;
  pointer check_vgabase;
  Bool fooled = FALSE;
  int save_vidmem;
  
  if (ram > 4096) {
    ErrorF("%s %s: ET6000: Detected more than 4096 kb of video RAM. Clipped to 4096kb\n",
            XCONFIG_PROBED, vga256InfoRec.name);
    ram = 4096;
  }
  
 /* vga256InfoRec.VGAbase is NULL at this point, because the VGA aperture
  * hasn't been memory-mapped yet when we enter the probing code.
  */
  check_vgabase = xf86MapVidMem(vga256InfoRec.scrnIndex, VGA_REGION,
                           (pointer)0xA0000, 0x10000);

 /*
  * We need to set the VGA controller in VGA graphics mode, or else we won't
  * be able to access the full 4MB memory range. First, we save the
  * registers we modify, of course.
  */
  oldSegSel1 = inb(0x3CD);
  oldSegSel2 = inb(0x3CB);
  outb(0x3CE, 5); oldGR5 = inb(0x3CF);
  outb(0x3CE, 6); oldGR6 = inb(0x3CF);
  outb(0x3C4, 2); oldSEQ2 = inb(0x3C5);
  outb(0x3C4, 4); oldSEQ4 = inb(0x3C5);

  /* set graphics mode */  
  outb(0x3CE, 6); outb(0x3CF, 5);
  outb(0x3CE, 5); outb(0x3CF, 0x40);
  outb(0x3C4, 2); outb(0x3C5, 0x0f);
  outb(0x3C4, 4); outb(0x3C5, 0x0e);

 /*
  * count down from presumed amount of memory in SEGSIZE steps, and
  * look at each segment for real RAM.
  */

  for (segment = (ram / SEGSIZE) - 1; segment >= 0; segment--)
  {
      Tseng_set_segment(segment);

      /* save contents of memory probing location */
      save_vidmem = *(VIDMEM);

      /* test with pattern */
      *VIDMEM = 0xAAAA5555;
      if (*VIDMEM != 0xAAAA5555) {
          *VIDMEM = save_vidmem;
          continue;
      }
      
      /* test with inverted pattern */
      *VIDMEM = 0x5555AAAA;
      if (*VIDMEM != 0x5555AAAA) {
          *VIDMEM = save_vidmem;
          continue;
      }
      
      /* check if we aren't fooled by address wrapping (mirroring) */
      fooled = FALSE;
      for (i = segment-1; i >=0; i--) {
          Tseng_set_segment(i);
          if (*VIDMEM == 0x5555AAAA) {
               fooled = TRUE;
               break;
          }
      }
      if (!fooled) {
          real_ram = (segment+1) * SEGSIZE;
          break;
      }
      /* restore old contents again */
      *VIDMEM = save_vidmem;
  }

  /* restore original register contents */  
  outb(0x3CD, oldSegSel1);
  outb(0x3CB, oldSegSel2);
  outb(0x3CE, 5); outb(0x3CF, oldGR5);
  outb(0x3CE, 6); outb(0x3CF, oldGR6);
  outb(0x3C4, 2); outb(0x3C5, oldSEQ2);
  outb(0x3C4, 4); outb(0x3C5, oldSEQ4);

  xf86UnMapVidMem(vga256InfoRec.scrnIndex, VGA_REGION,
                (pointer)0xA0000, 0x10000);

  return real_ram;
}

/*
 * TsengDetectMem --
 *      try to find amount of video memory installed.
 */

static void
TsengDetectMem()
{
  unsigned char config;
  int ramtype=0;
  
  if (et4000_type >= TYPE_ET6000)
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
        /* 8*32kb MDRAM refresh control granularity in the ET6000 fails to */
        /* recognize 2.25 MB of memory (detects 2.5 instead) */
        vga256InfoRec.videoRam = et6000_check_videoram(vga256InfoRec.videoRam);
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
 
  /* Try to detect a Tseng video card -- first see if it is forced */
  if (vga256InfoRec.chipset)   /* no auto-detect: chipset is given */
  {
    et4000_type = xf86StringToToken(chipsets, vga256InfoRec.chipset);
    if (et4000_type < 0)
        return FALSE;
    autodetect = FALSE;
  }

  /* next step: try finding one on the PCI bus */
  if ((et4000_type == TYPE_UNKNOWN) && vgaPCIInfo)
  {
    int i = 0;
    /* find an active TSENG card in the list of PCI devices */
    if (vgaPCIInfo->AllCards) {
      while (tseng_pcr = vgaPCIInfo->AllCards[i++]) {
            if ((tseng_pcr->_vendor == PCI_VENDOR_TSENG)
                && (tseng_pcr->_command & PCI_CMD_IO_ENABLE)
                && (tseng_pcr->_command & PCI_CMD_MEM_ENABLE)) {
                  /*
                   * At this moment, "tseng_pcr" holds a pointer to the PCI
                   * structure of the active TSENG VGA card.
                   *
                   * This "searching" should not be necessary anymore once
                   * the XFree PCI code gives us only the ACTIVE PCI VGA
                   * card to begin with.
                   */
                  switch(tseng_pcr->_device)
                    {
                      case PCI_CHIP_ET6000:
                        et4000_type = TYPE_ET6000;
                        break;
                      case PCI_CHIP_ET4000_W32P_A:
                        et4000_type = TYPE_ET4000W32Pa;
                        break;
                      case PCI_CHIP_ET4000_W32P_B:
                        et4000_type = TYPE_ET4000W32Pb;
                        break;
                      case PCI_CHIP_ET4000_W32P_C:
                        et4000_type = TYPE_ET4000W32Pc;
                        break;
                      case PCI_CHIP_ET4000_W32P_D:
                        et4000_type = TYPE_ET4000W32Pd;
                        break;
                      default: 
                        ErrorF("%s %s: Unknown Tseng Labs PCI device 0x%x -- please report.\n",
                               XCONFIG_PROBED, vga256InfoRec.name, tseng_pcr->_device);
                    }
                  vga256InfoRec.chipset = xf86TokenToString(chipsets, et4000_type);      
                  break;
            }
      }
    }
  }

  /* last resort -- try old-style autodetect (port probing) */
  if (et4000_type == TYPE_UNKNOWN) {
     if (ET4000AutoDetect()==FALSE) return(FALSE);
  }

  if (et4000_type >= TYPE_ET6000)
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

 /*
  * Check for RAMDAC type
  */
  Check_Tseng_Ramdac();
  tseng_init_clockscale(vgaBitsPerPixel/8);
  tseng_set_dacspeed(vgaBitsPerPixel/8);

#ifdef USE_XAA
  /*
   * Linear mode and >8bpp mode handling.
   *
   * Defaults: banked addressing in 8bpp modes,
   *           linear addressing in >8bpp modes.
   *
   * unless of course it is overridden from XF86Config.
   */

  if (CHIP_SUPPORTS_LINEAR) {
    /* enable using option "linear" in XF86Config */
    OFLG_SET(OPTION_LINEAR, &ET4000.ChipOptionFlags);
  }
  else {
    /* clear the option flag if it isn't supported */
    OFLG_CLR(OPTION_LINEAR, &vga256InfoRec.options);
  }
  
  if (vgaBitsPerPixel > 8) {
    if (!CHIP_SUPPORTS_LINEAR) {
      FatalError("%s %s: A color depth of %d bits per pixel is not supported on this device.\n",
                 XCONFIG_PROBED, vga256InfoRec.name, vgaBitsPerPixel);
    }
    /* OK, so we can do > 8bpp. Force linear mode -- can't do banked at >8bpp */
    OFLG_SET(OPTION_LINEAR, &vga256InfoRec.options);
  }

  if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options)) {
    if (!ET4000LinMem(autodetect))
      FatalError("%s %s: Linear memory mode not supported on this device.\n",
                 XCONFIG_PROBED, vga256InfoRec.name);
  }

  /* what color depths can we handle? */
  switch(TsengRamdacType) {
    case ET6000_DAC:
    case ICS5341_DAC:
       ET4000.ChipHas16bpp = TRUE;
       ET4000.ChipHas24bpp = TRUE;
       ET4000.ChipHas32bpp = TRUE;
       break;
    case STG1703_DAC:
    case STG1702_DAC:
    case ATT20C490_DAC:
    case ATT20C491_DAC:
    case ATT20C492_DAC:
    case ATT20C493_DAC:
       ET4000.ChipHas16bpp = TRUE;
       ET4000.ChipHas24bpp = TRUE;
       break;
    case CH8398_DAC:  /* CH8398 seems to have trouble with "hibit" (MCLK/2) clocks at 24bpp */
    case STG1700_DAC: /* STG1700 can't do packed 24bpp over a 16-bit bus */
       ET4000.ChipHas16bpp = TRUE;
       break;
  }

 /*
  * Acceleration is currently only supported on W32p or newer chips. W32i
  * doesn't work yet.
  */  

  if (et4000_type < TYPE_ET4000W32P)
  {
    /* this makes life easier in the rest of the server code */
    OFLG_SET(OPTION_NOACCEL, &vga256InfoRec.options);
  }

 /*
  * Acceleration combined with linear mode doesn't work yet on chips before
  * W32p rev c.
  */  

  if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)
      && OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))
  {
    if (et4000_type < TYPE_ET4000W32Pc)
    {
      ErrorF("%s %s: Acceleration disabled (not yet supported in linear mode).\n",
             XCONFIG_PROBED, vga256InfoRec.name);
      OFLG_SET(OPTION_NOACCEL, &vga256InfoRec.options);
    }
  }

 /*
  * Some combinations can't use all available memory.
  */  

#define TSENG_MEMLIMIT(m,reason) \
    { \
      if (vga256InfoRec.videoRam > (m)) \
      { \
        ErrorF("%s %s: Only %d kb of memory can be used in %s.\n", \
              XCONFIG_PROBED, vga256InfoRec.name, (m), (reason)); \
        vga256InfoRec.videoRam = (m); \
      } \
    }

  /* This is from experience, and not from the data book. Should get fixed some day */
  if ( (et4000_type >= TYPE_ET4000W32P) && (et4000_type < TYPE_ET4000W32Pc)
      && OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options) )
  {
      TSENG_MEMLIMIT(1024, "linear memory mode on W32p rev a & b");
  }

  /* more differentiation could be used later, but it'll do for now */
  if ( (et4000_type < TYPE_ET6000)
      && OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options)
      && !OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options) )
  {
      TSENG_MEMLIMIT(2048, "accelerated mode with linear memory mapping on any W32");
  }

#ifdef KMG_FORGOT_WHY_THIS_WAS
  if ( (et4000_type < TYPE_ET6000)
      && !OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options)
      && !OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options) )
  {
      TSENG_MEMLIMIT(4096-516, "banked accelerated mode on any W32");
  }
#endif

  if ( !OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)
      && OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options) )
  {
      TSENG_MEMLIMIT(4096-8, "accelerated mode with linear memory mapping on any Tseng card");
  }

  TSENG_MEMLIMIT(4096, "Tseng cards");

 /*
  * Acceleration requires 1kb scratch buffer memory.
  */  

  if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
  {
    ErrorF("%s %s: Reserving 1kb of video memory for accelerator.\n",
           XCONFIG_PROBED, vga256InfoRec.name);
    vga256InfoRec.videoRam -= 1;
    scratchVidBase = vga256InfoRec.videoRam * 1024;
  }

      /* Hardware Cursor support */
#ifdef W32_HW_CURSOR_FIXED
  if (et4000_type >= TYPE_ET4000W32P)
#else
  if (et4000_type >= TYPE_ET6000)
#endif
  {
          /* Set HW Cursor option valid */
      OFLG_SET(OPTION_HW_CURSOR, &ET4000.ChipOptionFlags);
  }

  if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options))
  {
#ifdef W32_HW_CURSOR_FIXED
      if (et4000_type >= TYPE_ET4000W32P)
#else
      if (et4000_type >= TYPE_ET6000)
#endif
      {
          ErrorF("%s %s: Reserving 1kb of video memory for hardware cursor.\n",
                 XCONFIG_PROBED, vga256InfoRec.name);
          vga256InfoRec.videoRam -= 1; /* 1kb reserved for hardware cursor */
          tsengCursorAddress = vga256InfoRec.videoRam * 1024;
      }
      else   /* clear illegal option */
          OFLG_CLR(OPTION_HW_CURSOR, &vga256InfoRec.options);
  }
        
  /* Set PCI_RETRY option valid */
  OFLG_SET(OPTION_PCI_RETRY, &ET4000.ChipOptionFlags);

  OFLG_SET(OPTION_NOACCEL, &ET4000.ChipOptionFlags);

#endif

#ifdef MONOVGA
    OFLG_CLR(OPTION_HW_CURSOR, &vga256InfoRec.options);
    OFLG_CLR(OPTION_LINEAR, &vga256InfoRec.options);
    OFLG_SET(OPTION_NOACCEL, &vga256InfoRec.options);
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

#ifdef W32_SUPPORT
    if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
    {
      if (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions))
      {
        ClockSelect = Tseng_ICS5341ClockSelect;
        numClocks = 3;
      }
      else if (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions))
      {
        ClockSelect = Tseng_STG1703ClockSelect;
        numClocks = 3;
      }
      else if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &vga256InfoRec.clockOptions))
      {
        ClockSelect = Tseng_ICD2061AClockSelect;
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
          ClockSelect = Tseng_LegendClockSelect;
          numClocks   = 32;
        }
      else
        {
          ClockSelect = Tseng_ET4000ClockSelect;
          /*
           * The CH8398 RAMDAC uses CS3 for register selection (RS2), not for clock selection.
           * The ICS5341 RAMDAC only has 8 clocks. Together with MCLK/2, that's 16 clocks.
           */
          if ( ( et4000_type > TYPE_ET4000 ) && (TsengRamdacType != CH8398_DAC)
             && (TsengRamdacType != ICS5341_DAC) )
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
        tseng_save_divide = 0x40;
      }
    else if (OFLG_ISSET(OPTION_HIBIT_LOW, &vga256InfoRec.options))
      tseng_save_divide = 0;
    else
      {
        /* Check for initial state of divide flag */
        outb(0x3C4, 7);
        tseng_save_divide = inb(0x3C5) & 0x40;
        ErrorF("%s %s: ET4000: Initial hibit state: %s\n", XCONFIG_PROBED,
               vga256InfoRec.name, tseng_save_divide & 0x40 ? "high" : "low");
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
#endif
    if (et4000_type >= TYPE_ET6000)
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

  if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) {
    if (!vga256InfoRec.clocks) vgaGetClocks(numClocks, ClockSelect);
  }
  
  vga256InfoRec.bankedMono = TRUE;
#ifndef MONOVGA
#ifdef XFreeXDGA
  vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

#ifdef DPMSExtension
  /* Support for DPMS, the ET4000W32Pc and newer uses a different and
   * simpler method than the older cards.
   */
  if (et4000_type >= TYPE_ET4000W32Pc) {
    vga256InfoRec.DPMSSet = TsengCrtcDPMSSet;
  } else {
    vga256InfoRec.DPMSSet = TsengHVSyncDPMSSet;
  }
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


  if (!ET4000.ChipUseLinearAddressing)
  {
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
  }

      /* Hardware cursor setup */
  if (OFLG_ISSET(OPTION_HW_CURSOR, &vga256InfoRec.options)) {
      tsengCursorWidth = 64;
      tsengCursorHeight = 64;
      vgaHWCursor.Initialized = TRUE;
      vgaHWCursor.Init = TsengCursorInit;
      vgaHWCursor.Restore = TsengRestoreCursor;
      vgaHWCursor.Warp = TsengWarpCursor;
      vgaHWCursor.QueryBestSize = TsengQueryBestSize;
      if (xf86Verbose)
          ErrorF("%s %s: %s: Using hardware cursor\n",
                 XCONFIG_PROBED, vga256InfoRec.name,
                 vga256InfoRec.chipset);             
  }

  if (OFLG_ISSET(OPTION_PCI_RETRY, &vga256InfoRec.options))
    {
      ErrorF("%s %s: Using PCI retrys.\n",XCONFIG_PROBED, vga256InfoRec.name);
      Use_Pci_Retry = 1;
    }

#ifdef USE_XAA
  if (vgaBitsPerPixel >= 8) {
  if (OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
    {
      Use_ACL = FALSE;
    }
    else
    {
      /* initialize the XAA interface software */
      /* TsengAccelInit();
         This relies on variables that are setup later, so it's called there */ 
      Use_ACL = TRUE;
    }
  }
#endif
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
  if (vga256InfoRec.directMode&XF86DGADirectGraphics && !enter) {
    if (vgaHWCursor.Initialized == TRUE)
      TsengHideCursor();
    return;
  }
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

#ifdef W32_SUPPORT
  if (TsengRamdacType == ICS5341_DAC)
  {
     /* Restore ICS 5341 GenDAC Command and PLL registers */
     outb(vgaIOBase + 4, 0x31);
     i = inb(vgaIOBase + 5);
     outb(vgaIOBase + 5, i | 0x40);

     outb(0x3c6, restore->gendac.cmd_reg);        /* Enhanced command register*/

     if ( (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) &&
          (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions)) ) {
       outb(0x3c8, 2);                            /* index to f2 reg */
       outb(0x3c9, restore->gendac.PLL_f2_M);     /* f2 PLL M divider */
       outb(0x3c9, restore->gendac.PLL_f2_N);     /* f2 PLL N1/N2 divider */

       outb(0x3c8, 0x0e);                         /* index to PLL control */
       outb(0x3c9, restore->gendac.PLL_ctrl);     /* PLL control */
       outb(0x3c8, restore->gendac.PLL_w_idx);    /* PLL write index */
       outb(0x3c7, restore->gendac.PLL_r_idx);    /* PLL read index */
     }
     outb(vgaIOBase + 4, 0x31);
     outb(vgaIOBase + 5, i & ~0x40);
  }

  if ( (TsengRamdacType == STG1702_DAC) || (TsengRamdacType == STG1703_DAC)
      || (TsengRamdacType == STG1700_DAC) )
  {
     /* Restore STG 170x GenDAC Command and PLL registers 
      * we share one data structure with the gendac code, so the names
      * are not too good.
      */

     if ( (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) &&
          (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions)) ) {
       STG1703setIndex(0x24,restore->gendac.PLL_f2_M);
       outb(0x3c6,restore->gendac.PLL_f2_N);      /* use autoincrement */
     }
     STG1703setIndex(0x03,restore->gendac.PLL_ctrl);/* write same value to */
     outb(0x3c6,restore->gendac.PLL_ctrl);	  /* primary and secondary 
       						   * pixel mode select register */
     STG1703magic(0);
     xf86dactopel();
     xf86setdaccomm(restore->gendac.cmd_reg);   /* write enh command reg */
  }

  if (TsengRamdacType == CH8398_DAC) {
    xf86dactopel();
    xf86setdaccomm(restore->gendac.cmd_reg);
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
  
  if (DAC_IS_ATT49x) xf86setdaccomm(restore->ATTdac_cmd);

  if (et4000_type >= TYPE_ET6000)
  {
    outb(ET6Kbase+0x13, restore->ET6KMemBase);
    outb(ET6Kbase+0x40, restore->ET6KMMAPCtrl);
    outb(ET6Kbase+0x58, restore->ET6KVidCtrl1);
    outb(ET6Kbase+0x41, restore->ET6KPerfContr);
    outb(ET6Kbase+0x46, restore->ET6KDispFeat);
  }
  
  outw(vgaIOBase + 4, (restore->HorOverflow << 8)  | 0x3F);
  outw(vgaIOBase + 4, (restore->SegMapComp << 8)  | 0x30);
  outw(vgaIOBase + 4, (restore->GenPurp << 8)  | 0x31);
 
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
  if (Use_ACL & (restore->std.Attribute[16] & 1)) /* are we going to graphics mode? */
    tseng_init_acl(); /* initialize acceleration hardware */
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
  unsigned char             temp, saveseg1=0, saveseg2=0;

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
  temp = inb(vgaIOBase + 0x0A); /* reset flip-flop */
  outb(0x3C0,0x36); save->Misc = inb(0x3C1); outb(0x3C0, save->Misc);
#ifdef W32_SUPPORT
  if (TsengRamdacType == ICS5341_DAC)
  {
     /* Save ICS 5341 GenDAC Command and PLL registers */
     outb(vgaIOBase + 4, 0x31);
     temp = inb(vgaIOBase + 5);
     outb(vgaIOBase + 5, temp | 0x40);

     save->gendac.cmd_reg = inb(0x3c6);      /* Enhanced command register */
     if ( (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) &&
          (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions)) ) {
       save->gendac.PLL_w_idx = inb(0x3c8);    /* PLL write index */
       save->gendac.PLL_r_idx = inb(0x3c7);    /* PLL read index */
       outb(0x3c7, 2);                         /* index to f2 reg */
       save->gendac.PLL_f2_M = inb(0x3c9);     /* f2 PLL M divider */
       save->gendac.PLL_f2_N = inb(0x3c9);     /* f2 PLL N1/N2 divider */
       outb(0x3c7, 0x0e);                      /* index to PLL control */
       save->gendac.PLL_ctrl = inb(0x3c9);     /* PLL control */
      }
     outb(vgaIOBase + 4, 0x31);
     outb(vgaIOBase + 5, temp & ~0x40);
  }
  
  if ( (TsengRamdacType == STG1702_DAC) || (TsengRamdacType == STG1703_DAC) 
      || (TsengRamdacType == STG1700_DAC) )
  {
     /* Restore STG 1703 GenDAC Command and PLL registers 
      * unfortunately we reuse the gendac data structure, so the 
      * field names are not really good.
      */

     xf86dactopel();
     save->gendac.cmd_reg = xf86getdaccomm();/* Enhanced command register */
     if ( (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) &&
          (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions)) ) {
       save->gendac.PLL_f2_M = STG1703getIndex(0x24); /* f2 PLL M divider */
       save->gendac.PLL_f2_N = inb(0x3c6);            /* f2 PLL N1/N2 divider */
     }
     save->gendac.PLL_ctrl = STG1703getIndex(0x03);   /* pixel mode select control */
  }

  if (TsengRamdacType == CH8398_DAC) {
    xf86dactopel();
    save->gendac.cmd_reg = xf86getdaccomm();
  }
#endif

  if ( (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions)) &&
       (OFLG_ISSET(CLOCK_OPTION_ET6000, &vga256InfoRec.clockOptions)) ) {
     /* Save ET6000 CLKDAC PLL registers */
     temp = inb(ET6Kbase+0x67); /* remember old CLKDAC index register pointer */
     outb(ET6Kbase+0x67, 2);
     save->gendac.PLL_f2_M = inb(ET6Kbase+0x69);
     save->gendac.PLL_f2_N = inb(ET6Kbase+0x69);
     /* save MClk values */
     outb(ET6Kbase+0x67, 10);
     save->ET6KMclkM = inb(ET6Kbase+0x69);
     save->ET6KMclkN = inb(ET6Kbase+0x69);
     /* restore old index register */
     outb(ET6Kbase+0x67, temp);
  }

  if (DAC_IS_ATT49x) save->ATTdac_cmd = xf86getdaccomm();

  if (et4000_type >= TYPE_ET6000)
  {
    save->ET6KMemBase   = inb(ET6Kbase+0x13);
    save->ET6KMMAPCtrl  = inb(ET6Kbase+0x40);
    save->ET6KVidCtrl1  = inb(ET6Kbase+0x58);
    save->ET6KPerfContr = inb(ET6Kbase+0x41);
    save->ET6KDispFeat  = inb(ET6Kbase+0x46);
  }
  
  outb(vgaIOBase + 4, 0x30); save->SegMapComp = inb(vgaIOBase + 5);
  outb(vgaIOBase + 4, 0x31); save->GenPurp = inb(vgaIOBase + 5);
  outb(vgaIOBase + 4, 0x3F); save->HorOverflow = inb(vgaIOBase + 5);

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
  int BytesPerPix = vgaBitsPerPixel>>3;

#ifdef W32_SUPPORT

  /*
   * this is a kludge, but the ET4000Validate() code should already have
   * done this, and it _has_ (supposing we call tseng_validate_mode in
   * there. I tried that, with no effect). But we seem to be getting a
   * different mode structure (a copy?) at this point.
   *
   * Another weirdness is that _here_, mode->Clock points to the clcok
   * _index_ in the array off clocks (usually 2 for a programmable clock),
   * while in ET4000Validate() mode->Clock is the actual pixel clock (in
   * kHZ).
   */
  tseng_validate_mode(mode, BytesPerPix, TRUE);

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
     if ((vga256InfoRec.clock[mode->Clock] * BytesPerPix) > 80000)
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

#ifdef W32_SUPPORT
    if (    (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &vga256InfoRec.clockOptions))
         && (    (OFLG_ISSET(CLOCK_OPTION_STG1703, &vga256InfoRec.clockOptions)))
              || (OFLG_ISSET(CLOCK_OPTION_ICS5341, &vga256InfoRec.clockOptions)) )
    { 
      /* for pixmux: must use post-div of 4 on ICS GenDAC clock generator!
       */

       if (mode->Flags & V_PIXMUX)
       {
         commonCalcClock(mode->SynthClock,1,1,31,2,3,100000,vga256InfoRec.dacSpeeds[0]*2+1, 
         		 &(new->gendac.PLL_f2_M), &(new->gendac.PLL_f2_N));
       }
       else
       {
         commonCalcClock(mode->SynthClock,1,1,31,0,3,100000,vga256InfoRec.dacSpeeds[0]*2+1, 
         		 &(new->gendac.PLL_f2_M), &(new->gendac.PLL_f2_N));
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
       Tseng_ICD2061AClockSelect(mode->SynthClock);
    }
    else
#endif

    if (et4000_type >= TYPE_ET6000)
    {
       /* setting min_n2 to "1" will ensure a more stable clock ("0" is allowed though) */
       commonCalcClock(vga256InfoRec.clock[new->std.NoClock],1,1,31,1,3,
                 100000,vga256InfoRec.dacSpeeds[0]*2, 
       		 &(new->gendac.PLL_f2_M), &(new->gendac.PLL_f2_N));

       /* ErrorF("M=0x%x ; N=0x%x\n",new->gendac.PLL_f2_M, new->gendac.PLL_f2_N);*/
       /* above 130MB/sec, we enable the "LOW FIFO threshold" */
       if (vga256InfoRec.clock[new->std.NoClock] * BytesPerPix > 130000)
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
       /* Tseng_ET6000ClockSelect(vga256InfoRec.clock[new->std.NoClock]); */
    }
    else
    if (new->std.NoClock >= 0)
    {
      /* CS2 */
      new->Compatibility = (new->Compatibility & 0xFD) | 
      				((new->std.NoClock & 0x04) >> 1);
#ifndef OLD_CLOCK_SCHEME
      /* clock select bit 3 = divide-by-2 disable/enable */
      new->AuxillaryMode = (tseng_save_divide ^ ((new->std.NoClock & 0x08) << 3)) |
                           (new->AuxillaryMode & 0xBF);
      /* clock select bit 4 = CS3 */
      new->GenPurp = ((new->std.NoClock & 0x10) << 2) | (new->GenPurp & 0x3F);
#else
      new->AuxillaryMode = (tseng_save_divide ^ ((new->std.NoClock & 0x10) << 2)) |
                           (new->AuxillaryMode & 0xBF);
#endif
    }
  
  /*
   * linear mode handling
   */

   if (et4000_type >= TYPE_ET6000)
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
         if ( (et4000_type < TYPE_ET4000W32P) && (bustype == BUS_VLB) ) {  /* W32i */
           new->SegMapComp = new->SegMapComp ^ 0x1c; /* invert A26..A24 */
         }
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
   * 16/24/32 bpp handling.
   */

   if (vgaBitsPerPixel>=8) {
     tseng_set_ramdac_bpp(mode, new, BytesPerPix);
     row_offset *= BytesPerPix;
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

  /*
   * Enable memory mapped IO registers when acceleration is needed.
   */

#ifdef USE_XAA
  if (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))
  {
    if (et4000_type >= TYPE_ET6000)
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
  int BytesPerPix = vgaBitsPerPixel>>3;
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
 * partially reset to "0" when TS register index 0 bit 1 is set (synchronous
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
ET4000ValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
/*    tseng_validate_mode(mode, verbose);*/
    return MODE_OK;
}


#ifdef DPMSExtension
/*
 * TsengCrtcDPMSSet --
 *
 * Sets VESA Display Power Management Signaling (DPMS) Mode.
 * This routine is for the ET4000W32P rev. c and later, which can
 * use CRTC indexed register 34 to turn off H/V Sync signals.
 */
static void TsengCrtcDPMSSet(Mode)
     CARD16 Mode;
{
  unsigned char seq1, crtc34;
  if (!xf86VTSema) return;
  switch (Mode)
    {
    case DPMSModeOn:
      /* Screen: On; HSync: On, VSync: On */
      seq1 = 0x00;
      crtc34 = 0x00;
      break;
    case DPMSModeStandby:
      /* Screen: Off; HSync: Off, VSync: On */
      seq1 = 0x20;
      crtc34 = 0x01;
      break;
    case DPMSModeSuspend:
      /* Screen: Off; HSync: On, VSync: Off */
      seq1 = 0x20;
      crtc34 = 0x20;
      break;
    case DPMSModeOff:
      /* Screen: Off; HSync: Off, VSync: Off */
      seq1 = 0x20;
      crtc34 = 0x21;
      break;
    }
  outb(0x3C4, 0x01);	/* Select SEQ1 */
  seq1 |= inb(0x3C5) & ~0x20;
  outb(0x3C5, seq1);
  outb(vgaIOBase+4, 0x34);	/* Select CRTC34 */
  crtc34 |= inb(vgaIOBase+5) & ~0x21;
  outb(vgaIOBase+5, crtc34);
}


/*
 * TsengHVSyncDPMSSet --
 *
 * Sets VESA Display Power Management Signaling (DPMS) Mode.
 * This routine is for Tseng et4000 chips that do not have any
 * registers to disable sync output.
 */
static void TsengHVSyncDPMSSet(Mode)
     CARD16 Mode;
{
  unsigned char seq1, tmpb;
  unsigned int HSync, VSync, HTot, VTot, tmp;
  Bool chgHSync, chgVSync;

  if (!xf86VTSema) return;

  /* Code here to read the current values of HSync through VTot:
   *  HSYNC:
   *    bits 0..7 : CRTC index 0x04
   *    bit 8     : CRTC index 0x3F, bit 4
   */
  outb(vgaIOBase+4, 0x04);
  HSync = inb(vgaIOBase+5);
  outb(vgaIOBase+4, 0x3F);
  HSync += (inb(vgaIOBase+5) & 0x10) << 4;
  /*  VSYNC:
   *    bits 0..7 : CRTC index 0x10
   *    bits 8..9 : CRTC index 0x07 bits 2 (VSYNC bit 8) and 7 (VSYNC bit 9)
   *    bit 10    : CRTC index 0x35 bit 3
   */
  outb(vgaIOBase+4, 0x10);
  VSync = inb(vgaIOBase+5);
  outb(vgaIOBase+4, 0x07);
  tmp = inb(vgaIOBase+5);
  VSync += (tmp & 0x04)<< 6 + (tmp & 0x80) << 2;
  outb(vgaIOBase+4, 0x35);
  VSync += (inb(vgaIOBase+5) & 0x08) << 7;
  /*  HTOT:
   *    bits 0..7 : CRTC index 0x00.
   *    bit 8     : CRTC index 0x3F, bit 0
   */
  outb(vgaIOBase+4, 0x00);
  HTot = inb(vgaIOBase+5);
  outb(vgaIOBase+4, 0x3F);
  HTot += (inb(vgaIOBase+5) & 0x01) << 8;
  /*  VTOT:
   *    bits 0..7 : CRTC index 0x06
   *    bits 8..9 : CRTC index 0x07 bits 0 (VTOT bit 8) and 5 (VTOT bit 9)
   *    bit 10    : CRTC index 0x35 bit 1
   */
  outb(vgaIOBase+4, 0x06);
  VTot = inb(vgaIOBase+5);
  outb(vgaIOBase+4, 0x07);
  tmp = inb(vgaIOBase+5);
  VTot += (tmp & 0x01)<< 8 + (tmp & 0x20) << 4;
  outb(vgaIOBase+4, 0x35);
  VTot += (inb(vgaIOBase+5) & 0x02) << 9;

  /* Don't write these unless we have to. */
  chgHSync = chgVSync = FALSE;

  switch (Mode)
    {
    case DPMSModeOn:
      /* Screen: On; HSync: On, VSync: On */
      seq1 = 0x00;
      if(HSync > HTot +3) { /* Sync is off now, turn it on. */
	HSync = (HTot - HSync) + HTot + 7;
	chgHSync = TRUE;
      }
      if(VSync > VTot +1) { /* Sync is off now, turn it on. */
	VSync = (VTot - VSync) + VTot + 4;
	chgVSync = TRUE;
      }
      break;
    case DPMSModeStandby:
      /* Screen: Off; HSync: Off, VSync: On */
      seq1 = 0x20;
      if(HSync <= HTot +3) { /* Sync is on now, turn it off. */
	HSync = (HTot - HSync) + HTot + 7;
	chgHSync = TRUE;
      }
      if(VSync > VTot +1) { /* Sync is off now, turn it on. */
	VSync = (VTot - VSync) + VTot + 4;
	chgVSync = TRUE;
      }
      break;
    case DPMSModeSuspend:
      /* Screen: Off; HSync: On, VSync: Off */
      seq1 = 0x20;
      if(HSync > HTot +3) { /* Sync is off now, turn it on. */
	HSync = (HTot - HSync) + HTot + 7;
	chgHSync = TRUE;
      }
      if(VSync <= VTot +1) { /* Sync is on now, turn it off. */
	VSync = (VTot - VSync) + VTot + 4;
	chgVSync = TRUE;
      }
      break;
    case DPMSModeOff:
      /* Screen: Off; HSync: Off, VSync: Off */
      seq1 = 0x20;
      if(HSync <= HTot +3) { /* Sync is on now, turn it off. */
	HSync = (HTot - HSync) + HTot + 7;
	chgHSync = TRUE;
      }
      if(VSync <= VTot +1) { /* Sync is on now, turn it off. */
	VSync = (VTot - VSync) + VTot + 4;
	chgVSync = TRUE;
      }
      break;
    }

  /* The code to turn on and off video output is equal for all. */
  outb(0x3C4, 0x01);	/* Select SEQ1 */
  seq1 |= inb(0x3C5) & ~0x20;
  outb(0x3C5, seq1);

  /* Then the code to write VSync and HSync to the card.
   *  HSYNC:
   *    bits 0..7 : CRTC index 0x04
   *    bit 8     : CRTC index 0x3F, bit 4
   */
  if(chgHSync) {
    outb(vgaIOBase+4, 0x04);
    tmpb = HSync & 0xFF;
    outb(vgaIOBase+5, tmpb);
    outb(vgaIOBase+4, 0x3F);
    tmpb = (HSync & 0x100) >>4;
    tmpb |= inb(vgaIOBase+5) & ~0x10;
    outb(vgaIOBase+5, tmpb);
  }
  /*  VSYNC:
   *    bits 0..7 : CRTC index 0x10
   *    bits 8..9 : CRTC index 0x07 bits 2 (VSYNC bit 8) and 7 (VSYNC bit 9)
   *    bit 10    : CRTC index 0x35 bit 3
   */
  if(chgVSync) {
    outb(vgaIOBase+4, 0x10);
    tmpb = VSync & 0xFF;
    outb(vgaIOBase+5, tmpb);
    outb(vgaIOBase+4, 0x07);
    tmpb = (VSync & 0x100) >> 6;
    tmpb |= (VSync & 0x200) >> 2;
    tmpb |= inb(vgaIOBase+5) & ~0x84;
    outb(vgaIOBase+5, tmpb);
    outb(vgaIOBase+4, 0x35);
    tmpb = (VSync & 0x400) >> 7;
    tmpb |= inb(vgaIOBase+5) & ~0x08;
    outb(vgaIOBase+5, tmpb);
  }
}
#endif
