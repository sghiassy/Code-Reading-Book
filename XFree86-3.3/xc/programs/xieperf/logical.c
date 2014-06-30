/* $XConsortium: logical.c,v 1.8 94/04/17 20:39:24 rws Exp $ */

/**** module logical.c ****/
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
  
	logical.c -- logical flo element tests 

	Syd Logan -- AGE Logic, Inc.
  
*****************************************************************************/
#include "xieperf.h"
#include <stdio.h>

static XiePhotomap XIEPhotomap1;
static XiePhotomap XIEPhotomap2;
static XiePhotomap ControlPlane;
static XieLut XIELut;
static XieRoi XIERoi;
static XIEimage *image1, *image2;

static int monoflag = 0;

static XiePhotoflo flo;
static int flo_notify;
static XiePhotoElement *flograph;
static int flo_elements;
static XieRectangle *rects;

int InitLogical(xp, p, reps)
    XParms  xp;
    Parms   p;
    int     reps;
{
        int i, idx;
	XieProcessDomain domain;
        unsigned int bandMask;
        int useDomain;
	unsigned long logicalOp;
        int numROIs, src1, src2;
	XieConstant logicalConstant;

	XIELut = ( XieLut ) NULL;
        useDomain = ( ( LogicalParms * )p->ts )->useDomain;
        numROIs = ( ( LogicalParms * )p->ts )->numROIs;
        bandMask = ( ( LogicalParms * )p->ts )->logicalBandMask;
	logicalOp = ( ( LogicalParms * )p->ts )->logicalOp;
	logicalConstant[ 0 ] = (( LogicalParms * )p->ts)->logicalConstant[ 0 ];
	logicalConstant[ 1 ] = (( LogicalParms * )p->ts)->logicalConstant[ 1 ];
	logicalConstant[ 2 ] = (( LogicalParms * )p->ts)->logicalConstant[ 2 ];

        image1 = p->finfo.image1;

        /* we must have image1. image2, however, is optional ( dyadic only ) */

        if ( !image1 )
		return( 0 );

        image2 = p->finfo.image2;

        if ( image2 )
                flo_elements = 4;
        else
                flo_elements = 3;

        if ( useDomain == DomainROI || useDomain == DomainCtlPlane )
                flo_elements++;

	ControlPlane = ( XiePhotomap ) NULL;
	monoflag = 0;

        if ( xp->screenDepth != image1->depth[ 0 ] )
        {
		monoflag = 1;
		flo_elements+=2;
                if ( ( XIELut = CreatePointLut( xp, p,
                        1 << image1->depth[ 0 ], 1 << xp->screenDepth, False ) )
                        == ( XieLut ) NULL )
		{
			return( 0 );
		}
        }

	rects = ( XieRectangle * ) NULL;
	if ( useDomain == DomainROI )
	{
		rects = (XieRectangle *)
			malloc( numROIs * sizeof( XieRectangle ) );
		if ( rects == ( XieRectangle * ) NULL )
			reps = 0;
		else
		{
			for ( i = 0; i < numROIs; i++ )
			{
				rects[ i ].x = i * 100;
				rects[ i ].y = i * 100;
				rects[ i ].width = 100;
				rects[ i ].height = 100;
			}
		}
        }
	else if ( useDomain == DomainCtlPlane )
	{
		ControlPlane = GetControlPlane( xp, 2 );
		if ( ControlPlane == ( XiePhotomap ) NULL )
			reps = 0;
	}

	flograph = ( XiePhotoElement * ) NULL;
	XIEPhotomap1 = XIEPhotomap2 = ( XiePhotomap ) NULL;
	XIERoi = ( XieRoi ) NULL;
	flo = ( XiePhotoflo ) NULL;
	if ( reps )
	{
		flograph = XieAllocatePhotofloGraph( flo_elements );
		if ( flograph == ( XiePhotoElement * ) NULL )
		{
			fprintf( stderr, "XieAllocatePhotofloGraph failed\n" );
			reps = 0;
		}
		else if ( ( XIEPhotomap1 =
			GetXIEPhotomap( xp, p, 1 ) ) == ( XiePhotomap ) NULL )
		{
			reps = 0;
		}
		else if ( image2 )
		{
			if ( ( XIEPhotomap2 = GetXIEPhotomap( xp, p, 2 ) ) == 
				( XiePhotomap ) NULL )
			{
				reps = 0;
			}
		}

		if ( useDomain == DomainROI )
		{
			if ( ( XIERoi = GetXIERoi( xp, p, rects, numROIs ) ) ==
				( XieRoi ) NULL )
			{
				reps = 0;
			}
		}
               	else if ( useDomain == DomainCtlPlane )
                {
                        ControlPlane = GetControlPlane( xp, 2 );
                        if ( ControlPlane == ( XiePhotomap ) NULL )
                                reps = 0;
                }
	}
	if ( reps )
	{
		idx = 0;

		domain.offset_x = 0;
		domain.offset_y = 0;

		if ( useDomain == DomainROI )
		{
			XieFloImportROI(&flograph[idx], XIERoi);
			idx++;
			domain.phototag = idx;
		}
                else if ( useDomain == DomainCtlPlane )
                {
                        XieFloImportPhotomap(&flograph[idx], ControlPlane, False
);
                        idx++;
                        domain.phototag = idx;
                }
                else
                {
                        domain.phototag = 0;
                }

                XieFloImportPhotomap(&flograph[idx], XIEPhotomap1, False );
                idx++;
                src1 = idx;

                if ( image2 )
                {
                        XieFloImportPhotomap(&flograph[idx], XIEPhotomap2,
                                False );
                        idx++;
                        src2 = idx;
                }
                else
                        src2 = 0;

                XieFloLogical(&flograph[idx],
                        src1,
                        src2,
                        &domain,
                        logicalConstant,
                        logicalOp,
                        bandMask
                );
                idx++;

		if ( monoflag )
		{
                        XieFloImportLUT(&flograph[idx], XIELut );
                        idx++;

                        domain.phototag = 0;
                        domain.offset_x = 0;
                        domain.offset_y = 0;
                        XieFloPoint(&flograph[idx],
                                idx-1,
                                &domain,
                                idx,
                                0x7
                        );
                        idx++;
		}

                XieFloExportDrawable(&flograph[idx],
                        idx,            /* source phototag number */
                        xp->w,
                        xp->fggc,
                        0,       /* x offset in window */
                        0        /* y offset in window */
                );

                idx++;

                flo = XieCreatePhotoflo( xp->d, flograph, flo_elements );
                flo_notify = True;
        }
	if ( !reps )
	{
		FreeLogicalStuff( xp, p );
	}
        return( reps );
}

void DoLogical(xp, p, reps)
    XParms  xp;
    Parms   p;
    int     reps;
{
        int     i;

        for (i = 0; i != reps; i++) {
		XieExecutePhotoflo( xp->d, flo, flo_notify );
		WaitForXIEEvent( xp, xieEvnNoPhotofloDone, flo, 0, False );	
                XSync( xp->d, 0 );
        }
}

int EndLogical(xp, p)
    XParms  xp;
    Parms   p;
{
	FreeLogicalStuff( xp, p );
}

int
FreeLogicalStuff( xp, p )
XParms	xp;
Parms	p;
{
	if ( XIEPhotomap1 && IsPhotomapInCache( XIEPhotomap1 ) == False )
	{
		XieDestroyPhotomap( xp->d, XIEPhotomap1 );
		XIEPhotomap1 = ( XiePhotomap ) NULL;
	}
	if ( XIEPhotomap2 && IsPhotomapInCache( XIEPhotomap2 ) == False )
	{
		XieDestroyPhotomap( xp->d, XIEPhotomap2 );
		XIEPhotomap1 = ( XiePhotomap ) NULL;
	}
        if ( ControlPlane != ( XiePhotomap ) NULL && IsPhotomapInCache( ControlPlane ) == False )
        {
                XieDestroyPhotomap( xp->d, ControlPlane );
                ControlPlane = ( XiePhotomap ) NULL;
        }

	if ( rects )
	{
		free( rects );
		rects = ( XieRectangle * ) NULL;
	}
	if ( XIERoi )
	{
		XieDestroyROI( xp->d, XIERoi );
		XIERoi = ( XieRoi ) NULL;
	}
	if ( XIELut )
	{
		XieDestroyLUT( xp->d, XIELut );
		XIELut = ( XieLut ) NULL;
	}
	if ( flo )
	{
		XieDestroyPhotoflo( xp->d, flo );
		flo = ( XiePhotoflo ) NULL;
	}
	if ( flograph )
	{
		XieFreePhotofloGraph(flograph,flo_elements);
		flograph = ( XiePhotoElement * ) NULL;
	}
}
