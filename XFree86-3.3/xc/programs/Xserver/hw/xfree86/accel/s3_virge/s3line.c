/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3line.c,v 3.11.2.3 1997/05/24 08:36:01 dawes Exp $ */
/*

Copyright (c) 1987  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL AND KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
IN NO EVENT SHALL DIGITAL OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL,
INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

Modified for the 8514/A by Kevin E. Martin (martin@cs.unc.edu)

*/

/*
 * Modified by Amancio Hasty and Jon Tombs
 *
 */
/* $XConsortium: s3line.c /main/8 1996/10/27 18:07:10 kaleb $ */


#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"
#include "miline.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "misc.h"
#include "xf86.h"
#include "s3v.h"

static __inline__ int double2int(double d)
{
#ifndef MAXINT
#define MAXINT 0x7fffffff
#endif
   if (d > (double)(MAXINT))
      return MAXINT;
   if (d < -(double)(MAXINT))
      return -MAXINT;
   return (int)(d);
}

/* cmd pack to draw a line, "long" is 32-bit -hu */
static void _s3drwline (x1, x2, delx, xstart, ystart, ycnt)
unsigned int  x1;
unsigned int  x2;
unsigned long delx;
unsigned long xstart;
unsigned long ystart;
unsigned long ycnt;
{
	WaitQueue(5);
	SETL_LXEND0_END1(x1, x2);
	SETL_LDX(delx);
	SETL_LXSTART(xstart);
	SETL_LYSTART(ystart);
	SETL_LYCNT(ycnt);
}

/*++ merged simple H/V dashed-line stuff -hu ++*/
#define NextDash {\
    dashIndexTmp++; \
    if (dashIndexTmp == numInDashList) \
        dashIndexTmp = 0; \
    dashRemaining = pDash[dashIndexTmp]; \
    thisDash = dashRemaining; \
    }

#define FillDashPat {\
   int i; \
\
   for (i = 0; i < 16; i++) {\
      dashPat <<= 1;\
      if (tmp + i < len) {\
	 if (!(dashIndexTmp & 1))\
	    dashPat |= 1;\
	 if (--thisDash == 0)\
	    NextDash\
      }\
   }\
}

#define s3_dashlineV { \
	int len, pixon; \
	unsigned short mask; \
	/* use tmp dash index and offsets */ \
	dashIndexTmp = dashIndex; \
	dashOffsetTmp = dashOffset; \
 \
	if (y1t != y1) { /* advance the dash index */ \
	  miStepDash (y1t - y1, &dashIndexTmp, pDash, \
		      numInDashList, &dashOffsetTmp); \
	} \
	dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp; \
	thisDash = dashRemaining ; \
	len = y2t - y1t; \
	pixon = 0; \
	y2t = y1t - 1; \
	for (tmp = 0 ; tmp < len; tmp+=16) { \
	  FillDashPat; \
	  mask = 0x8000; \
	  while (mask) { \
	    if (dashPat & mask) { \
		if (pixon == 0) y2t++; \
		pixon++; \
	    } \
	    else { \
		if (pixon) { \
		  _s3drwline (x1, x1, 0L, (long)(x1 << 20), \
			      (long)(y2t+pixon-1), pixon); \
		  y2t += (pixon - 1); \
		} \
		pixon = 0; \
		y2t++; \
	    } \
	    mask >>= 1; \
	  } \
	} \
	if (pixon) { \
	  if ((tmp = pixon-1) <= 0) tmp = 1; \
	  _s3drwline (x1, x1, 0L, (long)(x1<<20),\
		      (long)(y2t+pixon-1), tmp);\
	} \
}

#define s3_dashlineH { \
	int len, pixon; \
	unsigned short mask; \
\
	dashIndexTmp = dashIndex; \
	dashOffsetTmp = dashOffset; \
\
	if (x1t != x1) { /* advance the dash index */ \
	     miStepDash (x1t - x1, &dashIndexTmp, pDash, \
			 numInDashList, &dashOffsetTmp); \
	} \
	dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp; \
	thisDash = dashRemaining ; \
	len = x2t - x1t; \
	x1t--; \
	pixon = 0; \
	for (tmp = 0 ; tmp < len; tmp+=16) { \
	  FillDashPat; \
	  mask = 0x8000; \
	  while (mask) { \
	    if ((dashPat & mask) == 0) { \
	      if (pixon) { \
		_s3drwline (x1t, x1t+pixon-1, 0L, (long)(x1t << 20), \
			    (long)y1, 0x80000001); \
		x1t += pixon - 1; \
	      } \
	      x1t++; \
	      pixon = 0; \
	    } \
	    else { \
	      if (pixon == 0) x1t++; \
	      pixon++; \
	    } \
	    mask >>= 1; \
	  } \
	} \
	if (pixon) { \
	  _s3drwline (x1t, x1t+pixon-1, 0L, (long)(x1t << 20), \
		      (long)y1, 0x80000001); \
	} \
}


void
#ifndef POLYSEGMENT
s3Line(pDrawable, pGC, mode, npt, pptInit)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   mode;		/* Origin or Previous */
     int   npt;			/* number of points */
     DDXPointPtr pptInit;
#else /* POLYSEGMENT */
s3Segment(pDrawable, pGC, nseg, pSeg)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   nseg;
     register xSegment *pSeg;
#endif /* POLYSEGMENT */
{
   int   nboxInit;
   register int nbox;
   BoxPtr pboxInit;
   register BoxPtr pbox;
#ifndef POLYSEGMENT
   register DDXPointPtr ppt;	/* pointer to list of translated points */
#endif /* not POLYSEGMENT */

   unsigned int oc1;		/* outcode of point 1 */
   unsigned int oc2;		/* outcode of point 2 */

   int   xorg, yorg;		/* origin of window */

   int   adx;			/* abs values of dx and dy */
   int   ady;
   int   signdx;		/* sign of dx and dy */
   int   signdy;
   int   len;			/* length of segment */
   int   axis;			/* major axis */
   int   octant;
   unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);

   /* a bunch of temporaries */
   int   tmp;
   register int y1, y2;
   register int x1, x2;
   RegionPtr cclip;
   cfbPrivGCPtr devPriv;
   int s3_clr, s3_rop;

   unsigned char *pDash;
   int   dashOffset;
   int numInDashList;
   int dashIndex;
   int dashIndexTmp, dashOffsetTmp, thisDash, dashRemaining;
   int unclippedlen;
   short dashPat = 0;

   if (!pGC->planemask)  /* for xgc "benchmarks" ;-) */
      return;

   s3_rop = s3ConvertPlanemask(pGC, &s3_clr);

   if (!xf86VTSema || (s3_rop == -1) /*|| (pGC->planemask & s3BppPMask) != s3BppPMask*/) {
      if (xf86VTSema) WaitIdleEmpty();
#ifndef POLYSEGMENT
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbLineSS(pDrawable, pGC, mode, npt, pptInit);
	 break;
      case 16:
	 cfb16LineSS(pDrawable, pGC, mode, npt, pptInit);
	 break;
      case 24:
	 cfb24LineSS(pDrawable, pGC, mode, npt, pptInit);
	 break;
      case 32:
	 cfb32LineSS(pDrawable, pGC, mode, npt, pptInit);
	 break;
      }
#else /* POLYSEGMENT */
      switch (s3InfoRec.bitsPerPixel) {
      case 8:
	 cfbSegmentSS(pDrawable, pGC, nseg, pSeg);
         break;
      case 16:
	 cfb16SegmentSS(pDrawable, pGC, nseg, pSeg);
         break;
      case 24:
	 cfb24SegmentSS(pDrawable, pGC, nseg, pSeg);
         break;
      case 32:
	 cfb32SegmentSS(pDrawable, pGC, nseg, pSeg);
	 break;
      }
#endif /* POLYSEGMENT */
      if (xf86VTSema) WaitIdleEmpty();
      return;
   }

   devPriv = (cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr);
   cclip = devPriv->pCompositeClip;
   pboxInit = REGION_RECTS(cclip);
   nboxInit = REGION_NUM_RECTS(cclip);

   BLOCK_CURSOR;
   ;SET_WRT_MASK(pGC->planemask);

#if 0
   /* Fix problem writing to the cursor storage area */
   WaitQueue(4);
   SETL_CLIP_L_R(0, pDrawable->pScreen->width-1);
   SETL_CLIP_T_B(0, pDrawable->pScreen->height-1);
#else
   WaitQueue(2);
#endif
   SETL_PAT_FG_CLR(s3_clr);
   SETL_CMD_SET(s3_gcmd | CMD_LINE | MIX_MONO_PATT | CMD_AUTOEXEC | s3_rop);

   xorg = pDrawable->x;
   yorg = pDrawable->y;

#ifndef POLYSEGMENT
   ppt = pptInit;
   x2 = ppt->x + xorg;
   y2 = ppt->y + yorg;
#endif /* not POLYSEGMENT */

   if (pGC->lineStyle == LineOnOffDash  ||
       pGC->lineStyle == LineDoubleDash) {
     pDash = (unsigned char *) pGC->dash;
     numInDashList = pGC->numInDashList;

     dashIndex = 0;
     dashOffset = 0;
     miStepDash ((int)pGC->dashOffset, &dashIndex, pDash,
                  numInDashList, &dashOffset);

     dashRemaining = pDash[dashIndex] - dashOffset;
     thisDash = dashRemaining ;
   }

#ifndef POLYSEGMENT
   while (--npt) {
#else /* POLYSEGMENT */
   while (nseg--) {
#endif /* POLYSEGMENT */

      nbox = nboxInit;
      pbox = pboxInit;

#ifndef POLYSEGMENT
      x1 = x2;
      y1 = y2;
      ++ppt;
      if (mode == CoordModePrevious) {
	 xorg = x1;
	 yorg = y1;
      }
      x2 = ppt->x + xorg;
      y2 = ppt->y + yorg;
#else /* POLYSEGMENT */
      x1 = pSeg->x1 + xorg;
      y1 = pSeg->y1 + yorg;
      x2 = pSeg->x2 + xorg;
      y2 = pSeg->y2 + yorg;
      pSeg++;
#endif /* POLYSEGMENT */

      if (x1 == x2) {

	 /*
	  * make the line go top to bottom of screen, keeping endpoint
	  * semantics
	  */
	 if (y1 > y2) {
	    register int tmp;

	    tmp = y2;
	    y2 = y1 + 1;
	    y1 = tmp + 1;
#ifndef POLYSEGMENT
	 }
#else /* POLYSEGMENT */
	    if (pGC->capStyle != CapNotLast)
	       y1--;
	 } else if (pGC->capStyle != CapNotLast)
	    y2++;
#endif /* POLYSEGMENT */

	 /* get to first band that might contain part of line */
	 while ((nbox) && (pbox->y2 <= y1)) {
	    pbox++;
	    nbox--;
	 }

	 if (nbox) {
	    /* stop when lower edge of box is beyond end of line */
	    while ((nbox) && (y2 >= pbox->y1)) {
	       if ((x1 >= pbox->x1) && (x1 < pbox->x2)) {
		  int   y1t, y2t;

		  /* this box has part of the line in it */
		  y1t = max(y1, pbox->y1);
		  y2t = min(y2, pbox->y2);
		  if (y1t != y2t) {

		    if (pGC->lineStyle == LineOnOffDash  ||
			pGC->lineStyle == LineDoubleDash) {
		      s3_dashlineV;
		    }
		    else {
                      /* Since the ViRGE draws from bottom to top, I draw
                         from x2,y2 to x1,y1, where x2,y2 is skipped. */
#if 0
		      WaitQueue(5);
		      SETL_LXEND0_END1(x1, x1);
		      SETL_LDX(0);  /* dX == 0 */
		      SETL_LXSTART(x1 << 20);
		      SETL_LYSTART(y2t - 1);
		      SETL_LYCNT(y2t - y1t);
#endif
		      _s3drwline (x1, x1, 0L, (long)(x1<<20),
				 (long)(y2t-1), y2t-y1t);
		    }
		  }
	       }
	       nbox--;
	       pbox++;
	    }
	 }
#ifndef POLYSEGMENT
	 y2 = ppt->y + yorg;
#endif /* not POLYSEGMENT */

      } else if (y1 == y2) {

	 /*
	  * force line from left to right, keeping endpoint semantics
	  */
	 if (x1 > x2) {
	    register int tmp;

	    tmp = x2;
	    x2 = x1 + 1;
	    x1 = tmp + 1;
#ifndef POLYSEGMENT
	 }
#else /* POLYSEGMENT */
	    if (pGC->capStyle != CapNotLast)
	       x1--;
	 } else if (pGC->capStyle != CapNotLast)
	    x2++;
#endif /* POLYSEGMENT */

	 /* find the correct band */
	 while ((nbox) && (pbox->y2 <= y1)) {
	    pbox++;
	    nbox--;
	 }

	 /* try to draw the line, if we haven't gone beyond it */
	 if ((nbox) && (pbox->y1 <= y1)) {
	    /* when we leave this band, we're done */
	    tmp = pbox->y1;
	    while ((nbox) && (pbox->y1 == tmp)) {
	       int   x1t, x2t;

	       if (pbox->x2 <= x1) {
		  /* skip boxes until one might contain start point */
		  nbox--;
		  pbox++;
		  continue;
	       }
	       /* stop if left of box is beyond right of line */
	       if (pbox->x1 >= x2) {
		  nbox = 0;
		  break;
	       }
	       x1t = max(x1, pbox->x1);
	       x2t = min(x2, pbox->x2);
	       if (x1t != x2t) {
		 if (pGC->lineStyle == LineOnOffDash  ||
		     pGC->lineStyle == LineDoubleDash) {
		   s3_dashlineH;
		 }
		 else {
                   /* Skip x2,y2 */
#if 0
		   WaitQueue(5);
		   SETL_LXEND0_END1(x1t, x2t-1);
		   SETL_LDX(0);	/* dY == 0 */
		   SETL_LXSTART(x1t << 20);
		   SETL_LYSTART(y1);
		   SETL_LYCNT(1 | 0x80000000);
#endif
		   _s3drwline (x1t, x2t-1, 0L, (long)(x1t<<20),
			       y1, 0x80000001);
		 }
	       }
	       nbox--;
	       pbox++;
	    }
	 }
#ifndef POLYSEGMENT
	 x2 = ppt->x + xorg;
#endif /* not POLYSEGMENT */

      } else {			/* sloped line */
	 int xdelta, xfixup, xdir;

	 adx = x2 - x1;
	 ady = y2 - y1;
	 CalcLineDeltas(x1, y1, x2, y2, adx, ady, signdx, signdy,
			1, 1, octant);

	 if (adx > ady) {
	    axis = X_AXIS;
	 } else {
	    axis = Y_AXIS;
	    SetYMajorOctant(octant);
	 }

	 if (adx > 2047) /* avoid integer overflow */
	    xdelta = -double2int((double)(adx) * (1<<20) / (double)(ady));
	 else
	    xdelta = -(((adx) << 20)) / (ady);

	 if ((x1 < x2) ^ (y1 < y2)) {
	    xdelta = -xdelta;
	    xdir = 0x80000000;
	 } 
	 else
	    xdir = 0;

	 if (axis == Y_AXIS)
	    if (xdelta >= 0)
	       xfixup = 0x7ffff;
	    else
	       xfixup = 0x80000;
	 else
	    if (xdelta > 0)
	       xfixup = (xdelta-1) >> 1;
	    else {
	       xdelta--;
	       xfixup = ((xdelta+1) >> 1) + ((1<<20) - 1);
	    }

	 /*
	  * we have bresenham parameters and two points. all we have to do now
	  * is clip and draw.
	  */

	 while (nbox--) {
	    int n,xss,xs,xe,ys;

	    oc1 = 0;
	    oc2 = 0;
	    OUTCODES(oc1, x1, y1, pbox);
	    OUTCODES(oc2, x2, y2, pbox);
	    if ((oc1 | oc2) == 0) {
	       len = ady;

	       /*
		* NOTE:  The ViRGE hardware routines for generating lines may
		* not match the software generated lines of mi, cfb, and mfb
		* because it's using simple DDA instead of bresenham.
		*/
	       ;SET_CURPT((short)x1, (short)y1);
	       ;SET_ERR_TERM((short)(e + fix));
	       ;SET_DESTSTP((short)e2, (short)e1);
	       ;SET_MAJ_AXIS_PCNT((short)len);
	       ;SET_CMD(cmd2);

	       if (y1 > y2) {
		  ys = y1;
		  xs = x1;
		  xss = (x1 << 20) + xfixup;
#ifdef POLYSEGMENT
		  if (pGC->capStyle != CapNotLast)
		     xe = x2;
		  else
#endif /* POLYSEGMENT */
		     if (xdir)
			xe = x2 - 1;
		     else
			xe = x2 + 1;
	       } else {
		  ys = y2;
		  xe = x1;
		  xss = (x2 << 20) + xfixup;
#ifdef POLYSEGMENT
		  if (pGC->capStyle != CapNotLast)
		     xs = x2;
		  else
#endif /* POLYSEGMENT */
		     if (xdir)
			xs = x2 + 1;
		     else
			xs = x2 - 1;
	       }

	       /* split long lines 
		* 
		* ViRGE vector generator uses DDA instead of bresenham and
		* for long sloped lines (> 500 scanlines) rounding error can result
		* in wrong pixels being drawn.  longer lines are split in 
		* multple parts and starting points fraction is set so that
		* parts will join exactly (mostly experimental values)
		*/

#define LEN 250
	       if (len <= LEN) { /* use old code to avoid FP stuff for short lines */
#if 0
		  WaitQueue(5);
		  SETL_LXEND0_END1(xs, xe);
		  SETL_LDX(xdelta);
		  SETL_LXSTART(xss);
		  SETL_LYSTART(ys);
		  SETL_LYCNT((len+1) | xdir);
#endif
		  _s3drwline (xs, xe, xdelta, xss, ys, ((len+1) | xdir));
	       }
	       else {
		  double xd2 = -(double)(x2-x1) * (1<<20) / (double)(y2-y1);

		  for (n=0; len>0; n++, len -= LEN) {
		     WaitQueue(5);
		     SETL_LXEND0_END1(n==0 ? xs :
				      (xss + double2int(n * (LEN * xd2))) >> 20,
				      len <= LEN ? xe : 
				      (((xss + double2int((n+1) * (LEN * xd2))) >> 20)
				       + (xdir ? -1 : 1)));
		     SETL_LDX(xdelta);
		     SETL_LXSTART(xss + double2int(n * (LEN * xd2)) - (axis != Y_AXIS && xd2 < 0));
		     SETL_LYSTART(ys - n * LEN);
		     if (len > LEN)
			SETL_LYCNT((LEN+1) | xdir);
                     else
			SETL_LYCNT((len+1) | xdir);
		  }
	       }
	       break;
	    } else if (oc1 & oc2) {
	       pbox++;
	    } else {

	       /*
		* let the mi helper routine do our work; better than
		* duplicating code...
		*/
	       int   clip1=0, clip2=0; /* clippedness of the endpoints */
	       int new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;

               if (miZeroClipLine(pbox->x1, pbox->y1,
				  pbox->x2-1, pbox->y2-1,
				  &new_x1, &new_y1,
				  &new_x2, &new_y2,
				  adx, ady,
				  &clip1, &clip2,
				  octant, bias,
				  oc1, oc2) == -1)
		  {
		     pbox++;
		     continue;
		  }
	       if (axis == X_AXIS)
		  len = abs(new_x2 - new_x1);
	       else
		  len = abs(new_y2 - new_y1);
#ifndef POLYSEGMENT
	       len += (clip2 != 0);
#else /* POLYSEGMENT */
	       if (clip2 != 0 || pGC->capStyle != CapNotLast)
		  len++;
#endif /* POLYSEGMENT */

	       if (len) {
		  int yofs;

		  len = abs(new_y2 - new_y1);

		  ;SET_CURPT((short)new_x1, (short)new_y1);
		  ;SET_ERR_TERM((short)(err + fix));
		  ;SET_DESTSTP((short)e2, (short)e1);
		  ;SET_MAJ_AXIS_PCNT((short)len);
		  ;SET_CMD(cmd2);
		  
		  if (y1 > y2) {
		     xss = (new_x1 << 20) + xfixup;
#ifndef POLYSEGMENT
		     if (new_x2 != x2 || new_y2 != y2)
#else /* POLYSEGMENT */
		     if (new_x2 != x2 || new_y2 != y2 || pGC->capStyle != CapNotLast)
#endif /* POLYSEGMENT */
			xe = new_x2;
		     else if (xdir)
			xe = new_x2 - 1;
		     else
			xe = new_x2 + 1;
		     ys = new_y1;
		     xs = new_x1;
		     yofs = y1 - new_y1;
		  } else {
		     ys = new_y2;
		     xe = new_x1;
		     xss = (new_x2 << 20) + xfixup;
#ifndef POLYSEGMENT
		     if (new_x2 != x2 || new_y2 != y2)
#else /* POLYSEGMENT */
		     if (new_x2 != x2 || new_y2 != y2 || pGC->capStyle != CapNotLast)
#endif /* POLYSEGMENT */
			xs = new_x2;
		     else if (xdir)
			xs = new_x2 + 1;
		     else
			xs = new_x2 - 1;
		     yofs = y2 - new_y2;
		  }		  

		  if (len <= LEN) { /* use old code to avoid FP stuff for short lines */
		     WaitQueue(5);
		     SETL_LXEND0_END1(xs, xe);
		     SETL_LDX(xdelta);
		     if (0 && yofs == 0) 
			SETL_LXSTART(xss);
		     else {
			double xd2 = -(double)(x2-x1) * (1<<20) / (double)(y2-y1);
			if (y1 > y2)
			   xss = (x1 << 20) + xfixup;
			else 
			   xss = (x2 << 20) + xfixup;
			SETL_LXSTART(xss + double2int(yofs * xd2) - (axis != Y_AXIS && xd2 < 0));
		     }
		     SETL_LYSTART(ys);
		     SETL_LYCNT((len+1) | xdir);
		  }
		  else {
		     double xd2 = -(double)(x2-x1) * (1<<20) / (double)(y2-y1);

		     if (y1 > y2)
			xss = (x1 << 20) + xfixup;
		     else 
			xss = (x2 << 20) + xfixup;

		     for (n=0; len+(new_y1 ==  new_y2) > 0; n++, len -= LEN) {
			WaitQueue(5);
			SETL_LXEND0_END1(n==0 ? xs :
					 (xss + double2int((n * LEN + yofs) * xd2)) >> 20,
					 len <= LEN ? xe : 
					 ((xss + double2int(((n+1) * LEN + yofs) * xd2)) >> 20)
					 + (xdir ? -1 : 1));
			SETL_LDX(xdelta);
			SETL_LXSTART(xss + double2int((n * LEN + yofs) * xd2) - (axis != Y_AXIS && xd2 < 0));
			SETL_LYSTART(ys - n * LEN);
			if (len > LEN)
			   SETL_LYCNT((LEN+1) | xdir);
			else
			   SETL_LYCNT((len+1) | xdir);
		     }
		  }
	       }
	       pbox++;
	    }
	 } /* while (nbox--) */
      }	/* sloped line */
   } /* while (nline--) */

#ifndef POLYSEGMENT
   /*
    * paint the last point if the end style isn't CapNotLast. (Assume that a
    * projecting, butt, or round cap that is one pixel wide is the same as the
    * single pixel of the endpoint.)
    */

   if ((pGC->capStyle != CapNotLast) &&
       ((ppt->x + xorg != pptInit->x + pDrawable->x) ||
	(ppt->y + yorg != pptInit->y + pDrawable->y) ||
	(ppt == pptInit + 1))) {
      nbox = nboxInit;
      pbox = pboxInit;
      WaitQueue(1);
      SETL_LDX(0);
      while (nbox--) {
	 if ((x2 >= pbox->x1) &&
	     (y2 >= pbox->y1) &&
	     (x2 < pbox->x2) &&
	     (y2 < pbox->y2)) {
	    WaitQueue(4);
	    SETL_LXEND0_END1(x2, x2);
	    SETL_LXSTART(x2 << 20);
	    SETL_LYSTART(y2);
	    SETL_LYCNT(1);

	    break;
	 } else
	    pbox++;
      }
   }
#endif /* not POLYSEGMENT */

#if 0
   WaitQueue(7);
   SETL_CLIP_L_R(0, s3ScissR);
   SETL_CLIP_T_B(0, s3ScissB);
#else
   WaitQueue(5);
#endif
   SETL_CMD_SET(CMD_NOP);

   /* avoid system hangs again :-( */
   SETB_RSRC_XY(0,0);
   SETB_RDEST_XY(0,0);
   SETB_RWIDTH_HEIGHT(0,1);
   SETB_CMD_SET(s3_gcmd | CMD_BITBLT | ROP_S);
   WaitIdle();

   UNBLOCK_CURSOR;
}
