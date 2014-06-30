/* $XConsortium: tex.h,v 1.4 94/04/17 20:35:06 rws Exp $ */
/**** module tex.h ****/
/****************************************************************************

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
******************************************************************************
  
	tex.h -- generic device dependent photoflo definitions
  
	Dean Verheiden -- AGE Logic, Inc. April 1993
  
*****************************************************************************/

#ifndef _XIEH_TEX
#define _XIEH_TEX

/* symbolic constants -- for flo manager initialization routines
 */
	/* Band mask constants for specifying all posible bands or no bands.
	 */
#define ALL_BANDS  ((bandMsk)~0)
#define NO_BANDS   ((bandMsk) 0)

	/* Constant for specifying that in-place operations are not wanted
	 * (i.e. use new Dst buffers).  The alternative is to specify a Src
	 * (e.g. SRCtag or SRCt1) whose buffer contents can be replaced with
	 * the result.  When requesting a Dst line, the data manager will map
	 * the requested Dst line to the corresponding Src line (the Src and
	 * Dst canonic data types must match for this to work).
	 */
#define NO_INPLACE ((INT32)-1)

	/* Constant for specifying that no space needs to be allotted for a
	 * data manager DataMap (i.e. data will be accessed on a line by line
	 * basis).  The alternative is to specify the maximum number of lines
	 * that will be needed in the DataMap (anywhere from a few lines to
	 * the full image).
	 */
#define NO_DATAMAP ((CARD32) 0)

	/* Constant for specifying that the element does not require any
	 * private parameter space.  The alternative is to specify the number
	 * of bytes of private storage that will be needed for parameters etc.
	 * The private area is allocated as contiguous bytes beyond the peTex
	 * structure, and therefore should not be freed explicitly.
	 */
#define NO_PRIVATE ((CARD32) 0)

	/* Constants for specifying whether or not the data manager and
	 * scheduler should cooperate to keep inputs and/or bands in sync.
	 */
#define SYNC	   ((Bool) 1)
#define NO_SYNC	   ((Bool) 0)


/* symbolic constants -- for data manager macros
 */
	/* The following pair of constants can be supplied for the "purge"
	 * argument in many of the data manager's strip access macros.
	 *
	 * KEEP specifies that all data currently owned by the Src or Dst
	 * should be retained (either the data will be needed again or there
	 * is no advantage in releasing the data as it is consumed).
	 *
	 * FLUSH specifies that all data that precedes the current unit (line
	 * or byte) can be dispensed with.  Src data may be freed, whereas Dst
	 * data may be forwarded to downstream elements.  FLUSH is a suggestion
	 * rather than a command.  Whether or not the data is actually flushed
	 * depends on the crossing of strip boundaries.  If one or more strips
	 * precede the strip containing the current unit, they will be flushed.
	 * If a downstream element becomes runnable as a result of forwarding
	 * a Dst strip, the data manager will signal to the calling element
	 * that it should defer to the downstream element(s).
	 */
#define KEEP	((Bool) 0)
#define FLUSH	((Bool) 1)


#ifndef _XIEC_FLOMAN
extern	int	InitFloManager();
extern	int	MakePETex();
extern	Bool	InitReceptors();
extern	Bool	InitReceptor();
extern	Bool	InitEmitter();
extern	Bool	InitBand();
extern	void	ResetReceptors();
extern	void	ResetEmitter();
#endif

#ifndef _XIEC_SCHED
extern	int	InitScheduler();
#endif

#ifndef _XIEC_STRIP
extern	int	InitStripManager();
extern  int	DebriefStrips();
extern  void	FreeStrips();
#endif

#endif /* end _XIEH_TEX */
