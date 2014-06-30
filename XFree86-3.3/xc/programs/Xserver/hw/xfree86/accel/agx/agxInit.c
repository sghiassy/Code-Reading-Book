/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxInit.c,v 3.28 1996/12/23 06:32:52 dawes Exp $ */
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
/* $XConsortium: agxInit.c /main/12 1996/10/19 17:50:15 kaleb $ */


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
#include "agx.h"
#include "regagx.h"
#include "xf86RamDac.h"
#include "hercRamDac.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#if defined(CSRG_BASED) || defined(MACH386)
#include <sys/resource.h>
#endif

extern Bool xf86Resetting, xf86Exiting, xf86ProbeFailed;

extern ScrnInfoRec agxInfoRec;

/*
 *  Saved VGA DATA 
 */
static LUTENTRY oldlut[256];
static Bool LUTInited = FALSE;
agxSaveBlock *agxSavedState = NULL; 

extern int   vgaIOBase; /* These defaults are overriden in s3Probe() */
extern int   vgaCRIndex;
extern int   vgaCRReg;

Bool agxInited = FALSE;

/*
 *  agxHWSave( saveState, stateSize )
 *  and agxHWRestore( saveState ) 
 */

agxSaveReg agxSaveDA[MAX_AGX_DA_REGS] = {
 { AGX_ALL|XGA_1	, DA_OP_MODE,		DA_OM_MASK,		0 },
 { XGA_2		, DA_OP_MODE,		DA_OM_NI_MASK,		0 },
 { AGX_ALL|XGA_ALL	, DA_APERATURE_CNTL,	DA_AC_MASK,		0 },
 { XGA_ALL		, DA_INTR_CNTL,		DA_IC_MASK,		0 },
 { XGA_ALL		, DA_INTR_STAT, 	DA_IS_MASK, DA_IS_CLEAR_ALL },
 { XGA_ALL		, DA_VIRT_MEM_CNTL,     DA_VM_MASK,		0 },
 { XGA_ALL	        , DA_VIRT_MEM_STAT,	DA_VS_MASK, DA_VS_CLEAR_ALL },
 { AGX_ALL|XGA_ALL	, DA_APERATURE_INDEX,	DA_AI_MASK,		0 },
 { AGX_ALL|XGA_ALL	, DA_MEM_ACCESS,        DA_MA_MASK,		0 },
 { AGX_ALL|XGA_ALL	, DA_INDEX_REGISTER,    0xFF,			0 },
 { 0, 0, 0, 0 }
};


agxSaveReg  agxSaveIdx[MAX_AGX_IDX_REGS] = {
#if 0
 { AGX_ALL|XGA_ALL, 	IR_MC0_MEM_CONF_0,		IR_MC0_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_MC1_MEM_CONF_1,		IR_MC1_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_MC2_MEM_CONF_2,		IR_MC2_MASK,	0 },
#endif

 { AGX_ALL|XGA_ALL, 	IR_CRTC_HTOTAL_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HTOTAL_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HDISP_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HDISP_END_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_START_LO,	0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_START_HI,	0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_END_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_START_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_START_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_END_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_POS1,		IR_CHP1_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_POS2,		IR_CHP2_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HTOTAL_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HTOTAL_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HDISP_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HDISP_END_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_START_LO,	0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_START_HI,	0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HBLANK_END_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_START_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_START_HI,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_HSYNC_END_LO,		0xFF,		0 },

 { AGX_ALL|XGA_ALL, 	IR_CRTC_VTOTAL_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VTOTAL_HI,		0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VDISP_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VDISP_END_HI,		0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VBLANK_START_LO,	0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VBLANK_START_HI,	0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VBLANK_END_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VBLANK_END_HI,		0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VSYNC_START_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VSYNC_START_HI,		0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VSYNC_END,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VLINE_COMP_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CRTC_VLINE_COMP_HI,		0x07,		0 },

 { AGX_ALL|XGA_ALL, 	IR_DISP_MAP_LO,			0xFF,	      	0 },
 { AGX_ALL|XGA_ALL, 	IR_DISP_MAP_MID,		0xFF,	      	0 },
 { AGX_ALL|XGA_ALL,	IR_DISP_MAP_HI,			0x07,	      	0 },
 { AGX_ALL|XGA_ALL, 	IR_DISP_MAP_WIDTH_LO,		0xFF,	      	0 },
 { AGX_ALL|XGA_ALL, 	IR_DISP_MAP_WIDTH_HI,		0x07,	      	0 },

 { AGX_NEW,	 	IR_M1_MODE_REG_1,		IR_M1_MASK,	0 },
 { AGX_10,	 	IR_M1_AGX10_MODE_REG_1,		IR_M1_MASK,	0 },
 { AGX_NEW,	 	IR_M2_MODE_REG_2,		IR_M2_MASK,	0 },
 { AGX_NEW,	 	IR_M3_MODE_REG_3,		IR_M3_MASK,	0 },
 { AGX_NEW,	 	IR_M4_MODE_REG_4,		IR_M4_MASK,	0 },
 { AGX_NEW,	 	IR_M5_MODE_REG_5,		IR_M5_MASK,	0 },
 { AGX_NEW,	 	IR_M7_MODE_REG_7,		IR_M7_MASK,	0x20 },
 { AGX_16,	 	IR_M8_MODE_REG_8,		0xFF,          	0 },
 { AGX_16,	 	IR_M10_MODE_REG_10,		IR_M10_MASK,   	0 },

 { XGA_2, 		IR_NI_PLL_PRG_REG,		0xFF,		0 },
 { XGA_2, 		IR_NI_DIR_CNTL,			0x07,		0 },
 { XGA_2, 		IR_NI_MISC_CNTL,		0x01,		0 },
 { XGA_2, 		IR_NI_MFI_CNTL,			0xFF,		0 },

 { AGX_ALL|XGA_ALL, 	IR_CUR_HPOS_LO,			0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_HPOS_HI,			0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_HHOT_SPOT,		IR_CHHS_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_VPOS_LO,			0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_VPOS_HI,			0x07,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_VHOT_SPOT,		IR_CVHS_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_CNTL,			IR_CUR_ENABLE,	0 },
 { XGA_ALL, 		IR_CUR_COLOR0_RED,		0xFF,		0 },
 { XGA_ALL,		IR_CUR_COLOR0_GREEN,		0xFF,		0 },
 { XGA_ALL,  		IR_CUR_COLOR0_BLUE,		0xFF,		0 },
 { XGA_ALL, 		IR_CUR_COLOR1_RED,		0xFF,		0 },
 { XGA_ALL, 		IR_CUR_COLOR1_GREEN,		0xFF,		0 },
 { XGA_ALL, 		IR_CUR_COLOR1_BLUE,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_PAL_INDEX_LO,		0xFF,		0 },
 { AGX_ALL|XGA_ALL, 	IR_CUR_INDEX_HI,		0x3F,		0 },
 { XGA_ALL, 		IR_CUR_PAL_INDEX_PREF_LO,	0xFF,		0 },
 { XGA_ALL, 		IR_CUR_INDEX_PREF_HI,		0x3F,		0 },

 { XGA_ALL, 		IR_BORDER_CLR,			0xFF,		0 },
 { XGA_ALL, 		IR_PAL_MASK,			0xFF,		0 },
 { XGA_ALL, 		IR_PAL_SEQUENCE,		IR_PS_MASK,	0 },

 { AGX_ALL|XGA_1, 	IR_CLOCK_SEL_1,			IR_CS1_MASK,   	0 },
 { XGA_2,	 	IR_CLOCK_SEL_1,			IR_CS1_NI_MASK,	0 },
 { AGX_ALL|XGA_ALL, 	IR_CLOCK_SEL_2,			IR_CS2_MASK,   	0 },
 { AGX_ALL|XGA_ALL, 	IR_DISP_CNTL_2,			0xF7,	      	0 },
 { AGX_ALL|XGA_ALL, 	IR_DISP_CNTL_1,			0xFB,	      0x4 },

 { 0, 0, 0, 0 }
};

void
vgarestore( save )
   agxSaveBlock *save;
{
   vgaProtect(TRUE);
   vgaHWRestore((vgaHWPtr)save);
   vgaProtect(FALSE);
}

agxSaveBlock *
agxHWSave( save, size )
   agxSaveBlock *save;
   int          size;    
{
   agxSaveReg *ptr;
   unsigned char newmode;
   unsigned char oldmode;
   Bool          saveRamDac = FALSE;

   if( hercBigDAC ) {
      hercSwitchToLittleDac();
   }

   /* save VGA state */
   if (save == NULL) {
      save = vgaHWSave( (vgaHWPtr)save, size );
      saveRamDac = TRUE;
   }
   vgarestore( save );

   /* enable XGA registers */
   oldmode = inb(agxDAReg+DA_OP_MODE) & DA_OM_MASK;
   switch( oldmode ) {
      case DA_OM_DISP_VGA_XGA_DIS : 
      case DA_OM_DISP_VGA_XGA_EN  : 
         newmode = DA_OM_DISP_VGA_XGA_EN;
         break;
      case DA_OM_DISP_132_XGA_DIS :
      case DA_OM_DISP_132_XGA_EN  :
         newmode = DA_OM_DISP_132_XGA_EN;
         break;
      default :
         newmode = DA_OM_DISP_XGA_MODE;
   }
   outb(agxDAReg+DA_OP_MODE,newmode);
   outb(agxIdxReg,0);

   if( save == NULL ) {
      save = (agxSaveBlock*)xalloc(size);
      memset( save, 0x00, size );
   }

   if( saveRamDac )
      (*xf86RamDacHWSave)(&save->agxRamDacSave);

   save->restoreOpMode = newmode;
   save->opMode = oldmode;
 
   for( ptr = agxSaveDA; ptr->chip; ptr++ ) 
      if( ptr->chip & agxChipId )
         save->agxDA.reg[ptr->reg] = inb(agxDAReg+ptr->reg) & ptr->mask;
   
   for( ptr = agxSaveIdx; ptr->chip; ptr++ ) 
      if( ptr->chip & agxChipId ) {
         outb(agxIdxReg,ptr->reg);
         save->agxIdx.reg[ptr->reg] = inb(agxByteData) & ptr->mask;
      }

   return save;
}

void   
agxHWRestore( save )
   agxSaveBlock *save;
{
   agxSaveReg *ptr;
   unsigned char newmode;
  
   agxResetCRTC(CRTC_PRERESET);
   agxResetCRTC(CRTC_RESET);

   for( ptr = agxSaveIdx; ptr->chip; ptr++ )
      if( ptr->chip & agxChipId ) {
         outb(agxIdxReg,ptr->reg);
         outb(agxByteData, save->agxIdx.reg[ptr->reg] | ptr->set);
      }

   for( ptr = agxSaveDA; ptr->chip; ptr++ )
      if( ptr->chip & agxChipId )
         outb(agxDAReg+ptr->reg, save->agxDA.reg[ptr->reg] | ptr->set);

   if( hercBigDAC ) {
      hercSwitchToLittleDac();
   }

   (*xf86RamDacHWRestore)(&save->agxRamDacSave);

   outb(agxIdxReg, 0);
   outb(agxDAReg+DA_OP_MODE,save->opMode);

   vgarestore(save);
}

/*
 *  agxInitDisplay(scrIndx)
 *
 *  Initializes the display 
 */
void 
agxInitDisplay(screen_idx, crtcRegs)
   int screen_idx;
   agxCRTCRegPtr crtcRegs;
{
   unsigned char byteData;
   unsigned char oldPalMask;

   if (agxInited)
      return;

   xf86EnableIOPorts(agxInfoRec.scrnIndex);
   xf86MapDisplay(screen_idx, VGA_REGION);

   if ((inb(agxDAReg+DA_OP_MODE) & DA_OM_DISP_XGA_MODE) != DA_OM_DISP_XGA_MODE )
      agxSavedState = agxHWSave( agxSavedState, sizeof(agxSaveBlock) );

   agxClearColor0();
   if(XGA_PALETTE_CONTROL(agxChipId)) {
      outb(agxIdxReg, IR_PAL_MASK);
      outb(agxByteData, 0x00);
   }
   else {
      outb(VGA_PAL_MASK, 0x00);
   }

   /*
    * set the CRTC registers
    */
   agxSetCRTCRegs(crtcRegs);
   
   agxInitGE();

   outb(agxIdxReg, 0);
   agxInited = TRUE;
}


/*     
 *   agxCalcCRTCRegs(crtcRegs, mode)
 *
 *     Initializes the crtcRegs structure for the selected mode. 
 */
void 
agxCalcCRTCRegs(crtcRegs, mode)
     agxCRTCRegPtr crtcRegs;
     DisplayModePtr mode;
{
    unsigned int temp;
    Bool hercDoubledClocks = hercBigDAC && mode->Clock > 15;
    Bool usingHercBigDac = hercBigDAC
                           && ( mode->Clock > 15
                                || agxInfoRec.depth > 8 ) ;

    if (!mode->CrtcVAdjusted && (mode->Flags & V_DBLSCAN)) {
       mode->CrtcVDisplay <<= 1;
       mode->CrtcVSyncStart <<= 1;
       mode->CrtcVSyncEnd <<= 1;
       mode->CrtcVTotal <<= 1;
       mode->CrtcVAdjusted = TRUE;
    }

    crtcRegs->clock_sel = mode->Clock;
    crtcRegs->disp_cntl_1 = 0;
    crtcRegs->disp_cntl_2 = 0;

    crtcRegs->hblnk_end_lo =
       crtcRegs->htotal_lo = (mode->CrtcHTotal >> 3) -1; 
    crtcRegs->hdisp_end_lo = 
       crtcRegs->hblnk_strt_lo = (mode->CrtcHDisplay >> 3) - 1;
    crtcRegs->hsync_strt_lo    = (mode->CrtcHSyncStart >> 3) - 1;
    crtcRegs->hsync_end_lo     = (mode->CrtcHSyncEnd >> 3) - 1;

    temp = (mode->CrtcHSyncEnd - mode->CrtcHSyncStart) >> 3;

    if (temp > 0x1f) {
        ErrorF("%s %s: Horizontal Sync width (%d) in mode \"%s\"\n",
               XCONFIG_PROBED, agxInfoRec.name, temp<<3,
               mode->name);
        ErrorF("\tshortened to 248 pixels\n");
        crtcRegs->hsync_end_lo = crtcRegs->hsync_strt_lo + 0x1f;
    }

#if 0
    ErrorF("DEBUG: hercBigDAC:%x, crtcRegs->clock_sel:%d, crtcRegs->bpp:%d, \
agxInfoRec.depth:%d, hercDoubledClocks:%x, usingHercBigDac:%x\n",
                hercBigDAC, crtcRegs->clock_sel, 
                crtcRegs->bpp, agxInfoRec.depth,
                hercDoubledClocks, usingHercBigDac );
#endif

    switch( agxInfoRec.depth ) {
       case 8:
            crtcRegs->bpp = 8;
            if (hercDoubledClocks) {
               crtcRegs->hblnk_strt_lo -= 2;
               crtcRegs->hblnk_end_lo  += 1;
            }
            else if (usingHercBigDac) {
#if 0           /* XGA CRTC DELAY */
               crtcRegs->hblnk_strt_lo -= 3;
               crtcRegs->hblnk_end_lo  -= 2;
#else
               crtcRegs->hblnk_strt_lo -= 2;
               crtcRegs->hblnk_end_lo  += 3;
#endif
            }
            crtcRegs->hsync_pos_1 = IR_CHP1_NO_DELAY;
            crtcRegs->hsync_pos_2 = IR_CHP2_NO_DELAY;
            crtcRegs->disp_cntl_2 |= IR_DC2_8_BPP;
            break;
       case 15:
       case 16:
            crtcRegs->bpp = 16;
            if (hercDoubledClocks) {
               crtcRegs->hblnk_strt_lo -= 1;
               crtcRegs->hblnk_end_lo  += 1;
            }
            else if (usingHercBigDac) {
               crtcRegs->hblnk_strt_lo -= 1;
               crtcRegs->hblnk_end_lo  -= 0;
            }

            crtcRegs->hsync_pos_1 = IR_CHP1_NO_DELAY;
            crtcRegs->hsync_pos_2 = IR_CHP2_NO_DELAY;
            crtcRegs->disp_cntl_2 |= IR_DC2_16_BPP;
            break;
       case 24:
       case 32:
            crtcRegs->bpp = 32;
            if (usingHercBigDac) {
               crtcRegs->hblnk_strt_lo -= 0;
               crtcRegs->hblnk_end_lo  += 0;
            }
            crtcRegs->hsync_pos_1 = IR_CHP1_NO_DELAY;
            crtcRegs->hsync_pos_2 = IR_CHP2_NO_DELAY;
            crtcRegs->disp_cntl_2 |= IR_DC2_24_BPP;
            break;
    }

    if (mode->Flags & V_NHSYNC) 
       crtcRegs->disp_cntl_1 |= IR_DC1_SYNC_HNEG;

    if (mode->Flags & V_NVSYNC) 
       crtcRegs->disp_cntl_1 |= IR_DC1_SYNC_VNEG;

    if (mode->Flags & V_INTERLACE) {
       crtcRegs->disp_cntl_1 |= IR_DC1_INTERLACED;
       crtcRegs->interlaced = TRUE;
    }
    else {
       crtcRegs->interlaced = FALSE;
    }

    temp = mode->CrtcVTotal - 1;
    crtcRegs->vblnk_end_lo =
       crtcRegs->vtotal_lo = temp & 0xFF;
    crtcRegs->vblnk_end_hi =
       crtcRegs->vtotal_hi = (temp>>8) & 0xFF;

    temp = mode->CrtcVDisplay - 1;
    crtcRegs->vdisp_end_lo = 
       crtcRegs->vblnk_strt_lo = temp & 0xFF;
    crtcRegs->vdisp_end_hi =
       crtcRegs->vblnk_strt_hi = (temp>>8) & 0xFF;

    temp = mode->CrtcVSyncStart - 1;
    crtcRegs->vsync_strt_lo = temp & 0xFF;
    crtcRegs->vsync_strt_hi = (temp>>8) & 0xFF;

    crtcRegs->vsync_end = mode->CrtcVSyncEnd & 0xFF;

    temp = mode->CrtcVSyncEnd - mode->CrtcVSyncStart;
    if (temp > 0x1F) {
        ErrorF("%s %s: Vertical Sync width (%d) in mode \"%s\"\n",
               XCONFIG_PROBED, agxInfoRec.name, temp, mode->name);
        ErrorF("\tshortened to 31 lines\n");
        crtcRegs->vsync_end = (crtcRegs->vsync_strt_lo + 0x1F) & 0xFF;
    }

    crtcRegs->mem_acc_mode = agxVideoMapFormat;

    if (mode->Flags & V_DBLSCAN) {
       crtcRegs->disp_cntl_2 |= IR_DC2_DBL_SCAN;
       crtcRegs->dbl_scan = TRUE;
    }
    else {
       crtcRegs->dbl_scan = FALSE;
    }

    temp = (crtcRegs->bpp * agxDisplayWidth) >> 6;
    crtcRegs->disp_width_lo = temp & 0xFF;
    crtcRegs->disp_width_hi = (temp>>8) & 0xFF;


    if( xf86FlipPixels ) 
       crtcRegs->overscan = 0x01;
    else 
       crtcRegs->overscan = 0x00;

    /* composite sync??? */
}

/*
 *  agxInitGE()
 *
 */
void
agxInitGE()
{
   
   GE_RESET();

   agxCurPixMap[0] = NULL;
   agxCurPixMap[1] = NULL;

   MAP_INIT( GE_MS_MAP_A,
             agxVideoMapFormat,
             agxMemBase,
             agxAdjustedVirtX-1,
             agxVirtY-1,
             agx128WidthAdjust,
             agx256WidthAdjust, 
             agx288WidthAdjust );

   MAP_SET_SRC_AND_DST( GE_MS_MAP_A );

   GE_SET_MAP( GE_MS_MAP_A );
   GE_OUT_B( GE_PIXEL_MAP_FORMAT, agxVideoMapFormat ); 
   GE_OUT_B( GE_FRGD_MIX, MIX_SRC );
   GE_OUT_B( GE_BKGD_MIX, MIX_SRC );
   GE_OUT_B( GE_CLR_COMP_FUNC, GE_CC_FALSE ); 
   GE_OUT_D( GE_PIXEL_BIT_MASK, agxPixMask ); 
   GE_OUT_D( GE_CARRY_CHAIN, agxPixMask ); 
   GE_OUT_D( GE_FRGD_CLR, 0x00000000 );
   GE_OUT_D( GE_BKGD_CLR, 0x00000000 );

}

/*
 *  agxSetCRTCRegs(crtcRegs)
 *
 *  Initializes the agx for the currently selected CRTC parameters.
 */
void 
agxSetCRTCRegs(crtcRegs)
     agxCRTCRegPtr crtcRegs;
{
   unsigned char byteData;
   Bool hercDoubledClocks = hercBigDAC 
                            && crtcRegs->clock_sel > 15;
   Bool usingHercBigDac = hercBigDAC 
                          && ( crtcRegs->clock_sel > 15 
                               ||  crtcRegs->bpp > 8 ) ;

   agxCurPixMap[0] = NULL;
   agxCurPixMap[1] = NULL;

   /*
    * Now initialize the display controller.
    * The CRTC registers are passed in from the calling routine.
    */

   if (AGX_SERIES(agxChipId)) {
      if( hercBigDAC ) {
         if( !hercDoubledClocks && inb(hercBrdIO+4) & 0x01 ) {
            /* needed to reliably get AGX-015 out of clock-doubling mode */ 
            outb(agxDAReg+DA_OP_MODE, DA_OM_DISP_VGA_XGA_EN);
            outb(agxIdxReg,IR_M2_MODE_REG_2); 
            byteData = inb(agxByteData) & IR_M2_PRESERVE_MASK;
            outb(agxByteData,byteData);
         }
         if( usingHercBigDac ) 
            hercSwitchToBigDac( hercDoubledClocks );
         else
            hercSwitchToLittleDac();
         agxClearColor0();
         outb(agxIdxReg, 0);
         outb(VGA_PAL_MASK, 0x00);
      }
      (*xf86RamDacInit)();
   }

   /*
    * switch to XGA graphics mode 
    */
   outb(agxDAReg+DA_OP_MODE, DA_OM_DISP_XGA_MODE);
   GE_RESET();
   agxResetCRTC(CRTC_PRERESET);
   if( agxClockSelectFunc == xgaNiClockSelect )
      xgaNiClockSelect(0,-1);
   else
      (*agxClockSelectFunc)(0,0);
   agxResetCRTC(CRTC_RESET);

   if(AGX_SERIES(agxChipId)) {
      if(AGX_10_ONLY(agxChipId))
         outb(agxIdxReg,IR_M1_AGX10_MODE_REG_1); 
      else
         outb(agxIdxReg,IR_M1_MODE_REG_1); 
      byteData = inb(agxByteData) & IR_M1_PRESERVE_MASK;
      if (OFLG_ISSET(OPTION_8_BIT_BUS, &agxInfoRec.options)) 
          byteData &= ~IR_M1_AGX_BUS_SIZE;
      else
          byteData |= IR_M1_AGX_BUS_SIZE;
      if (OFLG_ISSET(OPTION_WAIT_STATE, &agxInfoRec.options)) 
          byteData |= IR_M1_CPU_WAIT_STATE;
      if (OFLG_ISSET(OPTION_NO_WAIT_STATE, &agxInfoRec.options)) 
          byteData &= ~IR_M1_CPU_WAIT_STATE;
      if (OFLG_ISSET(OPTION_CRTC_DELAY, &agxInfoRec.options)) 
          byteData |= IR_M1_XGA_CRTC_DELAY;
      else
          byteData &= ~IR_M1_XGA_CRTC_DELAY;
      if (crtcRegs->interlaced) 
          byteData |= IR_M1_INTERLACED;         
      else
          byteData &= ~IR_M1_INTERLACED;

      if( !usingHercBigDac 
          && xf86RamDacType != SC15021_DAC  
          && !(agxInfoRec.depth == 16 && DAC_IS_ATT490)
        ) {
          /* for edge triggered RAMDAC modes */
          if( crtcRegs->bpp != 8 )
              byteData |= IR_M1_XGA_CRTC_DELAY;
      }

      outb(agxByteData,byteData);

      outb(agxIdxReg,IR_M2_MODE_REG_2); 
      byteData = inb(agxByteData) & IR_M2_PRESERVE_MASK;
      if (OFLG_ISSET(OPTION_VRAM_128, &agxInfoRec.options)) 
          byteData &= ~IR_M2_VRAM_256;
      if (OFLG_ISSET(OPTION_VRAM_256, &agxInfoRec.options)) 
          byteData |= IR_M2_VRAM_256;
      if (usingHercBigDac) {
          /* for AGX-015/016 pix-mux mode */
          byteData |= IR_M2_84DAC_SELECT 
                      | IR_M2_CCLK_DOUBLED;
      }
      outb(agxByteData,byteData);

      outb(agxIdxReg,IR_M3_MODE_REG_3); 
      byteData = inb(agxByteData) & IR_M3_PRESERVE_MASK;
      if (agxGEPhysBase == (pointer) 0xD1F00)
          byteData &= ~IR_M3_B1F00_GE_ADDRESS; 
      else
          byteData |= IR_M3_B1F00_GE_ADDRESS; 

      switch( crtcRegs->bpp ) {
         case 15:
         case 16:
            if( !usingHercBigDac
                 && xf86RamDacType != SC15021_DAC
                 && !(agxInfoRec.depth == 16 && DAC_IS_ATT490)
              )
               byteData |= IR_M3_PCLK_EDGE_TRIGGERED;
            break;
         case 24:
         case 32:
            if (!usingHercBigDac && xf86RamDacType != SC15021_DAC)
               byteData |= IR_M3_PCLK_EDGE_TRIGGERED; 
            if( xf86RamDacType == SC15021_DAC )
               byteData |= IR_M3_RGB_PACKED | IR_M3_24BPP_ENGINE;
            else
               byteData |= IR_M3_RGBX_UNPACKED | IR_M3_24BPP_ENGINE;
            break;
      }

      if (OFLG_ISSET(OPTION_REFRESH_20, &agxInfoRec.options)) 
          byteData &= ~IR_M3_SCREEN_REFRESH_25;
      if (OFLG_ISSET(OPTION_REFRESH_25, &agxInfoRec.options)) 
          byteData |= IR_M3_SCREEN_REFRESH_25; 
      outb(agxByteData,byteData);

      outb(agxIdxReg,IR_M5_MODE_REG_5); 
      byteData = 0;
      /*
       * Some boards only need refresh split at higher resolutions,
       * others need it at always. Haven't found any problems leaving
       * it on all the time.    
       */
      byteData |= IR_M5_REFRESH_SPLIT;
      if (OFLG_ISSET(OPTION_ENGINE_DELAY, &agxInfoRec.options))
          byteData |= IR_M5_ENGINE_DELAY;
#if 0
      if (!usingHercBigDAC) {
         switch( crtcRegs->bpp ) {
            case 15:
            case 16:
               byteData |= IR_M5_HICOLOR_DAC;
      }
#endif
      outb(agxByteData,byteData);

      if (AGX_15_16_ONLY(agxChipId)) {
         outb(agxIdxReg,IR_M7_MODE_REG_7); 
         byteData = inb(agxByteData) & IR_M7_PRESERVE_MASK;
         byteData &= ~IR_M7_BUFFER_ENABLE;
         if (OFLG_ISSET(OPTION_FIFO_MODERATE, &agxInfoRec.options)
             || OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &agxInfoRec.options))
             byteData |= IR_M7_BUFFER_ENABLE;
         if (OFLG_ISSET(OPTION_FIFO_CONSERV, &agxInfoRec.options))
             byteData &= ~IR_M7_BUFFER_ENABLE;  /* actually the default */
         if (OFLG_ISSET(OPTION_VLB_A, &agxInfoRec.options)) 
             byteData &= ~IR_M7_VLB_B;
         if (OFLG_ISSET(OPTION_VLB_B, &agxInfoRec.options)) 
             byteData |= IR_M7_VLB_B;
         if (OFLG_ISSET(OPTION_FAST_DRAM, &agxInfoRec.options)
             || OFLG_ISSET(OPTION_FAST_VRAM, &agxInfoRec.options))
             byteData &= ~(IR_M7_VRAM_RAS_DELAY | IR_M7_VRAM_LATCH_DELAY);
         if (OFLG_ISSET(OPTION_MED_DRAM, &agxInfoRec.options)) {
             byteData |= IR_M7_VRAM_LATCH_DELAY;
             byteData &= ~IR_M7_VRAM_RAS_DELAY;
         }
         if (OFLG_ISSET(OPTION_SLOW_DRAM, &agxInfoRec.options)
             || OFLG_ISSET(OPTION_SLOW_VRAM, &agxInfoRec.options)) {
             byteData |= IR_M7_VRAM_RAS_DELAY;
             byteData |= IR_M7_VRAM_LATCH_DELAY;
         }
         if (OFLG_ISSET(OPTION_VRAM_DELAY_LATCH, &agxInfoRec.options))
             byteData |= IR_M7_VRAM_LATCH_DELAY;
         if (OFLG_ISSET(OPTION_VRAM_DELAY_RAS, &agxInfoRec.options))
             byteData |= IR_M7_VRAM_RAS_DELAY;
         outb(agxByteData,byteData);
      }

      if (AGX_16_ONLY(agxChipId)) {
         outb(agxIdxReg,IR_M8_MODE_REG_8); 
         byteData = inb(agxByteData) & IR_M8_PRESERVE_MASK;
         if (OFLG_ISSET(OPTION_SPRITE_REFRESH, &agxInfoRec.options)) 
             byteData &= ~IR_M8_SPRITE_REFRESH;
         else
             byteData |= IR_M8_SPRITE_REFRESH;
         if (OFLG_ISSET(OPTION_SCREEN_REFRESH, &agxInfoRec.options)) 
             byteData |= IR_M8_SCREEN_REFRESH;
         else
             byteData &= ~IR_M8_SCREEN_REFRESH;
         if (OFLG_ISSET(OPTION_FIFO_AGGRESSIVE, &agxInfoRec.options))
             byteData |= IR_M8_BIG_BUFFER_ENABLE;
         else
             byteData &= ~IR_M8_BIG_BUFFER_ENABLE;
         if ( OFLG_ISSET(OPTION_MED_DRAM, &agxInfoRec.options)
              | OFLG_ISSET(OPTION_FAST_DRAM, &agxInfoRec.options) )
             byteData &= ~IR_M8_VRAM_RAS_EXTEND;
         if (OFLG_ISSET(OPTION_SLOW_DRAM, &agxInfoRec.options))
             byteData |= IR_M8_VRAM_RAS_EXTEND;
         if (OFLG_ISSET(OPTION_VRAM_EXTEND_RAS, &agxInfoRec.options))
             byteData |= IR_M8_VRAM_RAS_EXTEND;
         outb(agxByteData,byteData);
      } 

      if (AGX_16_ONLY(agxChipId)) {
         outb(agxIdxReg,IR_M10_MODE_REG_10);
         byteData = inb(agxByteData) & IR_M10_PRESERVE_MASK; 
         if (OFLG_ISSET(OPTION_WAIT_STATE, &agxInfoRec.options)) 
             byteData |= IR_M10_BUS_WAIT_STATE;
         if (OFLG_ISSET(OPTION_NO_WAIT_STATE, &agxInfoRec.options)) 
             byteData &= ~IR_M10_BUS_WAIT_STATE;
         outb(agxByteData,byteData);
      }
   } /* AGX_SERIES */

   /* Direct Access Registers */
   if (vgaPhysBase == (pointer)0xB0000) {
      outb(agxDAReg+DA_APERATURE_CNTL, DA_AC_64K_AT_B0000);
   }
   else {
      outb(agxDAReg+DA_APERATURE_CNTL, DA_AC_64K_AT_A0000);
   }
   outb(agxDAReg+DA_INTR_CNTL, DA_IC_INTR_DISABLE);
   outb(agxDAReg+DA_INTR_STAT, DA_IS_CLEAR_ALL);
   outb(agxDAReg+DA_VIRT_MEM_CNTL, DA_VM_DISABLE);
   outb(agxDAReg+DA_VIRT_MEM_STAT, DA_VS_CLEAR_ALL);
   outb(agxDAReg+DA_APERATURE_INDEX, 0);
   outb(agxDAReg+DA_MEM_ACCESS, crtcRegs->mem_acc_mode);

   /* Display Control */
   outb(agxIdxReg, IR_DISP_CNTL_1);
   byteData = inb(agxByteData) & IR_DC1_PRESERVE_MASK;
   byteData |= crtcRegs->disp_cntl_1 | IR_DC1_CRTC_PREP;
   outb(agxByteData, byteData);
   outb(agxIdxReg, IR_DISP_CNTL_2);
   outb(agxByteData, crtcRegs->disp_cntl_2);

    /* Horizontal CRTC registers */
   outb(agxIdxReg, IR_CRTC_HTOTAL_LO);
   outb(agxByteData, crtcRegs->htotal_lo);
   outb(agxIdxReg, IR_CRTC_HTOTAL_HI);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_CRTC_HDISP_END_LO);
   outb(agxByteData, crtcRegs->hdisp_end_lo);
   outb(agxIdxReg, IR_CRTC_HDISP_END_HI);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_CRTC_HBLANK_START_LO);
   outb(agxByteData, crtcRegs->hblnk_strt_lo);
   outb(agxIdxReg, IR_CRTC_HBLANK_START_HI);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_CRTC_HBLANK_END_LO);
   outb(agxByteData, crtcRegs->hblnk_end_lo);
   outb(agxIdxReg, IR_CRTC_HBLANK_END_HI);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_CRTC_HSYNC_START_LO); 
   outb(agxByteData, crtcRegs->hsync_strt_lo);
   outb(agxIdxReg, IR_CRTC_HSYNC_START_HI);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_CRTC_HSYNC_END_LO);
   outb(agxByteData, crtcRegs->hsync_end_lo);
   outb(agxIdxReg, IR_CRTC_HSYNC_END_HI);
   outb(agxByteData, 0x00); 
   outb(agxIdxReg, IR_CRTC_HSYNC_POS1);
   outb(agxByteData, crtcRegs->hsync_pos_1);
   outb(agxIdxReg, IR_CRTC_HSYNC_POS2);
   outb(agxByteData, crtcRegs->hsync_pos_2);

    /* Vertical CRTC registers */
   outb(agxIdxReg, IR_CRTC_VTOTAL_LO);
   outb(agxByteData, crtcRegs->vtotal_lo);
   outb(agxIdxReg, IR_CRTC_VTOTAL_HI);
   outb(agxByteData, crtcRegs->vtotal_hi);
   outb(agxIdxReg, IR_CRTC_VDISP_END_LO);
   outb(agxByteData, crtcRegs->vdisp_end_lo);
   outb(agxIdxReg, IR_CRTC_VDISP_END_HI);
   outb(agxByteData, crtcRegs->vdisp_end_hi);
   outb(agxIdxReg, IR_CRTC_VBLANK_START_LO);
   outb(agxByteData, crtcRegs->vblnk_strt_lo);
   outb(agxIdxReg, IR_CRTC_VBLANK_START_HI);
   outb(agxByteData, crtcRegs->vblnk_strt_hi);
   outb(agxIdxReg, IR_CRTC_VBLANK_END_LO);
   outb(agxByteData, crtcRegs->vblnk_end_lo);
   outb(agxIdxReg, IR_CRTC_VBLANK_END_HI);
   outb(agxByteData, crtcRegs->vblnk_end_hi);
   outb(agxIdxReg, IR_CRTC_VSYNC_START_LO);
   outb(agxByteData, crtcRegs->vsync_strt_lo);
   outb(agxIdxReg, IR_CRTC_VSYNC_START_HI);
   outb(agxByteData, crtcRegs->vsync_strt_hi);
   outb(agxIdxReg, IR_CRTC_VSYNC_END);
   outb(agxByteData,  crtcRegs->vsync_end);
   outb(agxIdxReg, IR_CRTC_VLINE_COMP_LO);
   outb(agxByteData, 0xFF);
   outb(agxIdxReg, IR_CRTC_VLINE_COMP_HI);
   outb(agxByteData, 0xFF);
   outb(agxIdxReg, IR_DISP_MAP_LO);
   outb(agxByteData, 0);
   outb(agxIdxReg, IR_DISP_MAP_MID);
   outb(agxByteData, 0);
   outb(agxIdxReg, IR_DISP_MAP_HI);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_DISP_MAP_WIDTH_LO);
   outb(agxByteData, crtcRegs->disp_width_lo);
   outb(agxIdxReg, IR_DISP_MAP_WIDTH_HI);
   outb(agxByteData, crtcRegs->disp_width_hi);
   outb(agxIdxReg, IR_CUR_CNTL);
   outb(agxByteData, 0x00);
   outb(agxIdxReg, IR_BORDER_CLR); 
   outb(agxByteData, crtcRegs->overscan);
   outb(agxIdxReg, IR_PAL_SEQUENCE);
   outb(agxByteData, IR_PS_FRMT_RGB);

   if(XGA_2_ONLY(agxChipId)) {
      outb(agxIdxReg, IR_NI_DIR_CNTL);
      outb(agxByteData, IR_NI_DC_LINEARIZED_COLOR);
      outb(agxIdxReg, IR_NI_MISC_CNTL);
      outb(agxByteData, IR_NI_MC_ENABLE_REDBLUE);
      outb(agxIdxReg, IR_NI_MFI_CNTL);
      outb(agxByteData, IR_NI_MFI_DISABLE);
   }
   outb(agxIdxReg, 0);

   /* Clock select register */
   (*agxClockSelectFunc)(crtcRegs->clock_sel,0);
   agxResetCRTC(CRTC_PRERESET);
   agxResetCRTC(CRTC_RESET);
   agxResetCRTC(CRTC_RUN);

   outb(agxIdxReg, 0);
}



/*
 * agxSwitchMode --
 *      Reinitialize the CRTC registers for the new mode.
 */
Bool
agxSwitchMode(mode)
    DisplayModePtr mode;
{
   extern LUTENTRY agxsavedLUT[256];

   xf86EnableIOPorts(agxInfoRec.scrnIndex);
   if (vgaPhysBase) {
      xf86MapDisplay(agxInfoRec.scrnIndex, VGA_REGION);
   }
   if (agxPhysVidMem != vgaPhysBase)  {
      xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION);
   }
   xf86MapDisplay(agxInfoRec.scrnIndex, LINEAR_REGION);

   GE_WAIT_IDLE();

   if( hercBigDAC )
      agxSaveLUT(agxsavedLUT);

   agxClearColor0();
   outb(agxIdxReg, 0);
   if( agxInfoRec.bitsPerPixel == 8 ) {
      if(XGA_PALETTE_CONTROL(agxChipId)) {
         outb(agxIdxReg, IR_PAL_MASK);
         outb(agxByteData, 0x00);
      }
      else {
         outb(VGA_PAL_MASK, 0x00);
      }
   }

   agxCalcCRTCRegs(&agxCRTCRegs, mode);
   agxInited = FALSE;
   agxInitDisplay(agxInfoRec.scrnIndex,&agxCRTCRegs);

   agxRestoreCursor(savepScreen);
   agxAdjustFrame(agxInfoRec.frameX0, agxInfoRec.frameY0);

   if( hercBigDAC )
      agxRestoreLUT(agxsavedLUT);
   else
      agxRestoreColor0(savepScreen);

   outb(agxIdxReg, 0);
   if( agxInfoRec.bitsPerPixel == 8 ) {
      if(XGA_PALETTE_CONTROL(agxChipId)) {
         outb(agxIdxReg, IR_PAL_MASK);
        outb(agxByteData, 0xFF);
      }
      else {
         outb(VGA_PAL_MASK, 0xFF);
      }
   }

   agxInited = TRUE;

   return(TRUE);
}

/*
 *  agxGetClocks() - derived from xf86GetClocks(). The XGA has  
 *                   V-retrace and frame-start interrupt status biti
 *                   which must be reset to clear.
 */

Bool
agxGetClocks(num, scale, knownclkindex, knownclkvalue, InfoRec)
   int num;
   int scale;
   int knownclkindex, knownclkvalue;
   ScrnInfoRec *InfoRec;
{
   unsigned char byteData; 
   Bool result = TRUE;

   unsigned long i, cnt, rcnt, sync;
   int saved_nice;

   /* First save registers that get written on */
   agxClockSelect(CLK_REG_SAVE,0);

#if defined(CSRG_BASED) || defined(MACH386)
   saved_nice = getpriority(PRIO_PROCESS, 0);
   setpriority(PRIO_PROCESS, 0, -20);
#endif
#if defined(SYSV) || defined(SVR4) || defined(linux)
   saved_nice = nice(0);
   nice(-20 - saved_nice);
#endif

   for (i = 0; i < num; i++) 
   {
      agxResetCRTC(CRTC_PRERESET); 
      agxResetCRTC(CRTC_RESET); 
      if (!agxClockSelect(i,scale))
      {
         InfoRec->clock[i] = -1;
         continue;
      }
      agxResetCRTC(CRTC_RUN); 

      /*
       * disable the outputs to reduce risk to the monitor
       */
      outb(agxIdxReg, IR_DISP_CNTL_1);
      byteData = inb(agxByteData) & IR_DC1_WRITE_MASK;
      byteData |= IR_DC1_CRTC_PREP;
      outb(agxByteData, byteData);
            
      usleep(50000);     /* let VCO stabilise */

      cnt  = 0;
      sync = 200000;

      if (!xf86DisableInterrupts())
      {
         agxClockSelect(CLK_REG_RESTORE,scale);
         ErrorF("Failed to disable interrupts during clock probe.  If\n");
         ErrorF("your OS does not support disabling interrupts, then you\n");
         FatalError("must specify a Clocks line in the XF86Config file.\n");
      }
      outb(agxDAReg+DA_INTR_STAT, DA_IS_START_VBLNK|DA_IS_START_PIC);
      while (!(inb(agxDAReg+DA_INTR_STAT) & DA_IS_START_VBLNK)) 
         if (sync-- == 0) goto nosync;
      outb(agxDAReg+DA_INTR_STAT, DA_IS_START_VBLNK|DA_IS_START_PIC);
      while (!(inb(agxDAReg+DA_INTR_STAT) & DA_IS_START_PIC)) 
         if (sync-- == 0) goto nosync;
      outb(agxDAReg+DA_INTR_STAT, DA_IS_START_VBLNK|DA_IS_START_PIC);
    
      for (rcnt = 0; rcnt < 5; rcnt++) 
      {
         while (!(inb(agxDAReg+DA_INTR_STAT) & DA_IS_START_VBLNK)) cnt++;
         outb(agxDAReg+DA_INTR_STAT, DA_IS_START_VBLNK|DA_IS_START_PIC);
         while (!(inb(agxDAReg+DA_INTR_STAT) & DA_IS_START_PIC)) cnt++;
         outb(agxDAReg+DA_INTR_STAT, DA_IS_START_VBLNK|DA_IS_START_PIC);
      }
      goto finish;

nosync:
      ErrorF("Failed to detect XGA vertical sync agxDAReg+DA_INTR_STAT, \
unable to probe clock values.\n");

finish:
                
      xf86EnableInterrupts();

      InfoRec->clock[i] = cnt ? cnt : -1;
   }

#if defined(CSRG_BASED) || defined(MACH386)
   setpriority(PRIO_PROCESS, 0, saved_nice);
#endif
#if defined(SYSV) || defined(SVR4) || defined(linux)
   nice(20 + saved_nice);
#endif

   /* Restore registers that were written on */
   agxClockSelect(CLK_REG_RESTORE,scale);

   for (i = 0; i < num; i++) 
   {
      if (i != knownclkindex)
      {
         if (InfoRec->clock[i] == -1)
         {
            InfoRec->clock[i] = 0;
         }
         else 
         {
            InfoRec->clock[i] = 
               (int)(0.5 + (((float)knownclkvalue)
                             * InfoRec->clock[knownclkindex]) 
                             / (InfoRec->clock[i]));
            /* Round to nearest 100KHz */
            InfoRec->clock[i] += 50;
            InfoRec->clock[i] /= 100;
            InfoRec->clock[i] *= 100;
         }
      }
   }

   InfoRec->clock[knownclkindex] = knownclkvalue;
   InfoRec->clocks = num; 

   return(TRUE);
}

void
agxResetCRTC( reset )
   int reset;
{
   unsigned char byteData;

   switch( reset ) {

      case CRTC_PRERESET:
         outb(agxIdxReg, IR_DISP_CNTL_1);
         byteData = inb(agxByteData) & IR_DC1_WRITE_MASK;
         byteData |= IR_DC1_CRTC_PREP;
         outb(agxByteData, byteData);
         break;

      case CRTC_RESET:
         outb(agxIdxReg, IR_DISP_CNTL_1);
         byteData = inb(agxByteData) & IR_DC1_WRITE_MASK;
         byteData |= IR_DC1_CRTC_RESET;
         outb(agxByteData, byteData);
         break;

      case CRTC_RUN:
         outb(agxIdxReg, IR_DISP_CNTL_1);
         byteData = inb(agxByteData) & IR_DC1_WRITE_MASK;
         byteData |= IR_DC1_CRTC_RUN;
         outb(agxByteData, byteData);
         break;
   }
}
   
/*
 * agxClockSelect - the AGX-16 has a lot!!! of clock control lines
 *                  there are 32 internal combinations (are all safe
 *                  to probe -- several combinations and bits like 
 *                  MSB Scale Factor are "reserved" in XGA documentation?)
 *                  and 16 external combinatons.
 *
 *  Clock Number Bit Assignments
 *
 *
 *  XGA and AGX-010
 *
 *          XGA Internal Clocks   REG       BIT(s)  Description
 *
 *              --------------- 0x54 ClkSel1    0    Clock Scale
 *              | ------------- 0x70 ClkSel2    7    XGA&VGA CLock Select
 *              | |  ---------- 0x54 ClkSel1   3-2   XGA&VGA CLock Select
 *              | |  |
 *  0-7         n n n n
 *
 *          Notable XGA clock conventions (hopefully useful for probing)
 *
 *     0    0 0 0 0 0 0    -  VGA 8-pixel text mode 
 *                            and 640x480 graphics (25.175MHz)
 *     1    0 0 0 0 0 1    -  VGA 9-pixel text mode (28.322MHz?)
 *     2    0 X X 0 1 0    -  Clock from feature connector
 *     3    0 0 0 0 1 1    -  XGA 1024x768 Interlaced (~44.9MHz * 2)
 *     4    0 0 0 1 0 0    -  132 Column Text Mode                
 *
 *
 *  AGX-010 only - AGX extended clocks 
 *
 *          AGX Extended XGA Clocks    REG       BIT(s)  Description
 *
 *          ------------------- 0x6F Mode5      6    Ext/XGA Clock
 *          |  ---------------- 0x77 Mode1     5-4   ClkSel3 (Ext MC:1-0) 
 *          |  |  ------------- 0x70 ClkSel2    7    XGA&VGA CLock Select
 *          |  |  |  ---------- 0x54 ClkSel1   3-2   XGA&VGA CLock Select
 *          |  |  |  |
 *   8-11   0 n n 0 1 1 
 *
 *    08    0 0 0 0 1 1    -  80.00MHz 
 *    09    0 0 1 0 1 1    -  50.35MHz
 *    10    0 1 0 0 1 1    -  44.90MHz 
 *    11    0 1 1 0 1 1    -  65.00Mhz 
 *
 *  12-15   8-11 repeated     
 *
 *
 *
 *  AGX-014 and later - Only the External Clocks are used.      
 *                      First two clocks should match XGA internal for
 *                      probing to work (i.e 25.175 and 28.322).
 *
 *          AGX External Clocks    REG     BIT(s)  Description
 *
 *          ----------------- 0x6F Mode5      6    Ext/XGA Clock
 *          |  -------------- 0x6F Mode5     5-4   ClkSel4 (Ext MC:3-2)
 *          |  |   ---------- 0x77 Mode1     5-4   ClkSel3 (Ext MC:1-0) 
 *          |  |   |
 *  00-16   1 n n n n 
 *
 */
Bool
agxClockSelect(no,scale)
     int no;
     int scale;
{
   unsigned char byteData; 

   switch(no&&0x0F)
   {
     case CLK_REG_SAVE:
       break;
     case CLK_REG_RESTORE:
       break;
     default:

        if ( XGA_SERIES(agxChipId) 
             || (AGX_10_ONLY(agxChipId) && no < 8) ) {
           /* XGA clock selects */
           outb(agxIdxReg, IR_CLOCK_SEL_1);
           byteData = inb(agxByteData) & IR_CS1_WRITE_MASK;
           byteData &= ~IR_CS1_CLOCK_MASK & ~IR_CS1_SCALE_MASK;
           byteData = 0;
           byteData |= ((no << IR_CS1_CLOCK_SHIFT) & IR_CS1_CLOCK_MASK);
           byteData |= no >> 3;
           outb(agxByteData, byteData);
    
           outb(agxIdxReg, IR_CLOCK_SEL_2);
           byteData = (no << (IR_CS2_CLOCK_SHIFT - 2)) & IR_CS2_CLOCK_MASK;
           outb(agxByteData, byteData);
        }
        else if (AGX_10_ONLY(agxChipId)) { 
           /* AGX-10 unique clocks */
           outb(agxIdxReg, IR_M5_MODE_REG_5);
           byteData = inb(agxByteData) & IR_M5_WRITE_MASK 
                      & ~IR_M5_CS4_MASK & ~IR_M5_EXT_CLOCK;
           outb(agxByteData, byteData);

           outb(agxIdxReg, IR_M1_AGX10_MODE_REG_1);
           byteData = inb(agxByteData)
                         & IR_M1_WRITE_MASK & ~IR_M1_CS3_MASK;
           byteData |= (no << IR_M1_CS3_SHIFT) & IR_M1_CS3_MASK;
           outb(agxByteData, byteData);

           outb(agxIdxReg, IR_CLOCK_SEL_1);
           byteData = inb(agxByteData) & IR_CS1_WRITE_MASK;
           byteData &= ~IR_CS1_CLOCK_MASK & ~IR_CS1_SCALE_MASK;
           byteData |= IR_CS1_CLOCK_MASK; 
           byteData |= scale & IR_CS1_SCALE_MASK; 
           outb(agxByteData, byteData);

           outb(agxIdxReg, IR_CLOCK_SEL_2);
           byteData = 0;
           outb(agxByteData, byteData);
        }
        else { 
           /* AGX-14 and later - external clocks */
           outb(agxIdxReg, IR_M5_MODE_REG_5);
	   byteData = inb(agxByteData) & IR_M5_WRITE_MASK & ~IR_M5_CS4_MASK;
           byteData |= IR_M5_EXT_CLOCK;
           byteData |= (no << (IR_M5_CS4_SHIFT - 2)) & IR_M5_CS4_MASK;
	   outb(agxByteData, byteData);

	   outb(agxIdxReg, IR_M1_MODE_REG_1);
	   byteData = inb(agxByteData)
                         & IR_M1_WRITE_MASK & ~IR_M1_CS3_MASK;
           byteData |= (no << IR_M1_CS3_SHIFT) & IR_M1_CS3_MASK;
	   outb(agxByteData, byteData);

           outb(agxIdxReg, IR_CLOCK_SEL_1);
           byteData = inb(agxByteData) & IR_CS1_WRITE_MASK;
           byteData &= ~IR_CS1_CLOCK_MASK & ~IR_CS1_SCALE_MASK;
           byteData |= scale & IR_CS1_SCALE_MASK; 
           outb(agxByteData, byteData);

           outb(agxIdxReg, IR_CLOCK_SEL_2);
           byteData = 0;
           outb(agxByteData, byteData);
      }
  }
  return(TRUE);
}

/* 
 * xgaNiClockSelect() - Programmable clock controller
 */

Bool
xgaNiClockSelect(no,scl)
   int no;
   int scl;
{
   long  clockNo;
   long  freq;
   unsigned char base, scale, byteData;
   Bool result = TRUE;

   switch(no)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = agxClockSelect(no,scl);
      break;
   default:
      /* Start with freq in kHz */
      if( scl == -1 )
         freq = 25000;
      else
         freq = agxInfoRec.clock[no];
      /* Check if clock frequency is within range */
      if (freq > agxInfoRec.maxClock) {
         ErrorF("%s %s: Specified dot clock (%7.3f) too high for RAMDAC.",
                XCONFIG_PROBED, agxInfoRec.name, freq / 1000.0);
         ErrorF("\tUsing %7.3MHz\n", agxInfoRec.maxClock / 1000.0);
         freq = agxInfoRec.maxClock;
      }
      if (freq >= 65000) {
         base = (freq / 1000) - 65;
         scale = IR_NI_PLL_SCALE_1;
      }
      else if (freq >= 32500) {
         base = ((freq * 2) / 1000) - 65;
         scale = IR_NI_PLL_SCALE_2;
      }
      else if (freq >= 16250) {
         base = ((freq * 4) / 1000) - 65;
         scale = IR_NI_PLL_SCALE_4;
      }
      else {
         ErrorF("%s %s: Specified dot clock (%d-%7.3f) too low for clock synthesizer.\n",
                XCONFIG_PROBED, agxInfoRec.name, no, freq / 1000.0 );
         result = FALSE;
         break;
      }
      
      outb(agxIdxReg, IR_CLOCK_SEL_1);
      byteData = inb(agxByteData) & ~IR_CS1_CLOCK_MASK & ~IR_CS1_SCALE_MASK;
      byteData &= ~IR_CS1_PRG_CLK_SEL;
      outb(agxByteData, byteData);

      outb(agxIdxReg, IR_NI_PLL_PRG_REG);
      byteData = scale | (base & IR_NI_PLL_SCALE_MASK); 
      outb(agxByteData, byteData);

      outb(agxIdxReg, IR_CLOCK_SEL_1);
      byteData = inb(agxByteData) & ~IR_CS1_CLOCK_MASK & ~IR_CS1_SCALE_MASK;
      byteData |= IR_CS1_PRG_CLK_SEL;
      outb(agxByteData, byteData);


      usleep(150000);
   }
   return(result);
}


/*
 * agxValidateClocks - Check the known clocks for consistency
 *
 */

struct clockValidate {
   unsigned char  clkNo;
   unsigned int   freq;
};

struct clockValidate xgaKnownClocks[] = {
   { 0x00, 25175 },
   { 0x01, 28322 },
   { 0x00,     0 } 
};

Bool
agxValidateClocks(tol)
   int tol;              /* percent tolerance */
{
   int error;
   unsigned int freq;
   unsigned int known;
   int no;
   int i;
   struct clockValidate *clk;
   int result = TRUE;

   i = 0;
   clk = xgaKnownClocks;
   while( known = clk[i].freq ) {
      no    = clk[i].clkNo;
      freq  = agxInfoRec.clock[no];
      error = freq - known;
      if ( error < 0 ) error = -error;
      error = (error*100)/known;
      if ( error > tol ) {
         result = FALSE;
         ErrorF("%s %s: dot clock %02d (%02.3f), %d%% out of tolerance, \
expected %02.3f+/-%d%%.\n",
             XCONFIG_PROBED, agxInfoRec.name,
             no, freq/1000.0, error, known/1000.0, tol);
      }
      i++;
   }
   return result;
}

/*
 * agxSetUpProbeCRTC - Setup the CRTC values for clock probe
 *
 */

Bool
agxSetUpProbeCRTC(crtcRegs)
   agxCRTCRegPtr crtcRegs;
{

  /* setup standard XGA 640x480 mode */

   crtcRegs->disp_cntl_1 = 0;
   crtcRegs->disp_cntl_2 = 0;
   crtcRegs->clock_sel   = 0;
   crtcRegs->bpp         = 0;

   crtcRegs->hblnk_end_lo =
      crtcRegs->htotal_lo = 0x63;
   crtcRegs->hdisp_end_lo =
      crtcRegs->hblnk_strt_lo = 0x4F;
   crtcRegs->hsync_strt_lo    = 0x55; 
   crtcRegs->hsync_end_lo     = 0x61; 

   crtcRegs->vblnk_end_lo =
      crtcRegs->vtotal_lo = 0x0C;
   crtcRegs->vblnk_end_hi =
      crtcRegs->vtotal_hi = 0x02;

   crtcRegs->vdisp_end_lo =
      crtcRegs->vblnk_strt_lo = 0xDF; 
   crtcRegs->vdisp_end_hi =
      crtcRegs->vblnk_strt_hi = 0x01;

   crtcRegs->vsync_strt_lo = 0xEA; 
   crtcRegs->vsync_strt_hi = 0x01;

   crtcRegs->vsync_end = 0xEC; 

   crtcRegs->disp_cntl_2 = IR_DC2_8_BPP;
   crtcRegs->mem_acc_mode = DA_MA_8_BPP;

   crtcRegs->disp_width_lo = 0x50;
   crtcRegs->disp_width_hi = 0x00;

   crtcRegs->clock_sel = 0;

   if( xf86FlipPixels ) 
      crtcRegs->overscan = 0x01;
   else 
      crtcRegs->overscan = 0x00;

   return(TRUE);
}


/*
 * agxProbeClocks - Try to determine the available clocks.
 *
 */

Bool
agxProbeClocks(scale)
   int scale;
{
   unsigned char  byteData;
   unsigned char  oldPalMask;
   unsigned short  wordData;
   int test = 0;

   if (AGX_SERIES(agxChipId) || AGX_10_ONLY(agxChipId)) {
      agxGetClocks( 16, 		     /* num of clocks */ 
                    scale-1,	             /* clock scaling factor */
                    0, 25175/scale,          /* known clock num & KHz value */ 
                    &agxInfoRec );
   }
   else { /* XGA */ 
      agxGetClocks( 8,   		     /* num of clocks */ 
                    scale-1,	             /* clock scaling factor */
                    0, 25175/scale,          /* known clock num & KHz value */ 
                    &agxInfoRec );
   }

   agxResetCRTC(CRTC_PRERESET);
   agxResetCRTC(CRTC_RESET);
   agxClockSelect(0,0);      /* set to a standard 640x480 clock */
   agxResetCRTC(CRTC_RUN);
   
   agxValidateClocks(5);

   if (xf86Verbose) {
      int i;

      ErrorF("%s ", XCONFIG_PROBED );
      ErrorF("%s: ", agxInfoRec.name);
      ErrorF("Number of Input Clocks: %d (use in XF86Config Clocks Statement)",
              agxInfoRec.clocks);

      for (i = 0; i < agxInfoRec.clocks; i++) {
         if (i % 8 == 0) 
            ErrorF("\n%s %s: clocks:", XCONFIG_PROBED, agxInfoRec.name);
	    ErrorF(" %6.2f", agxInfoRec.clock[i]/1000.0);
      }
      ErrorF("\n");
   }

   return TRUE;
}


/*
 *  agxSaveLUT(lut)
 *
 *       Saves the LUT in lut.
 */
void 
agxSaveLUT(lut)
   LUTENTRY *lut;
{
   short i;
   unsigned char oldIndex;

   oldIndex = inb(agxIdxReg);
   if(XGA_PALETTE_CONTROL(agxChipId)) {
      for (i = 0; i < 256; i++) {
         outb(agxIdxReg, IR_CUR_PAL_INDEX_PREF_LO);
         outb(agxByteData, i);
         outb(agxIdxReg,IR_PAL_RED_PRE);
         lut[i].r = inb(agxByteData);
         outb(agxIdxReg,IR_PAL_GREEN_PRE);
         lut[i].g = inb(agxByteData);
         outb(agxIdxReg,IR_PAL_BLUE_PRE);
         lut[i].b = inb(agxByteData);
      }
   }
   else {
      outb(agxIdxReg, 0);   /* make sure index is not 0x51 */
      outb(VGA_PAL_READ_INDEX, 0x00);
      for (i = 0; i < 256; i++) {
         inb(agxIdxReg);   /* Some RAMDAC's (SC15021) can't take full speed */
         lut[i].r = inb(VGA_PAL_DATA);
         lut[i].g = inb(VGA_PAL_DATA);
         lut[i].b = inb(VGA_PAL_DATA);
      }
   }
   
   outb(agxIdxReg, oldIndex);
}

/*
 *  agxRestoreLUT(lut)
 *
 *        Restores the LUT in lut.
 */
void agxRestoreLUT(lut)
   LUTENTRY *lut;
{
   short i;
   unsigned char oldIndex;
   unsigned int  palDataReg;

   oldIndex = inb(agxIdxReg); 
   if(XGA_PALETTE_CONTROL(agxChipId)) {
      outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
      outb(agxByteData, 0x00);
      outb(agxIdxReg, IR_PAL_DATA);
      palDataReg = agxByteData;
   }
   else {
      outb(agxIdxReg, 0);   /* make sure index is not 0x51 */
      outb(VGA_PAL_WRITE_INDEX, 0x00);
      palDataReg = VGA_PAL_DATA;
   } 

   for (i = 0; i < 256; i++) {
      outb(palDataReg, lut[i].r);
      outb(palDataReg, lut[i].g);
      outb(palDataReg, lut[i].b);
   }

   outb(agxIdxReg, oldIndex);
}

/*
 *  agxInitLUT()
 *
 *      Loads the Look-Up Table with all black.
 *      Assumes 8-bit board is in use.
 */
void 
agxInitLUT()
{
   short i;
   unsigned char oldIndex;
   unsigned int  palDataReg;

   agxSaveLUT(oldlut);
   LUTInited = TRUE;

   oldIndex = inb(agxIdxReg);
   if(XGA_PALETTE_CONTROL(agxChipId)) {
      outb(agxIdxReg, IR_CUR_PAL_INDEX_LO);
      outb(agxByteData, 0x00);
      outb(agxIdxReg, IR_PAL_DATA);
      palDataReg = agxByteData;
   }
   else {
      outb(agxIdxReg, 0);   /* make sure index is not 0x51 */
      outb(VGA_PAL_WRITE_INDEX, 0x00);
      palDataReg = VGA_PAL_DATA;
   }

   /* Load the LUT entries */
   for (i = 0; i < 256; i++) {
       outb(palDataReg, 0);
       outb(palDataReg, 0);
       outb(palDataReg, 0);
   }
   outb(agxIdxReg, oldIndex);
}

/*
 *   agxCleanUp()
 */
void
agxCleanUp()
{
   if (!agxInited)
      return;

   xf86EnableIOPorts(agxInfoRec.scrnIndex);

   agxHWRestore(agxSavedState);

   if (LUTInited) {
      agxRestoreLUT(oldlut);
      LUTInited = FALSE;
   }

   if(XGA_PALETTE_CONTROL(agxChipId)) {
      outb(agxIdxReg, IR_PAL_MASK);
      outb(agxByteData, 0xFF);
   }
   else {
      outb(VGA_PAL_MASK, 0xFF);
   }

   xf86DisableIOPorts(agxInfoRec.scrnIndex);

   agxInited = FALSE;
}

/*
 *   agxImageClear()
 *
 */

void
agxImageClear( unsigned int clr )
{

   GE_WAIT_IDLE();

   MAP_SET_SRC_AND_DST( GE_MS_MAP_A );

   GE_OUT_W(GE_FRGD_MIX, MIX_SRC);
   GE_OUT_D(GE_PIXEL_BIT_MASK, agxPixMask);
   GE_OUT_D(GE_FRGD_CLR, clr );

   GE_OUT_W( GE_PIXEL_OP, GE_OP_PAT_FRGD
                            | GE_OP_MASK_DISABLED
                            | GE_OP_INC_X
                            | GE_OP_INC_Y         );

   GE_OUT_D( GE_DEST_MAP_X, 0 );
   GE_OUT_W( GE_OP_DIM_WIDTH, agxVirtX-1 );
   GE_OUT_W( GE_OP_DIM_HEIGHT, agxVirtY-1 );
   GE_START_CMDW( GE_OPW_BITBLT
                  | GE_OPW_FRGD_SRC_CLR
                  | GE_OPW_DEST_MAP_A   );

   GE_WAIT_IDLE_EXIT();
}


