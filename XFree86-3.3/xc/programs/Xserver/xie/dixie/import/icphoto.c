/* $XConsortium: icphoto.c,v 1.4 94/04/17 20:33:34 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/dixie/import/icphoto.c,v 3.0 1996/03/29 22:10:24 dawes Exp $ */
/**** module icphoto.c ****/
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
  
	icphoto.c -- DIXIE routines for managing the ImportClientPhoto element
  
	Robert NC Shelley, Dean Verheiden -- AGE Logic, Inc. April 1993
  
*****************************************************************************/

#define _XIEC_ICPHOTO

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


/*
 *  routines referenced by other modules
 */
peDefPtr	MakeICPhoto();
Bool		CopyICPhotoUnSingle();
Bool		CopyICPhotoUnTriple();
Bool		CopyICPhotoG31D();
Bool		CopyICPhotoG32D();
Bool		CopyICPhotoG42D();
Bool		CopyICPhotoJPEGBaseline();
#ifdef  BEYOND_SI
Bool		CopyICPhotoJPEGLossless();
#endif /* BEYOND_SI */
Bool		CopyICPhotoTIFF2();
Bool		CopyICPhotoTIFFPackBits();

Bool		PrepICPhotoUnSingle();
Bool		PrepICPhotoUnTriple();
Bool		PrepICPhotoG31D();
Bool		PrepICPhotoG32D();
Bool		PrepICPhotoG42D();
Bool		PrepICPhotoJPEGBaseline();
#ifdef  BEYOND_SI
Bool		PrepICPhotoJPEGLossless();
#endif /* BEYOND_SI */
Bool		PrepICPhotoTIFF2();
Bool		PrepICPhotoTIFFPackBits();

/*
 *  routines internal to this module
 */
static Bool	PrepICPhoto();

/*
 * dixie element entry points
 */
static diElemVecRec iCPhotoVec = {
  PrepICPhoto			/* prepare for analysis and execution	*/
  };


/*------------------------------------------------------------------------
--------------- routine: make an import client photo element -------------
------------------------------------------------------------------------*/
peDefPtr MakeICPhoto(flo,tag,pe)
     floDefPtr      flo;
     xieTypPhototag tag;
     xieFlo        *pe;
{
  peDefPtr ped;
  ELEMENT(xieFloImportClientPhoto);
  ELEMENT_AT_LEAST_SIZE(xieFloImportClientPhoto);
  
  if(!(ped = MakePEDef(1, (CARD32)stuff->elemLength<<2, 0)))
    FloAllocError(flo,tag,xieElemImportClientPhoto, return(NULL)) ;

  ped->diVec	     = &iCPhotoVec;
  ped->phototag      = tag;
  ped->flags.import  = TRUE;
  ped->flags.putData = TRUE;
  raw = (xieFloImportClientPhoto *)ped->elemRaw;
  /*
   * copy the standard client element parameters (swap if necessary)
   */
  if( flo->reqClient->swapped ) {
    raw->elemType   = stuff->elemType;
    raw->elemLength = stuff->elemLength;
    raw->notify = stuff->notify;
    raw->class  = stuff->class;
    cpswapl(stuff->width0,  raw->width0);
    cpswapl(stuff->width1,  raw->width1);
    cpswapl(stuff->width2,  raw->width2);
    cpswapl(stuff->height0, raw->height0);
    cpswapl(stuff->height1, raw->height1);
    cpswapl(stuff->height2, raw->height2);
    cpswapl(stuff->levels0, raw->levels0);
    cpswapl(stuff->levels1, raw->levels1);
    cpswapl(stuff->levels2, raw->levels2);
    cpswaps(stuff->decodeTechnique, raw->decodeTechnique);
    cpswaps(stuff->lenParams, raw->lenParams);
  }
  else
    memcpy((char *)raw, (char *)stuff, sizeof(xieFloImportClientPhoto));
  /*
   * copy technique data (if any)
   */
  if(!(ped->techVec = FindTechnique(xieValDecode, raw->decodeTechnique)) ||
     !(ped->techVec->copyfnc(flo, ped, &stuff[1], &raw[1], raw->lenParams)))
    TechniqueError(flo,ped,xieValDecode,raw->decodeTechnique,raw->lenParams,
		   return(ped));

  return(ped);
}                               /* end MakeICPhoto */


Bool CopyICPhotoUnSingle(flo, ped, rparms, cparms, tsize) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeUncompressedSingle *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

Bool CopyICPhotoUnTriple(flo, ped, rparms, cparms, tsize) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeUncompressedTriple *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

Bool CopyICPhotoG31D(flo, ped, rparms, cparms, tsize) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeG31D *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

Bool CopyICPhotoG32D(flo, ped, rparms, cparms, tsize) 
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeG32D *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

Bool CopyICPhotoG42D(flo, ped, rparms, cparms, tsize)
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeG42D *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

Bool CopyICPhotoJPEGBaseline(flo, ped, rparms, cparms, tsize)
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeJPEGBaseline *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

#ifdef  BEYOND_SI
Bool CopyICPhotoJPEGLossless(flo, ped, rparms, cparms, tsize)
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeJPEGLossless *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}
#endif /* BEYOND_SI */

Bool CopyICPhotoTIFF2(flo, ped, rparms, cparms, tsize)
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeTIFF2 *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}

Bool CopyICPhotoTIFFPackBits(flo, ped, rparms, cparms, tsize)
     floDefPtr  flo;
     peDefPtr   ped;
     xieTecDecodeTIFFPackBits *rparms, *cparms;
     CARD16	tsize;
{
  VALIDATE_TECHNIQUE_SIZE(ped->techVec, tsize, FALSE);
  
  /* Nothing to swap for this technique */
  memcpy((char *)cparms, (char *)rparms, tsize<<2);
  
  return(TRUE);
}


/*------------------------------------------------------------------------
---------------- routine: prepare for analysis and execution -------------
------------------------------------------------------------------------*/
static Bool PrepICPhoto(flo,ped)
     floDefPtr  flo;
     peDefPtr   ped;
{
  int i;
  xieFloImportClientPhoto *raw = (xieFloImportClientPhoto *)ped->elemRaw;
  inFloPtr inflo = &ped->inFloLst[IMPORT];

  /*
   * check for data-class, dimension, and levels errors, and stash attributes
   */
  switch(raw->class) {
  case xieValSingleBand :
    if(!raw->width0 || !raw->height0 || !raw->levels0)
      ValueError(flo,ped,0, return(FALSE));
    if(raw->levels0 > MAX_LEVELS(1))
      MatchError(flo,ped, return(FALSE));
    inflo->bands = 1;
    break;
  case xieValTripleBand :
    if(!raw->width0 || !raw->height0 || !raw->levels0 ||
       !raw->width1 || !raw->height1 || !raw->levels1 ||
       !raw->width2 || !raw->height2 || !raw->levels2)
      ValueError(flo,ped,0, return(FALSE));
    if(raw->levels0 > MAX_LEVELS(3) ||
       raw->levels1 > MAX_LEVELS(3) ||
       raw->levels2 > MAX_LEVELS(3))
    MatchError(flo,ped, return(FALSE));
    inflo->bands	    = 3;
    inflo->format[1].band   = 1;
    inflo->format[1].width  = raw->width1;
    inflo->format[1].height = raw->height1;
    inflo->format[1].levels = raw->levels1;
    inflo->format[2].band   = 2;
    inflo->format[2].width  = raw->width2;
    inflo->format[2].height = raw->height2;
    inflo->format[2].levels = raw->levels2;
    break;
  default :
    ValueError(flo,ped,raw->class, return(FALSE));
  }
  inflo->format[0].band   = 0;
  inflo->format[0].width  = raw->width0;
  inflo->format[0].height = raw->height0;
  inflo->format[0].levels = raw->levels0;

  for(i = 0; i < inflo->bands; i++)
    SetDepthFromLevels(inflo->format[i].levels, inflo->format[i].depth);

  if(!(ped->techVec->prepfnc(flo, ped, raw, &raw[1])))
    TechniqueError(flo,ped,xieValDecode,raw->decodeTechnique,raw->lenParams,
		   return(FALSE));

  return(TRUE);
}                               /* end PrepICPhoto */


/*------------------------------------------------------------------------
----- routines: verify technique parameters against element parameters ----
-------------- and prepare for analysis and execution                 ----
------------------------------------------------------------------------*/

/* Prep routine for uncompressed single band data */
Bool PrepICPhotoUnSingle(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeUncompressedSingle *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  CARD32  padmod =   tec->scanlinePad * 8;
  CARD32   pitch =   tec->pixelStride * raw->width0 + tec->leftPad;
  BOOL   aligned = !(tec->pixelStride & (tec->pixelStride-1)) ||
		     tec->pixelStride == 24;
  int i;

  if(tec->fillOrder  != xieValLSFirst &&	    /* check fill-order     */
     tec->fillOrder  != xieValMSFirst)
    return(FALSE);
  if(tec->pixelOrder != xieValLSFirst &&	    /* check pixel-order    */
     tec->pixelOrder != xieValMSFirst)
    return(FALSE);
  if(tec->pixelStride < inf->format[0].depth)       /* check pixel-stride   */
    return(FALSE);
  if(ALIGNMENT == xieValAlignable && !aligned ||    /* alignment & left-pad */
     ALIGNMENT == xieValAlignable &&  aligned &&
    (tec->leftPad % tec->pixelStride ||
     tec->leftPad % 8))
    return(FALSE);
  if(tec->scanlinePad & (tec->scanlinePad-1) ||     /* check scanline-pad   */
     tec->scanlinePad > 16)
    return(FALSE);
  if(raw->class != xieValSingleBand)
    return(FALSE);

  inf->format[0].interleaved = FALSE;
  inf->format[0].class  = STREAM;
  inf->format[0].stride = tec->pixelStride;
  inf->format[0].pitch  = pitch + (padmod ? Align(pitch,padmod) : 0);

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);
} /* PrepICPhotoUnSingle */

/* Prep routine for uncompressed triple band data */
Bool PrepICPhotoUnTriple(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeUncompressedTriple *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if(tec->fillOrder  != xieValLSFirst &&	    /* check fill-order     */
     tec->fillOrder  != xieValMSFirst)
    return(FALSE);
  if(tec->pixelOrder != xieValLSFirst &&	    /* check pixel-order    */
     tec->pixelOrder != xieValMSFirst)
    return(FALSE);
  if(tec->bandOrder  != xieValLSFirst &&	    /* check band-order     */
     tec->bandOrder  != xieValMSFirst)
    return(FALSE);
  if(tec->interleave != xieValBandByPixel &&	    /* check interleave     */
     tec->interleave != xieValBandByPlane)
    return(FALSE);
  if (tec->interleave == xieValBandByPixel && 	    /* check inter-band dim */
      (inf->format[0].width  != inf->format[1].width   ||
       inf->format[1].width  != inf->format[2].width   ||
       inf->format[0].height != inf->format[1].height  ||
       inf->format[1].height != inf->format[2].height))
    return(FALSE);
  if(raw->class != xieValTripleBand)
    return(FALSE);
  if (tec->interleave == xieValBandByPlane) {
    for (i = 0; i < 3; i++) {
      CARD32  padmod =   tec->scanlinePad[i] * 8;
      CARD32   pitch =   tec->pixelStride[i] * inf->format[i].width + 
					tec->leftPad[i];
      BOOL   aligned = !(tec->pixelStride[i] & (tec->pixelStride[i] - 1));

     if(tec->pixelStride[i] < inf->format[i].depth)   /* check pixel-stride   */
        return(FALSE);
      if(inf->format[i].depth > MAX_DEPTH(3)) 	      /* check pixel-depth   */
 	return(FALSE);
      if(ALIGNMENT == xieValAlignable && !aligned ||  /* alignment & left-pad */
         ALIGNMENT == xieValAlignable &&  aligned &&
         (tec->leftPad[i] % tec->pixelStride[i] ||
          tec->leftPad[i] % 8))
             return(FALSE);
      if(tec->scanlinePad[i] & (tec->scanlinePad[i] - 1) || 
         tec->scanlinePad[i] > 16)              /*check scanline-pad*/
        return(FALSE);
      inf->format[i].interleaved = FALSE;
      inf->format[i].class  = STREAM;
      inf->format[i].stride = tec->pixelStride[i];
      inf->format[i].pitch  = pitch + (padmod ? Align(pitch,padmod) : 0);
    }
    ped->outFlo.bands = inf->bands;
  } else {	/* xieValBandByPixel */
      CARD32  padmod =   tec->scanlinePad[0] * 8;
      CARD32   pitch =   tec->pixelStride[0] * inf->format[0].width + 
					tec->leftPad[0];
      CARD32  tdepth = inf->format[0].depth + inf->format[1].depth + 
					inf->format[2].depth;
      BOOL   aligned = !(tec->pixelStride[0] & (tec->pixelStride[0] - 1)) ||
						tec->pixelStride[0] == 24;

      if(inf->format[0].depth > MAX_DEPTH(3) ||       /* check pixel-depth   */
         inf->format[1].depth > MAX_DEPTH(3) ||
         inf->format[2].depth > MAX_DEPTH(3))
 	return(FALSE);
      if(tec->pixelStride[0] < tdepth)  /* check overall pixel-stride   */
 	return(FALSE);
      if(ALIGNMENT == xieValAlignable && !aligned ||  /* alignment & left-pad */
         ALIGNMENT == xieValAlignable &&  aligned &&
         (tec->leftPad[0] % tec->pixelStride[0] ||
          tec->leftPad[0] % 8))
             return(FALSE);
      if(tec->scanlinePad[0] & (tec->scanlinePad[0] - 1) || 
         tec->scanlinePad[0] > 16)              /*check scanline-pad*/
        return(FALSE);

      /* Now, go stomp on band zero values that should be changed */
      inf->bands = 1;
      inf->format[0].interleaved = TRUE;
      inf->format[0].class  = STREAM;
      inf->format[0].stride = tec->pixelStride[0];
      inf->format[0].pitch  = pitch + (padmod ? Align(pitch,padmod) : 0);

      /* Set up 3 outflows for the one interleaved inflo */
      ped->outFlo.bands = 3;
  }

  /* Copy input to output, setting differing output parameters when necessary */
  for (i = 0; i < ped->outFlo.bands; i++) {
     ped->outFlo.format[i] = inf->format[i];
     ped->outFlo.format[i].interleaved = FALSE;
  }

   /* Fill in other format parameters based on the number of output levels */
  if (UpdateFormatfromLevels(ped) == FALSE)
     MatchError(flo, ped, return(FALSE));

  return(TRUE);
} /* PrepICPhotoUnTriple */

Bool PrepICPhotoG31D(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeG31D *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if(tec->encodedOrder  != xieValLSFirst &&	    /* check encoding-order  */
     tec->encodedOrder  != xieValMSFirst)
    return(FALSE);

  inf->format[0].interleaved = FALSE;
  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));
  return(TRUE);

} /* PrepICPhotoG31D */

Bool PrepICPhotoG32D(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeG32D *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if(tec->encodedOrder  != xieValLSFirst &&	    /* check encoding-order  */
     tec->encodedOrder  != xieValMSFirst)
    return(FALSE);

  inf->format[0].interleaved = FALSE;
  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);

} /* PrepICPhotoG32D */

Bool PrepICPhotoG42D(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeG42D *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if(tec->encodedOrder  != xieValLSFirst &&	    /* check encoding-order  */
     tec->encodedOrder  != xieValMSFirst)
    return(FALSE);

  inf->format[0].interleaved = FALSE;
  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);

} /* PrepICPhotoG42D */

Bool PrepICPhotoJPEGBaseline(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeJPEGBaseline *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if (raw->class == xieValSingleBand) 
  	inf->format[0].interleaved = FALSE;
  else {
  	if(tec->bandOrder  != xieValLSFirst &&	    /* check encoding-order  */
	     tec->bandOrder  != xieValMSFirst)
	    return(FALSE);

	if(tec->interleave != xieValBandByPixel && /* check interleave     */
	     tec->interleave != xieValBandByPlane)
	    return(FALSE);

	inf->format[0].interleaved =
		inf->format[1].interleaved =
		inf->format[2].interleaved =
		(tec->interleave == xieValBandByPixel);
	inf->format[1].class  = STREAM;
	inf->format[2].class  = STREAM;
  }

  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  /*
   * except if TripleBand Interleaved, we lied:  there's only one 
   * band coming in. We copy the bogus inflos first just because 
   * we can assume they have been set up suitably by nice general
   * purpose code above.  :-)
   */
  if (raw->class == xieValTripleBand && tec->interleave == xieValBandByPixel)
      inf->bands = 1;

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);

} /* PrepICPhotoJPEGBaseline */

#ifdef  BEYOND_SI
Bool PrepICPhotoJPEGLossless(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeJPEGLossless *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if (raw->class == xieValSingleBand) 
  	inf->format[0].interleaved = FALSE;
  else {
  	if(tec->bandOrder  != xieValLSFirst &&	    /* check encoding-order  */
	     tec->bandOrder  != xieValMSFirst)
	    return(FALSE);

	if(tec->interleave != xieValBandByPixel && /* check interleave     */
	     tec->interleave != xieValBandByPlane)
	    return(FALSE);

	inf->format[0].interleaved = 
		inf->format[1].interleaved  =
		inf->format[2].interleaved  =
		(tec->interleave == xieValBandByPixel);
	inf->format[1].class  = STREAM;
	inf->format[2].class  = STREAM;
  }

  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);

} /* PrepICPhotoJPEGLossless */
#endif /* BEYOND_SI */

Bool PrepICPhotoTIFF2(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeTIFF2 *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if(tec->encodedOrder  != xieValLSFirst &&	    /* check encoding-order  */
     tec->encodedOrder  != xieValMSFirst)
    return(FALSE);

  inf->format[0].interleaved = FALSE;
  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);

} /* PrepICPhotoTIFF2 */

Bool PrepICPhotoTIFFPackBits(flo, ped, raw, tec) 
     floDefPtr flo;
     peDefPtr  ped;
     xieFloImportClientPhoto *raw;
     xieTecDecodeTIFFPackBits *tec;
{
  inFloPtr   inf =  &ped->inFloLst[IMPORT];
  int i;

  if(tec->encodedOrder  != xieValLSFirst &&	    /* check encoding-order  */
     tec->encodedOrder  != xieValMSFirst)
    return(FALSE);

  inf->format[0].interleaved = FALSE;
  inf->format[0].class  = STREAM;

  /*
   * determine output attributes from input parameters
   */
  ped->outFlo.bands = inf->bands;
  for (i = 0; i < inf->bands; i++) {
    ped->outFlo.format[i] = inf->format[i];
    ped->outFlo.format[i].interleaved = FALSE;
  }

  if (UpdateFormatfromLevels(ped) == FALSE)
	MatchError(flo, ped, return(FALSE));

  return(TRUE);

} /* PrepICPhotoTIFFPackBits */

/* end module icphoto.c */
