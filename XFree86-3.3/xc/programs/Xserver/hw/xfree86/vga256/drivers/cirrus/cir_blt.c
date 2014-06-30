/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_blt.c,v 3.5 1996/12/23 06:56:34 dawes Exp $ */
/*
 *
 * Copyright 1993 by Bill Reynolds, Santa Fe, New Mexico
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Bill Reynolds not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Bill Reynolds makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * BILL REYNOLDS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL BILL REYNOLDS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Bill Reynolds, bill@goshawk.lanl.gov
 *
 * Reworked by: Simon P. Cooper, <scooper@vizlab.rutgers.edu>
 * Modifications: Harm Hanemaayer <hhanemaa@cs.ruu.nl>
 *
 */
/* $XConsortium: cir_blt.c /main/4 1996/02/21 18:03:14 kaleb $ */

#include "misc.h"
#include "xf86.h"
#include "X.h"
#include "Xos.h"
#include "Xmd.h"
#include "Xproto.h"
#include "gcstruct.h"
#include "windowstr.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "cfb.h"
#include "cfbmskbits.h"
#include "cfb8bit.h"

#include "mergerop.h"
#include "vgaBank.h"
#include "compiler.h"
#include "os.h"		/* For FatalError */

#include "cir_driver.h"

extern pointer vgaBase;


/*
 * This is the mid-level BitBlt function that calls the appropriate
 * low level function, depending on size, specific chipset, etc.
 *
 * Only called if alu == GXcopy && (planemask & 0xff) == 0xff.
 *
 * We could also use the BitBLT engine for non-GXcopy blits; do they
 * ever happen?
 */

#if 0

void
CirrusBitBlt (pdstBase, psrcBase, widthSrc, widthDst, x, y,
	      x1, y1, w, h, xdir, ydir, alu, planemask)
     pointer pdstBase, psrcBase;	/* start of src bitmap */
     int widthSrc, widthDst;
     int x, y, x1, y1, w, h;	/* Src x,y; Dst x1,y1; Dst (w)idth,(h)eight */
     int xdir, ydir;
     int alu;
     unsigned long planemask;
#endif

void
CirrusPolyBitBlt(pdstBase, psrcBase, widthSrc, widthDst, nbox, pptSrc,
		 pbox, xdir, ydir)
	pointer pdstBase, psrcBase;
	int widthSrc, widthDst;
	int nbox;
	DDXPointPtr pptSrc;
	BoxPtr pbox;
	int xdir, ydir;
{
  unsigned int psrc, pdst;
  int i;

  for (; nbox; pbox++, pptSrc++, nbox--) {
      int x, y, x1, y1, w, h;
      int bltWidthSrc, bltWidthDst, bltxdir;
      x = pptSrc->x;
      y = pptSrc->y;
      x1 = pbox->x1;
      y1 = pbox->y1;
      w = pbox->x2 - x1;
      h = pbox->y2 - y1;

      if (!HAVEBITBLTENGINE()) {
          if (xdir == 1 && ydir == 1) {
              /* Special case for extended write mode bitblt (scroll up). */
              if (w >= 32 && (x & 7) == (x1 & 7)) {
                  CirrusLatchedBitBlt(x, y, x1, y1, w, h, widthDst);
                  continue;
              }

              /* Call Cirrus framebuffer memcpy routine for remaining */
              /* forward blits. */
              CirrusSimpleBitBlt(x, y, x1, y1, w, h, widthDst);
              continue;
          }

	  if (xdir == 1 && ydir == -1) {
              /* Special case for reversed extended write mode bitblt */
              /* (scroll down). */
              if (w >= 32 && (x & 7) == (x1 & 7)) {
                  CirrusLatchedBitBltReversed(x, y, x1, y1, w, h, -widthDst);
                  continue;
              }
          }

          /* Let cfb do remaining (non-forward) blits. */
          vgaBitBlt(pdstBase, psrcBase, widthSrc, widthDst, x, y,
	      x1, y1, w, h, xdir, ydir, GXcopy, 0xff);
	  continue;
      }

      /* We have a hardware BitBLT engine. */
      /* For small areas, use the Cirrus framebuffer memcpy routine. */
      if (w * h < 100) {

	  if (cirrusBLTisBusy)
              /* We can't access video memory during a blit. */
	      CirrusBLTWaitUntilFinished();

          if (xdir == 1 && ydir == 1) {
              CirrusSimpleBitBlt(x, y, x1, y1, w, h, widthDst);
              continue;
          }
          vgaBitBlt(pdstBase, psrcBase, widthSrc, widthDst, x, y,
              x1, y1, w, h, xdir, ydir, GXcopy, 0xff);
          continue;
      }

      /* Use the hardware blit. */
      /* The line-by-line blits can probably be largely avoided similar to */
      /* the paradise/wd driver. -- HH */

      bltWidthSrc = widthSrc < 0 ? -widthSrc : widthSrc;
      bltWidthDst = widthDst < 0 ? -widthDst : widthDst;

      bltxdir = xdir;
      if (xdir == 1 && ydir == -1 && y != y1) {
          /* Tranform to xdir = -1 blit. */
          bltxdir = -1;
      }

      if (bltxdir == 1)		/* left to right */
	{
	  if (ydir == 1)	/* top to bottom */
	    {
	      psrc = (y * bltWidthSrc) + x;
	      pdst = (y1 * bltWidthDst) + x1;
	    }
	  else
	    /* bottom to top */
	    {
	      psrc = ((y + h - 1) * bltWidthSrc) + x;
	      pdst = ((y1 + h - 1) * bltWidthDst) + x1;
	    }
	}
      else
	/* right to left */
	{
	  if (ydir == 1)	/* top to bottom */
	    {
	      psrc = (y * bltWidthSrc) + x + w - 1;
	      pdst = (y1 * bltWidthDst) + x1 + w - 1;
	    }
	  else
	    /* bottom to top */
	    {
	      psrc = ((y + h - 1) * bltWidthSrc) + x + w - 1;
	      pdst = ((y1 + h - 1) * bltWidthDst) + x1 + w - 1;
	    }
	}

      /* I could probably do the line by line */
      /* blits a little faster by breaking the */
      /* blit regions into rectangles */
      /* and blitting those, making sure I don't */
      /* overwrite stuff. However, the */
      /* difference between the line by line */
      /* and block blits isn't noticable to */
      /* me, so I think I'll blow it off. */
      
      /* HH: Hmm, it is very noticable; moving up while editing is */
      /* significantly slower (this is now fixed). */

      cirrusDoBackgroundBLT = TRUE;

      if (bltxdir == 1)
	{
	  if (ydir == 1)
	    {			/* Nothing special, straight blit */
	      if (cirrusUseMMIO)
	          CirrusMMIOBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, 1);
	      else
	          CirrusBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, 1);
	    }
	  else
	    /* Line by line, going up. */
	    {
	      int bltpsrc, bltpdst;
	      bltpsrc = psrc;
	      bltpdst = pdst;
	      for (i = 0; i < h; i++)
		{
		  if (cirrusUseMMIO)
		      CirrusMMIOBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, 1);
		  else
		      CirrusBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, 1);
		  bltpsrc -= bltWidthSrc;
		  bltpdst -= bltWidthDst;
		}
	    }
	}
      else
	{

	  if (ydir == 1)	/* Line by line, going down and to the left */
	    {
	      int bltpsrc, bltpdst;
	      bltpsrc = psrc;
	      bltpdst = pdst;
	      for (i = 0; i < h; i++)
		{
		  if (cirrusUseMMIO)
		      CirrusMMIOBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, -1);
		  else
		      CirrusBLTBitBlt(bltpdst, bltpsrc, bltWidthDst, bltWidthSrc, w, 1, -1);
		  bltpsrc += bltWidthSrc;
		  bltpdst += bltWidthDst;
		}
	    }
	  else
	    /* Another stock blit, albeit backwards */
	    {
	      if (cirrusUseMMIO)
	          CirrusMMIOBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, -1);
	      else
	          CirrusBLTBitBlt(pdst, psrc, bltWidthDst, bltWidthSrc, w, h, -1);
	    }
	}

      /* Next region. */
  }

  /* Make sure any background blits are finished. */
  if (cirrusBLTisBusy)
      CirrusBLTWaitUntilFinished();
  cirrusDoBackgroundBLT = FALSE;
}
