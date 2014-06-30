/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/mach32im.c,v 3.11 1996/12/23 06:38:39 dawes Exp $ */
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
 * KEVIN E. MARTIN, RICKARD E. FAITH, TIAGO GONS, AND CRAIG E. GROESCHEL
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL ANY OR ALL OF
 * THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Rik Faith (faith@cs.unc.edu), Mon Jul  5 20:00:01 1993:
 *   Added 16-bit and some 64kb aperture optimizations.
 *   Waffled in stipple optimization by Jon Tombs (from s3/s3im.c)
 *   Added outsw code.
 * Modified for the new cache by Mike Bernson (mike@mbsun.mlb.org)
 */
/* $XConsortium: mach32im.c /main/10 1996/10/28 05:30:52 kaleb $ */

#include "X.h"
#include "misc.h"
#include "xf86.h"
#include "xf86_HWlib.h"
#include "mach32.h"
#include "mach32im.h"

#define	mach32BankSize  (64 * 1024)
void	(*mach32ImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
void	(*mach32ImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
void	(*mach32ImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);

static	void	mach32ImageWrite(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static	void	mach32ImageRead(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static	void	mach32ImageFill(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
static	void	mach32ImageWriteBank(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static	void	mach32ImageReadBank(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static	void	mach32ImageFillBank(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
static	void	mach32ImageWriteNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static	void	mach32ImageReadNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static	void	mach32ImageFillNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);

static	short	bank_to_page_1[48];
static	short	bank_to_page_2[48];

static int _internal_mach32_mskbits[17] = {
   0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0x1ff, 0x3ff, 0x7ff,
   0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

static unsigned long PMask;
static int BytesPerPixel;
static int screenStride;

#define MSKBIT(n) (_internal_mach32_mskbits[(n)])
#define SWPBIT(s) (swapbits[pline[(s)]])

#ifdef __GNUC__

#ifdef NO_INLINE
#define __inline__ /**/
#endif

static __inline__ void outsw(void *buf, int count, unsigned short port)
{
   __asm__ __volatile__ ("cld;rep;outsw"
		     ::"d" (port),"S" (buf),"c" (count):"cx","si");
}

__inline__ void mach32SetVGAPage(int page)
{
	outw(ATIEXT, bank_to_page_1[page]);
	outw(ATIEXT, bank_to_page_2[page]);
}

static __inline__ void XYSetVGAPage(int x, int y)
{
    mach32SetVGAPage(((x + y*(mach32MaxX+1))*(mach32InfoRec.bitsPerPixel/8))
								>> 16);
}

#else /* __GNUC__ */

static void outsw(buf, count, port)
void *buf;
int count;
register unsigned short port;
{
	register int i;
	register unsigned short *p = (unsigned short *)buf;

	for (i=0; i < count; i++)
		outw(port, *(p++));
}

void mach32SetVGAPage(page)
int page;
{
	outw(ATIEXT, bank_to_page_1[page]);
	outw(ATIEXT, bank_to_page_2[page]);
}

static	void XYSetVGAPage(x, y)
int x;
int y;
{
    mach32SetVGAPage(((x + y*(mach32MaxX+1))*(mach32InfoRec.bitsPerPixel/8))
								>> 16);
}


#endif /* __GNUC__ */

#define	reorder(a,b)	b = \
	(a & 0x80) >> 7 | \
	(a & 0x40) >> 5 | \
	(a & 0x20) >> 3 | \
	(a & 0x10) >> 1 | \
	(a & 0x08) << 1 | \
	(a & 0x04) << 3 | \
	(a & 0x02) << 5 | \
	(a & 0x01) << 7;

unsigned char swapbits[256];
unsigned short mach32stipple_tab[256];
void
mach32ImageInit()
{
    int i;

    PMask = (1UL << mach32InfoRec.depth) - 1;
    BytesPerPixel = mach32InfoRec.bitsPerPixel / 8;
    screenStride = mach32DisplayWidth * BytesPerPixel;

    for (i = 0; i < 256; i++) {
	reorder(i,swapbits[i]);
	mach32stipple_tab[i] =((swapbits[i] & 0x0f) << 1) | ((swapbits[i] & 0xf0) << 5);
    }


    for(i = 0 ; i < 48; i++) {
	bank_to_page_1[i] = ((i & 0x30) << 4) | ((i & 0x30)<<6) | ATI2E;
	bank_to_page_2[i] = ((i & 0x0f)<<9) | ATI32;
    }

    if (mach32Use4MbAperture) {
	/* This code does not need to deal with bank select at all */
	mach32ImageReadFunc = mach32ImageRead;
	mach32ImageWriteFunc = mach32ImageWrite;
	mach32ImageFillFunc = mach32ImageFill;
    } else {
	/* This code does need to deal with bank select */
	mach32ImageReadFunc = mach32ImageReadBank;
	mach32ImageWriteFunc = mach32ImageWriteBank;
	mach32ImageFillFunc = mach32ImageFillBank;
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageWrite(
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
mach32ImageWrite(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
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
    unsigned char *curvm;
    int byteCount;

    if ((w == 0) || (h == 0))
	return;

    if (alu == MIX_DST)
	return;

    if ((alu != MIX_SRC) || ((planemask&PMask) != PMask)) {
	mach32ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py,
			      alu, planemask);
	return;
    }
	
    WaitIdleEmpty();

    psrc += pwidth * py + px * BytesPerPixel;
    curvm = mach32VideoMem + (x + y * mach32DisplayWidth) * BytesPerPixel;

    byteCount = w * BytesPerPixel;
    while(h--) {
	MemToBus((void *)curvm, psrc, byteCount);
	curvm += screenStride; 
	psrc += pwidth;
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageWriteBank(
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
mach32ImageWriteBank(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
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
    unsigned char *curvm;
    int offset;
    int bank;
    int left;
    int count;

    if ((w == 0) || (h == 0))
	return;

    if (alu == MIX_DST)
	  return;

    if ((alu != MIX_SRC) || ((planemask&PMask) != PMask)) {
	mach32ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py,
			      alu, planemask);
	return;
    }
	
    WaitIdleEmpty();

    psrc += pwidth * py + px * BytesPerPixel;
    offset = (x + y * mach32DisplayWidth) * BytesPerPixel;
    bank = offset / mach32BankSize;
    offset &= (mach32BankSize-1);
    curvm = &mach32VideoMem[offset];
    mach32SetVGAPage(bank);


    while(h) {
	/*
	 * calc number of line before need to switch banks
	 */
	count = (mach32BankSize - offset) / mach32DisplayWidth / BytesPerPixel;
	if (count >= h) {
		count = h;
		h = 0;
	} else {
		offset += (count * screenStride);
		if (offset + w  * BytesPerPixel < mach32BankSize) {
			count++;
			offset += screenStride;
		}
		h -= count;
	}

	/*
	 * Output line till bank switch
	 */
	while(count--) {
		MemToBus((void *)curvm, psrc, w * BytesPerPixel);
		curvm += screenStride;
		psrc += pwidth;
	}

	if (h) {
	   if (offset < mach32BankSize) {
		h--;
	        left = mach32BankSize - offset;
		MemToBus((void *)curvm, psrc, left);
		bank++;
		mach32SetVGAPage(bank);
		
		MemToBus(mach32VideoMem, psrc+left, w * BytesPerPixel - left);
		psrc += pwidth;
		offset += screenStride;
	    } else {
		bank++;
		mach32SetVGAPage(bank);
	    }
	offset &= (mach32BankSize-1);
	curvm = &mach32VideoMem[offset];
	}
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageWriteNoMem(
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
mach32ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
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
    int count = ((w + 1) >> 1) * BytesPerPixel;
    int j;
       
    WaitIdleEmpty();
    outw(FRGD_MIX, FSS_PCDATA | alu);
    outw(WRT_MASK, planemask);
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    if (w&1)
	outw(MAJ_AXIS_PCNT, (short)w);
    else
	outw(MAJ_AXIS_PCNT, (short)w-1);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h-1));
    outw(EXT_SCISSOR_L, x);
    outw(EXT_SCISSOR_R, x+w-1);

    WaitQueue(1);
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW 
       | PCDATA | WRTDATA | _16BIT | BYTSEQ);

    WaitQueue(16);

    w += px;
    psrc += pwidth * py;

    /* ASSUMPTION: it is ok to read one byte past
       the psrc structure (for odd width). */

    for (j = 0; j < h; j++) {
	outsw( psrc + px * BytesPerPixel, count, PIX_TRANS );
	psrc += pwidth;
    }
	
    WaitQueue(3);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(EXT_SCISSOR_L, 0);
    outw(EXT_SCISSOR_R, mach32MaxX);
}


static void
#if NeedFunctionPrototypes
mach32ImageRead(
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
mach32ImageRead(x, y, w, h, psrc, pwidth, px, py, planemask)
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
    int j;
    unsigned char *curvm;

    if ((w == 0) || (h == 0))
	return;

    if ((planemask & PMask) != PMask) {
	mach32ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask);
	return;
    }

    WaitIdleEmpty();

    psrc += pwidth * py + px * BytesPerPixel;
    curvm = mach32VideoMem + x * BytesPerPixel;
    
    for (j = y; j < y+h; j++) {
	BusToMem(psrc, (void *)(curvm + j * screenStride), w * BytesPerPixel);
	psrc += pwidth;
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageReadBank(
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
mach32ImageReadBank(x, y, w, h, psrc, pwidth, px, py, planemask)
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
    unsigned char *curvm;
    int offset;
    int bank;
    int left;

    if ((w == 0) || (h == 0))
	return;

    if ((planemask & PMask) != PMask) {
	mach32ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask);
	return;
    }

    WaitIdleEmpty();

    psrc += pwidth * py + px * BytesPerPixel;
    offset = (x + y * mach32DisplayWidth) * BytesPerPixel;
    bank = offset / mach32BankSize;
    offset &= (mach32BankSize-1);
    curvm = &mach32VideoMem[offset];
    mach32SetVGAPage(bank);

    while(h--) {
	if (offset + w * BytesPerPixel > mach32BankSize) {
	    if (offset < mach32BankSize) {
		left = mach32BankSize - offset;
		BusToMem(psrc, (void *)curvm, left);
		bank++;
		mach32SetVGAPage(bank);
		
		BusToMem(psrc+left, mach32VideoMem, w * BytesPerPixel - left);
		psrc += pwidth;
		offset = (offset + screenStride) & (mach32BankSize-1);
		curvm = &mach32VideoMem[offset];
	    } else {
		bank++;
		mach32SetVGAPage(bank);
		offset &= (mach32BankSize-1);
		curvm = &mach32VideoMem[offset];
		BusToMem(psrc, (void *)curvm, w * BytesPerPixel);
		offset += screenStride;
		curvm += screenStride;
		psrc += pwidth;
	    }
	} else {
	    BusToMem(psrc, (void *)curvm, w * BytesPerPixel);
	    offset += screenStride;
	    curvm += screenStride;
	    psrc += pwidth;
	}
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageReadNoMem(
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
mach32ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask)
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
    int	j;
    int	i;
    short   pix;
    
    WaitIdleEmpty();
    outw(MULTIFUNC_CNTL, PIX_CNTL | 0);
    outw(FRGD_MIX, FSS_PCDATA | MIX_SRC);
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    outw(MAJ_AXIS_PCNT, (short)(w - 1));
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h - 1));
    
    if (BytesPerPixel == 1)
	outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW | PCDATA);
    else
	outw(CMD, CMD_RECT | BYTSEQ | _16BIT | INC_Y | INC_X | DRAW | PCDATA);
    
    WaitQueue(16);
    
    w += px;
    psrc += pwidth * py;
    
    if (BytesPerPixel == 1) {
	for (j = 0; j < h; j++) {
	    for (i = px; i < w; i++) {
		pix = inw(PIX_TRANS);
		psrc[i] = (unsigned char)(pix & planemask);
	    }
	    psrc += pwidth;
	}
    } else { /* 2 */
	short *spsrc = (short *) psrc;
	for (j = 0; j < h; j++) {
	    for (i = px; i < w; i++) {
		pix = inw(PIX_TRANS);
		spsrc[i] = pix & planemask;
	    }
	    spsrc += pwidth / 2;
	}
    }
    
    WaitQueue(1);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
}

static void
#if NeedFunctionPrototypes
mach32ImageFill(
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
mach32ImageFill(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
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
    int i,j;
    char		*pline;
    int                 mod, ymod;
    unsigned char       *curvm;
    int                 count;

    if ((w == 0) || (h == 0))
	return;

    if (alu == MIX_DST)
	  return;

    if ((alu != MIX_SRC) || ((planemask&PMask) != PMask))
    {
	mach32ImageFillNoMem(x, y, w, h, psrc, pwidth, pw, ph, pox,
			     poy, alu, planemask);
	return;
    }

    WaitIdleEmpty();
    modulus(y-poy,ph,ymod);

    for (j = y; j < y+h; j++) {
	curvm = mach32VideoMem + (x + j * mach32DisplayWidth) * BytesPerPixel;
	
	pline = psrc + pwidth*ymod;
	if (++ymod >= ph)
	    ymod -= ph;
	modulus(x-pox,pw,mod);
	for (i = 0, count = 0; i < w; i += count ) {
	    count = pw - mod;
	    if (i + count > w)
		count = w - i;
	    bcopy(pline + mod * BytesPerPixel, curvm, count * BytesPerPixel);
	    curvm += count * BytesPerPixel;
	    mod += count;
	    while(mod >= pw)
		mod -= pw;
	}
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageFillBank(
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
mach32ImageFillBank(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
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
    int i,j;
    char		*pline;
    int                 mod, ymod;
    unsigned char       *curvm;
    int                 count;

    if ((w == 0) || (h == 0))
	return;

    if (alu == MIX_DST)
	return;

    if ((alu != MIX_SRC) || ((planemask&PMask) != PMask))
    {
	mach32ImageFillNoMem(x, y, w, h, psrc, pwidth, pw, ph, pox,
			     poy, alu, planemask);
	return;
    }

    WaitIdleEmpty();
    modulus(y-poy,ph,ymod);

    for (j = y; j < y+h; j++) {
	XYSetVGAPage(x,j);
	curvm = mach32VideoMem +
		(((x + j * mach32DisplayWidth) * BytesPerPixel) & 0xffff);
	pline = psrc + pwidth*ymod;
	if (++ymod >= ph)
	    ymod -= ph;
	modulus(x-pox,pw,mod);
	if ((x >= mach32VideoPageBoundary[j])
	    || (x+w <= mach32VideoPageBoundary[j])) {
	    for (i = 0, count = 0; i < w; i += count ) {
		count = pw - mod;
		if (i + count > w)
		    count = w - i;
		bcopy(pline + mod * BytesPerPixel, curvm, count * BytesPerPixel);
		curvm += count * BytesPerPixel;
		mod += count;
		while(mod >= pw)
		    mod -= pw;
	    }
	} else {
	    for (i=0, count=0; i < mach32VideoPageBoundary[j]-x; i += count ) {
		count = pw - mod;
		if (i + count > mach32VideoPageBoundary[j]-x)
		    count = mach32VideoPageBoundary[j] - x - i;
		bcopy(pline + mod * BytesPerPixel, curvm, count * BytesPerPixel);
		curvm += count * BytesPerPixel;
		mod += count;
		while(mod >= pw)
		    mod -= pw;
	    }
	    XYSetVGAPage(x+w,j);
	    curvm = mach32VideoMem;
	    for (i = mach32VideoPageBoundary[j]-x, count = 0;
		 i < w; i += count ) {
		count = pw - mod;
		if (i + count > w)
		    count = w - i;
		bcopy(pline + mod * BytesPerPixel, curvm, count * BytesPerPixel);
		curvm += count * BytesPerPixel;
		mod += count;
		while(mod >= pw)
		    mod -= pw;
	    }
	}
    }
}

static void
#if NeedFunctionPrototypes
mach32ImageFillNoMem(
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
mach32ImageFillNoMem(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
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
    int i,j;
    char		*pline;
    int                 mod, ymod;
    int                 count;

    WaitIdleEmpty();
    outw(FRGD_MIX, FSS_PCDATA | alu);
    outw(WRT_MASK, planemask);
    outw(CUR_X, (short)x);
    outw(CUR_Y, (short)y);
    if (w&1)
	outw(MAJ_AXIS_PCNT, (short)w);
    else
	outw(MAJ_AXIS_PCNT, (short)w-1);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h-1));
    outw(EXT_SCISSOR_L, x);
    outw(EXT_SCISSOR_R, x+w-1);
    
    WaitQueue(1);
    outw(CMD, CMD_RECT | INC_Y | INC_X | DRAW
	 | PCDATA | WRTDATA | _16BIT | BYTSEQ);
    
    WaitQueue(16);
    
    modulus(y-poy,ph,ymod);
    for (j = 0; j < h; j++) {
	pline = psrc + pwidth*ymod;
	if (++ymod >= ph)
	    ymod -= ph;
	modulus(x-pox,pw,mod);
	for (i = 0, count = 0; i < w; i += count) {
	    count = pw - mod;
	    if (count == 1) {
		if (BytesPerPixel == 1) {
		    outw(PIX_TRANS, ((short)pline[mod] << 8)
		     | (short)pline[mod+1-pw]);
		    ++count;
		} else
		    outw(PIX_TRANS, (short)pline[mod*2]);
	    } else {
		if (i + count > w)
		    count = w - i;
		if (BytesPerPixel == 1) {
		    if (count == 1)
			++count;
		    outsw( pline + mod, count >> 1, PIX_TRANS );
		    count &= ~1;
		} else
		    outsw( pline + mod * 2, count, PIX_TRANS );
	    }
	    mod += count;
	    if (mod >= pw)
		mod -= pw;
	}
    }
    
    WaitQueue(3);
    outw(FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
    outw(EXT_SCISSOR_L, 0);
    outw(EXT_SCISSOR_R, mach32MaxX);
}

void
#if NeedFunctionPrototypes
mach32ImageStipple(
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
mach32ImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
		  fgPixel, bgPixel, alu, planemask, opaque)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pw, ph, pox, poy;
    int			pwidth;
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

    WaitQueue(13);
    outw(EXT_SCISSOR_L, x);
    outw(EXT_SCISSOR_R, (x+w-1));
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_EXPPC | COLCMPOP_F);
    outw(MULTIFUNC_CNTL, MIN_AXIS_PCNT | h-1);
    outw(WRT_MASK, planemask);
    outw(FRGD_MIX, FSS_FRGDCOL | alu);
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
    WaitQueue(16);
    /*
     * When the source bitmap is properly aligned, max 16 pixels wide
     * and nonrepeating, use this faster loop instead.
     * This speeds up all copying to the font cache.
     */
    if( (x & 7) == 0 && w <= 16 && x+w <= pw && y+h <= ph ) {
	unsigned short pix;
	unsigned char *pnt;

	pnt = (unsigned char *)(psrc + pwidth * y + (x >> 3));
	while( h-- > 0 ) {
	    pix = *((unsigned short *)(pnt));
	    outw( PIX_TRANS, mach32stipple_tab[ pix & 0xff ] );
	    if( w > 8 )
		outw( PIX_TRANS, mach32stipple_tab[ ( pix >> 8 ) & 0xff ] );

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
		    outw( PIX_TRANS, mach32stipple_tab[ pix & 0xff ] );
		    if( dstw > 8 )
			outw( PIX_TRANS,
			      mach32stipple_tab[ ( pix >> 8 ) & 0xff ] );
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
    outw(EXT_SCISSOR_L, 0);
    outw(EXT_SCISSOR_R, mach32MaxX);
    outw(MULTIFUNC_CNTL, PIX_CNTL | MIXSEL_FRGDMIX | COLCMPOP_F);
}

#if NeedFunctionPrototypes
void
mach32ImageOpStipple(
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
    unsigned long	planemask)
#else
void
mach32ImageOpStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, fgPixel, bgPixel, alu, planemask)
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
    Pixel		fgPixel;
    Pixel		bgPixel;
    short		alu;
    unsigned long	planemask;
#endif
{
    mach32ImageStipple(x, y, w, h , psrc, pwidth, pw, ph, pox, poy,
		       fgPixel, bgPixel, alu, planemask, 1);
}

void
#if NeedFunctionPrototypes
mach32FontOpStipple(
    int			x,
    int			y,
    int			w,
    int			h,
    unsigned char	*psrc,
    int			pwidth,
    Pixel		id)
#else
mach32FontOpStipple(x, y, w, h, psrc, pwidth, id)
    int			x;
    int			y;
    int			w;
    int			h;
    unsigned char	*psrc;
    int			pwidth;
    Pixel		id;
#endif
{
    extern unsigned short mach32cachemask[];

    mach32ImageStipple(x, y, w, h, (char *) psrc, pwidth, w, h, x, y,
		       ~0, 0, mach32alu[GXcopy], (Pixel)mach32cachemask[id], 1);
}
