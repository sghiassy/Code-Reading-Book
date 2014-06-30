/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/oak/oak_driver.c,v 3.28.2.2 1997/05/09 07:15:44 hohndel Exp $ */

/*
 * Copyright 1994 by Jorge Delgado <ernar@dit.upm.es>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Jorge Delgado not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission. Jorge Delgado makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 */
/*
 * JORGE DELGADO DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL JORGE DELGADO BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */
/* $XConsortium: oak_driver.c /main/16 1996/10/28 05:43:48 kaleb $ */

/* 
 * XF86_SVGA driver for Oak Technologies Inc. chipsets. 
 *
 * It includes support for the following chipsets:
 *  - oti067
 *  - oti077
 *  - oti087  (with some accelerated features)
 *  - oti037c (hopefully, I haven't tested it (disabled for now))
 */

/* Version information:
 *
 * 24/ 9/95 The driver was rewritten to clarify the code, facilitate 
 *          portability and upward compatibility and to try to fix some 
 *          little errors. Added option "undoc_clocks".
 *
 * 22/ 7/95 Added "fb_debug" option, to allow for register dump. This will help
 *          to debug some ISA boards which are reported to work awfully with
 *          high (72 & 78Mhz) dot clocks.
 *
 * 18/ 1/95 Found an undocumented way of setting linear addressing above
 *          16 Mbytes for OTI087 VLB cards, activating the 64Mbyte pin.
 *          Thanks should be given to the people that wrote the README.cirrus,
 *          it gave me the clue.
 *
 * 23/12/94 Linear addressing is cleaner, and more efficient.
 *          Membase has been set at 0xE00000 (14Mb) and
 *          aperture is selected matching video memory.
 *
 * 24/11/94 Managed to fix the linear mode that did not
 *          work in the previous release, also fixed a typo which
 *          made impossible to hardcode "oti077" as chipset.
 *
 * 24/10/94 New version by Jorge Delgado with support for
 *          2MB of DRAM in 087 chipsets with extended bank
 *          switching, ported to X11R6 structure.
 *
 * 10/ 9/94 Revamped 067/077 version with added support for 
 *          087 chipset by Jorge Delgado  ernar@dit.upm.es  
 *
 *  9/ 1/93 Initial version Steve Goldman  sgoldman@encore.com
 */

/* 
 * Other information:
 *
 * This one file can be used for both the color and monochrome servers.
 * Remember that the monochrome server is actually using a 16-color mode,
 * with only one bitplane active.  To distinguish between the two at
 * compile-time, use '#ifdef MONOVGA', etc. (But there is NO support for
 * monochrome in the 087, use the 077 driver harcoding chipset oti077
 * and VideoRam 1024 in XF86Config)
 */

/***********************************************************************
 **************   Beginning of the Oak driver code     *****************
 ***********************************************************************/

/* 
 * Header files
 */

#include "X.h"                 /* XF86 specific headers */
#include "input.h"
#include "screenint.h"
#include "dix.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#include "xf86_Config.h"    /* Header for extra options */
#include "xf86_Option.h"
#include "vga.h"
#include "region.h"

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
#include "oak_driver.h"
#include "vga256.h"
#endif

/*
 * Driver data structures.
 */

typedef struct {

  vgaHWRec std;                            /* Old VGA compatibility */

  unsigned char oakMisc;    	           /* 67 / 77 / 37C Boards  */
  unsigned char oakOverflow;    
  unsigned char oakHsync2;    	
  unsigned char oakOverflow2;   
  unsigned char oakConfig;    	
  unsigned char oakBCompat;    	
  unsigned char oakBank;    	

  unsigned char oti87RBank             ;   /* 87 Boards */
  unsigned char oti87WBank             ;
  unsigned char oti87VLBControl        ;
  unsigned char oti87Mapping           ;
  unsigned char oti87Clock             ; 
  unsigned char oti87BusControl        ;
  unsigned char oti87Hsync2            ; 
  unsigned char oti87XCrt              ;
  unsigned char oti87ColorPalette      ;
  unsigned char oti87Overflow          ;
  unsigned char oti87FIFODepth         ;
  unsigned char oti87ModeSelect        ; 
  unsigned char oti87Feature           ;
  unsigned char oti87ColorExpansion    ;
  unsigned char oti87FGExpansion       ;
  unsigned char oti87BGExpansion       ;
  unsigned char oti87ColorPattern      ;
  unsigned char oti87PixMask           ;
  unsigned char oti87HC1               ;
  unsigned char oti87HC2               ;
  unsigned char oti87HC3               ;
  unsigned char oti87HC4               ;
  unsigned char oti87HC5               ;
  unsigned char oti87HC6               ;
  unsigned char oti87HC7               ;
  unsigned char oti87HC8               ;
  unsigned char oti87HC9               ;
  unsigned char oti87HC10              ;
  unsigned char oti87HC11              ;
  unsigned char oti87HC12              ;

} vgaOAKRec, *vgaOAKPtr;

/* 
 * Forward definitions for the Driver's functions.
 */

static Bool     OAKProbe();
static char *   OAKIdent();
static Bool     OAKClockSelect();
static void     OAKEnterLeave();
static Bool     OAKInit();
static int      OAKValidMode();
static void *   OAKSave();
static void     OAKRestore();
static void     OAKAdjust();

#if 0
static void     OAKSaveScreen();
static void     OAKGetMode();
#endif


static void     OAKFbInit();


extern void     OAKSetRead();
extern void     OAKSetWrite();
extern void     OAKSetReadWrite();

extern void     OTI87SetRead();
extern void     OTI87SetWrite();
extern void     OTI87SetReadWrite();

/*
 * This data structure defines the driver itself.
 */

vgaVideoChipRec OAK = {
  OAKProbe,
  OAKIdent,
  OAKEnterLeave,
  OAKInit,
  OAKValidMode,
  OAKSave,
  OAKRestore,
  OAKAdjust,
  (void (*)())NoopDDA, /*  OAKSaveScreen,  */
  (void (*)())NoopDDA, /* OAKGetMode, */
  OAKFbInit,
  OAKSetRead,
  OAKSetWrite,
  OAKSetReadWrite,
  0x10000,	  
  0x10000,
  16,
  0xFFFF,
  0x00000, 0x10000,
  0x00000, 0x10000,
  TRUE,
  VGA_NO_DIVIDE_VERT,
  {0,},
  16,
  FALSE,             /* Initially no linear support, don't know if 87 */
  0,
  0,
  FALSE,
  FALSE,
  FALSE,
  NULL,
  1,            /* ClockMulFactor */
  1             /* ClockDivFactor */
};

/* 
 * Macro to dereference driver structure entries as  'new->xxxx'.
 */

#define new ((vgaOAKPtr)vgaNewVideoState)

/*
 * Oak Extended Register Names, 67/77/87/37c.
 */

#define OTI_INDEX 0x3DE			/* Oak extended index register */
#define OTI_R_W 0x3DF			/* Oak extended r/w register */
#define OTI_CRT_CNTL 0xC		/* Oak CRT COntrol Register */
#define OTI_MISC  0xD			/* Oak Misc register */
#define OTI_BCOMPAT  0xE		/* Oak Back compat register */
#define OTI_SEGMENT  0x11		/* Oak segment register */
#define OTI_CONFIG  0x12		/* Oak config register */
#define OTI_OVERFLOW  0x14		/* Oak overflow register */
#define OTI_HSYNC2  0x15		/* Oak hsync/2 start register */
#define OTI_OVERFLOW2  0x16		/* Oak overflow2 register */

/*
 * Oak Extended Register Names, 87 only.
 */


#define OTI87_IDENTIFY            0x0     /* r Identify register, should not be
					   *    used to identify the board   
					   */
#define OTI87_STATUS              0x2     /* r/w Identifies CPUtype & VG-RAM */
#define OTI87_TEST                0x3     /* r/w DON'T USE */
#define OTI87_VLB_CONTROL         0x4     /* r/w Sets VLB's wait states */ 
#define OTI87_MEM_MAPPING         0x5     /* r/w Enables and controls the 
					   *     linear framebuffer 
					   */ 

#define OTI87_CLOCK               0x6      /* r/w Selects from 16 clocks */
#define OTI87_CONFIG_1            0x7      /* r  DON'T USE */ 
#define OTI87_CONFIG_2            0x8      /* r  DON'T USE */ 
#define OTI87_MISC_COMPAT         0xD      /* r/w Is for compatibility with
					    *     67/77 boards, DON'T USE 
					    */
#define OTI87_BUS_CTRL           0x13      /* r/w Controls the AT-BUS */
#define OTI87_OVERFLOW           0x14
#define OTI87_HSYNC              0x15      
#define OTI87_EXT_CRTC_CTRL      0x17      
#define OTI87_PALETTE_RANGE      0x19
#define OTI87_FIFO_DEPTH         0x20     /* r/w Controls the command execution
					   *     FIFO depth 
					   */
#define OTI87_MODE               0x21     /* r/w Enables HiColor, Packed pixel
					   *     etc... 
					   */ 
#define OTI87_FEATURE_CTRL       0x22      /* r/w Accel. functions enabling */
#define OTI87_XREAD              0x23      /* r/w Extended Read Bank  */
#define OTI87_XWRITE             0x24      /* r/w Extended Write Bank */
#define OTI87_XREADWRITE         0x25      /* r/w Extended R/W Bank */

/* The next registers control the color expansion routines */

#define OTI87_COLOR_EXPANDER     0x30      
#define OTI87_FORE               0x31
#define OTI87_BACK               0x32
#define OTI87_PATTERN            0x33
#define OTI87_MASK               0x34

/* Latch control */

#define OTI87_LATCH_INDEX        0x35
#define OTI87_LATCH_DATA         0x36

/* The next registers control the Hardware Cursor  */

#define OTI87_HC_HORIZONTAL_H    0x40
#define OTI87_HC_HORIZONTAL_L    0x41
#define OTI87_HC_VERTICAL_H      0x42
#define OTI87_HC_VERTICAL_L      0x43
#define OTI87_HC_HOR_PRESET      0x44
#define OTI87_HC_VER_PRESET      0x45
#define OTI87_HC_START_HL        0x47
#define OTI87_HC_START_LH        0x48
#define OTI87_HC_START_LL        0x49
#define OTI87_HC_BACK            0x4A
#define OTI87_HC_FORE            0x4B
#define OTI87_HC_CTRL            0x4C

/*
 * Board indexes to be used throughout the code .
 */
   
#define OTI67  0    /* same index as ident function */
#define OTI77  1    /* same index as ident function */
#define OTI87  2    /* same index as ident function */
#define OTI37C 3    /* same index as ident function */

/*
 * Other variables defined here.
 */

static int OTI_chipset;
static Bool OTI_2mb_bank = FALSE;
static Bool OTI_linear = FALSE;
static Bool OTI_vlb = FALSE;

static unsigned OAK_ExtPorts[] = { OTI_INDEX, OTI_R_W };
static int Num_OAK_ExtPorts = (sizeof(OAK_ExtPorts)/sizeof(OAK_ExtPorts[0]));

/* 
 * OAKIdent --
 */

static char *
OAKIdent(n)
     int n;
{
  static char *chipsets[] = {"oti067","oti077","oti087","oti037c"};
  
  if (n + 1 > sizeof(chipsets) / sizeof(char *))
    return(NULL);
  else
    return(chipsets[n]);
}

/* 
 * OAKClockSelect --
 */

static Bool
OAKClockSelect(no)
     int no;
{
  static unsigned char save1,save2;
  unsigned char temp;

  switch (no)
    {
    case CLK_REG_SAVE:        /* save clocks */

      switch (OTI_chipset)
	{
	case OTI87:          /* only using Extended Clock Register */
	  outb(OTI_INDEX, OTI87_CLOCK); 
	  save1 = inb(OTI_R_W);
	  break;

	case OTI37C:        /* for all these get the info from the various */
	case OTI67:         /* registers. */  
	case OTI77:          
	default:
	  save1 = inb(0x3CC); 
	  outb(OTI_INDEX, OTI_MISC);
	  save2 = inb(OTI_R_W);
	  break;
	}
      break;
      
    case CLK_REG_RESTORE:     /* here we restore the previous values */

      switch (OTI_chipset)
	{
	case OTI87:
	  outb(OTI_INDEX, OTI87_CLOCK);
	  outb(OTI_R_W,save1);
	  break;

	case OTI37C:
	case OTI67:
	case OTI77:
	default:
      	  outb(0x3C2, save1);
	  outb(OTI_INDEX, OTI_MISC | (save2 << 8));
	  break;
	}
      break;

    default:

      switch (OTI_chipset)
	{
	case OTI87:
	  outb(OTI_INDEX, OTI87_CLOCK);
	  outb(OTI_R_W,no);
	  break;

	case OTI37C:
	case OTI67:
	case OTI77:
	default:
	  temp = inb(0x3CC);
	  outb(0x3C2, ( temp & 0xF3) | ((no << 2) & 0x0C));
	  outb(OTI_INDEX, OTI_MISC);
	  temp = inb(OTI_R_W);
	  outw(OTI_INDEX, OTI_MISC | 
	       ((( temp & 0xDF ) | (( no & 4) << 3)) << 8));
	  break;

	}
    }
}

/*
 * OAKProbe --
 */

static Bool
OAKProbe()
{
  unsigned char save, temp1, temp, temp2;
  
  xf86ClearIOPortList(vga256InfoRec.scrnIndex);
  xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
  xf86AddIOPorts(vga256InfoRec.scrnIndex, Num_OAK_ExtPorts, OAK_ExtPorts);
  
  /* See if this is one of the Oak Tech. Inc. chipsets */

  if (vga256InfoRec.chipset)      /* if harcoded.... */
    {
      if (!StrCaseCmp(vga256InfoRec.chipset, OAKIdent(0))) {
	OTI_chipset = OTI67;
      } else if (!StrCaseCmp(vga256InfoRec.chipset, OAKIdent(1))) {
	OTI_chipset = OTI77;
      } else if (!StrCaseCmp(vga256InfoRec.chipset, OAKIdent(2))) {
	OTI_chipset = OTI87;
      } else if (!StrCaseCmp(vga256InfoRec.chipset, OAKIdent(3))) {
	OTI_chipset = OTI37C;
      }       
      else 
	{
	  return (FALSE); 
	}
      OAKEnterLeave(ENTER);
    }

  else                  /* i.e. not harcoded, we have to actually test */
    {
      OAKEnterLeave(ENTER);
      outb(OTI_INDEX, OTI_SEGMENT);
      save = inb(OTI_R_W);
      outb(OTI_R_W, save ^ 0x11);
      temp1 = inb(OTI_R_W);
      outb(OTI_R_W, save);
      if (temp1 != ( save ^ 0x11 )) {
	OAKEnterLeave(LEAVE);
	return(FALSE);         /* we got a failure, tell to go on probing */
      }

      /* Here we know we have an Oak */

      outb(OTI_INDEX, OTI87_IDENTIFY );
      if (inb(OTI_R_W) != 1) 
	{      
	  temp1 = inb(OTI_INDEX) >> 5;
	  switch (temp1) {
	  case 0x07 : /* oti 57 don't know it */
	    ErrorF("%s %s: oak: OTI-57 unsupported.\n", XCONFIG_PROBED,
		   vga256InfoRec.name);
	    OAKEnterLeave(LEAVE);
	    return(FALSE);
	  case 0x02 : /* oti 67 */
	    OTI_chipset = OTI67;
	    break;
	  case 0x05 : /* oti 77 */
	    OTI_chipset = OTI77;
	    break;
#ifdef OTI37_PROBE
	  case 0x00 : /* oti 37c */
	    OTI_chipset = OTI37C;
	    break;
#endif
	  default : /* don't know it by these bits */
#if 0
	    /* Some Avance Logic chipsets get this far */
	    ErrorF("%s %s: oak: unknown chipset, perhaps a 083?.\n", 
		   XCONFIG_PROBED,
		   vga256InfoRec.name);
#endif
	    OAKEnterLeave(LEAVE);
	    return(FALSE);
	  }
	}
      else 
	{ /* oti087 */
	  OTI_chipset = OTI87;
	}
    }

  /* Chipset deterined, let's test the DRAM's */

  if (!vga256InfoRec.videoRam) 
    {

      switch (OTI_chipset)
	{
	case OTI87:
	  
	  outb(OTI_INDEX, OTI87_STATUS);
	  temp = ( inb(OTI_R_W) & 0x6);
	  switch (temp) 
	    {
	    case 0:
	      vga256InfoRec.videoRam = 256;
	      break;
	    case 2:
	      vga256InfoRec.videoRam = 512;
	      break;        
	    case 4:
	      vga256InfoRec.videoRam = 1024;
	      break;
	    case 6:
	      vga256InfoRec.videoRam = 2048;
	      break;
	    default:
	      ErrorF("%s %s: oti087: unknown video memory\n",
		     XCONFIG_PROBED, vga256InfoRec.name);
	      OAKEnterLeave(LEAVE);
	      return(FALSE);
	    }
	  break;

	case OTI67:
	case OTI77:
	case OTI37C:
	default:
	  outb(OTI_INDEX, OTI_MISC);
	  temp1 = inb(OTI_R_W);
	  temp1 &= 0xC0;
	  if (temp1 == 0xC0 )
	    vga256InfoRec.videoRam = 1024;
	  else if (temp1 == 0x80 )
	    vga256InfoRec.videoRam = 512;
	  else if (temp1 == 0x00 )
	    vga256InfoRec.videoRam = 256;
	  else {
	    ErrorF("%s %s: oak: unknown video memory.\n",
		   XCONFIG_PROBED, vga256InfoRec.name);
	    OAKEnterLeave(LEAVE);
	    return(FALSE);
	  }  
	}
    }

  /* Now, see to the clocks */

  if (!vga256InfoRec.clocks)
    {
      switch (OTI_chipset)
	{
	case OTI87:
	  vgaGetClocks(16, OAKClockSelect);
	  break;
	case OTI67:
	case OTI77:
	case OTI37C:
	default:
	  vgaGetClocks(8, OAKClockSelect);
	  break;
	}
    }
  
  /*
   * I could have used a #define to set this max speed, but the board 
   * manual recomends a max dot clock of 80Mhz, and the usual clock
   * generator of these boards is the OTI068, which gives a maximum clock
   * of 78Mhz, so I suppose its not necessary to define it. Personally,
   * I doubt any OEM will be able to use higher dot clocks with 70ns DRAM
   * and this chipset.
   */

  vga256InfoRec.maxClock = 80000 ;
 
  /* We know, Oak + chipset + memory + clocks, lets find something else */

  switch (OTI_chipset)
    {
    case OTI87:
      outb(OTI_INDEX,OTI87_CONFIG_1);
      temp2 = ( inb(OTI_R_W) & 0x06);
      switch ( temp2 )
	{
	case 0x00 :
	  ErrorF("%s %s: oti087: VLB card integrated in MotherBoard. \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: MemBase for Linear Addressing will be\n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: set at 0x4E00000 (the 78Mbyte mark). \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  OTI_vlb = TRUE;
	  break;
	case 0x02 :
	  ErrorF("%s %s: oti087: VLB card in a 32-bit VESA slot. \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: MemBase for Linear Addressing will be \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: set at 0x4E00000 (the 78Mbyte mark). \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  OTI_vlb = TRUE;
	  break;
	case 0x04 :
	  ErrorF("%s %s: oti087: ISA card integrated in MotherBoard. \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: MemBase for Linear Addressing will be \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: set at 0xE00000 (the 14Mbyte mark). \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  OTI_vlb = FALSE;
	  break;
	case 0x06 :
	  ErrorF("%s %s: oti087: ISA card in a 16-bit AT-BUS slot.  \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: MemBase for Linear Addressing will be \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  ErrorF("%s %s: oti087: set at 0xE00000 (the 14Mbyte mark). \n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	  OTI_vlb = FALSE;
	  break;
	}
      
     outb(OTI_INDEX,OTI87_CONFIG_2);
     temp2 = ( inb(OTI_R_W) & 0x0C);
     switch (temp2 )
       {
       case 0x00 :
	 ErrorF("%s %s: oti087: BT476,SC11487,IMSG174 or equivalent RAMDAC.\n",
		XCONFIG_PROBED, vga256InfoRec.name);
	 break;
       case 0x04 :
	 ErrorF("%s %s: oti087: MU9C1715 or equivalent RAMDAC.\n",
		XCONFIG_PROBED, vga256InfoRec.name);
	 break;
       case 0x08 :
	  ErrorF("%s %s: oti087: BT484 or equivalent RAMDAC.\n",
		 XCONFIG_PROBED, vga256InfoRec.name);
	 break;
       }
    default:    /* No special probes for other chipsets - yet - */
      ;
    }

  /*
   * Once we know the chipset, we can modify the driver structure 
   * to allow 87's fancy things. 
   */

  switch (OTI_chipset)
    {
    case OTI87:

      /* set the banking routines */

      OAK.ChipSetRead = OTI87SetRead ;    
      OAK.ChipSetWrite = OTI87SetWrite ;
      OAK.ChipSetReadWrite = OTI87SetReadWrite ;
      OTI_2mb_bank = TRUE;
  
      /* deal with linear addressing */

     if (OFLG_ISSET(OPTION_LINEAR, &vga256InfoRec.options))     
	{
	  OAK.ChipUseLinearAddressing = TRUE ;
	  if (OTI_vlb) 
	    {
	      OAK.ChipLinearBase=0x4E00000 ;
	    } 
	  else 
	    {
	      OAK.ChipLinearBase=0xE00000 ;
	    }
	  OAK.ChipLinearSize = vga256InfoRec.videoRam * 1024 ;
	  OTI_linear = TRUE;
	}
      
    default:        /* nothing to do for other chipsets */
      ;
    }

  vga256InfoRec.chipset = OAKIdent(OTI_chipset);
  vga256InfoRec.bankedMono = TRUE;
#ifndef MONOVGA
#ifdef XFreeXDGA 
  vga256InfoRec.directMode = XF86DGADirectPresent;
#endif
#endif

  /* Lastly we add the flags for the 'Option' fields of the driver */
  
  OFLG_SET(OPTION_LINEAR, &OAK.ChipOptionFlags);          /* set linear mode */
  OFLG_SET(OPTION_ENABLE_BITBLT, &OAK.ChipOptionFlags);   /* bitblt engine */
  OFLG_SET(OPTION_FIFO_CONSERV, &OAK.ChipOptionFlags);    /* FIFO = 14 */
  OFLG_SET(OPTION_FIFO_AGGRESSIVE, &OAK.ChipOptionFlags); /* FIFO = 2  */
  OFLG_SET(OPTION_FB_DEBUG, &OAK.ChipOptionFlags);   /* Dump registers */
  OFLG_SET(OPTION_NOACCEL, &OAK.ChipOptionFlags);    /* Don't use accel. */

  OFLG_SET(OPTION_CLOCK_66, &OAK.ChipOptionFlags);    /* 66Mhz Interface */
  OFLG_SET(OPTION_CLOCK_50, &OAK.ChipOptionFlags);    /* 50Mhz Interface */
  OFLG_SET(OPTION_NO_WAIT, &OAK.ChipOptionFlags);          /* 0-wait VLB */
  OFLG_SET(OPTION_FIRST_WWAIT, &OAK.ChipOptionFlags);/* 1st write wait VLB*/
  OFLG_SET(OPTION_ALL_WAIT, &OAK.ChipOptionFlags);   /* all wait VLB */
  OFLG_SET(OPTION_READ_WAIT, &OAK.ChipOptionFlags);  /* read cycle wait state*/
  OFLG_SET(OPTION_ONE_WAIT, &OAK.ChipOptionFlags);   /* min. 1 wait state, 
							all cycles */
  OFLG_SET(OPTION_WRITE_WAIT, &OAK.ChipOptionFlags);/* write cycle wait state*/

  return(TRUE);     /* Successful completion */
  
}

/*
 * OAKEnterLeave --
 */

static void 
OAKEnterLeave(enter)
     Bool enter;
{
  static unsigned char save;
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
     
      /* Unprotect CRTC[0-7] */
      
      outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, temp & 0x7F);
      
      /* There are no locks on the 087, we do it
       * anyway 'cause we dunno if is an 87 */
	
      outb(OTI_INDEX, OTI_CRT_CNTL);
      temp = inb(OTI_R_W);
      outb(OTI_R_W, temp & 0xF0);
      save = temp;
      
    }
  else 
    {
      outb(OTI_INDEX, OTI_CRT_CNTL);
      /* don't set the i/o write test bit even though
       * we cleared it on entry */
	
      outb(OTI_R_W, (save & 0xF7) );
      
      /* Protect CRTC[0-7] */
      
      outb(vgaIOBase + 4, 0x11); temp = inb(vgaIOBase + 5);
      outb(vgaIOBase + 5, (temp & 0x7F) | 0x80);
      
      xf86DisableIOPorts(vga256InfoRec.scrnIndex);
    }
}


/*
 * OAKRestore --
 */

extern void 
OAKRestore(restore)
     vgaOAKPtr restore;
{
  unsigned char temp;

  vgaProtect(TRUE);
      
  /* Go to bank 0 */

  switch (OTI_chipset)
    {
    case OTI87:
      outb(OTI_INDEX, OTI87_XWRITE); outb (OTI_R_W, 0x00);
      outb(OTI_INDEX, OTI87_XREAD) ; outb (OTI_R_W, 0x00);
      break;

    case OTI67:
    case OTI77:
    case OTI37C:
    default:
      outw(OTI_INDEX, OTI_SEGMENT);
      break;
    }
  
  /* To prevent text mode to be absolutely fouled up, oti077-67 
   * chipsets needed the extended clock bit set to 0 to avoid 
   * something, as the oti087 text mode is currenlty fouled when
   * I leave X, I will use sgoldman's experience.
   */
  /*
   *  Set the OTI-Misc register. We must be sure that we
   *  aren't in one of the extended graphics modes when
   *  we are leaving X and about to call vgaHWRestore for
   *  the last time. If we don't text mode is completely
   *  fouled up.
   */

  switch (OTI_chipset)
    {
    case OTI87:
      outb(OTI_INDEX, OTI87_MODE);
      outb(OTI_R_W, restore->oti87ModeSelect );
      break;
      
    case OTI67:
    case OTI77:
    case OTI37C:
    default:
      outb(OTI_INDEX, OTI_MISC);
      temp = inb(OTI_R_W) & 0x20; /* get the clock bit */
      temp |= (restore->oakMisc & 0xDF);
      outb(OTI_R_W, temp);
      break;
    }

  switch (OTI_chipset)
    {
    case OTI87:
      outb(OTI_INDEX, OTI87_XREAD);
      outb(OTI_R_W , restore->oti87RBank );
      outb(OTI_INDEX, OTI87_XWRITE);
      outb(OTI_R_W, restore->oti87WBank );
      outb(OTI_INDEX, OTI87_MODE );
      outb(OTI_R_W , restore->oti87ModeSelect );
      if (restore->std.NoClock >= 0) 
	{
	  outb(OTI_INDEX, OTI87_CLOCK );
	  outb(OTI_R_W , restore->oti87Clock );
	}
      outb(OTI_INDEX, OTI87_FIFO_DEPTH );
      outb(OTI_R_W, restore->oti87FIFODepth );
      outb(OTI_INDEX, OTI87_OVERFLOW );
      outb(OTI_R_W ,restore->oti87Overflow );
      outb(OTI_INDEX, OTI87_HSYNC );
      outb(OTI_R_W, restore->oti87Hsync2 );
      
      /* Advanced feature registers */
      
      outb(OTI_INDEX, OTI87_VLB_CONTROL );
      outb(OTI_R_W , restore->oti87VLBControl  );
      outb(OTI_INDEX, OTI87_MEM_MAPPING );
      outb(OTI_R_W , restore->oti87Mapping     );
      outb(OTI_INDEX, OTI87_BUS_CTRL    );
      outb(OTI_R_W , restore->oti87BusControl  );
      outb(OTI_INDEX, OTI87_EXT_CRTC_CTRL );
      outb(OTI_R_W , restore->oti87XCrt     );
      outb(OTI_INDEX, OTI87_PALETTE_RANGE );
      outb(OTI_R_W ,restore->oti87ColorPalette );
      outb(OTI_INDEX, OTI87_FEATURE_CTRL );
      outb(OTI_R_W , restore->oti87Feature     );
      outb(OTI_INDEX,OTI87_COLOR_EXPANDER );
      outb(OTI_R_W , restore->oti87ColorExpansion);
      outb(OTI_INDEX, OTI87_FORE      );
      outb(OTI_R_W , restore->oti87FGExpansion   );
      outb(OTI_INDEX, OTI87_BACK       );
      outb(OTI_R_W , restore->oti87BGExpansion  );
      outb(OTI_INDEX, OTI87_PATTERN   );
      outb(OTI_R_W , restore->oti87ColorPattern  );
      outb(OTI_INDEX, OTI87_MASK       );
      outb(OTI_R_W , restore->oti87PixMask      );
      outb(OTI_INDEX, OTI87_HC_HORIZONTAL_H  );
      outb(OTI_R_W , restore->oti87HC1     );
      outb(OTI_INDEX, OTI87_HC_HORIZONTAL_L  );
      outb(OTI_R_W , restore->oti87HC2     );
      outb(OTI_INDEX, OTI87_HC_VERTICAL_H    );
      outb(OTI_R_W , restore->oti87HC3     );
      outb(OTI_INDEX, OTI87_HC_VERTICAL_L    );
      outb(OTI_R_W , restore->oti87HC4     );
      outb(OTI_INDEX, OTI87_HC_HOR_PRESET    );
      outb(OTI_R_W , restore->oti87HC5     );
      outb(OTI_INDEX, OTI87_HC_VER_PRESET    );
      outb(OTI_R_W , restore->oti87HC6     );
      outb(OTI_INDEX, OTI87_HC_START_HL      );
      outb(OTI_R_W , restore->oti87HC7     );
      outb(OTI_INDEX, OTI87_HC_START_LH      );
      outb(OTI_R_W , restore->oti87HC8     );
      outb(OTI_INDEX, OTI87_HC_START_LL      );
      outb(OTI_R_W , restore->oti87HC9     );
      outb(OTI_INDEX, OTI87_HC_BACK          );
      outb(OTI_R_W , restore->oti87HC10    );
      outb(OTI_INDEX, OTI87_HC_FORE          );
      outb(OTI_R_W , restore->oti87HC11    );
      outb(OTI_INDEX, OTI87_HC_CTRL          );
      outb(OTI_R_W , restore->oti87HC12    );
      break;

    case OTI67:
    case OTI77:
    case OTI37C:
    default:
      outb(OTI_INDEX, OTI_SEGMENT);
      outb(OTI_R_W, restore->oakBank);
      if (restore->std.NoClock >= 0) 
	{	/* restore clock-select bits. */
	  outw(OTI_INDEX, OTI_MISC + (restore->oakMisc << 8));
	} 
      else 
	{      /* don't restore clock-select bits. */
	  outb(OTI_INDEX, OTI_MISC);
	  temp = inb(OTI_R_W) & 0x20; /* get the clock bit */
	  temp |= (restore->oakMisc & 0xDF);
	  outb(OTI_R_W, temp);
	}
      outb(OTI_INDEX, OTI_BCOMPAT);
      temp = inb(OTI_R_W);
      temp &= 0xF9;
      temp |= (restore->oakBCompat & 0x6);
      outb(OTI_INDEX, OTI_CONFIG);
      temp = inb(OTI_R_W);
      temp &= 0xF7;
      temp |= (restore->oakConfig & 0x8);
      outb(OTI_R_W, temp);
      outw(OTI_INDEX, OTI_OVERFLOW + (restore->oakOverflow << 8));
      outw(OTI_INDEX, OTI_HSYNC2 + (restore->oakHsync2 << 8));
      if ( OTI_chipset == OTI77)
	{
	  outw(OTI_INDEX, OTI_OVERFLOW2 + (restore->oakOverflow2 << 8));
	}
      break;
    }

  /*
   * This function handles restoring the generic VGA registers.
   */
  
  vgaHWRestore((vgaHWPtr)restore);
    
  /* now reenable the timing sequencer */

  outw(0x3C4, 0x0300); /* now reenable the timing sequencer */

  vgaProtect(FALSE);
}

/*
 * OAKSave --
 */
extern void *
OAKSave(save)
     vgaOAKPtr save;
{
  unsigned char temp, temp1;

 /* Go to bank 0 */

  switch (OTI_chipset)
    {
    case OTI87:
      outb(OTI_INDEX, OTI87_XREAD);
      temp = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_XWRITE);
      temp1 = inb(OTI_R_W);
      outb(OTI_R_W, 0x0);
      break;

    case OTI67:
    case OTI77:
    case OTI37C:
    default:
      outb(OTI_INDEX, OTI_SEGMENT);
      temp = inb(OTI_R_W);
      outb(OTI_R_W, 0x0);
      break;
    }
  
  save = (vgaOAKPtr)vgaHWSave((vgaHWPtr)save, sizeof(vgaOAKRec));

  switch (OTI_chipset)
    {
    case OTI87:
      save->oti87RBank = temp;      
      save->oti87WBank = temp1; 
      outb(OTI_INDEX, OTI87_MODE );
      save->oti87ModeSelect          = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_CLOCK);
      save->oti87Clock               = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_FIFO_DEPTH );
      save->oti87FIFODepth           = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_OVERFLOW );
      save->oti87Overflow            = inb (OTI_R_W);
      outb(OTI_INDEX, OTI87_HSYNC );
      save->oti87Hsync2              = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_VLB_CONTROL);
      save->oti87VLBControl          = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_MEM_MAPPING );
      save->oti87Mapping             = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_CLOCK);
      save->oti87Clock               = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_BUS_CTRL);
      save->oti87BusControl          = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_EXT_CRTC_CTRL );
      save->oti87XCrt               = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_PALETTE_RANGE );
      save->oti87ColorPalette        = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_FEATURE_CTRL );
      save->oti87Feature             = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_COLOR_EXPANDER );
      save->oti87ColorExpansion      = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_FORE );
      save->oti87FGExpansion         = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_BACK );
      save->oti87BGExpansion         = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_PATTERN);
      save->oti87ColorPattern        = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_MASK );
      save->oti87PixMask             = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_HORIZONTAL_H);
      save->oti87HC1                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_HORIZONTAL_L );
      save->oti87HC2                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_VERTICAL_H );
      save->oti87HC3                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_VERTICAL_L );
      save->oti87HC4                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_HOR_PRESET);
      save->oti87HC5                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_VER_PRESET);
      save->oti87HC6                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_START_HL);
      save->oti87HC7                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_START_LH);
      save->oti87HC8                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_START_LL );
      save->oti87HC9                 = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_BACK);
      save->oti87HC10                = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_FORE);
      save->oti87HC11                = inb(OTI_R_W);
      outb(OTI_INDEX, OTI87_HC_CTRL);
      save->oti87HC12                = inb(OTI_R_W);
      break;

    case OTI67:
    case OTI77:
    case OTI37C:
    default:
      save->oakBank = temp;       
      outb(OTI_INDEX, OTI_MISC);
      save->oakMisc = inb(OTI_R_W);
      outb(OTI_INDEX, OTI_CONFIG);
      save->oakConfig = inb(OTI_R_W);
      outb(OTI_INDEX, OTI_BCOMPAT);
      save->oakBCompat = inb(OTI_R_W);
      outb(OTI_INDEX, OTI_OVERFLOW);
      save->oakOverflow = inb(OTI_R_W);
      outb(OTI_INDEX, OTI_HSYNC2);
      save->oakHsync2 = inb(OTI_R_W);
      if ( OTI_chipset == OTI77) 
	{
	  outb(OTI_INDEX, OTI_OVERFLOW2);
	  save->oakOverflow2 = inb(OTI_R_W);
	}
      break;
    }
  return ((void *) save);
}

/*
 * OAKInit --
 */
static Bool
OAKInit(mode)
     DisplayModePtr mode;
{
  int tmp11,i;
  if (mode->Flags & V_INTERLACE ) {
    /*
       When in interlace mode cut the vertical numbers in half.
       (Try and find that in the manual!)
       Could just set VGA_DIVIDE_VERT but we'd have to
       test it and do the divides here anyway.
       */ 
    if (!mode->CrtcVAdjusted) {
      mode->CrtcVTotal >>= 1;
      mode->CrtcVDisplay >>= 1;
      mode->CrtcVSyncStart >>= 1;
      mode->CrtcVSyncEnd >>= 1;
      mode->CrtcVAdjusted = TRUE;
    }
  }
  /*
   * This will allocate the datastructure and initialize all of the
   * generic VGA registers.
   */
  if (!vgaHWInit(mode,sizeof(vgaOAKRec)))
    return(FALSE);
  
  /* Now we start with chipset-specific initializations */

  switch (OTI_chipset)
    {
    case OTI87:
      
      new->std.Attribute[16] = 0x01; 
      if ( new->std.NoClock >= 0 ) 
	{
	  new->oti87Clock = new->std.NoClock ;
	}
      new->oti87RBank = 0;
      new->oti87WBank = 0; 
      new->oti87ModeSelect = 0x04 ;
      if (mode->Flags & V_INTERLACE ) 
	{
	  new->oti87Overflow = 0x80 |
	    /* V-retrace-start */  (((mode->CrtcVSyncStart ) & 0x400) >> 8 ) |
	      /*V-blank-start */  ((((mode->CrtcVDisplay-1) ) & 0x400) >> 9 ) |
		/* V-total */      ((((mode->CrtcVTotal-2) ) & 0x400) >> 10 ) ;
	  new->oti87Hsync2 =  (mode->CrtcVTotal-2) >> 3; 
	} 
      else 
	{
	  new->oti87Overflow = (mode->Flags & V_INTERLACE ? 0x80 : 0x00) |
	    /* V-retrace-start */  ((mode->CrtcVSyncStart & 0x400) >> 8 ) |
	      /* V-blank-start */  (((mode->CrtcVDisplay-1) & 0x400) >> 9 ) |
		/* V-total */      (((mode->CrtcVTotal-2) & 0x400) >> 10 ) ;
	  new->oti87Hsync2 = 0;
	}

      /* After general Init, do other chipset things */

      /* FIFO data */

      if (OFLG_ISSET(OPTION_FIFO_CONSERV, &vga256InfoRec.options)) 
	{
	  new->oti87FIFODepth = 0x14;
	  ErrorF ("%s %s: oti087: FIFO depth:            14. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	} 
      else if (OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &vga256InfoRec.options)) 
	{
	  new->oti87FIFODepth = 0x00;
	  ErrorF ("%s %s: oti087: FIFO depth:            0. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	} 
      else 
	{
	  new->oti87FIFODepth = 0x06;
	  ErrorF ("%s %s: oti087: FIFO depth:            6. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}

      

      /* Undocumented clock + wait states, do it both ways just in case */
      if (OFLG_ISSET(OPTION_NO_WAIT, &vga256InfoRec.options))
	{
	  new->oti87VLBControl = 0x00;
	  ErrorF ("%s %s: oti087: VLB interface:         0 wait states. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}
      else if (OFLG_ISSET(OPTION_FIRST_WWAIT, &vga256InfoRec.options))
	{
	  new->oti87VLBControl = 0x01;
       ErrorF ("%s %s: oti087: VLB interface:         First write wait state. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}
      else if (OFLG_ISSET(OPTION_ONE_WAIT, &vga256InfoRec.options))
	{
	  new->oti87VLBControl = 0x08;
       ErrorF ("%s %s: oti087: VLB interface:         Minimun 1 wait state. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}
      else if (OFLG_ISSET(OPTION_READ_WAIT, &vga256InfoRec.options))
	{
	  new->oti87VLBControl = 0x04;
       ErrorF ("%s %s: oti087: VLB interface:         Read cycle wait state. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);

	}
      else if (OFLG_ISSET(OPTION_WRITE_WAIT, &vga256InfoRec.options))
	{
	  new->oti87VLBControl = 0x02;
       ErrorF ("%s %s: oti087: VLB interface:         Write cycle wait state. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}
      else 
	{
	  new->oti87VLBControl = 0x0F;
	  ErrorF ("%s %s: oti087: VLB interface:         All wait states. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}

      if (OFLG_ISSET(OPTION_CLOCK_66, &vga256InfoRec.options))
	{
	  new->oti87VLBControl &= 0x1F;
	  ErrorF ("%s %s: oti087: Chipset speed:         66 Mhz. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}
      else 
	{
	  new->oti87VLBControl &= 0x0F;
	  ErrorF ("%s %s: oti087: Chipset speed:         50 Mhz. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}

      /* BitBlt transfer engine */
      

      if (OFLG_ISSET(OPTION_ENABLE_BITBLT, &vga256InfoRec.options))
	{
	  if (OTI_vlb)
	    {
	      new->oti87Feature = 0x1C;
	  ErrorF ("%s %s: oti087: BitBlt engine status:  Enabled. \n", 
		      XCONFIG_GIVEN,
		      vga256InfoRec.name);
	    }
	  else 
	    {
	      new->oti87Feature = 0x14;
	  ErrorF ("%s %s: oti087: BitBlt engine status:  Enabled. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	    }
	}
      else /* not bitblt */
	{
	      new->oti87Feature = 0x18;
	  ErrorF ("%s %s: oti087: BitBlt engine status:  Disabled. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}
      


      new-> oti87BusControl = 0xC8 ;
      new-> oti87ColorPalette = 0xf ; 
      new-> oti87ColorExpansion = 0x0 ; 
      new-> oti87FGExpansion = 0x0;
      new-> oti87BGExpansion = 0x0; 
      new-> oti87ColorPattern = 0xff ;
      new-> oti87PixMask = 0xff ;

      /* Take care of Linear addressing */

      if (OTI_linear)
	{
	  if (OTI_vlb)
	    {
	      
    /* This is an undocumented option of the board. Disabling DMA transfers
     * the 64MB (0x4000000) address mark pin is enabled, allowing higher-than
     * 16Mbyte-mark addressing, I must thank the people who did the
     * README.cirrus for the clue, and the binaries of the OTI087 MS-Windoze
     * drivers for allowing me to do some diff's :), it was a hell to find 
     * and debug, but at last....it's here (and I will be able to buy another
     * 4 Mbytes of RAM and be allowed linear addressing)
     */
	      
	      switch(vga256InfoRec.videoRam)
		{
		case 256:
		  new -> oti87Mapping = 0xE3;
		  outb (OTI_INDEX, OTI87_MEM_MAPPING);
		  outb (OTI_R_W, 0xE3);
		  break;
		case 512:
		  new -> oti87Mapping = 0xE7;
		  outb (OTI_INDEX, OTI87_MEM_MAPPING);
		  outb (OTI_R_W, 0xE7);
		  break;
		case 1024:
		  new -> oti87Mapping = 0xEB; 
		  outb (OTI_INDEX, OTI87_MEM_MAPPING);
		  outb (OTI_R_W, 0xEB);
		  break;
		case 2048:
		  new -> oti87Mapping = 0xEF;
		  outb (OTI_INDEX, OTI87_MEM_MAPPING);
		  outb (OTI_R_W, 0xEF); 
		  break;
		} 
	    }
	  else
	    {
	      switch(vga256InfoRec.videoRam)
		{
		case 256:
		  new -> oti87Mapping = 0xE1;
		  break;
		case 512:
		  new -> oti87Mapping = 0xE5;
		  break;
		case 1024:
		  new -> oti87Mapping = 0xE9; 
		  break;
		case 2048:
		  new -> oti87Mapping = 0xED; 
		  break;
		} 
	    }    
	  ErrorF ("%s %s: oti087: Addressing Mode:       Linear. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	  ErrorF ("%s %s: oti087:         base address:  0x%X. \n",
		  XCONFIG_GIVEN,
		  vga256InfoRec.name, 
		  OAK.ChipLinearBase);
	  ErrorF ("%s %s: oti087:         aperture:      %d Kbytes. \n", 
		  XCONFIG_PROBED,
		  vga256InfoRec.name,
		  (OAK.ChipLinearSize/1024));
	}
      else  /* Segmented addressing */
	{
	  ErrorF ("%s %s: oti087: Addressing Mode:       Segmented. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	  ErrorF ("%s %s: oti087:         page size:     64 Kb. \n", 
		  XCONFIG_GIVEN,
		  vga256InfoRec.name);
	}

      
      if (OFLG_ISSET(OPTION_FB_DEBUG, &vga256InfoRec.options))
	{

	  if (OTI_2mb_bank)
	    {
	      ErrorF ("%s %s: oti087: Using 2 MB banking routines. \n", 
		      XCONFIG_PROBED,
		      vga256InfoRec.name);
	    }

	  outb(OTI_INDEX, OTI87_FIFO_DEPTH);
	  tmp11 = inb(OTI_R_W);
	  ErrorF ("%s %s: oti087: Value of the FIFO register, 0x%X. \n", 
		  XCONFIG_PROBED,
		  vga256InfoRec.name, tmp11);
	  
	  outb(OTI_INDEX, OTI87_VLB_CONTROL);
	  tmp11 = inb(OTI_R_W);
	  ErrorF ("%s %s: oti087: Value for the Clock register is: 0x%X.\n", 
		  XCONFIG_PROBED, vga256InfoRec.name, tmp11 );
	  
	  ErrorF ("%s %s: oti087: Debug request parsed, dumping registers\n", 
		  XCONFIG_GIVEN,vga256InfoRec.name); 
	  for ( i = 0 ; i <= 0x4C ; i++)
	    {
	      outb(OTI_INDEX, i);
	      ErrorF ("%s %s: oti087: Register %X has value: .... %X \n", 
		      XCONFIG_GIVEN, vga256InfoRec.name, i, inb(OTI_R_W)); 
	    }
	}
      
      break;
    case OTI67:
    case OTI77:
    case OTI37C:
    default:
#ifndef MONOVGA
#if 0
      new->std.CRTC[19] = vga256InfoRec.displayWidth >> 3; /* 3 in byte mode */
#endif
      /* much clearer as 0x01 than 0x41, seems odd though... */
      new->std.Attribute[16] = 0x01; 
      if ( new->std.NoClock >= 0 ) 
	{
	  new->oakMisc = 0x0F | ((new->std.NoClock & 0x04) << 3);
	} 
      else
	new->oakMisc = 0x0F; /*  high res mode, deep fifo */
#else
      if ( new->std.NoClock >= 0 ) 
	{
	  new->oakMisc = 0x18 | ((new->std.NoClock & 0x04) << 3);
	} 
      else
	new->oakMisc = 0x18; /*  16 color high res mode */
#endif
      new->oakBank = 0; 
      new->oakBCompat = 0x80; /* vga mode */
      new->oakConfig = (OTI_chipset == OTI77 ? 0x8 : 0 );
      /* set number of ram chips! */                    /* 40 */
      new->oakMisc |= (vga256InfoRec.videoRam == 1024 ? 0x40 : 0x00 );
      new->oakMisc |= (vga256InfoRec.videoRam >= 512 ? 0x80 : 0x00 );
      if (mode->Flags & V_INTERLACE ) 
	{
	  new->oakOverflow = 0x80 |
	    /* V-retrace-start */  (((mode->CrtcVSyncStart ) & 0x400) >> 8 ) |
	      /*V-blank-start */  ((((mode->CrtcVDisplay-1) ) & 0x400) >> 9 ) |
		/* V-total */      ((((mode->CrtcVTotal-2) ) & 0x400) >> 10 ) ;
	  /* can set overflow2 no matter what here since restore will
	     do the right thing */
	  new->oakOverflow2 = 0;
	  /* Doc. says this is when vertical retrace will start in 
	     every odd frame in interlaced mode in characters. Hmm??? */
	  new->oakHsync2 =  (mode->CrtcVTotal-2) >> 3; 
	} 
      else 
	{
	  new->oakOverflow = (mode->Flags & V_INTERLACE ? 0x80 : 0x00) |
	    /* V-retrace-start */  ((mode->CrtcVSyncStart & 0x400) >> 8 ) |
	      /* V-blank-start */  (((mode->CrtcVDisplay-1) & 0x400) >> 9 ) |
		/* V-total */      (((mode->CrtcVTotal-2) & 0x400) >> 10 ) ;
	  /* can set overflow2 no matter what here since restore will
	     do the right thing */
	  new->oakOverflow2 = 0;
	  new->oakHsync2 = 0;
	}
      break;
    }
  return(TRUE);
}

/*
 * OAKAdjust --
 */
static void 
OAKAdjust(x, y)
     int x, y;
{
  int temp;
  int Base = (y * vga256InfoRec.displayWidth + x ) >> 3;
  outw(vgaIOBase + 4, (Base & 0x00FF00) | 0x0C);
  outw(vgaIOBase + 4, ((Base & 0x00FF) << 8) | 0x0D);

  switch (OTI_chipset)
    {
    case OTI87:
      outb (OTI_INDEX, OTI87_EXT_CRTC_CTRL);
      outb (OTI_R_W, Base >> 16 );
      break;
    
    case OTI67:
    case OTI77:
    case OTI37C:
    default:
      outb(OTI_INDEX, OTI_OVERFLOW);
      temp = inb(OTI_R_W);
      temp &= 0xF7;
      temp |= ((Base & 0x10000) >> (5+8));
      outb(OTI_R_W, temp);
      if ( OTI_chipset == OTI77) 
	{ 
	  outb(OTI_INDEX, OTI_OVERFLOW2);
	  temp = inb(OTI_R_W);
	  temp &= 0xF7;
	  temp |= ((Base & 0x20000) >> (6 + 8));
	  outb(OTI_R_W, temp);
	}
      break;
    }
#ifdef XFreeXDGA
  if (vga256InfoRec.directMode & XF86DGADirectGraphics) {
    /* Wait until vertical retrace is in progress. */
    while (inb(vgaIOBase + 0xA) & 0x08);
    while (!(inb(vgaIOBase + 0xA) & 0x08));
  }
#endif
}

/*
 * OAKValidMode --
 *
 */
static int
OAKValidMode(mode, verbose,flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}

/*
 *
 * OAKFbInit --
 */
static void
OAKFbInit()
{
#ifndef MONOVGA
  int VLB_CONTROL_VALUE = 0x07; /* all wait states */

  leftmask[0]= 0x80;
  leftmask[1]= 0xc0;
  leftmask[2]= 0xe0;
  leftmask[3]= 0xf0;
  leftmask[4]= 0xf8;
  leftmask[5]= 0xFc;
  leftmask[6]= 0xFe;
  
  rightmask[0]= 0x01;
  rightmask[1]= 0x03;
  rightmask[2]= 0x07;
  rightmask[3]= 0x0f;
  rightmask[4]= 0x1f;
  rightmask[5]= 0x3f;
  rightmask[6]= 0x7f;
  
  
  switch (OTI_chipset)
    {
    case OTI87:
      
      ErrorF ("%s %s: oti087: \n",
	      XCONFIG_PROBED,
	      vga256InfoRec.name);
      ErrorF ("%s %s: oti087: Offscreen Memory:      %d bytes.\n", 
	      XCONFIG_PROBED,
	      vga256InfoRec.name,
	      (1024*vga256InfoRec.videoRam)-(vga256InfoRec.displayWidth*vga256InfoRec.virtualY));
      
      
      /* Framebuffer accelerations */
      
      if ((!OTI_linear) && 
	  (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)))
	{
	  if (vga256InfoRec.displayWidth == 1024)
	    {
      ErrorF ("%s %s: oti087: Acceleration Status:   ON.\n", 
		      XCONFIG_PROBED,
		      vga256InfoRec.name);
	      vga256LowlevFuncs.fillBoxSolid = OAKFillBoxSolid;
	      vga256LowlevFuncs.fillRectSolidCopy = OAKFillRectSolidCopy;
#if 0	      
	      vga256LowlevFuncs.copyPlane1to8 = OAKCopyPlane1to8;
	      
	      
	      /* the code is still unworking */
	      vga256TEOps1Rect.FillSpans = OAKFillSolidSpansGeneral;
	      vga256TEOps.FillSpans = OAKFillSolidSpansGeneral;
	      vga256LowlevFuncs.fillSolidSpans = OAKFillSolidSpansGeneral;
#endif
	    }
	  else
	    {
      ErrorF ("%s %s: oti087: Acceleration Status:   Disabled (displayWidth != 1024).\n", 
		      XCONFIG_PROBED,
		      vga256InfoRec.name);
	    }
	}
      else if (OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options))   
	/* linear addressing */
	{
	  ErrorF ("%s %s: oti087: Acceleration Status:   OFF.\n", 
		  XCONFIG_PROBED,
		  vga256InfoRec.name);
	}
      else if ((OTI_linear) && 
	       (!OFLG_ISSET(OPTION_NOACCEL, &vga256InfoRec.options)))
	{
#if 0
	  ErrorF("%s %s: oti087: Enabling linear acceleration.\n", 
		 XCONFIG_PROBED,
		 vga256InfoRec.name);
	  vga256LowlevFuncs.fillBoxSolid = OAKLinearFillBoxSolid;
	  vga256LowlevFuncs.fillRectSolidCopy = OAKLinearFillRectSolidCopy;
#endif
	}
    }
#endif
}


