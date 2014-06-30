/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agx.c,v 3.53.2.3 1997/05/11 02:56:01 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
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
 * THOMAS ROELL, KEVIN E. MARTIN, RICKARD E. FAITH, AND HENRY A. WORTH
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
 * THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA 
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 * Rewritten for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Rewritten for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Rewritten for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 *
 */
/* $XConsortium: agx.c /main/20 1996/10/27 11:04:14 kaleb $ */


#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "mipointer.h"
#include "cursorstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "cfb.h"
#include "cfb16.h"
#ifdef AGX_32BPP
#include "cfb32.h"
#endif
#include "mi.h"
#include "agx.h"
#include "regagx.h"
#include "xf86RamDac.h"
#include "hercRamDac.h"

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

extern Bool xf86Resetting, xf86Exiting, xf86ProbeFailed;
extern Bool miDCInitialize();
extern void SetTimeSinceLastInputEvent();

static int agxValidMode(
#if NeedFunctionPrototypes
    DisplayModePtr,
    Bool,
    int
#endif
); 

ScrnInfoRec agxInfoRec = {
    FALSE,		/* Bool configured */
    -1,			/* int tmpIndex */
    -1,			/* int scrnIndex */
    agxProbe,      	/* Bool (* Probe)() */
    agxInit,	        /* Bool (* Init)() */
    agxValidMode,       /* int (* ValidMode)() */
    agxEnterLeaveVT,    /* void (* EnterLeaveVT)() */
    (void(*)())NoopDDA,	/* void (* EnterLeaveMonitor)() */
    (void(*)())NoopDDA,	/* void (* EnterLeaveCursor)() */
    agxAdjustFrame, 	/* void (* AdjustFrame)() */
    agxSwitchMode,      /* Bool (* SwitchMode)() */
    (void (*)())NoopDDA,/* void (* DPMSSet)() */
    agxPrintIdent,	/* void (* PrintIdent)() */
    8,			/* int depth */
    {0, 0, 0},          /* xrgb weight */
    8,			/* int bitsPerPixel */
    PseudoColor,       	/* int defaultVisual */
    -1, -1,		/* int virtualX,virtualY */
    -1,                 /* int displayWidth */
    -1, -1, -1, -1,	/* int frameX0, frameY0, frameX1, frameY1 */
    {0, },	       	/* OFlagSet options */
    {0, },	       	/* OFlagSet clockOptions */
    {0, },	       	/* OFlagSet xconfigFlag */
    NULL,       	/* char *chipset */
    NULL,       	/* char *ramdac */
    {0, 0, 0, 0},	/* int dacSpeeds[MAXDACSPEEDS] */
    0,			/* int dacSpeedBpp */
    0,			/* int clocks */
    {0, },		/* int clock[MAXCLOCKS] */
    0,			/* int maxClock */
    2048,		/* int videoRam */
    0xC0000,            /* int BIOSbase */   
    0,	        	/* unsigned long MemBase */
    240, 180,		/* int width, height */
    0,                  /* unsigned long  speedup */
    NULL,	       	/* DisplayModePtr modes */
    NULL,	       	/* MonPtr monitor */
    NULL,               /* char *clockprog */
    -1,                 /* int textclock */   
    FALSE,              /* Bool bankedMono */
    "AGX",              /* char *name */
    {0, },		/* xrgb blackColour */
    {0, },		/* xrgb whiteColour */
    agxValidTokens,	/* int *validTokens */
    AGX_PATCHLEVEL,	/* char *patchlevel */
    0,		 	/* unsigned int IObase */
    0, 			/* unsigned int PALbase */
    0,			/* unsigned long COPbase */
    POS_DEF_IO_BASE,    /* unsigned int POSbase      */
    -1,                 /* int instance     */
    0,                  /* int s3Madjust    */
    0,                  /* int s3Nadjust    */
    0,                  /* int s3MClk    */
    0,                  /* int chipID    */
    0,                  /* int chipRev   */
    0xA0000,		/* unsigned long VGAbase */
    0,                  /* int s3RefClk    */
    -1,                 /* int s3BlankDelay    */
    0,			/* int textClockFreq */
  NULL,                 /* char* DCConfig */
  NULL,                 /* char* DCOptions */
    0			/* int MemClk */
#ifdef XFreeXDGA
    ,0,                  /* int directMode    */
    agxSetVidPage,      /* Set Vid Page    */
    0,                  /* unsigned long physBase    */
    0                  /* int physSize    */
#endif
};

int vgaInterlaceType = VGA_NO_DIVIDE_VERT;
/* void (*vgaSaveScreenFunc)() = (void (*)())NoopDDA; */
void (*vgaSaveScreenFunc)() = vgaHWSaveScreen; 
pointer vgaNewVideoState = NULL;

static Bool LUTissaved = FALSE;
ScreenPtr savepScreen = NULL;
static PixmapPtr ppix = NULL;

static unsigned agxDAIOPorts[ DA_NUM_IO_REG ] = {0, };
static unsigned agxPOSIOPorts[ POS_NUM_IO_REG ] = {0, };

static SymTabRec agxDacTable[] = {
   { NORMAL_DAC,        "normal" },
   { BT481_DAC,         "bt481" },
   { BT482_DAC,         "bt482" },
#if 0  /* not yet directly supported */
   { BT485_DAC,         "bt485" },
   { ATT20C505_DAC,     "att20c505" },
#endif
   { SC15025_DAC,       "sc15025" },
   { HERC_DUAL_DAC,     "herc_dual_dac" },
   { HERC_SMALL_DAC,    "herc_small_dac" },
   { XGA_DAC,           "xga" },
   { ATT490_DAC,        "att20c490" },
   { -1,                "" },
};


unsigned short agxMemorySize = 0;
pointer  agxPhysVidMem = NULL;
pointer  vgaPhysBase = NULL;
pointer  vgaBase = NULL;
pointer  vgaVirtBase = NULL;
pointer  agxVideoMem = NULL;
unsigned long agxVideoBase = 0x0;
unsigned long agxPOSMemBase = 0x0;
unsigned long vgaBankSize = 0x10000;
unsigned long agxBankSize = 0x10000;
unsigned long agxMemBase; 
unsigned long agxFontCacheOffset;
unsigned long agxFontCacheSize;
unsigned long agxFontRowLength;
unsigned int  agxFontRowNumLines;

unsigned long agxScratchOffset;
unsigned long agxScratchSize;
unsigned long agxHWCursorOffset;
Bool     agxHWCursor = FALSE;


extern Bool xf86VTSema;
int agxMaxX, agxMaxY;
int agxVirtX, agxVirtY;
int agxAdjustedVirtX;
int agxDisplayWidth;
Bool  agx128WidthAdjust;
Bool  agx256WidthAdjust;
Bool  agx288WidthAdjust;
Bool agxUse4MbAperture;
Bool (*agxClockSelectFunc)();


Bool agxSaveVGA = TRUE;
Bool agxInited;

pointer      agxGEBase;
pointer      agxGEPhysBase;
unsigned int agxIdxReg;
unsigned int agxDAReg;
unsigned int agxApIdxReg;
unsigned int agxByteData;
unsigned int agxChipId;
unsigned int xf86RamDacBase;

int agxBpp;
int agxWeight = RGB8_PSEUDO; 
extern char *xf86VisualNames[];

agxCRTCRegRec agxCRTCRegs;

extern miPointerScreenFuncRec xf86PointerScreenFuncs;

LUTENTRY agxsavedLUT[256];

typedef struct XGAInfoBlock {
   unsigned char  instance;
   unsigned char  pos0;
   unsigned char  pos1;
   unsigned char  pos2;
   unsigned char  pos3;
   unsigned char  pos4;
   unsigned char  pos5;
   unsigned long  xgaBiosAddress; 
   unsigned long  xgaCoProcAddress;
   unsigned long  xgaIOAddress;
   unsigned long  xgaMemBase;
} XGAInformationBlock;  

static 
GetXGAInfoBlock(instance,info)
   int instance;
   XGAInformationBlock *info;
{
   int i, tmp;
   int  inst = instance;
   int posBase = agxInfoRec.POSbase;

   if (xf86Verbose) {
      ErrorF( "%s %s: ------------- XGA POS Register Probe -------------\n", 
              XCONFIG_PROBED, agxInfoRec.name );
  
      ErrorF( "%s %s: POS Probe Register Base Address: 0x0%x (POSBASE)\n", 
              OFLG_ISSET(XCONFIG_POSBASE, &agxInfoRec.xconfigFlag) 
                 ? XCONFIG_GIVEN : XCONFIG_PROBED,
              agxInfoRec.name, posBase );
   }

   if( posBase & POS_EISA_MASK == POS_EISA_MASK ) {
      ErrorF( "%s %s: EISA POS Probe\n", XCONFIG_PROBED, agxInfoRec.name );

      /* enable pos regs for video board instance */
      outb(posBase+POS_EISA_CONTROL, POS_CONF_EISA_ENABLE_MASK | inst);

      posBase += POS_EISA_OFFSET;
   }
   else {
      ErrorF( "%s %s: ISA POS  Probe\n", XCONFIG_PROBED, agxInfoRec.name );
      if( inst < 0 || inst > 7 ) {
         unsigned char in; 
         inst = -1;
         for( i=0; i<8; i++) {
            GlennsIODelay();
            in = inb(posBase+POS_CONTROL+i);
            if (in != 0xFF) 
               inst = i; 
            if (xf86Verbose) 
               ErrorF( "%s %s: POS instance %d - control register: 0x%x, \
value: 0x%02x\n",
                       XCONFIG_PROBED, agxInfoRec.name,
                       i, POS_CONTROL+i, in );
         }
      }
      else {
         if( inb(posBase+POS_CONTROL+inst) == 0xFF )
            inst = -1;
      }
      if( inst < 0 ) {
         return inst;
      }
      /* enable pos regs for video board instance */
      outb(posBase+POS_CONTROL+inst, POS_CONF_ISA_ENABLE_MASK | inst);
   }

   if( inst < 0 ) {
      return inst;
   }

   GlennsIODelay();
   info->pos0 = inb(posBase);
   GlennsIODelay();
   info->pos1 = inb(posBase+1);
   GlennsIODelay();
   info->pos2 = inb(posBase+2);
   GlennsIODelay();
   info->pos3 = inb(posBase+3);
   GlennsIODelay();
   info->pos4 = inb(posBase+4);
   GlennsIODelay();
   info->pos5 = inb(posBase+5);
   
   /* disable the POS registers */
   GlennsIODelay();
   if( posBase & POS_EISA_MASK == POS_EISA_MASK ) { 
      posBase -= POS_EISA_OFFSET;
      outb(posBase+POS_EISA_CONTROL, 0x0 );
   }
   else {
      outb(posBase+POS_CONTROL+inst, inst & POS_INSTANCE_MASK);
   }

   info->instance = POS_CONF_INSTANCE(info->pos2);

   info->xgaBiosAddress = ((info->pos2 & POS_CONF_EXT_MEM_MASK) 
                             * POS_CONF_EXT_MEM_MULT) + POS_CONF_EXT_MEM_BASE;

   info->xgaCoProcAddress = info->xgaBiosAddress 
                            + POS_CONF_COPROC_REG_OFF
                            + inst * 0x80;

   info->xgaIOAddress =  0x2100 | inst<<4;

   info->xgaMemBase  = (info->pos4 & 0xFE)<<24 | (inst&0x07)<<22 ;

   if (xf86Verbose) {
      ErrorF( "%s %s: POS XGA Instance: %d (INSTANCE)\n", 
              instance < 0 ? XCONFIG_PROBED : XCONFIG_GIVEN,
              agxInfoRec.name, info->instance);

      ErrorF( "%s %s: POS Register 0 - ID hi:   0x%02x\n", XCONFIG_PROBED,
              agxInfoRec.name, info->pos0 );
      ErrorF( "%s %s: POS Register 1 - ID low:  0x%02x\n", XCONFIG_PROBED,
              agxInfoRec.name, info->pos1 );
      ErrorF( "%s %s: POS Register 2 - Ext Mem: 0x%02x\n",
               XCONFIG_PROBED, agxInfoRec.name, info->pos2 );
      ErrorF( "%s %s: POS Register 3 - Bus Arb: 0x%02x\n", XCONFIG_PROBED,
              agxInfoRec.name, info->pos3 );
      ErrorF( "%s %s: POS Register 4 - Dis Adr: 0x%02x\n", XCONFIG_PROBED,
              agxInfoRec.name, info->pos4 );
      ErrorF( "%s %s: POS Register 5 - 1MB Adr: 0x%02x\n", XCONFIG_PROBED,
              agxInfoRec.name, info->pos5 );

      ErrorF( "%s %s: POS XGA External Memory (XGA BIOS) Address: \
0x%06x (BIOSBASE)\n", 
               XCONFIG_PROBED, agxInfoRec.name, info->xgaBiosAddress );

      ErrorF( "%s %s: POS XGA Coproc Mem-Mapped Reg \
Base Address: 0x%06x (COPBASE)\n", 
              XCONFIG_PROBED, agxInfoRec.name, info->xgaCoProcAddress );

      ErrorF( "%s %s: POS XGA I/O Register Base Address: \
0x%04x (IOBASE)\n", 
              XCONFIG_PROBED, agxInfoRec.name, info->xgaIOAddress );

      ErrorF( "%s %s: POS XGA Coproc's Video Memory Base \
Address: 0x%06x (MEMBASE)\n", 
              XCONFIG_PROBED, agxInfoRec.name, info->xgaMemBase );

      ErrorF( "%s %s: ------------- End POS Register Probe -------------\n", 
              XCONFIG_PROBED, agxInfoRec.name );
   }

   return inst;
}
   
/*
 * agxProbe --
 *     Probe the hardware
 */
Bool
agxProbe()
{
   int                   i;
   DisplayModePtr        pMode, pEnd;
   OFlagSet              validOptions;
   int                   tx, ty;
   agxCRTCRegRec         agxProbeCRTC;

   xf86ProbeFailed = FALSE;

   vgaBase = NULL;
   vgaVirtBase = NULL;
   vgaPhysBase = NULL;

   OFLG_ZERO(&validOptions);
   OFLG_SET(OPTION_CLKDIV2, &validOptions);
   OFLG_SET(OPTION_SW_CURSOR, &validOptions);
   OFLG_SET(OPTION_BT485_CURS, &validOptions);
   OFLG_SET(OPTION_BT482_CURS, &validOptions);
   OFLG_SET(OPTION_DAC_8_BIT, &validOptions);
   OFLG_SET(OPTION_DAC_6_BIT, &validOptions);
   OFLG_SET(OPTION_NOACCEL, &validOptions);
   OFLG_SET(OPTION_SYNC_ON_GREEN, &validOptions);
   OFLG_SET(OPTION_8_BIT_BUS, &validOptions);
   OFLG_SET(OPTION_WAIT_STATE, &validOptions);
   OFLG_SET(OPTION_NO_WAIT_STATE, &validOptions);
   OFLG_SET(OPTION_CRTC_DELAY, &validOptions);
   OFLG_SET(OPTION_VRAM_128, &validOptions);
   OFLG_SET(OPTION_VRAM_256, &validOptions);
   OFLG_SET(OPTION_REFRESH_20, &validOptions);
   OFLG_SET(OPTION_REFRESH_25, &validOptions);
   OFLG_SET(OPTION_SPRITE_REFRESH, &validOptions);
   OFLG_SET(OPTION_SCREEN_REFRESH, &validOptions);
   OFLG_SET(OPTION_FIFO_CONSERV, &validOptions);
   OFLG_SET(OPTION_FIFO_MODERATE, &validOptions);
   OFLG_SET(OPTION_FIFO_AGGRESSIVE, &validOptions);
   OFLG_SET(OPTION_VLB_A, &validOptions);
   OFLG_SET(OPTION_VLB_B, &validOptions);
   OFLG_SET(OPTION_FAST_VRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_VRAM, &validOptions);
   OFLG_SET(OPTION_FAST_DRAM, &validOptions);
   OFLG_SET(OPTION_SLOW_DRAM, &validOptions);
   OFLG_SET(OPTION_MED_DRAM, &validOptions);
   OFLG_SET(OPTION_VRAM_DELAY_LATCH, &validOptions);
   OFLG_SET(OPTION_VRAM_DELAY_RAS, &validOptions);
   OFLG_SET(OPTION_VRAM_EXTEND_RAS, &validOptions);
   OFLG_SET(OPTION_ENGINE_DELAY, &validOptions);

   xf86VerifyOptions(&validOptions, &agxInfoRec);

   if( !StrCaseCmp( agxInfoRec.chipset, "XGA-1" ) )
      agxChipId = XGA_1;
   else if( !StrCaseCmp( agxInfoRec.chipset, "XGA-2" ) )
      agxChipId = XGA_2;
   else if( !StrCaseCmp( agxInfoRec.chipset, "AGX-010" ) )
      agxChipId = AGX_10;
   else if( !StrCaseCmp( agxInfoRec.chipset, "AGX-014" ) )
      agxChipId = AGX_14;
   else if( !StrCaseCmp( agxInfoRec.chipset, "AGX-015" ) )
      agxChipId = AGX_15;
   else if( !StrCaseCmp( agxInfoRec.chipset, "AGX-016" ) )
      agxChipId = AGX_16;
   else {
      agxChipId = 0;
      agxInfoRec.chipset = "UNKNOWN";
      ErrorF("%s %s:Valid AGX/XGA Chip type must be specified.\n",
             XCONFIG_GIVEN, agxInfoRec.name);
      return FALSE;
   }
   ErrorF( "%s %s: Graphics chip type \"%s\" (CHIPSET)\n", 
           XCONFIG_GIVEN, agxInfoRec.name, agxInfoRec.chipset );

   xf86ClearIOPortList(agxInfoRec.scrnIndex); 

   /* probe and auto-config XGA chips */ 
   if( XGA_SERIES(agxChipId) && agxInfoRec.POSbase != 0 ) {
      int inst;
      XGAInformationBlock info = { 0xFF, };

      if( agxInfoRec.instance < 0 || agxInfoRec.instance > 7 )
         agxInfoRec.instance = -1;

      for( i= 0; i <= POS_LAST_IO_REG; i++ )
         agxPOSIOPorts[ i ] = agxInfoRec.POSbase + i;

      xf86AddIOPorts(agxInfoRec.scrnIndex, POS_NUM_IO_REG, agxPOSIOPorts);

      xf86EnableIOPorts(agxInfoRec.scrnIndex);

      inst = GetXGAInfoBlock(agxInfoRec.instance,&info);

      xf86DisableIOPorts(agxInfoRec.scrnIndex);
      xf86ClearIOPortList(agxInfoRec.scrnIndex); 

      if( inst < 0 || inst > 7 ) {
         ErrorF( "%s %s: Unable to probe the XGA POS registers, see README.agx \
for information on how to manually configure.\n", 
                 XCONFIG_GIVEN, agxInfoRec.name );
         if( !OFLG_ISSET(XCONFIG_INSTANCE, &agxInfoRec.xconfigFlag) )
            return FALSE;
      }
      else {
         if( !OFLG_ISSET(XCONFIG_INSTANCE, &agxInfoRec.xconfigFlag) ) {
            agxInfoRec.instance = info.instance;
         }
         if( !OFLG_ISSET(XCONFIG_MEMBASE, &agxInfoRec.xconfigFlag) ) {
            agxInfoRec.MemBase  = (info.pos4 & 0xFE)<<24
                                  |  (agxInfoRec.instance&0x07)<<22 ;
         }
         if( !OFLG_ISSET(XCONFIG_COPBASE, &agxInfoRec.xconfigFlag) ) {
            agxInfoRec.COPbase  = info.xgaCoProcAddress;
         }
         if( !OFLG_ISSET(XCONFIG_IOBASE, &agxInfoRec.xconfigFlag) ) {
            agxInfoRec.IObase   = info.xgaIOAddress;
         }
         if( !OFLG_ISSET(XCONFIG_BIOSBASE, &agxInfoRec.xconfigFlag) ) {
            agxInfoRec.BIOSbase   = info.xgaBiosAddress;
         }
      }
   }
   else {  /* AGX */
      if( !OFLG_ISSET(XCONFIG_INSTANCE, &agxInfoRec.xconfigFlag) ) {
         agxInfoRec.instance = 6;
      }
      if( !OFLG_ISSET(XCONFIG_MEMBASE, &agxInfoRec.xconfigFlag) ) {
         agxInfoRec.MemBase  = (agxInfoRec.instance&0x07)<<22 ;
      }
   }

   /* configure I/O address */
   if(agxInfoRec.IObase == 0)
      if(AGX_SERIES(agxChipId))
          agxInfoRec.IObase = DA_AGX_IO_BASE;
      else
          agxInfoRec.IObase = DA_XGA_IO_BASE + (agxInfoRec.instance << 4);

   ErrorF( "%s %s: Instance = %d (INSTANCE).\n",
            OFLG_ISSET(XCONFIG_INSTANCE, &agxInfoRec.xconfigFlag) 
               ? XCONFIG_GIVEN : XCONFIG_PROBED,
            agxInfoRec.name, agxInfoRec.instance, agxInfoRec.IObase );

   ErrorF( "%s %s: I/O Mapped Register Base Address = 0x%04x (IOBASE)\n",
            OFLG_ISSET(XCONFIG_IOBASE, &agxInfoRec.xconfigFlag) 
               ? XCONFIG_GIVEN : XCONFIG_PROBED,
            agxInfoRec.name, agxInfoRec.IObase );

   /* XGA direct access I/O regs */
   for( i= 0; i <= DA_LAST_IO_REG; i++ )
      agxDAIOPorts[ i ] = agxInfoRec.IObase + i;

   xf86AddIOPorts(agxInfoRec.scrnIndex, Num_VGA_IOPorts, VGA_IOPorts);
   xf86AddIOPorts(agxInfoRec.scrnIndex, DA_NUM_IO_REG, agxDAIOPorts);
   xf86EnableIOPorts(agxInfoRec.scrnIndex);


   /* various address globals */
   agxIdxReg   = agxInfoRec.IObase + DA_INDEX_REGISTER;
   agxApIdxReg = agxInfoRec.IObase + DA_APERATURE_INDEX;
   agxDAReg    = agxInfoRec.IObase;
   agxByteData = agxInfoRec.IObase + DA_DATA_BYTE_REG;
   vgaIOBase   = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
   vgaPhysBase    = (pointer)agxInfoRec.VGAbase;
   agxPhysVidMem  = vgaPhysBase;
   xf86RamDacBase = 0x3C0;

   /* determine the video memory aperature base address */
   switch( agxInfoRec.VGAbase ) {
      unsigned char oldmode;

      case 0xA0000:
      case 0xB0000:
         break;
      default:
         oldmode = inb(agxDAReg+DA_OP_MODE) & DA_OM_MASK;
         outb(agxDAReg+DA_OP_MODE,oldmode|DA_OM_DISP_VGA_XGA_EN);

         switch( inb(agxDAReg+DA_APERATURE_CNTL) & DA_AC_MASK ) {
            case DA_AC_64K_AT_B0000:
               agxInfoRec.VGAbase = 0xB0000;
               break;
            case DA_AC_64K_AT_A0000:
            default:
               agxInfoRec.VGAbase = 0xA0000;
               break;

         }
         outb(agxDAReg+DA_OP_MODE,oldmode);
   }
   ErrorF( "%s %s: Video Memory 64K Aperature Base Address = 0x%05x \
(VGABASE)\n",
            OFLG_ISSET(XCONFIG_VGABASE, &agxInfoRec.xconfigFlag) 
               ? XCONFIG_GIVEN : XCONFIG_PROBED,
            agxInfoRec.name, agxInfoRec.VGAbase );

   /* various address globals */
   agxIdxReg   = agxInfoRec.IObase + DA_INDEX_REGISTER;
   agxApIdxReg = agxInfoRec.IObase + DA_APERATURE_INDEX;
   agxDAReg    = agxInfoRec.IObase;
   agxByteData = agxInfoRec.IObase + DA_DATA_BYTE_REG;
   vgaIOBase   = (inb(0x3CC) & 0x01) ? 0x3D0 : 0x3B0;
   vgaPhysBase    = (pointer)agxInfoRec.VGAbase;
   agxPhysVidMem  = vgaPhysBase;
   xf86RamDacBase = 0x3C0;

   /* map the video memory aperature */
   vgaBase = xf86MapVidMem(agxInfoRec.scrnIndex, VGA_REGION,
                           (pointer)vgaPhysBase, vgaBankSize);
   xf86MapDisplay(agxInfoRec.scrnIndex, VGA_REGION); 
   agxVideoMem = vgaBase;

   if (xf86Verbose) {
      ErrorF("%s %s: Using 64 kb aperture @phys-0x%x, virt-0x%x\n",
             OFLG_ISSET(OPTION_NOLINEAR_MODE, &agxInfoRec.options)
		? XCONFIG_GIVEN : XCONFIG_PROBED,
             agxInfoRec.name, vgaPhysBase, vgaBase );
   }

   /* determine the coprocessor memory mapped register base */
   if(AGX_SERIES(agxChipId)) {
      switch( agxInfoRec.COPbase ) {
         unsigned char oldmode;

         case 0xB1F00:
         case 0xD1F00:
            break;
         default:
            /* enable XGA registers */
            oldmode = inb(agxDAReg+DA_OP_MODE) & DA_OM_MASK;
            outb(agxDAReg+DA_OP_MODE,oldmode|DA_OM_DISP_VGA_XGA_EN);

            outb(agxIdxReg, IR_M3_MODE_REG_3);
            if(inb(agxByteData)&IR_M3_B1F00_GE_ADDRESS)
               if( agxInfoRec.VGAbase != 0xB0000 )
                  agxInfoRec.COPbase = 0xB1F00;
               else
                  agxInfoRec.COPbase = 0xD1F00;
            else
                agxInfoRec.COPbase = 0xD1F00;

            /* disable XGA registers */
            outb(agxDAReg+DA_OP_MODE,oldmode);
      }
   }
   else if( !OFLG_ISSET(XCONFIG_COPBASE, &agxInfoRec.xconfigFlag) ) {
      if( OFLG_ISSET(XCONFIG_BIOSBASE, &agxInfoRec.xconfigFlag) ) {
            agxInfoRec.COPbase = agxInfoRec.BIOSbase
                                 + POS_CONF_COPROC_REG_OFF
                                 + agxInfoRec.instance * 0x80;
      }
      else if( agxInfoRec.COPbase == 0 ) {
            agxInfoRec.COPbase = GE_DEF_MEM_BASE;
            agxInfoRec.COPbase += agxInfoRec.instance << 7;
      }
   }
   ErrorF( "%s %s: Coprocessor Memory-Mapped Register Base Address = 0x%05x \
(COPBASE)\n",
            OFLG_ISSET(XCONFIG_COPBASE, &agxInfoRec.xconfigFlag) 
            || OFLG_ISSET(XCONFIG_BIOSBASE, &agxInfoRec.xconfigFlag)
               ? XCONFIG_GIVEN : XCONFIG_PROBED,
            agxInfoRec.name, agxInfoRec.COPbase );

   /* map the coprocessor memory-mapped registers */
   agxGEPhysBase = (pointer) agxInfoRec.COPbase;
   agxGEBase = xf86MapVidMem( agxInfoRec.scrnIndex, LINEAR_REGION,
                              (pointer)((unsigned int)agxGEPhysBase & 0xFFF000),
                               0x1000 );
   agxGEBase = (void *)((unsigned int)agxGEBase +
			((unsigned int)agxGEPhysBase & 0x0FFF));
   xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION); 

   /* determine coprocessor's video memory address base */
   agxMemBase = agxInfoRec.MemBase;
   if (xf86Verbose) {
      ErrorF( "%s %s: Coprocessor's Video Memory Base Address = 0x%06x \
(MEMBASE)\n",
            OFLG_ISSET(XCONFIG_MEMBASE, &agxInfoRec.xconfigFlag) 
               ? XCONFIG_GIVEN : XCONFIG_PROBED,
            agxInfoRec.name, agxMemBase );
   }

   /*
    * Set Max Clock Frequency, for the chipset
    */

   if (XGA_2_ONLY(agxChipId)) {
      /* has programmable clocks */ 
      agxClockSelectFunc = xgaNiClockSelect;  
      if( OFLG_ISSET(XCONFIG_DACSPEED, &agxInfoRec.xconfigFlag) )
         xf86MaxClock = min( MAX_XGA_NI_CLOCK, agxInfoRec.dacSpeeds[0] );
      else
         xf86MaxClock = MAX_XGA_NI_CLOCK_CONSERV;
      OFLG_SET(CLOCK_OPTION_PROGRAMABLE, &(agxInfoRec.clockOptions));
   }
   else {
      agxClockSelectFunc = agxClockSelect;
      if (AGX_16_ONLY(agxChipId))
         xf86MaxClock = MAX_AGX_16_CLOCK;
      else if (AGX_15_ONLY(agxChipId))
         xf86MaxClock = MAX_AGX_15_CLOCK;
      else if (AGX_14_ONLY(agxChipId))
         xf86MaxClock = MAX_AGX_14_CLOCK;
      else 
         xf86MaxClock = MAX_XGA_1_CLOCK;
   }
   agxInfoRec.maxClock = xf86MaxClock;

   /*
    * Handle the depth options
    */
   
   if( XGA_SERIES(agxChipId) && xf86bpp == 15 ) {
      xf86bpp = agxInfoRec.depth = 16;
   }

   if (xf86bpp < 0) {
      xf86bpp = agxInfoRec.depth;
   }
   if (xf86weight.red == 0 || xf86weight.green == 0 || xf86weight.blue == 0) {
      xf86weight = agxInfoRec.weight;
   }
   if (xf86Verbose)
      ErrorF("%s %s: Pixel Depth = %d\n", 
             XCONFIG_GIVEN, agxInfoRec.name, xf86bpp);
   switch (xf86bpp) {
   case 8:
      BytesPerPixelShift = 0;
      xf86RamDacBPP = 8;
      break;
   case 15:
      BytesPerPixelShift = 1;
      agxInfoRec.depth = 15;
      xf86bpp = 16;
      agxWeight = RGB16_555;
      xf86weight.red = xf86weight.green = xf86weight.blue = 5;
      xf86RamDacBPP = 15;
      agxInfoRec.bitsPerPixel = 16;
      if (agxInfoRec.defaultVisual < 0)
         agxInfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
         defaultColorVisualClass = agxInfoRec.defaultVisual;
      break;
   case 16:
      BytesPerPixelShift = 1;
      xf86weight.red = xf86weight.blue = 5;
      xf86weight.green = 6;
      agxWeight = RGB16_565;
      agxInfoRec.depth = 16;
      xf86RamDacBPP = 16;
      agxInfoRec.bitsPerPixel = 16;
      if (agxInfoRec.defaultVisual < 0)
         agxInfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
         defaultColorVisualClass = agxInfoRec.defaultVisual;
      break;
#ifdef AGX_32BPP
   case 24:
   case 32:
      BytesPerPixelShift = 2;
      xf86bpp = 32;
      agxInfoRec.depth = 24;
      agxInfoRec.bitsPerPixel = 32; /* Use sparse 24 bpp (RGBX) */
      xf86RamDacBPP = 32;
      agxWeight = RGB32_888;
      /* s3MaxClock = S3_MAX_32BPP_CLOCK; */
      xf86weight.red =  xf86weight.green = xf86weight.blue = 8;
      if (agxInfoRec.defaultVisual < 0)
         agxInfoRec.defaultVisual = TrueColor;
      if (defaultColorVisualClass < 0)
         defaultColorVisualClass = agxInfoRec.defaultVisual;
      break;
#endif
   default:
#ifdef AGX_32BPP
      ErrorF(
        "Invalid value for bpp.  Valid values are 8, 15, 16, 24 and 32.\n");
#else
      ErrorF(
        "Invalid value for bpp.  Valid values are 8, 15, and 16.\n");
#endif
      xf86DisableIOPorts(agxInfoRec.scrnIndex);
      return(FALSE);
   }

   if (agxInfoRec.bitsPerPixel > 8 &&
       defaultColorVisualClass >= 0 && defaultColorVisualClass != TrueColor) {
      ErrorF("Invalid default visual type: %d (%s)\n", defaultColorVisualClass,
             xf86VisualNames[defaultColorVisualClass]);
      xf86DisableIOPorts(agxInfoRec.scrnIndex);
      return(FALSE);
   }

   /*
    * Save the VGA state
    */
   agxSavedState = agxHWSave(agxSavedState, sizeof(agxSaveBlock));
   agxClearColor0();
   outb(agxIdxReg, 0);
   if(XGA_PALETTE_CONTROL(agxChipId)) {
      outb(agxIdxReg, IR_PAL_MASK);
      outb(agxByteData, 0x00);
   }
   else {
      outb(VGA_PAL_MASK, 0x00);
   }

   /* 
    * The Hercules specific routines are also generic for accessing
    * RAMDAC registers 0-7.
    */
   xf86OutRamDacReg  = hercOutRamDacReg;
   xf86OutRamDacData = hercOutRamDacData;
   xf86InRamDacReg   = hercInRamDacReg;

   /*
    * Handle RAMDAC Option flags.
    */
   

   if(AGX_SERIES(agxChipId)) {
      /* Default is 8-bit DAC -- unless default "NORMAL" RAMDAC (handled below) */ 
      if (OFLG_ISSET(OPTION_DAC_8_BIT, &agxInfoRec.options)) {
         xf86Dac8Bit = TRUE;
      }
      else if (OFLG_ISSET(OPTION_DAC_6_BIT, &agxInfoRec.options)) {
         xf86Dac8Bit = FALSE;
      }
      else {
         xf86Dac8Bit = TRUE;
      }
   }
   else
      xf86Dac8Bit = TRUE;


   if(AGX_SERIES(agxChipId)) {

      if (agxInfoRec.ramdac) {
         xf86RamDacType = xf86StringToToken(agxDacTable, agxInfoRec.ramdac);
         if (xf86RamDacType < 0 && AGX_SERIES(agxChipId)) {
            ErrorF("%s %s: Unknown RAMDAC type \"%s\"\n", XCONFIG_GIVEN,
                   agxInfoRec.name, agxInfoRec.ramdac);
            xf86RamDacType = NORMAL_DAC; 
         }
         else {
            ErrorF("%s %s: RAMDAC type: \"%s\"\n", XCONFIG_GIVEN,
                   agxInfoRec.name, agxInfoRec.ramdac);
         }
      }

      if( xf86RamDacType == HERC_DUAL_DAC
          || xf86RamDacType == HERC_SMALL_DAC ) {
         hercProbeRamDac();
      }

      if( xf86RamDacType == NORMAL_DAC ) {
         /* Default needs to be 6 bit */
         if (OFLG_ISSET(OPTION_DAC_8_BIT, &agxInfoRec.options)) {
            xf86Dac8Bit = TRUE;
         }
         else {
            xf86Dac8Bit = FALSE;
         }
      }

      xf86DacSyncOnGreen = FALSE;
      if ( DAC_IS_BT485_SERIES || DAC_IS_BT481_SERIES || DAC_IS_ATT490 ) {
         if (OFLG_ISSET(OPTION_SYNC_ON_GREEN, &agxInfoRec.options)) {
            xf86DacSyncOnGreen = TRUE;
            if (xf86Verbose)
            ErrorF("%s %s: Putting RAMDAC into sync-on-green mode\n",
                   XCONFIG_GIVEN, agxInfoRec.name);
         } 
      }
   }
   else {
      xf86RamDacType = XGA_DAC;
   }

   if( !hercBigDAC && !hercSmallDAC )
      xf86SetUpRamDac();

   agxInfoRec.maxClock = xf86MaxClock;

   /* do we need to probe? */
   if( !agxInfoRec.videoRam 
       || agxClockSelectFunc != xgaNiClockSelect
          && !agxInfoRec.clocks ) {
      agxClearColor0();
      outb(agxIdxReg, 0);
      if(XGA_PALETTE_CONTROL(agxChipId)) {
         outb(agxIdxReg, IR_PAL_MASK);
         outb(agxByteData, 0x00);
      }
      else {
         outb(VGA_PAL_MASK, 0x00);
      }
      if( agxClockSelectFunc == xgaNiClockSelect )  
         agxInfoRec.clock[0] = 25000;
      agxSetUpProbeCRTC( &agxProbeCRTC ); 
      agxSetCRTCRegs(&agxProbeCRTC);
      agxInitGE();
      if (!agxInfoRec.videoRam) {
         agxInfoRec.videoRam = agxGetMemSize();
         if (agxInfoRec.videoRam == 0 )
            ErrorF("%s %s: Video memory probe failed, specify the \
   memory size in your XF86Config file.\n",
                   XCONFIG_PROBED, agxInfoRec.name);
      }
   
      /*
       * Get the clocks
       */
      if (agxClockSelectFunc != xgaNiClockSelect) {
         if (!agxInfoRec.clocks) {
            agxProbeClocks(1);
         }
         else if( hercBigDAC && agxInfoRec.clocks > 16 ) {
            ErrorF("%s %s: WARNING!!! Clocks have changed since 3.1.1 \
(see README.agx)\n",
                   XCONFIG_GIVEN, agxInfoRec.name);
            return FALSE;
         }
         else if (agxInfoRec.clocks > 16) {
            agxInfoRec.clocks = 16;
         }
      }
   
      outb(agxIdxReg, 0);
      if(XGA_PALETTE_CONTROL(agxChipId)) {
         outb(agxIdxReg, IR_PAL_MASK);
         outb(agxByteData, 0xFF);
      }
      else {
         outb(VGA_PAL_MASK, 0xFF);
      }
   }
   
   agxHWRestore(agxSavedState);
   
   xf86DisableIOPorts(agxInfoRec.scrnIndex);

   /*
    * Adjust the clocks for depth
    */
   if( hercBigDAC ) {
      /* for 2MB Graphites */
      if( agxInfoRec.depth == 8 ) {
          hercAddDoubledClocks(&agxInfoRec);
      }
      else if( agxInfoRec.depth > 16 ) {
          int clk, max;
          max = agxInfoRec.clocks;
          for( clk=0; clk < max; clk++ )
             agxInfoRec.clock[clk] >>= 1;
      }
      hercValidateClocks(&agxInfoRec) ; 
   } 
   else {
      switch( agxInfoRec.depth ) {
         int clk, max;
         case 8:
            break;
   
         case 15:
         case 16:
            max = agxInfoRec.clocks;
            for( clk=0; clk < max; clk++ )
               agxInfoRec.clock[clk] >>= 1;
            break;
   
         case 24:
         case 32:
            max = agxInfoRec.clocks;
            for( clk=0; clk < max; clk++ )
               agxInfoRec.clock[clk] >>= 2;
            break;
      }
   }

   if (xf86Verbose) {
      int i;

      ErrorF("%s ",OFLG_ISSET(XCONFIG_CLOCKS,&agxInfoRec.xconfigFlag)
                      ? XCONFIG_GIVEN : XCONFIG_PROBED);
      ErrorF("%s: ", agxInfoRec.name);

      if(OFLG_ISSET(CLOCK_OPTION_PROGRAMABLE,&(agxInfoRec.clockOptions)))
         ErrorF("Dot-clocks at %d bpp: Programmable", agxInfoRec.depth);
      else
         ErrorF("Dot-clocks at %d bpp: %d", 
                 agxInfoRec.depth, agxInfoRec.clocks );

      for (i = 0; i < agxInfoRec.clocks; i++) {
         if (i % 8 == 0)
            ErrorF("\n%s %s:            ",
                   OFLG_ISSET(XCONFIG_CLOCKS,&agxInfoRec.xconfigFlag)
                   ? XCONFIG_GIVEN : XCONFIG_PROBED,
                   agxInfoRec.name);
            ErrorF(" %6.2f", agxInfoRec.clock[i]/1000.0);
      }
      ErrorF("\n");
   }


   tx = agxInfoRec.virtualX;
   ty = agxInfoRec.virtualY;
   pMode = pEnd = agxInfoRec.modes;
   if( pMode == NULL ) {
      ErrorF("No modes supplied in XF86Config\n");
      return FALSE;
   } 
   pEnd = NULL;
   do {
      DisplayModePtr pModeSv;

      pModeSv = pMode->next;
      /*
       * xf86LookupMode returns FALSE if it ran into an invalid
       * parameter 
       */
      if (!xf86LookupMode(pMode, &agxInfoRec, LOOKUP_DEFAULT)) {
         xf86DeleteMode(&agxInfoRec, pMode);
      }
      else {
         if (pMode->HDisplay > 2048) {
            ErrorF("%s %s: Width of mode \"%s\" is too large (max is %d)\n",
                   XCONFIG_PROBED, agxInfoRec.name, pMode->name, 2048);
            xf86DeleteMode(&agxInfoRec, pMode);
         }
         else if ( (pMode->HDisplay * (1 + pMode->VDisplay))
                    > agxInfoRec.videoRam * 1024) {
            ErrorF("%s %s: Too little memory for mode \"%s\"\n", XCONFIG_PROBED,
                   agxInfoRec.name, pMode->name);
            xf86DeleteMode(&agxInfoRec, pMode);
         }
         else if (((tx > 0) && (pMode->HDisplay > tx)) ||
                    ((ty > 0) && (pMode->VDisplay > ty))) {
            ErrorF("%s %s: Resolution %dx%d too large for virtual %dx%d\n",
                   XCONFIG_PROBED, agxInfoRec.name,
                   pMode->HDisplay, pMode->VDisplay, tx, ty);
            xf86DeleteMode(&agxInfoRec, pMode);
         }
         else {
            /*
             * Successfully looked up this mode.  If pEnd isn't
             * initialized, set it to this mode.
             */
            if (pEnd == (DisplayModePtr) NULL)
               pEnd = pMode;
   
            agxInfoRec.virtualX = max( agxInfoRec.virtualX, pMode->HDisplay );
            agxInfoRec.virtualY = max( agxInfoRec.virtualY, pMode->VDisplay );
         }
      }
      pMode = pModeSv;
   } while (pMode != pEnd);

   if ((tx != agxInfoRec.virtualX) || (ty != agxInfoRec.virtualY))
      OFLG_CLR(XCONFIG_VIRTUAL,&agxInfoRec.xconfigFlag);

   agxVirtX = agxInfoRec.virtualX;
   agxVirtY = agxInfoRec.virtualY;

   /*
    * AGX -014,-015,-016 and XGA display map width must be power of
    * 2, if the coprocessor is to be used, except that the -015
    * and later have a +256 adjust. The -016 has some additional
    * adjusts for 640 and 800. The XGA's (AGX-10) also appear to 
    * have the similiar limits as the -016
    */
   agx128WidthAdjust = FALSE;
   agx256WidthAdjust = FALSE;
   agx288WidthAdjust = FALSE;
   if (XGA_SERIES(agxChipId) && agxVirtX <= 320 ) {
      agxAdjustedVirtX = 320;
      agxDisplayWidth = 320;
   }
   else
   if (agxVirtX <= 512) {
      agxAdjustedVirtX = 512;
      agxDisplayWidth = 512;
   }
   else
   if (AGX_16_ONLY(agxChipId) && agxVirtX <= 640) {
      agx128WidthAdjust = TRUE;
      agxAdjustedVirtX = 512;
      agxDisplayWidth = 640;
   }
   else
   if (AGX_10_XGA_ONLY(agxChipId) && agxVirtX <= 640) {
      agxAdjustedVirtX = 640;
      agxDisplayWidth = 640;
   }
   else
   if (AGX_16_ONLY(agxChipId) && agxVirtX <= 800) {
      agx288WidthAdjust = TRUE;
      agxAdjustedVirtX = 512;
      agxDisplayWidth = 800;
   }
   else
   if (AGX_10_XGA_ONLY(agxChipId) && agxVirtX <= 800) {
      agxAdjustedVirtX = 800;
      agxDisplayWidth = 800;
   }
   else
   if (agxVirtX <= 1024) {
      agxAdjustedVirtX = 1024;
      agxDisplayWidth = 1024;
   }
   else
   if (AGX_10_XGA_ONLY(agxChipId) && agxVirtX <= 1152) {
      agxAdjustedVirtX = 1152;
      agxDisplayWidth = 1152;
   }
   else
   if (AGX_15_16_ONLY(agxChipId) && agxVirtX <= 1280) {
      agx256WidthAdjust = TRUE;
      agxAdjustedVirtX = 1024;
      agxDisplayWidth = 1280;
   }
   else
   if (AGX_10_XGA_ONLY(agxChipId) && agxVirtX <= 1280) {
      agxAdjustedVirtX = 1280;
      agxDisplayWidth = 1280;
   }
   else
   if (agxVirtX <= 2048) {
      agxAdjustedVirtX = 2048;
      agxDisplayWidth = 2048;
   }

   agxMaxX = agxVirtX - 1;
   agxMaxY = agxVirtY - 1;

   if (xf86Verbose) {
      ErrorF("%s %s: Virtual resolution = %dx%d, CRTC Line Width = %d\n",
	     OFLG_ISSET(XCONFIG_VIRTUAL, &agxInfoRec.xconfigFlag)
                ? XCONFIG_GIVEN : XCONFIG_PROBED,
	     agxInfoRec.name, agxVirtX, agxVirtY, agxDisplayWidth);
   }

    /* Set agxMemorySize to required MEM_SIZE value in MISC_OPTIONS */
   if (agxInfoRec.videoRam <= 512)
      agxMemorySize = MEM_SIZE_512K;
   else if (agxInfoRec.videoRam <= 1024)
      agxMemorySize = MEM_SIZE_1M;
   else if (agxInfoRec.videoRam <= 2048)
      agxMemorySize = MEM_SIZE_2M;
   else
      agxMemorySize = MEM_SIZE_4M;

   if (xf86Verbose) {
      ErrorF("%s %s: Videoram = %dk\n",
	     OFLG_ISSET(XCONFIG_VIDEORAM, &agxInfoRec.xconfigFlag)
	        ? XCONFIG_GIVEN : XCONFIG_PROBED, 
             agxInfoRec.name,
	     agxInfoRec.videoRam );
   }

   if ( ((xf86bpp * (agxDisplayWidth*agxVirtY)) >> 3) 
        > (agxInfoRec.videoRam<<10) ) {
      ErrorF(
         "%s %s: Not enough memory for requested CRTC resolution (%dx%d).\n",
         XCONFIG_PROBED, agxInfoRec.name,
         agxDisplayWidth, agxVirtY );
      return FALSE;
   }

   {
     unsigned long end = ((xf86bpp * (agxDisplayWidth*agxVirtY)) >> 3);
     unsigned long avail;
     unsigned long total = agxInfoRec.videoRam << 10;
 
     /* align to 64K -- 16K for the XGA */
     if( AGX_10_XGA_ONLY(agxChipId) )  
        agxScratchOffset = (end + 0x3FFF) & 0xFFFFC000;
     else
        agxScratchOffset = (end + 0xFFFF) & 0xFFFF0000;
     avail = total - agxScratchOffset;

    if( !OFLG_ISSET(OPTION_NOACCEL, &agxInfoRec.options) ) {  
        if( AGX_10_XGA_ONLY(agxChipId) 
            && avail >= 0x0C000 && avail < 0x10000 )
           agxScratchSize = 0x04000;    /* 32K Font Cache */
        else if( AGX_10_XGA_ONLY(agxChipId) && avail < 0x10000 )
           agxScratchSize = avail & 0x1C000;   /* No Font Cache */
        else if( AGX_10_XGA_ONLY(agxChipId) && avail < 0x20000 )
           agxScratchSize = 0x08000 + (avail & 0x04000);    
        else if( avail < 0x10000 )
           agxScratchSize = 0;
        else if( avail < 0x40000 )
           agxScratchSize = 0x10000 + (avail & 0x04000);
        else if( avail < 0x80000 )
           agxScratchSize = 0x20000 + (avail & 0x0C000); 
        else 
           agxScratchSize = 0x40000; 
    }
    else {
        agxScratchSize = avail; 
    }
    
    if( !OFLG_ISSET(OPTION_NOACCEL, &agxInfoRec.options) ) {  
        /* align to 64K -- 32K/64K for the XGA */
        agxFontCacheOffset = (agxScratchOffset + agxScratchSize) & 0XFF8000;
        agxFontCacheSize   = total - agxFontCacheOffset;
        if( AGX_10_XGA_ONLY(agxChipId) )  {
           if( agxFontCacheSize < 0x08000 ) {
              agxFontCacheSize   = 0;
              agxFontRowLength   = 2048; /* to prevent div-by-zero */
              agxFontRowNumLines = 0;
           }
           else if( agxFontCacheOffset & 0x08000 && agxFontCacheSize < 0x20000 ) {
              agxFontCacheSize   &= 0xFF8000;
              agxFontRowLength    = 0x008000;
              agxFontRowNumLines  = 1024;
           }
           else {
              agxFontCacheOffset &= 0xFF0000;
              agxFontCacheSize   &= 0xFF0000;
              agxFontRowLength    = 0x010000;
              agxFontRowNumLines  = 2048;
           }
        }
        else {
           agxFontCacheOffset &= 0xFF0000;
           agxFontCacheSize   &= 0xFF0000;
           agxFontRowLength    = 0x010000;
           agxFontRowNumLines  = 2048;
        }
     }
     else {
        agxFontCacheSize   = 0;
        agxFontRowLength   = 2048;
        agxFontRowNumLines = 0;
     }

     if( agxScratchSize < 0x10000 && !AGX_10_XGA_ONLY(agxChipId) ) {
         ErrorF("%s %s: 64K video memory required for scratchpad, \
0x%05x available\n",
             XCONFIG_PROBED, agxInfoRec.name, agxScratchSize );
         return FALSE; 
     }
     else if( agxScratchSize < 0x04000 && AGX_10_XGA_ONLY(agxChipId) ) {
         ErrorF("%s %s: 16K video memory required for scratchpad, \
0x%05x available\n",
             XCONFIG_PROBED, agxInfoRec.name, agxScratchSize );
         return FALSE; 
     }
     
     if (xf86Verbose) 
       ErrorF( "%s %s: ScratchPad = %dk @ offset 0x%x\n",
	       XCONFIG_PROBED, agxInfoRec.name,
               agxScratchSize>>10, agxScratchOffset );
   }

#ifdef XFreeXDGA
    agxInfoRec.physBase = (unsigned long) agxInfoRec.VGAbase;
    agxInfoRec.physBase = (unsigned long) agxPhysVidMem;
    agxInfoRec.physSize = 64 * 1024;
    agxInfoRec.displayWidth = agxDisplayWidth;
    agxInfoRec.directMode = XF86DGADirectPresent;
#endif


   return TRUE;
}


/*
 * agxPrintIdent --
 *     Print the indentification of the video card.
 */
void
agxPrintIdent()
{
    ErrorF("  %s: Accelerated server for AGX graphics adaptors ",
	   agxInfoRec.name);
    ErrorF("(Patchlevel %s)\n", agxInfoRec.patchLevel);
}

int
agxGetMemSize()
{
   char i, lastBank=-1;
   unsigned int mask;
   unsigned int tmp1, tmp2;
   volatile unsigned char *x1, *x2;

   x1 = vgaBase;
   x2 = x1 + 0xFFFC;
   xf86MapDisplay(agxInfoRec.scrnIndex, VGA_REGION); 
   for( i=0; i<0x40; i++) {
      outb( agxApIdxReg, i );
      usleep(10000);
      mask = (~i<<24) | 0x00A55A00 | i;
      MemToBus( (pointer)x1, &mask, 4);
      MemToBus( (pointer)x2, &mask, 4);
      x1 += 128;
      x2 -= 128;
   }
   x1 = vgaBase;
   x2 = x1 + 0xFFFC;
   for( i=0; i<0x40; i++ ) {
      outb( agxApIdxReg, i );
      usleep(10000);
      mask = (~i<<24) | 0x00A55A00 | i;
      BusToMem( &tmp1, (pointer)x1, 4);
      BusToMem( &tmp2, (pointer)x2, 4);
      if( tmp1 == mask && tmp2 == mask ) { 
         lastBank = i;
      }
      else {
         lastBank++;
         break;
      }
      x1 += 128;
      x2 -= 128;
   }
   outb( agxApIdxReg, 0 );
   return lastBank*0x40;
}


/*
 * agxInit --
 *      Attempt to find and initialize the Linear Framebuffer
 *      Most of the elements of the ScreenRec are filled in.  The
 *      video is enabled for the frame buffer...
 */

Bool
agxInit (scr_index, pScreen, argc, argv)
   int            scr_index;    /* The index of pScreen in the ScreenInfo */
   ScreenPtr      pScreen;      /* The Screen to initialize */
   int            argc;         /* The number of the Server's arguments. */
   char           **argv;       /* The arguments themselves. Don't change! */
{
   int displayResolution = 75;  /* default to 75dpi */
   extern int monitorResolution;

   agxImageInit();

   xf86EnableIOPorts(scr_index);

   if (vgaPhysBase) {
      xf86MapDisplay(scr_index, VGA_REGION); 
   }
   if (agxPhysVidMem != vgaPhysBase)  {
      xf86MapDisplay(scr_index, LINEAR_REGION);
   }
   xf86MapDisplay(scr_index, LINEAR_REGION);

   vgaVirtBase = (pointer)VGABASE;

   agxCalcCRTCRegs(&agxCRTCRegs, agxInfoRec.modes);

   agxInited = FALSE;
   agxInitDisplay(scr_index,&agxCRTCRegs);

   /* Clear the display.
    * Need to set the color, origin, and size.  Then draw.
    */
#ifndef DIRTY_STARTUP
   if(agxInfoRec.bitsPerPixel == 8 && xf86FlipPixels)         
      agxImageClear(1);
   else
      agxImageClear(0);
#endif
   agxBitCache8Init(agxDisplayWidth, agxVirtY);

   /*
    * Take display resolution from the -dpi flag if specified
    */

   if (monitorResolution)
      displayResolution = monitorResolution;

   if (!agxScreenInit(pScreen, 
		      (pointer) vgaVirtBase,
		      agxInfoRec.virtualX,
		      agxInfoRec.virtualY,
		      displayResolution,
		      displayResolution,
		      agxDisplayWidth))
      return(FALSE);

   pScreen->CloseScreen = agxCloseScreen;
   pScreen->SaveScreen = agxSaveScreen;

   switch (agxInfoRec.bitsPerPixel) {
        case 8:
            pScreen->InstallColormap = agxInstallColormap;
            pScreen->UninstallColormap = agxUninstallColormap;
            pScreen->ListInstalledColormaps = agxListInstalledColormaps;
            pScreen->StoreColors = agxStoreColors;
            break;
        case 16:
        case 32:
            pScreen->InstallColormap = cfbInstallColormap;
            pScreen->UninstallColormap = cfbUninstallColormap;
            pScreen->ListInstalledColormaps = cfbListInstalledColormaps;
            pScreen->StoreColors = (void (*)())NoopDDA;
        }

   if (TRUE | OFLG_ISSET(OPTION_SW_CURSOR, &agxInfoRec.options)) { 
      miDCInitialize (pScreen, &xf86PointerScreenFuncs);
   }
   else {
      pScreen->QueryBestSize = agxQueryBestSize;
      xf86PointerScreenFuncs.WarpCursor = agxWarpCursor;
      (void)agxCursorInit(0, pScreen);
   } 

   savepScreen = pScreen;

   /* enable DAC output */
   outb(agxIdxReg, 0);
   if(XGA_PALETTE_CONTROL(agxChipId)) {
      outb(agxIdxReg, IR_PAL_MASK);
      outb(agxByteData, 0xFF);
   }
   else {
      outb(VGA_PAL_MASK, 0xFF);
   }
   return cfbCreateDefColormap(pScreen);
}

/*
 * agxEnterLeaveVT -- 
 *      grab/ungrab the current VT completely.
 */

void
agxEnterLeaveVT(enter, screen_idx)
   Bool enter;
   int screen_idx;
{
   PixmapPtr pspix;
   ScreenPtr pScreen = savepScreen;
   unsigned  int  palDataReg;

#if 0
   ErrorF( "agxEnterLeaveVT() - Enter %x, screen_idx %x\n", enter, screen_idx );
   ErrorF( "agxEnterLeaveVT() - savepScreen %x\n", savepScreen );
#endif

   if ( !xf86Resetting && !xf86Exiting ) {
      switch (agxInfoRec.bitsPerPixel) {
         case 8:
            pspix = (PixmapPtr)pScreen->devPrivate;
            break;
         case 16:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb16ScreenPrivateIndex].ptr;
            break;
#ifdef AGX_32BPP
         case 32:
            pspix =
                  (PixmapPtr)pScreen->devPrivates[cfb32ScreenPrivateIndex].ptr;
            break;
#endif
      }
   }

   if (enter) {
#if 0
      ErrorF( "agxEnterLeaveVT() - ENTER \n");
#endif 
      xf86EnableIOPorts(screen_idx);
      if (vgaPhysBase)
	 xf86MapDisplay(screen_idx, VGA_REGION); 
#if 0 
      if (agxPhysVidMem != vgaPhysBase) 
	 xf86MapDisplay(screen_idx, LINEAR_REGION);
#endif

      xf86MapDisplay(screen_idx, LINEAR_REGION);

      if (!xf86Resetting) {
	 ScrnInfoPtr pScr = XF86SCRNINFO(pScreen);
#if 0
         ErrorF( "agxEnterLeaveVT() - ENTER  - !xf86Resetting\n");
#endif

         outb(agxIdxReg, 0);
         if( agxInfoRec.bitsPerPixel == 8 ) {
            /* make sure screen is blanked during setup */
            if(XGA_PALETTE_CONTROL(agxChipId)) {
               outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
               outb(agxByteData, 0x00);
               outb(agxIdxReg, IR_PAL_DATA);
               palDataReg = agxByteData;
            }
            else {
               outb(agxIdxReg, 0);  /* make sure index is not 0x51 */
               outb(VGA_PAL_WRITE_INDEX, 0x00);
               palDataReg = VGA_PAL_DATA;
            }
            outb(palDataReg, 0);
            outb(palDataReg, 0);
            outb(palDataReg, 0);
         }
         if(XGA_PALETTE_CONTROL(agxChipId)) {
            outb(agxIdxReg, IR_PAL_MASK);
            outb(agxByteData, 0x00);
         }
         else {
            outb(VGA_PAL_MASK, 0x00);
         }

         agxCalcCRTCRegs(&agxCRTCRegs, agxInfoRec.modes);
         agxInited = FALSE;
         agxInitDisplay(screen_idx,&agxCRTCRegs);

         /* Clear the display.
          * Need to set the color, origin, and size.  Then draw.
          */
#ifndef DIRTY_STARTUP
         if( agxInfoRec.bitsPerPixel == 8 )
            agxImageClear(1);
         else
            agxImageClear(0);
#endif
  	 agxBitCache8Init(agxDisplayWidth, agxVirtY);
  	 agxRestoreCursor(pScreen);
  	 agxAdjustFrame(pScr->frameX0, pScr->frameY0);
#if 0
         ErrorF( "agxEnterLeaveVT() - ENTER - pspix->devPrivate.ptr %x\n", 
                 pspix->devPrivate.ptr );
         ErrorF( "agxEnterLeaveVT() - ENTER - vgaVirtBase %x\n", 
                 vgaVirtBase );
         ErrorF( "agxEnterLeaveVT() - ENTER - ppix %x\n", ppix);
#endif
	 if ( (pointer)pspix->devPrivate.ptr != (pointer)vgaVirtBase && ppix ) {
	    pspix->devPrivate.ptr = vgaVirtBase;
            GE_WAIT_IDLE();
#if 0
            ErrorF( "agxEnterLeaveVT() - DisplayWidth %d, pScreen->width %d, depth %d\n",
                    agxDisplayWidth, pScreen->width, pScreen->rootDepth);
            ErrorF( "agxEnterLeaveVT() - ENTER - agxImageWriteFunc %x\n", ppix);
#endif
            (*agxImageWriteFunc)( 0, 0, 
                                  pScreen->width, pScreen->height,
		 	          ppix->devPrivate.ptr,
				  PixmapBytePad( agxDisplayWidth,
					         pScreen->rootDepth ),
				  0, 0, MIX_SRC, ~0 );
	 }
	 if (LUTissaved) {
	    agxRestoreLUT(agxsavedLUT);
	    LUTissaved = FALSE;
	    /*agxRestoreColor0(pScreen);*/
	 }

         outb(agxIdxReg, 0);
         if(XGA_PALETTE_CONTROL(agxChipId)) {
            outb(agxIdxReg, IR_PAL_MASK);
            outb(agxByteData, 0xFF);
         }
         else {
            outb(VGA_PAL_MASK, 0xFF);
         }
      }
      if (ppix) {
	 (pScreen->DestroyPixmap)(ppix);
	 ppix = NULL;
      }
   } 
   else {
#if 0
      ErrorF( "agxEnterLeaveVT() - EXIT \n");
#endif
      xf86EnableIOPorts(screen_idx);
      if (vgaPhysBase)
	 xf86MapDisplay(screen_idx, VGA_REGION);
#if 0
      if (agxPhysVidMem != vgaPhysBase) 
	 xf86MapDisplay(screen_idx, LINEAR_REGION);
#endif

      xf86MapDisplay(screen_idx, LINEAR_REGION);

      if (!xf86Exiting) {

         /*
          * Create a dummy pixmap to write to while VT is switched out.
          * Copy the screen to that pixmap
          */
#if 0
         ErrorF( "agxEnterLeaveVT() - EXIT  - !xf86Exiting\n");
#endif

	 ppix = (pScreen->CreatePixmap)(pScreen,
					agxDisplayWidth, pScreen->height,
					pScreen->rootDepth);
#if 0
         ErrorF( "agxEnterLeaveVT() - DisplayWidth %d, pScreen->width %d, depth %d\n",
                  agxDisplayWidth, pScreen->width, pScreen->rootDepth);
         ErrorF( "agxEnterLeaveVT() - EXIT  - ppix %x\n", ppix);
         ErrorF( "agxEnterLeaveVT() - EXIT  - pspix %x\n", pspix);
#endif
	 if (ppix) {
#if 0
            ErrorF( "agxEnterLeaveVT() - EXIT  - agxImageReadFunc %x\n", ppix);
#endif
            GE_WAIT_IDLE();
   	    (agxImageReadFunc)( 0, 0, 
				pScreen->width, pScreen->height,
				ppix->devPrivate.ptr,
				PixmapBytePad( agxDisplayWidth,
					       pScreen->rootDepth ),
				0, 0, ~0 );
            pspix->devPrivate.ptr = ppix->devPrivate.ptr;
	 }
      }
      
#if 0
   ErrorF( "agxEnterLeaveVT() - LUT\n");
#endif
#if 0
      agxCursorOff();
#endif
      agxSaveLUT(agxsavedLUT);
      LUTissaved = TRUE;

      outb(agxIdxReg, 0);  /* make sure index is not 0x51 */
      if( agxInfoRec.bitsPerPixel == 8
#ifdef XFreeXDGA
           && (!(agxInfoRec.directMode & XF86DGADirectGraphics))
#endif
      ) {
         /* make sure screen is blanked during exit */
         if(XGA_PALETTE_CONTROL(agxChipId)) {
            outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
            outb(agxByteData, 0x00);
            outb(agxIdxReg, IR_PAL_DATA);
            palDataReg = agxByteData;
         }
         else {
            outb(VGA_PAL_WRITE_INDEX, 0x00);
            palDataReg = VGA_PAL_DATA;
         }
         outb(palDataReg, 0);
         outb(palDataReg, 0);
         outb(palDataReg, 0);
         if(XGA_PALETTE_CONTROL(agxChipId)) {
            outb(agxIdxReg, IR_PAL_MASK);
            outb(agxByteData, 0x00);
         }
         else {
            outb(VGA_PAL_MASK, 0x00);
         }
      }

      if (!xf86Resetting) {

#ifdef XFreeXDGA
          if (!(agxInfoRec.directMode & XF86DGADirectGraphics)) {
#endif
             if( agxInfoRec.bitsPerPixel == 8 ) {
                /* make sure screen is blanked during exit */
                if(XGA_PALETTE_CONTROL(agxChipId)) {
                   outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
                   outb(agxByteData, 0x00);
                   outb(agxIdxReg, IR_PAL_DATA);
                   palDataReg = agxByteData;
                }
                else {
                   outb(VGA_PAL_WRITE_INDEX, 0x00);
                   palDataReg = VGA_PAL_DATA;
                }
                outb(palDataReg, 0);
                outb(palDataReg, 0);
                outb(palDataReg, 0);
                if(XGA_PALETTE_CONTROL(agxChipId)) {
                    outb(agxIdxReg, IR_PAL_MASK);
                    outb(agxByteData, 0x00);
                }
                else {
                    outb(VGA_PAL_MASK, 0x00);
                }
             }

             agxCleanUp();
#ifdef XFreeXDGA
          } 
#endif
      }

      if (vgaPhysBase)
         xf86UnMapDisplay(screen_idx, VGA_REGION);
#if 0
      if (agxPhysVidMem != vgaPhysBase)
         xf86UnMapDisplay(screen_idx, LINEAR_REGION);
#endif

      xf86UnMapDisplay(screen_idx, LINEAR_REGION);
   }
#if 0
   ErrorF( "agxEnterLeaveVT() - exiting\n");
#endif
}

/*
 * agxCloseScreen --
 *      called to ensure video is enabled when server exits.
 */

Bool
agxCloseScreen(screen_idx, pScreen)
   int screen_idx;
   ScreenPtr pScreen;
{
   /*
    * Hmm... The server may shut down even if it is not running on the
    * current vt. Let's catch this case here.
    */
   xf86Exiting = TRUE;
   if (xf86VTSema)
      agxEnterLeaveVT(LEAVE, screen_idx);
   else if (ppix) {
      /* 7-Jan-94 CEG: The server is not running on the current vt.
       * Free the screen snapshot taken when the server vt was left.
       */
      (savepScreen->DestroyPixmap)(ppix);
      ppix = NULL;
   }
#if 0 
   savepScreen = NULL;
   agxClearSavedCursor(screen_idx);
#endif
   return(TRUE);
}

/*
 * agxSaveScreen --
 *      blank the screen.
 */

Bool
agxSaveScreen (pScreen, on)
   ScreenPtr     pScreen;
   Bool          on;
{
   unsigned char oldIndex;
   unsigned int  palDataReg;
   static PixmapPtr savePix = NULL;

   if (on)
      SetTimeSinceLastInputEvent();

   if (xf86VTSema || savePix) {
      xf86EnableIOPorts(agxInfoRec.scrnIndex);
      if (vgaPhysBase)
         xf86MapDisplay(agxInfoRec.scrnIndex, VGA_REGION);
      if (agxPhysVidMem != vgaPhysBase) 
         xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION);

      xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION);

      GE_WAIT_IDLE();

      oldIndex = inb(agxIdxReg);
      if( agxInfoRec.bitsPerPixel == 8 ) {
         if (on) {
            agxBitCache8Init(agxDisplayWidth, agxVirtY);

            agxRestoreColor0(pScreen);
            if(XGA_PALETTE_CONTROL(agxChipId)) {
               outb(agxIdxReg, IR_PAL_MASK);
               outb(agxByteData, 0xff);
            }
            else {
               outb(VGA_PAL_MASK, 0xff);
            }
         }
         else {
            agxClearColor0();
            if(XGA_PALETTE_CONTROL(agxChipId)) {
               outb(agxIdxReg, IR_PAL_MASK);
               outb(agxByteData, 0x00);
            }
            else {
               outb(VGA_PAL_MASK, 0x00);
            }
         }
      }
      else {
         PixmapPtr pspix;
         ScreenPtr pScreen = savepScreen;

         switch (agxInfoRec.bitsPerPixel) {
            case 8:
               pspix = (PixmapPtr)pScreen->devPrivate;
               break;
            case 16:
               pspix = (PixmapPtr)pScreen->
                           devPrivates[cfb16ScreenPrivateIndex].ptr;
               break;
#ifdef AGX_32BPP
            case 32:
               pspix = (PixmapPtr)pScreen->
                          devPrivates[cfb32ScreenPrivateIndex].ptr;
            break;
#endif
         }

         if (on) {
            ScrnInfoPtr pScr = XF86SCRNINFO(pScreen);

            agxBitCache8Init(agxDisplayWidth, agxVirtY);
            agxRestoreCursor(pScreen);
            agxAdjustFrame(pScr->frameX0, pScr->frameY0);

            if( (pointer)pspix->devPrivate.ptr != (pointer)vgaVirtBase
                && savePix != NULL
              ) {

               (*agxImageWriteFunc)( 0, 0, 
                                     pScreen->width, pScreen->height,
                                     savePix->devPrivate.ptr,
                                     PixmapBytePad( agxDisplayWidth,
                                                    pScreen->rootDepth ),
                                     0, 0, MIX_SRC, ~0 );
               (pScreen->DestroyPixmap)(savePix);
               savePix = NULL;
               xf86VTSema = TRUE;
               pspix->devPrivate.ptr = vgaVirtBase;
            }
         }
         else {
            if( savePix == NULL ) {
               savePix = (pScreen->CreatePixmap)( pScreen,
                                                  agxDisplayWidth,
                                                  pScreen->height,
                                                  pScreen->rootDepth );


               (agxImageReadFunc)( 0, 0,
                                   pScreen->width, pScreen->height,
                                   savePix->devPrivate.ptr,
                                   PixmapBytePad( agxDisplayWidth,
                                                  pScreen->rootDepth ),
                                   0, 0, ~0 );
               agxImageClear(0);

               pspix->devPrivate.ptr = savePix->devPrivate.ptr;
               xf86VTSema = FALSE;  
            }
         }
      }
      outb(agxIdxReg, oldIndex); 
   }
   return(TRUE);
}

/*
 * agxAdjustFrame --
 *      Modify the CRT_OFFSET for panning the display.
 */
void
agxAdjustFrame(x, y)
   int x, y;
{
   unsigned int byte_offset;
   /*
    * XGA and AGX are documented as being offset in units of 8.
    * But, the AGX is actually offset in units of 4.
    */
   if( AGX_SERIES(agxChipId) ) {
      byte_offset = AGX_PIXEL_ADJUST( x + y*agxDisplayWidth ) >> 2; 
   }
   else {
      byte_offset = AGX_PIXEL_ADJUST(x + y*agxDisplayWidth ) >> 3;
   }
                    
   xf86EnableIOPorts(agxInfoRec.scrnIndex);
   if (vgaPhysBase)
      xf86MapDisplay(agxInfoRec.scrnIndex, VGA_REGION); 
   if (agxPhysVidMem != vgaPhysBase) 
      xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION);
   xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION);

   /*agxCursorOff();*/ 
   outb(agxIdxReg, IR_DISP_MAP_LO);
   outb(agxByteData, byte_offset & 0xff);
   byte_offset >>= 8;
   outb(agxIdxReg, IR_DISP_MAP_MID);
   outb(agxByteData, byte_offset & 0xff);
   byte_offset >>= 8;
   outb(agxIdxReg, IR_DISP_MAP_HI);
   outb(agxByteData, byte_offset & 0x0f);
   /*agxRepositionCursor(savepScreen); */

#if 0 /* Is the following correct? */
#ifdef XFreeXDGA
   if (agxInfoRec.directMode & XF86DGADirectGraphics) {
      /* Wait until vertical retrace is in progress. */
      while (inb(vgaIOBase + 0xA) & 0x08);
      while (!(inb(vgaIOBase + 0xA) & 0x08));
   }
#endif
#endif
}

/*
 * agxValidMode --
 *
 */
static int
agxValidMode(mode, verbose, flag)
DisplayModePtr mode;
Bool verbose;
int flag;
{
return MODE_OK;
}
