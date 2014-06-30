/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3.c,v 3.14.2.8 1997/06/01 12:33:31 dawes Exp $ */
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
/* $XConsortium: s3.c /main/9 1996/12/28 14:43:20 kaleb $ */

#include "misc.h"
#include "cfb.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "s3v.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "s3ELSA.h"
#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#ifdef PC98
#include "pc98_vers.h"
#include "s3pc98.h"
#endif

extern int s3MaxClock;
int s3Weight = RGB8_PSEUDO;
extern char *xf86VisualNames[];
char *clockchip_probed = XCONFIG_GIVEN;

extern s3VideoChipPtr s3Drivers[];

int vgaInterlaceType = VGA_DIVIDE_VERT;
void (*vgaSaveScreenFunc)() = vgaHWSaveScreen;

extern int defaultColorVisualClass;

static int s3ValidMode(
#if NeedFunctionPrototypes
    DisplayModePtr, Bool, int
#endif
);

ScrnInfoRec s3InfoRec =
{
   FALSE,			/* Bool configured */
   -1,				/* int tmpIndex */
   -1,				/* int scrnIndex */
   s3Probe,			/* Bool (* Probe)() */
   (Bool (*)())NoopDDA,		/* Bool (* Init)() */
   s3ValidMode,			/* int (* ValidMode)() */
   (void (*)())NoopDDA,		/* void (* EnterLeaveVT)() */
   (void (*)())NoopDDA,		/* void (* EnterLeaveMonitor)() */
   (void (*)())NoopDDA,		/* void (* EnterLeaveCursor)() */
   (void (*)())NoopDDA,		/* void (* AdjustFrame)() */
   (Bool (*)())NoopDDA,		/* Bool (* SwitchMode)() */
   s3DPMSSet,			/* void (* DPMSSet)() */
   s3PrintIdent,		/* void (* PrintIdent)() */
   8,				/* int depth */
   {5, 6, 5},			/* xrgb weight */
   8,				/* int bitsPerPixel */
   PseudoColor,			/* int defaultVisual */
   -1, -1,			/* int virtualX,virtualY */
   -1,				/* int displayWidth */
   -1, -1, -1, -1,		/* int frameX0, frameY0, frameX1, frameY1 */
   {0,},			/* OFlagSet options */
   {0,},			/* OFlagSet clockOptions */
   {0, },              		/* OFlagSet xconfigFlag */
   NULL,			/* char *chipset */
   NULL,			/* char *ramdac */
   {0, 0, 0, 0},		/* int dacSpeeds[MAXDACSPEEDS] */
   0,				/* int dacSpeedBpp */
   0,				/* int clocks */
   {0,},			/* int clock[MAXCLOCKS] */
   0,				/* int maxClock */
   0,				/* int videoRam */
   0xC0000,                     /* int BIOSbase */
   0,				/* unsigned long MemBase */
   240, 180,			/* int width, height */
   0,				/* unsigned long  speedup */
   NULL,			/* DisplayModePtr modes */
   NULL,			/* MonPtr monitor */
   NULL,			/* char           *clockprog */
   -1,			        /* int textclock */
   FALSE,			/* Bool           bankedMono */
   "S3V",			/* char           *name */
   {0, },			/* xrgb blackColour */
   {0, },			/* xrgb whiteColour */
   s3ValidTokens,		/* int *validTokens */
   S3_PATCHLEVEL,		/* char *patchlevel */
   0,				/* int IObase */
   0,				/* int PALbase */
   0,				/* int COPbase */
   0,				/* int POSbase */
   0,				/* int instance */
   0,				/* int s3Madjust */
   0,				/* int s3Nadjust */
   0,				/* int s3MClk */
   0,				/* int chipID */
   0,				/* int chipRev */
   0,				/* unsigned long VGAbase */
   0,				/* int s3RefClk */
   -1,				/* int s3BlankDelay */
   0,				/* int textClockFreq */
   NULL,                        /* char* DCConfig */
   NULL,                        /* char* DCOptions */
   0				/* int MemClk */
#ifdef XFreeXDGA
   ,0,				/* int directMode */
   s3SetVidPage,		/* Set Vid Page */
   0,				/* unsigned long physBase */
   0				/* int physSize */
#endif
};

typedef struct S3PCIInformation {
   int DevID;
   int ChipType;
   int ChipRev;
   unsigned long MemBase;
} S3PCIInformation;

#if 0
static unsigned S3_IOPorts[] = { };
static int Num_S3_IOPorts = (sizeof(S3_IOPorts)/sizeof(S3_IOPorts[0]));
#endif

static SymTabRec s3DacTable[] = {
   { NORMAL_DAC,	"normal" },
   { S3_TRIO64_DAC,	"s3_trio64" },
   { -1,		"" },
};

static SymTabRec s3ChipTable[] = {
   { S3_UNKNOWN,	"unknown" },
   { S3_ViRGE,		"ViRGE" },
   { S3_ViRGE_VX,	"ViRGE/VX" },
   { S3_ViRGE_DXGX,	"ViRGE/DX or /GX" },
   { -1,		"" },
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
Bool  (*s3ClockSelectFunc) ();
static Bool s3ClockSelect();
static Bool s3GendacClockSelect();
ScreenPtr s3savepScreen;
Bool  s3Localbus = FALSE;
Bool  s3VLB = FALSE;
Bool  s3LinearAperture = FALSE;
Bool  s3NewMmio = TRUE;
Bool  s3PixelMultiplexing = FALSE;
Bool  s3DAC8Bit = FALSE;
Bool  s3DACSyncOnGreen = FALSE;
unsigned char s3LinApOpt;
unsigned char s3SAM256 = 0x00;
int s3BankSize;
int s3DisplayWidth;
pointer vgaBase = NULL;
pointer vgaBaseLow = NULL;
pointer vgaBaseHigh = NULL;
pointer s3VideoMem = NULL;
pointer s3MmioMem = NULL;
int s3Trio32FCBug = 0;
int s3_968_DashBug = 0;
unsigned long s3MemBase = 0;
int s3_gcmd = CMD_NOP;
int s3bltbug_width1, s3bltbug_width2;
Bool tmp_useSWCursor = FALSE;

extern Bool xf86Exiting, xf86Resetting, xf86ProbeFailed;
extern int  xf86Verbose;
int s3ScissR;

int s3ScissB;
unsigned char s3SwapBits[256];
unsigned char s3Port40;
unsigned char s3Port51;
unsigned char s3Port54;
unsigned char s3Port59 = 0x00;
unsigned char s3Port5A = 0x00;
unsigned char s3Port31 = 0x8d;
void (*s3ImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
void (*s3ImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, unsigned long
#endif
);
void (*s3ImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, int, unsigned long
#endif
);
int s3hotX, s3hotY;
Bool s3BlockCursor, s3ReloadCursor;
int s3CursorStartX, s3CursorStartY, s3CursorLines;
int s3RamdacType = UNKNOWN_DAC;
Bool s3UsingPixMux = FALSE;
Bool s3ATT498PixMux = FALSE;
static int maxRawClock = 0;
static Bool clockDoublingPossible = FALSE;
int s3AdjustCursorXPos = 0;
static int s3BiosVendor = UNKNOWN_BIOS;
static Bool in_s3Probe = TRUE;

#ifdef PC98
extern Bool	BoardInit();
extern int	pc98BoardType;
#endif

/*
 * s3PrintIdent -- print identification message
 */
void
s3PrintIdent()
{
  int i, j, n = 0, c = 0;
  char *id;

  ErrorF("  %s: accelerated server for S3 graphics adaptors (Patchlevel %s)\n",
	 s3InfoRec.name, s3InfoRec.patchLevel);

  ErrorF("      ");
  for (i = 0; s3Drivers[i]; i++)
    for (j = 0; (id = (s3Drivers[i]->ChipIdent)(j)); j++, n++)
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
#ifdef PC98
  ErrorF("  PC98: Supported Video Boards:\n\t%s\n",PC98_S3_BOARDS);
#endif
}


static unsigned char *find_bios_string(int BIOSbase, char *match1, char *match2)
{
#define BIOS_BSIZE 1024
#define BIOS_BASE  0xc0000

   static unsigned char bios[BIOS_BSIZE];
   static int init=0;
   int i,j,l1,l2;

   if (!init) {
      init = 1;
      if (xf86ReadBIOS(BIOSbase, 0, bios, BIOS_BSIZE) != BIOS_BSIZE)
	 return NULL;
      if ((bios[0] != 0x55) || (bios[1] != 0xaa))
	 return NULL;
   }
   if (match1 == NULL)
      return NULL;

   l1 = strlen(match1);
   if (match2 != NULL) 
      l2 = strlen(match2);
   else	/* for compiler-warnings */
      l2 = 0;

   for (i=0; i<BIOS_BSIZE-l1; i++)
      if (bios[i] == match1[0] && !memcmp(&bios[i],match1,l1))
	 if (match2 == NULL) 
	    return &bios[i+l1];
	 else
	    for(j=i+l1; (j<BIOS_BSIZE-l2) && bios[j]; j++) 
	       if (bios[j] == match2[0] && !memcmp(&bios[j],match2,l2))
		  return &bios[j+l2];
   return NULL;
}


/*
 * s3GetPCIInfo -- probe for PCI information
 */
S3PCIInformation *
s3GetPCIInfo()
{
   static S3PCIInformation info = {0, };
   pciConfigPtr pcrp, *pcrpp;
   Bool found = FALSE;
   int i = 0;

   pcrpp = xf86scanpci(s3InfoRec.scrnIndex);

   if (!pcrpp)
      return NULL;

   while ((pcrp = pcrpp[i])) {
      if (pcrp->_vendor == PCI_S3_VENDOR_ID) {
	 found = TRUE;
	 switch (pcrp->_device) {
	 case PCI_ViRGE:
	    info.ChipType = S3_ViRGE;
	    break;
	 case PCI_ViRGE_VX:
	    info.ChipType = S3_ViRGE_VX;
	    break;
	 case PCI_ViRGE_DXGX:
	    info.ChipType = S3_ViRGE_DXGX;
	    break;
	 default:
	    info.ChipType = S3_UNKNOWN;
	    info.DevID = pcrp->_device;
	    break;
	 }
	 info.ChipRev = pcrp->_rev_id;
	 info.MemBase = pcrp->_base0 & 0xFF800000;
#ifdef PC98_GA968
	 xf86writepci(s3InfoRec.scrnIndex, pcrp->_bus, pcrp->_cardnum,
			pcrp->_func,
			PCI_CMD_STAT_REG, PCI_CMD_MASK,
			PCI_CMD_IO_ENABLE | PCI_CMD_MEM_ENABLE);
#endif
	 break;
      }
      i++;
   }
   if (pcrp == NULL) return NULL;

   /* for new mmio we have to ensure that the PCI base address is
    * 64MB aligned and that there are no address collitions within 64MB.
    * S3 868/968 only pretend to need 32MB and thus fool
    * the BIOS PCI auto configuration :-(  */

   if (info.ChipType == S3_ViRGE) {
      unsigned long base0;
      char *probed;
      char map_64m[64];
      int j;

      if (s3InfoRec.MemBase == 0) {
	 base0  = info.MemBase;
	 probed = XCONFIG_PROBED;
      }
      else {
	 base0  = s3InfoRec.MemBase;
	 probed = XCONFIG_GIVEN;
      }

      /* map allocated 64MB blocks */
      for (j=0; j<64; j++) map_64m[j] = 0;
      map_64m[63] = 1;  /* don't use the last 64MB area */
      for (j=0; (pcrp = pcrpp[j]); j++) {
	 if (i != j) {
	    map_64m[ (pcrp->_base0 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base0+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base1 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base1+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base2 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base2+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base3 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base3+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base4 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base4+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base5 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base5+0x3ffffff) >> 26) & 0x3f] = 1;
	 }
      }

      /* check for 64MB alignment and free space */

      if ((base0 & 0x3ffffff) ||
	  map_64m[(base0 >> 26) & 0x3f] ||
	  map_64m[((base0+0x3ffffff) >> 26) & 0x3f]) {
	 for (j=63; j>=16 && map_64m[j]; j--);
	 info.MemBase = ((unsigned long)j) << 26;
	 ErrorF("%s %s: PCI: base address not correctly aligned or address conflict\n",
		probed, s3InfoRec.name);
	 ErrorF("\t\tbase address changed from 0x%08lx to 0x%08lx\n",
		base0, info.MemBase);
         xf86writepci(s3InfoRec.scrnIndex, pcrpp[i]->_bus, pcrpp[i]->_cardnum,
		    pcrpp[i]->_func, PCI_MAP_REG_START, ~0L,
		    info.MemBase | PCI_MAP_MEMORY | PCI_MAP_MEMORY_TYPE_32BIT);
      }
      s3Port59 = (info.MemBase >> 24) & 0xfc;
      s3Port5A = 0;
   }
   else {
      if (s3InfoRec.MemBase != 0) {
	 s3Port59 =  s3InfoRec.MemBase >> 24;
	 s3Port5A = (s3InfoRec.MemBase >> 16) & 0x08;
      }
      else {
	 s3Port59 =  info.MemBase >> 24;
	 s3Port5A = (info.MemBase >> 16) & 0x08;
      }
   }

   /* Free PCI information */
   xf86cleanpci();

   if (found && xf86Verbose) {
      if (info.ChipType != S3_UNKNOWN) {
	 ErrorF("%s %s: PCI: %s rev %x, Linear FB @ 0x%08lx\n", XCONFIG_PROBED,
		s3InfoRec.name, xf86TokenToString(s3ChipTable, info.ChipType),
		info.ChipRev, info.MemBase);
      } else {
	 ErrorF("%s %s: PCI: unknown (please report), ID 0x%04x rev %x,"
		" Linear FB @ 0x%08lx\n", XCONFIG_PROBED,
		s3InfoRec.name, info.DevID, info.ChipRev, info.MemBase);
      }
   }
   if (found)
      return &info;
   else
      return NULL;
}

/*
 * s3Probe -- probe and initialize the hardware driver
 */
/* moved out of s3Probe() so s3ValidMode() can see them (MArk)*/
static Bool pixMuxPossible = FALSE;
static Bool allowPixMuxInterlace = FALSE;
static Bool allowPixMuxSwitching = FALSE;
static int nonMuxMaxClock = 0;
static int nonMuxMaxMemory = 8192;
static int maxDisplayWidth;
static int maxDisplayHeight;
static int pixMuxMinWidth = 1024;
static int pixMuxMinClock = 0;

Bool
s3Probe()
{
   DisplayModePtr pMode, pEnd;
   unsigned char config, config2;
   int i, j, numClocks;
   int tx, ty;
   OFlagSet validOptions;
   char *card, *serno;
   int card_id, max_pix_clock, max_mem_clock, hwconf;
   int lookupFlags;
   int MemOffScreen = 0;
   unsigned char tmp;

   /*
    * These characterise a RAMDACs pixel multiplexing capabilities and
    * requirements:
    *
    *   pixMuxPossible         - pixmux is supported for the current RAMDAC
    *   allowPixMuxInterlace   - pixmux supports interlaced modes
    *   allowPixMuxSwitching   - possible to use pixmux for some modes
    *                            and non-pixmux for others
    *   pixMuxMinWidth         - smallest physical width supported in
    *                            pixmux mode
    *   nonMuxMaxClock         - highest dot clock supported without pixmux
    *   pixMuxMinClock         - lowest dot clock supported with pixmux
    *   nonMuxMaxMemory        - max video memory accessible without pixmux
    *   pixMuxLimitedWidths    - pixmux only works for logical display
    *                            widths 1024 and 2048
    *   pixMuxInterlaceOK      - FALSE if pixmux isn't possible because
    *                            there is an interlaced mode present
    *   pixMuxWidthOK          - FALSE if pixmux isn't possible because
    *                            there is mode has too small a width
    */
   Bool pixMuxPossible = FALSE;
   Bool allowPixMuxInterlace = FALSE;
   Bool allowPixMuxSwitching = FALSE;
   Bool pixMuxNeeded = FALSE;
   int pixMuxMinWidth = 1024;
   int nonMuxMaxClock = 0;
   int pixMuxMinClock = 0;
   int nonMuxMaxMemory = 8192;
   Bool pixMuxLimitedWidths = TRUE;
   Bool pixMuxInterlaceOK = TRUE;
   Bool pixMuxWidthOK = TRUE;
   S3PCIInformation *pciInfo = NULL;

#if !defined(PC98) || defined(PC98_GA968)
   /* Do general PCI probe first */
   pciInfo = s3GetPCIInfo();
   if (pciInfo && pciInfo->MemBase)
      s3MemBase = pciInfo->MemBase;
#endif

   xf86ClearIOPortList(s3InfoRec.scrnIndex);
   xf86AddIOPorts(s3InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
#if 0
   xf86AddIOPorts(s3InfoRec.scrnIndex, Num_S3_IOPorts, S3_IOPorts);
#endif

   /* Enable I/O access */
   xf86EnableIOPorts(s3InfoRec.scrnIndex);

#ifdef PC98
#ifdef PC98_PW
   pc98BoardType = PW;
#endif
#ifdef PC98_NEC
   pc98BoardType = NECWAB;
#endif
#ifdef PC98_PWLB
   pc98BoardType = PWLB;
#endif
#ifdef PC98_GA968
   pc98BoardType = GA968;
#endif

   if (OFLG_ISSET(OPTION_PCSKB, &s3InfoRec.options))
	pc98BoardType = PCSKB;
   if (OFLG_ISSET(OPTION_PCSKB4, &s3InfoRec.options))
	pc98BoardType = PCSKB4;
   if (OFLG_ISSET(OPTION_PCHKB, &s3InfoRec.options))
	pc98BoardType = PCHKB;
   if (OFLG_ISSET(OPTION_NECWAB, &s3InfoRec.options))
	pc98BoardType = NECWAB;
   if (OFLG_ISSET(OPTION_PW805I, &s3InfoRec.options))
	pc98BoardType = PW805I;
   if (OFLG_ISSET(OPTION_PWLB, &s3InfoRec.options))
	pc98BoardType = PWLB;
   if (OFLG_ISSET(OPTION_PW968, &s3InfoRec.options))
	pc98BoardType = PW968;
   ErrorF("   PC98:Board Type = %X \n",pc98BoardType);
   if(BoardInit() == FALSE)
	return(FALSE);
#endif

   vgaIOBase = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
   vgaCRIndex = vgaIOBase + 4;
   vgaCRReg = vgaIOBase + 5;

   outb(vgaCRIndex, 0x11);	/* for register CR11 */
   tmp = inb(vgaCRReg);         /* enable CR0-7 and disable interrupts */
   outb(vgaCRReg, tmp & 0x0f);

   outb(vgaCRIndex, 0x38);		/* check if we have an S3 */
   outb(vgaCRReg, 0x00);

   /* Make sure we can't write when locked */

   if (testinx2(vgaCRIndex, 0x35, 0x0f)) {
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }

   outb(vgaCRIndex, 0x38);	/* for register CR38, (REG_LOCK1) */
   outb(vgaCRReg, 0x48);	/* unlock S3 register set for read/write */

   /* Make sure we can write when unlocked */

   if (!testinx2(vgaCRIndex, 0x35, 0x0f)) {
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }

   outb(vgaCRIndex, 0x36);		/* for register CR36 (CONFG_REG1), */
   config = inb(vgaCRReg);		/* get amount of vram installed */

   outb(vgaCRIndex, 0x37);		/* for register CR37 (CONFG_REG2), */
   config2 = inb(vgaCRReg);		/* get amount of off-screen ram  */

   outb(vgaCRIndex, 0x30);
   s3ChipId = inb(vgaCRReg);         /* get chip id */

   s3ChipRev = s3ChipId & 0x0f;
   if (s3ChipId >= 0xe0) {
      outb(vgaCRIndex, 0x2d);
      s3ChipId = inb(vgaCRReg) << 8;
      outb(vgaCRIndex, 0x2e);
      s3ChipId |= inb(vgaCRReg);
      outb(vgaCRIndex, 0x2f);
      s3ChipRev = inb(vgaCRReg);
   }

   if (s3InfoRec.chipID) {
      ErrorF("%s %s: S3 chipset override, using chip_id = 0x%02x instead of 0x%02x\n",
	     XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.chipID, s3ChipId);
      s3ChipId = s3InfoRec.chipID;
   }
   if (s3InfoRec.chipRev) {
      ErrorF("%s %s: S3 chipset override, using chip_rev = %x instead of %x\n",
	     XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.chipRev, s3ChipRev);
      s3ChipRev = s3InfoRec.chipRev;
   }

   if (!S3_ANY_SERIES(s3ChipId)) {
      ErrorF("%s %s: Unknown S3 chipset: chip_id = 0x%02x rev. %x\n",
	     XCONFIG_PROBED,s3InfoRec.name,s3ChipId,s3ChipRev);
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }

   for (i = 0; s3Drivers[i]; i++) {
      if ((s3Drivers[i]->ChipProbe)()) {
	 xf86ProbeFailed = FALSE;
	 s3InfoRec.Init = s3Drivers[i]->ChipInitialize;
	 s3InfoRec.EnterLeaveVT = s3Drivers[i]->ChipEnterLeaveVT;
	 s3InfoRec.AdjustFrame = s3Drivers[i]->ChipAdjustFrame;
	 s3InfoRec.SwitchMode = s3Drivers[i]->ChipSwitchMode;
	 break;
      }
   }
   if (xf86ProbeFailed) {
      if (s3InfoRec.chipset) {
	 ErrorF("%s: '%s' is an invalid chipset", s3InfoRec.name,
		s3InfoRec.chipset);
      }
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }

   if (s3InfoRec.ramdac) {
      s3RamdacType = xf86StringToToken(s3DacTable, s3InfoRec.ramdac);
      if (s3RamdacType < 0) {
	 ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
		s3InfoRec.name, s3InfoRec.ramdac);
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return(FALSE);
      }
   }

   OFLG_ZERO(&validOptions);
   OFLG_SET(OPTION_CLKDIV2, &validOptions);
   OFLG_SET(OPTION_SW_CURSOR, &validOptions);
   OFLG_SET(OPTION_SHOWCACHE, &validOptions);
   OFLG_SET(OPTION_NO_FONT_CACHE, &validOptions);
   OFLG_SET(OPTION_NO_PIXMAP_CACHE, &validOptions);
   OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
   OFLG_SET(OPTION_DAC_6_BIT, &validOptions);
   OFLG_SET(OPTION_SYNC_ON_GREEN, &validOptions);
#ifdef PC98
   OFLG_SET(OPTION_PCSKB, &validOptions);
   OFLG_SET(OPTION_PCSKB4, &validOptions);
   OFLG_SET(OPTION_PCHKB, &validOptions);
   OFLG_SET(OPTION_NECWAB, &validOptions);
   OFLG_SET(OPTION_PW805I, &validOptions);
   OFLG_SET(OPTION_PWLB, &validOptions);
   OFLG_SET(OPTION_PW968, &validOptions);
   OFLG_SET(OPTION_EPSON_MEM_WIN, &validOptions);
   OFLG_SET(OPTION_PW_MUX, &validOptions);
   OFLG_SET(OPTION_NOINIT, &validOptions);
#endif
   OFLG_SET(OPTION_POWER_SAVER, &validOptions);
   OFLG_SET(OPTION_SLOW_DRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_EDODRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_VRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_DRAM_REFRESH, &validOptions);
   OFLG_SET(OPTION_FAST_VRAM, &validOptions);
   OFLG_SET(OPTION_FPM_VRAM, &validOptions);
   OFLG_SET(OPTION_EDO_VRAM, &validOptions);
   OFLG_SET(OPTION_EARLY_RAS_PRECHARGE, &validOptions);
   OFLG_SET(OPTION_LATE_RAS_PRECHARGE, &validOptions);
   xf86VerifyOptions(&validOptions, &s3InfoRec);

#ifdef PC98
   if (OFLG_ISSET(OPTION_PW_MUX, &s3InfoRec.options)) {
      OFLG_SET(OPTION_SPEA_MERCURY, &s3InfoRec.options);
    }
#endif


   /* ViRGE is always PCI (or VLB if ever?!),  ViRGE/VX is only PCI */
   s3Localbus = TRUE;

   if (S3_ViRGE_SERIES(s3ChipId) && s3ChipRev < 0x06) {
      if (config & 0x02) {
	 if (xf86Verbose)
	    ErrorF("%s %s: card type: PCI\n", XCONFIG_PROBED, s3InfoRec.name);
      } else {
	 if (xf86Verbose)
	    ErrorF("%s %s: card type: VLB\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	 s3VLB = TRUE;
      }
   }

   /* reset S3 graphics engine to avoid memory corruption */
   if (!S3_ViRGE_VX_SERIES(s3ChipId)) {
      outb(vgaCRIndex, 0x66);
      i = inb(vgaCRReg);
      outb(vgaCRReg, i |  0x02);
      usleep(10000);  /* wait a little bit... */
   }

   if (find_bios_string(s3InfoRec.BIOSbase,"S3 86C325",
			"MELCO WGP-VG VIDEO BIOS") != NULL) {
      if (s3BiosVendor == UNKNOWN_BIOS)
	 s3BiosVendor = MELCO_BIOS;
      if (xf86Verbose)
	 ErrorF("%s %s: MELCO BIOS found\n",
		XCONFIG_PROBED, s3InfoRec.name);
      if (s3InfoRec.MemClk <= 0)       s3InfoRec.MemClk       =  74000;
      if (s3InfoRec.dacSpeeds[0] <= 0) s3InfoRec.dacSpeeds[0] = 191500;
      if (s3InfoRec.dacSpeeds[1] <= 0) s3InfoRec.dacSpeeds[1] = 162500;
      if (s3InfoRec.dacSpeeds[2] <= 0) s3InfoRec.dacSpeeds[2] = 111500;
      if (s3InfoRec.dacSpeeds[3] <= 0) s3InfoRec.dacSpeeds[3] =  83500;
   }

   card_id = s3DetectELSA(s3InfoRec.BIOSbase, &card, &serno, &max_pix_clock,
			  &max_mem_clock, &hwconf);

   if (!S3_ViRGE_VX_SERIES(s3ChipId)) {
      outb(vgaCRIndex, 0x66);
      outb(vgaCRReg, i & ~0x02);  /* clear reset flag */
      usleep(10000);  /* wait a little bit... */
   }

   if (card_id > 0) {
      if (s3BiosVendor == UNKNOWN_BIOS)
	 s3BiosVendor = ELSA_BIOS;
      if (xf86Verbose)
         ErrorF("%s %s: card: %s, Ser.No. %s\n",
	        XCONFIG_PROBED, s3InfoRec.name, card, serno);
      xfree(card);
      xfree(serno);

      if (s3InfoRec.dacSpeeds[0] <= 0)
	 s3InfoRec.dacSpeeds[0] = max_pix_clock;

      do {
	 switch (card_id) {
	 default:
	    continue; /* unknown card_id, don't set ICD2061A flags */
	 }

	 /* a known ELSA card_id was returned, set ICD 2061A clock support
	    if there is no ClockChip specified in XF86Config */

	 if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
	    OFLG_SET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions);
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	    clockchip_probed = XCONFIG_PROBED;
	 }
      } while (0);
   }

   if (xf86Verbose) {
      char *chipname = "unknown";

      if (S3_ViRGE_SERIES(s3ChipId)) {
	 chipname = "ViRGE";
      }
      else if (S3_ViRGE_DXGX_SERIES(s3ChipId)) {
	 outb(vgaCRIndex, 0x39);
	 outb(vgaCRReg, 0xa5);
	 outb(vgaCRIndex, 0x6f);
	 if (inb(vgaCRReg) & 1)
	    chipname = "ViRGE/GX";
	 else
	    chipname = "ViRGE/DX";
      }
      else if (S3_ViRGE_VX_SERIES(s3ChipId)) {
	 chipname = "ViRGE/VX";
      }
      ErrorF("%s %s: chipset:   %s rev. %x\n",
	     XCONFIG_PROBED, s3InfoRec.name, chipname, s3ChipRev);
   }

   if (xf86Verbose) {
      ErrorF("%s %s: chipset driver: %s\n",
	     OFLG_ISSET(XCONFIG_CHIPSET, &s3InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED,
	     s3InfoRec.name, s3InfoRec.chipset);
   }

   if (!s3InfoRec.videoRam) {
      if (S3_ViRGE_VX_SERIES(s3ChipId)) {
	 switch((config2 & 0x60) >> 5) {
	 case 1:
	    MemOffScreen = 4 * 1024;
	    break;
	 case 2:
	    MemOffScreen = 2 * 1024;
	    break;
	 }
	 switch ((config & 0x60) >> 5) {
	 case 0:
	    s3InfoRec.videoRam = 2 * 1024;
	    break;
	 case 1:
	    s3InfoRec.videoRam = 4 * 1024;
	    break;
	 case 2:
	    s3InfoRec.videoRam = 6 * 1024;
	    break;
	 case 3:
	    s3InfoRec.videoRam = 8 * 1024;
	    break;
	 }
	 s3InfoRec.videoRam -= MemOffScreen;
      }
      else {
	 switch((config & 0xE0) >> 5) {
	 case 0:
	    s3InfoRec.videoRam = 4 * 1024;
	    break;
	 case 4:
	    s3InfoRec.videoRam = 2 * 1024;
	    break;
	 case 6:
#if 0
	    /*
	     * normally this setting is illegal for older ViRGE
	     * chips, but it is in fact used for example on the
	     * Elsa Gloria L board
	     */
	    if (S3_ViRGE_DXGX_SERIES(s3ChipId))
#endif
	       s3InfoRec.videoRam = 1 * 1024;
	    break;
	 }
      }

      if (xf86Verbose) {
	 if (MemOffScreen)
	    ErrorF("%s %s: videoram:  %dk (plus %dk off-screen)\n",
		   XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.videoRam,
		   MemOffScreen);
	 else
	    ErrorF("%s %s: videoram:  %dk\n",
		   XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.videoRam);
      }
   } else {
      if (xf86Verbose) {
	 ErrorF("%s %s: videoram:  %dk\n",
              XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.videoRam);
      }
   }

   if (xf86bpp < 0) {
      xf86bpp = s3InfoRec.depth;
   }
   if (xf86weight.red == 0 || xf86weight.green == 0 || xf86weight.blue == 0) {
      xf86weight = s3InfoRec.weight;
   }
   switch (xf86bpp) {
   case 8:
      break;
   case 15:
      s3InfoRec.depth = 15;
      xf86bpp = 16;
      s3Weight = RGB16_555;
      xf86weight.red = xf86weight.green = xf86weight.blue = 5;
s3InfoRec.weight.red = s3InfoRec.weight.green = s3InfoRec.weight.blue = 5;
      s3InfoRec.bitsPerPixel = 16;
      if (s3InfoRec.defaultVisual < 0)
	 s3InfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
	 defaultColorVisualClass = s3InfoRec.defaultVisual;
      break;
   case 16:
      if (xf86weight.red==5 && xf86weight.green==5 && xf86weight.blue==5) {
	 s3Weight = RGB16_555;
	 s3InfoRec.depth = 15;
s3InfoRec.weight.red = s3InfoRec.weight.green = s3InfoRec.weight.blue = 5;
      }
      else if (xf86weight.red==5 && xf86weight.green==6 && xf86weight.blue==5) {
	 s3Weight = RGB16_565;
	 s3InfoRec.depth = 16;
s3InfoRec.weight.red = s3InfoRec.weight.blue = 5;
s3InfoRec.weight.green = 6;
      }
      else {
	 ErrorF(
	   "Invalid color weighting %1d%1d%1d (only 555 and 565 are valid)\n",
	   xf86weight.red,xf86weight.green,xf86weight.blue);
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return(FALSE);
      }
      s3InfoRec.bitsPerPixel = 16;
      if (s3InfoRec.defaultVisual < 0)
	 s3InfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
	 defaultColorVisualClass = s3InfoRec.defaultVisual;
      break;
   case 24:
   case 32:
      xf86bpp = 32;
      s3InfoRec.depth = 24;	   /* actually 24 bpp */
      s3InfoRec.bitsPerPixel = 32; /* to work with things higher up */
      s3Weight = RGB32_888;
      /* s3MaxClock = S3_MAX_32BPP_CLOCK; */
      xf86weight.red =  xf86weight.green = xf86weight.blue = 8;
s3InfoRec.weight.red = s3InfoRec.weight.green = s3InfoRec.weight.blue = 8;
      s3InfoRec.defaultVisual = TrueColor;
      defaultColorVisualClass = s3InfoRec.defaultVisual;
      break;
   #if 0
   case 32:
      s3InfoRec.depth = 24;
      s3InfoRec.bitsPerPixel = 32; /* Use sparse 24 bpp (RGBX) */
      s3Weight = RGB32_888;
      /* s3MaxClock = S3_MAX_32BPP_CLOCK; */
      xf86weight.red =  xf86weight.green = xf86weight.blue = 8;
      if (s3InfoRec.defaultVisual < 0)
	 s3InfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
	 defaultColorVisualClass = s3InfoRec.defaultVisual;
      break;
   #endif
   default:
      ErrorF(
	"Invalid value for bpp.  Valid values are 8, 15, 16, and 24/32"
	/*", 24 and 32"*/
	".\n");
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }

   if (s3InfoRec.bitsPerPixel > 8 &&
       defaultColorVisualClass >= 0 && defaultColorVisualClass != TrueColor) {
      ErrorF("Invalid default visual type: %d (%s)\n", defaultColorVisualClass,
	     xf86VisualNames[defaultColorVisualClass]);
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }

   s3Bpp = xf86bpp / 8;
   realS3Bpp = s3Bpp;
   if (s3Bpp == 4) realS3Bpp = 3;

   if (S3_ANY_ViRGE_SERIES(s3ChipId)) {
      if (s3RamdacType != UNKNOWN_DAC && !DAC_IS_TRIO) {
	 ErrorF("%s %s: for ViRGE chips you shouldn't specify a Ramdac\n",
		XCONFIG_PROBED, s3InfoRec.name);
	 OFLG_CLR(XCONFIG_RAMDAC, &s3InfoRec.xconfigFlag);
      }
      if (!DAC_IS_TRIO) {
	 s3RamdacType = S3_TRIO64_DAC;
	 s3InfoRec.ramdac = xf86TokenToString(s3DacTable, s3RamdacType);
      }
      if (S3_ViRGE_DXGX_SERIES(s3ChipId)) {
	 if ( OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions)) {
	    ErrorF("%s %s: for ViRGE/DX/GX chips you shouldn't specify any Clockchip\n"
		   "\t other than \"s3_trio64v2\" or maybe \"s3_trio64\"\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	    /* Clear the other clock options */
	    OFLG_ZERO(&s3InfoRec.clockOptions);
	 }
	 if (S3_ViRGE_DXGX_SERIES(s3ChipId) &&
	     !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions)) {
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	    OFLG_SET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions);
	    clockchip_probed = XCONFIG_PROBED;
	 }
      }
      else
      if ( OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
	  !OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions)) {
	 ErrorF("%s %s: for ViRGE chips you shouldn't specify a Clockchip\n",
		XCONFIG_PROBED, s3InfoRec.name);
	 /* Clear the other clock options */
	 OFLG_ZERO(&s3InfoRec.clockOptions);
      }
      if (!OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions) && 
	  !OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions)) {
	 OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	 OFLG_SET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions);
	 clockchip_probed = XCONFIG_PROBED;
      }
   }

   /* If we still don't know the ramdac type, set it to NORMAL_DAC */
   if (s3RamdacType == UNKNOWN_DAC) {
      s3RamdacType = NORMAL_DAC;
   }

   /* make sure s3InfoRec.ramdac is set correctly */
   s3InfoRec.ramdac = xf86TokenToString(s3DacTable, s3RamdacType);

   /* Check Ramdac type is supported on the current S3 chipset */
   {
      char *chips = NULL;

      switch (s3RamdacType) {
      case BT485_DAC:
      case ATT20C505_DAC:
      case TI3020_DAC:
      case TI3025_DAC:
      case TI3026_DAC:
      case TI3030_DAC:
      case IBMRGB524_DAC:
      case IBMRGB525_DAC:
      case IBMRGB528_DAC:
      case ATT20C498_DAC:
      case ATT22C498_DAC:
      case ATT20C409_DAC:
      case STG1700_DAC:
      case STG1703_DAC:
      case S3_SDAC_DAC:
      case S3_GENDAC_DAC:
      case S3_TRIO32_DAC:
	 chips = "S3 chips other than S3 ViRGE";
	 break;
      case S3_TRIO64_DAC:
	 if (!S3_ANY_ViRGE_SERIES(s3ChipId))
	    chips = "ViRGE";
	 break;
      }
      if (chips) {
	 ErrorF("%s %s: Ramdac \"%s\" is only supported with %s\n",
		XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.ramdac, chips);
	 OFLG_CLR(XCONFIG_RAMDAC, &s3InfoRec.xconfigFlag);
	 /* Treat the ramdac as a "normal" dac */
	 s3RamdacType = NORMAL_DAC;
	 s3InfoRec.ramdac = xf86TokenToString(s3DacTable, s3RamdacType);
      }
   }

   if (S3_ANY_ViRGE_SERIES(s3ChipId)) {
      if (!DAC_IS_TRIO) {
	 ErrorF("%s %s: for ViRGE chips you shouldn't specify a Ramdac\n",
		XCONFIG_PROBED, s3InfoRec.name);
	 OFLG_CLR(XCONFIG_RAMDAC, &s3InfoRec.xconfigFlag);
	 s3RamdacType = S3_TRIO64_DAC;
	 s3InfoRec.ramdac = xf86TokenToString(s3DacTable, s3RamdacType);
      }
   }

   if (xf86Verbose) {
      ErrorF("%s %s: Ramdac type: %s\n",
	     OFLG_ISSET(XCONFIG_RAMDAC, &s3InfoRec.xconfigFlag) ?
	     XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.ramdac);
   }

   /* Now check/set the DAC speed */

   if (s3InfoRec.dacSpeeds[3] <= 0 && s3InfoRec.dacSpeeds[2] > 0) 
      s3InfoRec.dacSpeeds[3] = s3InfoRec.dacSpeeds[2];

   if (S3_ViRGE_VX_SERIES(s3ChipId)) {
      if (s3InfoRec.dacSpeeds[0] <= 0) s3InfoRec.dacSpeeds[0] = 220000;
      if (s3InfoRec.dacSpeeds[1] <= 0) s3InfoRec.dacSpeeds[1] = 220000;
      if (s3InfoRec.dacSpeeds[2] <= 0) s3InfoRec.dacSpeeds[2] = 135000;
      if (s3InfoRec.dacSpeeds[3] <= 0) s3InfoRec.dacSpeeds[3] = 135000;
   }
   else if (S3_ViRGE_DXGX_SERIES(s3ChipId)) {
      if (s3InfoRec.dacSpeeds[0] <= 0) s3InfoRec.dacSpeeds[0] = 170000;
      if (s3InfoRec.dacSpeeds[1] <= 0) s3InfoRec.dacSpeeds[1] = 170000;
      if (s3InfoRec.dacSpeeds[2] <= 0) s3InfoRec.dacSpeeds[2] = 135000;
      if (s3InfoRec.dacSpeeds[3] <= 0) s3InfoRec.dacSpeeds[3] = 135000;
   }
   else {
      if (s3InfoRec.dacSpeeds[0] <= 0) s3InfoRec.dacSpeeds[0] = 135000;
      if (s3InfoRec.dacSpeeds[1] <= 0) s3InfoRec.dacSpeeds[1] =  95000;
      if (s3InfoRec.dacSpeeds[2] <= 0) s3InfoRec.dacSpeeds[2] =  57000;
      if (s3InfoRec.dacSpeeds[3] <= 0) s3InfoRec.dacSpeeds[3] =  57000;
   }

   if (s3InfoRec.dacSpeedBpp <= 0)
/*
      if (xf86bpp > 24 && s3InfoRec.dacSpeeds[3] > 0)
	 s3InfoRec.dacSpeedBpp = s3InfoRec.dacSpeeds[3];
*/
      if (0) /* no real 32 bpp here */ ;
      else if (xf86bpp >= 24 && s3InfoRec.dacSpeeds[2] > 0)
	 s3InfoRec.dacSpeedBpp = s3InfoRec.dacSpeeds[2];
      else if (xf86bpp > 8 && xf86bpp < 24 && s3InfoRec.dacSpeeds[1] > 0)
	 s3InfoRec.dacSpeedBpp = s3InfoRec.dacSpeeds[1];
      else if (xf86bpp <= 8 && s3InfoRec.dacSpeeds[0] > 0)
	 s3InfoRec.dacSpeedBpp = s3InfoRec.dacSpeeds[0];

   if (xf86Verbose) {
      ErrorF("%s %s: Ramdac speed: %d MHz",
	     OFLG_ISSET(XCONFIG_DACSPEED, &s3InfoRec.xconfigFlag) ?
	     XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name,
	     s3InfoRec.dacSpeeds[0] / 1000);
      if (s3InfoRec.dacSpeedBpp != s3InfoRec.dacSpeeds[0])
	 ErrorF("  (%d MHz for %d bpp)",s3InfoRec.dacSpeedBpp / 1000, xf86bpp);
      ErrorF("\n");
   }

   /* Check when pixmux is supported */

   if (DAC_IS_TRIO)
	 if (S3_ViRGE_VX_SERIES(s3ChipId)) {
	    if (xf86bpp <= 16) s3ATT498PixMux = TRUE;
	 }
	 else
	    if (xf86bpp <=  8) s3ATT498PixMux = TRUE;

   if (s3ATT498PixMux) {
      pixMuxPossible = TRUE;
      if (DAC_IS_TRIO) {
	 if (S3_ViRGE_VX_SERIES(s3ChipId)) {
	    nonMuxMaxClock = 110000;  /* could be 135000 */
	    pixMuxMinClock = 110000;  /* could be 135000 */
	 } else {
	    nonMuxMaxClock = 80000;
	    pixMuxMinClock = 80000;
	 }
      }
      else {
	 nonMuxMaxClock = 67500;
	 pixMuxMinClock = 67500;
      }
      allowPixMuxInterlace = TRUE;
      allowPixMuxSwitching = TRUE;
      pixMuxLimitedWidths = FALSE;
      pixMuxMinWidth = 0;
   }

   /*
    * clock options are now done after the ramdacs because the next
    * generation ramdacs will have a built in clock (i.e. TI 3025)
    */

   if (OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions) ||
       OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions)) {
      unsigned char sr8;
      int m,n,n1,n2, mclk;

      s3ClockSelectFunc = s3GendacClockSelect;
      numClocks = 3;

      outb(0x3c4, 0x08);
      sr8 = inb(0x3c5);
      outb(0x3c5, 0x06);

      outb(0x3c4, 0x11);
      m = inb(0x3c5);
      outb(0x3c4, 0x10);
      n = inb(0x3c5);

      outb(0x3c4, 0x08);
      outb(0x3c5, sr8);

      m &= 0x7f;
      n1 = n & 0x1f;
      n2 = (n>>5) & 0x03;
      mclk = ((1431818 * (m+2)) / (n1+2) / (1 << n2) + 50) / 100;
      if (xf86Verbose)
	 ErrorF("%s %s: Using Trio32/64 programmable clock (MCLK %1.3f MHz)\n"
		,clockchip_probed, s3InfoRec.name
		,mclk / 1000.0);
      if (s3InfoRec.s3MClk > 0) {
	 if (xf86Verbose)
	    ErrorF("%s %s: using specified MCLK value of %1.3f MHz for DRAM timings\n",
		   XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.s3MClk / 1000.0);
      }
      else {
	 s3InfoRec.s3MClk = mclk;
      }
      if (s3InfoRec.MemClk > 0) {
	 /* some sanity checks */
	 if (s3InfoRec.MemClk < 40000 || s3InfoRec.MemClk > 100000) {
	    ErrorF("%s %s: MCLK %1.3f MHz too low/high, not changed!\n",
		   OFLG_ISSET(XCONFIG_DACSPEED, &s3InfoRec.xconfigFlag) ?
		   XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name, 
		   s3InfoRec.MemClk / 1000.0);
	    s3InfoRec.MemClk = 0;
	 }
	 else if (xf86Verbose)
	    ErrorF("%s %s: set MCLK to %1.3f MHz\n",
		   XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.MemClk / 1000.0);
      }
   } else {
      s3ClockSelectFunc = s3ClockSelect;
      numClocks = 16;
      if (!s3InfoRec.clocks)
         vgaGetClocks(numClocks, s3ClockSelectFunc);
   }

   /*
    * Set the maximum raw clock for programmable clock chips.
    * Setting maxRawClock to 0 means no clock-chip limit imposed.
    */
   if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions)) {
	 maxRawClock = 120000;
      } else if (OFLG_ISSET(CLOCK_OPTION_S3TRIO, &s3InfoRec.clockOptions) ||
		 OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions)) {
	 if (S3_ViRGE_VX_SERIES(s3ChipId))
	    maxRawClock = s3InfoRec.dacSpeeds[0];
	 else if (S3_ViRGE_DXGX_SERIES(s3ChipId))
	    maxRawClock = s3InfoRec.dacSpeeds[0];
	 else
	    maxRawClock = s3InfoRec.dacSpeeds[0];
      } else {
	 /* Shouldn't get here */
	 maxRawClock = 0;
      }
   } else {
      maxRawClock = 0;
   }

   /*
    * Set pixel clock limit based on RAMDAC type/speed/bpp and pixmux usage.
    * Also scale maxRawClock so that it can be compared with a pixel clock,
    * and re-adjust the pixel clock limit if required.
    */

   switch (s3RamdacType) {
   case S3_TRIO64_DAC:
      s3InfoRec.maxClock = s3InfoRec.dacSpeedBpp;
      break;
   default:
      /* For DACs we don't have special code for, keep this as a limit */
      s3InfoRec.maxClock = s3MaxClock;
   }
   /* Check that maxClock is not higher than dacSpeeds[0] */
   if (s3InfoRec.maxClock > s3InfoRec.dacSpeeds[0])
      s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];

   /* Check if this exceeds the clock chip's limit */
   if (clockDoublingPossible)
      maxRawClock *= 2;
   if (maxRawClock > 0 && s3InfoRec.maxClock > maxRawClock)
      s3InfoRec.maxClock = maxRawClock;


   if (xf86Verbose) {
      if (! OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
	 for (j = 0; j < s3InfoRec.clocks; j++) {
	    if ((j % 8) == 0) {
	       if (j != 0)
		  ErrorF("\n");
               ErrorF("%s %s: clocks:",
                OFLG_ISSET(XCONFIG_CLOCKS,&s3InfoRec.xconfigFlag) ?
                    XCONFIG_GIVEN : XCONFIG_PROBED ,
                s3InfoRec.name);
	    }
	    ErrorF(" %6.2f", (double)s3InfoRec.clock[j] / 1000.0);
         }
         ErrorF("\n");
      }
   }

   if (pixMuxPossible && s3InfoRec.videoRam > nonMuxMaxMemory)
      pixMuxNeeded = TRUE;

   /* Adjust s3InfoRec.clock[] when not using a programable clock chip */

   if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      Bool clocksChanged = FALSE;
      Bool numClocksChanged = FALSE;
      int newNumClocks = s3InfoRec.clocks;

      for (j = 0; j < s3InfoRec.clocks; j++) {
	 switch(s3RamdacType) {
	 case NORMAL_DAC:
	    /* only suports 8bpp -- nothing to do */
	    break;
	 case S3_TRIO64_DAC:
	    /*
	     * We should never get here since these have a programmable
	     * clock built in.
	     */
	    break;
	 default:
	    /* Do nothing */
	    break;
	 }
      }
      if (numClocksChanged)
	 s3InfoRec.clocks = newNumClocks;

      if (xf86Verbose && clocksChanged) {
	 ErrorF("%s %s: Effective pixel clocks available for depth %d:\n",
		XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.depth);
	 for (j = 0; j < s3InfoRec.clocks; j++) {
	    if ((j % 8) == 0) {
	       if (j != 0)
		  ErrorF("\n");
               ErrorF("%s %s: pixel clocks:", XCONFIG_PROBED, s3InfoRec.name);
	    }
	    ErrorF(" %6.2f", (double)s3InfoRec.clock[j] / 1000.0);
         }
         ErrorF("\n");
      }
   }

   /* At this point, the s3InfoRec.clock[] values are pixel clocks */

   maxDisplayWidth = 2048;
   /* S3 ViRGE has only 11 bit X/Y addresses, limiting to 2048 :-( */
   maxDisplayHeight = 2048 - 3; /* Cursor can take up to 3 lines */

   if (s3InfoRec.virtualX > maxDisplayWidth) {
      ErrorF("%s: Virtual width (%d) is too large.  Maximum is %d\n",
	     s3InfoRec.name, s3InfoRec.virtualX, maxDisplayWidth);
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return (FALSE);
   }
   if (s3InfoRec.virtualY > maxDisplayHeight) {
      ErrorF("%s: Virtual height (%d) is too large.  Maximum is %d\n",
	     s3InfoRec.name, s3InfoRec.virtualY, maxDisplayHeight);
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return (FALSE);
   }

   lookupFlags = LOOKUP_DEFAULT;

redo_mode_lookup:

   tx = s3InfoRec.virtualX;
   ty = s3InfoRec.virtualY;
   pMode = s3InfoRec.modes;
   if (pMode == NULL) {
      ErrorF("No modes supplied in XF86Config\n");
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return (FALSE);
   }
   pEnd = NULL;
   do {
      DisplayModePtr pModeSv;

      pModeSv = pMode->next;
      /*
       * xf86LookupMode returns FALSE if it ran into an invalid
       * parameter
       */
      if (!xf86LookupMode(pMode, &s3InfoRec, lookupFlags)) {
	 xf86DeleteMode(&s3InfoRec, pMode);
      } else if (pMode->HDisplay > maxDisplayWidth) {
	 ErrorF("%s %s: Width of mode \"%s\" is too large (max is %d)\n",
		XCONFIG_PROBED, s3InfoRec.name, pMode->name, maxDisplayWidth);
	 xf86DeleteMode(&s3InfoRec, pMode);
      } else if (pMode->VDisplay > maxDisplayHeight) {
	 ErrorF("%s %s: Height of mode \"%s\" is too large (max is %d)\n",
		XCONFIG_PROBED, s3InfoRec.name, pMode->name, maxDisplayHeight);
	 xf86DeleteMode(&s3InfoRec, pMode);
      } else if ((pMode->HDisplay * (1 + pMode->VDisplay) * realS3Bpp/*s3Bpp*/) >
		 s3InfoRec.videoRam * 1024) {
	 ErrorF("%s %s: Too little memory for mode \"%s\"\n", XCONFIG_PROBED,
		s3InfoRec.name, pMode->name);
	 if (!OFLG_ISSET(OPTION_SW_CURSOR,   &s3InfoRec.options))
	 ErrorF("%s %s: NB. 1 scan line is required for the hardware cursor\n",
	        XCONFIG_PROBED, s3InfoRec.name);
	 xf86DeleteMode(&s3InfoRec, pMode);
      } else if (((tx > 0) && (pMode->HDisplay > tx)) ||
		 ((ty > 0) && (pMode->VDisplay > ty))) {
	 ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
		XCONFIG_PROBED, s3InfoRec.name,
		pMode->HDisplay, pMode->VDisplay, tx, ty);
	 xf86DeleteMode(&s3InfoRec, pMode);
      } else {
	 /*
	  * Successfully looked up this mode.  If pEnd isn't
	  * initialized, set it to this mode.
	  */
	 if (pEnd == (DisplayModePtr) NULL)
	    pEnd = pMode;

	 s3InfoRec.virtualX = max(s3InfoRec.virtualX, pMode->HDisplay);
	 s3InfoRec.virtualY = max(s3InfoRec.virtualY, pMode->VDisplay);

	 /*
	  * Check what impact each mode has on pixel multiplexing,
	  * and mark those modes for which pixmux must be used.
	  */
	 if (pixMuxPossible) {
	    if (s3Bpp == 1 && s3ATT498PixMux
		&& !OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,
			       &s3InfoRec.clockOptions)) {
	       if (pMode->Clock > 15) {
		  pMode->Flags |= V_PIXMUX;
		  pixMuxNeeded = TRUE;
	       }
	    }
	    else if (s3InfoRec.clock[pMode->Clock] > nonMuxMaxClock) {
	       pMode->Flags |= V_PIXMUX;
	       pixMuxNeeded = TRUE;
	    }
	    if (s3InfoRec.videoRam > nonMuxMaxMemory)
	       pMode->Flags |= V_PIXMUX;
	    /* XXXX this needs some changes */
	    if ((OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
		 OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) &&
		s3InfoRec.virtualX * s3InfoRec.virtualY > 2*1024*1024) {
	      /* PIXMUX must be used to access more than 2mb memory. */
	      pMode->Flags |= V_PIXMUX;
	      pixMuxNeeded = TRUE;
	    }

	    /*
	     * Check if pixmux can't be used.  There are two cases:
	     *
	     *   1. No switching between mux and non-mux modes.  In this case
	     *      the presence of any mode which can't be used in pixmux
	     *      mode is flagged.
	     *   2. Switching allowed.  In this cases the presence of modes
	     *      which require mux for one feature, but can't use it
	     *      because of another is flagged.
	     */
	    if (!allowPixMuxSwitching || (pMode->Flags & V_PIXMUX)) {
	       if (pMode->HDisplay < pixMuxMinWidth)
		  pixMuxWidthOK = FALSE;
	       if ((pMode->Flags & V_INTERLACE) && !allowPixMuxInterlace)
		  pixMuxInterlaceOK = FALSE;
	    }
	 }
      }
      pMode = pModeSv;
   } while (pMode != pEnd);

   if ((tx != s3InfoRec.virtualX) || (ty != s3InfoRec.virtualY))
      OFLG_CLR(XCONFIG_VIRTUAL,&s3InfoRec.xconfigFlag);

   /*
    * Are we using pixel multiplexing, or does the mode combination mean
    * we can't continue.  Note, this is a case we can't really deal with
    * by deleting modes -- there is no unique choice of modes to delete,
    * so let the user deal with it.
    */
   if (pixMuxPossible && pixMuxNeeded) {
      if (!pixMuxWidthOK) {
	 pMode = s3InfoRec.modes;
	 pEnd = NULL;
	 do {
	    DisplayModePtr pModeSv;

	    pModeSv = pMode->next;

	    if (pMode->HDisplay < pixMuxMinWidth) {
	       xf86DeleteMode(&s3InfoRec, pMode);
	    } else {
	       if (pEnd == (DisplayModePtr) NULL)
		  pEnd = pMode;
	    }

	    pMode = pModeSv;
	 } while (pMode != pEnd);
	 if (s3InfoRec.videoRam > nonMuxMaxMemory) {
	    ErrorF("%s %s: To access more than %dkB video memory the RAMDAC "
		   "must\n", XCONFIG_PROBED, s3InfoRec.name, nonMuxMaxMemory);
	    ErrorF("\toperate in pixel multiplex mode, but pixel "
		   "multiplexing\n");
	    ErrorF("\tcannot be used for modes of width less than %d.\n",
		   pixMuxMinWidth);
	    ErrorF("\tAdjust the Modes and/or VideoRam and Virtual lines in\n");
	    ErrorF("\tyour XF86Config to meet these requirements\n");
	 } else {
	    if (nonMuxMaxClock > 0) {
	       ErrorF("%s %s: Modes with a dot-clock above %dMHz require the "
		      "RAMDAC to\n", XCONFIG_PROBED, s3InfoRec.name,
		      nonMuxMaxClock / 1000);
	       ErrorF("\toperate in pixel multiplex mode, but pixel "
		      "multiplexing\n");
	       ErrorF("\tcannot be used for modes with width less than %d.\n",
		      pixMuxMinWidth);
	    } else {
	       ErrorF("%s %s: The RAMDAC must operate in pixel multiplex "
		      "mode,\n", XCONFIG_PROBED, s3InfoRec.name);
	       ErrorF("\tbut pixel multiplexing cannot be used for modes\n");
	       ErrorF("\twith width less than %d.\n", pixMuxMinWidth);
	    }
	    ErrorF("\tAdjust the Modes line in your XF86Config to meet these ");
	    ErrorF("\trequirements.\n");
	 }
      }
      if (!pixMuxInterlaceOK) {
	 if (s3InfoRec.videoRam > nonMuxMaxMemory) {
	    ErrorF("%s %s: To access more than %dkB video memory the RAMDAC "
		   "must\n", XCONFIG_PROBED, s3InfoRec.name, nonMuxMaxMemory);
	    ErrorF("\toperate in pixel multiplex mode, but pixel "
		   "multiplexing\n");
	    ErrorF("\tcannot be used for interlaced modes.\n");
#if 1
	    ErrorF("\tRescanning modes with interlaced modes elimintated.\n");
#else
	    ErrorF("Adjust the Modes and/or VideoRam and Virtual lines in\n");
	    ErrorF("your XF86Config to meet these requirements\n");
#endif
	 } else {
	    if (nonMuxMaxClock > 0) {
	       ErrorF("%s %s: Modes with a dot-clock above %dMHz require the "
		      "RAMDAC to\n", XCONFIG_PROBED, s3InfoRec.name,
		      nonMuxMaxClock / 1000);
	       ErrorF("\toperate in pixel multiplex mode, but pixel "
		      "multiplexing\n");
	       ErrorF("\tcannot be used for interlaced modes.\n");
	    } else {
	       ErrorF("%s %s: The RAMDAC must operate in pixel multiplex "
		      "mode,\n", XCONFIG_PROBED, s3InfoRec.name);
	       ErrorF("\tbut pixel multiplexing cannot be used for interlaced "
		      "modes.\n");
	    }
#if 1
	    ErrorF("\tRescanning modes with interlaced modes elimintated.\n");
#else
	    ErrorF("Adjust the Modes line in your XF86Config to meet these ");
	    ErrorF("requirements.\n");
#endif
	 }
	 pixMuxInterlaceOK = TRUE;
	 lookupFlags = LOOKUP_NO_INTERLACED;
	 goto redo_mode_lookup;
      }
#if 0
      if (!pixMuxWidthOK || !pixMuxInterlaceOK) {
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return(FALSE);
      } else
#endif
      {
	 if (xf86Verbose)
	    ErrorF("%s %s: Operating RAMDAC in pixel multiplex mode\n",
		   XCONFIG_PROBED, s3InfoRec.name);
	 s3UsingPixMux = TRUE;
      }
   }

   if (s3UsingPixMux && !allowPixMuxSwitching) {
      /* Mark all modes as V_PIXMUX */
      pEnd = pMode = s3InfoRec.modes;
      do {
	 pMode->Flags |= V_PIXMUX;
         pMode = pMode->next;
      } while (pMode != pEnd);
   }

   /*
    * For programmable clocks, fill in the SynthClock value
    * and set V_DBLCLK as required for each mode
    */

   if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      /* First just copy the pixel values */
      pEnd = pMode = s3InfoRec.modes;
      do {
	 pMode->SynthClock = s3InfoRec.clock[pMode->Clock];
	 pMode = pMode->next;
      } while (pMode != pEnd);
      /* Now make adjustments */
      pEnd = pMode = s3InfoRec.modes;
      do {
	 switch(s3RamdacType) {
	 case NORMAL_DAC:
	    /* only suports 8bpp -- nothing to do */
	    break;
	 case S3_TRIO64_DAC:
	    switch (s3Bpp) {
	    case 1:
#if 0
	       /* XXXX pMode->SynthClock /= 2 might be better with sr15 &= ~0x40
		  in s3init.c if screen wouldn't completely blank... */
	       if (pMode->SynthClock > nonMuxMaxClock) {
		  pMode->SynthClock /= 2;
		  pMode->Flags |= V_DBLCLK;
	       }
#endif
	       break;
	    case 2:
	    case 4:
	       /* No change for 16bpp and 24bpp */
	       break;
	    }
	    break;
	 default:
	    /* Do nothing */
	    break;
	 }
	 pMode = pMode->next;
      } while (pMode != pEnd);
   }

   pEnd = pMode = s3InfoRec.modes;
   do {
      /* Setup the Mode.Private if required */
      if (!pMode->PrivSize || !pMode->Private) {
	 pMode->PrivSize = S3_MODEPRIV_SIZE;
	 pMode->Private = (INT32 *)xcalloc(sizeof(INT32), S3_MODEPRIV_SIZE);
	 pMode->Private[0] = 0;
      }

      /* Set default for invert_vclk */
      if (!(pMode->Private[0] & (1 << S3_INVERT_VCLK))) {
	 pMode->Private[S3_INVERT_VCLK] = 0;
	 pMode->Private[0] |= 1 << S3_INVERT_VCLK;
      }

      /* Set default for blank_delay */
      if (!(pMode->Private[0] & (1 << S3_BLANK_DELAY))) {
	 pMode->Private[0] |= (1 << S3_BLANK_DELAY);
	 if (S3_ViRGE_VX_SERIES(s3ChipId))
	    if (s3Bpp == 1)
	       pMode->Private[S3_BLANK_DELAY] = 0x20;
	    else if (s3Bpp == 2)
	       pMode->Private[S3_BLANK_DELAY] = 0x31;
	    else
	       pMode->Private[S3_BLANK_DELAY] = 0x70;
	 else
	    if (s3Bpp == 1)
	       pMode->Private[S3_BLANK_DELAY] = 0x00;
	    else if (s3Bpp == 2)
	       pMode->Private[S3_BLANK_DELAY] = 0x02;
	    else
	       pMode->Private[S3_BLANK_DELAY] = 0x04;
      }

      /* Set default for early_sc */
      if (!(pMode->Private[0] & (1 << S3_EARLY_SC))) {
	 pMode->Private[0] |= 1 << S3_EARLY_SC;
	 pMode->Private[S3_EARLY_SC] = 0;
      }

      pMode = pMode->next;
   } while (pMode != pEnd);

   if (S3_ViRGE_VX_SERIES(s3ChipId)) {
      if (!OFLG_ISSET(OPTION_DAC_6_BIT, &s3InfoRec.options) || s3Bpp > 1)
	 s3DAC8Bit = TRUE;
   }

   if (OFLG_ISSET(OPTION_DAC_8_BIT, &s3InfoRec.options) && !s3DAC8Bit) {
      ErrorF("%s %s: Option \"dac_8_bit\" not recognised for RAMDAC \"%s\"\n",
	     XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.ramdac);
   }

   if (xf86Verbose) {
      if (s3InfoRec.bitsPerPixel == 8)
	 ErrorF("%s %s: Using %d bits per RGB value\n",
		OFLG_ISSET(OPTION_DAC_8_BIT, &s3InfoRec.options) ||
		OFLG_ISSET(OPTION_DAC_6_BIT, &s3InfoRec.options) ?
		XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name,
		s3DAC8Bit ?  8 : 6);
      else if (s3InfoRec.bitsPerPixel == 16)
	 ErrorF("%s %s: Using 16 bpp.  Color weight: %1d%1d%1d\n",
		XCONFIG_GIVEN, s3InfoRec.name, xf86weight.red,
		xf86weight.green, xf86weight.blue);
      else if (realS3Bpp == 3)
	 ErrorF("%s %s: Using packed 24 bpp.  Color weight: %1d%1d%1d\n",
		XCONFIG_GIVEN, s3InfoRec.name, xf86weight.red,
		xf86weight.green, xf86weight.blue);
      else if (s3InfoRec.bitsPerPixel == 32)
	 ErrorF("%s %s: Using sparse 32 bpp.  Color weight: %1d%1d%1d\n",
		XCONFIG_GIVEN, s3InfoRec.name, xf86weight.red,
		xf86weight.green, xf86weight.blue);
   }

   /* Select the appropriate logical line width */
   if (s3UsingPixMux && pixMuxLimitedWidths) {
      if (s3InfoRec.virtualX <= 1024) {
	 s3DisplayWidth = 1024;
      } else if (s3InfoRec.virtualX <= 2048) {
	 s3DisplayWidth = 2048;
      } else { /* should never get here */
	 ErrorF("Internal error in DisplayWidth check, virtual width = %d\n",
	        s3InfoRec.virtualX);
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return (FALSE);
      }
   } else {
      if (s3InfoRec.virtualX <= 640) {
	 s3DisplayWidth = 640;
      } else if (s3InfoRec.virtualX <= 800) {
	 s3DisplayWidth = 800;
      } else if (s3InfoRec.virtualX <= 1024) {
	 s3DisplayWidth = 1024;
      } else if (s3InfoRec.virtualX <= 1152) {
	 s3DisplayWidth = 1152;
      } else if (s3InfoRec.virtualX <= 1280) {
	 s3DisplayWidth = 1280;
      } else if (s3InfoRec.virtualX <= 1600) {
	 s3DisplayWidth = 1600;
      } else if (s3InfoRec.virtualX <= 2048) {
	 s3DisplayWidth = 2048;
      } else { /* should never get here */
	 ErrorF("Internal error in DisplayWidth check, virtual width = %d\n",
	        s3InfoRec.virtualX);
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return (FALSE);
      }
   }
   s3BppDisplayWidth = realS3Bpp /*s3Bpp*/ * s3DisplayWidth;
   /*
    * Work out where to locate S3's HW cursor storage.  It must be on a
    * 1k boundary.  We now set the cursor at the top of video memory -1K
    * to fix some problems on OS/2. SM[07/03/97]
    */

   if (OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options)) {
      s3CursorStartY = s3InfoRec.virtualY;
      s3CursorLines = 0;
   } else {
      int st_addr = s3InfoRec.videoRam * 1024 - 1024;
      s3CursorStartX = st_addr % s3BppDisplayWidth;
      s3CursorStartY = st_addr / s3BppDisplayWidth;
      s3CursorLines = ((s3CursorStartX + 1023) / s3BppDisplayWidth) + 1;
   }

   /*
    * Reduce the videoRam value if necessary to prevent Y coords exceeding
    * the 12-bit (4096) limit when small display widths are used on cards
    * with a lot of memory
    */
   if (s3InfoRec.videoRam * 1024 / s3BppDisplayWidth > 4096) {
      s3InfoRec.videoRam = s3BppDisplayWidth * 4096 / 1024;
      ErrorF("%s %s: videoram usage reduced to %dk to avoid co-ord overflow\n",
	     XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.videoRam);
   }

   /*
    * This one is difficult to deal with by deleting modes.  Do you delete
    * modes to reduce the vertical size or to reduce the displayWidth?  Either
    * way it will require the recalculation of everything above.  This one
    * is in the too-hard basket.
    */
   if ((s3BppDisplayWidth * s3InfoRec.virtualY) >
       (s3InfoRec.videoRam * 1024 - 1024)) { /* XXXX improve this message */
      ErrorF("%s %s: Display size %dx%d is too large: ",
             OFLG_ISSET(XCONFIG_VIRTUAL,&s3InfoRec.xconfigFlag) ?
                 XCONFIG_GIVEN : XCONFIG_PROBED,
             s3InfoRec.name,
	     s3DisplayWidth, s3InfoRec.virtualY);
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return (FALSE);
   }
   if (xf86Verbose) {
      ErrorF("%s %s: Virtual resolution set to %dx%d\n",
             OFLG_ISSET(XCONFIG_VIRTUAL,&s3InfoRec.xconfigFlag) ?
                 XCONFIG_GIVEN : XCONFIG_PROBED,
             s3InfoRec.name,
	     s3InfoRec.virtualX, s3InfoRec.virtualY);
   }

#ifdef DPMSExtension
   if (DPMSEnabledSwitch ||
       (OFLG_ISSET(OPTION_POWER_SAVER, &s3InfoRec.options) &&
	!DPMSDisabledSwitch))
      defaultDPMSEnabled = DPMSEnabled = TRUE;
#endif

   if (! (s3Port59 | s3Port5A)) { /* s3Port59/s3Port5A not yet initialized */
      if (s3InfoRec.MemBase != 0) {
	 s3Port59 =  s3InfoRec.MemBase >> 24;
	 s3Port5A = (s3InfoRec.MemBase >> 16) & 0x08;
      }
      else {
	 outb(vgaCRIndex, 0x59);
	 s3Port59 = inb(vgaCRReg);
	 outb(vgaCRIndex, 0x5a);
	 s3Port5A = inb(vgaCRReg);
      }
   }


#ifdef XFreeXDGA
      s3InfoRec.displayWidth = s3DisplayWidth;
      s3InfoRec.directMode = XF86DGADirectPresent;
#endif
   in_s3Probe = FALSE;
   return TRUE;
}

#ifdef PC98
void
#if NeedFunctionPrototypes
s3ConnectPCI(CARD16 vendor, CARD16 device)
#else
s3ConnectPCI(vendor, device)
    CARD16 vendor;
    CARD16 device;
#endif
{
    pciConfigPtr pcrp, *pcrpp;
    unsigned int dev;

    pcrpp = xf86scanpci(s3InfoRec.scrnIndex);

    if (!pcrpp)
	return;

    for (dev = 0; (pcrp = pcrpp[dev]) != NULL; dev ++)
    {
	if (pcrp->_vendor == vendor && pcrp->_device == device)
	{
	    xf86writepci(s3InfoRec.scrnIndex, pcrp->_bus, pcrp->_cardnum,
		pcrp->_func,
		PCI_CMD_STAT_REG, PCI_CMD_MASK,
		PCI_CMD_IO_ENABLE | PCI_CMD_MEM_ENABLE);
	    break;
	}
    }

    xf86cleanpci();
    return;
}

void
#if NeedFunctionPrototypes
s3DisconnectPCI(CARD16 vendor, CARD16 device)
#else
s3DisconnectPCI(vendor, device)
    CARD16 vendor;
    CARD16 device;
#endif
{
    pciConfigPtr pcrp, *pcrpp;
    unsigned int dev;

    pcrpp = xf86scanpci(s3InfoRec.scrnIndex);

    if (!pcrpp)
	return;

    for (dev = 0; (pcrp = pcrpp[dev]) != NULL; dev ++)
    {
	if (pcrp->_vendor == vendor && pcrp->_device == device)
	{
	    xf86writepci(s3InfoRec.scrnIndex, pcrp->_bus, pcrp->_cardnum,
		pcrp->_func,
		PCI_CMD_STAT_REG, PCI_CMD_MASK,
		0);
	    break;
	}
    }

    xf86cleanpci();
    return;
}
#endif

static Bool
s3ClockSelect(no)
     int   no;

{
   unsigned char temp;
   static unsigned char save1, save2;

   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xa5);

   switch(no)
   {
   case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaCRIndex, 0x42);
      save2 = inb(vgaCRReg);
      break;
   case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg, save2);
      break;
   default:
      no &= 0xF;
      if (no == 0x03)
      {
	 /*
	  * Clock index 3 is a 0Hz clock on all the S3-recommended
	  * synthesizers (except the Chrontel).  A 0Hz clock will lock up
	  * the chip but good (requiring power to be cycled).  Nuke that.
	  */
	 return(FALSE);
      }
      temp = inb(0x3CC);
      outb(0x3C2, temp | 0x0C);
      outb(vgaCRIndex, 0x42);
      temp = inb(vgaCRReg) & 0xf0;
      outb(vgaCRReg, temp | no);
      usleep(150000);
   }
   return(TRUE);
}



/* The GENDAC code also works for the SDAC, used for Trio32/Trio64 too */

static Bool
s3GendacClockSelect(freq)
     int   freq;

{
   Bool result = TRUE;

   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xa5);

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = s3ClockSelect(freq);
      break;
   default:
      {
#if defined(PC98_PW)
	 (void) S3gendacSetClock(freq, 7);  /* PW805i can't use reg 2 */
#else
	 
	 if (OFLG_ISSET(CLOCK_OPTION_S3TRIO64V2, &s3InfoRec.clockOptions))
	    (void) S3Trio64V2SetClock(freq, 2); /* can't fail */
	 else if (S3_ViRGE_VX_SERIES(s3ChipId))
	    (void) S3ViRGE_VXSetClock(freq, 2); /* can't fail */
	 else
	    (void) S3TrioSetClock(freq, 2); /* can't fail */
#endif
	 outb(vgaCRIndex, 0x42);/* select the clock */
#if defined(PC98_PW)
	 tmp = inb(vgaCRReg) & 0xf0;
	 outb(vgaCRReg, tmp | 0x07);
#endif
	 usleep(150000);
      }
   }
   return(result);
}



/*
 * Quick way to get s3ValidMode actually doing something
 * so the new vidmode extension functions can use it.
 * s3Probe() should be altered to use this... eventually.
 *			MArk (mvojkovi@ucsd.edu)
 */
static int
s3ValidMode(DisplayModePtr pMode, Bool verbose, int flag)
{
    Bool ModeCantPixmux = FALSE;

    if(in_s3Probe)
	return MODE_OK;

    /* Trivial size tests */
    if(pMode->HDisplay > maxDisplayWidth) {
	if(verbose)
	   ErrorF("%s %s: Width of mode \"%s\" is too large (max is %d)\n",
		XCONFIG_PROBED, s3InfoRec.name, pMode->name, maxDisplayWidth);
	return MODE_BAD;
    } else if(pMode->VDisplay > maxDisplayHeight) {
	if(verbose)
	   ErrorF("%s %s: Height of mode \"%s\" is too large (max is %d)\n",
		XCONFIG_PROBED, s3InfoRec.name, pMode->name, maxDisplayHeight);
	return MODE_BAD;
    } else if((pMode->HDisplay * (1 + pMode->VDisplay) * realS3Bpp/*s3Bpp*/) >
		 s3InfoRec.videoRam * 1024) {
	if(verbose)
	   ErrorF("%s %s: Too little memory for mode \"%s\"\n", XCONFIG_PROBED,
		s3InfoRec.name, pMode->name);
	if (!OFLG_ISSET(OPTION_SW_CURSOR,   &s3InfoRec.options))
	if(verbose)
	   ErrorF("%s %s: NB. 1 scan line is required for the hardware "
		"cursor\n", XCONFIG_PROBED, s3InfoRec.name);
	return MODE_BAD;
    } else if(((s3InfoRec.virtualX > 0) &&
	       (pMode->HDisplay > s3InfoRec.virtualX)) ||
	       ((s3InfoRec.virtualY > 0) &&
	       (pMode->VDisplay > s3InfoRec.virtualY))) {
	if(verbose)
	  ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
		XCONFIG_PROBED, s3InfoRec.name, pMode->HDisplay,
		pMode->VDisplay, s3InfoRec.virtualX, s3InfoRec.virtualY);
	return MODE_BAD;
    }

    if (pixMuxPossible) {

	/* Find out if the mode requires pixmux */

 	if (s3Bpp == 1 && s3ATT498PixMux
		&& !OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,
			       &s3InfoRec.clockOptions)) {
	       if (pMode->Clock > 15) {
		  pMode->Flags |= V_PIXMUX;
	       }
	}
	else if (s3InfoRec.clock[pMode->Clock] > nonMuxMaxClock) {
	        pMode->Flags |= V_PIXMUX;
	}
	if (s3InfoRec.videoRam > nonMuxMaxMemory) {
	       pMode->Flags |= V_PIXMUX;
	}
	if ((OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
		 OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) &&
		s3InfoRec.virtualX * s3InfoRec.virtualY > 2*1024*1024) {
	      /* PIXMUX must be used to access more than 2mb memory. */
	      pMode->Flags |= V_PIXMUX;
	}

	/* Find out if the mode can't be used with pixmux */

	if (pMode->HDisplay < pixMuxMinWidth)
	    ModeCantPixmux = TRUE;

	if ((pMode->Flags & V_INTERLACE) && !allowPixMuxInterlace)
	    ModeCantPixmux = TRUE;

	if (s3InfoRec.clock[pMode->Clock] < pixMuxMinClock)
	    ModeCantPixmux = TRUE;

	if (!allowPixMuxSwitching ) {

	    /* set V_PIXMUX if s3UsingPixMux and the mode can do pimux */

	    if (s3UsingPixMux && !ModeCantPixmux)
		pMode->Flags |= V_PIXMUX;

	    /*
	     * If switching between pixmux and non-pixmux isn't allowed, the
	     * mode is rejected when:
	     *
	     *   1. The initial mode set contains no modes requiring pixmux,
	     *      but this one needs it.
	     *
	     *   2. The initial mode set contains modes requiring pixmux,
	     *      and this one can't use pixmux.
	     */

	    if (s3UsingPixMux && ModeCantPixmux) {
		if (verbose) {
		   ErrorF("%s %s: Mode \"%s\" can't work with pixel "
			"multiplexing and is\n",
			XCONFIG_PROBED, s3InfoRec.name, pMode->name);
		   ErrorF("\tincompatible with the current modes.\n");
		}
		return MODE_BAD;
	    }

	    if (!s3UsingPixMux && (pMode->Flags & V_PIXMUX)) {
		if (verbose) {
		   ErrorF("%s %s: Mode \"%s\" requires pixel multiplexing "
			"and is\n", XCONFIG_PROBED, s3InfoRec.name,
			pMode->name);
		   ErrorF("\tincompatible with in the current mode.\n");
		}
		return MODE_BAD;
	    }
	}

    }  /* pixMuxPossible */


   /*
    * For programmable clocks, fill in the SynthClock value
    * and set V_DBLCLK as required for each mode
    */

   pMode->SynthClock = s3InfoRec.clock[pMode->Clock];

   switch(s3RamdacType) {
	 case NORMAL_DAC:
	    /* only suports 8bpp -- nothing to do */
	    break;
	 case S3_TRIO64_DAC:
	    switch (s3Bpp) {
	    case 1:
#if 0
	       /* XXXX pMode->SynthClock /= 2 might be better with sr15 &= ~0x40
		  in s3init.c if screen wouldn't completely blank... */
	       if (pMode->SynthClock > nonMuxMaxClock) {
		  pMode->SynthClock /= 2;
		  pMode->Flags |= V_DBLCLK;
	       }
#endif
	       break;
	    case 2:
	    case 4:
	       /* No change for 16bpp and 24bpp */
	       break;
	    }
	    break;
	 default:
	    /* Do nothing */
	    break;
   }


   return MODE_OK;
}
