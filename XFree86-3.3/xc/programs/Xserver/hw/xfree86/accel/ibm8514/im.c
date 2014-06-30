/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/ibm8514/im.c,v 3.8 1996/12/23 06:37:54 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN AND TIAGO GONS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL KEVIN E. MARTIN OR TIAGO GONS BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified by Tiago Gons (tiago@comosjn.hobby.nl)
 */
/* $XConsortium: im.c /main/5 1996/10/28 05:30:49 kaleb $ */


#include "os.h"
#include "reg8514.h"
#include "8514im.h"
#include "ibm8514.h"

#define ASM_IMAGE

#define	reorder(a)	( \
	(a & 0x80) >> 7 | \
	(a & 0x40) >> 5 | \
	(a & 0x20) >> 3 | \
	(a & 0x10) >> 1 | \
	(a & 0x08) << 1 | \
	(a & 0x04) << 3 | \
	(a & 0x02) << 5 | \
	(a & 0x01) << 7 )

unsigned short ibm8514stipple_tab[256];

static unsigned short _internal_ibm8514_mskbits[17] = {
	0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff,
	0x01ff, 0x03ff, 0x07ff, 0x0fff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

#define MSKBIT(n) (_internal_ibm8514_mskbits[(n)])

void
ibm8514ImageInit()
{
    int i;

    for (i = 0; i < 256; i++) {
	ibm8514stipple_tab[i] = ((reorder(i) & 0xf0) << 5) |
			 ((reorder(i) & 0x0f) << 1);
    }
}

#ifdef ASM_IMAGE

void
#if NeedFunctionPrototypes
ibm8514ImageFill(
    int			x,
    int			y,
    int			w,
    int			h,
    char		*psrc,
    int			pwidth,
    int			pw,
    int			ph,
    int			pox,
    int			poy,
    short		alu,
    unsigned long	planemask)
#else
ibm8514ImageFill(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			pw;
    int			ph;
    int			pox;
    int			poy;
    short		alu;
    unsigned long	planemask;
#endif
{
    if (alu == MIX_DST)
	return;

    if( pox == 0 && poy == 0 && pw >= x+w && ph >= y+h )
	ibm8514ImageWrite(x, y, w, h, psrc, pwidth, x, y, alu, planemask);
    else
	ibm8514RealImageFill(x, y, w, h, psrc, pwidth,
			     pw, ph, pox, poy, alu, planemask);
}

#else /* ASM_IMAGE */

void
#if NeedFunctionPrototypes
ibm8514ImageWrite(
    int			x,
    int			y,
    int			w,
    int			h,
    char		*psrc,
    int			pwidth,
    int			px,
    int			py,
    short		alu,
    unsigned long	planemask)
#else
ibm8514ImageWrite(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			px;
    int			py;
    short		alu;
    unsigned long	planemask;
#endif
{
    unsigned short *p;
    int i,j;
    int count = (w + 1) >> 1;

    if (alu == MIX_DST)
	return;

    if ((w == 0) || (h == 0))
	return;

    WaitQueue(2);
    outw(FRGD_MIX, FSS_PCDATA | alu);
    outw(WRT_MASK, planemask);
    WaitQueue(7);
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    if (w&1)
	outw(MAJ_AXIS_PCNT, (short)w);
    else
        outw(MAJ_AXIS_PCNT, (short)w-1);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | h-1);
    outw(MULTIFUNC_CNTL, SCISSORS_L | x);
    outw(MULTIFUNC_CNTL, SCISSORS_R | (x+w-1));
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW |
	      PCDATA | WRTDATA | _16BIT | BYTSEQ);
    psrc += pwidth * py + px;
    WaitQueue(8);
				/* ASSUMPTION: it is ok to read one byte past
				   the psrc structure (for odd width). */
    for (j = 0; j < h; j++) {
        p = (unsigned short *)psrc;
        for( i = 0; i < count; i++ )
	    outw( PIX_TRANS, *p++ );

	psrc += pwidth;
    }
    WaitQueue(3);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(MULTIFUNC_CNTL, SCISSORS_L);
    outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
}

void
#if NeedFunctionPrototypes
ibm8514ImageRead(
    int			x,
    int			y,
    int			w,
    int			h,
    char		*psrc,
    int			pwidth,
    int			px,
    int			py,
    unsigned long	planemask)
#else
ibm8514ImageRead(x, y, w, h, psrc, pwidth, px, py, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			px;
    int			py;
    unsigned long	planemask;
#endif
{
    int i,j;
    unsigned short *sp;


    WaitQueue(6);
    outw(FRGD_MIX, FSS_PCDATA | MIX_SRC);
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    if( w & 1 )
      outw(MAJ_AXIS_PCNT, (short)w);
    else
      outw(MAJ_AXIS_PCNT, (short)w-1);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | h-1);
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | BYTSEQ |_16BIT | PCDATA);
    sp = (unsigned short*)(psrc + pwidth * py + px);
    pwidth = pwidth / 2 - w / 2;
    planemask = (planemask & 0x00ff) | ((planemask << 8) & 0xff00);
    WaitDataReady();
    if( w & 1 ) {
        w /= 2;
        for (j = 0; j < h; j++) {
            for (i = 0; i < w; i++)
	        *sp++ = inw(PIX_TRANS) & planemask;
	    *((unsigned char *)(sp)) = (unsigned char)(inw(PIX_TRANS))
					& (unsigned char)(planemask);
	    sp += pwidth;
	}
    }
    else {
        w /= 2;
        for (j = 0; j < h; j++) {
            for (i = 0; i < w; i++)
	        *sp++ = inw(PIX_TRANS) & planemask;
	    sp += pwidth;
	}
    }
    WaitQueue(1);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
}

void
#if NeedFunctionPrototypes
ibm8514ImageFill(
    int			x,
    int			y,
    int			w,
    int			h,
    char		*psrc,
    int			pwidth,
    int			pw,
    int			ph,
    int			pox,
    int			poy,
    short		alu,
    unsigned long	planemask)
#else
ibm8514ImageFill(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			pw;
    int			ph;
    int			pox;
    int			poy;
    short		alu;
    unsigned long	planemask;
#endif
{
    int srcxsave, srcx, srcy, dstw, srcw, srch;
    int wtemp, count, i, j;
    unsigned short btemp, *p;


    if (alu == MIX_DST)
	return;

    if( pox == 0 && poy == 0 && pw >= x+w && ph >= y+h )
      ibm8514ImageWrite(x, y, w, h, psrc, pwidth, x, y, alu, planemask);
    else {
      WaitQueue(2);
      outw(FRGD_MIX, FSS_PCDATA | alu);
      outw(WRT_MASK, planemask);
      WaitQueue(7);
      outw(CUR_X, (short)x);
      outw(CUR_Y, (short)y);
      if (w&1)
	outw(MAJ_AXIS_PCNT, (short)w);
      else
        outw(MAJ_AXIS_PCNT, (short)w-1);
      outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | h-1);
      outw(MULTIFUNC_CNTL, SCISSORS_L | x);
      outw(MULTIFUNC_CNTL, SCISSORS_R | (x+w-1));
      outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW |
		PCDATA | WRTDATA | _16BIT | BYTSEQ);
      WaitQueue(8);
      modulus(x-pox,pw,srcxsave);
      modulus(y-poy,ph,srcy);
      while( h > 0 ) {
	srch = ( srcy+h > ph ? ph - srcy : h );
	for( i = 0; i < srch; i++ ) {
	  dstw = w;
	  srcx = srcxsave;
	  srcw = ( srcx+w > pw ? pw - srcx : w );
	  wtemp = 0;
	  while( dstw > 0 ) {
	    p = (unsigned short *)((unsigned char *)(psrc + pwidth * srcy + srcx));
	    if( wtemp & 1 ) {
	      outw( PIX_TRANS, (btemp & 0x00ff) | (*p << 8 ) );
	      p = (unsigned short *)((unsigned char *)(p)++);
	      wtemp = srcw - 1;
	    }
	    else
	      wtemp = srcw;

	    count = wtemp / 2;
	    for( j = 0; j < count; j++ )
	      outw( PIX_TRANS, *p++ );

	    dstw -= srcw;
	    srcx = 0;
	    if( wtemp & 1 ) {
	      if( dstw != 0 )
		btemp = *p;
	      else
		outw( PIX_TRANS, *p );
	    }
	    srcw = ( dstw < pw ? dstw : pw );
          }
	  srcy++;
	  h--;
	}
	srcy = 0;
      }
      WaitQueue(3);
      outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
      outw(MULTIFUNC_CNTL, SCISSORS_L);
      outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
    }
}

void
#if NeedFunctionPrototypes
ibm8514ImageStipple(
    int			x,
    int			y,
    int			w,
    int			h,
    char		*psrc,
    int			pwidth,
    int			pw, 
    int			ph, 
    int			pox, 
    int			poy,
    Pixel		fgPixel,
    Pixel		bgPixel,
    short		alu,
    unsigned long	planemask,
    int			opaque)
#else
ibm8514ImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
			fgPixel, bgPixel, alu, planemask, opaque)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			pw, ph, pox, poy;
    Pixel		fgPixel;
    Pixel		bgPixel;
    short		alu;
    unsigned long	planemask;
    int			opaque;
#endif
{
    int			srcx, srch, dstw;
    unsigned short	*ptmp;


    if (alu == MIX_DST || w == 0 || h == 0)
	return;

    WaitQueue(6);
    outw(MULTIFUNC_CNTL, SCISSORS_L | x);
    outw(MULTIFUNC_CNTL, SCISSORS_R | (x+w-1));
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | h-1);
    outw(WRT_MASK, planemask);
    outw(FRGD_MIX, FSS_FRGDCOL | alu);
    WaitQueue(7);
    if( opaque ) {
	outw(BKGD_MIX, BSS_BKGDCOL | alu);
	outw(BKGD_COLOR, (short)bgPixel);
    }
    else
	 outw(BKGD_MIX, BSS_BKGDCOL | MIX_DST);
    outw(FRGD_COLOR, (short)fgPixel);
    w += (x & 3);			/* Adjust for nibble mode. */
    x &= ~3;
    outw(MAJ_AXIS_PCNT, (short)(((w + 7) & ~7)-1));
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    outw(CMD, CMD_RECT | PCDATA | _16BIT | INC_Y | INC_X |
	      YMAJAXIS | DRAW | PLANAR | WRTDATA);
    modulus(x - pox, pw, x);
    modulus(y - poy, ph, y);
    WaitQueue(8);
    /*
     * When the source bitmap is properly aligned, max 16 pixels wide,
     * and nonrepeating use this faster loop instead.
     * This speeds up all copying to the font cache.
     */
    if( (x & 7) == 0 && w <= 16 && x+w <= pw && y+h <= ph ) {
	unsigned short pix;
	unsigned char *pnt;

	pnt = (unsigned char *)(psrc + pwidth * y + (x >> 3));
	while( h-- > 0 ) {
	    pix = *((unsigned short *)(pnt));
	    outw( PIX_TRANS, ibm8514stipple_tab[ pix & 0xff ] );
	    if( w > 8 )
		outw( PIX_TRANS, ibm8514stipple_tab[ ( pix >> 8 ) & 0xff ] );

	    pnt += pwidth;
	}
    }
    else {
	while( h > 0 ) {
	    srch = ( y+h > ph ? ph - y : h );
	    while( srch > 0 ) {
		dstw = w;
		srcx = x;
		ptmp = (unsigned short *)(psrc + pwidth * y);
		while( dstw > 0 ) {
		    int np, x2;
		    unsigned short *pnt, pix;
		    /*
		     * Assemble 16 bits and feed them to the draw engine.
		     */
		    np = pw - srcx;		/* No. pixels left in bitmap.*/
		    pnt =(unsigned short *)
				       ((unsigned char *)(ptmp) + (srcx >> 3));
		    x2 = srcx & 7;		/* Offset within byte. */
		    if( np >= 16 ) {
			pix = (unsigned short)(*((unsigned int *)(pnt)) >> x2);
		    }
		    else if( pw >= 16 ) {
			pix = (unsigned short)((*((unsigned int *)(pnt)) >> x2)
						 & MSKBIT(np)) | (*ptmp << np);
		    }
		    else if( pw >= 8 ) {
			pix = ((*pnt >> x2) & MSKBIT(np))
			   | ((*ptmp & MSKBIT(pw)) << np) | (*pnt << (np+pw));
		    }
		    else {
			pix = (*ptmp >> x2) & MSKBIT(np);
			while( np < 16 && np < dstw ) {
			    pix |= (*ptmp & MSKBIT(pw)) << np;
			    np += pw;
			}
		    }
		    outw( PIX_TRANS, ibm8514stipple_tab[ pix & 0xff ] );
		    if( dstw > 8 )
			outw( PIX_TRANS,
			      ibm8514stipple_tab[ ( pix >> 8 ) & 0xff ] );
		    srcx += 16;
		    if( srcx >= pw )
			srcx -= pw;
		    dstw -= 16;
		}
		y++;
		h--;
		srch--;
	    }
	    y = 0;
	}
    }
    WaitQueue(5);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(BKGD_MIX, BSS_BKGDCOL | MIX_SRC);
    outw(MULTIFUNC_CNTL, SCISSORS_L | 0);
    outw(MULTIFUNC_CNTL, SCISSORS_R | 1023);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
}

#endif /* ASM_IMAGE */

void
ibm8514FontOpStipple(x, y, w, h, psrc, pwidth, id)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    Pixel		id;
{

    ibm8514ImageStipple(x, y, w, h, psrc, pwidth, w, h, x, y,
		      ~0, 0, ibm8514alu[GXcopy], 1 << id, 1 );
}
