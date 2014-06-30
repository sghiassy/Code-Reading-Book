/* $XConsortium: pmath.c,v 1.3 94/04/17 20:33:50 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/dixie/process/pmath.c,v 3.0 1996/03/29 22:11:03 dawes Exp $ */
/**** module pmath.c ****/
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
  
	pmath.c -- DIXIE routines for managing the Math element
  
	Dean Verheiden -- AGE Logic, Inc. July 1993
  
*****************************************************************************/

#define _XIEC_PMATH

/*
 *  Include files
 */
#include <stdio.h>
  /*
   *  Core X Includes
   */
#define NEED_EVENTS
#include <X.h>
#include <Xproto.h>
  /*
   *  XIE Includes
   */
#include <XIE.h>
#include <XIEproto.h>
  /*
   *  more X server includes.
   */
#include <misc.h>
#include <dixstruct.h>
  /*
   *  Server XIE Includes
   */
#include <corex.h>
#include <macro.h>
#include <element.h>


/*
 *  routines referenced by other modules.
 */
peDefPtr	MakeMath();

/*
 *  routines internal to this module
 */
static Bool	PrepMath();

/*
 * dixie entry points
 */
static diElemVecRec pMathVec =
{
	PrepMath		/* prepare for analysis and execution	*/
};


/*------------------------------------------------------------------------
----------------------- routine: make a math element --------------------
------------------------------------------------------------------------*/
peDefPtr MakeMath(flo,tag,pe)
	floDefPtr      flo;
	xieTypPhototag tag;
	xieFlo        *pe;
{
	int inputs;
	peDefPtr ped;
	inFloPtr inFlo;
	ELEMENT(xieFloMath);
	ELEMENT_SIZE_MATCH(xieFloMath);
	ELEMENT_NEEDS_1_INPUT(src);

	inputs = 1 + (stuff->domainPhototag ? 1 :0);
	
	if (!(ped = MakePEDef(inputs, (CARD32)stuff->elemLength<<2, 0)))
		FloAllocError(flo, tag, xieElemMath, return(NULL));

	ped->diVec	   = &pMathVec;
	ped->phototag      = tag;
	ped->flags.process = TRUE;
	raw = (xieFloMath *)ped->elemRaw;
	/*
	 * copy the client element parameters (swap if necessary)
	 */
	if (flo->reqClient->swapped)
	{
		raw->elemType   = stuff->elemType;
		raw->elemLength = stuff->elemLength;
		cpswaps(stuff->src, raw->src);
		cpswapl(stuff->domainOffsetX, raw->domainOffsetX);
		cpswapl(stuff->domainOffsetY, raw->domainOffsetY);
		cpswaps(stuff->domainPhototag,raw->domainPhototag);
		raw->operator = stuff->operator;
		raw->bandMask = stuff->bandMask;
	}
	else
		memcpy((char *)raw, (char *)stuff, sizeof(xieFloMath));

	/* assign phototags to inFlos */
	inFlo = ped->inFloLst;
	inFlo[SRCt1].srcTag = raw->src;
	if(raw->domainPhototag)
		inFlo[ped->inCnt-1].srcTag = raw->domainPhototag;
	return ped;
}                               /* end MakeMath */


/*------------------------------------------------------------------------
---------------- routine: prepare for analysis and execution -------------
------------------------------------------------------------------------*/
static Bool PrepMath(flo,ped)
	floDefPtr  flo;
	peDefPtr   ped;
{
	xieFloMath *raw = (xieFloMath *)ped->elemRaw;
	inFloPtr  ind, in = &ped->inFloLst[SRCt1];
	outFloPtr src = &in->srcDef->outFlo;
	outFloPtr dst = &ped->outFlo;
	int b;

        /* make sure input is not bitonal */
        for (b = 0; b < src->bands; b++)
	    if (IsntCanonic(src->format[b].class) ||
		((raw->bandMask & (1<<b)) && src->format[b].class == BIT_PIXEL))
	            MatchError(flo,ped, return(FALSE));


	/* check out our process domain */
	if(raw->domainPhototag) {
		outFloPtr dom;

		ind = &ped->inFloLst[ped->inCnt-1];
		dom = &ind->srcDef->outFlo;
		if(IsntDomain(dom->format[0].class) || 
		   (ind->bands = dom->bands) != 1)
			DomainError(flo,ped,raw->domainPhototag, return(FALSE));
		ind->format[0] = dom->format[0];
	}

	/* grab a copy of the input attributes and propagate them to output */
	dst->bands = in->bands = src->bands;
	for(b = 0; b < dst->bands; b++)
		dst->format[b] = in->format[b] = src->format[b];

	switch(raw->operator) {
	case xieValExp:    break;
	case xieValLn:     break;
	case xieValLog2:   break;
	case xieValLog10:  break;
	case xieValSquare: break;
	case xieValSqrt:   break;
	default:	   OperatorError(flo,ped,raw->operator,return(FALSE));
	}
	return (TRUE);
}                               /* end PrepMath */

/* end module pmath.c */
