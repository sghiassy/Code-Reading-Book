/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach64/mach64im.c,v 3.5 1996/12/23 06:39:18 dawes Exp $ */
/*
 * Copyright 1992,1993,1994,1995,1996 by Kevin E. Martin, Chapel Hill, North Carolina.
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
 * KEVIN E. MARTIN, RICKARD E. FAITH, AND TIAGO GONS DISCLAIM ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Modified for the Mach64 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: mach64im.c /main/4 1996/05/13 16:36:12 kaleb $ */

#include "X.h"
#include "misc.h"
#include "xf86.h"
#include "xf86_HWlib.h"
#include "mach64.h"
#include "mach64im.h"

void	(*mach64ImageReadFunc)();
void	(*mach64ImageWriteFunc)();

static void mach64ImageRead();
static void mach64ImageWrite();
static void mach64ImageWriteHW();

static unsigned long PMask;
static int BytesPerPixel;
static int screenStride;

void
mach64ImageInit()
{
    int i;

    PMask = (1UL << mach64InfoRec.depth) - 1;
    BytesPerPixel = mach64InfoRec.bitsPerPixel / 8;
    screenStride = mach64VirtX * BytesPerPixel;

    mach64ImageReadFunc = mach64ImageRead;
    mach64ImageWriteFunc = mach64ImageWrite;
}

static void
mach64ImageWrite(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			px;
    int			py;
    int			alu;
    unsigned long	planemask;
{
    pointer curvm;
    int byteCount;

    if ((w <= 0) || (h <= 0))
	return;

    if (alu == MIX_DST)
	return;

    if ((alu != MIX_SRC) || ((planemask & PMask) != PMask)) {
	mach64ImageWriteHW(x, y, w, h, psrc, pwidth, px, py,
			   alu, planemask);
	return;
    }
	
    WaitIdleEmpty();

    psrc += pwidth * py + px * BytesPerPixel;
    curvm = (pointer)((unsigned char *)mach64VideoMem +
	    (x + y * mach64VirtX) * BytesPerPixel);

    byteCount = w * BytesPerPixel;
    while(h--) {
	MemToBus((void *)curvm, psrc, byteCount);
	curvm = (pointer)((unsigned char *)curvm + screenStride); 
	psrc += pwidth;
    }
}


static void
mach64ImageWriteHW(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			px;
    int			py;
    int			alu;
    unsigned long	planemask;
{
    int wordsPerLine;
    register int count;
    register int *pword;
    int old_DP_PIX_WIDTH;

    WaitQueue(8);
    old_DP_PIX_WIDTH = regr(DP_PIX_WIDTH);
    regw(SC_LEFT_RIGHT, (((x+w-1) << 16) | (x & 0x0000ffff)));
    regw(SC_TOP_BOTTOM, (((y+h-1) << 16) | (y & 0x0000ffff)));
    regw(DP_WRITE_MASK, planemask);
    regw(DP_MIX, (alu << 16) | alu);
    regw(DP_SRC, (MONO_SRC_ONE | FRGD_SRC_HOST | BKGD_SRC_HOST));
    switch(mach64InfoRec.bitsPerPixel)
    {
	case 8:
            regw(DP_PIX_WIDTH, (SRC_8BPP | HOST_8BPP | DST_8BPP));
	    break;
	case 16:
            regw(DP_PIX_WIDTH, (SRC_16BPP | HOST_16BPP | DST_16BPP));
	    break;
	case 32:
            regw(DP_PIX_WIDTH, (SRC_32BPP | HOST_32BPP | DST_32BPP));
	    break;
    }

    w = (w + 3) / 4; /* round up to int boundry */

    regw(DST_Y_X, ((x << 16) | (y & 0x0000ffff)));
    regw(DST_HEIGHT_WIDTH, (((w * 4) << 16) | (h & 0x0000ffff)));

    psrc += pwidth * py + px * BytesPerPixel;
    wordsPerLine = w * BytesPerPixel; 

    while (h--) 
    {
	count = wordsPerLine;
	pword = (int *)psrc;

	while (count >= 16)
	{
            WaitQueue(16);
            regw(HOST_DATAF, *pword++);
            regw(HOST_DATAE, *pword++);
            regw(HOST_DATAD, *pword++);
            regw(HOST_DATAC, *pword++);
            regw(HOST_DATAB, *pword++);
            regw(HOST_DATAA, *pword++);
            regw(HOST_DATA9, *pword++);
            regw(HOST_DATA8, *pword++);
            regw(HOST_DATA7, *pword++);
            regw(HOST_DATA6, *pword++);
            regw(HOST_DATA5, *pword++);
            regw(HOST_DATA4, *pword++);
            regw(HOST_DATA3, *pword++);
            regw(HOST_DATA2, *pword++);
            regw(HOST_DATA1, *pword++);
            regw(HOST_DATA0, *pword++);
	    count -= 16;
	}

	WaitQueue(16);
	switch(count)
	{
	    case 15: regw(HOST_DATAE, *pword++);
	    case 14: regw(HOST_DATAD, *pword++);
	    case 13: regw(HOST_DATAC, *pword++);
	    case 12: regw(HOST_DATAB, *pword++);
	    case 11: regw(HOST_DATAA, *pword++);
	    case 10: regw(HOST_DATA9, *pword++);
	    case  9: regw(HOST_DATA8, *pword++);
	    case  8: regw(HOST_DATA7, *pword++);
	    case  7: regw(HOST_DATA6, *pword++);
	    case  6: regw(HOST_DATA5, *pword++);
	    case  5: regw(HOST_DATA4, *pword++);
	    case  4: regw(HOST_DATA3, *pword++);
	    case  3: regw(HOST_DATA2, *pword++);
	    case  2: regw(HOST_DATA1, *pword++);
	    case  1: regw(HOST_DATA0, *pword);
	    default:
		break;
	}
	psrc += pwidth;
    }

    WaitQueue(3);
    regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
    regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));
    regw(DP_PIX_WIDTH, old_DP_PIX_WIDTH);

    WaitIdleEmpty();
}


static void
mach64ImageRead(x, y, w, h, psrc, pwidth, px, py, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;
    int			px;
    int			py;
    unsigned long	planemask;
{
    int j;
    pointer curvm;

    if ((w <= 0) || (h <= 0))
	return;

    if ((planemask & PMask) != PMask) {
#ifdef NOT_DEFINED
        mach64ImageReadHW(x, y, w, h, psrc, pwidth, px, py, planemask);
	return;
#else
	ErrorF ("mach64ImageRead: unsupported planemask\n");
#endif
    }

    WaitIdleEmpty();

    psrc += pwidth * py + px * BytesPerPixel;
    curvm = (pointer)((unsigned char *)mach64VideoMem + x * BytesPerPixel);
    
    for (j = y; j < y+h; j++) {
	BusToMem(psrc, (void *)((unsigned char *)curvm + j * screenStride),
		 w * BytesPerPixel);
	psrc += pwidth;
    }
}

void
mach64ImageStippleFunc(x, y, w, h, psrc, pwidth, px, py, fgPixel, bgPixel, 
		       alu, planemask, opaque)
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    char                *psrc;
    int                 pwidth;
    int                 px;
    int                 py;
    Pixel               fgPixel;
    Pixel               bgPixel;
    int                 alu;
    unsigned long       planemask;
    int 		opaque;
{
    register int *pword;
    register int *pline;
    register int count;
    int wordsPerLine;
    int old_DP_PIX_WIDTH;
    int old_DST_OFF_PITCH;


    if (alu == MIX_DST || w <= 0 || h <= 0)
	return;

    WaitQueue(12);
    old_DP_PIX_WIDTH = regr(DP_PIX_WIDTH);
    old_DST_OFF_PITCH = regr(DST_OFF_PITCH);

    regw(SC_LEFT_RIGHT, (((x+w-1) << 16) | (x & 0x0000ffff)));
    regw(SC_TOP_BOTTOM, (((y+h-1) << 16) | (y & 0x0000ffff)));

    regw(DP_WRITE_MASK, planemask);
    regw(DP_FRGD_CLR, fgPixel);
    regw(DP_BKGD_CLR, bgPixel);

    if( opaque )
        regw(DP_MIX, (alu << 16) | alu);
    else
        regw(DP_MIX, (alu << 16) | MIX_DST);

    switch(mach64InfoRec.bitsPerPixel)
    {
        case 8:
    	    regw(DP_PIX_WIDTH, (BYTE_ORDER_LSB_TO_MSB | SRC_1BPP | HOST_1BPP |
				DST_8BPP));
            break;
        case 16:
    	    regw(DP_PIX_WIDTH, (BYTE_ORDER_LSB_TO_MSB | SRC_1BPP | HOST_1BPP | 
				DST_16BPP));
            break;
        case 32:
    	    regw(DP_PIX_WIDTH, (BYTE_ORDER_LSB_TO_MSB | SRC_1BPP | HOST_1BPP | 
				DST_32BPP));
            break;
    }
    regw(DP_SRC, (MONO_SRC_HOST | FRGD_SRC_FRGD_CLR | BKGD_SRC_BKGD_CLR));

    w = (w + (px % 32) + 31) >> 5; /* round up to int boundry and take into */
				   /* account the pixels skipped over */

    if (px)
        regw(DST_Y_X, (((x - (px % 32)) << 16) | (y & 0x0000ffff)));
    else
	regw(DST_Y_X, ((x << 16) | (y & 0x0000ffff)));
    regw(DST_HEIGHT_WIDTH, (((w * 32) << 16) | h));

    wordsPerLine = pwidth >> 2;
    pline = (int*)psrc + (wordsPerLine * py) + (px >> 5);

    while (h--) 
    {
        count = w;
	pword = pline;

        while (count >= 16)
        {
            WaitQueue(16);
            regw(HOST_DATAF, *pword++);
            regw(HOST_DATAE, *pword++);
            regw(HOST_DATAD, *pword++);
            regw(HOST_DATAC, *pword++);
            regw(HOST_DATAB, *pword++);
            regw(HOST_DATAA, *pword++);
            regw(HOST_DATA9, *pword++);
            regw(HOST_DATA8, *pword++);
            regw(HOST_DATA7, *pword++);
            regw(HOST_DATA6, *pword++);
            regw(HOST_DATA5, *pword++);
            regw(HOST_DATA4, *pword++);
            regw(HOST_DATA3, *pword++);
            regw(HOST_DATA2, *pword++);
            regw(HOST_DATA1, *pword++);
            regw(HOST_DATA0, *pword++);
	    count -= 16;
        }

        WaitQueue(16);
        switch(count)
        {
            case 15: regw(HOST_DATAE, *pword++);
            case 14: regw(HOST_DATAD, *pword++);
            case 13: regw(HOST_DATAC, *pword++);
            case 12: regw(HOST_DATAB, *pword++);
            case 11: regw(HOST_DATAA, *pword++);
            case 10: regw(HOST_DATA9, *pword++);
            case  9: regw(HOST_DATA8, *pword++);
            case  8: regw(HOST_DATA7, *pword++);
            case  7: regw(HOST_DATA6, *pword++);
            case  6: regw(HOST_DATA5, *pword++);
            case  5: regw(HOST_DATA4, *pword++);
            case  4: regw(HOST_DATA3, *pword++);
            case  3: regw(HOST_DATA2, *pword++);
            case  2: regw(HOST_DATA1, *pword++);
            case  1: regw(HOST_DATA0, *pword);
            default:
                break;
        }
	pline += wordsPerLine;
    }

    WaitQueue(4);

    regw(DST_OFF_PITCH, old_DST_OFF_PITCH);
    regw(DP_PIX_WIDTH, old_DP_PIX_WIDTH);
    regw(SC_LEFT_RIGHT, ((mach64MaxX << 16) | 0 ));
    regw(SC_TOP_BOTTOM, ((mach64MaxY << 16) | 0 ));

    WaitIdleEmpty();
}
