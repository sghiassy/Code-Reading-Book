/* $XConsortium: error.h,v 1.4 94/04/17 20:34:03 rws Exp $ */
/* $XFree86: xc/programs/Xserver/XIE/include/error.h,v 3.1 1996/10/16 14:52:30 dawes Exp $ */
/**** module error.h ****/
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

	error.h -- error specific definitions

	Robert NC Shelley -- AGE Logic, Inc. April, 1993

******************************************************************************/

#ifndef _XIEH_ERROR
#define _XIEH_ERROR

#ifndef _XIEC_ERROR
#include <flo.h>
extern	int	 SendResourceError();
extern	int	 SendFloIDError();
extern	int	 SendFloError();
extern	void	 ErrGeneric();
extern	void	 ErrResource();
extern	void	 ErrDomain();
extern	void	 ErrOperator();
extern	void	 ErrTechnique();
extern	void	 ErrValue();
#endif

/*
 *  Convenience macros for dealing with the floDef generic error packet
 */
#define ferrTag(flo)          ((flo)->error.phototag)
#define ferrType(flo)         ((flo)->error.type)
#define ferrCode(flo)         ((flo)->error.floErrorCode)
#define	ferrError(flo,tag,type,code) \
		(ferrTag(flo)=(tag),ferrType(flo)=(type),ferrCode(flo)=(code))

/*
 * convenience macros for general flo errors
 */
#define xieFloError(flo,tag,type,code) \
	FloError(flo,(xieTypPhototag)tag,(CARD16)(long)type,(CARD8)code)

#define	FloAccessError(flo,tag,type,xfer) \
		{xieFloError(flo,tag,type,xieErrNoFloAccess); xfer;}
#define	FloAllocError(flo,tag,type,xfer) \
		{xieFloError(flo,tag,type,xieErrNoFloAlloc); xfer;}
#define	FloElementError(flo,tag,type,xfer) \
		{xieFloError(flo,tag,type,xieErrNoFloElement); xfer;}
#define	FloImplementationError(flo,tag,type,xfer) \
		{xieFloError(flo,tag,type,xieErrNoFloImplementation); xfer;}
#define	FloLengthError(flo,tag,type,xfer) \
		{xieFloError(flo,tag,type,xieErrNoFloLength); xfer;}
#define	FloSourceError(flo,tag,type,xfer) \
		{xieFloError(flo,tag,type,xieErrNoFloSource); xfer;}

/*
 * convenience macros for element-specific flo errors
 */
#define	AccessError(flo,ped,xfer) \
		{ErrGeneric(flo,ped,(CARD8)xieErrNoFloAccess); xfer;}
#define	AllocError(flo,ped,xfer) \
		{ErrGeneric(flo,ped,(CARD8)xieErrNoFloAlloc); xfer;}
#define	ColorListError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloColorList,id); xfer;}
#define	ColormapError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloColormap,id); xfer;}
#define	DomainError(flo,ped,dom,xfer) \
		{ErrDomain(flo,ped,dom); xfer;}
#define	DrawableError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloDrawable,id); xfer;}
#define	ElementError(flo,ped,xfer) \
		{ErrGeneric(flo,ped,(CARD8)xieErrNoFloElement); xfer;}
#define	GCError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloGC,id); xfer;}
#define	ImplementationError(flo,ped,xfer) \
		{ErrGeneric(flo,ped,(CARD8)xieErrNoFloImplementation); xfer;}
#define	LUTError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloLUT,id); xfer;}
#define	MatchError(flo,ped,xfer) \
		{ErrGeneric(flo,ped,(CARD8)xieErrNoFloMatch); xfer;}
#define	OperatorError(flo,ped,op,xfer) \
		{ErrOperator(flo,ped,(CARD8)op); xfer;}
#define	PhotomapError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloPhotomap,id); xfer;}
#define	ROIError(flo,ped,id,xfer) \
		{ErrResource(flo,ped,(CARD8)xieErrNoFloROI,id); xfer;}
#define	SourceError(flo,ped,xfer) \
		{ErrGeneric(flo,ped,(CARD8)xieErrNoFloSource); xfer;}
#define	TechniqueError(flo,ped,group,tech,len,xfer) \
		{ErrTechnique(flo,ped,(CARD8)group,(CARD16)tech,(CARD16)len); \
		   xfer;}
#define	ValueError(flo,ped,value,xfer) \
		{ErrValue(flo,ped,value); xfer;}

#endif /* end _XIEH_ERROR */
