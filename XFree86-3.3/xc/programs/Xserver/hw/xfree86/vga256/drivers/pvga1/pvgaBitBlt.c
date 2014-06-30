/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/pvga1/pvgaBitBlt.c,v 3.7 1996/12/23 06:58:06 dawes Exp $ */
/*
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
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
 * THOMAS ROELL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Thomas Roell, roell@informatik.tu-muenchen.de
 *
 */
/* $XConsortium: pvgaBitBlt.c /main/7 1996/02/21 18:06:42 kaleb $ */


/* 90C31 accel code: Mike Tierney <floyd@eng.umd.edu> */
/* WD90C33 accel code: Bill Morgart <wsm@morticia.ssw.com> */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"
#include	"cfb.h"
#include	"cfbmskbits.h"
#include	"cfb8bit.h"
#include	"fastblt.h"
#include	"mergerop.h"
#include        "vgaBank.h"

#include "compiler.h"
#include "xf86.h"
#include "vga.h"
#include "paradise.h"


void
pvgaBitBlt(pdstBase, psrcBase, widthSrc, widthDst, x, y,
	    x1, y1, w, h, xdir, ydir, alu, planemask)
     unsigned char *psrcBase, *pdstBase;  /* start of src and dst bitmaps */
     int    widthSrc, widthDst;
     int    x, y, x1, y1, w, h;
     int    xdir, ydir;
     int    alu;
     unsigned long  planemask;

{
  if (WDchipset == WD90C31 || WDchipset == WD90C24)
    {
      wd90c31BitBlt(pdstBase, psrcBase,
		    widthSrc, widthDst,
		    x, y,
		    x1, y1,
		    w, h,
		    xdir, ydir,
		    alu,
		    planemask,
		    BLT_SRC_COLR,
		    0);
      WAIT_BLIT; /* must wait, since memory writes can mess up as well */
    }
  else if (WDchipset == WD90C33)
    {
      wd90c33BitBlt(pdstBase, psrcBase,
		    widthSrc, widthDst,
		    x, y,
		    x1, y1,
		    w, h,
		    xdir, ydir,
		    alu,
		    planemask,
		    COLOR_FORMAT,
		    0);
    }
}


/************************************************/
/*                                              */
/*      WD90C31 HARDWARE BITBLT FUNCTION        */
/*                                              */
/************************************************/
void
wd90c31BitBlt(unsigned char *psrcBase, unsigned char *pdstBase,
	      int widthSrc, int  widthDst,
	      int x,     int  y,  
	      int x1,    int  y1,
	      int w,     int  h,
	      int xdir,  int  ydir,
	      int alu,
	      unsigned long  planemask,
	      unsigned long source_format,
	      unsigned long foreground_color)
{
  int psrc, pdst;
  int blit_direct;

  if (xdir == 1 && ydir == 1)    /* left to right */ /* top to bottom */
    {
      psrc = (y * widthSrc) + x;
      pdst = (y1 * widthDst) + x1;
      blit_direct = 0x0;
    }
  else                           /* right to left */ /* bottom to top */
    {
      psrc = ((y + h - 1) * widthSrc) + x + w - 1;
      pdst = ((y1 + h - 1 ) * widthDst) + x1 + w - 1;
      blit_direct = BLT_DIRECT;
    }
  
  WAIT_BLIT;
  SET_BLT_SRC_LOW ((psrc & 0xFFF));
  SET_BLT_SRC_HGH ((psrc >> 12));
  SET_BLT_DST_LOW ((pdst & 0xFFF));
  SET_BLT_DST_HGH ((pdst >> 12));
  SET_BLT_ROW_PTCH (widthDst);
  SET_BLT_DIM_X    (w);
  SET_BLT_DIM_Y    (h);
  SET_BLT_MASK     ((planemask & 0xFF));
  SET_BLT_RAS_OP   ((alu << 8));
  SET_BLT_CNTRL2   (0x00);
  if (source_format == BLT_SRC_FCOL)
    SET_BLT_FOR_COLR (foreground_color & 0xFF);
  SET_BLT_CNTRL1 (BLT_ACT_STAT | BLT_PACKED | source_format | blit_direct);
}

/************************************************/
/*                                              */
/*      WD90C33 HARDWARE BITBLT FUNCTION        */
/*                                              */
/************************************************/
void
wd90c33BitBlt(
	      unsigned char *psrcBase, unsigned char *pdstBase,
	      int widthSrc, int widthDst,
	      int x,    int y,
	      int x1,   int y1,
	      int w,    int h,
	      int xdir, int ydir,
	      int alu,
	      unsigned long  planemask,
	      unsigned long source_format,
	      unsigned long foreground_color)
{
  int xSrc, ySrc, xDst, yDst;
  int blit_direction = X_DIR_POS | Y_DIR_POS;
  int de_stat;

  /*******************************/
  /*                             */
  /* set the operation direction */
  /* and the starting corner     */
  /*                             */
  /*******************************/
  xSrc = x; xDst = x1;
  if (xdir == -1)
    {
      blit_direction |= X_DIR_NEG;
      xSrc += w - 1;
      xDst += w - 1;
    }

  ySrc = y; yDst = y1;
  if (ydir == -1)
    {
      blit_direction |= Y_DIR_NEG;
      ySrc += h - 1;
      yDst += h - 1;
    }


  
  /*********************************/
  /*                               */
  /* Access control register set 2 */
  /*                               */
  /*********************************/
  outw(EXT_REG_ACCESS_PORT,
       (EXT_REG_BLK(DRAWING_ENGINE_REG2)
	| EXT_REG_AUTO_INCR_DIS));

  /* Wait for room in the command buffer */
  C33_WAIT_COMMAND_BUFFER(4);
  
  /* set map base to 0 */
  SET_EXT_REG(DR_ENG_MAP_BASE, 0x00);

  /* set the row pitch */
  SET_EXT_REG(DR_ENG_ROW_PITCH, widthDst);
  
  /* set the mask */
  SET_EXT_REG(DR_ENG_MASK_0, planemask & 0xff);

  if (source_format == FIXED_COLOR)
    /* set forground color */
    SET_EXT_REG(DR_ENG_FORE_COLOR_0, foreground_color & 0xff);
  
  /*********************************/
  /*                               */
  /* Access control register set 1 */
  /*                               */
  /*********************************/
  outw(EXT_REG_ACCESS_PORT,
       (EXT_REG_BLK(DRAWING_ENGINE_REG1)
	| EXT_REG_AUTO_INCR_DIS));
  
  /* Wait for room in the command buffer */
  C33_WAIT_COMMAND_BUFFER(8);

  /* set (x,y) src */
  SET_EXT_REG(DR_ENG_X_SRC, xSrc);
  SET_EXT_REG(DR_ENG_Y_SRC, ySrc);
  
  /* set (x,y) dst */
  SET_EXT_REG(DR_ENG_X_DST, xDst);
  SET_EXT_REG(DR_ENG_Y_DST, yDst);
  
  /* set x,y dimensions */
  SET_EXT_REG(DR_ENG_X_DIM, w - 1);
  SET_EXT_REG(DR_ENG_Y_DIM, h - 1);
  
  /* set the (top, left) clip position */
  SET_EXT_REG(DR_ENG_LEFT_CLIP, x1);
  SET_EXT_REG(DR_ENG_TOP_CLIP, y1);
  
  /* Wait for room in the command buffer */
  C33_WAIT_COMMAND_BUFFER(5);

  /* set the (bottom, right) clip position */
  SET_EXT_REG(DR_ENG_BOTTOM_CLIP, y1 + h);
  SET_EXT_REG(DR_ENG_RIGHT_CLIP, x1 + w);
  
  /* set the raster op */
  SET_EXT_REG(DR_ENG_RAS_OP, (alu << 8));

  /* set the depth to 8 bits per pixel */
  SET_EXT_REG(DR_ENG_CNTRL2_INDEX,
	      PIXEL_DEPTH(BITS_8));
  
  /* Now let's do a bit blit operation */
  SET_EXT_REG(DR_ENG_CNTRL1_INDEX,
	      (DRAWING_MODE(BITBLT)
	       | blit_direction
	       | SRC_SCREEN_MEMORY
	       | source_format
	       | PATTERN_NOT_USED
	       | DEST_SCREEN_MEMORY));

  /******************************/
  /*                            */
  /* Wait for this op to finish */
  /*                            */
  /******************************/
  C33_WAIT_DRAWING_ENGINE;
}
