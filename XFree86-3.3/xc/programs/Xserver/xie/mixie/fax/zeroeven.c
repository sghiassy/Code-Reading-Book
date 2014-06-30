/* $XConsortium: zeroeven.c /main/5 1995/12/02 16:50:41 dpw $ */
/**** module fax/zeroeven.c ****/
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
  
	fax/zeroeven.c -- DDXIE fax utility
  
	Ben && Larry -- AGE Logic, Inc. May, 1993
  
*****************************************************************************/


/***	zero_even.c - zero out even transitions. 

	The array wbtrans is arranged as a table of positions at which
	the input changes from white to black, then black to white, then
	white to black, and so on.  We assume all output pixels have
	already been set white. Therefore we only have to fill the
	black ranges with zeros...

	W->B   B->W   W->B   B->W   W->B   B->W   W->B   B->W

	12	17     29     30     65    68      90     92

	bit positions 12-16, 29, 65-67, and 90-91 in the above
	example are black.  It should be clear from the example
	that the size of the run length is t[w]-t[b],  where 
	w=b+1 and b  is the index of a transition from white to 
	black.  Note that if the line was only 92 pixels wide,
	setting the imaginary pixel with index = image width
	B->W still gives us the right answer.

***/

#include <xiemd.h>

/**********************************************************************/

void
zero_even(cp,t,nt,w,white)
register LogInt *cp;
register int	*t, nt,w,white;
{
unsigned int	sbit,ebit;
LogInt		*sbyt, *ebyt, black;

#define ONES ~((LogInt)0)
   
    memset(cp, white ? 0xff: 0, (w+7)>>3);
    black = white ? 0 : ONES; 

    for (nt = (nt>>1) - 1; nt >= 0; nt--) {
	sbit = *t++;
	ebit = *t++; /* if (!nt && (ebit > w)) ebit = w; */
	sbyt = cp + (sbit >> LOGSHIFT); sbit &= LOGMASK;
	ebyt = cp + (ebit >> LOGSHIFT); ebit &= LOGMASK;

	if (sbyt < ebyt) {
	    if (sbit)	    	*sbyt++ ^= BitRight(ONES,sbit);
	    while (sbyt < ebyt)	*sbyt++  = black;
	    if (ebit)		*sbyt   ^= ~BitRight(ONES,ebit);
	} else
	    *sbyt ^= BitRight(ONES,sbit) & ~(BitRight(ONES,ebit));
    }
}
/**** module fax/zeroeven.c ****/
