/* $XConsortium: meuncomp.h,v 1.4 94/04/17 20:35:01 rws Exp $ */
/**** module miuncomp.h ****/
/******************************************************************************

Copyright (c) 1993, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


				NOTICE
                              
This software is being provided by AGE Logic, Inc. under the
following license.  By obtaining, using and/or copying this software,
you agree that you have read, understood, and will comply with these
terms and conditions:

     Permission to use, copy, modify, distribute and sell this
     software and its documentation for any purpose and without
     fee or royalty and to grant others any or all rights granted
     herein is hereby granted, provided that you agree to comply
     with the following copyright notice and statements, including
     the disclaimer, and that the same appears on all copies and
     derivative works of the software and documentation you make.
     
     "Copyright 1993, 1994 by AGE Logic, Inc."
     
     THIS SOFTWARE IS PROVIDED "AS IS".  AGE LOGIC MAKES NO
     REPRESENTATIONS OR WARRANTIES, EXPRESS OR IMPLIED.  By way of
     example, but not limitation, AGE LOGIC MAKE NO
     REPRESENTATIONS OR WARRANTIES OF MERCHANTABILITY OR FITNESS
     FOR ANY PARTICULAR PURPOSE OR THAT THE SOFTWARE DOES NOT
     INFRINGE THIRD-PARTY PROPRIETARY RIGHTS.  AGE LOGIC 
     SHALL BEAR NO LIABILITY FOR ANY USE OF THIS SOFTWARE.  IN NO
     EVENT SHALL EITHER PARTY BE LIABLE FOR ANY INDIRECT,
     INCIDENTAL, SPECIAL, OR CONSEQUENTIAL DAMAGES, INCLUDING LOSS
     OF PROFITS, REVENUE, DATA OR USE, INCURRED BY EITHER PARTY OR
     ANY THIRD PARTY, WHETHER IN AN ACTION IN CONTRACT OR TORT OR
     BASED ON A WARRANTY, EVEN IF AGE LOGIC LICENSEES
     HEREUNDER HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH
     DAMAGES.
    
     The name of AGE Logic, Inc. may not be used in
     advertising or publicity pertaining to this software without
     specific, written prior permission from AGE Logic.

     Title to this software shall at all times remain with AGE
     Logic, Inc.
*****************************************************************************
  
	meuncomp.c -- DDXIE includes for exporting uncompressed data
  
	Dean Verheiden -- AGE Logic, Inc. October 1993
  
*****************************************************************************/

typedef struct _meuncompdef {
  void     (*action)();
  CARD32   Bstride;
  CARD8	   dstoffset;	/* Number of bytes offset to this pixel's band 	*/
  CARD8    mask;	/* For obtaining subbyte pixels 	 	*/
  CARD8    shift;	/* Necessary shift after obtaining pixel 	*/
  CARD8	   bandMap;
  CARD8    clear_dst;	/* Scanline pad (in bits)			*/
  CARD8	   bitOff;	/* more that we can chew? 			*/
  CARD8	   leftOver;
  CARD8	   depth;
  CARD16   stride;
  CARD8	   unaligned;
  CARD8    pad;
  CARD32   width;
  CARD32   pitch;
  pointer  buf;
} meUncompRec, *meUncompPtr;

#ifdef _XIEC_MEUNCOMP

/* Action routines for encoding pixel planes */
void btoS(), sbtoS(), btoIS(), sbtoIS();
void BtoS(), PtoS(), sPtoS(), QtoS(), sQtoS(), QtoIS(), sQtoIS();

/* Action routines used by Triple band by pixel encoding */
void BtoIS(), PtoIS(), sPtoIS(), BtoISb(), btoISb();

/* Unaligned data packing routines */
void BtoLLUB(), BtoLMUB(), BtoMLUB(), BtoMMUB();
void PtoLLUP(), PtoLMUP(), PtoMLUP(), PtoMMUP();
void QtoLLUQ(), QtoLMUQ(), QtoMLUQ(), QtoMMUQ();


#else /* ifdef _XIEC_MEUNCOMP */

/* Action routines for encoding pixel planes */
extern void btoS(), sbtoS(), btoIS(), sbtoIS();
extern void BtoS(), PtoS(), sPtoS(), QtoS(), sQtoS(), QtoIS(), sQtoIS();

/* Action routines used by Triple band by pixel encoding */
extern void BtoIS(), PtoIS(), sPtoIS(), BtoISb(), btoISb();
extern void BtoLLUB(), BtoLMUB(), BtoMLUB(), BtoMMUB();
extern void PtoLLUP(), PtoLMUP(), PtoMLUP(), PtoMMUP();
extern void QtoLLUQ(), QtoLMUQ(), QtoMLUQ(), QtoMMUQ();

#if XIE_FULL
/* Array of pointers to actions routines for unaligned triple band by pixel */
extern void (*EncodeTripleFuncs[2][2][2][2][2])();
#endif

#endif /* ifdef _XIEC_MEUNCOMP */
