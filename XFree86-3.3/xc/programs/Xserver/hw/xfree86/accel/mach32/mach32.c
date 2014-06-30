/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32.c,v 3.63.2.4 1997/05/26 14:36:16 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, AND CRAIG E. GROESCHEL
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ANY OR ALL OF
 * THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Rewritten for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for 16 bpp and VTSema-independence by Craig E. Groeschel
 * Modified for more 16 bpp and Ramdac parsing by Bryan K. Feir
 *
 */
/* $XConsortium: mach32.c /main/23 1996/10/27 11:46:34 kaleb $ */

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
#include "gc.h"
#include "windowstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_HWlib.h"
#include "mach32.h"
#include "regmach32.h"

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
extern int mach32MaxClock;
extern int mach32MaxTlc34075Clock;
extern int mach32Max16bppClock;
extern Bool xf86Resetting, xf86Exiting, xf86ProbeFailed;
unsigned short mach32MemorySize = 0;
extern char *xf86VisualNames[];
extern unsigned short vgaIOBase;

static int mach32ValidMode(
#if NeedFunctionPrototypes 
    DisplayModePtr,
    Bool,
    int
#endif
);

ScrnInfoRec mach32InfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    mach32Probe,      	/* Bool (* Probe)() */
    mach32Initialize,	/* Bool (* Init)() */
    mach32ValidMode,	/* int (* ValidMode)() */
    mach32EnterLeaveVT, /* void (* EnterLeaveVT)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveMonitor)() */
    (void (*)())NoopDDA,/* void (* EnterLeaveCursor)() */
    mach32AdjustFrame,	/* void (* AdjustFrame)() */
    mach32SwitchMode,	/* Bool (* SwitchMode)() */
    (void (*)())NoopDDA,/* void (* DPMSSet)() */
    mach32PrintIdent,	/* void (* PrintIdent)() */
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
    NULL,	       	/* char *ramdac */
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
    NULL,	       	/* MonPtr monitor */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock */   
    FALSE,              /* Bool bankedMono */
    "Mach32",           /* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    mach32ValidTokens,	/* int *validTokens */
    MACH32_PATCHLEVEL,	/* char *patchlevel */
    0,			/* int IObase */
    0,			/* int PALbase */
    0,			/* int COPbase */
    0,			/* int POSbase */
    0,			/* int instance */
    0,			/* int s3Madjust */
    0,			/* int s3Nadjust */
    0,			/* int s3MClk */
    0,			/* int chipID */
    0,			/* int chipRev */
    0,			/* unsigned long VGAbase */
    0,			/* int s3RefClk */
    -1,			/* int s3BlankDelay */
    0,			/* int textClockFreq */
    NULL,               /* char* DCConfig */
    NULL,               /* char* DCOptions */
    0			/* int MemClk */
#ifdef XFreeXDGA
    ,0,			/* int directMode */
    mach32SetVGAPage,	/* Set Vid Page */
    0,			/* unsigned long physBase */
    0			/* int physSize */
#endif
};

short mach32alu[16] = {
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

LUTENTRY mach32savedLUT[256];
static Bool LUTissaved = FALSE;

short mach32MaxX, mach32MaxY;
short mach32VirtX, mach32VirtY;
short mach32DisplayWidth;

Bool mach32Use4MbAperture = FALSE;
Bool mach32DAC8Bit = FALSE;
Bool mach32DoubleClock = FALSE;

Bool mach32clkprobedif4fix = FALSE;

static unsigned Mach32_IOPorts[] = {
	/* VGA Registers */
        0x3B4, 0x3B5, 0x3BA, 0x3C0, 0x3C1, 0x3C2, 0x3C4, 0x3C5, 0x3C6, 0x3C7, 
	0x3C8, 0x3C9, 0x3CA, 0x3CB, 0x3CC, 0x3CE, 0x3CF, 0x3D4, 0x3D5, 0x3DA,
	/* ATI VGA Registers */
	ATIEXT, ATIEXT+1,
	/* 8514 Registers */
        DAC_MASK, DAC_R_INDEX, DAC_W_INDEX, DAC_DATA, DISP_STAT,
	H_TOTAL, H_DISP, H_SYNC_STRT, H_SYNC_WID,
	V_TOTAL, V_DISP, V_SYNC_STRT, V_SYNC_WID,
	DISP_CNTL, ADVFUNC_CNTL, SUBSYS_CNTL, ROM_PAGE_SEL,
	CUR_Y, CUR_X, DESTY_AXSTP, DESTX_DIASTP, 
	ERR_TERM, MAJ_AXIS_PCNT, GP_STAT, CMD, SHORT_STROKE,
	BKGD_COLOR, FRGD_COLOR, WRT_MASK, RD_MASK,
	COLOR_CMP, BKGD_MIX, FRGD_MIX, MULTIFUNC_CNTL, PIX_TRANS,
	/* Mach32 Registers */
	GE_OFFSET_LO, GE_OFFSET_HI, CRT_OFFSET_LO, CRT_OFFSET_HI,
	CLOCK_SEL, SHADOW_SET, SHADOW_CTL, CRT_PITCH, GE_PITCH,
	EXT_SCISSOR_L, EXT_SCISSOR_R, EXT_SCISSOR_T, EXT_SCISSOR_B,
	MISC_OPTIONS, CONFIG_STATUS_1, CONFIG_STATUS_2, 
	MEM_CFG, MEM_BNDRY, ROM_ADDR_1, READ_SRC_X, CHIP_ID, EXT_FIFO_STATUS,
	R_EXT_GE_CONFIG, EXT_GE_CONFIG, DP_CONFIG, DEST_X_START, DEST_X_END,
	DEST_Y_END, ALU_FG_FN, MISC_CNTL, R_MISC_CNTL, HORZ_OVERSCAN,
	VERT_OVERSCAN, EXT_GE_STATUS, LINEDRAW, LINEDRAW_OPT, LINEDRAW_INDEX,
	EXT_CURSOR_COLOR_0, EXT_CURSOR_COLOR_1,
};
static int Num_Mach32_IOPorts = (sizeof(Mach32_IOPorts)/
				 sizeof(Mach32_IOPorts[0]));

/* mach32WeightMasks must match mach32weights[], below */
static short mach32WeightMasks[] = { RGB16_565, RGB16_555,
					RGB16_655, RGB16_664 };
short mach32WeightMask;

static mach32CRTCRegRec mach32CRTCRegs;
static ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

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
   int  Graphics_Ultra_Present;
   int  ATI_Accelerator_Present;
   int  Mach8_Present;
   int  Mach32_Present;
   int  Bus_8bit_Only;
   int  Bus_Type;
   int  Mem_Size;
   int  Mem_Type;
   int  DAC_Type;
} ATIInformationBlock;

int	mach32Ramdac;
int	mach32RamdacSubType;

char	*mach32ramdac_names[] = {
	"ATI-68830",
	"IMS-G173/SC1148[368]",
	"ATI68875/TLC34075/Bt885",
	"Bt47[68]/INMOS17[68]",
	"AT&T20C49[01]/Bt48[12]/IMS-G174/MU9C{1880,4910}/SC1502[56]",
	"ATI-68860",
	"STG1700 (or similar)",
	"SC15021/STG1702/AT&T21C498"
};

SymTabRec mach32RamdacTable[] = {
    { DAC_ATI68830,  "ati68830" },
    { DAC_SC11483,   "sc11483" },
    { DAC_SC11483,   "sc11486" },
    { DAC_SC11483,   "sc11488" },
    { DAC_SC11483,   "ims_g173" },
    { DAC_SC11483,   "mu9c4870" },
    { DAC_ATI68875,  "ati68875" },
    { DAC_ATI68875,  "bt885" },
    { DAC_ATI68875,  "tlc34075" },
    { DAC_BT476,     "bt476" },
    { DAC_BT476,     "bt478" },
    { DAC_BT476,     "inmos176" },
    { DAC_BT476,     "inmos178" },
    { DAC_BT481,     "bt481" },  
    { DAC_BT481,     "bt482" },  
    { DAC_BT481,     "ims_g174" },  
    { DAC_BT481,     "mu9c1880" },  
    { DAC_BT481,     "mu9c4910" },  
    { DAC_BT481,     "sc15025" },  
    { DAC_BT481,     "sc15026" },  
    { DAC_ATT20C490, "att20c490" },
    { DAC_ATT20C490, "att20c491" },
    { DAC_ATI68860,  "ati68860" },
    { DAC_STG1700,   "stg1700" },
    { DAC_ATT21C498, "sc15021" },
    { DAC_ATT21C498, "stg1702" },
    { DAC_ATT21C498, "att21c498" },
    { -1, "" },
};

SymTabRec mach32RamdacNames[] = {
    { DAC_ATI68830,  "ATI-68830" },
    { DAC_SC11483,   "IMS-G173/SC1148[368]" },
    { DAC_ATI68875,  "ATI68875/TLC34075/Bt885" },
    { DAC_BT476,     "Bt47[68]/INMOS17[68]" },
    { DAC_BT481,     "Bt48[12]/IMS-G174/MU9C{1880,4910}/SC1502[56]" },
    { DAC_ATT20C490, "AT&T20C49[01]" },
    { DAC_ATI68860,  "ATI-68860" },
    { DAC_STG1700,   "STG1700 (or similar)" },
    { DAC_ATT21C498, "SC15021/STG1702/AT&T21C498" },
    { -1, "" },
};

int	mach32BusType;

static ATIInformationBlock *GetATIInformationBlock()
{
#define BIOS_DATA_SIZE 0x70
   char                       signature[]    = " 761295520";
   char                       bios_data[BIOS_DATA_SIZE];
   char                       bios_signature[10];
   int                        tmp;
   static ATIInformationBlock info = { 0, };
	 
   if (xf86ReadBIOS(mach32InfoRec.BIOSbase, 0x30,
		    (unsigned char *)bios_signature, 10) != 10) {
      return NULL;
   }
   if (strncmp( signature, bios_signature, 10 ))
	 return NULL;

   if (xf86ReadBIOS(mach32InfoRec.BIOSbase, 0x00,
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
   info.Graphics_Ultra_Present = !(bios_data[ 0x44 ] & 0x40);


				/* Test for graphics acceleration */
   if (!info.Graphics_Ultra_Present)
	 return &info;

				/* Test for ATI accelerator product */
   info.ATI_Accelerator_Present = 1;

   /* Reset the 8514/A, and disable all interrupts. */
   outw(SUBSYS_CNTL, GPCTRL_RESET | CHPTEST_NORMAL);
   outw(SUBSYS_CNTL, GPCTRL_ENAB | CHPTEST_NORMAL);

   tmp = inw( ROM_ADDR_1 );
   outw( ROM_ADDR_1, 0x5555 );
   ProbeWaitIdleEmpty();
   if (inw( ROM_ADDR_1 ) != 0x5555) {
      info.ATI_Accelerator_Present = 0;
   } else {
      outw( ROM_ADDR_1, 0x2a2a );
      ProbeWaitIdleEmpty();
      if (inw( ROM_ADDR_1 ) != 0x2a2a) {
	 info.ATI_Accelerator_Present = 0;
      }
   }
   outw( ROM_ADDR_1, tmp );

   if (!info.ATI_Accelerator_Present)
	 return &info;
   
				/* Determine chip family */
   outw( DESTX_DIASTP, 0xaaaa );
   WaitIdleEmpty();
   if (inw( READ_SRC_X ) != 0x02aa)
	 info.Mach8_Present = 1;
   else
	 info.Mach32_Present = 1;

   outw( DESTX_DIASTP, 0x5555 );
   WaitIdleEmpty();
   if (inw( READ_SRC_X ) != 0x0555) {
      if (!info.Mach8_Present) {
	 ErrorF( "WARNING: Chip should be Mach8, but isn't!\n" );
	 info.Mach8_Present = 0;
      }
   } else {
      if (!info.Mach32_Present) {
	 ErrorF( "WARNING: Chip should be Mach32, but isn't!\n" );
	 info.Mach32_Present = 0;
      }
   }

   if (info.Mach8_Present) {
      tmp = inw( CONFIG_STATUS_1 );

      if (!(tmp & 0x02))
	    info.Bus_8bit_Only = 1;
      
      switch( (tmp & MEM_INSTALLED) >> 5 ) {
      case 0:
	 info.Mem_Size = 512;
	 break;
      case 1:
	 info.Mem_Size = 1024;
	 break;
      }

      info.Mem_Type = (tmp & DRAM_ENA) >> 4;
   }

   if (info.Mach32_Present) {
      tmp = inw( CONFIG_STATUS_1 );
      info.Bus_Type = tmp & BUS_TYPE;
      info.Mem_Type = tmp & MEM_TYPE;
      info.DAC_Type = (tmp & DACTYPE) >> 9;

      tmp = inw( CONFIG_STATUS_2 );
      if ((info.Bus_Type == ISA_16_BIT) && !(tmp & ISA_16_ENA))
	    info.Bus_8bit_Only = 1;

      tmp = inw( MISC_OPTIONS );
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
      }
   }

   return &info;
}

/*
 * mach32Probe --
 *     Probe the hardware
 */
Bool
mach32Probe()
{
    int                   i, j;
    DisplayModePtr        pMode, pEnd;
    ATIInformationBlock   *info;
    int                   available_ram;
    Bool                  sw_cursor_supplied;
    OFlagSet              validOptions;
    int                   tx, ty;
    xrgb mach32weights[] = { { 5, 6, 5 }, { 5, 5, 5 }, { 6, 5, 5 },
			     { 6, 6, 4 } };


    xf86ClearIOPortList(mach32InfoRec.scrnIndex);
    xf86AddIOPorts(mach32InfoRec.scrnIndex, Num_Mach32_IOPorts, Mach32_IOPorts);

    xf86EnableIOPorts(mach32InfoRec.scrnIndex);

    info = GetATIInformationBlock();
    if (!info || !info->Mach32_Present) {
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

    if (xf86bpp < 0) {
	xf86bpp = mach32InfoRec.depth;
    }
    if (xf86weight.red == 0 || xf86weight.green == 0 || xf86weight.blue == 0) {
	xf86weight = mach32InfoRec.weight;
    }

    switch (xf86bpp) {
    case 8:
	break;
    case 15:
    case 16:
        if (info->DAC_Type == DAC_BT476) {
/*
 * Hate to break the news to them, but hopefully this will forestall
 * queries of, "But you said it supports 16bpp...?"
 */
	    ErrorF("Unsupported bpp--this ramdac supports only 8 bpp\n");
	    return(FALSE);
	}
        if (info->DAC_Type != DAC_TLC34075 && info->DAC_Type != DAC_BT481 &&
            info->DAC_Type != DAC_ATT20C490) {
	    ErrorF("Unsupported bpp in this version of code.\n");
	    return(FALSE);
	}

	mach32InfoRec.depth = xf86bpp;	/* if 16/555, set to 15, below */
	mach32InfoRec.bitsPerPixel = 16;
	if (mach32InfoRec.defaultVisual < 0)
	    mach32InfoRec.defaultVisual = TrueColor;
	if (defaultColorVisualClass < 0)
	    defaultColorVisualClass = mach32InfoRec.defaultVisual;
	if (defaultColorVisualClass != TrueColor) {
	    ErrorF("Invalid default visual type: %d (%s)\n",
		   defaultColorVisualClass,
		   xf86VisualNames[defaultColorVisualClass]);
	    return(FALSE);
	}
	break;
    default:
	ErrorF("Invalid bpp--valid ");
	if (info->DAC_Type != DAC_TLC34075 && info->DAC_Type != DAC_BT481)
	    ErrorF("number of bpp is 8\n");
	else
	    ErrorF("numbers of bpp are 8, 15 and 16\n");
	return(FALSE);
    }

    if (xf86bpp == 15) {
        if (xf86weight.red != 5 && xf86weight.green != 5
            && xf86weight.blue != 5) {
	    ErrorF("Invalid color weighting\n");
	    return(FALSE);
	}
        mach32WeightMask = mach32WeightMasks[1];
    }
    if (xf86bpp == 16) {
	for (i = 0; i < 4; i++) {
	    if (xf86weight.red == mach32weights[i].red
		&& xf86weight.green == mach32weights[i].green
		&& xf86weight.blue == mach32weights[i].blue)
	    break;
	}
	if (i == 4) {
	    ErrorF("Invalid color weighting\n");
	    return(FALSE);
	}
	if (i == 1)
	    mach32InfoRec.depth = 15;
        /* Of course, the SC11483 doesn't get here, but... */
	if ( (info->DAC_Type == DAC_SC11483 || info->DAC_Type == DAC_BT481 ||
              info->DAC_Type == DAC_ATT20C490) && (i > 1)) {
	    ErrorF("Invalid RGB weighting--valid weights are 555 and 565.\n");
	    return(FALSE);
	}
	mach32WeightMask = mach32WeightMasks[i];
    }

    /* There are several considerations in working out the max clock: */
    mach32InfoRec.maxClock = mach32MaxClock;
    /* Type 2 and 5 DACs are high performance, and can handle 135MHz */
    if (info->DAC_Type == DAC_TLC34075 || info->DAC_Type == DAC_ATI68860) {
        mach32InfoRec.maxClock = mach32MaxTlc34075Clock;
    }
    if (mach32InfoRec.bitsPerPixel == 16) {
        /* Only the ATI68860 can MUX 4 pixels at 16bpp each */
        if (info->DAC_Type != DAC_ATI68860) {
            if (info->DAC_Type == DAC_ATI68875)
                mach32InfoRec.maxClock = mach32MaxClock;
            else
                mach32InfoRec.maxClock /= 2;
        }
        /* 8-bit data path DACs have to be double-clocked */
        if (info->DAC_Type == DAC_ATI68830 || info->DAC_Type == DAC_SC11483 ||
            info->DAC_Type == DAC_BT481 || info->DAC_Type == DAC_ATT20C490) {
	    ErrorF("%s %s: Eight-bit DAC path, halving clocks\n",
                   XCONFIG_PROBED, mach32InfoRec.name);
            mach32DoubleClock = TRUE;
        }
    }

    OFLG_ZERO(&validOptions);
    OFLG_SET(OPTION_CLKDIV2, &validOptions);
    OFLG_SET(OPTION_SW_CURSOR, &validOptions);
    OFLG_SET(OPTION_NOLINEAR_MODE, &validOptions);
    OFLG_SET(OPTION_CSYNC, &validOptions);
    OFLG_SET(OPTION_INTEL_GX, &validOptions);
    OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
    xf86VerifyOptions(&validOptions, &mach32InfoRec);
    
    mach32InfoRec.chipset = "mach32";
    xf86ProbeFailed = FALSE;

    mach32BusType = info->Bus_Type;
    if (xf86Verbose)
    {
	ErrorF("%s %s: card type: ", XCONFIG_PROBED, mach32InfoRec.name);
	switch(mach32BusType)
	{
	case EISA:
	    ErrorF("EISA\n");
	    break;
	case LOCAL_386SX:
	    ErrorF("localbus (386SX)\n");
	    break;
	case LOCAL_386DX:
	    ErrorF("localbus (386DX)\n");
	    break;
	case LOCAL_486:
	    ErrorF("localbus (486)\n");
	    break;
	case PCI:
	    ErrorF("PCI\n");
	    break;
	default:
	    ErrorF("ISA\n");
	}
    }

    if (!mach32InfoRec.clocks) {
        outb(DISP_CNTL, DISPEN_DISAB | MEMCFG_4 | ODDBNKENAB);
        /* 13-jun-93 TCG : set up dummy video mode */
        outw(SHADOW_SET, 1);
        outw(SHADOW_CTL, 0);
        outw(SHADOW_SET, 2);
        outw(SHADOW_CTL, 0);
        outw(SHADOW_SET, 0);
        outw(ADVFUNC_CNTL, DISABPASSTHRU);

        /* vt: 480 488 +31 528 hz: 640 656 +248 920 */
        outw(V_TOTAL, 0x420);
        outw(V_DISP, 0x3c0);
        outw(V_SYNC_STRT, 0x3d0);
        outw(V_SYNC_WID, 0x1f);
        outb(H_TOTAL, 0x72);
        outw(H_DISP, 0x4f);
        outb(H_SYNC_STRT, 0x51);
        outb(H_SYNC_WID, 0x1f);

        outb(DAC_MASK, 0x00);
        outb(DISP_CNTL, DISPEN_ENAB | MEMCFG_4 | ODDBNKENAB);

        /* 2-oct-93 TCG : detect clocks with dif4 fix */
        mach32clkprobedif4fix = TRUE;
        xf86GetClocks(16, mach32ClockSelect, (void (*)())NoopDDA,
			(SaveScreenProcPtr)NoopDDA, DISP_STAT, 2, 7, 44900,
			&mach32InfoRec);
        outw(CLOCK_SEL, 0); /* reset pass-through */
        mach32clkprobedif4fix = FALSE;

        outw(SHADOW_SET, 1);
        outw(SHADOW_CTL, 0x3f);
        outw(SHADOW_SET, 2);
        outw(SHADOW_CTL, 0x3f);
        outw(SHADOW_SET, 0);

        outb(DAC_MASK, 0xff);

        if (mach32DoubleClock) {
            for (j = 0; j < 16; j++)
                mach32InfoRec.clock[j] /= 2;
            mach32InfoRec.clocks = 16;
        } else {
            for (j = 0; j < 16; j++)
                mach32InfoRec.clock[j + 16] = mach32InfoRec.clock[j] / 2;
            mach32InfoRec.clocks = 32;
        }
    } else if (mach32DoubleClock) {
        if (mach32InfoRec.clocks > 16)
            mach32InfoRec.clocks = 16;
        for (j = 0; j < mach32InfoRec.clocks; j++)
            mach32InfoRec.clock[j] /= 2;
    }

    if (xf86Verbose) {
	ErrorF("%s ",OFLG_ISSET(XCONFIG_CLOCKS,&mach32InfoRec.xconfigFlag) ?
			XCONFIG_GIVEN : XCONFIG_PROBED);
	ErrorF("%s: ", mach32InfoRec.name);
	ErrorF("Number of Clocks: %d", mach32InfoRec.clocks);

	for (i = 0; i < mach32InfoRec.clocks; i++) {
	    if (i % 8 == 0) 
               ErrorF("\n%s %s: clocks:", 
                 OFLG_ISSET(XCONFIG_CLOCKS,&mach32InfoRec.xconfigFlag) ?
                 XCONFIG_GIVEN : XCONFIG_PROBED,
                 mach32InfoRec.name);
	    ErrorF(" %6.2f", mach32InfoRec.clock[i]/1000.0);
	}
	ErrorF("\n");
    }

    if (!mach32InfoRec.videoRam) {
#ifdef NEW_MEM_DETECT
	mach32InfoRec.videoRam = mach32GetMemSize();
#else
	mach32InfoRec.videoRam = info->Mem_Size;
#endif
    }

    tx = mach32InfoRec.virtualX;
    ty = mach32InfoRec.virtualY;
    pMode = mach32InfoRec.modes;
    if (pMode == NULL) {
	ErrorF("No modes supplied in XF86Config\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }
    pEnd = (DisplayModePtr)NULL;
    do {
	  DisplayModePtr pModeSv;
	  /*
	   * xf86LookupMode returns FALSE if it ran into an invalid
	   * parameter
	   */
	  if(xf86LookupMode(pMode, &mach32InfoRec, LOOKUP_DEFAULT) == FALSE) {
		pModeSv=pMode->next;
		xf86DeleteMode(&mach32InfoRec, pMode);
		pMode = pModeSv; 
	  } else if (pMode->HDisplay > 1536) {
		pModeSv=pMode->next;
		ErrorF("%s %s: Width of mode \"%s\" is too large (max is"
		       " 1536)\n", XCONFIG_PROBED, mach32InfoRec.name,
		       pMode->name);
		xf86DeleteMode(&mach32InfoRec, pMode);
		pMode = pModeSv; 
	  } else if (pMode->VDisplay > 1536) {
		pModeSv=pMode->next;
		ErrorF("%s %s: Height of mode \"%s\" is too large (max is"
		       " 1536)\n", XCONFIG_PROBED, mach32InfoRec.name,
		       pMode->name);
		xf86DeleteMode(&mach32InfoRec, pMode);
		pMode = pModeSv; 
	  } else if (pMode->HDisplay * pMode->VDisplay *
		     (mach32InfoRec.bitsPerPixel / 8) >
		     mach32InfoRec.videoRam*1024) {
		pModeSv=pMode->next;
		ErrorF("%s %s: Too little memory for mode \"%s\"\n",
		       XCONFIG_PROBED, mach32InfoRec.name, pMode->name);
		xf86DeleteMode(&mach32InfoRec, pMode);
		pMode = pModeSv; 
	  } else if (((tx > 0) && (pMode->HDisplay > tx)) || 
		     ((ty > 0) && (pMode->VDisplay > ty))) {
		pModeSv=pMode->next;
		ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
		       XCONFIG_PROBED, mach32InfoRec.name,
			pMode->HDisplay, pMode->VDisplay, tx, ty);
		xf86DeleteMode(&mach32InfoRec, pMode);
		pMode = pModeSv;
	  } else {
		/*
		 * Successfully looked up this mode.  If pEnd isn't 
		 * initialized, set it to this mode.
		 */
		if(pEnd == (DisplayModePtr) NULL)
			pEnd = pMode;

		mach32InfoRec.virtualX = max(mach32InfoRec.virtualX,
					     pMode->HDisplay);
		mach32InfoRec.virtualY = max(mach32InfoRec.virtualY,
					     pMode->VDisplay);
		pMode = pMode->next;
	  }
    } while (pMode != pEnd);

    mach32VirtX = mach32InfoRec.virtualX = (mach32InfoRec.virtualX+7) & 0xfff8;
    mach32VirtY = mach32InfoRec.virtualY;

    if ((tx != mach32InfoRec.virtualX) || (ty != mach32InfoRec.virtualY))
	OFLG_CLR(XCONFIG_VIRTUAL,&mach32InfoRec.xconfigFlag);

    /*
     * Limitation of 8514 drawing commands
     */
    if (mach32VirtY > 1536) {
	ErrorF("Virtual screen height must be no greater than 1536\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }
    if (mach32VirtX > 1536) {
	ErrorF("Virtual screen width must be no greater than 1536\n");
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

    if (xf86Verbose) {
	ErrorF("%s %s: Virtual resolution: %dx%d\n",
	       OFLG_ISSET(XCONFIG_VIRTUAL, &mach32InfoRec.xconfigFlag) ?
		XCONFIG_GIVEN : XCONFIG_PROBED,
		mach32InfoRec.name, mach32VirtX, mach32VirtY);
    }

    if (mach32VirtX < 1024)
	mach32DisplayWidth = 1024;
    else
	mach32DisplayWidth = mach32VirtX;

    if (xf86Verbose) {
	ErrorF("%s %s: Display width: %d\n",
		XCONFIG_PROBED, mach32InfoRec.name, mach32DisplayWidth);
    }

    /* Set mach32MemorySize to required MEM_SIZE value in MISC_OPTIONS */
    if (mach32InfoRec.videoRam <= 512)
	mach32MemorySize = MEM_SIZE_512K;
    else if (mach32InfoRec.videoRam <= 1024)
	mach32MemorySize = MEM_SIZE_1M;
    else if (mach32InfoRec.videoRam <= 2048)
	mach32MemorySize = MEM_SIZE_2M;
    else
	mach32MemorySize = MEM_SIZE_4M;
	
    if (xf86Verbose) {
       ErrorF("%s %s: videoram: %dk\n",
	      OFLG_ISSET(XCONFIG_VIDEORAM, &mach32InfoRec.xconfigFlag) ?
	      XCONFIG_GIVEN : XCONFIG_PROBED, mach32InfoRec.name,
	      mach32InfoRec.videoRam );
    }

    if (((mach32DisplayWidth) * (mach32VirtY) *
	 (mach32InfoRec.bitsPerPixel / 8)) >
	(mach32InfoRec.videoRam*1024)) {
	ErrorF("Not enough memory for requested display size (%dx%d)\n",
	       mach32DisplayWidth, mach32VirtY);
	xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	return(FALSE);
    }

			/* Set values of mach32MaxX and mach32MaxY.
			 * This must be done here so that cache area and
			 * scissor limits are set up correctly (this setup
			 * happens before the pixmap cache is initialized).
			 */
    
    mach32MaxX = mach32DisplayWidth - 1;
    available_ram = mach32InfoRec.videoRam * 1024;

    sw_cursor_supplied = OFLG_ISSET(OPTION_SW_CURSOR, &mach32InfoRec.options);
    if (!sw_cursor_supplied)
    {
	if (available_ram - (mach32DisplayWidth * mach32VirtY) >=
	    MACH32_CURSBYTES)
	{
	    available_ram -= (MACH32_CURSBYTES + 1023) & ~1023;
	    mach32InfoRec.videoRam -= (MACH32_CURSBYTES + 1023) / 1024;
	}
	else
	{ 
	    OFLG_SET(OPTION_SW_CURSOR, &mach32InfoRec.options);
	    ErrorF("Warning: Not enough memory to use the hardware cursor.\n");
	    ErrorF("  Decreasing the virtual Y resolution by 1 will allow\n");
	    ErrorF("  you to use the hardware cursor.\n");
	}
    }
	
    ErrorF("%s %s: Using %s cursor\n", sw_cursor_supplied ? XCONFIG_GIVEN :
	   XCONFIG_PROBED, mach32InfoRec.name,
	   OFLG_ISSET(OPTION_SW_CURSOR, &mach32InfoRec.options) ?
	   "software" : "hardware");

    mach32MaxY = available_ram /
                 (mach32DisplayWidth * (mach32InfoRec.bitsPerPixel / 8)) - 1;

    if (mach32MaxY > 1535)
	  mach32MaxY = 1535;	/* Limitation of 8514 drawing commands */
	  
    mach32Use4MbAperture = !OFLG_ISSET(OPTION_NOLINEAR_MODE,
				       &mach32InfoRec.options) &&
			   (info->Bus_Type == LOCAL_386SX ||
			    info->Bus_Type == LOCAL_386DX ||
			    info->Bus_Type == LOCAL_486 ||
			    info->Bus_Type == EISA ||
                            info->Bus_Type == PCI)
			   && xf86LinearVidMem();

    if (mach32InfoRec.ramdac) {
	mach32RamdacSubType =
	    xf86StringToToken(mach32RamdacTable, mach32InfoRec.ramdac);
	mach32Ramdac = mach32RamdacSubType & 0x0f;
	if (mach32RamdacSubType < 0) {
	    ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
		   mach32InfoRec.name, mach32InfoRec.ramdac);
	    xf86DisableIOPorts(mach32InfoRec.scrnIndex);
	    return(FALSE);
	}
    } else {
	mach32Ramdac = info->DAC_Type;
	mach32RamdacSubType = mach32Ramdac;
    }

    if (xf86Verbose) {
	if (mach32Use4MbAperture) {
	    ErrorF("%s %s: Using 4 MB aperture\n", XCONFIG_PROBED,
		   mach32InfoRec.name);
	} else {
	    ErrorF("%s %s: Using 64 kb aperture\n",
		   OFLG_ISSET(OPTION_NOLINEAR_MODE, &mach32InfoRec.options) ?
		   XCONFIG_GIVEN : XCONFIG_PROBED, mach32InfoRec.name);
	}
	for (i = 0;
	     ((mach32RamdacNames[i].token != -1) &&
	      (mach32RamdacNames[i].token != mach32RamdacSubType));
	     i++);
	if (mach32RamdacNames[i].token == -1) {
	    ErrorF("%s %s: Ramdac is Unknown (%d)\n",
		   (mach32InfoRec.ramdac ? XCONFIG_GIVEN : XCONFIG_PROBED),
		   mach32InfoRec.name,
		   mach32RamdacSubType);
	} else {
	    ErrorF("%s %s: Ramdac is %s\n",
		   (mach32InfoRec.ramdac ? XCONFIG_GIVEN : XCONFIG_PROBED),
		   mach32InfoRec.name,
		   mach32RamdacNames[i].name);
	}
    }

    /* The Type 2 RAMDACS can support 8 bits per RGB value, not just the
     * VGA-standard 6 bits; however, according to ATI, this is not an
     * official feature of the mach32.
     */
    mach32DAC8Bit = OFLG_ISSET(OPTION_DAC_8_BIT, &mach32InfoRec.options)
		    && (info->DAC_Type == DAC_TLC34075
                        || info->DAC_Type == DAC_ATT20C490)
		    && mach32InfoRec.bitsPerPixel == 8;

    if (xf86Verbose) {
	if (mach32InfoRec.bitsPerPixel == 8)
	    ErrorF("%s %s: Using %d bits per RGB value\n",
		(info->DAC_Type == DAC_TLC34075) ?
		XCONFIG_GIVEN : XCONFIG_PROBED, mach32InfoRec.name,
		mach32DAC8Bit ?  8 : 6);
	else if (mach32InfoRec.bitsPerPixel == 16)
	    ErrorF("%s %s: Color weight: %1d%1d%1d\n", XCONFIG_GIVEN,
		mach32InfoRec.name, xf86weight.red,
		xf86weight.green, xf86weight.blue);
    }

#ifdef XFreeXDGA
    mach32InfoRec.displayWidth = mach32DisplayWidth;
    mach32InfoRec.directMode = XF86DGADirectPresent;
#endif

    return(TRUE);
}


/*
 * mach32PrintIdent --
 *     Print the indentification of the video card.
 */
void
mach32PrintIdent()
{
    ErrorF("  %s: accelerated server for ATI Mach32 graphics adaptors ",
	   mach32InfoRec.name);
    ErrorF("(Patchlevel %s)\n", mach32InfoRec.patchLevel);
}


/*
 * mach32Initialize --
 *      Attempt to find and initialize a VGA framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
mach32Initialize (scr_index, pScreen, argc, argv)
    int            scr_index;    /* The index of pScreen in the ScreenInfo */
    ScreenPtr      pScreen;      /* The Screen to initialize */
    int            argc;         /* The number of the Server's arguments. */
    char           **argv;       /* The arguments themselves. Don't change! */
{
    int displayResolution = 75;  /* default to 75dpi */
    extern int monitorResolution;

    mach32InitGC();
    mach32InitDisplay(scr_index);
    mach32InitAperture(scr_index);
    mach32CalcCRTCRegs(&mach32CRTCRegs, mach32InfoRec.modes);
    mach32SetCRTCRegs(&mach32CRTCRegs);
    mach32InitEnvironment();


    /* Clear the display.
     * Need to set the color, origin, and size.  Then draw.
     */
    WaitQueue(6);
    if (xf86bpp == 8 && xf86FlipPixels)
	outw(FRGD_COLOR, 1);
    else
	outw(FRGD_COLOR, 0);
    outw(CUR_X, 0);
    outw(CUR_Y, 0);
    outw(MAJ_AXIS_PCNT, mach32MaxX);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | mach32MaxY);
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

    xf86InitCache(mach32CacheMoveBlock);
    mach32FontCache8Init();

    mach32ImageInit();

    WaitIdleEmpty(); /* Make sure that all commands have finished */

    /*
     * Take display resolution from the -dpi flag if specified
     */

    if (monitorResolution)
	displayResolution = monitorResolution;

    if (!mach32ScreenInit(pScreen, mach32VideoMem,
			  mach32VirtX, mach32VirtY,
			  displayResolution, displayResolution,
			  mach32DisplayWidth))

		return(FALSE);

    savepScreen = pScreen;

    pScreen->CloseScreen = mach32CloseScreen;
    pScreen->SaveScreen = mach32SaveScreen;

    switch (mach32InfoRec.bitsPerPixel) {
    case 8:
	pScreen->InstallColormap = mach32InstallColormap;
	pScreen->UninstallColormap = mach32UninstallColormap;
	pScreen->ListInstalledColormaps = mach32ListInstalledColormaps;
	pScreen->StoreColors = mach32StoreColors;
	break;
    case 16:
	pScreen->InstallColormap = cfbInstallColormap;
	pScreen->UninstallColormap = cfbUninstallColormap;
	pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
	pScreen->StoreColors = (void (*)())NoopDDA;
    }

    if (OFLG_ISSET(OPTION_SW_CURSOR, &mach32InfoRec.options)) {
	miDCInitialize (pScreen, &xf86PointerScreenFuncs);
    } else {
	pScreen->QueryBestSize = mach32QueryBestSize;
	xf86PointerScreenFuncs.WarpCursor = mach32WarpCursor;
	(void)mach32CursorInit(0, pScreen);
    }

    return (cfbCreateDefColormap(pScreen));
}


/*
 *	Assign a new serial number to the window.
 *	Used to force GC validation on VT switch.
 */

/*ARGSUSED*/
static int
mach32NewSerialNumber(pWin, data)
    WindowPtr pWin;
    pointer data;
{
    pWin->drawable.serialNumber = NEXT_SERIAL_NUMBER;
    return WT_WALKCHILDREN;
}


/*
 * mach32EnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
mach32EnterLeaveVT(enter, screen_idx)
     Bool enter;
     int screen_idx;
{
    PixmapPtr pspix;
    ScreenPtr pScreen = savepScreen;

    if (!xf86Exiting && !xf86Resetting) {
	/* cfbGetScreenPixmap(pScreen) */
	switch (mach32InfoRec.bitsPerPixel) {
	case 8:
	    pspix = (PixmapPtr)pScreen->devPrivate;
	    break;
	case 16:
	    {
		pspix =
		  (PixmapPtr)pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr;
	    }
	    break;
	}
    }

    if (pScreen && !xf86Exiting && !xf86Resetting)
	WalkTree(pScreen, mach32NewSerialNumber, 0);

    if (enter) {
	if (vgaBase)
	    xf86MapDisplay(screen_idx, VGA_REGION);
	if (mach32VideoMem != vgaBase)
	    xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!xf86Resetting) {
	    ScrnInfoPtr pScr = XF86SCRNINFO(pScreen);

	    mach32InitDisplay(screen_idx);
            mach32CalcCRTCRegs(&mach32CRTCRegs, mach32InfoRec.modes); /* 96/02/12 SM */
	    mach32SetCRTCRegs(&mach32CRTCRegs);
	    mach32InitEnvironment();

	    WaitQueue(6);
	    outw(FRGD_COLOR, 1);
	    outw(CUR_X, 0);
	    outw(CUR_Y, 0);
	    outw(MAJ_AXIS_PCNT, mach32MaxX);
	    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | mach32MaxY);
	    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PLANAR | WRTDATA);

	    mach32FontCache8Init();
	    mach32RestoreCursor(pScreen);
	    mach32AdjustFrame(pScr->frameX0, pScr->frameY0);

	    WaitIdleEmpty(); /* Make sure that all commands have finished */

	    if (LUTissaved) {
		mach32RestoreLUT(mach32savedLUT);
		LUTissaved = FALSE;
		mach32RestoreColor0(pScreen);
	    }

	    if (pspix->devPrivate.ptr != mach32VideoMem && ppix) {
		pspix->devPrivate.ptr = mach32VideoMem;
		(mach32ImageWriteFunc)(0, 0, pScreen->width, pScreen->height,
				 ppix->devPrivate.ptr,
				 PixmapBytePad(mach32DisplayWidth,
					       pScreen->rootDepth),
				 0, 0, MIX_SRC, ~0);
	    }
	    if (pScreen) {
		pScreen->CopyWindow = mach32CopyWindow;
		pScreen->GetSpans = mach32GetSpans;
		pScreen->PaintWindowBackground = mach32PaintWindow;
		pScreen->PaintWindowBorder = mach32PaintWindow;
	    }
	}
	if (ppix) {
	    (pScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
	}
    } else {
	if (vgaBase)
	    xf86MapDisplay(screen_idx, VGA_REGION);
	if (mach32VideoMem != vgaBase)
	    xf86MapDisplay(screen_idx, LINEAR_REGION);
	if (!xf86Exiting) {
	    ppix = (pScreen->CreatePixmap)(pScreen,
					   mach32DisplayWidth, pScreen->height,
					   pScreen->rootDepth);

	    if (ppix) {
		(mach32ImageReadFunc)(0, 0, pScreen->width, pScreen->height,
				ppix->devPrivate.ptr,
				PixmapBytePad(mach32DisplayWidth,
					      pScreen->rootDepth),
				0, 0, ~0);
		pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	    }
	    switch (mach32InfoRec.bitsPerPixel) {
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
	    }
	    if (!mach32Use4MbAperture) {
		pScreen->PaintWindowBackground = miPaintWindow;
		pScreen->PaintWindowBorder = miPaintWindow;
	    }
	}

	mach32CursorOff();
	mach32SaveLUT(mach32savedLUT);
	LUTissaved = TRUE;
	if (!xf86Resetting) {
#ifdef XFreeXDGA
	    if (!(mach32InfoRec.directMode & XF86DGADirectGraphics))
#endif
	        mach32CleanUp();
	}
	if (vgaBase)
	    xf86UnMapDisplay(screen_idx, VGA_REGION);
	if (mach32VideoMem != vgaBase)
	    xf86UnMapDisplay(screen_idx, LINEAR_REGION);
    }
}

/*
 * mach32CloseScreen --
 *      called to ensure video is enabled when server exits.
 */

/*ARGSUSED*/
Bool
mach32CloseScreen(screen_idx, pScreen)
     int	screen_idx;
     ScreenPtr	pScreen;
{
    extern void mach32ClearSavedCursor();

    /*
     * Hmm... The server may shut down even if it is not running on the
     * current vt. Let's catch this case here.
     */
    xf86Exiting = TRUE;
    if (xf86VTSema)
	mach32EnterLeaveVT(LEAVE, screen_idx);
    else if (ppix) {
    /* 7-Jan-94 CEG: The server is not running on the current vt.
     * Free the screen snapshot taken when the server vt was left.
     */
	    (savepScreen->DestroyPixmap)(ppix);
	    ppix = NULL;
    }
    mach32ClearSavedCursor(screen_idx);
    if (mach32InfoRec.bitsPerPixel == 8)
	cfbCloseScreen(screen_idx, savepScreen);
    else
	cfb16CloseScreen(screen_idx, savepScreen);
    savepScreen = NULL;
    return(TRUE);
}

/*
 * mach32SaveScreen --
 *      blank the screen.
 */

Bool
mach32SaveScreen (pScreen, on)
     ScreenPtr     pScreen;
     Bool          on;
{
     int	   ext_ge_config;

    if (on)
	SetTimeSinceLastInputEvent();

    if (xf86VTSema) {
	/*
	 *	Make sure that register 0-3 are addressed
	 */
	ext_ge_config = inw(R_EXT_GE_CONFIG) & ~0x3000;
	outw(EXT_GE_CONFIG, ext_ge_config);

/* Quick and dirty hack:
 * To save the screen at 16 bpp, disable the CRT controller.  CEG
 */
	if (on) {
	    if (mach32InfoRec.bitsPerPixel == 8) {
		mach32RestoreColor0(pScreen);
		outb(DAC_MASK, 0xff);
	    } else {
                mach32CalcCRTCRegs(&mach32CRTCRegs, mach32InfoRec.modes); /* 96/02/12 SM */
		mach32SetCRTCRegs(&mach32CRTCRegs);
	    }
	} else {
	    if (mach32InfoRec.bitsPerPixel == 8) {
		WaitQueue(5);
		outb(DAC_W_INDEX, 0);
		outb(DAC_DATA, 0);
		outb(DAC_DATA, 0);
		outb(DAC_DATA, 0);
		outb(DAC_MASK, 0x00);
	    } else {
		WaitQueue(1);
		outb(DISP_CNTL, DISPEN_DISAB);
	    }
	}
    }
    return(TRUE);
}

/*
 * mach32AdjustFrame --
 *      Modify the CRT_OFFSET for panning the display.
 */
void
mach32AdjustFrame(x, y)
    int x, y;
{
    /* cursor offset in units of bytes/4 */
    int byte_offset = ((x + y * mach32DisplayWidth) *
			(mach32InfoRec.bitsPerPixel / 8)) >> 2;

    mach32CursorOff();
    WaitQueue(2);
    outw(CRT_OFFSET_LO, byte_offset & 0xffff);
    outw(CRT_OFFSET_HI, (byte_offset >> 16) & 0xf);
    mach32RepositionCursor(savepScreen);

#ifdef XFreeXDGA
    if (mach32InfoRec.directMode & XF86DGADirectGraphics) {
	/* Wait until vertical retrace is in progress. */
	while (inb(vgaIOBase + 0xA) & 0x08);
	while (!(inb(vgaIOBase + 0xA) & 0x08));
    }
#endif
}

/*
 * mach32SwitchMode --
 *      Reinitialize the CRTC registers for the new mode.
 */
Bool
mach32SwitchMode(mode)
    DisplayModePtr mode;
{
    mach32CalcCRTCRegs(&mach32CRTCRegs, mode);
    mach32SetCRTCRegs(&mach32CRTCRegs);

    return(TRUE);
}

/*
 * mach32ClockSelect
 */
Bool
mach32ClockSelect(no)
     int no;
{
  switch(no)
  {
    case CLK_REG_SAVE:
      break;
    case CLK_REG_RESTORE:
      break;
    default:
      if (mach32clkprobedif4fix)
         outw(CLOCK_SEL, (no << 2) | 0xac1);
      else
         outw(CLOCK_SEL, (no << 2) | 0xa01);
  }
  return(TRUE);
}

/*
 * mach32ValidMode --
 *
 */
static int
mach32ValidMode(pMode, verbose, flag)
DisplayModePtr pMode;
Bool verbose;
{
    return MODE_OK;
}

