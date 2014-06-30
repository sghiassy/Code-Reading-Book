/* $XConsortium: nglecopy.c,v 1.1 93/08/08 12:56:54 rws Exp $ */

/*
****************************************************************************
** DDX WINDOW ROUTINES
**
** (c) Copyright Hewlett-Packard Company, 1992.
Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.
**
****************************************************************************
*/

#include "ngle.h"

/* Extern decalarations for CFB routines */
/* Depth 8 CFB functions */
extern int cfbDoBitbltGeneral();
extern int cfbDoBitbltCopy();
extern int cfbDoBitbltXor();
extern int cfbDoBitbltOr();
extern RegionPtr cfbBitBlt();

/* Depth 24 CFB functions */
extern int cfb32DoBitbltGeneral();
extern int cfb32DoBitbltCopy();
extern int cfb32DoBitbltXor();
extern int cfb32DoBitbltOr();
extern RegionPtr cfb32BitBlt();

/* Ngle routines */

extern int ngleBitBlt();

/***************************************************************************
 *
 *  ngleCopyArea8()
 *
 *  This routine is a replacement for cfbCopyArea(), to allow us to
 *  call our own block move routine when we are in the scrolling
 *  case.
 *
 ***************************************************************************
 */

RegionPtr ngleCopyArea8(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    int	(*doBitBlt) ();
    
    doBitBlt = cfbDoBitbltCopy;

    if (( pSrcDrawable == pDstDrawable) &&
	    (pSrcDrawable->type == DRAWABLE_WINDOW) &&
	    (pDstDrawable->type == DRAWABLE_WINDOW))
    {
	doBitBlt = ngleBitBlt;
    }

    if (pGC->alu != GXcopy || (pGC->planemask & 0xff) != 0xff)
    {
	doBitBlt = cfbDoBitbltGeneral;
	if ((pGC->planemask & 0xff) == 0xff)
	{
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfbDoBitbltXor;
		break;
	    case GXor:
		doBitBlt = cfbDoBitbltOr;
		break;
	    }
	}
    }

    return cfbBitBlt (pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0);
}


/***************************************************************************
 *
 *  ngleCopyArea24()
 *
 *  This routine is a replacement for cfb32CopyArea(), to allow us to
 *  call our own block move routine when we are in the scrolling
 *  case.
 *
 ***************************************************************************
 */

RegionPtr ngleCopyArea24(pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty)
    register DrawablePtr pSrcDrawable;
    register DrawablePtr pDstDrawable;
    GC *pGC;
    int srcx, srcy;
    int width, height;
    int dstx, dsty;
{
    int	(*doBitBlt) ();
    
    doBitBlt = cfb32DoBitbltCopy;

    if (( pSrcDrawable == pDstDrawable) &&
	    (pSrcDrawable->type == DRAWABLE_WINDOW) &&
	    (pDstDrawable->type == DRAWABLE_WINDOW))
    {
	doBitBlt = ngleBitBlt;
    }

    if (pGC->alu != GXcopy || (pGC->planemask & 0xffffffff) != 0xffffffff)
    {
	doBitBlt = cfb32DoBitbltGeneral;
	if ((pGC->planemask & 0xffffffff) == 0xffffffff)
	{
	    switch (pGC->alu) {
	    case GXxor:
		doBitBlt = cfb32DoBitbltXor;
		break;
	    case GXor:
		doBitBlt = cfb32DoBitbltOr;
		break;
	    }
	}
    }

    return cfb32BitBlt (pSrcDrawable, pDstDrawable,
            pGC, srcx, srcy, width, height, dstx, dsty, doBitBlt, 0);
}
