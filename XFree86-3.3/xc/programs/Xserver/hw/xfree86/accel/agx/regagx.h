/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/regagx.h,v 3.17 1996/12/23 06:33:06 dawes Exp $ */
/*
 * AGXregs.h
 *
 * Copyright (c) 1994, Henry A. Worth, Sunnyvale, California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry A. Worth not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Henry A. Worth makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * HENRY A. WORTH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 *
 */
/* $XConsortium: regagx.h /main/7 1996/02/21 17:18:50 kaleb $ */

#ifndef _AGXregs_H
#define _AGXregs_H

#include "compiler.h"
#include "xf86.h"
#include "vga.h"
#include "vgaBank.h"
#include "xf86RamDac.h"

#define MAX_XGA_NI_CLOCK        	90000
#define MAX_XGA_NI_CLOCK_CONSERV 	80000
#define MAX_XGA_1_CLOCK         	45000
#define MAX_AGX_16_CLOCK        	85000
#define MAX_AGX_15_CLOCK        	85000
#define MAX_AGX_14_CLOCK        	85000

#define MEM_SIZE_512K           0x0000
#define MEM_SIZE_1M             0x0004
#define MEM_SIZE_2M             0x0008
#define MEM_SIZE_4M             0x000c

#define NO_MULTI_IO 	1

/*
 * crtc regs structure 
 */
typedef struct {
    unsigned int  bpp;
    unsigned char htotal_lo;
    unsigned char hdisp_end_lo;
    unsigned char hblnk_strt_lo;
    unsigned char hblnk_end_lo;
    unsigned char hsync_strt_lo;
    unsigned char hsync_end_lo;
    unsigned char hsync_pos_1;
    unsigned char hsync_pos_2;
    unsigned char vtotal_lo,      vtotal_hi;
    unsigned char vdisp_end_lo,   vdisp_end_hi;
    unsigned char vblnk_strt_lo,  vblnk_strt_hi;
    unsigned char vblnk_end_lo,   vblnk_end_hi;
    unsigned char vsync_strt_lo,  vsync_strt_hi;
    unsigned char vsync_end;
    unsigned char mem_acc_mode;
    unsigned char disp_cntl_1,   disp_cntl_2;
    unsigned char disp_width_lo,  disp_width_hi;
    Bool          disp_width_plus_128;
    Bool          disp_width_plus_256;
    Bool          disp_width_plus_288;
    unsigned char clock_sel;
    unsigned char overscan;
    unsigned char interlaced;
    unsigned char dbl_scan;
} agxCRTCRegRec, *agxCRTCRegPtr;

/*
 * State save regiters
 */

#define MAX_AGX_IDX_REGS 128
typedef struct {
   unsigned char reg[MAX_AGX_IDX_REGS];
} agxIdxRec;

#define MAX_AGX_DA_REGS 16
typedef struct {
   unsigned char reg[MAX_AGX_DA_REGS];
} agxDARec;

typedef struct {
   unsigned int  chip;
   unsigned char reg;
   unsigned char mask;
   unsigned char set;
} agxSaveReg;

typedef struct {
   vgaHWRec      std;
   unsigned char vgaSegReg;
   unsigned char opMode;
   unsigned char restoreOpMode;
   agxDARec      agxDA;
   agxIdxRec     agxIdx;
   union xf86RamDacSave agxRamDacSave;
} agxSaveBlock;

/*
 * ChipID's
 *
 */

#define AGX_16			0x0800
#define AGX_15			0x0400
#define AGX_14			0x0200
#define AGX_10			0x0100
#define AGX_NEW			0x0E00
#define AGX_15_16		0x0C00
#define AGX_ALL			0x0F00
#define XGA_2			0x0002
#define XGA_1                   0x0001
#define XGA_UNKNOWN		0x0000
#define XGA_ALL			0x0103 
#define XGA_AGX_ALL		0x0F03

#define AGX_SERIES(chip)	(((chip)&(AGX_10|AGX_14|AGX_15|AGX_16))!=0)   
#define AGX_14_15_16_ONLY(chip)	(((chip)&(AGX_14|AGX_15|AGX_16))!=0)   
#define AGX_15_16_ONLY(chip)	(((chip)&(AGX_15|AGX_16))!=0)   
#define AGX_14_15_ONLY(chip)	(((chip)&(AGX_14|AGX_15))!=0)   
#define AGX_14_ONLY(chip)	((chip)==AGX_14)   
#define AGX_15_ONLY(chip)	((chip)==AGX_15)   
#define AGX_16_ONLY(chip)	((chip)==AGX_16)   
#define AGX_10_ONLY(chip)	((chip)==AGX_10)   
#define AGX_10_XGA_ONLY(chip)   (((chip)&(AGX_10|XGA_1|XGA_2))!=0)
#define XGA_SERIES(chip)	(((chip)&(XGA_1|XGA_2))!=0)   
#define XGA_2_ONLY(chip)	((chip)==XGA_2)   
#define XGA_1_ONLY(chip)	((chip)==XGA_1)   
#define XGA_DEFAULT(chip)	((chip)==XGA_UNKNOWN)   

#define XGA_PALETTE_CONTROL(notused)	(xf86RamDacType == XGA_DAC)
#define XGA_SPRITE(chip)		XGA_SERIES((chip))	
#define AGX_LIMITED_DISPLAY_WIDTH(chip)	AGX_14_15_ONLY((chip))	
#define EXTENDED_XGA_CLOCKS(chip)	AGX_SERIES((chip))	

/*
 *  Map management data strucs and macros
 *
 * 
 */

typedef struct {
      unsigned char  format;
      unsigned int   base;
      union { 
         struct {
            unsigned short width;                 /* pixel width-1 */
            unsigned short height;                /* pixel height-1 */
         } dim;
         unsigned    int combined;
      } size; 
      Bool           agx128Adjust;          /* agx-016 only */
      Bool           agx256Adjust;                  
      Bool           agx288Adjust;          /* agx-016 only */
} agxPixMap;

extern agxPixMap agxPixMaps[5];
extern agxPixMap *agxCurPixMap[2];

#define AGX_SRC_MAP  	0
#define AGX_DST_MAP  	1
#define AGX_PAT_MAP  	2
#define AGX_MSK_MAP  	3
      
#define MAP_SET_SRC( map )   if(agxCurPixMap[0] != &agxPixMaps[(map)]) \
                                agxMapSetSrc((map));
#define MAP_SET_DST( map )   if(agxCurPixMap[1] != &agxPixMaps[(map)]) \
                                agxMapSetDst((map));

#define MAP_SET_SRC_AND_DST( map ) \
                             if( agxCurPixMap[0] != &agxPixMaps[(map)] \
                                 || agxCurPixMap[1] != &agxPixMaps[(map)]) {\
                                agxMapSetSrc((map)); \
                                agxMapSetDst((map)); \
                             } 

#define MAP_INIT( map, fmt, bse, wid, hght, a128, a256, a288 ) \
{ \
   agxPixMap     *pMap  = &agxPixMaps[(map)]; \
   pMap->format          = (fmt); \
   pMap->base            = (bse); \
   pMap->size.dim.width  = (wid); \
   pMap->size.dim.height = (hght); \
   pMap->agx128Adjust    = (a128); \
   pMap->agx256Adjust    = (a256); \
   pMap->agx288Adjust    = (a288); \
   if( agxCurPixMap[0] == pMap ) \
      agxCurPixMap[0] = NULL; \
   if( agxCurPixMap[1] == pMap ) \
      agxCurPixMap[1] = NULL; \
} 


#define AGX_PIXEL_ADJUST( pix )  ( (pix) << BytesPerPixelShift )
#define AGX_TO_PIXEL( byt )  ( (byt) >> BytesPerPixelShift )


#define RGB8_PSEUDO      (-1)
#define RGB16_565         0
#define RGB16_555         1
#define RGB32_888         2


/*
 * AGX chipset register definitions
 *
 *
 */



/*
 *  POS - Programmable Option Select Registers
 *
 *   These are setup by BIOS, and appear to require BIOS to easily
 *   determine the address they are mapped at (at least if multiple 
 *   boards are installed). They are not documented in the AGX 
 *   databooks, but are in various XGA programming references. 
 *   On single board systems these may be mapped at I/O address 
 *   0x100-0x105.
 *
 *   References to be investigated:
 *   
 *             IBM DMQS Standard
 *
 *             VESA VXE 1.0
 *
 *
 *  These are I/O mapped registers.
 */

#define POS_DEF_IO_BASE		0x100
#define POS_EISA_MASK           0x0C80
#define POS_LAST_IO_REG	 	0x0F
#define POS_NUM_IO_REG		(POS_LAST_IO_REG+1)

#define POS_ID			0x00
#define POS_ID_LO		0x00
#define POS_ID_HI		0x01

#define POS_ID_MIN			0x8FD8
#define POS_ID_MAX			0x8FDB
#define POS_ID_LO_MIN 			0xD8
#define POS_ID_LO_MAX			0xDB
#define POS_ID_HI_VALID			0x8F


#define POS_CONF	 	0x02

#define POS_CONF_EXT_MEM_MASK    	0xF0      
#define POS_CONF_EXT_MEM_MULT    	0x200      
#define POS_CONF_EXT_MEM_BASE    	0xC0000      
#define POS_CONF_COPROC_REG_OFF		0x01C00      
#define POS_CONF_INSTANCE_MASK	 	0x0E
#define POS_CONF_XGA_ENABLE_MASK	0x01
#define POS_CONF_ISA_ENABLE_MASK	0x08
#define POS_CONF_EISA_ENABLE_MASK	0x01

#define POS_CONF_INSTANCE( pos2 )  \
             ((( pos2 ) & POS_CONF_INSTANCE_MASK) >> 1)

#define POS_CONF_XGA_IS_ENABLED( pos2 )   \
             (( pos2 ) & POS_CONF_XGA_IS_ENABLED)


#define POS_BUS_ARB_INDEX	0x00
#define POS_BUS_ARB		0x03

#define POS_BUS_ARB_LEVEL_MASK   	0x78      
#define POS_BUS_FAIRNESS_ENABLE  	0x04
#define POS_BUS_EXT_MEM_ENABLE   	0x02


#define POS_MEM_ACCESS_INDEX	0x00
#define POS_MEM_ACCESS		0x04

#define POS_MA_LINEAR_ACCESS_IDX	0x00

#define POS_MA_LINEAR_BASE_MASK            	0xFE
#define POS_MA_LINEAR_ENABLED_MASK		0x01
#define POS_MA_LINEAR_SHIFT			  23

#define POS_MA_LINEAR_ENABLED( pos4_0 )   \
           (( pos4_0 ) && POS_IDX_LINEAR_ENABLED_MASK)

#define POS_MA_LINEAR_BASE( pos4_0 )   \
           ( (void *) (((( pos4_0 ) >> 1 ) && 0x7F) << 24))


#define POS_1MB_APERTURE	0x05
#define POS_1MB_APERTURE_BASE_MASK	0x0F
#define POS_1MB_APERTURE_SHIFT		0x14

#define POS_INDEX       	0x06
#define POS_INDEX_HI      	0x06
#define POS_INDEX_LO      	0x07

#define POS_VESA_ID_INDEX	0x01
#define POS_VESA_ID		0x04

#define POS_VENDOR_ID_INDEX	0x02
#define POS_VENDOR_ID		0x04

#define POS_BIOS_CONF_INDEX	0x03
#define POS_BIOS_CONF		0x04
#define POS_BIOS_ENABLED_MASK		0x01
#define POS_BIOS_ADDRESS_MASK		0x0D
#define POS_BIOS_ADDRESS_MULT		0x400
#define POS_BIOS_ADDRESS_BASE		0xC0000

#define POS_CONTROL       		0x08
#define POS_EISA_CONTROL       	 	0x05
#define POS_EISA_OFFSET        	 	0x08
#define POS_INSTANCE_MASK		0x07


/*
 *
 * Direct Access Registers
 *
 *  These are I/O mapped registers with an address of the form:
 *
 *      21ix -- where i is the instance number of the XGA adapter
 *              and x is the register number
 */

#define DA_XGA_IO_BASE		0x2100
#define DA_AGX_IO_BASE		0x2160
#define DA_LAST_IO_REG		0x7F
#define DA_NUM_IO_REG 		(DA_LAST_IO_REG+1)

#define DA_OP_MODE		0x0
#define DA_OM_MASK		  	0x0F
#define DA_OM_NI_MASK		  	0x1F
#define DA_OM_CLEAR	  	  	0x00
#define DA_OM_CO_ENDIAN_MASK	  	0x08 
#define DA_OM_CO_ENDIAN_CLEAR     	0xF7 
#define DA_OM_CO_LITTLE_ENDIAN	  	0x00
#define DA_OM_CO_BIG_ENDIAN	  	0x08
#define DA_OM_DISP_MODE_MASK	  	0x07
#define DA_OM_DISP_MODE_CLEAR     	0xF8
#define DA_OM_DISP_VGA_XGA_DIS	  	0x00
#define DA_OM_DISP_VGA_XGA_EN	  	0x01
#define DA_OM_DISP_132_XGA_DIS	  	0x02
#define DA_OM_DISP_132_XGA_EN	  	0x03
#define DA_OM_DISP_XGA_MODE       	0x05


#define DA_APERATURE_CNTL	0x1
#define DA_AC_MASK		  	0x03
#define DA_AC_CLEAR		  	0x00
#define DA_AC_NO_64K		  	0x00
#define DA_AC_64K_AT_A0000	  	0x01
#define DA_AC_64K_AT_B0000	  	0x02

#define DA_INTR_CNTL		0x4
#define DA_IC_MASK			0xC7
#define DA_IC_INTR_DISABLE		0x00
#define DA_IC_CMD_DONE_ENABLE		0x80
#define DA_IC_ACCESS_REJ_ENABLE		0x40
#define DA_IC_SPRT_DSPCMP_ENABLE	0x04
#define DA_IC_START_PIC_ENABLE		0x02
#define DA_IC_START_BLNK_ENABLE		0x01

#define DA_INTR_STAT 		0x5
#define DA_IS_MASK			0xC7
#define DA_IS_CLEAR_ALL			0xC7
#define DA_IS_CMD_DONE			0x80
#define DA_IS_ACCESS_REJ		0x40
#define DA_IS_SPRT_DSPCMP		0x04
#define DA_IS_START_PIC			0x02
#define DA_IS_START_VBLNK		0x01

#define DA_VIRT_MEM_CNTL	0x6
#define DA_VM_MASK			0xC7
#define DA_VM_DISABLE			0x00

#define DA_VIRT_MEM_STAT	0x7
#define DA_VS_MASK			0xC0
#define DA_VS_CLEAR_ALL			0xC0


#define DA_APERATURE_INDEX  	0x8
#define DA_AI_MASK		  	0x3F
#define DA_AI_CLEAR     	  	0x00

#define DA_MEM_ACCESS		0x9
#define DA_MA_MASK		  	0x0F
#define DA_MA_CLEAR	  	  	0x00
#define DA_MA_BPP_MASK		  	0x07
#define DA_MA_BPP_CLEAR		  	0xF8
#define DA_MA_BIG_ENDIAN                0x08
#define DA_MA_1_BPP		  	0x00
#define DA_MA_2_BPP		  	0x01
#define DA_MA_4_BPP		  	0x02
#define DA_MA_8_BPP		  	0x03
#define DA_MA_16_BPP		  	0x04
#define DA_MA_24_BPP		  	0x05

#define DA_INDEX_REGISTER	0xA

#define DA_DATA_BYTE_REG	0xB

#define DA_DATA_WORD_REG  	0xC


/*
 * Index Registers
 *
 *   Accessed by placing register number into the Index Register (0xA)
 *   and reading/writting the Data register (0xB or 0xC)
 *
 */


#define IR_MC0_MEM_CONF_0		0x00
#define IR_MC0_MASK				0xFF
#define IR_MC1_MEM_CONF_1		0x01
#define IR_MC1_MASK				0xFF
#define IR_MC2_MEM_CONF_2		0x02
#define IR_MC2_MASK				0xFF
#define IR_MC0_AUTO_CONFIG		0x04
#define IR_COP_STATE_A_DATA		0x0C
#define IR_COP_STATE_B_DATA		0x0D

/* Display Timing */

#define IR_CRTC_HTOTAL_LO		0x10
#define IR_CRTC_HTOTAL_HI		0x11
#define IR_CRTC_HDISP_END_LO		0x12
#define IR_CRTC_HDISP_END_HI		0x13
#define IR_CRTC_HBLANK_START_LO		0x14
#define IR_CRTC_HBLANK_START_HI		0x15
#define IR_CRTC_HBLANK_END_LO		0x16
#define IR_CRTC_HBLANK_END_HI		0x17
#define IR_CRTC_HSYNC_START_LO		0x18
#define IR_CRTC_HSYNC_START_HI		0x19
#define IR_CRTC_HSYNC_END_LO		0x1A
#define IR_CRTC_HSYNC_END_HI		0x1B
#define IR_CRTC_HSYNC_POS1		0x1C
#define IR_CHP1_MASK				0x60
#define IR_CHP1_DELAY_4				0x40
#define IR_CHP1_NO_DELAY			0x00
#define IR_CRTC_HSYNC_POS2		0x1E
#define IR_CHP2_MASK				0x06
#define IR_CHP2_DELAY_4				0x04
#define IR_CHP2_NO_DELAY			0x00
#define IR_CRTC_VTOTAL_LO		0x20
#define IR_CRTC_VTOTAL_HI		0x21
#define IR_CRTC_VDISP_END_LO		0x22
#define IR_CRTC_VDISP_END_HI		0x23
#define IR_CRTC_VBLANK_START_LO		0x24
#define IR_CRTC_VBLANK_START_HI		0x25
#define IR_CRTC_VBLANK_END_LO		0x26
#define IR_CRTC_VBLANK_END_HI		0x27
#define IR_CRTC_VSYNC_START_LO		0x28
#define IR_CRTC_VSYNC_START_HI		0x29
#define IR_CRTC_VSYNC_END		0x2A
#define IR_CRTC_VLINE_COMP_LO		0x2C
#define IR_CRTC_VLINE_COMP_HI		0x2D
#define IR_CLOCK_SEL_1			0x54
#define IR_CS1_MASK				0x0F
#define IR_CS1_NI_MASK				0x1F
#define IR_CS1_WRITE_MASK			0x0F
#define IR_CS1_CLOCK_MASK			0x0C
#define IR_CS1_CLOCK_SHIFT			0x02
#define IR_CS1_SCALE_MASK			0x03
#define IR_CS1_SCALE_SHIFT			0x00
#define IR_CS1_PRG_CLK_SEL			0x80
#define IR_CLOCK_SEL_2 			0x70
#define IR_CS2_MASK				0x80
#define IR_CS2_WRITE_MASK			0x80
#define IR_CS2_CLOCK_MASK			0x80
#define IR_CS2_CLOCK_SHIFT			0x07


/* Cursor Control */

#define IR_CUR_HPOS_LO			0x30
#define IR_CUR_HPOS_HI			0x31
#define IR_CUR_HHOT_SPOT		0x32
#define IR_CHHS_MASK				0x3F
#define IR_CUR_VPOS_LO			0x33
#define IR_CUR_VPOS_HI			0x34
#define IR_CUR_VHOT_SPOT		0x35
#define IR_CVHS_MASK				0x3F
#define IR_CUR_CNTL			0x36
#define IR_CUR_ENABLE				0x01
#define IR_CUR_DISABLE				0x00
#define IR_CUR_COLOR0_RED		0x38
#define IR_CUR_COLOR0_GREEN		0x39
#define IR_CUR_COLOR0_BLUE		0x3A
#define IR_CUR_COLOR1_RED		0x3B
#define IR_CUR_COLOR1_GREEN		0x3C
#define IR_CUR_COLOR1_BLUE		0x3D
#define IR_CUR_PAL_INDEX_LO		0x60
#define IR_CUR_INDEX_HI       		0x61
#define IR_CUR_PAL_INDEX_PREF_LO	0x62
#define IR_CUR_INDEX_PREF_HI  		0x63
#define IR_CUR_DATA        		0x6A
#define IR_CUR_PIX_CLR_0			0x00
#define IR_CUR_PIX_CLR_1			0x01
#define IR_CUR_PIX_TRANS			0x02
#define IR_CUR_PIX_COMPL			0x03
#define IR_CUR_DATA_PREFETCH   		0x6B


/* Display Control */


#define IR_DISP_MAP_LO			0x40
#define IR_DISP_MAP_MID			0x41
#define IR_DISP_MAP_HI			0x42
#define IR_DMH_MASK			0x01    /* XGA is 0x08 ? */
#define IR_DISP_MAP_WIDTH_LO		0x43
#define IR_DISP_MAP_WIDTH_HI		0x44
#define IR_DISP_CNTL_1			0x50
#define IR_DC1_WRITE_MASK			0xF8
#define IR_DC1_PRESERVE_MASK			0x20
#define IR_DC1_CRTC_MASK			0x07
#define IR_DC1_CRTC_PREP			0x01
#define    CRTC_PRERESET				0x1
#define IR_DC1_CRTC_RESET			0x00
#define    CRTC_RESET					0x2
#define IR_DC1_CRTC_RUN  			0x03
#define    CRTC_RUN					0x3
#define IR_DC1_INTERLACED			0x08
#define IR_DC1_FEAT_CONN_EN			0x10
#define IR_DC1_SYNC_VPOS			0x00
#define IR_DC1_SYNC_VNEG			0x80
#define IR_DC1_SYNC_HPOS			0x00
#define IR_DC1_SYNC_HNEG			0x40
#define IR_SET_RS2        	 	0x51
#define IR_CLEAR_RS2			0x00
#define IR_DISP_CNTL_2			0x51
#define IR_DC2_1_BPP				0x00
#define IR_DC2_2_BPP				0x01
#define IR_DC2_4_BPP				0x02
#define IR_DC2_8_BPP				0x03
#define IR_DC2_16_BPP				0x04
#define IR_DC2_24_BPP				0x05
#define IR_DC2_NORMAL_SCAN                      0x00
#define IR_DC2_DBL_SCAN                         0x40
#define IR_DC2_DBL_CLOCK                        0x10
#define IR_DC2_QUAD_SCAN                        0x80
#define IR_DC2_QUAD_CLOCK                       0x20
#define IR_DISP_MON_ID_DAC_COMP		0x52
#define IR_DISP_MON_ID_DAC_COMP		0x52

/* AGX Mode Control Extensions */

#define IR_M1_MODE_REG_1		0x77
#define IR_M1_AGX10_MODE_REG_1		0x7F
#define IR_M1_PRESERVE_MASK			0x03
#define IR_M1_MASK				0xFF
#define IR_M1_WRITE_MASK			0xFF
#define IR_M1_EXT_ENG_REQ			0x80
#define IR_M1_PCLK_DIV_2			0x40
#define IR_M1_CS3_MASK				0x30
#define IR_M1_CS3_SHIFT				0x04
#define IR_M1_XGA_CRTC_DELAY			0x08
#define IR_M1_INTERLACED			0x04
#define IR_M1_CPU_WAIT_STATE			0x02
#define IR_M1_AGX_BUS_SIZE			0x01

#define IR_M2_MODE_REG_2		0x76
#define IR_M2_PRESERVE_MASK			0x01
#define IR_M2_MASK				0x3F
#define IR_M2_CCLK_DOUBLED			0x20
#define IR_M2_DELAY_DISPLAY			0x10
#define IR_M2_84DAC_SELECT			0x08
#define IR_M2_COPROC_CLK_DIV_2 			0x04
#define IR_M2_VRAM_256		    		0x01

#define IR_M3_MODE_REG_3		0x6D
#define IR_M3_PRESERVE_MASK			0x23
#define IR_M3_MASK				0x2F
#define IR_M3_B1F00_GE_ADDRESS			0x01
#define IR_M3_MCS16_INHIBIT    			0x02
#define IR_M3_256_SRC_ADJUST			0x04
#define IR_M3_256_DST_ADJUST 			0x08
#define IR_M3_RGB_PACKED			0x00
#define IR_M3_RGBX_UNPACKED			0x80
#define IR_M3_PCLK_EDGE_TRIGGERED		0x40
#define IR_M3_SCREEN_REFRESH_25			0x20
#define IR_M3_24BPP_ENGINE         		0x10

#define IR_M4_MODE_REG_4		0x6E
#define IR_M4_MASK				0xFF

#define IR_M5_MODE_REG_5		0x6F
#define IR_M5_PRESERVE_MASK			0x2C
#define IR_M5_MASK				0x7F
#define IR_M5_WRITE_MASK			0x8F
#define IR_M5_CS4_MASK 				0x30
#define IR_M5_CS4_SHIFT				0x04
#define IR_M5_HICOLOR_DAC               	0x80U
#define IR_M5_ENGINE_DELAY                      0x08
#define IR_M5_EXT_CLOCK                 	0x40
#define IR_M5_REFRESH_SPLIT			0x04

#define IR_M7_MODE_REG_7		0x6C
#define IR_M7_PRESERVE_MASK                     0x3F
#define IR_M7_MASK				0xFF
#define IR_M7_ROM_DAC_DISABLE                   0x20  /* must be preserved! */
#define IR_M7_VRAM_RAS_DELAY                    0x10
#define IR_M7_VRAM_LATCH_DELAY                  0x08
#define IR_M7_VLB_B                             0x04
#define IR_M7_BUFFER_ENABLE                     0x02
#define IR_M7_LOCAL_BUS 	                0x01

#define IR_M8_MODE_REG_8		0x71
#define IR_M8_PRESERVE_MASK                     0x40
#define IR_M8_288_SRC_ADJUST			0x01
#define IR_M8_128_SRC_ADJUST			0x02
#define IR_M8_288_DST_ADJUST			0x04
#define IR_M8_128_DST_ADJUST			0x08
#define IR_M8_SPRITE_REFRESH			0x10
#define IR_M8_SCREEN_REFRESH			0x20
#define IR_M8_VRAM_RAS_EXTEND                   0x40
#define IR_M8_BIG_BUFFER_ENABLE			0x80

#define IR_M10_MODE_REG_10		0x78
#define IR_M10_MASK                             0x85
#define IR_M10_PRESERVE_MASK                    0x85
#define IR_M10_PCI_ENABLE			0x01
#define IR_M10_1MB_AP_ENABLE			0x02
#define IR_M10_BUS_WAIT_STATE			0x04
#define IR_M10_ENABLE_6MB			0x10
#define IR_M10_BANK_SELECT_MASK			0x60
#define IR_M10_16_BIT_PIXEL			0x80

#define IR_VESA_DMA_READBACK		0x74
#define IR_VDR_MASK				0x0F
#define IR_VDR_DMA_CHAN				0x0E
#define IR_VDR_DMA_ENABLE			0x01

#define IR_VESA_VENDOR_ID		0x75


/* XGA-NI Extensions */
    /* Programable Clock */
#define IR_NI_PLL_PRG_REG		0x58
#define IR_NI_PLL_SCALE_4			0x00
#define IR_NI_PLL_SCALE_2			0x40
#define IR_NI_PLL_SCALE_1			0x80
#define IR_NI_PLL_SCALE_MASK			0x3F
   /* Handling of missing lsb color bit for Red & Blue in 5:6:5 16bpp mode */
#define IR_NI_DIR_CNTL   		0x59
#define IR_NI_DC_ZERO_INTENSITY_BLACK		0x00  /* lsb == 0 */
#define IR_NI_DC_NONZERO_COLOR_MODE		0x01  /* lsb = color != 0 */
#define IR_NI_DC_FULL_INTENSITY_WHITE		0x03  /* lsb == 1   */
#define IR_NI_DC_LINEARIZED_COLOR		0x04  /* msb => lsb */
  /* For monochrome displays */
#define IR_NI_MISC_CNTL   		0x6C
#define IR_NI_MC_BLANK_REDBLUE			0x01
#define IR_NI_MC_ENABLE_REDBLUE			0x00
  /* Text-mode cursor attributes */
#define IR_NI_MFI_CNTL			0x6D
#define IR_NI_MFI_DISABLE			0x00
 

/* Palette Control */

#define IR_BORDER_CLR			0x55
#define IR_PAL_MASK			0x64
#define IR_PAL_DATA			0x65
#define IR_PAL_SEQUENCE			0x66
#define IR_PS_MASK				0x07
#define IR_PS_FRMT_MASK				0x04
#define IR_PS_FRMT_RGB				0x00
#define IR_PS_FRMT_RBGx				0x04
#define IR_PAL_RED_PRE			0x67
#define IR_PAL_GREEN_PRE		0x68
#define IR_PAL_BLUE_PRE			0x69

#define VGA_PAL_MASK			0x3C6
#define VGA_PAL_READ_INDEX		0x3C7
#define VGA_PAL_WRITE_INDEX		0x3C8
#define VGA_PAL_DATA			0x3C9

#define VGA_NUM_PAL_REG			4

typedef struct {
    unsigned char r, g, b;
} LUTENTRY;

/* Palette Registers */

#define DAC_DEF_IO_BASE		0x3C6
#define DAC_LAST_IO_REG		0x3
#define DAC_NUM_IO_REG		(DAC_LAST_IO_REG+1)

#define DAC_MASK			0x00
#define DAC_R_INDEX			0x01
#define DAC_W_INDEX			0x02
#define DAC_DATA			0x03

/*
 * Graphics Engine Registers (memory mapped) 
 */

#define GE_OUT_B( reg, val )   (*(volatile unsigned char*)((unsigned char*)agxGEBase+(reg)) = ((unsigned char)val))
#define GE_OUT_W( reg, val )   (*(volatile unsigned short*)((unsigned char*)agxGEBase+(reg)) = ((unsigned short)val))
#define GE_OUT_D( reg, val )   (*(volatile unsigned int*)((unsigned char*)agxGEBase+(reg)) = ((unsigned int)val))

#define GE_IN_STATUS()         (*(volatile char*)((char *)agxGEBase+0x11)) 

#define GE_BUSY()              (((*(volatile char*)((char *)agxGEBase+0x11))&0x80)!=0)
#if 0
#define GE_BUSY()              (!(inb(agxDAReg+DA_INTR_STAT)&DA_IS_CMD_DONE))
#endif

#define GE_XGA2_BUSY()         ((*(volatile char*)((char *)agxGEBase+0x09))&0x80)

#define GE_WAIT_IDLE()         { int i = 5000000, j; \
                                 if(GE_BUSY()) { \
                                    if (XGA_2_ONLY(agxChipId))  \
                                       while(GE_XGA2_BUSY()&&i--); \
                                    while(GE_BUSY()&&i--);\
                                 } \
                                 if(GE_BUSY()) { \
                                   ErrorF("GE_WAIT_IDLE Timed Out - %s:%d\n",\
                                           __FILE__,__LINE__); \
                                 } \
                               }

#define GE_WAIT_IDLE_SHORT()   { int i = 500000, j; \
                                 if(GE_BUSY()) { \
                                    if (XGA_2_ONLY(agxChipId))  \
                                       while(GE_XGA2_BUSY()&&i--);  \
                                    while(GE_BUSY()&&i--); \
                                 } \
                                 if(GE_BUSY()) { \
                                    ErrorF("GE_WAIT_IDLE_SHORT Timed Out \
- %s:%d\n", \
                                           __FILE__,__LINE__); \
                                 } \
                               }

#define GE_WAIT_IDLE_EXIT()    {}

#define GE_START_CMD( cmd )    GE_OUT_D(0x7C, (cmd))
#define GE_START_CMDW( cmd )   GE_OUT_W(0x7E, (cmd))

#define GE_RESET()             { int i = 5000000; \
                                 GE_OUT_B(0x11,0x20); \
                                 if (XGA_2_ONLY(agxChipId))  \
                                    while(GE_XGA2_BUSY()&&i--);  \
                                 while(GE_BUSY()&&i--) ; \
                                 if(GE_BUSY()) {\
                                    ErrorF("GE_RESET Timed Out - %s:%d\n",\
                                           __FILE__,__LINE__); \
                                 } \
                                 GE_OUT_B(0x11,0x00); \
                               }
                                     

#define GE_DEF_MEM_BASE		0x0C1C00

#define GE_CONTROL_STATUS	0x11
#define GE_CS_TERMINATE_OP		0x20
#define GE_CS_GE_BUSY			0x80

#define GE_PIXEL_MAP_SEL	0x12
#define GE_MS_MASK_MAP			0x00
#define GE_MS_MAP_A			0x01
#define GE_MS_MAP_B			0x02
#define GE_MS_MAP_C			0x03

#define GE_PIXEL_MAP_BASE	0x14
#define GE_PIXEL_MAP_WIDTH	0x18
#define GE_PIXEL_MAP_HEIGHT	0x1A
#define GE_PIXEL_MAP_FORMAT	0x1C
#define GE_MF_1BPP			0x00
#define GE_MF_2BPP			0x01
#define GE_MF_4BPP			0x02
#define GE_MF_8BPP			0x03
#define GE_MF_16BPP			0x04
#define GE_MF_24BPP			0x05
#define GE_MF_INTEL_FORMAT		0x00
#define GE_MF_MOTO_FORMAT		0x08


#define GE_BRES_ERROR_TERM	0x20
#define GE_BRES_CONST_K1  	0x24
#define GE_BRES_CONST_K2  	0x28

#define GE_SHORT_STROKE		0x2C
#define GE_SS_MOVE			0x00
#define GE_SS_DRAW              	0x10
#define GE_SS_0				0x00
#define GE_SS_45			0x20
#define GE_SS_90			0x40
#define GE_SS_135			0x60
#define GE_SS_180			0x80
#define GE_SS_225			0xA0
#define GE_SS_270			0xC0
#define GE_SS_315			0XE0	

#define GE_FRGD_MIX		0x48
#define GE_BKGD_MIX		0x49
#define MIX_0				0x00
#define MIX_AND				0x01
#define MIX_SRC_AND_NOT_DST		0x02
#define MIX_SRC				0x03
#define MIX_NOT_SRC_AND_DST		0x04
#define MIX_DST				0x05
#define MIX_XOR				0x06
#define MIX_OR				0x07
#define MIX_NOR				0x08
#define MIX_XNOR			0x09
#define MIX_NOT_DST			0x0A
#define MIX_SRC_OR_NOT_DST		0x0B
#define MIX_NOT_SRC			0x0C
#define MIX_NOT_SRC_OR_DST		0x0D
#define MIX_NAND			0x0E
#define MIX_1				0x0F

#define GE_CLR_COMP_FUNC	0x4A
#define GE_CC_TRUE			0x00
#define GE_CC_GREATER			0x01
#define GE_CC_EQUAL			0x02
#define GE_CC_LESS			0x03
#define GE_CC_FALSE			0x04
#define GE_CC_NOT_LESS			0x05
#define GE_CC_NOT_EQUAL		0x06
#define GE_CC_NOT_GREATER		0x07

#define GE_CLR_COMP_CLR	0x4C

#define GE_PIXEL_BIT_MASK	0x50

#define GE_CARRY_CHAIN		0x54

#define GE_FRGD_CLR		0x58
#define GE_BKGD_CLR		0x5C

#define GE_OP_DIM_WIDTH		0x60
#define GE_OP_DIM_LINE_MAJ	0x60
#define GE_OP_DIM_HEIGHT	0x62

#define GE_MASK_MAP_X		0x6C
#define GE_MASK_MAP_Y		0x6E

#define GE_SRC_MAP_X 		0x70
#define GE_SRC_MAP_Y		0x72

#define GE_PAT_MAP_X		0x74
#define GE_PAT_MAP_Y		0x76

#define GE_DEST_MAP_X 		0x78
#define GE_DEST_MAP_Y		0x7A


#define GE_PIXEL_OP		0x7C

#define GE_OP_SHRT_STRK_RD		0x02000000
#define GE_OPW_SHRT_STRK_RD		0x0200
#define GE_OP_LINE_DRAW_RD		0x03000000
#define GE_OPW_LINE_DRAW_RD		0x0300
#define GE_OP_SHRT_STRK_WR		0x04000000
#define GE_OPW_SHRT_STRK_WR		0x0400
#define GE_OP_LINE_DRAW_WR		0x05000000
#define GE_OPW_LINE_DRAW_WR		0x0500
#define GE_OP_BITBLT			0x08000000
#define GE_OPW_BITBLT			0x0800
#define GE_OP_INV_BITBLT		0x09000000
#define GE_OPW_INV_BITBLT		0x0900
#define GE_OP_AREA_FILL			0x0A000000
#define GE_OPW_AREA_FILL		0x0A00

#define GE_OP_FRGD_SRC_CLR		0x00000000
#define GE_OPW_FRGD_SRC_CLR		0x0000
#define GE_OP_FRGD_SRC_MAP		0x20000000
#define GE_OPW_FRGD_SRC_MAP		0x2000

#define GE_OP_BKGD_SRC_CLR		0x00000000
#define GE_OPW_BKGD_SRC_CLR		0x0000
#define GE_OP_BKGD_SRC_MAP		0x80000000
#define GE_OPW_BKGD_SRC_MAP		0x8000

#define GE_OP_SRC_MAP_A			0x00100000
#define GE_OPW_SRC_MAP_A		0x0010
#define GE_OP_SRC_MAP_B			0x00200000
#define GE_OPW_SRC_MAP_B		0x0020
#define GE_OP_SRC_MAP_C			0x00300000
#define GE_OPW_SRC_MAP_C		0x0030

#define GE_OP_DEST_MAP_A		0x00010000
#define GE_OPW_DEST_MAP_A		0x0001
#define GE_OP_DEST_MAP_B		0x00020000
#define GE_OPW_DEST_MAP_B		0x0002
#define GE_OP_DEST_MAP_C		0x00030000
#define GE_OPW_DEST_MAP_C		0x0003

#define GE_OP_PAT_MAP_A			0x00001000
#define GE_OP_PAT_MAP_B			0x00002000
#define GE_OP_PAT_MAP_C			0x00003000
#define GE_OP_PAT_FRGD			0x00008000
#define GE_OP_PAT_SRC  			0x00009000

#define GE_OP_MASK_DISABLED		0x00000000
#define GE_OP_MASK_ENABLED		0x00000080
#define GE_OP_MASK_BOUNDARY		0x00000040

#define GE_OP_DRAW_ALL			0x00000000
#define GE_OP_DRAW_FIRST_NULL		0x00000010
#define GE_OP_DRAW_LAST_NULL		0x00000020
#define GE_OP_DRAW_BOUNDARY		0x00000030

#define GE_OP_Y_MAJ			0x00000001
#define GE_OP_X_MAJ			0x00000000
#define GE_OP_DEC_X			0x00000004
#define GE_OP_DEC_Y			0x00000002
#define GE_OP_INC_X			0x00000000
#define GE_OP_INC_Y			0x00000000

#ifndef NO_MULTI_IO 
#define GE_SET_MAP( map )   \
{ \
   agxPixMap     *pMap = &agxPixMaps[ (map) ]; \
   GE_WAIT_IDLE(); \
   GE_OUT_B( GE_PIXEL_MAP_SEL,    (map) ); \
   GE_OUT_D( GE_PIXEL_MAP_BASE,   pMap->base ); \
   GE_OUT_D( GE_PIXEL_MAP_HEIGHT, pMap->size.combined ); \
   GE_OUT_B( GE_PIXEL_MAP_FORMAT, pMap->format ); \
}  
#else
#define GE_SET_MAP( map )   \
{ \
   agxPixMap     *pMap = &agxPixMaps[ (map) ]; \
   GE_WAIT_IDLE(); \
   GE_OUT_B( GE_PIXEL_MAP_SEL,    (map) ); \
   GE_OUT_D( GE_PIXEL_MAP_BASE,   pMap->base ); \
   GE_OUT_W( GE_PIXEL_MAP_HEIGHT, pMap->size.dim.height ); \
   GE_OUT_W( GE_PIXEL_MAP_WIDTH,  pMap->size.dim.width ); \
   GE_OUT_B( GE_PIXEL_MAP_FORMAT, pMap->format ); \
} 
#endif 

#endif
