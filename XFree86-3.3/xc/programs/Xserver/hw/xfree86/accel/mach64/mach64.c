/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64.c,v 3.62.2.9 1997/06/02 01:44:13 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993,1994,1995,1996 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * THOMAS ROELL, KEVIN E. MARTIN, AND RICKARD E. FAITH DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Rewritten for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Rewritten for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 * Support for the Mach64 CT added by David Dawes (dawes@XFree86.org)
 *
 */
/* $XConsortium: mach64.c /main/34 1996/10/28 04:46:47 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "mipointer.h"
#include "cursorstr.h"
#include "mi.h"
#include "cfb.h"
#include "cfb16.h"
#include "cfb32.h"
#include "gc.h"
#include "windowstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "mach64.h"
#ifdef PIXPRIV
#include "mach64im.h"
#endif

#ifdef DPMSExtension
#include "opaque.h"
#include "extensions/dpms.h"
#endif

#include "xf86_PCI.h"

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

extern int defaultColorVisualClass;
extern int mach64MaxClock;
extern Bool xf86Resetting, xf86Exiting, xf86ProbeFailed;
extern void mach64QueryBestSize();
extern void mach64WarpCursor();
extern void mach64RepositionCursor();
extern Bool miDCInitialize();
extern void SetTimeSinceLastInputEvent();
unsigned int mach64MemorySize = 0;
unsigned int mach64ApertureSize = 0;
unsigned long mach64ApertureAddr = 0;
extern char *xf86VisualNames[];

#ifdef PIXPRIV
int mach64PixmapIndex;
#endif

static int mach64ValidMode(
#if NeedFunctionPrototypes
    DisplayModePtr,
    Bool,
    int
#endif
); 

ScrnInfoRec mach64InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    mach64Probe,      	/* Bool (* Probe)() */
    mach64Initialize,	/* Bool (* Init)() */
    mach64ValidMode,	/* int (* ValidMode)() */
    mach64EnterLeaveVT, /* void (* EnterLeaveVT)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveMonitor)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveCursor)() */
    mach64AdjustFrame,	/* void (* AdjustFrame)() */
    mach64SwitchMode,	/* Bool (* SwitchMode)() */
    mach64DPMSSet	,/* void (* DPMSSet)() */
    mach64PrintIdent,	/* void (* PrintIdent)() */
    8,			/* int depth */
    {5, 6, 5},          /* xrgb weight */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1,                 /* int displayWidth */
    -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },	       	/* OFlagSet options */
    {0, },	       	/* OFlagSet clockOptions */
    {0, },	       	/* OFlagSet xconfigFlag */
    NULL,	       	/* char *chipset */
    NULL,               /* char *ramdac */
    {0, 0, 0, 0},	/* int dacSpeeds[MAXDACSPEEDS] */
    0,			/* int dacSpeedBpp */
    0,			/* int clocks */
    {0, },		/* int clock[MAXCLOCKS] */
    0,			/* int maxClock */
    0,			/* int videoRam */
    0xC0000,            /* int BIOSbase */   
    0,			/* unsigned long MemBase */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,               /* MonPtr monitor */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock */   
    FALSE,              /* Bool bankedMono */
    "Mach64",           /* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    mach64ValidTokens,	/* int *validTokens */
    MACH64_PATCHLEVEL,	/* char *patchlevel */
    0,                  /* int IObase */
    0,                  /* int PALbase */
    0,                  /* int COPbase */
    0,                  /* int POSbase */
    0,                  /* int instance */
    0,                  /* int s3Madjust */
    0,                  /* int s3Nadjust */
    0,                  /* int s3MClk */
    0,                  /* int chipID */
    0,                  /* int chipRev */
    0,                  /* unsigned long VGAbase */
    0,                  /* int s3RefClk */
    -1,                 /* int s3BlankDelay */
    0,			/* int textClockFreq */
    NULL,               /* char* DCConfig */
    NULL,               /* char* DCOptions */
    0			/* int MemClk */
#ifdef XFreeXDGA
    ,0,                  /* int directMode */
    NULL,               /* Set Vid Page */
    0,                  /* unsigned long physBase */
    0                  /* int physSize */
#endif
};

int mach64alu[16] = {
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

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

LUTENTRY mach64savedLUT[256];
static Bool LUTissaved = FALSE;

int mach64MaxX, mach64MaxY;
int mach64VirtX, mach64VirtY;

Bool mach64Use4MbAperture = FALSE;
Bool mach64DAC8Bit = FALSE;

static unsigned Mach64_IOPorts[] = {
	/* VGA Registers */
        0x3B4, 0x3B5, 0x3BA, 0x3C0, 0x3C1, 0x3C2, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 
	0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 0x3CE, 0x3CF, 0x3D4, 0x3D5, 0x3DA,
	/* ATI VGA Registers */
	sioATIEXT, sioATIEXT+1,
	/* Mach64 Registers */
	0x400,	/* This is enough to ensure access to all registers */
};
static int Num_Mach64_IOPorts = (sizeof(Mach64_IOPorts)/
				 sizeof(Mach64_IOPorts[0]));

short mach64WeightMask;

static mach64CRTCRegRec mach64CRTCRegs;
static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;
static int mach64HWCursorSave = -1;
unsigned ioCONFIG_CHIP_ID;
unsigned ioCONFIG_CNTL;
unsigned ioSCRATCH_REG0;
unsigned ioSCRATCH_REG1;
unsigned ioCONFIG_STAT0;
unsigned ioMEM_CNTL;
unsigned ioDAC_REGS;
unsigned ioDAC_CNTL;
unsigned ioGEN_TEST_CNTL;
unsigned ioCLOCK_CNTL;   
unsigned ioCRTC_GEN_CNTL;
unsigned ATIExtReg;

/*
 * ATI Hardware Probe
 *
 */

typedef struct ATIInformationBlock {
   char board_identifier[2];
   char equipment_flags[2];
   char asic_identifier;
   int  bios_major;
   int  bios_minor;
   char bios_date[21];
   int  VGA_Wonder_Present;
   int  Mach64_Present;
   int  Bus_Type;
   int  Mem_Size;
   int  Mem_Type;
   int  DAC_Type;
   int  DAC_SubType;
   int  Clock_Type;
   int  Clocks[MACH64_NUM_CLOCKS];
   mach64FreqRec Freq_Table[MACH64_NUM_FREQS];
   mach64FreqRec Freq_Table2[MACH64_NUM_FREQS];
   int  MinFreq;
   int  MaxFreq;
   int  RefFreq;
   int  RefDivider;
   int  NAdj;
   int  DRAMMemClk;
   int  VRAMMemClk;
   int  MemCycle;
   int  MemClk;
   int  CXClk;
   int  ChipType;
   int  ChipRev;
} ATIInformationBlock;

typedef struct ATIPCIInformation {
   int  ChipType;
   int  ChipRev;
   unsigned long ApertureBase;
   unsigned long IOBase;
   Bool BlockIO;
} ATIPCIInformation;
   
SymTabRec mach64RamdacTableNames[] = {
    { DAC_INTERNAL, "internal" },
    { DAC_IBMRGB514, "ibm_rgb514" },
    { DAC_ATI68875, "ati68875" },
    { DAC_ATI68875, "tlc34075" },
    { DAC_TVP3026_A, "tvp3026" },
    { DAC_BT476, "bt476" },
    { DAC_BT476, "bt478" },
    { DAC_BT476, "inmos176" },
    { DAC_BT476, "inmos178" },
    { DAC_BT481, "bt481" },  
    { DAC_ATT20C491, "att20c491" },
    { DAC_SC15026, "sc15026" },
    { DAC_MU9C1880, "mu9c1880" },
    { DAC_IMSG174, "ims_g174" },
    { DAC_ATI68860_B, "ati68860" },
    { DAC_ATI68860_B, "ati68860b" },
    { DAC_ATI68860_C, "ati68860c" },
    { DAC_TVP3026_B, "tvp3026" },
    { DAC_STG1700, "stg1700" },
    { DAC_ATT498, "att20c498" },
    { DAC_ATT498, "att498" },
    { DAC_STG1702, "stg1702" },
    { DAC_SC15021, "sc15021" },
    { DAC_ATT21C498, "att21c498" },
    { DAC_STG1703, "stg1703" },
    { DAC_CH8398, "ch8398" },
    { DAC_ATT20C408, "att20c408" },
    { -1, "" },
};  

SymTabRec mach64RamdacTable[] = {
    { DAC_INTERNAL, "Internal" },
    { DAC_IBMRGB514, "IBM-RGB514" },
    { DAC_ATI68875, "ATI-68875/TLC34075" },
    { DAC_TVP3026_A, "TVP3026" },
    { DAC_BT476, "Bt476/Bt478/INMOS176/INMOS178" },
    { DAC_BT481, "Bt481" },  
    { DAC_ATT20C491, "AT&T20C491" },
    { DAC_SC15026, "SC15026" },
    { DAC_MU9C1880, "MU9C1880" },
    { DAC_IMSG174, "IMS-G174" },
    { DAC_ATI68860_B, "ATI68860 Rev B" },
    { DAC_ATI68860_C, "ATI68860 Rev C" },
    { DAC_TVP3026_B, "TVP3026" },
    { DAC_STG1700, "STG1700" },
    { DAC_ATT498, "AT&T20C498" },
    { DAC_STG1702, "STG1702" },
    { DAC_SC15021, "SC15021" },
    { DAC_ATT21C498, "AT&T21C498" },
    { DAC_STG1703, "STG1703" },
    { DAC_CH8398, "CH8398" },
    { DAC_ATT20C408, "AT&T20C408" },
    { -1, "" },
};  

SymTabRec mach64ChipTable[] = {
    { MACH64_GX, "Mach64 GX" },
    { MACH64_CX, "Mach64 CX" },
    { MACH64_CT, "Mach64 CT" },
    { MACH64_ET, "Mach64 ET" },
    { MACH64_VT, "Mach64 VT" },
    { MACH64_GT, "Mach64 GT" },
    { -1, "" },
};

char *mach64ClockTypeTable[] = {
    "ATI18818-0",
    "ATI18818-1/ICS2595",
    "STG1703",
    "CH8398",
    "Internal",
    "AT&T20C408",
    "IBM-RGB514",
  };  

#define NUM_CLOCK_TYPES (sizeof(mach64ClockTypeTable) / sizeof(char *))

int	mach64Ramdac;
int	mach64RamdacSubType;
int	mach64BusType;
int	mach64MemType;
int	mach64ChipType;
int	mach64ChipRev;
int	mach64ClockType;
int	mach64Clocks[MACH64_NUM_CLOCKS];
mach64FreqRec mach64FreqTable[MACH64_NUM_FREQS];
mach64FreqRec mach64FreqTable2[MACH64_NUM_FREQS];
int	mach64MinFreq;
int	mach64MaxFreq;
int	mach64RefFreq;
int	mach64RefDivider;
int	mach64NAdj;
int	mach64CXClk;
int	mach64MemClk;
int	mach64DRAMMemClk;
int	mach64VRAMMemClk;
int	mach64MemCycle;
Bool	mach64IntegratedController;

static ATIInformationBlock *GetATIInformationBlock(BlockIO)
    Bool BlockIO;
{
#define BIOS_DATA_SIZE 0x8000
   char                       signature[]    = " 761295520";
   char                       bios_data[BIOS_DATA_SIZE];
   char                       bios_signature[10];
   unsigned short             *sbios_data = (unsigned short *)bios_data;
   int                        tmp,i,j;
   static ATIInformationBlock info = { 0, };
   int                        ROM_Table_Offset;
   int                        Freq_Table_Ptr;
   int                        CDepth_Table_Ptr;
   int                        CTable_Size;

	 
   if (xf86ReadBIOS(mach64InfoRec.BIOSbase, 0x30,
		    (unsigned char *)bios_signature, 10) != 10) {
      return NULL;
   }
   if (strncmp( signature, bios_signature, 10 )) {
#ifdef DEBUG
	 ErrorF("Mach64 probe failed on BIOS signature\n");
#endif
	 return NULL;
   }

   if (xf86ReadBIOS(mach64InfoRec.BIOSbase, 0x00,
		    (unsigned char *)bios_data, BIOS_DATA_SIZE)
       != BIOS_DATA_SIZE) {
      return NULL;
   }

   info.board_identifier[0]    = bios_data[ 0x40 ];
   info.board_identifier[1]    = bios_data[ 0x41 ];
   info.equipment_flags[0]     = bios_data[ 0x42 ];
   info.equipment_flags[1]     = bios_data[ 0x44 ];
   info.asic_identifier        = bios_data[ 0x43 ];
   info.bios_major             = bios_data[ 0x4c ];
   info.bios_minor             = bios_data[ 0x4d ];
   strncpy( info.bios_date, bios_data + 0x50, 20 );
   
   info.VGA_Wonder_Present     = bios_data[ 0x44 ] & 0x40;

   info.Mach64_Present = 1;	/* Test for Mach64 product */

   tmp = inl(ioSCRATCH_REG0);
   outl(ioSCRATCH_REG0, 0x55555555);
   if (inl(ioSCRATCH_REG0) != 0x55555555) {
      info.Mach64_Present = 0;
#ifdef DEBUG
      ErrorF("Mach64 probe failed on read 1 of SCRATCH_REG0 %x\n",
	     ioSCRATCH_REG0);
#endif
   } else {
      outl(ioSCRATCH_REG0, 0xaaaaaaaa);
      if (inl(ioSCRATCH_REG0) != 0xaaaaaaaa) {
	 info.Mach64_Present = 0;
#ifdef DEBUG
          ErrorF("Mach64 probe failed on read 2 of SCRATCH_REG0 %x\n",
	         ioSCRATCH_REG0);
#endif
      }
   }
   outl(ioSCRATCH_REG0, tmp);

   if (!info.Mach64_Present)
	 return &info;

   tmp = inl(ioCONFIG_CHIP_ID);
   if (mach64InfoRec.chipID) {
	ErrorF("%s %s: Mach64 chipset override, using ChipID 0x%04x instead"
		" of 0x%04x\n", XCONFIG_GIVEN, mach64InfoRec.name,
		mach64InfoRec.chipID & CFG_CHIP_TYPE, tmp & CFG_CHIP_TYPE);
	tmp = (tmp & ~CFG_CHIP_TYPE) | (mach64InfoRec.chipID & CFG_CHIP_TYPE);
   }
   switch (tmp & CFG_CHIP_TYPE) {
   case MACH64_GX_ID:
	info.ChipType = MACH64_GX;
	break;
   case MACH64_CX_ID:
	info.ChipType = MACH64_CX;
	break;
   case MACH64_CT_ID:
	info.ChipType = MACH64_CT;
	break;
   case MACH64_ET_ID:
	info.ChipType = MACH64_ET;
	break;
   case MACH64_VT_ID:
	info.ChipType = MACH64_VT;
	break;
   case MACH64_GT_ID:
	info.ChipType = MACH64_GT;
	break;
   default:
	/*
	 * Assume ATI won't produce any more adapters that don't (properly)
	 * register themselves in PCI configuration space.
	 */
	if (BlockIO) {
	    info.ChipType = MACH64_UNKNOWN;
	    break;
	}
	info.Mach64_Present = FALSE;
	return &info;
   }
   if (mach64InfoRec.chipRev) {
	ErrorF("%s %s: Mach64 chipset override, using ChipRev 0x%02x instead"
		" of 0x%02x\n", XCONFIG_GIVEN, mach64InfoRec.name,
		mach64InfoRec.chipRev & 0xFF, (tmp & CFG_CHIP_REV) >> 24);
	info.ChipRev = mach64InfoRec.chipRev & 0xFF;
   } else {
	info.ChipRev = (tmp & CFG_CHIP_REV) >> 24;
   }
#ifdef DEBUG
   ErrorF("CONFIG_CHIP_ID reports: %s rev %d\n",
	  xf86TokenToString(mach64ChipTable, info.ChipType), info.ChipRev);
#endif

   tmp = inl(ioCONFIG_STAT0);
   if (info.ChipType == MACH64_GX || info.ChipType == MACH64_CX) {
	info.Bus_Type = tmp & CFG_BUS_TYPE;
	info.Mem_Type = (tmp & CFG_MEM_TYPE) >> 3;
	info.DAC_Type = (tmp & CFG_INIT_DAC_TYPE) >> 9;
	info.DAC_SubType = inb(ioSCRATCH_REG1+1) & 0xf0 | info.DAC_Type;
   } else {
	info.Mem_Type = tmp & CFG_MEM_TYPE_xT;
	info.DAC_Type = DAC_INTERNAL;
	info.DAC_SubType = DAC_INTERNAL;
#ifdef DEBUG
	ErrorF("CONFIG_STAT0 reports mem type %d\n", info.Mem_Type);
	ErrorF("CONFIG_STAT0 is 0x%08x\n", tmp);
#endif
   }

   tmp = inl(ioMEM_CNTL);
   if ((info.ChipType == MACH64_VT || info.ChipType == MACH64_GT) &&
       (info.ChipRev & 0x01)) {
     switch (tmp & MEM_SIZE_ALIAS_GTB) {
     case MEM_SIZE_512K:
       info.Mem_Size = 512;
       break;
     case MEM_SIZE_1M:
       info.Mem_Size = 1024;
       break;
     case MEM_SIZE_2M_GTB:
       info.Mem_Size = 2*1024;
       break;
     case MEM_SIZE_4M_GTB:
       info.Mem_Size = 4*1024;
       break;
     case MEM_SIZE_6M_GTB:
       info.Mem_Size = 6*1024;
       break;
     case MEM_SIZE_8M_GTB:
       info.Mem_Size = 8*1024;
       break;
     }
   } else {
     switch (tmp & MEM_SIZE_ALIAS) {
     case MEM_SIZE_512K:
       info.Mem_Size = 512;
       break;
     case MEM_SIZE_1M:
       info.Mem_Size = 1024;
       break;
     case MEM_SIZE_2M:
       info.Mem_Size = 2*1024;
       break;
     case MEM_SIZE_4M:
       info.Mem_Size = 4*1024;
       break;
     case MEM_SIZE_6M:
       info.Mem_Size = 6*1024;
       break;
     case MEM_SIZE_8M:
       info.Mem_Size = 8*1024;
       break;
     }
   }

   
   ROM_Table_Offset = sbios_data[0x48 >> 1];
   Freq_Table_Ptr = sbios_data[(ROM_Table_Offset >> 1) + 8];
   info.Clock_Type = bios_data[Freq_Table_Ptr];

   info.MinFreq = sbios_data[(Freq_Table_Ptr >> 1) + 1];
   info.MaxFreq = sbios_data[(Freq_Table_Ptr >> 1) + 2];
   info.RefFreq = sbios_data[(Freq_Table_Ptr >> 1) + 4];
   info.RefDivider = sbios_data[(Freq_Table_Ptr >> 1) + 5];
   info.NAdj = sbios_data[(Freq_Table_Ptr >> 1) + 6];
   info.DRAMMemClk = sbios_data[(Freq_Table_Ptr >> 1) + 8];
   info.VRAMMemClk = sbios_data[(Freq_Table_Ptr >> 1) + 9];
   info.MemClk = bios_data[Freq_Table_Ptr + 22];
   info.CXClk = bios_data[Freq_Table_Ptr + 6];

   CDepth_Table_Ptr = sbios_data[(Freq_Table_Ptr >> 1) - 3];
   Freq_Table_Ptr = sbios_data[(Freq_Table_Ptr >> 1) - 1];

   for (i = 0; i < MACH64_NUM_CLOCKS; i++)
      info.Clocks[i] = sbios_data[(Freq_Table_Ptr >> 1) + i];

   info.MemCycle = bios_data[ROM_Table_Offset + 0];

   CTable_Size = bios_data[CDepth_Table_Ptr - 1];
   for (i = 0, j = 0;
        bios_data[CDepth_Table_Ptr + i] != 0;
        i += CTable_Size, j++) {
     info.Freq_Table[j].h_disp        = bios_data[CDepth_Table_Ptr + i];
     info.Freq_Table[j].dacmask       = bios_data[CDepth_Table_Ptr + i + 1];
     info.Freq_Table[j].ram_req       = bios_data[CDepth_Table_Ptr + i + 2];
     info.Freq_Table[j].max_dot_clock = bios_data[CDepth_Table_Ptr + i + 3];
     info.Freq_Table[j].color_depth   = bios_data[CDepth_Table_Ptr + i + 4];
   }
   info.Freq_Table[j].h_disp = 0;

   if (bios_data[CDepth_Table_Ptr + i + 1] != 0) {
       CDepth_Table_Ptr += i + 2;
       CTable_Size = bios_data[CDepth_Table_Ptr - 1];
       for (i = 0, j = 0;
	    bios_data[CDepth_Table_Ptr + i] != 0;
	    i += CTable_Size, j++) {
	   info.Freq_Table2[j].h_disp        = bios_data[CDepth_Table_Ptr + i];
	   info.Freq_Table2[j].dacmask       = bios_data[CDepth_Table_Ptr + i + 1];
	   info.Freq_Table2[j].ram_req       = bios_data[CDepth_Table_Ptr + i + 2];
	   info.Freq_Table2[j].max_dot_clock = bios_data[CDepth_Table_Ptr + i + 3];
	   info.Freq_Table2[j].color_depth   = bios_data[CDepth_Table_Ptr + i + 4];
       }
       info.Freq_Table2[j].h_disp = 0;
   } else
       info.Freq_Table2[0].h_disp = 0;

   return &info;
}

static ATIPCIInformation *
GetATIPCIInformation()
{
    static ATIPCIInformation info = { 0, };
    pciConfigPtr pcrp, *pcrpp;
    Bool found = FALSE;
    int i = 0;

    pcrpp = xf86scanpci(mach64InfoRec.scrnIndex);

    if (!pcrpp)
	return NULL;

    while (pcrp = pcrpp[i]) {
	if (pcrp->_vendor == PCI_ATI_VENDOR_ID) {
	    found = TRUE;
	    switch (pcrp->_device) {
	    case PCI_MACH64_GX:
		info.ChipType = MACH64_GX;
		break;
	    case PCI_MACH64_CX:
		info.ChipType = MACH64_CX;
		break;
	    case PCI_MACH64_CT:
		info.ChipType = MACH64_CT;
		break;
	    case PCI_MACH64_ET:
		info.ChipType = MACH64_ET;
		break;
	    case PCI_MACH64_VT:
		info.ChipType = MACH64_VT;
		break;
	    case PCI_MACH64_GT:
		info.ChipType = MACH64_GT;
		break;
	    default:
		info.ChipType = MACH64_UNKNOWN;
		break;
	    }
	    info.ChipRev = pcrp->_rev_id;
	    info.ApertureBase = pcrp->_base0 & 0xFFFFFFF0;
	    /*
	     * The docs say check (pcrp->_user_config_0 & 0x04) for BlockIO
	     * but this doesn't seem to be reliable.  Instead check if
	     * pcrp->_base1 is non-zero.
	     */
	    if (pcrp->_base1 & 0xfffffffc) {
		info.BlockIO = TRUE;
		info.IOBase = pcrp->_base1 & (pcrp->_base1 & 0x1 ?
					      0xfffffffc : 0xfffffff0);
		/* If the Block I/O bit isn't set in userconfig, set it */
#ifdef DEBUG
		ErrorF("PCI userconfig0 = 0x%02x\n", pcrp->_user_config_0);
#endif
		if ((pcrp->_user_config_0 & 0x04) != 0x04) {
#ifdef DEBUG
		    ErrorF("Setting bit 0x04 in PCI userconfig for card %d\n",
			   pcrp->_cardnum);
#endif
		    xf86writepci(mach64InfoRec.scrnIndex, pcrp->_bus,
			pcrp->_cardnum, pcrp->_func,
			PCI_REG_USERCONFIG, 0x04, 0x04);
		}
	    } else {
		info.BlockIO = FALSE;
		switch (pcrp->_user_config_0 & 0x03) {
		case 0:
		    info.IOBase = 0x2EC;
		    break;
		case 1:
		    info.IOBase = 0x1CC;
		    break;
		case 2:
		    info.IOBase = 0x1C8;
		    break;
		default:
		    info.IOBase = 0;
		    break;
		}
	    }
	    break;
	}
	i++;
    }

    /* Free PCI information */
    xf86cleanpci();

    if (found && xf86Verbose) {
      if (info.ChipType != MACH64_UNKNOWN) {
	ErrorF("%s %s: PCI: %s rev %d, Aperture @ 0x%08x,"
		" %s I/O @ 0x%04x\n", XCONFIG_PROBED, mach64InfoRec.name,
		xf86TokenToString(mach64ChipTable, info.ChipType),
		info.ChipRev, info.ApertureBase,
		info.BlockIO ? "Block" : "Sparse", info.IOBase);
      } else {
	ErrorF("%s %s: PCI: unknown ATI (%0x04x) rev %d, Aperture @ 0x%08x,"
		" %s I/O @ 0x%04x\n", XCONFIG_PROBED, mach64InfoRec.name,
		pcrp->_device, info.ChipRev, info.ApertureBase,
		info.BlockIO ? "Block" : "Sparse", info.IOBase);
      }
    }
    if (found) {
	return &info;
    } else {
	return NULL;
    }
}

#ifdef DEBUG
void
mach64PrintCTPLL()
{
    int i;
    unsigned char pll[16];
    int R = 1432;
    int M, N, P;

    for (i = 0; i < 16; i++) {
	outb(ioCLOCK_CNTL + 1, i << 2);
	ErrorF("PLL register %2d: 0x%02x\n", i, pll[i] = inb(ioCLOCK_CNTL + 2));
    }
    M = pll[PLL_REF_DIV];

    N = pll[VCLK0_FB_DIV];
    if ((mach64ChipType == MACH64_VT || mach64ChipType == MACH64_GT) &&
	(mach64ChipRev & 0x01) && (pll[PLL_XCLK_CNTL] & 0x10)) {
	switch (pll[VCLK_POST_DIV] & VCLK0_POST) {
	case 0: P = 3; break;
	case 1: P = 2; break; /* Unknown */
	case 2: P = 6; break;
	case 3: P = 12; break;
	}
    } else {
	P = 1 << (pll[VCLK_POST_DIV] & VCLK0_POST);
    }
    ErrorF("VCLK0: M=%d, N=%d, P=%d, Clk=%.2f\n", M, N, P,
	   (double)((2 * R * N)/(M * P)) / 100.0);
    N = pll[VCLK1_FB_DIV];
    if ((mach64ChipType == MACH64_VT || mach64ChipType == MACH64_GT) &&
	(mach64ChipRev & 0x01) && (pll[PLL_XCLK_CNTL] & 0x20)) {
	switch ((pll[VCLK_POST_DIV] & VCLK1_POST) >> 2) {
	case 0: P = 3; break;
	case 1: P = 2; break; /* Unknown */
	case 2: P = 6; break;
	case 3: P = 12; break;
	}
    } else {
	P = 1 << ((pll[VCLK_POST_DIV] & VCLK1_POST) >> 2);
    }
    ErrorF("VCLK1: M=%d, N=%d, P=%d, Clk=%.2f\n", M, N, P,
	   (double)((2 * R * N)/(M * P)) / 100.0);
    N = pll[VCLK2_FB_DIV];
    if ((mach64ChipType == MACH64_VT || mach64ChipType == MACH64_GT) &&
	(mach64ChipRev & 0x01) && (pll[PLL_XCLK_CNTL] & 0x40)) {
	switch ((pll[VCLK_POST_DIV] & VCLK2_POST) >> 4) {
	case 0: P = 3; break;
	case 1: P = 2; break; /* Unknown */
	case 2: P = 6; break;
	case 3: P = 12; break;
	}
    } else {
	P = 1 << ((pll[VCLK_POST_DIV] & VCLK2_POST) >> 4);
    }
    ErrorF("VCLK2: M=%d, N=%d, P=%d, Clk=%.2f\n", M, N, P,
	   (double)((2 * R * N)/(M * P)) / 100.0);
    N = pll[VCLK3_FB_DIV];
    if ((mach64ChipType == MACH64_VT || mach64ChipType == MACH64_GT) &&
	(mach64ChipRev & 0x01) && (pll[PLL_XCLK_CNTL] & 0x80)) {
	switch ((pll[VCLK_POST_DIV] & VCLK3_POST) >> 6) {
	case 0: P = 3; break;
	case 1: P = 2; break; /* Unknown */
	case 2: P = 6; break;
	case 3: P = 12; break;
	}
    } else {
	P = 1 << ((pll[VCLK_POST_DIV] & VCLK3_POST) >> 6);
    }
    ErrorF("VCLK3: M=%d, N=%d, P=%d, Clk=%.2f\n", M, N, P,
	   (double)((2 * R * N)/(M * P)) / 100.0);
    N = pll[MCLK_FB_DIV];
    P = 2;
    ErrorF("MCLK:  M=%d, N=%d, P=%d, Clk=%.2f\n", M, N, P,
	   (double)((2 * R * N)/(M * P)) / 100.0);
}
#endif

static void
InitIOAddresses(base, block)
    unsigned base;
    Bool block;
{
    if (block) {
	ioCONFIG_CHIP_ID = base + CONFIG_CHIP_ID;
	ioCONFIG_CNTL    = base + CONFIG_CNTL;
	ioSCRATCH_REG0   = base + SCRATCH_REG0;
	ioSCRATCH_REG1   = base + SCRATCH_REG1;
	ioCONFIG_STAT0   = base + CONFIG_STAT0;
	ioMEM_CNTL       = base + MEM_CNTL;
	ioDAC_REGS       = base + DAC_REGS;
	ioDAC_CNTL       = base + DAC_CNTL;
	ioGEN_TEST_CNTL  = base + GEN_TEST_CNTL;
	ioCLOCK_CNTL     = base + CLOCK_CNTL;
	ioCRTC_GEN_CNTL  = base + CRTC_GEN_CNTL;
	ATIExtReg        = bioATIEXT;
    } else {
	ioCONFIG_CHIP_ID = base + (sioCONFIG_CHIP_ID << 10);
	ioCONFIG_CNTL    = base + (sioCONFIG_CNTL << 10);
	ioSCRATCH_REG0   = base + (sioSCRATCH_REG0 << 10);
	ioSCRATCH_REG1   = base + (sioSCRATCH_REG1 << 10);
	ioCONFIG_STAT0   = base + (sioCONFIG_STAT0 << 10);
	ioMEM_CNTL       = base + (sioMEM_CNTL << 10);
	ioDAC_REGS       = base + (sioDAC_REGS << 10);
	ioDAC_CNTL       = base + (sioDAC_CNTL << 10);
	ioGEN_TEST_CNTL  = base + (sioGEN_TEST_CNTL << 10);
	ioCLOCK_CNTL     = base + (sioCLOCK_CNTL << 10);
	ioCRTC_GEN_CNTL  = base + (sioCRTC_GEN_CNTL << 10);
	ATIExtReg        = sioATIEXT;
    }
}

/*
 * mach64Probe --
 *     Probe the hardware
 */
Bool
mach64Probe()
{
    int                   i, j;
    DisplayModePtr        pMode, pEnd;
    ATIInformationBlock   *info;
    ATIPCIInformation     *pciInfo;
    int                   available_ram;
    Bool                  sw_cursor_supplied;
    OFlagSet              validOptions;
    int                   tx, ty;
    Bool                  clock_type_probed = TRUE;
    int                   new = -1;

    /* Do the general PCI probe first */
    pciInfo = GetATIPCIInformation();

    if (pciInfo) {
	InitIOAddresses(pciInfo->IOBase, pciInfo->BlockIO);
    } else {
	InitIOAddresses(0x2EC, FALSE);
    }

    xf86ClearIOPortList(mach64InfoRec.scrnIndex);
    xf86AddIOPorts(mach64InfoRec.scrnIndex, Num_Mach64_IOPorts, Mach64_IOPorts);

    xf86EnableIOPorts(mach64InfoRec.scrnIndex);

    if (pciInfo)
	info = GetATIInformationBlock(pciInfo->BlockIO);
    else
	info = GetATIInformationBlock(FALSE);

    /* If probe fails when assuming block I/O, try again with sparse I/O */
    if (pciInfo && pciInfo->BlockIO && info && !info->Mach64_Present) {
	InitIOAddresses(0x2EC, FALSE);
	info = GetATIInformationBlock(pciInfo->BlockIO);
	if (info && info->Mach64_Present) {
	    ErrorF("%s %s: PCI reports Block I/O, but really Sparse I/O"
		   " @ 0x2ec\n", XCONFIG_PROBED, mach64InfoRec.name);
	}
    }

    if (!info || !info->Mach64_Present) {
	xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	return(FALSE);
    }

    if (pciInfo && !mach64InfoRec.chipID) {
	if (pciInfo->ChipType != info->ChipType) {
	    ErrorF("%s %s: PCI (%s) and CONFIG_CHIP_ID (%s) don't agree on"
		   " ChipType,\n"
		   "\tusing PCI value\n", XCONFIG_PROBED, mach64InfoRec.name,
		   xf86TokenToString(mach64ChipTable, pciInfo->ChipType),
		   xf86TokenToString(mach64ChipTable, info->ChipType));
	}
	mach64ChipType = pciInfo->ChipType;
    } else {
	mach64ChipType = info->ChipType;
    }

    if (pciInfo && !mach64InfoRec.chipRev) {
	if (pciInfo->ChipRev != info->ChipRev) {
	    ErrorF("%s %s: PCI (%d) and CONFIG_CHIP_ID (%d) don't agree on"
		   " ChipRev,\n"
		   "\tusing PCI value\n", XCONFIG_PROBED, mach64InfoRec.name,
		   pciInfo->ChipRev, info->ChipRev);
	}
	mach64ChipRev = pciInfo->ChipRev;
    } else {
	mach64ChipRev = info->ChipRev;
    }

    if (!pciInfo) {
	ErrorF("%s %s: %s rev %d, Aperture @ 0x%08x,"
		" %s I/O @ 0x%04x\n", XCONFIG_PROBED, mach64InfoRec.name,
		xf86TokenToString(mach64ChipTable, mach64ChipType),
		mach64ChipRev, (inw(ioCONFIG_CNTL) & 0x3FF0) << 18,
		"Sparse", 0x2EC);
    }

    if (mach64ChipType == MACH64_GX || mach64ChipType == MACH64_CX)
	mach64IntegratedController = FALSE;
    else
	/* Even for MACH64_UNKNOWN more than likely */
	mach64IntegratedController = TRUE;

#ifdef DEBUG
    if (mach64IntegratedController)
	mach64PrintCTPLL();
#endif

    if (xf86bpp < 0) {
        xf86bpp = mach64InfoRec.depth;
    }

    if (xf86weight.red == 0 ||
	xf86weight.green == 0 ||
	xf86weight.blue == 0) {
        xf86weight = mach64InfoRec.weight;
    }

    /* Check for known contradictions */
    if (info->Clock_Type == CLK_CH8398 && info->DAC_SubType != DAC_CH8398)
	new = DAC_CH8398;
    if (info->Clock_Type == CLK_STG1703 && info->DAC_SubType != DAC_STG1703)
	new = DAC_STG1703;
    if (info->Clock_Type == CLK_ATT20C408 && info->DAC_SubType != DAC_ATT20C408)
	new = DAC_ATT20C408;
    if (new >= 0) {
	ErrorF("%s %s: Autodetected RAMDAC %s contradicts the Clock type\n",
	       XCONFIG_PROBED, mach64InfoRec.name,
	       xf86TokenToString(mach64RamdacTable, info->DAC_SubType));
	ErrorF("\tSetting probed RAMDAC to %s\n",
	       xf86TokenToString(mach64RamdacTable, new));
	info->DAC_SubType = new;
    }

    switch (info->DAC_SubType) {
    case DAC_ATI68860_B:
    case DAC_ATI68860_C:
    case DAC_ATI68875:
    case DAC_CH8398:
    case DAC_STG1702:
    case DAC_STG1703:
    case DAC_ATT20C408:
    case DAC_INTERNAL:
    case DAC_IBMRGB514:
	break;
    default:
	if (xf86bpp != 8) {
	    ErrorF("mach64Probe: Invalid bpp: %d\n", xf86bpp);
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	    return(FALSE);
	}
	break;
    }

    switch (xf86bpp) {
    case 8:
        break;
    case 16:
	if (xf86weight.red == 5 &&
	    xf86weight.green == 5 &&
	    xf86weight.blue == 5) {
	    mach64WeightMask = RGB16_555;
            mach64InfoRec.depth = 15;
	} else if (xf86weight.red == 5 &&
		   xf86weight.green == 6 &&
		   xf86weight.blue == 5) {
	    mach64WeightMask = RGB16_565;
	    mach64InfoRec.depth = 16;
	} else {
	    ErrorF("Invalid color weighting %1d%1d%1d (only 555 and 565 are valid)\n",
		   xf86weight.red, xf86weight.green, xf86weight.blue);
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	    return(FALSE);
	}
        mach64InfoRec.bitsPerPixel = 16;
	if (mach64InfoRec.defaultVisual < 0)
	    mach64InfoRec.defaultVisual = TrueColor;
        if (defaultColorVisualClass < 0)
	    defaultColorVisualClass = mach64InfoRec.defaultVisual;
        if (defaultColorVisualClass != TrueColor) {
            ErrorF("Invalid default visual type: %d (%s)\n",
		   defaultColorVisualClass,
                   xf86VisualNames[defaultColorVisualClass]);
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
            return(FALSE);
        }
	break;
    case 24:
    case 32:
        mach64InfoRec.depth = 24;
        mach64InfoRec.bitsPerPixel = 32;
	xf86weight.red = xf86weight.green = xf86weight.blue = 8;
        if (mach64InfoRec.defaultVisual < 0)
            mach64InfoRec.defaultVisual = TrueColor;
        if (defaultColorVisualClass < 0)
            defaultColorVisualClass = mach64InfoRec.defaultVisual;
        if (defaultColorVisualClass != TrueColor) {
            ErrorF("Invalid default visual type: %d (%s)\n",
                   defaultColorVisualClass,
                   xf86VisualNames[defaultColorVisualClass]);
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
            return(FALSE);
        }
        break;
    default:
	ErrorF("mach64Probe: Invalid bpp: %d\n", xf86bpp);
	xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	return(FALSE);
    }

    switch(info->DAC_SubType) {
    case DAC_ATI68860_B:
    case DAC_ATI68860_C:
	mach64InfoRec.maxClock = 135000;
	break;
    case DAC_ATI68875:
	mach64InfoRec.maxClock = 135000;
	break;
    case DAC_CH8398:
	mach64InfoRec.maxClock = 135000;
	break;
    case DAC_STG1702:
    case DAC_STG1703:
	mach64InfoRec.maxClock = 135000;
	break;
    case DAC_ATT20C408:
	mach64InfoRec.maxClock = 135000;
	break;
    case DAC_INTERNAL:
	if ((mach64ChipType == MACH64_VT || mach64ChipType == MACH64_GT) &&
	    (mach64ChipRev & 0x01)) {
	    mach64InfoRec.maxClock = 170000;
	} else {
	    if (xf86bpp == 8)
		mach64InfoRec.maxClock = 135000;
	    else
		mach64InfoRec.maxClock = 80000;
	}
	break;
    case DAC_IBMRGB514:
	mach64InfoRec.maxClock = 220000;
	break;
    default:
	mach64InfoRec.maxClock = mach64MaxClock;
	break;
    }

    OFLG_ZERO(&validOptions);
    OFLG_SET(OPTION_CLKDIV2, &validOptions);
    OFLG_SET(OPTION_HW_CURSOR, &validOptions);
    OFLG_SET(OPTION_SW_CURSOR, &validOptions);
    OFLG_SET(OPTION_CSYNC, &validOptions);
    if (info->DAC_SubType != DAC_CH8398) {
	OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
    }
    OFLG_SET(OPTION_DAC_6_BIT, &validOptions);
    OFLG_SET(OPTION_OVERRIDE_BIOS, &validOptions);
    if (!mach64IntegratedController) {
	OFLG_SET(OPTION_NO_BLOCK_WRITE, &validOptions);
	OFLG_SET(OPTION_BLOCK_WRITE, &validOptions);
    }
    OFLG_SET(OPTION_POWER_SAVER, &validOptions);
    OFLG_SET(OPTION_NO_BIOS_CLOCKS, &validOptions);
    OFLG_SET(OPTION_NO_PROGRAM_CLOCKS, &validOptions);
    OFLG_SET(OPTION_NO_FONT_CACHE, &validOptions);
    OFLG_SET(OPTION_NO_PIXMAP_CACHE, &validOptions);
    xf86VerifyOptions(&validOptions, &mach64InfoRec);

    mach64InfoRec.chipset = "mach64";
    xf86ProbeFailed = FALSE;

    if (mach64IntegratedController) {
	if (pciInfo)
	    mach64BusType = PCI;
	else
	    mach64BusType = LOCAL_BUS; /* How can this be confirmed? */
    } else {
	mach64BusType = info->Bus_Type;
    }

    if (xf86Verbose)
    {
	ErrorF("%s %s: card type: ", XCONFIG_PROBED, mach64InfoRec.name);
	switch(mach64BusType)
	{
	case ISA:
	    ErrorF("ISA\n");
	    break;
	case EISA:
	    ErrorF("EISA\n");
	    break;
	case LOCAL_BUS:
	    ErrorF("VESA LocalBus\n");
	    break;
	case PCI:
	    ErrorF("PCI\n");
	    break;
	default:
	    ErrorF("Unknown\n");
	}
    }

    if (mach64InfoRec.ramdac) {
	mach64RamdacSubType =
	    xf86StringToToken(mach64RamdacTableNames, mach64InfoRec.ramdac);
	mach64Ramdac = mach64RamdacSubType & 0x0f;
	if (mach64RamdacSubType < 0) {
	    ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
		   mach64InfoRec.name, mach64InfoRec.ramdac);
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	    return(FALSE);
	}
    } else {
	mach64Ramdac = info->DAC_Type;
	mach64RamdacSubType = info->DAC_SubType;
    }

    mach64MemType = info->Mem_Type;
    if (xf86Verbose) {
	ErrorF("%s %s: Memory type: %d\n", XCONFIG_PROBED,
	       mach64InfoRec.name, mach64MemType);
    }
    mach64MinFreq = info->MinFreq;
    mach64MaxFreq = info->MaxFreq;
    mach64RefFreq = info->RefFreq;
    mach64RefDivider = info->RefDivider;
    mach64NAdj = info->NAdj;
    mach64DRAMMemClk = info->DRAMMemClk;
    mach64VRAMMemClk = info->VRAMMemClk;
    mach64MemCycle = info->MemCycle;
    mach64MemClk = info->MemClk;
    if (mach64RamdacSubType == DAC_IBMRGB514)
	mach64CXClk = 7;  /* Use IBM RGB514 PLL */
    else
	mach64CXClk = info->CXClk;

#ifdef DEBUG
    ErrorF("MinFreq = %d, MaxFreq = %d, RefFreq = %d, RefDivider = %d\n",
	   info->MinFreq, info->MaxFreq, info->RefFreq, info->RefDivider);
    ErrorF("CXClk = %d\n", info->CXClk);
#endif

    mach64ClockType = info->Clock_Type;
    for (i = 0; i < MACH64_NUM_CLOCKS; i++)
	mach64Clocks[i] = info->Clocks[i];
    for (i = 0; i < MACH64_NUM_FREQS; i++)
	mach64FreqTable[i] = info->Freq_Table[i];
    for (i = 0; i < MACH64_NUM_FREQS; i++)
	mach64FreqTable2[i] = info->Freq_Table2[i];

    if (OFLG_ISSET(OPTION_NO_PROGRAM_CLOCKS, &mach64InfoRec.options)) {
	if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &mach64InfoRec.clockOptions)) {
	    ErrorF("Warning: Clock programming overriden\n");
	}

	/* No need to use auto probing code since the BIOS contains
	 * all of the clocks unless the "no_bios_clocks" option is
	 * set. */
	if (!mach64InfoRec.clocks ||
	    !OFLG_ISSET(OPTION_NO_BIOS_CLOCKS, &mach64InfoRec.options)) {
	    if (mach64InfoRec.clocks) {
		ErrorF("Warning: Clocks being read from the video card's BIOS,\n");
		ErrorF("  and the clocks lines in XF86Config file are ignored\n");
		ErrorF("  (see man page or README.Mach64 for more details).\n");
	    } else if (OFLG_ISSET(OPTION_NO_BIOS_CLOCKS, &mach64InfoRec.options)) {
		ErrorF("Warning: \"no_bios_clocks\" option set, but no Clocks\n");
		ErrorF("  given in XF86Config file.  Clocks being read from\n");
		ErrorF("  the video card's BIOS (see man page or README.Mach64\n");
		ErrorF("  for more details).\n");
	    }
	    for (i = 0; i < MACH64_NUM_CLOCKS; i++) {
		mach64InfoRec.clock[i] = mach64Clocks[i] * 10;
		mach64InfoRec.clock[i + MACH64_NUM_CLOCKS] = mach64Clocks[i] * 5;
	    }

	    mach64InfoRec.clocks = MACH64_NUM_CLOCKS*2;
	} else {
	    ErrorF("Warning: BIOS Clocks overidden.\n");
	}
    } else if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &mach64InfoRec.clockOptions)) {
	if (OFLG_ISSET(CLOCK_OPTION_ICS2595, &mach64InfoRec.clockOptions))
	    mach64ClockType = CLK_ATI18818_1;
	else if (OFLG_ISSET(CLOCK_OPTION_STG1703, &mach64InfoRec.clockOptions))
	    mach64ClockType = CLK_STG1703;
	else if (OFLG_ISSET(CLOCK_OPTION_CH8398, &mach64InfoRec.clockOptions))
	    mach64ClockType = CLK_CH8398;
	else if (OFLG_ISSET(CLOCK_OPTION_ATT409, &mach64InfoRec.clockOptions))
	    mach64ClockType = CLK_ATT20C408;
	else if (OFLG_ISSET(CLOCK_OPTION_IBMRGB, &mach64InfoRec.clockOptions))
	    mach64ClockType = CLK_IBMRGB514;
	else {
	    ErrorF("Unrecognized clock chip type\n");
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	    return(FALSE);
	}
	clock_type_probed = FALSE;
    } else {
	OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &mach64InfoRec.clockOptions);
    }

    if (xf86Verbose) {
	ErrorF("%s %s: Clock type: %s\n",
	       (clock_type_probed ? XCONFIG_PROBED : XCONFIG_GIVEN),
	       mach64InfoRec.name,
	       (mach64ClockType < NUM_CLOCK_TYPES ?
		(mach64RamdacSubType == DAC_IBMRGB514 ?
		 mach64ClockTypeTable[CLK_IBMRGB514] :
		 mach64ClockTypeTable[mach64ClockType]) : "Unknown"));
	if (!OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &mach64InfoRec.clockOptions) ||
	    OFLG_ISSET(OPTION_NO_PROGRAM_CLOCKS, &mach64InfoRec.options)) {
	    ErrorF("%s ",(OFLG_ISSET(XCONFIG_CLOCKS,&mach64InfoRec.xconfigFlag) &&
			  OFLG_ISSET(OPTION_NO_BIOS_CLOCKS, &mach64InfoRec.options)) ?
		   XCONFIG_GIVEN : XCONFIG_PROBED);
	    ErrorF("%s: ", mach64InfoRec.name);
	    ErrorF("Number of Clocks: %d", mach64InfoRec.clocks);

	    for (i = 0; i < mach64InfoRec.clocks; i++) {
		if (i % 8 == 0) 
		    ErrorF("\n%s %s: clocks:", 
			   (OFLG_ISSET(XCONFIG_CLOCKS,&mach64InfoRec.xconfigFlag) &&
			    OFLG_ISSET(OPTION_NO_BIOS_CLOCKS, &mach64InfoRec.options)) ?
			   XCONFIG_GIVEN : XCONFIG_PROBED,
			   mach64InfoRec.name);
		ErrorF(" %6.2f", mach64InfoRec.clock[i]/1000.0);
	    }
	    ErrorF("\n");
	}
    }

    if (!mach64InfoRec.videoRam) {
	mach64InfoRec.videoRam = info->Mem_Size;
    }

    tx = mach64InfoRec.virtualX;
    ty = mach64InfoRec.virtualY;
    pMode = mach64InfoRec.modes;
    if (pMode == NULL) {
        ErrorF("No modes supplied in XF86Config\n");
        xf86DisableIOPorts(mach64InfoRec.scrnIndex);
        return(FALSE);
    }
    pEnd = (DisplayModePtr)NULL;
    do {
          DisplayModePtr pModeSv;
          /*
           * xf86LookupMode returns FALSE if it ran into an invalid
           * parameter
           */
          if(xf86LookupMode(pMode, &mach64InfoRec, LOOKUP_DEFAULT) == FALSE) {
                pModeSv=pMode->next;
                xf86DeleteMode(&mach64InfoRec, pMode);
                pMode = pModeSv; 
	  } else if (pMode->HDisplay * pMode->VDisplay *
	      (mach64InfoRec.bitsPerPixel / 8) >
	      mach64InfoRec.videoRam*1024) {
		pModeSv=pMode->next;
		ErrorF("%s %s: Too little memory for mode \"%s\"\n",
		       XCONFIG_PROBED, mach64InfoRec.name, pMode->name);
		xf86DeleteMode(&mach64InfoRec, pMode);
		pMode = pModeSv;
          } else if (((tx > 0) && (pMode->HDisplay > tx)) || 
                     ((ty > 0) && (pMode->VDisplay > ty))) {
                pModeSv=pMode->next;
                ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
                       XCONFIG_PROBED, mach64InfoRec.name,
                        pMode->HDisplay, pMode->VDisplay, tx, ty);
                xf86DeleteMode(&mach64InfoRec, pMode);
                pMode = pModeSv;
          } else if ((pMode->Flags & V_DBLSCAN) &&
		     (mach64ChipType == MACH64_GX ||
		      mach64ChipType == MACH64_CX ||
		      mach64ChipType == MACH64_CT ||
		      mach64ChipType == MACH64_ET)) {
                pModeSv=pMode->next;
                ErrorF("%s %s: Doublescan mode not supported on the %s\n",
                       XCONFIG_PROBED, mach64InfoRec.name,
                        xf86TokenToString(mach64ChipTable, mach64ChipType));
                xf86DeleteMode(&mach64InfoRec, pMode);
                pMode = pModeSv;
          } else {
	        int found = FALSE;
		int mach64CDepths[] = { -1, 4, 8, 15, 16, 24, 32, -1 };

	        for (i = 0; mach64FreqTable2[i].h_disp != 0; i++)
		    if ((mach64FreqTable2[i].h_disp << 3 >= pMode->HDisplay) &&
			(mach64FreqTable2[i].dacmask == mach64RamdacSubType) &&
			(mach64FreqTable2[i].max_dot_clock >=
			 mach64InfoRec.clock[pMode->Clock] / 1000) &&
			(mach64CDepths[mach64FreqTable2[i].color_depth & 0x07]
			 >= mach64InfoRec.bitsPerPixel)) {
			found = TRUE;
			break;
		    }

		if (!found)
		    for (i = 0; mach64FreqTable[i].h_disp != 0; i++)
			if ((mach64FreqTable[i].h_disp << 3 >= pMode->HDisplay) &&
			    (mach64FreqTable[i].dacmask & (1 << mach64Ramdac)) &&
			    (mach64FreqTable[i].max_dot_clock >=
			     mach64InfoRec.clock[pMode->Clock] / 1000) &&
			    (mach64CDepths[mach64FreqTable[i].color_depth & 0x07]
			     >= mach64InfoRec.bitsPerPixel)) {
			    found = TRUE;
			    break;
			}

		if (!found &&
		    OFLG_ISSET(OPTION_OVERRIDE_BIOS, &mach64InfoRec.options)) {
		    /* BIOS doesn't think this mode is legal! */
		    ErrorF("%s %s: Illegal mode according to Mach64 BIOS (Overridden)\n",
			   XCONFIG_GIVEN, mach64InfoRec.name);
		    found = TRUE;
		}

		if (!found) {
		    pModeSv=pMode->next;
		    ErrorF("%s %s: Illegal mode according to Mach64 BIOS\n",
			   XCONFIG_PROBED, mach64InfoRec.name);
		    xf86DeleteMode(&mach64InfoRec, pMode);
		    pMode = pModeSv;
		} else {
		    /*
		     * Successfully looked up this mode.  If pEnd isn't 
		     * initialized, set it to this mode.
		     */
		    if(pEnd == (DisplayModePtr) NULL)
                        pEnd = pMode;
		    mach64InfoRec.virtualX = max(mach64InfoRec.virtualX,
						 pMode->HDisplay);
		    mach64InfoRec.virtualY = max(mach64InfoRec.virtualY,
						 pMode->VDisplay);
		    pMode = pMode->next;
		}
          }
    } while (pMode != pEnd);

    /* For programmable clocks, fill in the SynthClock value for each
     * mode */
    if (OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE, &mach64InfoRec.clockOptions)) {
	pEnd = pMode = mach64InfoRec.modes;
	do {
	    pMode->SynthClock = mach64InfoRec.clock[pMode->Clock];
	    pMode = pMode->next;
	} while (pMode != pEnd);
    }

    mach64VirtX = mach64InfoRec.virtualX = (mach64InfoRec.virtualX+7) & 0xfff8;
    mach64VirtY = mach64InfoRec.virtualY;

    if ((tx != mach64InfoRec.virtualX) || (ty != mach64InfoRec.virtualY))
	OFLG_CLR(XCONFIG_VIRTUAL,&mach64InfoRec.xconfigFlag);

    if (xf86Verbose) {
	ErrorF("%s %s: Virtual resolution: %dx%d\n",
	       OFLG_ISSET(XCONFIG_VIRTUAL, &mach64InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED,
		mach64InfoRec.name, mach64VirtX, mach64VirtY);
    }

    /* Set mach64MemorySize to required MEM_SIZE value in MISC_OPTIONS */
    if (mach64InfoRec.videoRam <= 512)
	mach64MemorySize = MEM_SIZE_512K;
    else if (mach64InfoRec.videoRam <= 1024)
	mach64MemorySize = MEM_SIZE_1M;
    else if (mach64InfoRec.videoRam <= 2048)
	mach64MemorySize = MEM_SIZE_2M;
    else if (mach64InfoRec.videoRam <= 4096)
	mach64MemorySize = MEM_SIZE_4M;
    else if (mach64InfoRec.videoRam <= 6144)
	mach64MemorySize = MEM_SIZE_6M;
    else
	mach64MemorySize = MEM_SIZE_8M;
	
    if (xf86Verbose) {
       ErrorF("%s %s: videoram: %dk\n",
	      OFLG_ISSET(XCONFIG_VIDEORAM, &mach64InfoRec.xconfigFlag) ?
	      XCONFIG_GIVEN : XCONFIG_PROBED, mach64InfoRec.name,
	      mach64InfoRec.videoRam );
    }

    if (((mach64VirtX) * (mach64VirtY) * (mach64InfoRec.bitsPerPixel / 8)) > 
	(mach64InfoRec.videoRam*1024)) {
	ErrorF("Not enough memory for requested virtual resolution (%dx%d)\n",
	       mach64VirtX, mach64VirtY);
	xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	return(FALSE);
    }

    /* Set values of mach64MaxX and mach64MaxY.
     * This must be done here so that cache area and
     * scissor limits are set up correctly (this setup
     * happens before the pixmap cache is initialized).
     */

    mach64MaxX = mach64VirtX - 1;
    mach64MaxY = (mach64InfoRec.videoRam * 1024) /
                 (mach64VirtX * (mach64InfoRec.bitsPerPixel / 8)) - 1;

    /* Reserve space for the registers at the end of video memory */
    mach64InfoRec.videoRam--;

    available_ram = mach64InfoRec.videoRam * 1024;

    sw_cursor_supplied = OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options);
    if (sw_cursor_supplied &&
	OFLG_ISSET(OPTION_HW_CURSOR, &mach64InfoRec.options)) {
	ErrorF("Cannot use hardware and software cursors simultaneously.\n");
	xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	return(FALSE);
    }

    if (!sw_cursor_supplied && mach64RamdacSubType != DAC_IBMRGB514)
    {
	if (available_ram - (mach64VirtX * mach64VirtY) >= MACH64_CURSBYTES)
	{
	    available_ram -= (MACH64_CURSBYTES + 1023) & ~1023;
	    mach64InfoRec.videoRam -= (MACH64_CURSBYTES + 1023) / 1024;
	}
	else if (OFLG_ISSET(OPTION_HW_CURSOR, &mach64InfoRec.options))
	{
	    ErrorF("Not enough memory to use the hardware cursor.\n");
	    ErrorF("  Decreasing the virtual Y resolution by 1 will allow\n");
	    ErrorF("  you to use the hardware cursor.\n");
	    xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	    return(FALSE);
	}
	else
	{
	    OFLG_SET(OPTION_SW_CURSOR, &mach64InfoRec.options);
	    ErrorF("Warning: Not enough memory to use the hardware cursor.\n");
	    ErrorF("  Decreasing the virtual Y resolution by 1 will allow\n");
	    ErrorF("  you to use the hardware cursor.\n");
	}
    }
    ErrorF("%s %s: Using %s cursor\n", 
	   (sw_cursor_supplied ||
	    OFLG_ISSET(OPTION_HW_CURSOR, &mach64InfoRec.options))
	   ? XCONFIG_GIVEN : XCONFIG_PROBED, mach64InfoRec.name,
	   OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options) ?
	   "software" : "hardware");

    /* Assume we are not on an ISA bus since supporting one would require
     * a tremendous rewrite of this code.  The best solution is to make
     * another accel server (XF86_Mach64ISA) for ISA cards if there are
     * any masochists out there.
     */
    mach64Use4MbAperture = xf86LinearVidMem();
    if (!mach64Use4MbAperture) {
	ErrorF("The mach64 X server requires at least a 4Mb memory aperture.\n");
	xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	return(FALSE);
    }

    if (mach64Use4MbAperture) {
	if (mach64InfoRec.MemBase != 0)
	    mach64ApertureAddr = mach64InfoRec.MemBase & 0xffc00000;
	else {
	    if (pciInfo && pciInfo->ApertureBase)
		mach64ApertureAddr = pciInfo->ApertureBase;
	    else
		mach64ApertureAddr = (inw(ioCONFIG_CNTL) & 0x3FF0) << 18;
	}
	/* Last-resort defaults */
	if (mach64ApertureAddr == 0) {
	    if (mach64BusType == PCI)
		mach64ApertureAddr = 0x7c000000;
	    else
		mach64ApertureAddr = 0x04000000;  /* for VLB */
	}

	if ((mach64BusType == ISA) && (mach64ChipType != MACH64_VT)) {
	    mach64ApertureSize = MEM_SIZE_4M;
	    if (xf86Verbose) {
	        ErrorF("%s %s: Using 4 MB aperture @ 0x%08x\n", XCONFIG_PROBED,
		       mach64InfoRec.name, mach64ApertureAddr);
	    }
	} else {
	    mach64ApertureSize = MEM_SIZE_8M;
	    if (xf86Verbose) {
	        ErrorF("%s %s: Using 8 MB aperture @ 0x%08x\n", XCONFIG_PROBED,
		       mach64InfoRec.name, mach64ApertureAddr);
	    }
	}
    } else {
	ErrorF("To use the Mach64 X server you need to be able to use\n");
	ErrorF("a 4 or 8 Mb memory aperture.\n");
	xf86DisableIOPorts(mach64InfoRec.scrnIndex);
	return(FALSE);
    }

    if (xf86Verbose) {
	for (i = 0;
	     ((mach64RamdacTable[i].token != -1) &&
	      (mach64RamdacTable[i].token != mach64RamdacSubType));
	     i++);
	if (mach64RamdacTable[i].token == -1) {
	    ErrorF("%s %s: Ramdac is Unknown (%d)\n",
		   (mach64InfoRec.ramdac ? XCONFIG_GIVEN : XCONFIG_PROBED),
		   mach64InfoRec.name,
		   mach64RamdacSubType);
	} else {
	    ErrorF("%s %s: Ramdac is %s\n",
		   (mach64InfoRec.ramdac ? XCONFIG_GIVEN : XCONFIG_PROBED),
		   mach64InfoRec.name,
		   mach64RamdacTable[i].name);
	}
    }

    mach64DAC8Bit = ((!OFLG_ISSET(OPTION_DAC_6_BIT, &mach64InfoRec.options) &&
		      (mach64InfoRec.bitsPerPixel == 8) &&
		      (mach64RamdacSubType != DAC_CH8398)) ||
		     (mach64InfoRec.bitsPerPixel == 16) ||
		     (mach64InfoRec.bitsPerPixel == 32));

    if (xf86Verbose) {
	if (mach64InfoRec.bitsPerPixel == 8) {
	    ErrorF("%s %s: Using %d bits per RGB value\n",
		   (OFLG_ISSET(OPTION_DAC_6_BIT, &mach64InfoRec.options) ||
		    OFLG_ISSET(OPTION_DAC_8_BIT, &mach64InfoRec.options))
	           ? XCONFIG_GIVEN : XCONFIG_PROBED,
	           mach64InfoRec.name,
	           mach64DAC8Bit ?  8 : 6);
	} else if (mach64InfoRec.bitsPerPixel == 16) {
            ErrorF("%s %s: Color weight: %1d%1d%1d\n", XCONFIG_GIVEN,
		   mach64InfoRec.name, xf86weight.red,
		   xf86weight.green, xf86weight.blue);
	}
    }

#ifdef DPMSExtension
    if (DPMSEnabledSwitch ||
	(OFLG_ISSET(OPTION_POWER_SAVER, &mach64InfoRec.options) &&
	 !DPMSDisabledSwitch))
	defaultDPMSEnabled = DPMSEnabled = TRUE;
#endif

#ifdef XFreeXDGA
    mach64InfoRec.displayWidth = mach64InfoRec.virtualX;
    mach64InfoRec.directMode = XF86DGADirectPresent;
#endif

    return(TRUE);
}


/*
 * mach64PrintIdent --
 *     Print the indentification of the video card.
 */
void
mach64PrintIdent()
{
    ErrorF("  %s: accelerated server for ATI Mach64 graphics adaptors ",
	   mach64InfoRec.name);
    ErrorF("(Patchlevel %s)\n", mach64InfoRec.patchLevel);
}


/*
 * mach64Initialize --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
mach64Initialize (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
    int displayResolution = 75;  /* default to 75dpi */
    extern int monitorResolution;

    mach64InitGC();
    mach64InitDisplay(scr_index);
    mach64CalcCRTCRegs(&mach64CRTCRegs, mach64InfoRec.modes);
    mach64SetCRTCRegs(&mach64CRTCRegs);
    mach64InitEnvironment();

    /* Clear the display.
     * Need to set the color, origin, and size.  Then draw.
     */
    WaitQueue(4);
    if (xf86bpp == 8 && xf86FlipPixels)
	regw(DP_FRGD_CLR, 1);
    else
	regw(DP_FRGD_CLR, 0);
    regw(DST_Y_X, 0);
    regw(DST_CNTL, 0x03);
    regw(DST_HEIGHT_WIDTH, ((mach64VirtX << 16) | mach64VirtY));

    mach64CacheInit(mach64VirtX, mach64VirtY);
    mach64FontCache8Init(mach64VirtX, mach64VirtY);

    mach64ImageInit();

    WaitIdleEmpty(); /* Make sure that all commands have finished */

    /*
     * Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
	displayResolution = monitorResolution;

    if (!mach64ScreenInit(pScreen, mach64VideoMem,
			  mach64VirtX, mach64VirtY,
			  displayResolution, displayResolution,
			  mach64VirtX))

		return(FALSE);

    savepScreen = pScreen;

    pScreen->CloseScreen = mach64CloseScreen;
    pScreen->SaveScreen = mach64SaveScreen;

    switch (mach64InfoRec.bitsPerPixel) {
    case 8:
        pScreen->InstallColormap = mach64InstallColormap;
        pScreen->UninstallColormap = mach64UninstallColormap;
        pScreen->ListInstalledColormaps = mach64ListInstalledColormaps;
        pScreen->StoreColors = mach64StoreColors;
        break;
    case 16:
    case 32:
        pScreen->InstallColormap = cfbInstallColormap;
        pScreen->UninstallColormap = cfbUninstallColormap;
        pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
        pScreen->StoreColors = (void (*)())NoopDDA;
    }

    if (OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options)) {
	miDCInitialize (pScreen, &xf86PointerScreenFuncs);
    } else {
        pScreen->QueryBestSize = mach64QueryBestSize;
        xf86PointerScreenFuncs.WarpCursor = mach64WarpCursor;
        (void)mach64CursorInit(0, pScreen);
    }

#ifdef PIXPRIV
    mach64PixmapIndex = AllocatePixmapPrivateIndex();
    if (!AllocatePixmapPrivate(pScreen, mach64PixmapIndex,
			       sizeof(mach64PixPrivRec)))
	return FALSE;
#endif

    return (cfbCreateDefColormap(pScreen));
}


/*
 *      Assign a new serial number to the window.
 *      Used to force GC validation on VT switch.
 */

/*ARGSUSED*/
static int
mach64NewSerialNumber(pWin, data)
    WindowPtr pWin;
    pointer data;
{
    pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    return WT_WALKCHILDREN;
}


/*
 * mach64EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
mach64EnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
    PixmapPtr pspix;
    ScreenPtr pScreen = savepScreen;

    if (!xf86Exiting && !xf86Resetting) {
        switch (mach64InfoRec.bitsPerPixel) {
        case 8:
            pspix = (PixmapPtr)pScreen->devPrivate;
            break;
        case 16:
	    pspix = (PixmapPtr)pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr;
            break;
        case 32:
	    pspix = (PixmapPtr)pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
            break;
        }
    }

    if (pScreen && !xf86Exiting && !xf86Resetting)
        WalkTree(pScreen, mach64NewSerialNumber, 0);

    if (enter) {
	if (vgaBase)
	    xf86MapDisplay(screen_idx, VGA_REGION);
	if (mach64VideoMem != vgaBase)
	    xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!xf86Resetting) {
	    ScrnInfoPtr pScr = (ScrnInfoPtr)XF86SCRNINFO(pScreen);

	    mach64InitDisplay(screen_idx);
	    mach64SetCRTCRegs(&mach64CRTCRegs);
	    mach64InitEnvironment();

	    WaitQueue(4);
	    regw(DP_FRGD_CLR, 1);
	    regw(DST_Y_X, 0);
	    regw(DST_CNTL, 0x03);
	    regw(DST_HEIGHT_WIDTH, ((mach64VirtX << 16) | mach64VirtY));

	    mach64CacheInit(mach64VirtX, mach64VirtY);
	    mach64FontCache8Init(mach64VirtX, mach64VirtY);

	    if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options))
		mach64RestoreCursor(pScreen);
	    mach64AdjustFrame(pScr->frameX0, pScr->frameY0);

	    WaitIdleEmpty(); /* Make sure that all commands have finished */

	    if (LUTissaved) {
		mach64RestoreLUT(mach64savedLUT);
		LUTissaved = FALSE;
		mach64RestoreColor0(pScreen);
	    }

	    if (pspix->devPrivate.ptr != mach64VideoMem && ppix) {
		pspix->devPrivate.ptr = mach64VideoMem;
		(mach64ImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
				 ppix->devPrivate.ptr,
				 PixmapBytePad(pScreen->width,
					       pScreen->rootDepth),
				 0, 0, MIX_SRC, ~0);
	    }
            if (pScreen) {
                pScreen->CopyWindow = mach64CopyWindow;
                pScreen->PaintWindowBackground = mach64PaintWindow;
                pScreen->PaintWindowBorder = mach64PaintWindow;
                switch (mach64InfoRec.bitsPerPixel) {
                case 8:
                    pScreen->GetSpans = cfbGetSpans;
		    break;
                case 16:
                    pScreen->GetSpans = cfb16GetSpans;
		    break;
                case 32:
                    pScreen->GetSpans = cfb32GetSpans;
		    break;
		}
            }
	}
	if (ppix) {
	    (pScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
	}
    } else {
	if (vgaBase)
	    xf86MapDisplay(screen_idx, VGA_REGION);
	if (mach64VideoMem != vgaBase)
	    xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!xf86Exiting) {
	    ppix = (pScreen->CreatePixmap)(pScreen,
					   pScreen->width, pScreen->height,
					   pScreen->rootDepth);

	    if (ppix) {
		(mach64ImageReadFunc)(0, 0, pScreen->width, pScreen->height,
				ppix->devPrivate.ptr,
				PixmapBytePad(pScreen->width,
					      pScreen->rootDepth),
				0, 0, ~0);
		pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	    }
            switch (mach64InfoRec.bitsPerPixel) {
            case 8:
                pScreen->CopyWindow = cfbCopyWindow;
                pScreen->GetSpans = cfbGetSpans;
                pScreen->PaintWindowBackground = cfbPaintWindow;
                pScreen->PaintWindowBorder = cfbPaintWindow;
                break;
            case 16:
                pScreen->CopyWindow = cfb16CopyWindow;
                pScreen->GetSpans = cfb16GetSpans;
                pScreen->PaintWindowBackground = cfb16PaintWindow;
                pScreen->PaintWindowBorder = cfb16PaintWindow;
                break;
            case 32:
                pScreen->CopyWindow = cfb32CopyWindow;
                pScreen->GetSpans = cfb32GetSpans;
                pScreen->PaintWindowBackground = cfb32PaintWindow;
                pScreen->PaintWindowBorder = cfb32PaintWindow;
                break;
            }
	}

	if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options))
	    mach64CursorOff();
	mach64SaveLUT(mach64savedLUT);
	LUTissaved = TRUE;
	if (!xf86Resetting) {
#ifdef XFreeXDGA
	    if (!(mach64InfoRec.directMode & XF86DGADirectGraphics))
#endif
		mach64CleanUp();
	}
	if (vgaBase)
	    xf86UnMapDisplay(screen_idx, VGA_REGION);
	if (mach64VideoMem != vgaBase)
	    xf86UnMapDisplay(screen_idx, LINEAR_REGION);
    }
}

/*
 * mach64CloseScreen --
 *      called to ensure video is enabled when server exits.
 */

/*ARGSUSED*/
Bool
mach64CloseScreen(screen_idx, pScreen)
    int        screen_idx;
    ScreenPtr  pScreen;
{
    extern void mach64ClearSavedCursor();

    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    xf86Exiting = TRUE;
    if (xf86VTSema)
	mach64EnterLeaveVT(LEAVE, screen_idx);
    else if (ppix) {
    /* 7-Jan-94 CEG: The server is not running on the current vt.
     * Free the screen snapshot taken when the server vt was left.
     */
	    (savepScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
    }
    if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options))
	mach64ClearSavedCursor(screen_idx);

    switch (mach64InfoRec.bitsPerPixel) {
    case 8:
        cfbCloseScreen(screen_idx, savepScreen);
	break;
    case 16:
        cfb16CloseScreen(screen_idx, savepScreen);
	break;
    case 32:
        cfb32CloseScreen(screen_idx, savepScreen);
	break;
    }

    savepScreen = NULL;
    return(TRUE);
}

/*
 * mach64SaveScreen --
 *      blank the screen.
 */
Bool
mach64SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
    if (on)
	SetTimeSinceLastInputEvent();

    if (xf86VTSema) {

	if (on) {
	    mach64SetRamdac(mach64CRTCRegs.color_depth, TRUE,
			    mach64CRTCRegs.dot_clock);
	    if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options))
		if (mach64HWCursorSave != -1) {
		    if (mach64RamdacSubType == DAC_IBMRGB514)
			mach64P_RGB514Index(0x30, mach64HWCursorSave);
		    else
			regwb(GEN_TEST_CNTL, mach64HWCursorSave);
		    mach64HWCursorSave = -1;
		}
	    mach64RestoreColor0(pScreen);
	    if (mach64RamdacSubType != DAC_ATI68875)
		outb(ioDAC_REGS+2, 0xff);
	} else {
	    outb(ioDAC_REGS, 0);
	    outb(ioDAC_REGS+1, 0);
	    outb(ioDAC_REGS+1, 0);
	    outb(ioDAC_REGS+1, 0);
	    outb(ioDAC_REGS+2, 0x00);

	    if (mach64InfoRec.bitsPerPixel > 8 &&
		mach64RamdacSubType == DAC_INTERNAL) {
		int temp;
		outb(ioDAC_REGS, 0);
		for (temp = 0; temp < 256; temp++) {
		    outb(ioDAC_REGS + 1, 0);
		    outb(ioDAC_REGS + 1, 0);
		    outb(ioDAC_REGS + 1, 0);
		}
	    }
	    mach64SetRamdac(CRTC_PIX_WIDTH_8BPP, TRUE,
			    mach64CRTCRegs.dot_clock);
	    if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options)) {
		if (mach64RamdacSubType == DAC_IBMRGB514) {
		    mach64HWCursorSave = mach64R_RGB514Index(0x30);
		    mach64P_RGB514Index(0x30, 0x00);
		} else {
		    mach64HWCursorSave = regrb(GEN_TEST_CNTL);
		    regwb(GEN_TEST_CNTL,
			  mach64HWCursorSave & ~HWCURSOR_ENABLE);
		}
	    }

	    if (mach64RamdacSubType != DAC_ATI68875)
		outb(ioDAC_REGS+2, 0x00);
	}
    }
    return(TRUE);
}

/*
 * mach64DPMSSet -- Sets VESA Display Power Management Signaling (DPMS) Mode
 */

void
mach64DPMSSet(PowerManagementMode)
    int PowerManagementMode;
{
#ifdef DPMSExtension
    int crtcGenCntl;
    if (!xf86VTSema) return;
    crtcGenCntl = regr(CRTC_GEN_CNTL) & ~(CRTC_HSYNC_DIS | CRTC_VSYNC_DIS);
    switch (PowerManagementMode)
    {
    case DPMSModeOn:
	/* HSync: On, VSync: On */
	break;
    case DPMSModeStandby:
	/* HSync: Off, VSync: On */
	crtcGenCntl |= CRTC_HSYNC_DIS;
	break;
    case DPMSModeSuspend:
	/* HSync: On, VSync: Off */
	crtcGenCntl |= CRTC_VSYNC_DIS;
	break;
    case DPMSModeOff:
	/* HSync: Off, VSync: Off */
	crtcGenCntl |= (CRTC_HSYNC_DIS | CRTC_VSYNC_DIS);
	break;
    }
    usleep(10000);
    regw(CRTC_GEN_CNTL, crtcGenCntl);
#endif
}

/*
 * mach64AdjustFrame --
 *      Modify the CRT_OFFSET for panning the display.
 */
void
mach64AdjustFrame(x, y)
    int x, y;
{
    int byte_offset = ((x + y*mach64VirtX) *
                        (mach64InfoRec.bitsPerPixel / 8)) >> 3;

    if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options))
	mach64CursorOff();

    regw(CRTC_OFF_PITCH, (regr(CRTC_OFF_PITCH) & 0xfff00000) | byte_offset);

    if (!OFLG_ISSET(OPTION_SW_CURSOR, &mach64InfoRec.options))
	mach64RepositionCursor(savepScreen);

#ifdef XFreeXDGA
    if (mach64InfoRec.directMode & XF86DGADirectGraphics) {
#if 0
	/* Wait until the current vertical line has gone past 0. */
	while (regr(CRTC_VLINE_CRNT_VLINE) & CRTC_CRNT_VLINE);
	while (!(regr(CRTC_VLINE_CRNT_VLINE) & CRTC_CRNT_VLINE));
#else
	/* Wait until vertical retrace is in progress. */
	/* This seems better */
	while (regrb(CRTC_INT_CNTL) & CRTC_VBLANK);
	while (!(regrb(CRTC_INT_CNTL) & CRTC_VBLANK));
#endif
    }
#endif
}

/*
 * mach64SwitchMode --
 *      Reinitialize the CRTC registers for the new mode.
 */
Bool
mach64SwitchMode(mode)
    DisplayModePtr mode;
{
    mach64CalcCRTCRegs(&mach64CRTCRegs, mode);
    mach64SetCRTCRegs(&mach64CRTCRegs);

    return(TRUE);
}

/*
 * mach64ValidMode --
 *
 */
static int
mach64ValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
{
return MODE_OK;
}
