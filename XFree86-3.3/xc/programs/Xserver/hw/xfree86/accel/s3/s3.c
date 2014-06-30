/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3.c,v 3.155.2.10 1997/06/01 12:33:29 dawes Exp $ */
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
 * Rather severely reorganized by MArk Vojkovich (mvojkovi@ucsd.edu)
 * 
 */
/* $XConsortium: s3.c /main/48 1996/12/28 15:54:30 kaleb $ */

#include "misc.h"
#include "cfb.h"
#include "pixmapstr.h"
#include "fontstruct.h"
#include "s3.h"
#include "regs3.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "s3linear.h"
#include "s3Bt485.h"
#include "Ti302X.h"
#include "IBMRGB.h"
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
char s3Mbanks;
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

extern unsigned char *find_bios_string(
#if NeedFunctionPrototypes 
    int		/* BIOSbase */,
    char *	/* match1 */,
    char *	/* match2 */
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
   vgaDPMSSet,			/* void (* DPMSSet)() */
   s3PrintIdent,		/* void (* PrintIdent)() */
   8,				/* int depth */
   {5, 6, 5},			/* xrgb weight */
   8,				/* int bitsPerPixel */
   PseudoColor,			/* int defaultVisual */
   -1, -1,			/* int virtualX,virtualY */
   -1,				/* int displayWidth */
   -1, -1, -1, -1,		/* int frameX0, frameY0, frameX1, frameY1 */
   {{0}},			/* OFlagSet options */
   {{0}},			/* OFlagSet clockOptions */   
   {{0}},              		/* OFlagSet xconfigFlag */
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
   "S3",			/* char           *name */
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

short s3alu[16] =
{
   MIX_0,
   MIX_AND,
   MIX_SRC_AND_NOT_DST,
   MIX_SRC,
   MIX_NOT_SRC_AND_DST,
   MIX_DST,
   MIX_XOR,
   MIX_OR,
   MIX_NOR,
   MIX_XNOR,
   MIX_NOT_DST,
   MIX_SRC_OR_NOT_DST,
   MIX_NOT_SRC,
   MIX_NOT_SRC_OR_DST,
   MIX_NAND,
   MIX_1
};

static unsigned S3_IOPorts[] = { DISP_STAT, H_TOTAL, H_DISP, H_SYNC_STRT,
  H_SYNC_WID, V_TOTAL, V_DISP, V_SYNC_STRT, V_SYNC_WID, DISP_CNTL,
  ADVFUNC_CNTL, SUBSYS_STAT, SUBSYS_CNTL, ROM_PAGE_SEL, CUR_Y, CUR_X,
  DESTY_AXSTP, DESTX_DIASTP, ERR_TERM, MAJ_AXIS_PCNT, GP_STAT, CMD,
  SHORT_STROKE, BKGD_COLOR, FRGD_COLOR, WRT_MASK, RD_MASK, COLOR_CMP,
  BKGD_MIX, FRGD_MIX, MULTIFUNC_CNTL, PIX_TRANS, PIX_TRANS_EXT,
};


static int Num_S3_IOPorts = (sizeof(S3_IOPorts)/sizeof(S3_IOPorts[0]));

static SymTabRec s3DacTable[] = {
   { NORMAL_DAC,	"normal" },
   { BT485_DAC,		"bt485" },
   { BT485_DAC,		"bt9485" },
   { ATT20C505_DAC,	"att20c505" },
   { TI3020_DAC,	"ti3020" },
   { ATT20C498_DAC,	"att20c498" },
   { ATT20C498_DAC,	"att21c498" },
   { ATT22C498_DAC,	"att22c498" },
   { TI3025_DAC,	"ti3025" },
   { TI3026_DAC,	"ti3026" },
   { TI3030_DAC,	"ti3030" },
   { IBMRGB525_DAC,	"ibm_rgb514" },
   { IBMRGB524_DAC,	"ibm_rgb524" },
   { IBMRGB525_DAC,	"ibm_rgb525" },
   { IBMRGB524_DAC,	"ibm_rgb526" },
   { IBMRGB528_DAC,	"ibm_rgb528" },
   { ATT20C490_DAC,	"att20c490" },
   { ATT20C490_DAC,	"att20c491" },
   { ATT20C490_DAC,	"ch8391" },
   { SC1148x_M2_DAC,	"sc11482" },
   { SC1148x_M2_DAC,	"sc11483" },
   { SC1148x_M2_DAC,	"sc11484" },
   { SC1148x_M3_DAC,	"sc11485" },
   { SC1148x_M3_DAC,	"sc11487" },
   { SC1148x_M3_DAC,	"sc11489" },
   { SC15025_DAC,	"sc15025" },
   { STG1700_DAC,	"stg1700" },
   { STG1703_DAC,	"stg1703" },
   { S3_SDAC_DAC,	"s3_sdac" },
   { S3_SDAC_DAC,	"ics5342" },       /* XXXX should be checked if true */
   { S3_GENDAC_DAC,	"s3gendac" },
   { S3_GENDAC_DAC,	"ics5300" },
   { S3_TRIO32_DAC,	"s3_trio32" },
   { S3_TRIO64_DAC,	"s3_trio64" },
   { S3_TRIO64_DAC,	"s3_trio" },
   { ATT20C409_DAC,	"att20c409" },
   { SS2410_DAC,	"ss2410" },
   { S3_TRIO64V2_DAC,	"s3_trio64v2" },
   { -1,		"" },
};

static SymTabRec s3ChipTable[] = {
   { S3_UNKNOWN,	"unknown" },
   { S3_911,		"911" },
   { S3_924,		"924" },
   { S3_801,		"801" },
   { S3_805,		"805" },
   { S3_928,		"928" },
   { S3_TRIO_32_64,	"Trio32/64" },
   { S3_864,		"864" },
   { S3_868,		"868" },
   { S3_964,		"964" },
   { S3_968,		"968" },
   { S3_TRIO32,		"Trio32" },
   { S3_TRIO64,		"Trio64" },
   { S3_TRIO64VP,	"Trio64V+" },
   { S3_ViRGE,		"ViRGE" },
   { S3_ViRGE_VX,	"ViRGE/VX" },
   { S3_ViRGE_DXGX,	"ViRGE/DX or /GX" },
   { S3_TRIO64UVP,	"Trio64UV+" },
   { S3_AURORA64VP,	"Aurora64V+" },
   { S3_TRIO64V2,	"Trio64V2" },
   { S3_PLATO_PX,	"PLATO/PX" },
   { -1,		"" },
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
ScreenPtr s3savepScreen;
Bool  s3Localbus = FALSE;
Bool  s3VLB = FALSE;
Bool  s3NewMmio = FALSE;
Bool  s3LinearAperture = FALSE;
Bool  s3Mmio928 = FALSE;
Bool  s3PixelMultiplexing = FALSE;
Bool  s3DAC8Bit = FALSE;
Bool  s3DACSyncOnGreen = FALSE;
Bool  s3PCIHack = FALSE;
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
Bool tmp_useSWCursor = FALSE;
#ifdef __alpha__
unsigned long _bus_base(void);
Bool isJensen = FALSE;
#endif

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
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
void (*s3ImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
int s3hotX, s3hotY;
Bool s3BlockCursor, s3ReloadCursor;
int s3CursorStartX, s3CursorStartY, s3CursorLines;
int s3RamdacType = UNKNOWN_DAC;
Bool s3UsingPixMux = FALSE;
Bool s3Bt485PixMux = FALSE;
Bool s3ATT498PixMux = FALSE;
int maxRawClock = 0;
Bool clockDoublingPossible = FALSE;
int s3AdjustCursorXPos = 0;
int s3BiosVendor = UNKNOWN_BIOS;

static Bool in_s3Probe = TRUE; /* s3ValidMode helpers */
static Bool not_safe = TRUE;
static int TempVirtualX, TempVirtualY;

int s3Bpp;
int s3BppDisplayWidth;

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


unsigned char *find_bios_string(int BIOSbase, char *match1, char *match2)
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




static int s3DetectMIRO_20SV_Rev(int BIOSbase)
{
   char *match1 = "miroCRYSTAL\37720SV", *match2 = "Rev.";
   unsigned char *p;

   if ((p = find_bios_string(BIOSbase,match1,match2)) != NULL) {
      if (s3BiosVendor == UNKNOWN_BIOS) 
	 s3BiosVendor = MIRO_BIOS;
      if (*p >= '0' && *p <= '9')
	 return *p - '0';
   }
   return -1;
}

static int check_SPEA_bios(int BIOSbase)
{
   char *match = " SPEA/Video";
   unsigned char *p;
   
   if ((p = find_bios_string(BIOSbase,match,NULL)) != NULL) {
      if (s3BiosVendor == UNKNOWN_BIOS) 
	 s3BiosVendor = SPEA_BIOS;
      return 1;
   }
   return 0;
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
	 case PCI_TRIO_32_64:
	    info.ChipType = S3_TRIO_32_64;
	    break;
	 case PCI_928:
	    info.ChipType = S3_928;
	    break;
	 case PCI_864_0:
	 case PCI_864_1:
	    info.ChipType = S3_864;
	    break;
	 case PCI_964_0:
	 case PCI_964_1:
	    info.ChipType = S3_964;
	    break;
	 case PCI_868:
	    info.ChipType = S3_868;
	    break;
	 case PCI_968:
	    info.ChipType = S3_968;
	    break;
	 case PCI_ViRGE:
	    info.ChipType = S3_ViRGE;
	    break;
	 case PCI_ViRGE_VX:
	    info.ChipType = S3_ViRGE_VX;
	    break;
	 case PCI_ViRGE_DXGX:
	    info.ChipType = S3_ViRGE_DXGX;
	    break;
	 case PCI_AURORA64VP:
	    info.ChipType = S3_AURORA64VP;
	    break;
	 case PCI_TRIO64UVP:
	    info.ChipType = S3_TRIO64UVP;
	    break;
	 case PCI_TRIO64V2_DXGX:
	    info.ChipType = S3_TRIO64V2;
	    break;
	 case PCI_PLATO_PX:
	    info.ChipType = S3_PLATO_PX;
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

   /* for new mmio we have to ensure that the PCI base address is 
    * 64MB aligned and that there are no address collitions within 64MB.
    * S3 868/968 only pretend to need 32MB and thus fool 
    * the BIOS PCI auto configuration :-(  */
 
   if (found 
       && info.ChipType != S3_928
       && info.ChipType != S3_864
       && info.ChipType != S3_964
       && !(info.ChipType == S3_TRIO_32_64 && info.ChipRev < 0x40)  /* only Trio64V+ */
       ) {
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
    *                            there is mode with too small a width
    *   pixMuxClockOK	       - FALSE if pixmux isn't possible because
    *				 of a clock with too low of a value.
    *	pixMuxNeeded	       - TRUE when all modes require pixmux due
    *				 to !allowPixMuxSwitching or too much 
    *			         memory (This is a different usage
    *				 from what was in the old s3.c)
    *
    */

Bool pixMuxPossible = FALSE;
Bool allowPixMuxInterlace = FALSE;
Bool allowPixMuxSwitching = FALSE;
Bool pixMuxNeeded = FALSE; 
Bool pixMuxInterlaceOK = TRUE; 
Bool pixMuxWidthOK = TRUE;
Bool pixMuxClockOK = TRUE;
int nonMuxMaxClock = 0;
int nonMuxMaxMemory = 8192;
int maxDisplayWidth;
int maxDisplayHeight;
int pixMuxMinWidth = 1024;
int pixMuxMinClock = 0;
Bool pixMuxLimitedWidths = TRUE;

int numClocks;

Bool
s3Probe()
{
   DisplayModePtr pMode, pEnd;
   unsigned char config;
   int i, j;
   OFlagSet validOptions;
   char *card, *serno, *elsa_modes;
   int card_id, max_pix_clock, max_mem_clock, hwconf;
   int lookupFlags;
   unsigned char tmp, cr66;

   Bool OldPixMuxStatus;

   S3PCIInformation *pciInfo = NULL;

#if !defined(PC98) || defined(PC98_GA968)
   /* Do general PCI probe first */
   pciInfo = s3GetPCIInfo();
   if (pciInfo && pciInfo->MemBase)
      s3MemBase = pciInfo->MemBase;
#endif

   xf86ClearIOPortList(s3InfoRec.scrnIndex);
   xf86AddIOPorts(s3InfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
   xf86AddIOPorts(s3InfoRec.scrnIndex, Num_S3_IOPorts, S3_IOPorts);

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

	/***********************************************\
	|		Is it an S3? 			|
	\***********************************************/

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
      if (S3_ANY_ViRGE_SERIES(s3ChipId)) {
         ErrorF("%s %s: S3 ViRGE chipset: please use \"XF86_S3V\" server\n",
		XCONFIG_PROBED,s3InfoRec.name);
      } else {
	 ErrorF("%s %s: Unknown S3 chipset: chip_id = 0x%02x rev. %x\n",
		XCONFIG_PROBED,s3InfoRec.name,s3ChipId,s3ChipRev);
      }
      xf86DisableIOPorts(s3InfoRec.scrnIndex);
      return(FALSE);
   }



 	/***************************************\
	|		Which driver ?		|
	\***************************************/

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

    /***************************************************************\
    |  Print out the chipset name and driver for the user's benefit |
    \***************************************************************/

   if (xf86Verbose) {
      if (S3_x64_SERIES(s3ChipId)) {
	 char *chipname = "unknown";

	 if (S3_868_SERIES(s3ChipId)) {
	    chipname = "868";
	 } else if (S3_866_SERIES(s3ChipId)) {
	    chipname = "866";
	 } else if (S3_864_SERIES(s3ChipId)) {
	    chipname = "864";
	 } else if (S3_968_SERIES(s3ChipId)) {
	    chipname = "968";
	 } else if (S3_964_SERIES(s3ChipId)) {
	    chipname = "964";
	 } else if (S3_TRIO32_SERIES(s3ChipId)) {
	    chipname = "Trio32";
	 } else if (S3_TRIO64UVP_SERIES(s3ChipId)) {
	    chipname = "Trio64UV+ (preliminary support; please report)";
	 } else if (S3_AURORA64VP_SERIES(s3ChipId)) {
	    chipname = "Aurora64V+ (preliminary support; please report)";
	 } else if (S3_TRIO64V_SERIES(s3ChipId /* , s3ChipRev */)) {
	    chipname = "Trio64V+";
	 } else if (S3_TRIO64V2_SERIES(s3ChipId)) {
	    outb(vgaCRIndex, 0x39);
	    outb(vgaCRReg, 0xa5);
	    outb(vgaCRIndex, 0x6f);
	    if (inb(vgaCRReg) & 1)
	       chipname = "Trio64V2/GX";
	    else
	       chipname = "Trio64V2/DX";
	 } else if (S3_TRIO64_SERIES(s3ChipId)) {
	    chipname = "Trio64";
	 } else if (S3_PLATO_PX_SERIES(s3ChipId)) {
	    chipname = "PLATO/PX (preliminary support; please report)";
	 }
	 ErrorF("%s %s: chipset:   %s rev. %x\n",
                XCONFIG_PROBED, s3InfoRec.name, chipname, s3ChipRev);
      } else if (S3_801_928_SERIES(s3ChipId)) {
	 if (S3_801_SERIES(s3ChipId)) {
            if (S3_805_I_SERIES(s3ChipId)) {
               ErrorF("%s %s: chipset:   805i",
                      XCONFIG_PROBED, s3InfoRec.name);
               if ((config & 0x03) == 3)
                  ErrorF(" (ISA)");
               else
                  ErrorF(" (VL)");
            }
	    else if (!((config & 0x03) == 3))
	       ErrorF("%s %s: chipset:   805",
                      XCONFIG_PROBED, s3InfoRec.name);
	    else
	       ErrorF("%s %s: chipset:   801",
                       XCONFIG_PROBED, s3InfoRec.name);
	    ErrorF(", ");
	    if (S3_801_REV_C(s3ChipId))
	       ErrorF("rev C or above\n");
	    else
	       ErrorF("rev A or B\n");
	 } else if (S3_928_SERIES(s3ChipId)) {
	    char *pci = S3_928_P(s3ChipId) ? "-P" : "";
	    if (S3_928_REV_E(s3ChipId))
		ErrorF("%s %s: chipset:   928%s, rev E or above\n",
                   XCONFIG_PROBED, s3InfoRec.name, pci);
	    else
	        ErrorF("%s %s: chipset:   928%s, rev D or below\n",
                   XCONFIG_PROBED, s3InfoRec.name, pci);
	 }
      } else if (S3_911_SERIES(s3ChipId)) {
	 if (S3_911_ONLY(s3ChipId)) {
	    ErrorF("%s %s: chipset:   911 \n",
                   XCONFIG_PROBED, s3InfoRec.name);
	 } else if (S3_924_ONLY(s3ChipId)) {
	    ErrorF("%s %s: chipset:   924\n",
                   XCONFIG_PROBED, s3InfoRec.name);
	 } else {
	    ErrorF("%s %s: S3 chipset type unknown, chip_id = 0x%02x\n",
		   XCONFIG_PROBED, s3InfoRec.name, s3ChipId);
	 }
      }
   }

   if (xf86Verbose) {
      ErrorF("%s %s: chipset driver: %s\n",
	     OFLG_ISSET(XCONFIG_CHIPSET, &s3InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED,
	     s3InfoRec.name, s3InfoRec.chipset);
   }


	/***************************************\
	| Was a ramdac supplied in XF86Config? 	|
	\***************************************/

   if (s3InfoRec.ramdac) {
      s3RamdacType = xf86StringToToken(s3DacTable, s3InfoRec.ramdac);
      if (s3RamdacType < 0) {
	 ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
		s3InfoRec.name, s3InfoRec.ramdac);
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return(FALSE);
      }
   }

	/*******************************\
	|	 Set valid options 	|
	\*******************************/

   OFLG_ZERO(&validOptions);
   OFLG_SET(OPTION_LEGEND, &validOptions);
   OFLG_SET(OPTION_CLKDIV2, &validOptions);
   OFLG_SET(OPTION_NOLINEAR_MODE, &validOptions);
   if (!S3_x64_SERIES(s3ChipId))
      OFLG_SET(OPTION_NO_MEM_ACCESS, &validOptions);
   OFLG_SET(OPTION_SW_CURSOR, &validOptions);
   OFLG_SET(OPTION_BT485_CURS, &validOptions);
   OFLG_SET(OPTION_SHOWCACHE, &validOptions);
   OFLG_SET(OPTION_FB_DEBUG, &validOptions);
   OFLG_SET(OPTION_NO_FONT_CACHE, &validOptions);
   OFLG_SET(OPTION_NO_PIXMAP_CACHE, &validOptions);
   OFLG_SET(OPTION_TI3020_CURS, &validOptions);
   OFLG_SET(OPTION_NO_TI3020_CURS, &validOptions);
   OFLG_SET(OPTION_TI3026_CURS, &validOptions);
   OFLG_SET(OPTION_IBMRGB_CURS, &validOptions);
   OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
   OFLG_SET(OPTION_DAC_6_BIT, &validOptions);
   OFLG_SET(OPTION_SYNC_ON_GREEN, &validOptions);
   OFLG_SET(OPTION_SPEA_MERCURY, &validOptions);
   OFLG_SET(OPTION_NUMBER_NINE, &validOptions);
   OFLG_SET(OPTION_STB_PEGASUS, &validOptions);
   OFLG_SET(OPTION_MIRO_MAGIC_S4, &validOptions);
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
   /* ELSA_W1000PRO isn't really required any more */
   OFLG_SET(OPTION_ELSA_W1000PRO, &validOptions);
   OFLG_SET(OPTION_ELSA_W2000PRO, &validOptions);
   OFLG_SET(OPTION_DIAMOND, &validOptions);
   OFLG_SET(OPTION_GENOA, &validOptions);
   OFLG_SET(OPTION_STB, &validOptions);
   OFLG_SET(OPTION_HERCULES, &validOptions);
   if (S3_928_P(s3ChipId))
      OFLG_SET(OPTION_PCI_HACK, &validOptions);
   OFLG_SET(OPTION_POWER_SAVER, &validOptions);
   OFLG_SET(OPTION_S3_964_BT485_VCLK, &validOptions);
   OFLG_SET(OPTION_SLOW_DRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_EDODRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_VRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_DRAM_REFRESH, &validOptions);
   OFLG_SET(OPTION_FAST_VRAM, &validOptions);
   OFLG_SET(OPTION_EARLY_RAS_PRECHARGE, &validOptions);
   OFLG_SET(OPTION_LATE_RAS_PRECHARGE, &validOptions);
   OFLG_SET(OPTION_TRIO32_FC_BUG, &validOptions);
   OFLG_SET(OPTION_S3_968_DASH_BUG, &validOptions);
   OFLG_SET(OPTION_TRIO64VP_BUG1, &validOptions);
   OFLG_SET(OPTION_TRIO64VP_BUG2, &validOptions);
   OFLG_SET(OPTION_TRIO64VP_BUG3, &validOptions);
   OFLG_SET(OPTION_ELSA_W2000PRO_X8, &validOptions);
   OFLG_SET(OPTION_MIRO_80SV, &validOptions);
   OFLG_SET(OPTION_NO_PCI_DISC, &validOptions);
   xf86VerifyOptions(&validOptions, &s3InfoRec);

#ifdef __alpha__
#ifdef TEST_JENSEN_CODE
   if (1)
#else
   if (!_bus_base()) 
#endif
   { /* Jensen */
       isJensen = TRUE;
       OFLG_SET(OPTION_NOLINEAR_MODE, &s3InfoRec.options);
   }
#endif /* __alpha__ */

#ifdef PC98
   if (OFLG_ISSET(OPTION_PW_MUX, &s3InfoRec.options)) {
      OFLG_SET(OPTION_SPEA_MERCURY, &s3InfoRec.options);
    }
#endif

   if (S3_x64_SERIES(s3ChipId)) {
      if (OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
	 ErrorF("%s %s: Option \"nomemaccess\" is ignored for 86x/96x/TRIOxx\n",
		XCONFIG_PROBED, s3InfoRec.name);
	 OFLG_CLR(OPTION_NO_MEM_ACCESS, &s3InfoRec.options);
      }
   }


   if (OFLG_ISSET(OPTION_TRIO32_FC_BUG, &s3InfoRec.options))
      s3Trio32FCBug = TRUE;
   if (OFLG_ISSET(OPTION_S3_968_DASH_BUG, &s3InfoRec.options))
      s3_968_DashBug = TRUE;

   if (OFLG_ISSET(OPTION_GENOA, &s3InfoRec.options))
      s3BiosVendor = GENOA_BIOS;
   else if (OFLG_ISSET(OPTION_STB, &s3InfoRec.options))
      s3BiosVendor = STB_BIOS;
   else if (OFLG_ISSET(OPTION_HERCULES, &s3InfoRec.options))
      s3BiosVendor = HERCULES_BIOS;
   else if (OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options))
      s3BiosVendor = NUMBER_NINE_BIOS;

	/***************************************\
   	|	 LocalBus, EISA or PCI ?	|
	\***************************************/


   s3Localbus = ((config & 0x03) <= 2) || S3_928_P(s3ChipId);

   if (xf86Verbose) {
      if (S3_928_P(s3ChipId)) {
	 ErrorF("%s %s: card type: PCI\n", XCONFIG_PROBED, s3InfoRec.name);
      } else {
	 switch (config & 0x03) {
	 case 0:
	    ErrorF("%s %s: card type: EISA\n", XCONFIG_PROBED, s3InfoRec.name);
	    break;
	 case 1:
            ErrorF("%s %s: card type: 386/486 localbus\n",
        	   XCONFIG_PROBED, s3InfoRec.name);
	    s3VLB = TRUE;
	    break;
	 case 3:
            ErrorF("%s %s: card type: ISA\n", XCONFIG_PROBED, s3InfoRec.name);
	    break;
	 case 2:
	    ErrorF("%s %s: card type: PCI\n", XCONFIG_PROBED, s3InfoRec.name);
	 }
      }
   }



	/*****************************************************\
	| Determine card vendor to aid in clockchip detection |
	\*****************************************************/
	
   /* reset S3 graphics engine to avoid memory corruption */
   if (S3_TRIO64V_SERIES(s3ChipId)) {
     outb(vgaCRIndex, 0x66);
     cr66 = inb(vgaCRReg);
     outb(vgaCRReg, i |  0x02);
     usleep(10000);  /* wait a little bit... */
   }

   card_id = s3DetectMIRO_20SV_Rev(s3InfoRec.BIOSbase);
   if (card_id > 1) {
      ErrorF("%s %s: MIRO 20SV Rev.2 or newer detected.\n",
             XCONFIG_PROBED, s3InfoRec.name);
      if (!OFLG_ISSET(OPTION_S3_964_BT485_VCLK, &s3InfoRec.options))
	 ErrorF("\tplease use Option \"s3_964_bt485_vclk\"\n");
   }

   if (find_bios_string(s3InfoRec.BIOSbase,
			"S3 Vision968 IBM RGB DAC", NULL)  != NULL
       && find_bios_string(s3InfoRec.BIOSbase, 
			   "miro\37780",NULL) != NULL) {
      OFLG_SET(OPTION_MIRO_80SV,  &s3InfoRec.options);
      if (s3InfoRec.dacSpeeds[0] <= 0)
	 s3InfoRec.dacSpeeds[0] = 250000;
      if (s3RamdacType == UNKNOWN_DAC) {
	 s3RamdacType = IBMRGB528_DAC;
	 ErrorF("%s %s: MIRO 80SV detected, using IBM RGB528 ramdac\n",
		XCONFIG_PROBED, s3InfoRec.name);
      }
   }

   if (find_bios_string(s3InfoRec.BIOSbase,"Stealth",
			"Diamond Computer Systems, Inc.") != NULL ||
       find_bios_string(s3InfoRec.BIOSbase,"Stealth",
			"Diamond Multimedia Systems, Inc.") != NULL) {
      if (s3BiosVendor == UNKNOWN_BIOS) 
	 s3BiosVendor = DIAMOND_BIOS;
      if (xf86Verbose)
	 ErrorF("%s %s: Diamond Stealth BIOS found\n",
		XCONFIG_PROBED, s3InfoRec.name);
   }

   card_id = s3DetectELSA(s3InfoRec.BIOSbase, &card, &serno, &max_pix_clock,
			  &max_mem_clock, &hwconf, &elsa_modes);

   if (S3_TRIO64V_SERIES(s3ChipId)) {
     outb(vgaCRIndex, 0x66);
     outb(vgaCRReg, cr66 & ~0x02);  /* clear reset flag */
     usleep(10000);  /* wait a little bit... */
   }

   if (card_id > 0) {
      if (s3BiosVendor == UNKNOWN_BIOS) 
	 s3BiosVendor = ELSA_BIOS;
      if (xf86Verbose) {
         ErrorF("%s %s: card: %s, Ser.No. %s\n",
	        XCONFIG_PROBED, s3InfoRec.name, card, serno);
	 if (elsa_modes && *elsa_modes)
	    ErrorF("%s %s: video modes stores in ELSA EEPROM:\n%s",
		   XCONFIG_PROBED, s3InfoRec.name, elsa_modes);
      }
      xfree(card);
      xfree(serno);
      xfree(elsa_modes);

      if (s3InfoRec.dacSpeeds[0] <= 0)
	 s3InfoRec.dacSpeeds[0] = max_pix_clock;

      do {
	 switch (card_id) {
	 case ELSA_WINNER_1000AVI:
	 case ELSA_WINNER_1000PRO:
	    /* This option isn't required at the moment */
	    OFLG_SET(OPTION_ELSA_W1000PRO,  &s3InfoRec.options);
	    /* fallthrough */
	 case ELSA_WINNER_1000:
	 case ELSA_WINNER_1000VL:
	 case ELSA_WINNER_1000PCI:
	 case ELSA_WINNER_1000ISA:
	    if ((s3Ramdacs[S3_SDAC_DAC].DacProbe)()) {
	       s3RamdacType = S3_SDAC_DAC;
	       continue;  /* SDAC detected, don't set ICD2061A clock */
	    }
            if((s3Ramdacs[ATT20C409_DAC].DacProbe)())
	    	s3RamdacType = ATT20C409_DAC;
               /* if ATT20C409 is detected, the clockchip is 
                * already set apropriately 
                */
	    /* otherwise it's a STG1700,20C498, or SC15025 and will get 
		detected later.  The clockchip ICD2061A will get set at
 		the at the end of this loop */
	    break;
	 case ELSA_WINNER_2000PRO:
	    OFLG_SET(OPTION_ELSA_W2000PRO,  &s3InfoRec.options);
	    break;
	 case ELSA_WINNER_2000PRO_X8:
	    OFLG_SET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options);
	    continue; /* use IBM RGB528 clock, don't set ICD2061A flags */
	 case ELSA_WINNER_2000:
	 case ELSA_WINNER_2000VL:
	 case ELSA_WINNER_2000PCI:
	    break;  /* set ICD2061A clock chip */
	 case ELSA_GLORIA_8:
	    if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
	       FatalError("%s %s: for the ELSA Gloria-8 card you should not "
		 "specify a clock chip!\n",XCONFIG_GIVEN, s3InfoRec.name);
	    }
	    OFLG_SET(CLOCK_OPTION_GLORIA8, &s3InfoRec.clockOptions);
	    OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
	    clockchip_probed = XCONFIG_PROBED;
	    /* fall through ... */

	 case ELSA_WINNER_2000AVI:
	 case ELSA_WINNER_2000PRO_X:
	 case ELSA_GLORIA_4:
	    if (OFLG_ISSET(OPTION_ELSA_W2000PRO,&s3InfoRec.options)) {
	       ErrorF("%s %s: for Ti3026/3030 RAMDACs you must not specify "
 		"Option \"elsa_w2000pro\"\n",XCONFIG_PROBED, s3InfoRec.name);
	       OFLG_CLR(OPTION_ELSA_W2000PRO, &s3InfoRec.options);
	    }
	    if ((card_id==ELSA_WINNER_2000PRO_X) && (hwconf & 2)) {
	       /*
	        * this version of the Winner 2000PRO/X has an external ICS9161A
	        * clockchip, so set ICD2061A flag
	        */
               if (xf86Verbose)
	          ErrorF("%s %s: Rev. G Winner 2000PRO/X with external " 
		    "clockchip detected\n",XCONFIG_PROBED, s3InfoRec.name);
	       break;
	    } else {
	       continue;
            }
	 case ELSA_WINNER_1000PRO_TRIO32:
	 case ELSA_WINNER_1000PRO_TRIO64:
	 case ELSA_WINNER_1000PRO_X:
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


	/***************************************\
	|	Detect VideoRam amount 		|
	\***************************************/


   if (!s3InfoRec.videoRam) {
      if (((config & 0x20) != 0) 	/* if bit 5 is a 1, then 512k RAM */
          && (!S3_964_SERIES(s3ChipId))) {
	 s3InfoRec.videoRam = 512;
      } else {			/* must have more than 512k */
	 if (S3_911_SERIES(s3ChipId)) {
	    s3InfoRec.videoRam = 1024;
	 } else {
	    if (S3_PLATO_PX_SERIES(s3ChipId))
	       s3InfoRec.videoRam = (8-((config & 0xE0) >> 5)) * 512;
	    else switch ((config & 0xE0) >> 5) {	/* look at bits 6 and 7 */
	       case 0:
	         s3InfoRec.videoRam = 4096;
		 break;
	       case 2:
	         s3InfoRec.videoRam = 3072;
		 break;
	       case 3:
	         s3InfoRec.videoRam = 8192;
		 break;
	       case 4:
		 s3InfoRec.videoRam = 2048;
	         break;
	       case 5:
		 s3InfoRec.videoRam = 6144;
	         break;
	       case 6:
	         s3InfoRec.videoRam = 1024;
		 break;
	    }
	 }
      }
      if (xf86Verbose) {
         ErrorF("%s %s: videoram:  %dk\n",
              XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.videoRam);
      }
   } else {
      if (xf86Verbose) {
	 ErrorF("%s %s: videoram:  %dk\n", 
              XCONFIG_GIVEN, s3InfoRec.name, s3InfoRec.videoRam);
      }
   }
   if (s3InfoRec.videoRam > 1024)
      s3Mbanks = -1;
   else
      s3Mbanks = 0;

	/***********************************************\
	| 	Set the visual, depth, weight, etc...   |
	\***********************************************/


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
      }
      else if (xf86weight.red==5 && xf86weight.green==6 && xf86weight.blue==5) {
	 s3Weight = RGB16_565;
	 s3InfoRec.depth = 16;
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
#ifdef NOT_YET
      s3InfoRec.depth = 24;
      s3InfoRec.bitsPerPixel = 32; /* Use packed 24 bpp (RGB) but this 
				      should be transparant for clients */
      s3InfoRec.bitsPerPixel = 24; /* not not yet or not here ? HACK24 */ 
      s3Weight = RGB32_888;
      /* s3MaxClock = S3_MAX_32BPP_CLOCK; */
      xf86weight.red =  xf86weight.green = xf86weight.blue = 8;
      if (s3InfoRec.defaultVisual < 0)
	 s3InfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
	 defaultColorVisualClass = s3InfoRec.defaultVisual;
      break;
#else
      xf86bpp = 32;
      /* FALLTHROUGH */
#endif
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
   default:
      ErrorF(
	"Invalid value for bpp.  Valid values are 8, 15, 16, 24 and 32.\n");
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


    	/*******************************\
   	|	 Probe for RAMDAC 	|
	\*******************************/

   /* Make sure CR55 is unlocked for Bt485 probe */

   outb(vgaCRIndex, 0x39);
   outb(vgaCRReg, 0xA5);

   if(s3RamdacType == UNKNOWN_DAC) {
     for (i = 1; s3Ramdacs[i].DacName; i++) {
      if ((s3Ramdacs[i].DacProbe)()) {
	 s3RamdacType = i;
	 break;
       }
     }
   } 

#if 0
   else {
    /* shouldn't we probe it anyway to ensure options are set? */
    if(!(s3Ramdacs[s3RamdacType].DacProbe)()) {
      ErrorF("WARNING: Did not detect a ramdac of type \"%s\" as specified!\n",
		s3Ramdacs[s3RamdacType].DacName);
	/* but we accept the user's assertion */
    }
   }
#endif

   /* If we still don't know the ramdac type, set it to NORMAL_DAC */
   if (s3RamdacType == UNKNOWN_DAC) {
      if (xf86Verbose) 
         ErrorF("%s %s: Unknown ramdac. Setting type to \"normal_dac\".\n",
              XCONFIG_PROBED, s3InfoRec.name);
      s3RamdacType = NORMAL_DAC;
   }

        
	/*******************************************************\
	| Set some SPEA specific options that had to wait until |
	| after we knew what the ramdac was.			|
	\*******************************************************/ 
  
	/* might want to consider moving this to the PreInit() 
		functions (MArk) */

   if ((!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions) &&
       !OFLG_ISSET(OPTION_SPEA_MERCURY, &s3InfoRec.options) &&
       (s3InfoRec.ramdac == NULL)) || S3_TRIO64_SERIES(s3ChipId))
				   { /* ensure that autodetection can be */
                                     /* overwritten 			 */	
     card_id = check_SPEA_bios(s3InfoRec.BIOSbase); 
     if (card_id > 0) {

       switch (s3RamdacType) {
       case BT485_DAC: 
       case ATT20C505_DAC: 
          if (S3_928_ONLY(s3ChipId)) {
             /* SPEA Mercury */
             ErrorF("%s %s: SPEA Mercury detected.\n",
             XCONFIG_PROBED, s3InfoRec.name);
             OFLG_SET(OPTION_SPEA_MERCURY, &s3InfoRec.options);
             OFLG_SET(CLOCK_OPTION_SC11412, &s3InfoRec.clockOptions);
             OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
             clockchip_probed = XCONFIG_PROBED; 
          } else if  (S3_964_SERIES(s3ChipId)) { 
             /* SPEA Mercury P64 */ 
             ErrorF("%s %s: SPEA Mercury P64 detected.\n",
             XCONFIG_PROBED, s3InfoRec.name);
             OFLG_SET(OPTION_SPEA_MERCURY, &s3InfoRec.options);
             OFLG_SET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions);
             OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
             clockchip_probed = XCONFIG_PROBED;
          } 
          break;
       case ATT20C498_DAC: 
          if (S3_864_SERIES(s3ChipId)) { 
            /* SPEA MirageP64 Bios 3.xx */
            ErrorF("%s %s: SPEA Mirage P64 detected.\n",
            XCONFIG_PROBED, s3InfoRec.name);
            OFLG_SET(CLOCK_OPTION_ICS2595, &s3InfoRec.clockOptions);
            OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions);
            clockchip_probed = XCONFIG_PROBED;
          }
          break;
       case S3_SDAC_DAC:
          if (S3_864_SERIES(s3ChipId)) 
            /* SPEA Mirage P64 Bios 4.xx */
            ErrorF("%s %s: SPEA Mirage P64 detected.\n",
            XCONFIG_PROBED, s3InfoRec.name);
          break;
       case S3_TRIO64_DAC: 
          if (S3_TRIO64_SERIES(s3ChipId)) 
            /* SPEA Mirage P64 Bios 5.xx */
            ErrorF("%s %s: SPEA Mirage P64 Trio64 detected.\n",
            XCONFIG_PROBED, s3InfoRec.name);
          break;
       case S3_GENDAC_DAC:
          if (S3_801_SERIES(s3ChipId))
            /* SPEA Mirage Bios 5.x */
            ErrorF("%s %s: SPEA Mirage detected.\n",
            XCONFIG_PROBED, s3InfoRec.name);
          break;
      } 
     }
    }   /* end SPEA autodetect */

  	/*********************************************\
	| make sure s3InfoRec.ramdac is set correctly |
	\*********************************************/

   s3InfoRec.ramdac = xf86TokenToString(s3DacTable, s3RamdacType);

   if (xf86Verbose) {
      ErrorF("%s %s: Ramdac type: %s\n",
	     OFLG_ISSET(XCONFIG_RAMDAC, &s3InfoRec.xconfigFlag) ?
	     XCONFIG_GIVEN : XCONFIG_PROBED, s3InfoRec.name, s3InfoRec.ramdac);
   }

	/*******************************************************\
   	|	 Now set the DAC speeds if not already set 	|
	\*******************************************************/

   if (s3InfoRec.dacSpeeds[0] <= 0) {
	s3InfoRec.dacSpeeds[0] = s3Ramdacs[s3RamdacType].DacSpeed;
   }
   if (s3InfoRec.dacSpeedBpp <= 0)
      if (xf86bpp > 24 && s3InfoRec.dacSpeeds[3] > 0)
	 s3InfoRec.dacSpeedBpp = s3InfoRec.dacSpeeds[3];
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
      if (s3InfoRec.dacSpeedBpp > 0 && s3InfoRec.dacSpeedBpp != s3InfoRec.dacSpeeds[0])
	 ErrorF("  (%d MHz for %d bpp)",s3InfoRec.dacSpeedBpp / 1000, xf86bpp);
      ErrorF("\n");
   }

   /*******************************************************************\
   | Check that the depth requested is supported by the ramdac/chipset |
   \*******************************************************************/


    if (S3_801_SERIES(s3ChipId)) {
      if (s3Bpp > 2) {
         ErrorF("Depths greater than 16bpp are not supported for 801/805 "
		"chips.\n");
	 return(FALSE);
      }
    }
    else if (S3_911_SERIES(s3ChipId)) {
      if (s3Bpp > 1) {
         ErrorF("Depths greater than 8bpp are not supported for 911/924 "
		"chips.\n");
	 return(FALSE);
      }
    }

    if (!S3_868_SERIES(s3ChipId) && !S3_968_SERIES(s3ChipId)) {
      if (s3Bpp == 3) {
         ErrorF("Packed-pixel 24bpp depths are only supported for 868/968 "
		"chips\n");
	 return(FALSE);
      }	 
    }

    /* I guess these should return a status flag. Just 0 or 1 for now (MArk)*/
    if ((s3Ramdacs[s3RamdacType].PreInit)() <= 0) {
	/* error message? Or rely on the PreInit() functions to 
	   provide that? */
	 xf86DisableIOPorts(s3InfoRec.scrnIndex);
	 return(FALSE);
    }

	/***************************************\
	|	Last minute Clock Checks 	|
	\***************************************/

   /* Check that maxClock is not higher than dacSpeeds */
   if (s3InfoRec.maxClock > s3InfoRec.dacSpeeds[0])
      s3InfoRec.maxClock = s3InfoRec.dacSpeeds[0];

   /* Check if this exceeds the clock chip's limit */
   if (clockDoublingPossible)
      maxRawClock *= 2;
   if (maxRawClock > 0 && s3InfoRec.maxClock > maxRawClock)
      s3InfoRec.maxClock = maxRawClock;

   /* check DCLK limit of 100MHz for 866/868 */
   if (S3_866_SERIES(s3ChipId) || S3_868_SERIES(s3ChipId)) {
      if (((s3Bpp==1 && !pixMuxPossible) || s3Bpp==2) 
	  && s3InfoRec.maxClock > 100000)
	 s3InfoRec.maxClock = 100000;
      else if (s3Bpp>2 && s3InfoRec.maxClock > 50000)
	 s3InfoRec.maxClock = 50000;  
   }
   /* check DCLK limit of 95MHz for 864 */
   else if (S3_864_SERIES(s3ChipId)) {
      if (((s3Bpp==1 && !pixMuxPossible) || s3Bpp==2) 
	  && s3InfoRec.maxClock > 95000)
	 s3InfoRec.maxClock = 95000;

      /* for 24bpp the limit should be 95/2 == 47.5MHz
	 but I set the limit to 50MHz to allow VESA 800x600@72Hz */
      else if (s3Bpp>2 && s3InfoRec.maxClock > 50000)
	 s3InfoRec.maxClock = 50000;  
   }

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


      /******************************************************************\
      | Adjust s3InfoRec.clock[] when not using a programable clock chip |
      \******************************************************************/
	/* Don't know how to clean this up (MArk) */

   if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &s3InfoRec.clockOptions)) {
      Bool clocksChanged = FALSE;
      Bool numClocksChanged = FALSE;
      int newNumClocks = s3InfoRec.clocks;

      if (S3_864_SERIES(s3ChipId))
	 nonMuxMaxClock = 95000;
      else if (S3_805_I_SERIES(s3ChipId))
	 nonMuxMaxClock = 90000;  /* XXXX just a guess, who has 805i docs? */

      for (j = 0; j < s3InfoRec.clocks; j++) {
	 switch(s3RamdacType) {
	 case NORMAL_DAC:
	    /* only suports 8bpp -- nothing to do */
	    break;
	 case BT485_DAC:
	 case ATT20C505_DAC:
	    /* XXXX What happens here for 16bpp/32bpp ? */
	    break;
	 case TI3020_DAC:
	    switch (s3Bpp) {
	    case 1:
	       break;
	    case 2:
	       s3InfoRec.clock[j] /= 2;
	       clocksChanged = TRUE;
	       break;
	    case 4:
	       s3InfoRec.clock[j] /= 4;
	       clocksChanged = TRUE;
	       break;
	    }
	    break;
	 case ATT20C498_DAC:
	 case ATT22C498_DAC:
	 case STG1700_DAC:
	 case STG1703_DAC:	/* XXXX should this be here? */
	    switch (s3Bpp) {
	    case 1:
	       if (!numClocksChanged) {
		  newNumClocks = 32;
		  numClocksChanged = TRUE;
		  clocksChanged = TRUE;
		  for(i = s3InfoRec.clocks; i < newNumClocks; i++)
		     s3InfoRec.clock[i] = 0;  /* XXXX is clock[] initialized? */
		  if (s3InfoRec.clocks > 16) 
		     s3InfoRec.clocks = 16;
	       }
	       if (s3InfoRec.clock[j] * 2 > pixMuxMinClock &&
		   s3InfoRec.clock[j] * 2 <= s3InfoRec.dacSpeeds[0])
		  s3InfoRec.clock[j + 16] = s3InfoRec.clock[j] * 2;
	       else
		  s3InfoRec.clock[j + 16] = 0;
	       if (s3InfoRec.clock[j] > nonMuxMaxClock)
		  s3InfoRec.clock[j] = 0;
	       break;
	    case 2:
	       /* No change for 16bpp */
	       break;
	    case 4:
	       s3InfoRec.clock[j] /= 2;
	       clocksChanged = TRUE;
	       break;
	    }
	    break;
	 case ATT20C490_DAC:
	 case SS2410_DAC:	/* GUESSING!! (based on 490)  */
	 case SC1148x_M2_DAC:
	    if (s3Bpp > 1) {
	       s3InfoRec.clock[j] /= s3Bpp;
	       clocksChanged = TRUE;
	    }
	    break;
	 case SC15025_DAC:
	    if (s3Bpp > 1) {
	       s3InfoRec.clock[j] /= s3Bpp;
	       clocksChanged = TRUE;
	    }
	    break;
	 case TI3025_DAC:
	 case TI3026_DAC:
	 case TI3030_DAC:
	 case IBMRGB524_DAC:
	 case IBMRGB525_DAC:
	 case IBMRGB528_DAC:
	 case S3_SDAC_DAC:
	 case S3_GENDAC_DAC:
	 case S3_TRIO32_DAC:
	 case S3_TRIO64_DAC:
	 case S3_TRIO64V2_DAC:
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


	/***********************************************\
	|	Set maxDisplayHeight and Width 		|
	\***********************************************/

   if (S3_911_SERIES(s3ChipId)) {
      maxDisplayWidth = 1024;
      maxDisplayHeight = 1024 - 1; /* Cursor takes exactly 1 line for 911 */
   } else if ((OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options) &&
	       DAC_IS_BT485_SERIES) ||
	      (OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options) &&
	       DAC_IS_TI3020_SERIES) ||
	      (OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options) &&
	       (DAC_IS_TI3026 || DAC_IS_TI3030)) ||
	      (OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options) &&
	       DAC_IS_IBMRGB) ||
	      OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options)) {
      maxDisplayWidth = 2048;
      maxDisplayHeight = 4096;
   } else {
      maxDisplayWidth = 2048;
      maxDisplayHeight = 4096 - 3; /* Cursor can take up to 3 lines */
   }

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
 

	/*******************************\
	|	Validate Modes		|
	\*******************************/

   if (pixMuxPossible && (s3InfoRec.videoRam > nonMuxMaxMemory))
      pixMuxNeeded = TRUE;
 
   lookupFlags = LOOKUP_DEFAULT;

   TempVirtualX = s3InfoRec.virtualX;
   TempVirtualY = s3InfoRec.virtualY;


redo_mode_lookup:

   OldPixMuxStatus = pixMuxNeeded;
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
      if (!xf86LookupMode(pMode, &s3InfoRec, lookupFlags)) {
	 xf86DeleteMode(&s3InfoRec, pMode);
      } 
      else {
   	not_safe = FALSE;

	if(s3ValidMode(pMode,TRUE,MODE_USED) != MODE_OK)
	   xf86DeleteMode(&s3InfoRec, pMode);
	else {
	 /*
	  * Successfully looked up this mode.  If pEnd isn't
	  * initialized, set it to this mode.
	  */
	    if (pEnd == (DisplayModePtr) NULL)
	       pEnd = pMode;
	}

   	not_safe = TRUE;
      }

       pMode = pModeSv;
   } while (pMode != pEnd);


   if(OldPixMuxStatus != pixMuxNeeded)
   {
   /* If the decision that pixel multiplexing is needed for all
	modes has been made sometime within the above do loop (most 
	likely due to !allowPixMuxSwitching), there is the possibility
	that some modes returned MODE_OK before the constraint
	of manditory pixel multiplexing was added. In this case
	we should recheck all modes under the new constraint (MArk)*/	
 
     ErrorF("%s %s: One or more modes require pixel multiplexing and\n",
		 XCONFIG_PROBED, s3InfoRec.name);
     ErrorF("\tthe hardware is incapable of switching between mux/non-mux\n");
     ErrorF("\tmodes. All modes will be re-evaluated under the new "
	   "constraint.\n");
	
     goto redo_mode_lookup;	
   }


   if ((TempVirtualX != s3InfoRec.virtualX) || 
			(TempVirtualY != s3InfoRec.virtualY)) {
      OFLG_CLR(XCONFIG_VIRTUAL,&s3InfoRec.xconfigFlag);
 	/* These will never get overwritten if they were specified 
		in the XF86Config file */
      s3InfoRec.virtualX = TempVirtualX;
      s3InfoRec.virtualY = TempVirtualY;
   }

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
 
   /*
    *  If we had to delete a mode due to the inability to switch
    *  between mux and non-mux modes, we favor the mux modes and
    *  print a little message telling why we had to delete it. (MArk)
    */
   if (pixMuxPossible && pixMuxNeeded) {
      if(!pixMuxWidthOK || !pixMuxInterlaceOK || !pixMuxClockOK)
      {
	 ErrorF("%s %s: Modes that require pixel multiplexing and modes\n"
	  "\tthat can't use it are both present in the XF86Config file but\n"
	  "\tthe hardware is incapable of switching between the two.\n",
			XCONFIG_PROBED, s3InfoRec.name);
	 ErrorF("\tModes that can't pixel multiplex are being sacrificed for\n"
	  "the following reason(s):\n");
      
	
        if (!pixMuxWidthOK) 
	    ErrorF("\t\tWidth is too narrow for multiplexing (min = %d).\n",
		pixMuxMinWidth);
        if (!pixMuxInterlaceOK) 
	    ErrorF("\t\tMultiplexing isn't possible for interlaced modes.\n");
        if (!pixMuxClockOK) 
	    ErrorF("\t\tClock is too low for multiplexing (min = %d).\n",
		pixMuxMinClock);
      }
   }

   /* s3ValidMode decided if s3UsingPixMux */
   if (xf86Verbose && s3UsingPixMux)
	ErrorF("%s %s: Operating RAMDAC in pixel multiplex mode\n",
		   XCONFIG_PROBED, s3InfoRec.name);



   /* pixmux on Bt485 requires use of Bt485's cursor */
   if (((s3Bt485PixMux && s3UsingPixMux) ||
	OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
	OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options)) &&
       !OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options)) {
      OFLG_SET(OPTION_BT485_CURS, &s3InfoRec.options);
      ErrorF("%s %s: Using hardware cursor from Bt485/20C505 RAMDAC\n",
	     XCONFIG_PROBED, s3InfoRec.name);
   }



   if (DAC_IS_BT485_SERIES || DAC_IS_TI3020_SERIES || DAC_IS_TI3026 
        || DAC_IS_TI3030 || DAC_IS_IBMRGB) {
      if (!OFLG_ISSET(OPTION_DAC_6_BIT, &s3InfoRec.options) || s3Bpp > 1)
	 s3DAC8Bit = TRUE;
      if (OFLG_ISSET(OPTION_SYNC_ON_GREEN, &s3InfoRec.options)) {
	 s3DACSyncOnGreen = TRUE;
	 if (xf86Verbose)
	    ErrorF("%s %s: Putting RAMDAC into sync-on-green mode\n",
		   XCONFIG_GIVEN, s3InfoRec.name);
      }
   }

   if (DAC_IS_SC15025 || DAC_IS_ATT498 || DAC_IS_STG1700 || DAC_IS_ATT20C409) {
      if (!OFLG_ISSET(OPTION_DAC_6_BIT, &s3InfoRec.options) || s3Bpp > 1)
         s3DAC8Bit = TRUE;
   }

   if (DAC_IS_ATT490) {
      if (OFLG_ISSET(OPTION_DAC_8_BIT, &s3InfoRec.options) || s3Bpp > 1)
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
      else if (xf86bpp == 24)
	 ErrorF("%s %s: Using packed 24 bpp.  Color weight: %1d%1d%1d\n",
		XCONFIG_GIVEN, s3InfoRec.name, xf86weight.red,
		xf86weight.green, xf86weight.blue);
      else if (s3InfoRec.bitsPerPixel == 32)
	 ErrorF("%s %s: Using sparse 32 bpp.  Color weight: %1d%1d%1d\n",
		XCONFIG_GIVEN, s3InfoRec.name, xf86weight.red,
		xf86weight.green, xf86weight.blue);
   }


	/*******************************************************\
   	|	Select the appropriate logical line width 	|
	\*******************************************************/

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
   } else if (S3_911_SERIES(s3ChipId)) {
      s3DisplayWidth = 1024;
   } else {
      if (s3InfoRec.virtualX <= 640) {
	 s3DisplayWidth = 640;
      } else if (s3InfoRec.virtualX <= 800) {
	 s3DisplayWidth = 800;
      } else if (s3InfoRec.virtualX <= 1024) {
	 s3DisplayWidth = 1024;
      } else if ((s3InfoRec.virtualX <= 1152) &&
		 (   S3_801_REV_C(s3ChipId) 
                  || S3_805_I_SERIES(s3ChipId)
		  || S3_928_REV_E(s3ChipId)
		  || S3_x64_SERIES(s3ChipId))) {
	 s3DisplayWidth = 1152;
      } else if (s3InfoRec.virtualX <= 1280) {
	 s3DisplayWidth = 1280;
      } else if ((s3InfoRec.virtualX <= 1600) && 
		 ((S3_928_REV_E(s3ChipId) &&
		   !(OFLG_ISSET(OPTION_NUMBER_NINE, &s3InfoRec.options) &&
		     (s3Bpp == 1)))
		  || S3_x64_SERIES(s3ChipId))) {
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
   s3BppDisplayWidth = s3Bpp * s3DisplayWidth;
   /*
    * Work out where to locate S3's HW cursor storage.  It must be on a
    * 1k boundary.  When using a RAMDAC cursor, set s3CursorStartY
    * and s3CursorLines appropriately for the memory usage calculation below
    */

   if (OFLG_ISSET(OPTION_BT485_CURS, &s3InfoRec.options) ||
       OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options) ||
       OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options) ||
       OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options) ||
       OFLG_ISSET(OPTION_SW_CURSOR, &s3InfoRec.options)) {
      s3CursorStartY = s3InfoRec.virtualY;
      s3CursorLines = 0;
   } else {
      int st_addr = (s3InfoRec.virtualY * s3BppDisplayWidth + 1023) & ~1023;
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
   if ((s3BppDisplayWidth * (s3CursorStartY + s3CursorLines)) >
       s3InfoRec.videoRam * 1024) { /* XXXX improve this message */
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

   if (OFLG_ISSET(OPTION_PCI_HACK, &s3InfoRec.options))
      s3PCIHack = TRUE;
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
	 if (!s3Port59) { /* CR59/CR5A not yet initialized */
	    s3Port59 =  0xf3000000 >> 24;
	    s3Port5A = (0xf3000000 >> 16) & 0x08;
	 }
      }
   }


#ifdef XFreeXDGA
      s3InfoRec.displayWidth = s3DisplayWidth;
      s3InfoRec.directMode = XF86DGADirectPresent;
#endif

#ifdef DPMSExtension
      /* Only supported at the moment for Trio, 864/964 and later. */
      if (S3_x64_SERIES(s3ChipId))
	s3InfoRec.DPMSSet = s3DPMSSet;
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

static int
s3ValidMode(DisplayModePtr pMode, Bool verbose, int flag)
{
    Bool ModeCantPixmux = FALSE;

    if(in_s3Probe && not_safe)
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
    } else if((pMode->HDisplay * (1 + pMode->VDisplay) * s3Bpp) >
		 s3InfoRec.videoRam * 1024) {
	if(verbose)
	   ErrorF("%s %s: Too little memory for mode \"%s\"\n", XCONFIG_PROBED,
		s3InfoRec.name, pMode->name);
	if (!OFLG_ISSET(OPTION_BT485_CURS,  &s3InfoRec.options) &&
	     !OFLG_ISSET(OPTION_TI3020_CURS, &s3InfoRec.options) &&
	     !OFLG_ISSET(OPTION_TI3026_CURS, &s3InfoRec.options) &&
	     !OFLG_ISSET(OPTION_IBMRGB_CURS, &s3InfoRec.options) &&
	     !OFLG_ISSET(OPTION_SW_CURSOR,   &s3InfoRec.options))
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



    if(in_s3Probe)
    {
	/* these will never actually change if a virtual size was
		explicitly given in the XF86Config file */
	 TempVirtualX = max(TempVirtualX, pMode->HDisplay);
	 TempVirtualY = max(TempVirtualY, pMode->VDisplay);
    }


    /* unpleasant pixmux complications */
    if (pixMuxPossible) {

     /* Find out if the mode requires pixmux */

 	if ((s3Bpp == 1) && s3ATT498PixMux && !DAC_IS_SDAC &&
		(S3_864_SERIES(s3ChipId) || S3_805_I_SERIES(s3ChipId))
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

	if((pMode->Flags & V_PIXMUX) && ModeCantPixmux) {
	/* Is this correct?  What else can we do? */
	   if(verbose)
	     ErrorF("%s %s: Mode \"%s\" is impossible for this hardware "
		"due to conflicting pixel multiplexing requirements.\n"
		XCONFIG_PROBED, s3InfoRec.name, pMode->name);
	   return MODE_BAD;
	}

	if (!in_s3Probe && !allowPixMuxSwitching ) {  
	    /* This is only for the vidmode extension case */

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
		   ErrorF("\tincompatible with the current modes.\n");
		}
		return MODE_BAD;
	    }
	} else if (in_s3Probe && !allowPixMuxSwitching ) { 
		/* this is for the initial modes */ 
		if(pMode->Flags & V_PIXMUX)
		    pixMuxNeeded = TRUE;
		else if(pixMuxNeeded && ModeCantPixmux) {
		  if(verbose) {
		     ErrorF("%s %s: Mode \"%s\" can't work with pixel "
			"multiplexing and is\n", XCONFIG_PROBED,
			 s3InfoRec.name, pMode->name);
		     ErrorF("\tincompatible with the current modes.\n");
		  }
		  /* set flags so we can get the error messages correct */
		  if (pMode->HDisplay < pixMuxMinWidth)
			pixMuxWidthOK = FALSE;
		  if (pMode->Flags & V_INTERLACE)
			pixMuxInterlaceOK = FALSE;
		  if (s3InfoRec.clock[pMode->Clock] < pixMuxMinClock)
			pixMuxClockOK = FALSE;
		  return MODE_BAD;
		}
		else if(pixMuxNeeded && !ModeCantPixmux)
		    pMode->Flags |= V_PIXMUX;
		/* otherwise we're not pixmuxing and we don't
		   need to be so everything is fine */
	}

	if((pMode->Flags & V_PIXMUX) && in_s3Probe)
		s3UsingPixMux = TRUE;

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
	 case BT485_DAC:
	    {
	       int c;

	       if (OFLG_ISSET(OPTION_STB_PEGASUS, &s3InfoRec.options) ||
		   OFLG_ISSET(OPTION_MIRO_MAGIC_S4, &s3InfoRec.options))
		  c = 85000;
	       else if (S3_964_SERIES(s3ChipId) && s3Bpp == 4)
		  c = 90000;
	       else
		  c = 67500;
	       if (pMode->SynthClock > c) {
		  pMode->SynthClock /= 2;
		  pMode->Flags |= V_DBLCLK;
	       }
	    }
	    break;
	 case ATT20C505_DAC:
	    if (pMode->SynthClock > 90000) {
	       pMode->SynthClock /= 2;
	       pMode->Flags |= V_DBLCLK;
	    }
	    break;
	 case TI3020_DAC:
	    if (pMode->SynthClock > 100000) {
	       pMode->SynthClock /= 2;
	       pMode->Flags |= V_DBLCLK;
	    }
	    break;
	 case TI3025_DAC:
	    if (pMode->SynthClock > 80000) {
               /* the SynthClock will be divided and clock doubled by the PLL */
	       pMode->Flags |= V_DBLCLK;
	    }
	    break;
	 case TI3026_DAC:  /* IBMRGB??? */
	 case TI3030_DAC:
            if (OFLG_ISSET(CLOCK_OPTION_ICD2061A, &s3InfoRec.clockOptions)) {
               /*
                * for the mixed Ti3026/3030 + ICD2061A cases we need to split
                * at 120MHz; Since the ICD2061A clock code dislikes 120MHz
                * we already double for that
                */
	       if (pMode->SynthClock >= 120000) {
	          pMode->Flags |= V_DBLCLK;
	          pMode->SynthClock /= 2;
	       }
	    } else {
	       /*
	        * use the Ti3026/3030 clock
	        */
	       if (pMode->SynthClock > 80000) {
                  /* 
                   * the SynthClock will be divided and clock doubled 
                   * by the PLL 
                   */
	          pMode->Flags |= V_DBLCLK;
	       }
	    }
	    break;
	 case IBMRGB524_DAC:
	 case IBMRGB525_DAC:
	 case IBMRGB528_DAC:
	    if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
	       if (pMode->SynthClock > 80000 && s3Bpp == 1)
		  pMode->Flags |= V_DBLCLK;
	    }
	    else if (pMode->SynthClock > 80000 || S3_968_SERIES(s3ChipId)) {
	       pMode->Flags |= V_DBLCLK;
	    }
	    break;
	 case ATT20C498_DAC:
	 case ATT22C498_DAC:
	 case ATT20C409_DAC:
	 case STG1700_DAC:
	 case STG1703_DAC:
	 case S3_SDAC_DAC:
	    switch (s3Bpp) {
	    case 1:
	       /*
	        * This one depend on pixel multiplexing for 8bpp.
	        * Although existing code implies it depends on ramdac
	        * clock doubling instead (are the two tied together?)
	        * We'll act based on clock doubling changeover at 67500
	        */
	       if (( DAC_IS_ATT20C498 && pMode->SynthClock > nonMuxMaxClock) ||
		   (!DAC_IS_ATT20C498 && pMode->SynthClock > 67500)) {
		  if (!(DAC_IS_SDAC)) {
		     pMode->SynthClock /= 2;
		     pMode->Flags |= V_DBLCLK;
		  }
	       }
	       break;
	    case 2:
	       /* No change for 16bpp */
	       break;
	    case 4:
	       pMode->SynthClock *= 2;
	       break;
	    }
	    break;
	 case S3_TRIO32_DAC:
	 case S3_TRIO64_DAC:
	 case S3_TRIO64V2_DAC:
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
	 case ATT20C490_DAC:
 	 case SS2410_DAC:    /* just guessing ( based on 490 ) */
	 case SC1148x_M2_DAC:
	 case SC15025_DAC:
	 case S3_GENDAC_DAC:
	    if (s3Bpp > 1) {
	       pMode->SynthClock *= s3Bpp;
	    }
	    break;
	 default:
	    /* Do nothing */
	    break;
   }
         
   /* Setup the Mode.Private if required */
   if (S3_964_SERIES(s3ChipId) || S3_968_SERIES(s3ChipId)) {
	 if (!pMode->PrivSize || !pMode->Private) {
	    pMode->PrivSize = S3_MODEPRIV_SIZE;
	    pMode->Private = (INT32 *)xcalloc(sizeof(INT32), S3_MODEPRIV_SIZE);
	    pMode->Private[0] = 0;
	 }

     	/* Set default for invert_vclk */
   	if (!(pMode->Private[0] & (1 << S3_INVERT_VCLK))) {
	    if (DAC_IS_TI3026 && (s3BiosVendor == DIAMOND_BIOS ||
				  OFLG_ISSET(OPTION_DIAMOND,
				  &s3InfoRec.options)))
	       pMode->Private[S3_INVERT_VCLK] = 1;
	    else if (DAC_IS_TI3030) 
	       if ((s3Bpp == 2 && (pMode->Flags & V_DBLCLK)) || s3Bpp == 4)
		  pMode->Private[S3_INVERT_VCLK] = 1;
	       else 
		  pMode->Private[S3_INVERT_VCLK] = 0;
	    else if (DAC_IS_IBMRGB)
	       if (OFLG_ISSET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options)) {
		  if (s3Bpp == 1)
		     pMode->Private[S3_INVERT_VCLK] = 1;
		  else 
		     pMode->Private[S3_INVERT_VCLK] = 0;
	       } 
	       else if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options))
		  pMode->Private[S3_INVERT_VCLK] = 0;
	       else if (s3Bpp == 4) 
		  pMode->Private[S3_INVERT_VCLK] = 0;
	       else if (s3BiosVendor == STB_BIOS && s3Bpp == 2 
			&& s3InfoRec.clock[pMode->Clock] > 125000 
			&& s3InfoRec.clock[pMode->Clock] < 175000)
		  pMode->Private[S3_INVERT_VCLK] = 0;
	       else if ((s3BiosVendor == NUMBER_NINE_BIOS ||
			 s3BiosVendor == HERCULES_BIOS) &&
			S3_968_SERIES(s3ChipId))
		  pMode->Private[S3_INVERT_VCLK] = 0;
	       else
		  pMode->Private[S3_INVERT_VCLK] = 1;
	    else 
	       pMode->Private[S3_INVERT_VCLK] = 0;
	    pMode->Private[0] |= 1 << S3_INVERT_VCLK;
    	}

    	/* Set default for blank_delay */
    	if (!(pMode->Private[0] & (1 << S3_BLANK_DELAY))) {
	    pMode->Private[0] |= (1 << S3_BLANK_DELAY);
	    if (S3_964_SERIES(s3ChipId) && DAC_IS_BT485_SERIES) {
	       if ((pMode->Flags & V_DBLCLK) || s3Bpp > 1)
		  pMode->Private[S3_BLANK_DELAY] = 0x00;
	       else
		  pMode->Private[S3_BLANK_DELAY] = 0x01;
	    } else if (DAC_IS_TI3025) {
	       if (s3Bpp == 1)
		  if (pMode->Flags & V_DBLCLK)
		     pMode->Private[S3_BLANK_DELAY] = 0x02;
		  else
		     pMode->Private[S3_BLANK_DELAY] = 0x03;
	       else if (s3Bpp == 2)
		  if (pMode->Flags & V_DBLCLK)
		     pMode->Private[S3_BLANK_DELAY] = 0x00;
		  else
		     pMode->Private[S3_BLANK_DELAY] = 0x01;
	       else /* (s3Bpp == 4) */
		  pMode->Private[S3_BLANK_DELAY] = 0x00;
	    } else if (DAC_IS_TI3026) {
	       if (s3BiosVendor == DIAMOND_BIOS 
                   || OFLG_ISSET(OPTION_DIAMOND, &s3InfoRec.options)) {
	          if (s3Bpp == 1) 
		     pMode->Private[S3_BLANK_DELAY] = 0x72;
	          else if (s3Bpp == 2) 
		     pMode->Private[S3_BLANK_DELAY] = 0x73;
	          else /*if (s3Bpp == 4)*/ 
		     pMode->Private[S3_BLANK_DELAY] = 0x75;
	       } else {
	          if (s3Bpp == 1) 
		     pMode->Private[S3_BLANK_DELAY] = 0x00;
	          else if (s3Bpp == 2) 
		     pMode->Private[S3_BLANK_DELAY] = 0x01;
	          else /*if (s3Bpp == 4)*/ 
		     pMode->Private[S3_BLANK_DELAY] = 0x00;
	       }
	    } else if (DAC_IS_TI3030){
	       if (s3Bpp == 1 || (s3Bpp == 2 && !(pMode->Flags & V_DBLCLK)))
		  pMode->Private[S3_BLANK_DELAY] = 0x01;
	       else
		  pMode->Private[S3_BLANK_DELAY] = 0x00;
	    } else if (DAC_IS_IBMRGB) {
	       if (s3BiosVendor == GENOA_BIOS) {
		  pMode->Private[S3_BLANK_DELAY] = 0x00;
	       }
	       else if (s3BiosVendor == STB_BIOS) {
		  if (s3Bpp == 1 && s3InfoRec.clock[pMode->Clock] > 50000)
		     pMode->Private[S3_BLANK_DELAY] = 0x55;
		  else
		     pMode->Private[S3_BLANK_DELAY] = 0x00;
	       }
	       else if (s3BiosVendor == HERCULES_BIOS) {
		 if (S3_968_SERIES(s3ChipId)) {
		   pMode->Private[S3_BLANK_DELAY] = 0x00;
		 }
		 else {
		   pMode->Private[S3_BLANK_DELAY] = (4/s3Bpp) - 1;
		   if (pMode->Flags & V_DBLCLK) 
		     pMode->Private[S3_BLANK_DELAY] >>= 1; 
		 }
	       }
	       else if (OFLG_ISSET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options)) {
		  if (s3Bpp == 2 && s3InfoRec.clock[pMode->Clock] >  220000)
		     pMode->Private[S3_BLANK_DELAY] = 0x00;
		  else if (s3Bpp == 4 && s3InfoRec.clock[pMode->Clock] >  110000)
		     pMode->Private[S3_BLANK_DELAY] = 0x00;
		  else 
		     pMode->Private[S3_BLANK_DELAY] = 0x06;
	       }
	       else if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
		  if (s3Bpp == 1 && (pMode->Flags & V_DBLCLK))
		     pMode->Private[S3_BLANK_DELAY] = 0x21;
		  else if (s3Bpp == 4 && s3InfoRec.clock[pMode->Clock] >  135000)
		     pMode->Private[S3_BLANK_DELAY] = 0x01;
		  else 
		     pMode->Private[S3_BLANK_DELAY] = 0x10;
	       }
	       else
		  pMode->Private[S3_BLANK_DELAY] = 0x00;
	    } else {
	       pMode->Private[S3_BLANK_DELAY] = 0x00;
	    }
    	}
	 
    	/* Set default for early_sc */      
    	if (!(pMode->Private[0] & (1 << S3_EARLY_SC))) {
	    pMode->Private[0] |= 1 << S3_EARLY_SC;
	    if (DAC_IS_TI3025) {
	       if (OFLG_ISSET(OPTION_NUMBER_NINE,&s3InfoRec.options))
		  pMode->Private[S3_EARLY_SC] = 1;
	       else
		  pMode->Private[S3_EARLY_SC] = 0;
	    } else if ((DAC_IS_TI3026 || DAC_IS_TI3030)
		       && OFLG_ISSET(CLOCK_OPTION_ICD2061A,
				     &s3InfoRec.clockOptions)) {
	       if (s3Bpp == 2 && (pMode->Flags & V_DBLCLK))
		  pMode->Private[S3_EARLY_SC] = 1;
	       else
		  pMode->Private[S3_EARLY_SC] = 0;
	    } else if (DAC_IS_TI3026 
		       && OFLG_ISSET(CLOCK_OPTION_ICD2061A,
				     &s3InfoRec.clockOptions)) {
	       if (s3Bpp == 2 && (pMode->Flags & V_DBLCLK))
		  pMode->Private[S3_EARLY_SC] = 1;
	       else
		  pMode->Private[S3_EARLY_SC] = 0;
	    } else if (DAC_IS_IBMRGB) {
	       if (s3BiosVendor == GENOA_BIOS) {
	          pMode->Private[S3_EARLY_SC] = 0;
	       }
	       else if (s3BiosVendor == STB_BIOS) {
		  if (s3Bpp == 2 && s3InfoRec.clock[pMode->Clock] > 125000)
		     pMode->Private[S3_EARLY_SC] = 0;
		  else if (s3Bpp == 4)
		     pMode->Private[S3_EARLY_SC] = 0;
		  else 
		     pMode->Private[S3_EARLY_SC] = 1;
	       }
	       else if (s3BiosVendor == HERCULES_BIOS) {
		 if (S3_968_SERIES(s3ChipId))
		   pMode->Private[S3_EARLY_SC] = 0;
		 else
		   pMode->Private[S3_EARLY_SC] = 0;
	       }
	       else if (OFLG_ISSET(OPTION_ELSA_W2000PRO_X8,  &s3InfoRec.options)) {
		  pMode->Private[S3_EARLY_SC] = 0;
	       }
	       else if (OFLG_ISSET(OPTION_MIRO_80SV,  &s3InfoRec.options)) {
		  pMode->Private[S3_EARLY_SC] = 0;
	       }
	       else
	          pMode->Private[S3_EARLY_SC] = 0;
	    } else {
	       pMode->Private[S3_EARLY_SC] = 0;
	    }
    	}
    }

   return MODE_OK;
}
