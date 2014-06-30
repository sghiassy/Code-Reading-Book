/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000reg.h,v 3.10 1996/12/23 06:40:51 dawes Exp $ */
/* p9000reg.h
 *
 * Copyright 1994, Erik Nygren (nygren@mit.edu)
 * Some VGA registers from svgalib.
 *
 * ERIK NYGREN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ERIK NYGREN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: p9000reg.h /main/6 1996/02/21 17:33:03 kaleb $ */

#ifndef INC_P9000_REG_H
#define INC_P9000_REG_H

#include "compiler.h"
#include "Xos.h"

/* VGA Registers - many of these are taken from libvga.h in svgalib */ 
#define CRT_IC        0x3D4
#define CRT_DC        0x3D5
#define IS1_RC        0x3DA
#define CRT_C         24
#define CRT           0   
#define ATT_C         21
#define ATT_IW        0x3C0
#define ATT_          CRT+CRT_C
#define ATT_R         0x3C1
#define GRA_C         9
#define GRA           ATT_+ATT_C
#define GRA_I         0x3CE         /* Graphics Controller Index */
#define GRA_D         0x3CF         /* Graphics Controller Data Register */
#define SEQ_C         5
#define SEQ           GRA+GRA_C
#define MIS           SEQ+SEQ_C

/* Sequencer Registers (see section 3.5 of the W5186 databook) */
#define SEQ_INDEX_REG        0x3C4  /* For setting the index */
#define SEQ_PORT             0x3C5
#define SEQ_MISC_INDEX       0x11
#define SEQ_CLKMODE_INDEX    0x01
#define SEQ_OUTPUT_CTL_INDEX 0x12   /* Vendor specific */       

/* VGA bank register */
#define VGA_BANK_REG 0x3CD

/* W5[12]86 Misc Output Register - See 3.4.3 of W5186 DataBook
 * Used for selecting which clock to use */
#define MISC_OUT_REG        0x3c2  /* Write to this port */
#define MISC_IN_REG         0x3cc  /* Read from this port */

/* VGA-related parameters */
#define FONT_BASE  0xA0000       /* The location of font data */
#define FONT_SIZE  0x2000        /* The size of font data */

/* BIOS parameters */
#define BIOS_BASE  0xC0000       /* The start of the BIOS */

/* Sync Polarities as stored in the VGA Sequencer Control Register */
#define SP_NEGATIVE 1     /*sync polarity is negative */
#define SP_POSITIVE 0     /*sync polarity is positive */

/* Clock related things */
#define CLK_JITTER_TOL 2000000L /* How close the mem and pixel clock
                                 * can be before we see jitter */
#define MEMSPEED      50000000L /* The primary value for the memory clock */
#define MEMSPEED_ALT  55000000L /* alternate: default to 55MHz (0x65AC3DL) */

#define P9000_MAX_32BPP_CLOCK 90000   /* The maximum clock at 32 bpp 
				       * see page 34 of the bt485 data sheet -
                                       * Maximum Lclk rate for either a
				       * 110Mhz or 135 Mhz Bt485 */

#define P9000_MAX_ORCHID_CLOCK 120000 /* they have ICD2061A's which max
				       * out at 120 Mhz */

/************* P9000 Registers and Contents ****************/
/* Note: Registers relative to CtlBase */

/*** System Control Registers ***/
#define SYSCONFIG       0x00004L  /* System Configuration */
#define INTERRUPT       0x00008L
#define INTERRUPT_EN    0x0000CL  /* Interrupt Enable */

/*** Video Control Registers ***/
#define HRZC            0x00104L  /* Horizontal Counter (r/o) */
#define HRZT            0x00108L  /* Horiz Length */
#define HRZSR           0x0010CL  /* Horiz Sync Rising Edge */
#define HRZBR           0x00110L  /* Horiz Blank Rising Edge */
#define HRZBF           0x00114L  /* Horiz Blank Falling Edge */
#define PREHRZC         0x00118L  /* Horiz Counter Preload Value */
#define VRTC            0x0011CL  /* Vertical Counter (r/o) */
#define VRTT            0x00120L  /* Vert Length */
#define VRTSR           0x00124L  /* Vert Sync Rising Edge */
#define VRTBR           0x00128L  /* Vert Blank Rising Edge */
#define VRTBF           0x0012CL  /* Vert Blank Falling Edge */
#define PREVRTC         0x00130L  /* Vert Counter Preload Value */
#define SRADDR          0x00134L  /* Screen Repaint Address (r/o) */
#define SRTCTL          0x00138L  /* Screen Refresh Timing Control */

/*** VRAM Control Registers ***/
#define MEM_CONFIG      0x00184L  /* Memory Configuration */
#define RFPERIOD        0x00188L  /* Refresh Period */
#define RLMAX           0x00190L  /* RAS Low Maximum (max time for RAS-) */

/*** Parameter Engine Control Registers ***/

/* Status Register */
#define STATUS          0x80000L  /* Status Register (ERROR???) */
#define SR_BUSY      0x40000000L  /* Drawing Engine Busy */
#define SR_ISSUE_QBN 0x80000000L  /* Quad/Blit Busy */
#define SR_PICKED    0x00000080L  /* Pick Detected */
#define SR_PIXEL_SW  0x00000040L  /* Error in Pixel Command */
#define SR_BLIT_SW   0x00000020L  /* Error in Blit Command */
#define SR_QUAD_SW   0x00000010L  /* Error in Quad Command */
#define SR_Q_CONCAVE 0x00000008L  /* Quad is Concave */
#define SR_Q_HIDDEN  0x00000004L  /* Quad is Outside Clipping Window */
#define SR_Q_VISIBLE 0x00000002L  /* Quad is Inside Clipping Window */
#define SR_Q_INTERS  0x00000001L  /* Quad Intersects Clipping Window */

/*** Clipping Regs ***/
/* initial tests show p9000 clipping slower than software - Chris Mason */
#define CINDEX          0x8018CL  /* current meta coord index */
#define W_OFFSET_XY     0x80190L  /* current window offset (use YX_PACK) */
#define YCLIP           0x801B0L  /* read only result of clipping checks */
#define XCLIP           0x801B4L  /* read only result of clipping checks */
#define CLIPLT0         0x00001L  /* and with results of XCLIP or YCLIP  */
#define CLIPGT0         0x00010L  /* and with results of XCLIP or YCLIP  */

/*** Drawing Engine Pixel Processing Registers ***/

#define FGROUND         0x80200L  /* Foreground Color */
#define BGROUND         0x80204L  /* Background Color */
#undef PMASK
#define PMASK           0x80208L  /* Plane Mask */
#define DRAW_MODE       0x8020CL  /* Draw Mode */
#define PAT_ORIGINX     0x80210L  /* Pattern Origin X */
#define PAT_ORIGINY     0x80214L  /* Pattern Origin Y */
#define RASTER          0x80218L  /* Raster */
#define PIXEL8_REG      0x8021CL  /* Pixel8 */
#define W_MIN           0x80220L  /* Window Minimum */
#define W_MAX           0x80224L  /* Window Maximum */
#define PATTERN01       0x80280L  /* Base of Pattern (0..1) */

/* Drawing Engine Raster Register (see p9000 docs, sec 4.1.5 for minterms) */
#define USE_PATTERN  0x00020000L  /* Enable Patter for Quad Draw */
#define IGM_OVERSIZED    0x00010000L  /* oversized mode (use to draw  lines) */
#define IGM_F_MASK       0xFF00L  /* Foreground Minterm Mask */
#define IGM_B_MASK       0xF0F0L  /* Backgound Minterm Mask */
#define IGM_S_MASK       0xCCCCL  /* Source Minterm Mask */
#define IGM_D_MASK       0xAAAAL  /* Destination Minterm Mask */

/*** Commands ***/
#define CMD_QUAD        0x80008L  /* Draw a Quadrilateral */
#define CMD_BLIT        0x80004L  /* Start a BitBlit */
#define CMD_PIXEL8      0x8000CL  /* Do a Pixel8 */
#define CMD_PIXEL8_SWAP 0xE000CL  /* do a pixel8 with endian swapping */
#define CMD_PIXEL1(n)  (0x80080L|((n-1)<<2)) /* Do a Pixel1 of n pixels */
#define CMD_PIXEL1_32   0x800FCL  /* Do a Pixel1 of 32 pixels */
#define CMD_NEXT_PIXELS 0x80014L  /* Do a Next_Pixels */

/*** Parameter Engine Device Coordinate Registers ***/

#define DEVICE_COORD    0x81000L  /* Should be combined with the following */
#define DC_0              0x000L  /* Reg:   X[0]/Y[0] */
#define DC_1              0x040L  /* Reg:   X[1]/Y[1] */
#define DC_2              0x080L  /* Reg:   X[2]/Y[2] */
#define DC_3              0x0C0L  /* Reg:   X[3]/Y[3] */
#define DC_ABS            0x000L  /* REL:   Absolute addressing in screen */
#define DC_REL            0x020L  /* REL:   Relative to window */
#define DC_X              0x008L  /* YX:    32-bit X value */
#define DC_Y              0x010L  /* YX:    32-bit Y value */
#define DC_YX             0x018L  /* YX:    16-bit X value (high bits) and
                                   *        16-bit Y value (low bits) */

/*** Meta Coordinate Pseudo Registers ***/

#define META_COORD      0x81200L  /* Should be combined with the following */
#define MC_POINT          0x000L  /* Vtype: Point */
#define MC_LINE           0x040L  /* Vtype: Line */
#define MC_TRI            0x080L  /* Vtype: Triangle */
#define MC_QUAD           0x0C0L  /* Vtype: Quadrilateral */
#define MC_RECT           0x800L  /* Vtype: Rectangle */
#define MC_ABS            0x000L  /* REL:   Absolute (rel to window offset) */
#define MC_REL            0x020L  /* REL:   Relative (rel to last vertex) */
#define MC_X              0x008L  /* YX:    32-bit X value */
#define MC_Y              0x010L  /* YX:    32-bit Y value */
#define MC_YX             0x018L  /* YX:    16-bit X value (high bits) and
                                   *        16-bit Y value (low bits) */

/* 
 * various endian swapping modes.  Usually endup swapping all for anything
 * that copies data from host to the card.
 *
 * these should be combined with the register address. ex:
 * p9000Store(CMD_PIXEL8 | HBBSWAP, CtlBase, psrc)
 */

#define HALFSWAP        0x40000L  /* half word swapping */
#define BYTESWAP        0x20000L  /* byte swapping      */
#define BITSWAP         0x10000L  /* bit swapping       */
#define HBBSWAP         0x70000L  /* swap everything for lazy typers ;) */

/* Packs two 16-bit values for MC_YX and DC_YX */
/* CAREFUL! This will drop y's sign bit. Don't use where y could be neg */
#define YX_PACK(x,y)   (((long)(x)<<16)|(long)(y))


/*** True Color parameters ***/

#define RGB16_565         0
#define RGB16_555         1
#define RGB32_888         2

/************ STRUCTURES *************/

typedef struct {
    unsigned char r, g, b;
} LUTENTRY;

typedef struct {
  unsigned long srtctl, memconfig, memsize;
} p9000MiscRegRec;  

typedef struct {
  unsigned long XSize, YSize, BytesPerPixel, MemAddrBits;
  unsigned long hrzt, hrzsr, hrzbr, hrzbf, prehrzc;
  unsigned long vrtt, vrtsr, vrtbr, vrtbf, prevrtc;
  unsigned long dotfreq;        /*IC Designs Pixel Dot Rate Shift Value */
  unsigned long sysconfig;      /* System config register used by accel
                                 * routines to determine things like
                                 * the resolution */
  unsigned long memspeed;       /*default to 55MHz (0x65AC3DL) */
  unsigned long hp;             /*horizontal sync polarity */
  unsigned long vp;             /*vertical sync polarity */
  unsigned interlaced;          /*If true 1, else 0 */
} p9000CRTCRegRec, *p9000CRTCRegPtr;

#endif /* INC_P9000_REG_H */
