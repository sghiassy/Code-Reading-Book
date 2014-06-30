/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/pvga1/paradise.h,v 3.5 1996/12/23 06:58:04 dawes Exp $ */
/* Author: Mike Tierney <floyd@eng.umd.edu> */

/* Modified by: Bill Morgart <wsm@morticia.ssw.com> */
/*              for the WD90C33                     */



/* $XConsortium: paradise.h /main/5 1996/02/21 18:06:33 kaleb $ */

/* Paradise/Western Digital Chip defines */
#define C_PVGA1	0		/* PVGA1 */
#define WD90C00	1		/* WD90C00 */
#define WD90C10	2		/* WD90C1x */
#define WD90C30	3		/* WD90C30 */
#define WD90C24	4		/* WD90C24 Treat this as 90C3X */
#define WD90C31	5		/* WD90C31 */
#define WD90C33 6		/* WD90C33 */
#define WD90C20 7		/* WD90C2x */
#define IS_WD90C3X(x)	(((x) >= WD90C30) \
			 && ((x) <= WD90C33))

/* wd chip type */
extern int WDchipset;

/* standard WD90C3X register definitions */
/** Paradise Registers **/

/*** PR30  0x3C4, 0x10, 0x3C5                          ***/
/***  Memory Interface, Write Buffer, and FIFO Control ***/
#define PR30_INDEX 0x10
#define PR30_ADDR  0x3C4
#define PR30_PORT  0x3C5

/*** PR31  0x3C4, 0x11, 0x3C5  ***/
/***  System Interface Control ***/
#define PR31_INDEX              0x11
#define PR31_ADDR               0x3C4
#define PR31_PORT               0x3C5
#define RW_OFFSET_MASK          0x80
#define TURBO_MODE_MASKE        0x40
#define READ_RDY_CNTRL1         0x10
#define READ_RDY_CNTRL2         0x08
#define WRITE_BUF_ENABLE        0x04
#define ENA_16BIT_IO_ATTR_CNTRL 0x02
#define ENA_16BIT_IO_GEN        0x01

/** Extended Registers **/
#define EXT_REG_ACCESS_PORT      0x23C0
#define EXT_REG_IO_PORT          0x23C2
#define HBITBLT_PORT0            0x23C4
#define HBITBLT_PORT1            0x23C6
#define LINE_DRAW_K1             0x23C8
#define LINE_DRAW_K2             0x23CA
#define LINE_DRAW_ERROR_TERM     0x23CC
#define COMMAND_BUFFER           0x23CE

/** EXT_REG_ACCESS_PORT           **/
#define EXT_REG_BLK(x)           ((x) & 0xff)
#define EXT_REGISTER_INDEX(x)    (((x) << 8) & 0xf00)
#define EXT_REG_AUTO_INCR_ENA    0
#define EXT_REG_AUTO_INCR_DIS    0x1000

/** Command Buffer and Interrupt Control Register **/
/** id: COMMAND_BUFFER                            **/
#define VERT_RETRACE_INTR            0x200
#define CMD_BUFFER_EMPTY_STATUS_INTR 0x100
#define DRAWING_ENGINE_BUSY          0x080
#define CMD_BUFFER_OVERFLOW          0x040
#define CMD_BUFFER_ENABLE            0x020
#define CMD_BUFFER_LOC_AVAIL         0x00f

/** Extended Register Blocks **/
#define DRAWING_ENGINE_REG1    0x1 /* Table 12-1 WD90C33 */
#define CURSOR_BLOCK           0x2 /* Table 11-1 WD90C33 */
#define DRAWING_ENGINE_REG2    0x3 /* Table 12-2 WD90C33 */
#define REG_INDX(r)            ((r) << 12)
#define SET_EXT_REG(r, v) outw(EXT_REG_IO_PORT, (REG_INDX(r) | (v)))

/******************************/
/**                          **/
/** Drawing Engine Registers **/
/**                          **/
/******************************/

/**                          **/
/** DRAWING_ENGINE_REG1      **/
/**                          **/
#define DR_ENG_CNTRL1_INDEX     0x00
#define DR_ENG_CNTRL2_INDEX     0x01
#define DR_ENG_X_SRC            0x02
#define DR_ENG_Y_SRC            0x03
#define DR_ENG_X_DST            0x04
#define DR_ENG_Y_DST            0x05
#define DR_ENG_X_DIM            0x06
#define DR_ENG_Y_DIM            0x07
#define DR_ENG_RAS_OP           0x08
#define DR_ENG_LEFT_CLIP        0x09
#define DR_ENG_RIGHT_CLIP       0x0a
#define DR_ENG_TOP_CLIP         0x0b
#define DR_ENG_BOTTOM_CLIP      0x0c

/**                          **/
/** DRAWING_ENGINE_REG2      **/
/**                          **/
#define DR_ENG_MAP_BASE         0x00
#define DR_ENG_ROW_PITCH        0x01
#define DR_ENG_FORE_COLOR_0     0x02
#define DR_ENG_FORE_COLOR_1     0x03
#define DR_ENG_BACK_COLOR_0     0x04
#define DR_ENG_BACK_COLOR_1     0x05
#define DR_ENG_TRNS_COLOR_0     0x06
#define DR_ENG_TRNS_COLOR_1     0x07
#define DR_ENG_TRNS_MASK_0      0x08
#define DR_ENG_TRNS_MASK_1      0x09
#define DR_ENG_MASK_0           0x09
#define DR_ENG_MASK_1           0x0a

/*************************************/
/**                                 **/
/** DRAWING_ENGINE_CNTRL1_INDEX     **/
/**  - accessed via                 **/
/**    DRAWING_ENGINE_REG1, index 0 **/
/**                                 **/
/*************************************/

/*** Drawing Modes ***/
#define DRAWING_MODE(x)    ((x) << 9)
#define NOP                0x00
#define BITBLT             0x01
#define LINESTRIP          0x02
#define TRPZOID_FILL_STRIP 0x03
#define BRESENHAM_LINE     0x04

/*** X Direction ***/
#define X_DIR_POS          0x000
#define X_DIR_NEG          0x100

/*** Y Direction ***/
#define Y_DIR_POS          0x00
#define Y_DIR_NEG          0x80

/*** Major Row   ***/
#define X_MAJOR            0x00
#define Y_MAJOR            0x40

/*** Source Select ***/
#define SRC_SCREEN_MEMORY  0x00
#define SRC_HOST_MEMORY    0x20

/*** Source Format ***/
#define COLOR_FORMAT       0x00
#define MONO_FROM_COLOR    0x08
#define FIXED_COLOR        0x10
#define MONO_FROM_HOST     0x18

/*** Pattern Enable ***/
#define PATTERN_NOT_USED   0x00
#define PATTERN_USED       0x04

/*** Destination Select ***/
#define DEST_SCREEN_MEMORY 0x00
#define DEST_HOST_MEMORY   0x02

/*** Last Pixel Off ***/
#define LAST_PIXEL_ON      0x00
#define LAST_PIXEL_OFF     0x01

/*************************************/
/**                                 **/
/** DRAWING_ENGINE_CNTRL2_INDEX     **/
/**  - accessed via                 **/
/**    DRAWING_ENGINE_REG1, index 1 **/
/**                                 **/
/*************************************/
#define PIXEL_DEPTH(x)       ((x) << 10)
#define BITS_4               0x00
#define BITS_8               0x01
#define BITS_16              0x02
#define DEST_TRANS_ENABLE    0x200
#define DEST_TRANS_POLARITY  0x100
#define MONO_TRANS_SOURCE    0x080
#define COMMAND_BUFFER_EMPTY 0x040
#define DATA_PATH_FIFO_DEPTH 0x008
#define HBLT_MEMORY_PORT     0x004

/*******************************/
/**                           **/
/** Hardware cursor control   **/
/** accessed via CURSOR_BLOCK **/
/**                           **/
/** Note -                    **/
/** I've included these but   **/
/** its unlikely they will be **/
/** used anytime soon         **/
/**     wsm 6/21/94           **/
/**                           **/
/*******************************/

/*** Cursor Registers      ***/
/*** x - value 11 bits max ***/
/*** b - byte index 0,1,2  ***/
#define CURSOR_CNTL(x)                    (REG_INDX(0) | (x))
#define CURSOR_PATTERN_ADDR_LOW(x)        (REG_INDX(1) | (x))
#define CURSOR_PATTERN_ADDR_HIGH(x)       (REG_INDX(2) | (x))
#define CURSOR_PRIMARY_COLOR_BYTE(b, x)   (REG_INDX(3 + (b)) | (x))
#define CURSOR_SECONDARY_COLOR_BYTE(b, x) (REG_INDX(6 + (b)) | (x))
#define CURSOR_AUXILIARY_COLOR_BYTE(b, x) (REG_INDX(9 + (b)) | (x))
#define CURSOR_ORIGIN(x)                  (REG_INDX(12) | (x))
#define CURSOR_DISPLAY_X_POS(x)           (REG_INDX(13) | (x))
#define CURSOR_DISPLAY_Y_POS(x)           (REG_INDX(14) | (x))

#define REGISTER_BLOCK_INDEX(x)           (REG_INDX(15) | (x))

/*** Cursor Control Register ***/
#define CURSOR_ENABLE                            0x0800
#define CURSOR_PATTERN_32X32                     0
#define CURSOR_PATTERN_64X64                     0x0200
#define CURSOR_PLANE_PROTECT                     0x0100
#define CURSOR_COLOR_MODE_SOFT                   0
#define CURSOR_COLOR_MODE_2_COLOR_INVERT         0x0020
#define CURSOR_COLOR_MODE_2_COLOR_SPECIAL_INVERT 0x0040
#define CURSOR_COLOR_MODE_3_COLOR                0x0060
#define CURSOR_ENHANCED_HARDWARE_MODE            0x0010

/**
 ** register definitions for WD90C31 hardware accelerator
 **/
/**** WD90C31 BITBLT ****/
/** BITBLT Control Part 1, Index 1 **/
#define BLIT_CNTRL1   0x0000  /* blit index 0 */
#define BLT_ACT_STAT 0x0800  /* activation/status bit */
#define BLT_DIRECT   0x0400  /* blit direction 0-top->bottom,left->right */
                             /*                1-bottom->top,right->left */
#define BLT_PLANAR   0x0000  /* planar mode */
#define BLT_PACKED   0x0100  /* packed mode, text and 256-color */
#define BLT_DLINEAR  0x0080  /* blit is dest linear, 0 - blit is rectangular */
#define BLT_SLINEAR  0x0040  /* blit is source linear, 0 - blit is rectangular */
#define BLT_DST_SCR  0x0000  /* blit dest is screen memory */
#define BLT_DST_SYS  0x0020  /* blit dest is system IO location */
#define BLT_SRC_COLR 0x0000  /* color source format */
#define BLT_SRC_MONC 0x0004  /* source format is mono from color comparators */
#define BLT_SRC_FCOL 0x0008  /* source format is fixed color (filled rect) */
#define BLT_SRC_MONH 0x000C  /* source format is mono from host */
#define BLT_SRC_SCR  0x0000  /* blit source is screen memory */
#define BLT_SRC_SYS  0x0002  /* blit source is system IO location */

/** BITBLT Control Part 2, Index 1 **/
#define BLIT_CNTRL2   0x1000  /* blit index 0 */
#define BLT_IENABLE  0X0400  /* interrupt enable when blt completes */
#define BLT_QSTART   0X0080  /* quick start when dest reg written to */
#define BLT_UPDST    0x0040  /* update dest on completion of blit */
#define BLT_NOPAT    0x0000  /* no pattern */
#define BLT_8x8PAT   0x0010  /* 8x8 pattern used for source */
#define BLT_MTRNS    0x0008  /* monochrome transparency enable */
#define BLT_PTRNS    0x0004  /* matching pixels are 1-opaque 0-transparent */
#define BLT_TENABLE  0x0001  /* destination transparency enable */

/** BITBLT SOURCE Low, Index 2 */
#define BLIT_SRC_LOW  0x2000

/** BITBLT SOURCE High, Index 3 */
#define BLIT_SRC_HGH  0x3000

/** BITBLT DEST LOW, Index 4 */
#define BLIT_DST_LOW  0x4000

/** BITBLT DEST LOW, Index 5 */
#define BLIT_DST_HGH  0x5000

/** BITBLT Dim X, Index 6 **/
#define BLIT_DIM_X    0x6000

/** BITBLT Dim Y, Index 7 **/
#define BLIT_DIM_Y    0x7000

/** BITBLT Row Pitch, Index 8 **/
#define BLIT_ROW_PTCH 0x8000

/** BITBLT Raster Op, Index 9 **/
#define BLIT_RAS_OP   0x9000
#define ROP_ZERO     0x0000
#define ROP_AND      0x0100
#define ROP_SAND     0x0200
#define ROP_SRC      0x0300
#define ROP_NSAD     0x0400
#define ROP_DST      0x0500
#define ROP_XOR      0x0600
#define ROP_OR       0x0700
#define ROP_NOR      0x0800
#define ROP_XNOR     0x0900
#define ROP_NDEST    0x0A00
#define ROP_SOND     0x0B00
#define ROP_NSRC     0x0C00
#define ROP_NSOD     0x0D00
#define ROP_NAND     0x0E00
#define ROP_ONE      0x0F00

/** BLT Forground Color, Index 10 **/
#define BLIT_FOR_COLR 0xA000

/** BLT Background Color, Index 11 **/
#define BLIT_BCK_COLR 0xB000

/** BLT Transparency Color, Index 12 **/
#define BLIT_TRN_COLR 0xC000

/** BLT Transparency Mask, Index 13 **/
#define BLIT_TRN_MASK 0xD000

/** BLT MASK, Index 14 **/
#define BLIT_MASK 0xE000


/** BITBLT Macros for wd90c31 **/

/** wait for current blit operation to finish **/
#define WAIT_BLIT  outw(0x23C0, 0x1 | (0x10 << 8)); \
                     while (inw(0x23C2) & BLT_ACT_STAT);

/** wait finite time for current blit operation to finish **/
#define WAIT_BLIT_FINITE  \
                   { int count = 0; \
                     do { outw(0x23C0, 0x1 | (0x10 << 8)); \
                         fprintf (fp, "waiting...\n"); \
                     } while ((inw(0x23C2) & BLT_ACT_STAT) && count++ < 100); \
                     if (count >= 200) {vgaCloseScreen(); exit(-1);}\
                   }

#define C33_MAX_LOCATIONS_AVAILABLE 8
#define C33_LOCATIONS_USED_MASK (0x0f)
/** wait for command buffer to have enough empty locations **/
#define C33_WAIT_COMMAND_BUFFER(num_needed) \
  outw(COMMAND_BUFFER,CMD_BUFFER_ENABLE); \
  while ((inw(COMMAND_BUFFER) & C33_LOCATIONS_USED_MASK) > \
             (C33_MAX_LOCATIONS_AVAILABLE - num_needed))

/** wait for drawing engine to complete **/
#define C33_WAIT_DRAWING_ENGINE \
  while ((inw(COMMAND_BUFFER) & DRAWING_ENGINE_BUSY))

/**
 ** used to save off default functions
 **   sometimes the speedup routines need to bow out to the default funtion
 **   most of the speedups only work if the operation is screen->screen
 **/

extern void (*pvga1_stdcfbFillRectSolidCopy)();
extern void  (*pvga1_stdcfbDoBitbltCopy)();
extern void (*pvga1_stdcfbBitblt)();
extern void (*pvga1_stdcfbFillBoxSolid)();


/**
 ** these variables hold the current known state of the accel registers
 **   that way, time isn't wasted outputing a value to a port that already
 **   has that value.
 **/

extern unsigned int pvga1_blt_cntrl2;
extern unsigned int pvga1_blt_src_low;
extern unsigned int pvga1_blt_src_hgh;
extern unsigned int pvga1_blt_dim_x;
extern unsigned int pvga1_blt_dim_y;
extern unsigned int pvga1_blt_row_pitch;
extern unsigned int pvga1_blt_rop;
extern unsigned int pvga1_blt_for_color;
extern unsigned int pvga1_blt_bck_color;
extern unsigned int pvga1_blt_trns_color;
extern unsigned int pvga1_blt_trns_mask;
extern unsigned int pvga1_blt_planemask;

/* wd90c33 bitblt function prototype */
extern void
wd90c33BitBlt(
#if NeedFunctionPrototypes
	      unsigned char *psrcBase, unsigned char *pdstBase,
	      int widthSrc, int widthDst,
	      int x, int y,
	      int x1, int y1,
	      int w, int h,
	      int xdir, int ydir,
	      int alu,
	      unsigned long  planemask,
	      unsigned long source_format,
	      unsigned long foreground_color
#endif
);

/* wd90c31 bitblt function prototype */
extern void
wd90c31BitBlt(
#if NeedFunctionPrototypes
	      unsigned char *psrcBase, unsigned char *pdstBase,
	      int widthSrc, int widthDst,
	      int x, int y,
	      int x1, int y1,
	      int w, int h,
	      int xdir, int ydir,
	      int alu,
	      unsigned long  planemask,
	      unsigned long source_format,
	      unsigned long foreground_color
#endif
);

/**
 ** macros to set each hardware register
 **/

#define SELECT_BLT_REG   outb (EXT_REG_ACCESS_PORT, 1); 

#define SET_BLT_CNTRL1(value)  outw(EXT_REG_IO_PORT, BLIT_CNTRL1 | value);

#define SET_BLT_CNTRL2(value) \
              if (pvga1_blt_cntrl2 != (value)) \
                 outw(EXT_REG_IO_PORT, (BLIT_CNTRL2 | value)); \
              pvga1_blt_cntrl2 = value;

#define SET_BLT_DST_LOW(value) \
              outw (EXT_REG_IO_PORT, BLIT_DST_LOW | value);

#define SET_BLT_DST_HGH(value) \
              outw (EXT_REG_IO_PORT, BLIT_DST_HGH | value);

#define SET_BLT_SRC_LOW(value) \
              if (pvga1_blt_src_low != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_SRC_LOW | value); \
              pvga1_blt_src_low = value;

#define SET_BLT_SRC_HGH(value) \
              if (pvga1_blt_src_hgh != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_SRC_HGH | value); \
              pvga1_blt_src_hgh = value;

#define SET_BLT_DIM_X(value) \
              if (pvga1_blt_dim_x != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_DIM_X | value); \
              pvga1_blt_dim_x = value;

#define SET_BLT_DIM_Y(value) \
              if (pvga1_blt_dim_y != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_DIM_Y | value); \
              pvga1_blt_dim_y = value;

#define SET_BLT_ROW_PTCH(value) \
              if (pvga1_blt_row_pitch != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_ROW_PTCH | value); \
              pvga1_blt_row_pitch = value;

#define SET_BLT_RAS_OP(value) \
              if (pvga1_blt_rop != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_RAS_OP | value); \
              pvga1_blt_rop = value;

#define SET_BLT_FOR_COLR(value) \
              if (pvga1_blt_for_color != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_FOR_COLR | value); \
              pvga1_blt_for_color = value;

#define SET_BLT_BCK_COLR(value) \
              if (pvga1_blt_bck_color != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_BCK_COLR | value); \
              pvga1_blt_bck_color = value;

#define SET_BLT_TRN_COLR(value) \
              if (pvga1_blt_trns_color != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_TRN_COLR | value); \
              pvga1_blt_trns_color = value;

#define SET_BLT_TRN_MASK(value) \
              if (pvga1_blt_trn_mask != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_TRN_MASK | value); \
              pvga1_blt_trn_mask = value;

#define SET_BLT_MASK(value) \
              if (pvga1_blt_planemask != (value)) \
                 outw (EXT_REG_IO_PORT, BLIT_MASK | value); \
              pvga1_blt_planemask = value;
