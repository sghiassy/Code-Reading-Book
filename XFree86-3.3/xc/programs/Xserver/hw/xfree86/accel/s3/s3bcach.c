/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3bcach.c,v 3.11 1996/12/23 06:41:36 dawes Exp $ */
/*
 * Copyright 1993 by Jon Tombs. Oxford University
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Jon Tombs or Oxford University shall
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. The authors  make no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * JON TOMBS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * 
 */
/* $XConsortium: s3bcach.c /main/7 1996/10/23 11:44:22 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include        "xf86.h"
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"xf86bcache.h"
#include	"s3.h"
#include	"regs3.h"

void
s3CacheMoveBlock(srcx, srcy, dstx, dsty, h, w, id)
int srcx, srcy, dstx, dsty, h, w;
unsigned int id;
{
   BLOCK_CURSOR;
   WaitQueue(8);
   SET_SCISSORS(0,0,(s3DisplayWidth - 1), s3ScissB);
   SET_PIX_CNTL(MIXSEL_FRGDMIX);  
   SET_MIX(FSS_BITBLT | MIX_SRC, BSS_BITBLT | MIX_SRC);

   WaitQueue16_32(6,8);		/* now shift the cache */
   
   SET_WRT_MASK(id);
   SET_RD_MASK(id);
   SET_CURPT(srcx, srcy);
   SET_DESTSTP(dstx, dsty);

   WaitQueue(3);
   SET_AXIS_PCNT(w-1,h-1);
   SET_CMD(CMD_BITBLT | INC_X | INC_Y | DRAW | PLANAR | WRTDATA);

   /* sanity returns */
   WaitQueue16_32(5,6);
   SET_RD_MASK(~0);
   SET_PIX_CNTL(MIXSEL_FRGDMIX | COLCMPOP_F);
   SET_MIX(FSS_FRGDCOL | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;
}
