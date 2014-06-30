/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxDSeg.c,v 3.7 1996/12/23 06:32:36 dawes Exp $ */
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
/* $XConsortium: agxDSeg.c /main/6 1996/02/21 17:17:02 kaleb $ */

/*s3 dline.c from s3line.c with help from cfbresd.c and cfbline.c - Jon */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"
#include "miline.h"

#include "cfb.h"
#include "cfbmskbits.h"
#include "misc.h"
#include "xf86.h"
#include "agx.h"
#include "regagx.h"

#define NextDash {\
    if (++dashIndexTmp == numInDashList) \
        dashIndexTmp = 0; \
    dashRemaining = pDash[dashIndexTmp]; \
    thisDash = dashRemaining; \
    }

#define FillDashPat {\
   register int i; \
   for (i = 0; i < 32; i++) {\
      dashPat >>= 1;\
      if (tmp + i < len) {\
	 if (!(dashIndexTmp & 1))\
	    dashPat |= 0x80000000;\
	 if (--thisDash == 0)\
	    NextDash\
      }\
   }\
}
/*
 * Dashed lines through the graphics engine.
 * Known Bugs: Jon 13/9/93
 * - Dash offset isn't caclulated correctly for clipped lines. [fixed?]
 * - Dash offset isn't updated correctly. [fixed?]
 * - Dash patters which are a power of 2 and < 16 can be done faster through
 *   the color compare register.
 * - DoubleDashed lines are are probably very incorrect.
 * - line caps are possible wrong too.
 * - Caclulating the dashes could probably be done more optimally,
 *   e.g. We could producing the pattern stipple before hand?
 */
void
agxDSegment(pDrawable, pGC, nseg, pSeg)
     DrawablePtr pDrawable;
     GCPtr pGC;
     int   nseg;
     register xSegment *pSeg;
{
   int   nboxInit;
   int nbox;
   BoxPtr pboxInit;
   BoxPtr pbox;
   DDXPointPtr ppt;	/* pointer to list of translated points */

   unsigned int oc1;		/* outcode of point 1 */
   unsigned int oc2;		/* outcode of point 2 */

   int   xorg, yorg;		/* origin of window */

   int   adx;			/* abs values of dx and dy */
   int   ady;
   int   signdx;		/* sign of dx and dy */
   int   signdy;
   int   e, e1, e2;		/* bresenham error and increments */
   int   len;			/* length of segment */
   int   axis;			/* major axis */
   int   octant;
   unsigned int bias = miGetZeroLineBias(pDrawable->pScreen);
   int   cmd2;
   unsigned char *pDash;
   int numInDashList;
   int dashOffset = 0;   
   int dashIndex = 0;
   int dashIndexTmp = 0;
   int dashOffsetTmp, thisDash, dashRemaining;
   unsigned int dashPat;
 /* a bunch of temporaries */
   int   tmp;
   register int y1, y2;
   register int x1, x2;
   RegionPtr cclip;
   cfbPrivGCPtr devPriv;
   short fix;

   if (!xf86VTSema)
   {
      switch (agxInfoRec.bitsPerPixel) {
        case 8:
           cfbSegmentSD(pDrawable, pGC, nseg, pSeg);
           break;
        case 16:
           cfb16SegmentSD(pDrawable, pGC, nseg, pSeg);
           break;
#ifdef AGX_32BPP
        case 32:
           cfb32SegmentSD(pDrawable, pGC, nseg, pSeg);
           break;
#endif
      }
      return;
   }

   devPriv = (cfbPrivGC *) (pGC->devPrivates[cfbGCPrivateIndex].ptr);
   cclip = devPriv->pCompositeClip;
   pboxInit = REGION_RECTS(cclip);
   nboxInit = REGION_NUM_RECTS(cclip);

   pDash = (unsigned char *) pGC->dash;
   numInDashList = pGC->numInDashList;
   dashIndex = 0;
   dashIndexTmp = 0;
   dashOffset = 0;
   miStepDash ((int)pGC->dashOffset, &dashIndex, pDash,
               numInDashList, &dashOffset);

   xorg = pDrawable->x;
   yorg = pDrawable->y;

   MAP_INIT( GE_MS_MAP_B, 
             GE_MF_1BPP | GE_MF_INTEL_FORMAT,
             agxMemBase + agxScratchOffset, 
             0xFFFF, 
             0,
             FALSE, FALSE, FALSE );

   GE_WAIT_IDLE();

   MAP_SET_DST( GE_MS_MAP_A );
   GE_SET_MAP( GE_MS_MAP_B );

   outb( agxApIdxReg, (agxScratchOffset >> 16) );  

   GE_OUT_B(GE_FRGD_MIX, pGC->alu);
   GE_OUT_D(GE_PIXEL_BIT_MASK, pGC->planemask);
   GE_OUT_D(GE_FRGD_CLR, pGC->fgPixel);

   if (pGC->lineStyle == LineDoubleDash) {
      GE_OUT_B(GE_BKGD_MIX, pGC->alu);
      GE_OUT_D(GE_BKGD_CLR, pGC->bgPixel);
   }
   else {
      GE_OUT_B(GE_BKGD_MIX, MIX_DST);
   }

   GE_OUT_W( GE_PIXEL_OP,
             GE_OP_PAT_MAP_B
             | GE_OP_MASK_DISABLED
             | GE_OP_INC_X
             | GE_OP_INC_Y         );

   while (nseg--) {
      nbox = nboxInit;
      pbox = pboxInit;

      x1 = pSeg->x1 + xorg;
      y1 = pSeg->y1 + yorg;
      x2 = pSeg->x2 + xorg;
      y2 = pSeg->y2 + yorg;
      pSeg++;

      /* The following code doesn't work as it can change the direction of
       * the line in order to simplify the cliping. Dashed lines need to
       * be drawn in the order given in order to caclulate the dash offset
       * correctly.
       */

#ifdef fastaxislinesfixed 
      if (x1 == x2) {

	 if (y2 >= y1) {
	    direction |= INC_X;
	 }
	 
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
		     /* use tmp dash index and offsets */
		     dashIndexTmp = dashIndex;    
		     dashOffsetTmp = dashOffset; 
		     if (y1t != y1) { /* advance the dash index */
			miStepDash (y1t - y1, &dashIndexTmp, pDash,
				 numInDashList, &dashOffsetTmp);
		     }
		     dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp;
		     thisDash = dashRemaining ;

                     {
                       register unsigned int opDim = (len-1) << 16;
                       register unsigned int mapCoOrd = y1t << 16 | x1;
                       register volatile unsigned int *pattern 
                          = (unsigned int*)agxVideoMem;
 
                       GE_WAIT_IDLE();
	               for (tmp = 0 ; tmp <= len; tmp+=32) {
		           FillDashPat;			
                           *pattern++ = dashPat; 
		       }		  
                       GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                       GE_OUT_D( GE_PAT_MAP_X, 0 );
                       GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                       GE_START_CMDW( GE_OPW_BITBLT
                                      | GE_OPW_FRGD_SRC_CLR
                                      | GE_OPW_BKGD_SRC_CLR
                                      | GE_OPW_SRC_MAP_A
                                      | GE_OPW_DEST_MAP_A   );
                     }
		  }
	       }
	       nbox--;
	       pbox++;
	    }
	 }
      } else if (y1 == y2) {

       /*
        * force line from left to right, keeping endpoint semantics
        */
	 if (x1 > x2) {
	    register int tmp;

	    tmp = x2;
	    x2 = x1 + 1;
	    x1 = tmp + 1;
	 }
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
		  dashIndexTmp = dashIndex;    
		  dashOffsetTmp = dashOffset; 
		  if (x1t != x1) { /* advance the dash index */
		     miStepDash (x1t - x1, &dashIndexTmp, pDash,
				 numInDashList, &dashOffsetTmp);
		  }
		  dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp;
		  thisDash = dashRemaining ;

                  {
                    register unsigned int mapCoOrd y1 << 16 | x1t;
                    register volatile unsigned int *pattern 
                       = (unsigned int *)agxVideoMem;
                     
                    GE_WAIT_IDLE();
                    for (tmp = 0 ; tmp <= len; tmp+=32) {
                        FillDashPat;                 
                        *pattern++ = dashPat; 
                    }                  
                    GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                    GE_OUT_D( GE_PAT_MAP_X, 0 );
                    GE_OUT_D( GE_OP_DIM_WIDTH, len-1 );
                    GE_START_CMDW( GE_OPW_BITBLT
                                   | GE_OPW_FRGD_SRC_CLR
                                   | GE_OPW_BKGD_SRC_CLR
                                   | GE_OPW_SRC_MAP_A
                                   | GE_OPW_DEST_MAP_A   );
                   }
		 } 
	       }
	       nbox--;
	       pbox++;
	    }
      }
      else
#endif
      {			/* sloped line */
         cmd2 = 0;
	 fix = -1;
	 if ((adx = x2 - x1) < 0) {
	    fix = 0;
            cmd2 |= GE_OP_DEC_X;
	 } 
	 if ((ady = y2 - y1) < 0) {
            cmd2  |= GE_OP_DEC_Y;
	 } 
	 CalcLineDeltas(x1, y1, x2, y2, adx, ady, signdx, signdy,
			1, 1, octant);

	 if (adx > ady) {
	    axis = X_AXIS;
	    e1 = ady << 1;
	    e2 = e1 - (adx << 1);
	    e = e1 - adx;
	 } else {
	    axis = Y_AXIS;
	    e1 = adx << 1;
	    e2 = e1 - (ady << 1);
	    e = e1 - ady;
            cmd2 |= GE_OP_Y_MAJ;
	    SetYMajorOctant(octant);
	 }

	 FIXUP_ERROR(e, octant, bias);

       /*
        * we have bresenham parameters and two points. all we have to do now
        * is clip and draw.
        */

	 while (nbox--) {
	    oc1 = 0;
	    oc2 = 0;
	    OUTCODES(oc1, x1, y1, pbox);
	    OUTCODES(oc2, x2, y2, pbox);
	    if ((oc1 | oc2) == 0) {
	       if (axis == X_AXIS) 
		  len = adx;
	       else 
		  len = ady;

             if (pGC->capStyle != CapNotLast) {
               len++;
             }
	     dashIndexTmp = dashIndex;    
	     dashOffsetTmp = dashOffset;
	     /* No need to adjust dash offset */
	     dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp;
	     thisDash = dashRemaining ;
	     /*
	      * NOTE:  The 8514/A hardware routines for generating lines do
	      * not match the software generated lines of mi, cfb, and mfb.
	      * This is a problem, and if I ever get time, I'll figure out
	      * the 8514/A algorithm and implement it in software for mi,
	      * cfb, and mfb.
	      * 2-sep-93 TCG: apparently only change needed is
	      * addition of 'fix' stuff in cfbline.c
	      */
              {
                 register unsigned int mapCoOrd  = y1 << 16 | x1;
                 register volatile unsigned int *pattern 
                     = (unsigned int *)agxVideoMem;

                 GE_WAIT_IDLE();

                 for (tmp = 0 ; tmp <= len; tmp+=32) {
                    FillDashPat;                 
                    *pattern++ = dashPat; 
                 }         

                 GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                 GE_OUT_D( GE_PAT_MAP_X, 0 );
                 GE_OUT_W( GE_OP_DIM_LINE_MAJ, len-1 );
                 GE_OUT_W( GE_BRES_ERROR_TERM, e+fix );
                 GE_OUT_W( GE_BRES_CONST_K1,   e1 );
                 GE_OUT_W( GE_BRES_CONST_K2,   e2 );
                 GE_START_CMD( GE_OP_LINE_DRAW_WR
                               | GE_OP_FRGD_SRC_CLR
                               | GE_OP_SRC_MAP_A
                               | GE_OP_DEST_MAP_A
                               | GE_OP_DRAW_LAST_NULL
                               | GE_OP_PAT_MAP_B
                               | GE_OP_MASK_DISABLED
                               | cmd2 );
               }
	       break;
	    } else if (oc1 & oc2) {
	       pbox++;
	    } else {

	     /*
	      * let the mi helper routine do our work; better than
	      * duplicating code...
	      */
	       int   err;		/* modified bresenham error term */
	       int   clip1=0, clip2=0;	/* clippedness of the endpoints */

	       int   clipdx, clipdy;	/* difference between clipped and
					 * unclipped start point */
	       int   new_x1 = x1, new_y1 = y1, new_x2 = x2, new_y2 = y2;

               if (miZeroClipLine(pbox->x1, pbox->y1,
					pbox->x2-1, pbox->y2-1,
					&new_x1, &new_y1,
					&new_x2, &new_y2,
					adx, ady,
					&clip1, &clip2,
					octant, bias,
					oc1, oc2) == -1) {
		     pbox++;
		     continue;
		}

		if (axis == X_AXIS) {
                     len = abs(new_x2 - new_x1);
		}
                else {
		     len = abs(new_y2 - new_y1);
		}

                if (clip2 != 0 || pGC->capStyle != CapNotLast)
                    len++;

		if (len) {
                     dashIndexTmp = dashIndex;
                     dashOffsetTmp = dashOffset;
		     /* unwind bresenham error term to first point */
		     if (clip1) {
	                int   dlen;
			clipdx = abs(new_x1 - x1);
			clipdy = abs(new_y1 - y1);
			if (axis == X_AXIS) {
			   err = e + ((clipdy * e2) + ((clipdx - clipdy) * e1));
                           dlen = clipdx;
                        }
			else {
			   err = e + ((clipdx * e2) + ((clipdy - clipdx) * e1));
                           dlen = clipdy;
                        }
                        /* advance dash pattern to clip point */
                        miStepDash ( dlen, &dashIndexTmp, pDash,
                               numInDashList, &dashOffsetTmp);
		     } 
                     else
			err = e;
                     dashRemaining = pDash[dashIndexTmp] - dashOffsetTmp;
                     thisDash = dashRemaining;

                     /*
                      * Here is a problem, the unwound error terms could be
                      * upto 16bit now. The poor AGX is only 13 bit.
                      */
                      
                     if (abs(err) > 8192  || abs(e1) > 8192 || abs(e2) > 8192) {
                        int div;

                        if (abs(err) > abs(e1))
                            div = (abs(err) > abs(e2)) ?
                            (abs(err) + 8192)/ 8192 : (abs(e2) + 8191)/ 8192;
                        else
                            div = (abs(e1) > abs(e2)) ?
                            (abs(e1) + 8191)/ 8192 : (abs(e2) + 8191)/ 8192;

                        err /= div;
                        e1 /= div;
                        e2 /= div;
                     }

                     {
                        register unsigned int mapCoOrd  = new_y1 << 16 | new_x1;
                        register volatile unsigned int *pattern 
                           = (unsigned int *)agxVideoMem;

                        GE_WAIT_IDLE();
                        for (tmp = 0 ; tmp <= len; tmp+=32) {
                           FillDashPat;                 
                           *pattern++ = dashPat; 
                        }         

                        GE_OUT_D( GE_DEST_MAP_X, mapCoOrd );
                        GE_OUT_D( GE_PAT_MAP_X, 0 );
                        GE_OUT_W( GE_OP_DIM_LINE_MAJ, len-1 );
                        GE_OUT_W( GE_BRES_ERROR_TERM, err+fix );
                        GE_OUT_W( GE_BRES_CONST_K1,   e1 );
                        GE_OUT_W( GE_BRES_CONST_K2,   e2 );
                        GE_START_CMD( GE_OP_LINE_DRAW_WR
                                      | GE_OP_FRGD_SRC_CLR
                                      | GE_OP_SRC_MAP_A
                                      | GE_OP_DEST_MAP_A
                                      | GE_OP_DRAW_LAST_NULL
                                      | GE_OP_PAT_MAP_B
                                      | GE_OP_MASK_DISABLED
                                      | cmd2 );
		     }

	       } /* len */
	       pbox++;
	    }/* outcodes */
	 }/* while (nbox--) */
      }/* sloped line */
   } /* while (nline--) */

   GE_WAIT_IDLE_EXIT();
}
