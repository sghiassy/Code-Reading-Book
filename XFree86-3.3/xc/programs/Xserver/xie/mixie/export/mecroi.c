/* $XConsortium: mecroi.c,v 1.5 94/04/17 20:34:16 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/mixie/export/mecroi.c,v 3.1 1996/08/20 13:16:13 dawes Exp $ */
/**** module mecroi.c ****/
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
  
	mecroi.c -- DDXIE export client roi element
  
	Larry Hare && Dean Verheiden -- AGE Logic, Inc. August, 1993
  
*****************************************************************************/

#define _XIEC_MECROI
#define _XIEC_ECROI

/*
 *  Include files
 */
#include <stdio.h>
/*
 *  Core X Includes
 */
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
#include <error.h>
#include <macro.h>
#include <element.h>
#include <texstr.h>
#include <memory.h>


/* routines referenced by other DDXIE modules
 */
int	miAnalyzeECROI();

/* routines used internal to this module
 */
static int CreateECROI();
static int InitializeECROI();
static int ActivateECROI();
static int ResetECROI();
static int DestroyECROI();

/* DDXIE ExportClientROI entry points
 */
static ddElemVecRec ECROIVec =
{
  CreateECROI,
  InitializeECROI,
  ActivateECROI,
  (xieIntProc)NULL,
  ResetECROI,
  DestroyECROI
};

/* Local routines */
static void ConvertToRect();

/*------------------------------------------------------------------------
------------------- see if we can handle this element --------------------
------------------------------------------------------------------------*/
int miAnalyzeECROI(flo,ped)
floDefPtr flo;
peDefPtr  ped;
{
	/* for now just stash our entry point vector in the peDef */
	ped->ddVec = ECROIVec;
	return TRUE;
}                               /* end miAnalyzeECROI */

/*------------------------------------------------------------------------
---------------------------- create peTex . . . --------------------------
------------------------------------------------------------------------*/
static int CreateECROI(flo,ped)
floDefPtr flo;
peDefPtr  ped;
{
	/* attach an execution context to the roi element definition */
	return MakePETex(flo, ped, NO_PRIVATE, NO_SYNC, NO_SYNC);
}                               /* end CreateECROI */

/*------------------------------------------------------------------------
---------------------------- initialize peTex . . . ----------------------
------------------------------------------------------------------------*/
static int InitializeECROI(flo,ped)
floDefPtr flo;
peDefPtr  ped;
{
	return InitReceptors(flo,ped,NO_DATAMAP,1) &&
	         InitEmitter(flo,ped,NO_DATAMAP,NO_INPLACE);
}                               /* end InitializeECROI */

/*------------------------------------------------------------------------
----------------------------- crank some data ----------------------------
------------------------------------------------------------------------*/
static int ActivateECROI(flo,ped,pet)
     floDefPtr flo;
     peDefPtr  ped;
     peTexPtr  pet;
{
  xieFloExportClientROI *raw = (xieFloExportClientROI*)ped->elemRaw;
  receptorPtr       rcp  = pet->receptor;
  bandPtr	    sbnd = &rcp->band[0];
  bandPtr	    dbnd = &pet->emitter[0];
  CARD32	    rectSize;
  xieTypRectangle  *prect;
  ROIPtr	    proi;
  
  if (!(proi = (ROIRec*)GetCurrentSrc(flo,pet,sbnd)))
    return FALSE;
  
  rectSize = sizeof(xieTypRectangle) * proi->nrects;
  
  if (!(prect = (xieTypRectangle*)GetDstBytes(flo,pet,dbnd,0,rectSize,KEEP)))
    return FALSE;
  
  ConvertToRect(proi,prect);
  
  SetBandFinal(dbnd);
  PutData(flo,pet,dbnd,rectSize);
  FreeData(flo,pet,sbnd,sbnd->maxLocal);
  
  switch(raw->notify) {
  case xieValFirstData:	/* fall thru */
  case xieValNewData:	SendExportAvailableEvent(flo,ped,0,proi->nrects,0,0);
  default:		break;
  }
  
  return TRUE;
}                               /* end ActivateECROI */

static void ConvertToRect(proi,prect) 
ROIPtr	        proi;
xieTypRectangle *prect;
{
	CARD32	nrects = 0;
	linePtr lp;

	/* Convert run lengths to xieTypRectangles */
	lp = (linePtr)&proi[1];
	while (lp < proi->lend) {
	    register runPtr rp = RUNPTR(0);
	    register CARD32 x = proi->x;
            register CARD32 j = lp->nrun;	

	    while (j--) {
		    x += rp->dstart;
		    prect[nrects].x        = x;
		    prect[nrects].y        = lp->y;
		    prect[nrects].width    = rp->length;
		    prect[nrects++].height = lp->nline;
		    x += (rp++)->length;
	    }
	    lp = (linePtr)RUNPTR(lp->nrun);
	}
}

/*------------------------------------------------------------------------
------------------------ get rid of run-time stuff -----------------------
------------------------------------------------------------------------*/
static int ResetECROI(flo,ped)
floDefPtr flo;
peDefPtr  ped;
{
	ResetReceptors(ped);
	ResetEmitter(ped);
	
	return TRUE;
}                               /* end ResetECROI */

/*------------------------------------------------------------------------
-------------------------- get rid of this element -----------------------
------------------------------------------------------------------------*/
static int DestroyECROI(flo,ped)
floDefPtr flo;
peDefPtr  ped;
{
	/* get rid of the peTex structure  */
	ped->peTex = (peTexPtr) XieFree(ped->peTex);

	/* zap this element's entry point vector */
	ped->ddVec.create     = (xieIntProc) NULL;
	ped->ddVec.initialize = (xieIntProc) NULL;
	ped->ddVec.activate   = (xieIntProc) NULL;
	ped->ddVec.flush      = (xieIntProc) NULL;
	ped->ddVec.reset      = (xieIntProc) NULL;
	ped->ddVec.destroy    = (xieIntProc) NULL;

	return TRUE;
}                               /* end DestroyECROI */
