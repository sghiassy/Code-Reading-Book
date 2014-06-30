/* $XConsortium: pgeom.c,v 1.5 94/04/17 20:33:46 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/dixie/process/pgeom.c,v 3.1 1996/08/25 14:04:28 dawes Exp $ */
/**** module pgeom.c ****/
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
  
	pgeom.c -- DIXIE routines for managing the Geometry element
  
	Ben Fahy -- AGE Logic, Inc. June 1993
  
*****************************************************************************/

#define _XIEC_PGEOM

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
#include <error.h>
#include <macro.h>
#include <element.h>
#include <technq.h>
#include <difloat.h>
#include <memory.h>


/*
 *  routines referenced by other modules.
 */
peDefPtr	MakeGeometry();
Bool		CopyGeomNearestNeighbor();
Bool		PrepGeomNearestNeighbor();
Bool		CopyGeomAntiAlias();
Bool		PrepGeomAntiAlias();

#if XIE_FULL
Bool		CopyGeomBilinearInterp();
Bool		PrepGeomBilinearInterp();
Bool		CopyGeomGaussian();
Bool		PrepGeomGaussian();
#endif

/*
 *  routines internal to this module
 */
static Bool	PrepGeometry();
static Bool	CopyGeomNoParams();

/*
 * dixie entry points
 */
static diElemVecRec pGeometryVec = {
    PrepGeometry		/* prepare for analysis and execution	*/
    };

/*------------------------------------------------------------------------
----------------------- routine: make a convolution element --------------
------------------------------------------------------------------------*/
peDefPtr MakeGeometry(flo,tag,pe)
     floDefPtr      flo;
     xieTypPhototag tag;
     xieFlo        *pe;
{
  int inputs;
  peDefPtr ped;
  inFloPtr inFlo;
  pGeomDefPtr pvt;
  xieTypFloat *kptr;
  int i;
  ELEMENT(xieFloGeometry);
  ELEMENT_AT_LEAST_SIZE(xieFloGeometry);
  ELEMENT_NEEDS_1_INPUT(src);
  inputs = 1;


  if(!(ped = MakePEDef(inputs, (CARD32)stuff->elemLength<<2,
			       sizeof(pGeomDefRec))))
    FloAllocError(flo, tag, xieElemGeometry, return(NULL));

  ped->diVec	     = &pGeometryVec;
  ped->phototag      = tag;
  ped->flags.process = TRUE;
  raw = (xieFloGeometry *)ped->elemRaw;
  /*
   * copy the client element parameters (swap if necessary)
   */
  if( flo->reqClient->swapped ) {
    raw->elemType   = stuff->elemType;
    raw->elemLength = stuff->elemLength;
    cpswaps(stuff->src, raw->src);
    raw->bandMask = stuff->bandMask;
    cpswapl(stuff->width, raw->width);
    cpswapl(stuff->height, raw->height);
    cpswaps(stuff->sample, raw->sample);
    cpswaps(stuff->lenParams, raw->lenParams);
  }
  else
    memcpy((char *)raw, (char *)stuff, sizeof(xieFloGeometry));

  /* Copy over and convert the floating point stuff */
  kptr = (xieTypFloat *)&stuff->a;
  pvt = (pGeomDefPtr)ped->elemPvt;

  if (flo->reqClient->swapped) {
	  for (i = 0; i < 6; ++kptr, ++i)
		pvt->coeffs[i] = ConvertFromIEEE(lswapl(*kptr));
	  for (i = 0; i < xieValMaxBands; ++kptr, ++i)
		pvt->constant[i] = ConvertFromIEEE(lswapl(*kptr));
  } else {
	  for (i = 0; i < 6; i++) 
		pvt->coeffs[i] = ConvertFromIEEE(*kptr++);
	  for (i = 0; i < xieValMaxBands; i++) 
		pvt->constant[i] = ConvertFromIEEE(*kptr++);
  }
  /*
   * copy technique data (if any) 
   */
  if(!(ped->techVec = FindTechnique(xieValGeometry, raw->sample)) ||
     !(ped->techVec->copyfnc(flo, ped,  &stuff[1], &raw[1], raw->lenParams,
					raw->sample == xieValDefault))) 
    TechniqueError(flo,ped,xieValGeometry,raw->sample,raw->lenParams,
		   return(ped));

  /*
   * assign phototags to inFlos
   */
  inFlo = ped->inFloLst;
  inFlo[SRCtag].srcTag = raw->src;
  
  return(ped);
}                               /* end MakeGeometry */

/*------------------------------------------------------------------------
----------- routine: copy routine for NearestNeighbor technique  ---------
------------------------------------------------------------------------*/

Bool CopyGeomNearestNeighbor(flo, ped, sparms, rparms, tsize, isDefault) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecGeomNearestNeighbor *sparms, *rparms;
     CARD16	tsize;
     Bool	isDefault;
{
     pTecGeomNearestNeighborDefPtr pvt;

     VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, isDefault);

     if (!(ped->techPvt = XieMalloc(sizeof(pTecGeomNearestNeighborDefRec))))
	     FloAllocError(flo, ped->phototag, xieElemGeometry, return(TRUE));

     pvt = (pTecGeomNearestNeighborDefPtr)ped->techPvt;

    /*
     *	Nearest Neighbor can be called with no parameters
     */
     if (isDefault)
         pvt->modify = xieValFavorUp;
     else
         pvt->modify = sparms->modify;

     return (TRUE);
}

#if XIE_FULL
/*------------------------------------------------------------------------
------ routine: copy routine for bilinear interpolation technique --------
------------------------------------------------------------------------*/

Bool CopyGeomBilinearInterp(flo, ped, sparms, rparms, tsize, isDefault) 
     floDefPtr  flo;
     peDefPtr   ped;
     pointer sparms, rparms;
     CARD16	tsize;
     Bool	isDefault;
{
     VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, isDefault);

     return( CopyGeomNoParams(flo, ped, sparms, rparms, tsize, isDefault) );
}
/*------------------------------------------------------------------------
------ routine: copy routine for gaussian interpolation technique --------
------------------------------------------------------------------------*/
Bool CopyGeomGaussian(flo, ped, sparms, rparms, tsize, isDefault) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecGeomGaussian *sparms, *rparms;
     CARD16	tsize;
     Bool	isDefault;
{
     pTecGeomGaussianDefPtr pvt;

     VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, isDefault);

     if (!(ped->techPvt=XieMalloc(sizeof(pTecGeomGaussianDefRec))))
	     FloAllocError(flo, ped->phototag, xieElemGeometry, return(TRUE));

     pvt = (pTecGeomGaussianDefPtr)ped->techPvt;

      if( flo->reqClient->swapped ) {
	     pvt->sigma     = ConvertFromIEEE(lswapl(sparms->sigma));
	     pvt->normalize = ConvertFromIEEE(lswapl(sparms->normalize));
      } else {
	     pvt->sigma     = ConvertFromIEEE(sparms->sigma);
	     pvt->normalize = ConvertFromIEEE(sparms->normalize);
      }
      pvt->radius = sparms->radius;
      pvt->simple = sparms->simple;

     if (pvt->radius < 1)
	return(FALSE);		

     if (pvt->sigma == 0.0)
	return(FALSE);		/* musn't divide by zero, deary */

     if (pvt->normalize <= 0.0)
	return(FALSE);		/* don't want to bother clipping pixels < 0 */

     return (TRUE);
}
#endif

/*------------------------------------------------------------------------
------ routine: copy routine for antialias technique --------
------------------------------------------------------------------------*/

Bool CopyGeomAntiAlias(flo, ped, sparms, rparms, tsize, isDefault) 
     floDefPtr  flo;
     peDefPtr   ped;
     pointer sparms, rparms;
     CARD16	tsize;
     Bool	isDefault;
{
     VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, isDefault);

     return( CopyGeomNoParams(flo, ped, sparms, rparms, tsize, isDefault) );
}

/*------------------------------------------------------------------------
------------ routine: copy routine for techniques with no params  --------
------------------------------------------------------------------------*/
static Bool CopyGeomNoParams(flo, ped, sparms, rparms, tsize, isDefault) 
     floDefPtr  flo;
     peDefPtr   ped;
     pointer sparms, rparms;
     CARD16	tsize;
     Bool	isDefault;
{
  return(tsize == 0);
}

/*------------------------------------------------------------------------
---------------- routine: prepare for analysis and execution -------------
------------------------------------------------------------------------*/
static Bool PrepGeometry(flo,ped)
     floDefPtr  flo;
     peDefPtr   ped;
{
  xieFloGeometry *raw = (xieFloGeometry *)ped->elemRaw;
  inFloPtr  in = &ped->inFloLst[SRCtag];
  outFloPtr src = &in->srcDef->outFlo;
  outFloPtr dst = &ped->outFlo;
  pGeomDefPtr pvt = (pGeomDefPtr)ped->elemPvt;
  CARD32 b, bits;

  /* grab a copy of the input attributes and propagate them to our output */
  dst->bands = in->bands = src->bands;

  for(b = 0; b < dst->bands; b++) {
	if (IsntCanonic(src->format[b].class))
		MatchError(flo, ped, return(FALSE));

	dst->format[b] = in->format[b] = src->format[b];
	pvt->do_band[b] = (dst->bands==1)? 1 : raw->bandMask & (1<<b);
	if (pvt->do_band[b]) {
  		dst->format[b].width  = raw->width;
  		dst->format[b].height = raw->height;
	}
	bits = dst->format[b].width * dst->format[b].stride;
	dst->format[b].pitch = bits + Align(bits,PITCH_MOD);
  }

  if(!(ped->techVec->prepfnc(flo, ped, raw, &raw[1]))) {
    TechniqueError(flo,ped,xieValGeometry,raw->sample,raw->lenParams,
		   return(FALSE));
  }

  return( TRUE );
}                               /* end PrepGeometry */

/*------------------------------------------------------------------------
---------------- routine: prep routine for nearest neighbor --------------
------------------------------------------------------------------------*/
Bool PrepGeomNearestNeighbor(flo, ped, raw, tec) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieFloGeometry *raw;
     pointer tec;
{
  return(TRUE);
}

#if XIE_FULL
/*------------------------------------------------------------------------
---------- routine: prep routine for bilinear interpolation --------------
------------------------------------------------------------------------*/
Bool PrepGeomBilinearInterp(flo, ped, raw, tec) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieFloGeometry *raw;
     pointer tec;
{
  return(TRUE);
}
/*------------------------------------------------------------------------
---------- routine: prep routine for gaussian ----------------------------
------------------------------------------------------------------------*/
Bool PrepGeomGaussian(flo, ped, raw, tec) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieFloGeometry *raw;
     pointer tec;
{
  return(TRUE);
}
#endif

/*------------------------------------------------------------------------
---------- routine: prep routine for antialias ---------------------------
------------------------------------------------------------------------*/
Bool PrepGeomAntiAlias(flo, ped, raw, tec) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieFloGeometry *raw;
     pointer tec;
{
  return(TRUE);
}

/* end module pgeom.c */
