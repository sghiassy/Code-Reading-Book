/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/p9000im.c,v 3.10 1996/12/23 06:40:44 dawes Exp $ */
/*
 * Copyright 1992,1993 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * CHRIS MASON, ERIK NYGREN, KEVIN E. MARTIN, RICKARD E. FAITH, TIAGO
 * GONS, AND CRAIG E. GROESCHEL DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL ANY OR ALL OF THE AUTHORS BE LIABLE
 * FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Rik Faith (faith@cs.unc.edu), Mon Jul  5 20:00:01 1993:
 *   Added 16-bit and some 64kb aperture optimizations.
 *   Waffled in stipple optimization by Jon Tombs (from s3/s3im.c)
 *   Added outsw code.
 * Modified for the P9000 by Chris Mason (mason@mail.csh.rit.edu)
 * */
/* $XConsortium: p9000im.c /main/9 1996/02/21 17:32:37 kaleb $ */

#include "X.h"
#include "misc.h"
#include "xf86.h"
#include "xf86_HWlib.h"
#include "p9000.h"
#include "p9000reg.h"
#include "p9000im.h"

#ifdef P9000_ACCEL

void (*p9000ImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);

void (*p9000ImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);

static	void	p9000ImageRead(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long 
#endif
);

static	void	p9000ImageWrite(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);

static	void	p9000ImageReadNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static	void	p9000ImageWriteNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);


static unsigned long PMask;
static int screenStride;

void
p9000ImageInit()
{
    int i;

    PMask = (1UL << p9000InfoRec.depth) - 1;
    screenStride = p9000InfoRec.virtualX * p9000BytesPerPixel;

    p9000ImageReadFunc = p9000ImageRead;
    p9000ImageWriteFunc = p9000ImageWrite;
    /* p9000ImageFillFunc = p9000ImageFill; */
}

static void
#if NeedFunctionPrototypes
p9000ImageRead(
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
p9000ImageRead(x, y, w, h, psrc, pwidth, px, py, planemask)
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    unsigned char       *psrc;
    int                 pwidth;
    int                 px;
    int                 py;
    unsigned long	planemask;
#endif
{
    register int j;
    register unsigned char *curvm;

    if ((w == 0) || (h == 0))
        return;

    if ((planemask & PMask) != PMask) { 
        p9000ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask);
        return;
    }

    psrc += pwidth * py + px * p9000BytesPerPixel;
    curvm = (unsigned char *)((int)VidBase + x * p9000BytesPerPixel);

    for (j = y; j < y+h; j++) {
        BusToMem(psrc, (void *)(curvm + j * screenStride), 
		 w * p9000BytesPerPixel);
      psrc += pwidth;
    }
}

static void
#if NeedFunctionPrototypes
p9000ImageReadNoMem(
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
p9000ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask)
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    unsigned char       *psrc;
    int                 pwidth;
    int                 px;
    int                 py;
    unsigned long	planemask;
#endif
{
    register int i, j;
    int pix;
    short word1, word2; 

    w += px;
    psrc += pwidth * py;
    
    if (p9000BytesPerPixel == 1) {
        for (j = 0; j < h; j++) {
            for (i = px; i < w; i += 2) {
                pix = p9000Fetch((i + y * p9000InfoRec.virtualX),VidBase);
                word1 = (pix << 16);
                word1 = (pix >> 16);
                word2 = (pix >> 16);
                psrc[i] = word1;
                psrc[i+1] = word2;
            }
            psrc += pwidth;
        }

    }
}

static void
#if NeedFunctionPrototypes
p9000ImageWrite(
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
p9000ImageWrite(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			px;
    int			py;
    short		alu; /* pGC->alu */
    unsigned long	planemask;
#endif
{
    unsigned char *curvm;
    int byteCount;

    if ((w == 0) || (h == 0))
	return;

    if (p9000alu[alu] == IGM_D_MASK)
	return;

    if ((p9000alu[alu] != IGM_S_MASK) | ((planemask&PMask) != PMask)) {
	p9000ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py,
			      alu, planemask);
	return;
    }
       
    psrc += pwidth * py + px * p9000BytesPerPixel;
    curvm = (unsigned char *)((int)VidBase +
                            (x+y * p9000InfoRec.virtualX) * p9000BytesPerPixel);
    
    byteCount = w * p9000BytesPerPixel;
    while(h--) {
        MemToBus((void *)curvm, psrc, byteCount);
        curvm += screenStride; 
        psrc += pwidth;
    }
}

static void
#if NeedFunctionPrototypes
p9000ImageWriteNoMem(
    int			x,
    int			y,
    int			w,
    int			h,
    register char	*psrc,
    int			pwidth,
    int			px,
    int			py,
    short		alu,
     unsigned long	planemask)
#else
p9000ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    register char       *psrc;
    int			pwidth;
    int			px;
    int			py;
    short		alu; /* pGC->alu */
    unsigned long	planemask;
#endif
{
    register int i,j;
    psrc += pwidth * py;
    p9000NotBusy();
    p9000Store(RASTER,CtlBase,p9000alu[alu]);
    p9000Store(PMASK,CtlBase,planemask);


    p9000Store(DEVICE_COORD | DC_YX | DC_2,CtlBase,YX_PACK(x,y));
    p9000Store(DEVICE_COORD | DC_Y | DC_3,CtlBase,1);
    p9000Store(CMD_NEXT_PIXELS,CtlBase,w);
    
    /* ASSUMPTION: it is ok to read one byte past
       the psrc structure (for odd width). */

    for (j=0; j < h; j++) {
        for (i=0;i<(w);i+=4) {
          p9000Store(CMD_PIXEL8_SWAP,CtlBase,*((int *)(psrc + (px + i) * 
	             p9000BytesPerPixel)));
        }
        psrc += pwidth;
    }
    p9000NotBusy();

}

static unsigned testalu[] = {
    0,	/* 0 */
    IGM_S_MASK & IGM_F_MASK & IGM_D_MASK, /* GXand src AND dst  */
    IGM_S_MASK & IGM_F_MASK & ~IGM_D_MASK, /* GXandReverse src AND NOT dst  */
    IGM_S_MASK & IGM_F_MASK, 	      /* GXcopy src */
    (IGM_S_MASK & ~IGM_F_MASK) & IGM_D_MASK, /* GXandInverted  NOT src AND dst*/
    IGM_D_MASK, 	      /* GXnoop */
    (IGM_S_MASK & IGM_F_MASK) ^ IGM_D_MASK, /* GXxor src ^ dst */
    (IGM_S_MASK & IGM_F_MASK) | IGM_D_MASK, /* GXor src or dst */
    (IGM_S_MASK & ~IGM_F_MASK) & ~IGM_D_MASK, /* GXnor  not src and not dst */ 
    (IGM_S_MASK & ~IGM_F_MASK) ^ IGM_D_MASK, /* GXequiv  not src XOR dst */ 
    (unsigned)~IGM_D_MASK, 		    /* GXinvert */
    (IGM_S_MASK & IGM_F_MASK) | (unsigned)~IGM_D_MASK, /* GXorReverse */
    (IGM_S_MASK & ~IGM_F_MASK),              /* copy inverted */
    (IGM_S_MASK & ~IGM_F_MASK) | IGM_D_MASK, /* GXorInverted NOT src or dst */
    (IGM_S_MASK & ~IGM_F_MASK) | (unsigned)~IGM_D_MASK, /* GXnand */
    (unsigned)~0				      /* GXset */
} ;

/*
** opaque stipple
** pw, ph, pox, poy are the pattern width, height, origin x and origin y
** pwidth is the byte padded width of the pattern psrc
**
*/

#if NeedFunctionPrototypes
void
p9000ImageOpStipple(int x, int y, int w, int h, char *psrc, int pwidth, int pw, 
		  int ph, int pox, int poy,
                  int fgPixel, int bgPixel, int alu, int planemask) 
#else
void
p9000ImageOpStipple(x, y, w, h, psrc, pwdith, pw, ph, pox, poy, fgPixel,
		  bgPixel, alu, planemask)
	int		x ;
	int		y ;
	int		w ;
	int		h ;
	char 		*psrc ;
	int		pwidth ;
	int		pw ;
	int		ph ;
	int		pox ;
	int		poy ;
	int		fgPixel ;
	int		bgPixel ;
	int		alu ;          /* X11 alu !! */
	int		planemask ;
#endif
{
    register int numpix ;
    register int px ; /* where in the pattern you are */
    int py ;
    int xpos ;
    int ypos ;
    register int bw ;  /* width and height for current blit */
    int bh ;
    int tmp ;
    int old_w_max ;
    register char *pix ;
    register int offset = 0 ;

    p9000NotBusy() ;

    old_w_max = p9000Fetch(W_MAX, CtlBase) ;

    /* 12 bits each for x and y max value in W_MAX reg */
    p9000Store(W_MAX, CtlBase, (8191<<16) | 8191 ) ;

    p9000Store(PMASK, CtlBase, planemask) ;
    p9000Store(FGROUND, CtlBase, fgPixel) ;
    p9000Store(BGROUND, CtlBase, bgPixel) ;


    /* this implementation copies offscreen and then blits */
    if ( ((STIPPLEBYTE + ph * pw * p9000BytesPerPixel) <=
         (p9000InfoRec.videoRam * 1024)) &&
	 ((w > pw) || (h > ph)) 
       ) {

	/* write the whole pattern off screen */
	p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_F_MASK) |
				    (~IGM_S_MASK & IGM_B_MASK)) ;
	p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, OFFSCREENX) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, OFFSCREENX) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, STIPPLEY) ;
	p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, OFFSCREENX + pw) ;
	p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 1) ;

	pix = psrc ;
	/* one line at a time, write out the stipple */
	for (ypos = 0 ; ypos < ph ; ypos++) {
	    numpix = pw ;
	    offset = 0 ;
	    /* print all pixels on this line */
	    while (numpix >= 32) {
	        p9000Store(CMD_PIXEL1_32 | HBBSWAP, CtlBase, 
	                   *( (int *)pix + offset)) ;
	        numpix -= 32 ; /* decrement by 32 pixels  */
	        offset++;      /* move forward by 4 bytes */
	    }
	    if (numpix>0) {
	        p9000Store(CMD_PIXEL1(numpix) | HBBSWAP, CtlBase,
	                   *( (int *)pix + offset)) ;
	    }
	    pix += pwidth ; /* wrap to next line */
	}

	/* blit it on screen */
	p9000NotBusy() ;

        p9000Store(RASTER, CtlBase, p9000alu[alu]) ;

	ypos = y ;
	while ((ypos < (y + h))) {
	    xpos = x ;
	    modulus(ypos - poy, ph, py) ; /* offset into pattern */

	    bh = min(h, (ph - py)) ;	  /* height of current write */
	    if ( (ypos + bh) > (y + h) ) {
	        bh = y + h - ypos ;
	    }

	    /* set all y coords */
	    p9000Store(DEVICE_COORD | DC_0 | DC_Y, CtlBase, STIPPLEY + py) ;
	    p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, 
	               STIPPLEY + py + bh - 1) ;
	    p9000Store(DEVICE_COORD | DC_2 | DC_Y, CtlBase, ypos) ;
	    p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, ypos + bh - 1) ;

	    while (xpos < (x + w)) {
	        modulus(xpos - pox, pw, px) ; /* offset into pattern */
	        bw = min(w, (pw - px)) ;       /* width of current write */
	        if ( (xpos + bw) > (x + w) ) {
	            bw = x + w - xpos ;
	        }

	        /* set all x coords */
	        p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, OFFSCREENX+px);
	        p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, 
	                   OFFSCREENX + px + bw - 1) ;
	        p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, xpos) ;
	        p9000Store(DEVICE_COORD | DC_3 | DC_X, CtlBase, xpos + bw - 1) ;
    
	        while( p9000Fetch(CMD_BLIT, CtlBase) & SR_ISSUE_QBN) ;
	        xpos += bw ;
	    }
	    ypos += bh; 
	}
    }
/* this implementation uses pixel1 ops for everything */
    else {
        p9000Store(RASTER, CtlBase, p9000PixOpAlu[alu]) ; 
                                     
	p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, x) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, x) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, y) ;
	p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, x + w) ;
	p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 1) ;

	ypos = y ;
	while (ypos < (y + h)) {
	    xpos = x ;
	    modulus(ypos - poy, ph, py) ; /* offset into pattern */

	    while(xpos < (x + w)) {
	        modulus(xpos - pox, pw, px) ; /* offset into pattern */
	        pix = psrc + py * pwidth ;
	        bw = min(w, (pw - px)) ;

	        if ( (xpos + bw) > (x + w) ) {
	            bw = x + w - xpos ;
	        }

		pix += px/8 ;
		if ((px % 8)) {
		    tmp = min(8 - (px % 8), bw) ;
		    p9000Store(CMD_PIXEL1(tmp) | HBBSWAP, 
		               CtlBase, (*pix) >> (px % 8)) ;
		    xpos += tmp;
		    bw   -= tmp;
		    pix++ ;
		}

		while(bw >= 32) {
		    p9000Store(CMD_PIXEL1_32 | HBBSWAP, CtlBase,
		               *( (int *)pix)) ;
		    bw -= 32 ;
		    xpos += 32; 
		    pix+= 4;
		}
		if (bw > 0) {
		    p9000Store(CMD_PIXEL1(bw) | HBBSWAP, CtlBase,
		    	       *( (int *)pix)) ;
		    xpos += bw ;
		}
	    }
	    ypos++ ;
	}
    }
    p9000NotBusy() ;
    p9000QBNotBusy() ;
    p9000Store(W_MAX, CtlBase, old_w_max) ;
}

/*
** stipple
** pw, ph, pox, poy are the pattern width, height, origin x and origin y
** pwidth is the byte padded width of the pattern psrc
**
*/

#if NeedFunctionPrototypes
void
p9000ImageStipple(int x, int y, int w, int h, char *psrc, int pwidth, int pw, 
		  int ph, int pox, int poy,
                  int fgPixel, int alu, int planemask) 
#else
void
p9000ImageStipple(x, y, w, h, psrc, pwdith, pw, ph, pox, poy, fgPixel,
		  alu, planemask)
	int		x ;
	int		y ;
	int		w ;
	int		h ;
	char 		*psrc ;
	int		pwidth ;
	int		pw ;
	int		ph ;
	int		pox ;
	int		poy ;
	int		fgPixel ;
	int		alu ;
	int		planemask ;
#endif
{
    register int numpix ;
    register int px ; /* where in the pattern you are */
    int py ;
    int xpos ;
    int ypos ;
    register int bw ;  /* width and height for current blit */
    int bh ;
    int tmp ;
    int old_w_max ;
    register char *pix ;
    register int offset = 0 ;

    p9000NotBusy() ;

    old_w_max = p9000Fetch(W_MAX, CtlBase) ;

    /* 12 bits each for x and y max value in W_MAX reg */
    p9000Store(W_MAX, CtlBase, (8191<<16) | 8191 ) ;

    p9000Store(PMASK, CtlBase, planemask) ;
    p9000Store(FGROUND, CtlBase, fgPixel) ;
    p9000Store(BGROUND, CtlBase, 0x00000000) ;

    /* this implementation copies offscreen and then blits */
    if ( ((STIPPLEBYTE + ph * pw * p9000BytesPerPixel) <=
         (p9000InfoRec.videoRam * 1024)) &&
	 ((w > pw) || (h > ph))
       ) {

	/* write the whole pattern off screen */
	p9000Store(RASTER, CtlBase, (IGM_S_MASK & IGM_F_MASK) |
				    (~IGM_S_MASK & IGM_B_MASK)) ;
	p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, OFFSCREENX) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, OFFSCREENX) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, STIPPLEY) ;
	p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, OFFSCREENX + pw) ;
	p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 1) ;

	pix = psrc ;
	/* one line at a time, write out the stipple */
	for (ypos = 0 ; ypos < ph ; ypos++) {
	    numpix = pw ;
	    offset = 0 ;
	    /* print all pixels on this line */
	    while (numpix >= 32) {
	        p9000Store(CMD_PIXEL1_32 | HBBSWAP, CtlBase, 
	                   *( (int *)pix + offset)) ;
	        numpix -= 32 ; /* decrement by 32 pixels  */
	        offset++;      /* move forward by 4 bytes */
	    }
	    if (numpix>0) {
	        p9000Store(CMD_PIXEL1(numpix) | HBBSWAP, CtlBase,
	                   *( (int *)pix + offset)) ;
	    }
	    pix += pwidth ; /* wrap to next line */
	}

	/* blit it on screen */
	p9000NotBusy() ;

        alu = p9000alu[alu] ;
        p9000Store(RASTER, CtlBase, alu | (~IGM_S_MASK & IGM_D_MASK)) ;

	ypos = y ;
	while ((ypos < (y + h))) {
	    xpos = x ;
	    modulus(ypos - poy, ph, py) ; /* offset into pattern */

	    bh = min(h, (ph - py)) ;	  /* height of current write */
	    if ( (ypos + bh) > (y + h) ) {
	        bh = y + h - ypos ;
	    }

	    /* set all y coords */
	    p9000Store(DEVICE_COORD | DC_0 | DC_Y, CtlBase, STIPPLEY + py) ;
	    p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, 
	               STIPPLEY + py + bh - 1) ;
	    p9000Store(DEVICE_COORD | DC_2 | DC_Y, CtlBase, ypos) ;
	    p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, ypos + bh - 1) ;

	    while (xpos < (x + w)) {
	        modulus(xpos - pox, pw, px) ; /* offset into pattern */
	        bw = min(w, (pw - px)) ;       /* width of current write */
	        if ( (xpos + bw) > (x + w) ) {
	            bw = x + w - xpos ;
	        }

	        /* set all x coords */
	        p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, OFFSCREENX+px);
	        p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, 
	                   OFFSCREENX + px + bw - 1) ;
	        p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, xpos) ;
	        p9000Store(DEVICE_COORD | DC_3 | DC_X, CtlBase, xpos + bw - 1) ;
    
	        while( p9000Fetch(CMD_BLIT, CtlBase) & SR_ISSUE_QBN) ;
	        xpos += bw ;
	    }
	    ypos += bh; 
	}
    }
/* this implementation uses pixel1 ops for everything */
    else {
        p9000Store(RASTER, CtlBase, p9000PixAlu[alu]) ;
	p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, x) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, x) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, y) ;
	p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, x + w) ;
	p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 1) ;

	ypos = y ;
	while (ypos < (y + h)) {
	    xpos = x ;
	    modulus(ypos - poy, ph, py) ; /* offset into pattern */

	    while(xpos < (x + w)) {
	        modulus(xpos - pox, pw, px) ; /* offset into pattern */
	        pix = psrc + py * pwidth ;
	        bw = min(w, (pw - px)) ;

	        if ( (xpos + bw) > (x + w) ) {
	            bw = x + w - xpos ;
	        }

		pix += px/8 ;
		if ((px % 8)) {
		    tmp = min(8 - (px % 8), bw) ;
		    p9000Store(CMD_PIXEL1(tmp) | HBBSWAP, 
		               CtlBase, (*pix++) >> (px % 8)) ;
		    xpos += tmp;
		    bw   -= tmp;
		}

		while(bw >= 32) {
		    p9000Store(CMD_PIXEL1_32 | HBBSWAP, CtlBase,
		               *( (int *)pix)) ;
		    bw -= 32 ;
		    xpos += 32; 
		    pix+= 4;
		}
		if (bw > 0) {
		    p9000Store(CMD_PIXEL1(bw) | HBBSWAP, CtlBase,
		    	       *( (int *)pix)) ;
		    xpos += bw ;
		}
	    }
	    ypos++ ;
	}
    }
    p9000NotBusy() ;
    p9000QBNotBusy() ;
    p9000Store(W_MAX, CtlBase, old_w_max) ;
}

#if NeedFunctionPrototypes
void
p9000ImageFill(int x, int y, int w, int h, char *psrc, int pwidth, int pw, 
		  int ph, int pox, int poy, int alu, int planemask) 
#else
void
p9000ImageFill(x, y, w, h, psrc, pwdith, pw, ph, pox, poy, 
		  alu, planemask)
	int		x ;
	int		y ;
	int		w ;
	int		h ;
	char 		*psrc ;
	int		pwidth ;
	int		pw ;
	int		ph ;
	int		pox ;
	int		poy ;
	int		alu ;
	int		planemask ;
#endif
{
    int numpix ;
    register int px ; /* where in the pattern you are */
    int py ;
    register int xpos ;
    int ypos ;
    register int bw ;  /* width and height for current blit */
    int bh ;
    int tmp ;
    int old_w_max ;
    int old_w_min ;
    register char *pix ;
    register char *cpsrc ; /* copy of psrc */
    register int offset = 0 ;
    int align ;
    int count = 0 ;

    p9000NotBusy() ;

    old_w_max = p9000Fetch(W_MAX, CtlBase) ;
    old_w_min = p9000Fetch(W_MIN, CtlBase) ;


    p9000Store(PMASK, CtlBase, planemask) ;

    #if 0
    /* this implementation copies offscreen and then blits */
    if ( ((STIPPLEBYTE + ph * pw * p9000BytesPerPixel) <=
         (p9000InfoRec.videoRam * 1024)) &&
	 ((w > pw) || (h > ph)) &&
	 ((w > 4) && (h > 4))
       ) {

	/* write the whole pattern off screen */
        p9000Store(W_MAX, CtlBase, (8191<<16) | 8191 ) ;
	p9000Store(RASTER, CtlBase, (IGM_S_MASK) ) ;
	p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, OFFSCREENX) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, OFFSCREENX) ;
	p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, STIPPLEY) ;
	p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, OFFSCREENX+pw) ;
	p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 1) ;

	/* one line at a time, write out the tile */
	cpsrc = psrc ;
	for (ypos = 0 ; ypos < ph ; ypos++) {
	    pix = cpsrc ;
	    for (xpos = 0 ; xpos < pw ; xpos += 4) {
	        p9000Store(CMD_PIXEL8 | HBBSWAP, CtlBase, *( (int *)pix)) ;
	        pix += 4 ;
	    }
	    cpsrc += pwidth ;
	}

	/* blit it on screen */
	p9000NotBusy() ;

        p9000Store(RASTER, CtlBase, p9000alu[alu])  ;

	ypos = y ;
	while ((ypos < (y + h))) {
	    xpos = x ;
	    modulus(ypos - poy, ph, py) ; /* offset into pattern */

	    bh = min(h, (ph - py)) ;	  /* height of current write */
	    if ( (ypos + bh) > (y + h) ) {
	        bh = y + h - ypos ;
	    }

	    /* set all y coords */
	    p9000Store(DEVICE_COORD | DC_0 | DC_Y, CtlBase, STIPPLEY + py) ;
	    p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, 
	               STIPPLEY + py + bh - 1) ;
	    p9000Store(DEVICE_COORD | DC_2 | DC_Y, CtlBase, ypos) ;
	    p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, ypos + bh - 1) ;

	    while (xpos < (x + w)) {
	        modulus(xpos - pox, pw, px) ; /* offset into pattern */
	        bw = min(w, (pw - px)) ;       /* width of current write */
	        if ( (xpos + bw) > (x + w) ) {
	            bw = x + w - xpos ;
	        }

	        /* set all x coords */
	        p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, OFFSCREENX+px);
	        p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, 
	                   OFFSCREENX + px + bw - 1) ;
	        p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, xpos) ;
	        p9000Store(DEVICE_COORD | DC_3 | DC_X, CtlBase, xpos + bw) ;
	        xpos += bw ;
   		while( p9000Fetch(CMD_BLIT, CtlBase) & SR_ISSUE_QBN) ; 
	    }
	    ypos += bh; 
	}
    }
#endif
   if (0) ;
/* this implementation uses pixel8 ops for everything */
    else {

        p9000Store(RASTER, CtlBase, p9000alu[alu]) ;
	p9000Store(DEVICE_COORD | DC_3 | DC_Y, CtlBase, 0) ;

	ypos = y ;
	cpsrc = psrc ;
	while (ypos < (y + h)) {
	    p9000Store(DEVICE_COORD | DC_1 | DC_Y, CtlBase, ypos) ;

	    xpos = x ;
	    modulus(ypos - poy, ph, py) ; /* offset into pattern */

	    while(xpos < (x + w)) {
	        modulus(xpos - pox, pw, px) ; /* offset into pattern */
	        pix = cpsrc + py * pwidth ;
	        bw = min(w, (pw - px)) ;

	        if ( (xpos + bw) > (x + w) ) {
	            bw = x + w - xpos ;
	        }

	        p9000Store(DEVICE_COORD | DC_0 | DC_X, CtlBase, xpos) ;
	        p9000Store(DEVICE_COORD | DC_1 | DC_X, CtlBase, xpos) ;

		pix += px ;
		xpos += bw; 

	        p9000Store(DEVICE_COORD | DC_2 | DC_X, CtlBase, xpos) ; 

		while(bw > 0) {
		    p9000Store(CMD_PIXEL8 | HBBSWAP, CtlBase,
		               *( (int *)pix)) ;
		    bw   -= 4 ;
		    pix  += 4;
		}
	    }
	    ypos++ ;
	}
    }
    p9000NotBusy() ;
    p9000QBNotBusy() ;
    p9000Store(W_MAX, CtlBase, old_w_max) ;
    p9000Store(W_MIN, CtlBase, old_w_min) ;
}


#endif /* P9000_ACCEL */

