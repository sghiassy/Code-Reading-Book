/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcBitmap.c,v 3.4 1996/12/23 06:52:53 dawes Exp $ */
/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

/* $XConsortium: ppcBitmap.c /main/4 1996/02/21 17:57:13 kaleb $ */

#include "X.h"
#include "pixmap.h"
#include "pixmapstr.h"

#include "maskbits.h"

#include "OScompiler.h"

/* ppcQuickBlt -- a quick and dirty bitblit routine
 * copies from psrcBase(xSrc, ySrc) to pdstBase(xDst, yDst) an array of bits
 * w wide by h high.  It is assumed that psrcBase and pdstBase point at 
 * things reasonable to bitblt between. It is assumed that all clipping has
 * been done.  It is also assumed that the rectangle fits on the destination
 * (that is, that (pdstBase + (yDst * h) + w) is a bit in the destination).
 * This routine does no error-checking of any type, CAVEAT HACKER 
 */
void
ppcQuickBlt(psrcBase, pdstBase, xSrc, ySrc, xDst, yDst, w, h, wSrc, wDst)
    int  *psrcBase, *pdstBase,	/* first bits in pixmaps */
         xSrc, ySrc,		/* origin of source */
	 xDst, yDst,		/* origin for destination */
	 w,			/* width to blt  */
	 h,			/* height to blt */
	 wSrc, wDst;		/* width of pixmaps */
{

    int *psrcLine, *pdstLine;
    int xdir, ydir;
    int nl, startmask, endmask, nlMiddle, *psrc, *pdst, tmpSrc;

    if (psrcBase + wSrc * h < pdstBase ||
       pdstBase + wDst * h < psrcBase)
    {
	/* the areas don't overlap  right and down is fine */
	xdir = ydir = 1;
    }
    else if (ySrc < yDst) /* move right and up */
    {
	xdir = 1;
	ydir = -1;
    }
    else if (ySrc > yDst) /* move right and down */
    {
	xdir = 1;
	ydir = 1;
    }
    else if (xSrc < xDst) /* move left and down */
    {
	xdir = -1;
	ydir = 1;
    }
    else /* if xSrc <= xDst */ /* move right and down */
    {
	xdir = 1;
	ydir = 1;
    }

    if (ydir == -1) /* start at last scanline of rectangle */
    {
	psrcLine = psrcBase + (((ySrc + h) -1) * wSrc);
	pdstLine = pdstBase + (((yDst + h) -1) * wDst);
	wSrc = -wSrc;
	wDst = -wDst;
    }
    else /* start at first scanline */
    {
	psrcLine = psrcBase + (ySrc * wSrc);
	pdstLine = pdstBase + (yDst * wDst);
    }

    /* x direction doesn't matter for < 1 longword */
    if (w <= 32)
    {
	int srcBit, dstBit;	/* bit offset of src and dst */

	pdstLine += (xDst >> 5);
	psrcLine += (xSrc >> 5);
	psrc = psrcLine;
	pdst = pdstLine;

	srcBit = xSrc & 0x1f;
	dstBit = xDst & 0x1f;

	while ( h-- )
	{
	    getbits(psrc, srcBit, w, tmpSrc);
	    putbits(tmpSrc, dstBit, w, pdst);
	    psrc += wSrc;
	    pdst += wDst;
	}
    }
    else
    {
	register int xoffSrc;	/* offset (>= 0, < 32) from which to
				   fetch whole longwords fetched 
				   in src */
	int nstart;		/* number of ragged bits 
				   at start of dst */
	int nend;		/* number of ragged bits at end 
				   of dst */
	int srcStartOver;	/* pulling nstart bits from src
				   overflows into the next word? */

	maskbits(xDst, w, startmask, endmask, nlMiddle);
	if (startmask)
	    nstart = 32 - (xDst & 0x1f);
	else
	    nstart = 0;
	if (endmask)
	    nend = (xDst + w)  & 0x1f;
	else
	    nend = 0;

	xoffSrc = ((xSrc & 0x1f) + nstart) & 0x1f;
	srcStartOver = ((xSrc & 0x1f) + nstart) > 31;

	if (xdir == 1) /* move left to right */
	{
	    pdstLine += (xDst >> 5);
	    psrcLine += (xSrc >> 5);

	    while (h--)
	    {
		psrc = psrcLine;
		pdst = pdstLine;

		if (startmask)
		{
		    getbits(psrc, (xSrc & 0x1f), nstart, tmpSrc);
		    putbits(tmpSrc, (xDst & 0x1f), nstart, pdst);
		    pdst++;
		    if (srcStartOver)
			psrc++;
		}

		nl = nlMiddle;
		while (nl--)
		{
		    getbits(psrc, xoffSrc, 32, tmpSrc);
		    *pdst++ = tmpSrc;
		    psrc++;
		}

		if (endmask)
		{
		    getbits(psrc, xoffSrc, nend, tmpSrc);
		    {
			int tmpmask ;
			maskpartialbits( 0, nend, tmpmask ) ;
			*pdst = ( *pdst & ~tmpmask ) | ( tmpSrc & tmpmask ) ;
		    }
		}

		psrcLine += wSrc;
		pdstLine += wDst;
	    }
	}
	else /* move right to left */
	{
	    pdstLine += ((xDst + w) >> 5);
	    psrcLine += (xSrc + w >> 5);
	    /* if fetch of last partial bits from source crosses
	       a longword boundary, start at the previous longword
	    */
	    if (xoffSrc + nend >= 32)
		--psrcLine;

	    while (h--)
	    {
		psrc = psrcLine;
		pdst = pdstLine;

		if (endmask)
		{
		    getbits(psrc, xoffSrc, nend, tmpSrc)
		    {
			int tmpmask ;
			maskpartialbits( 0, nend, tmpmask ) ;
			*pdst = ( *pdst & ~tmpmask ) | ( tmpSrc & tmpmask ) ;
		    }
		}

		nl = nlMiddle;
		while (nl--)
		{
		    --psrc;
		    getbits(psrc, xoffSrc, 32, tmpSrc)
		    *--pdst = tmpSrc;
		}

		if (startmask)
		{
		    if (srcStartOver)
			--psrc;
		    --pdst;
		    getbits(psrc, (xSrc & 0x1f), nstart, tmpSrc)
		    putbits(tmpSrc, (xDst & 0x1f), nstart, pdst)
		}

		pdstLine += wDst;
		psrcLine += wSrc;
	    }
	} /* move right to left */
    }
    return ;
}

/* Rotates pixmap pPix by w pixels to the right on the screen. Assumes that
 * words are 32 bits wide, and that the least significant bit appears on the
 * left.
 */
void
ppcRotBitmapRight(pPix, rw)
    register PixmapPtr	pPix;
    register int rw;
{
    register long	*pw, *pwFinal, *pwNew;
    register unsigned long	t;
    int			size;

    if ( (pPix == NullPixmap) || ( rw == 0 ) )
        return;

    pw = (long *)pPix->devPrivate.ptr;
    rw %= pPix->drawable.width;
    if (rw < 0)
	rw += pPix->drawable.width;
    if ( pPix->drawable.width == 32 )
    {
        pwFinal = pw + pPix->drawable.height;
	while ( pw < pwFinal )
	{
	    t = *pw;
	    *pw++ = SCRRIGHT(t, rw) | 
		    (SCRLEFT(t, (32-rw)) & endtab[rw]);
	}
    }
    else
    {
	int sz = pPix->drawable.height * pPix->devKind;
	pwNew = (long *) ALLOCATE_LOCAL( sz );
	if ( !pwNew )
		return;

	/* o.k., divide pw (the pixmap) in two vertically at (w - rw)
	 * pick up the part on the left and make it the right of the new
	 * pixmap.  then pick up the part on the right and make it the left
	 * of the new pixmap.
	 * now hook in the new part and throw away the old. All done.
	 */
	size = PixmapWidthInPadUnits(pPix->drawable.width, 1) ;
        ppcQuickBlt((int *)pw, (int *)pwNew, 0, 0, rw, 0,
		    pPix->drawable.width - rw, pPix->drawable.height,
		    size, size);
        ppcQuickBlt((int *)pw, (int *)pwNew, pPix->drawable.width - rw,
		    0, 0, 0, rw, pPix->drawable.height, size, size);
	MOVE( pwNew, pPix->devPrivate.ptr, sz );
	DEALLOCATE_LOCAL( pwNew );

    }
    return ;
}

void
ppcRotBitmapDown(pPix, rh)
    register PixmapPtr	pPix;
    register int	rh;
{
    int nbyDown;	/* bytes to move down to row 0; also offset of
			   row rh */
    int nbyUp;		/* bytes to move up to line rh; also
			   offset of first line moved down to 0 */
    register char *pbase;
    register char *ptmp;

    if (pPix == NullPixmap)
	return;
    rh %= pPix->drawable.height;
    if (rh < 0)
	rh += pPix->drawable.height;

    nbyDown = rh * pPix->devKind;
    nbyUp = (pPix->devKind * pPix->drawable.height) - nbyDown;
    if ( !( ptmp = (char *) ALLOCATE_LOCAL( nbyUp ) ) )
	return;

    pbase = (char *) pPix->devPrivate.ptr;

    MOVE(pbase, ptmp, nbyUp);		/* save the low rows */
    MOVE(pbase+nbyUp, pbase, nbyDown);	/* slide the top rows down */
    MOVE(ptmp, pbase+nbyDown, nbyUp);	/* move lower rows up to row rh */
    DEALLOCATE_LOCAL(ptmp);
    return ;
}
