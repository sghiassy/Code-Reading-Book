/* $XConsortium: tables.h,v 1.4 94/04/17 20:34:08 rws Exp $ */
/**** module tables.h ****/
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
*******************************************************************************

	tables.h: entry points etc.

	Dean Verheiden, Robert NC Shelley -- AGE Logic, Inc. April 1993

******************************************************************************/

#ifndef _XIEH_TABLES
#define _XIEH_TABLES

#include <flostr.h>

#define DDAnalyzeIndex		0
#define DDServerChoiceIndex	1

#ifndef _XIEC_TABLES

extern	void		init_proc_tables();     
extern	peDefPtr	(*MakeTable[])();
extern  xieVoidProc 	DDInterface[];

#else  /* _XIEC_TABLES */

	/* dd entry points for di */

extern int DAGalyze();
extern xieBoolProc GetServerChoice();

	/* flo.c */
extern  int		ProcAbort();
extern  int		ProcAwait();
extern  int		ProcGetClientData();
extern  int		ProcPutClientData();
extern  int		ProcQueryPhotoflo();
extern  int		SProcAbort();
extern  int		SProcAwait();
extern  int		SProcGetClientData();
extern  int		SProcPutClientData();
extern  int		SProcQueryPhotoflo();
	/* immediate.c */
extern	int		ProcCreatePhotospace();
extern	int		ProcDestroyPhotospace();
extern  int		ProcExecuteImmediate();
extern	int		SProcCreatePhotospace();
extern	int		SProcDestroyPhotospace();
extern  int		SProcExecuteImmediate();
/* lut.c */
extern	int		ProcCreateLUT();
extern	int		ProcDestroyLUT();
extern	int		SProcCreateLUT();
extern	int		SProcDestroyLUT();
	/* photomap.c */
extern	int		ProcCreatePhotomap();
extern	int		ProcDestroyPhotomap();
extern	int		ProcQueryPhotomap();
extern	int		SProcCreatePhotomap();
extern	int		SProcDestroyPhotomap();
extern	int		SProcQueryPhotomap();
	/* stored.c */
extern	int		ProcCreatePhotoflo();
extern	int		ProcDestroyPhotoflo();
extern	int		ProcExecutePhotoflo();
extern	int		ProcModifyPhotoflo();
extern	int		ProcRedefinePhotoflo();
extern	int		SProcCreatePhotoflo();
extern	int		SProcDestroyPhotoflo();
extern	int		SProcExecutePhotoflo();
extern	int		SProcModifyPhotoflo();
extern	int		SProcRedefinePhotoflo();
	/* technq.c */
extern	int		ProcQueryTechniques();
extern	int		SProcQueryTechniques();

#if XIE_FULL
	/* colorlist.c */
extern	int		ProcCreateColorList();
extern	int		ProcDestroyColorList();
extern	int		ProcPurgeColorList();
extern	int		ProcQueryColorList();
extern	int		SProcCreateColorList();
extern	int		SProcDestroyColorList();
extern	int		SProcPurgeColorList();
extern	int		SProcQueryColorList();
	/* roi.c */
extern	int		ProcCreateROI();
extern	int		ProcDestroyROI();
extern	int		SProcCreateROI();
extern	int		SProcDestroyROI();
#endif

/* elements */
extern	peDefPtr	MakeIDraw();
extern	peDefPtr	MakeIDrawP();
extern	peDefPtr	MakeICLUT();
extern	peDefPtr	MakeILUT();
extern	peDefPtr	MakeICPhoto();
extern	peDefPtr	MakeIPhoto();
extern	peDefPtr	MakeGeometry();
extern	peDefPtr	MakePoint();
extern	peDefPtr	MakeEDraw();
extern	peDefPtr	MakeEDrawPlane();
extern	peDefPtr	MakeECLUT();
extern	peDefPtr	MakeELUT();
extern	peDefPtr	MakeECPhoto();
extern	peDefPtr	MakeEPhoto();

#if XIE_FULL
extern	peDefPtr	MakeICROI();
extern	peDefPtr	MakeIROI();
extern	peDefPtr	MakeArith();
extern	peDefPtr	MakeBandCom();
extern	peDefPtr	MakeBandExt();
extern	peDefPtr	MakeBandSel();
extern	peDefPtr	MakeBlend();
extern	peDefPtr	MakeCompare();
extern	peDefPtr	MakeConstrain();
extern	peDefPtr	MakeConvertFromIndex();
extern	peDefPtr	MakeConvertFromRGB();
extern	peDefPtr	MakeConvertToIndex();
extern	peDefPtr	MakeConvertToRGB();
extern	peDefPtr	MakeConvolve();
extern	peDefPtr	MakeDither();
extern	peDefPtr	MakeLogic();
extern	peDefPtr	MakeMatchHistogram();
extern	peDefPtr	MakeMath();
extern	peDefPtr	MakePasteUp();
extern	peDefPtr	MakeUnconstrain();
extern	peDefPtr	MakeECHistogram();
extern	peDefPtr	MakeECROI();
extern	peDefPtr	MakeEROI();
#endif

#endif /* _XIEC_TABLES */

#endif /* _XIEH_TABLES */
