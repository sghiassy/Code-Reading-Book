/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxIm.c,v 3.19 1996/12/23 06:32:49 dawes Exp $ */
/*
 * Copyright 1992,1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
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
 * KEVIN E. MARTIN, RICKARD E. FAITH, TIAGO GONS AND HENRY A. WORTH
 * DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL 
 * THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA 
 * OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER 
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR 
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Modified for the Mach-8 by Rickard E. Faith (faith@cs.unc.edu)
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 * Rik Faith (faith@cs.unc.edu), Mon Jul  5 20:00:01 1993:
 *   Added 16-bit and some 64kb aperture optimizations.
 *   Waffled in stipple optimization by Jon Tombs (from s3/s3im.c)
 *   Added outsw code.
 * Modified for the AGX by Henry A. Worth (haw30@eng.amdahl.com)
 *
 */
/* $XConsortium: agxIm.c /main/11 1996/10/19 17:50:07 kaleb $ */

#include "X.h"
#include "misc.h"
#include "xf86.h"
#include "xf86_HWlib.h"
#include "agx.h"
#include "regagx.h"
#include "agxIm.h"

void	(*agxImageReadFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
void	(*agxImageWriteFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
void	(*agxImageFillFunc)(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);

static	void	agxImageFillBank(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
static  void    agxImageFillNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, 
    short, unsigned long
#endif
);
static	void	agxImageReadBank(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static	void	agxImageWriteBank(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static	void	agxImageWriteNoMem(
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);

unsigned long agxPixMask;
int BytesPerPixelShift;
static int screenStride;
unsigned char agxVideoMapFormat = GE_MF_8BPP | GE_MF_INTEL_FORMAT;

#define MAP_MNG 1

#if 1
#define agxSetVGAPage(bank) outb( agxApIdxReg, bank );
#else
#ifdef __GNUC__

#ifdef NO_INLINE
#define __inline__ /**/
#endif

static __inline__ void agxSetVGAPage(unsigned char bank)
{
	outb( agxApIdxReg, bank );
}

#else /* ! __GNUC__ */

#ifdef __STDC__
static void agxSetVGAPage(unsigned char bank)
#else
static void agxSetVGAPage(bank)
unsigned char bank;
#endif
{
	outb( agxApIdxReg, bank );
}

#endif /* __GNUC__ */
#endif

#ifdef __STDC__
void agxSetVidPage(int bank)
#else
void agxSetVidPage(bank)
int bank;
#endif
{
        outb( agxApIdxReg, (unsigned char) bank );
}

void
agxImageInit()
{
    int i;

    agxPixMask = (1UL << agxInfoRec.depth) - 1;
    agxVideoMapFormat = ((BytesPerPixelShift+3) & 0x07) 
                        | GE_MF_INTEL_FORMAT;
    screenStride = agxDisplayWidth << BytesPerPixelShift;


#if 0
    if (xgaUse4MbAperture) {
	/* This code does not need to deal with bank select at all */
	agxImageReadFunc = agxImageRead;
	agxImageWriteFunc = agxImageWrite;
	agxImageFillFunc = agxImageFill;
    }
    else {
#else
    {
#endif

	/* This code does need to deal with bank select */
	agxImageReadFunc = agxImageReadBank;
	agxImageWriteFunc = agxImageWriteBank;
	agxImageFillFunc = agxImageFillBank;
    }
}

/*
 * agxBytePadScratchMapPow2()
 *
 * Determine the map byte width, subject to the constraint that it be
 * a power of 2 and padded out to a byte boundary. Return the amount
 * that a one needs to be shifted by to get that width.
 */ 
int
agxBytePadScratchMapPow2( w, n )
   int  w;        /* map width in pixels */
   int  n;        /* 2^n == bytes per pixel, may be 0 or even neg */
{
   int pw;   /* pixel width */
   int size; 

   for( pw=0, size=1; size < w; size<<=1, pw++ );  /* padded width in pixels */
   pw += n;    /* adjust to bytes */
   return  pw >= 0 ? pw : 0;
}

/*
 * agxMemToVid()
 * 
 */
void
#if NeedFunctionPrototypes
agxMemToVid(
   int  dst,
   int  dstWidth,
   char *src,
   int  srcWidth,
   int  h)
#else
agxMemToVid(dst, dstWidth, src, srcWidth, h)
   int  dst;
   int  dstWidth;
   char *src;
   int  srcWidth;
   int  h;
#endif
{
    unsigned char * curvm;
    unsigned int offset;
    unsigned int bank;
    unsigned int cpyWidth;
    int left;
    int count;

    offset = dst & (agxBankSize-1);
    bank   = (dst >> 16) & 0x3F;
    agxSetVGAPage(bank);
    curvm = (unsigned char*)agxVideoMem + offset;
    cpyWidth = dstWidth < srcWidth ? dstWidth : srcWidth;

    while(h>0) {
        /*
         * calc number of line before need to switch banks
         */
        count = (agxBankSize - offset) / dstWidth;
        if (count >= h) {
                count = h;
                h = 0;
        }
        else {
           offset += count * dstWidth;
           if (offset + cpyWidth  < agxBankSize) {
              count++;
              offset += dstWidth;
           }
           h -= count;
        }

        /*
         * Output line till back switch
         */
        if( srcWidth == dstWidth ) {
           unsigned int size = count * dstWidth;
           MemToBus( curvm, src, size );
           count = 0;
           curvm += size;
           src += size;
        }
        else {
           while(count--) {
              MemToBus( curvm, src, cpyWidth);
              curvm += dstWidth;
              src += srcWidth;
           }
        }

        if (h>0) {
           if (offset < agxBankSize) {
                h--;
                left = agxBankSize - offset;
                MemToBus( curvm, src, left);
                bank++;
                agxSetVGAPage(bank);

                MemToBus( agxVideoMem, src+left, dstWidth-left );
                src += srcWidth;
                offset += dstWidth;
            } else {
                bank++;
                agxSetVGAPage(bank);
            }
            offset &= (agxBankSize-1);
            curvm = (unsigned char *)agxVideoMem + offset;
        }
    }
}

/*
 * agxPartMemToVid()
 * 
 */
void
#if NeedFunctionPrototypes
agxPartMemToVid(
   int  dst,
   int  dstWidth,
   char *src,
   int  srcWidth,
   int  w,
   int  h)
#else
agxPartMemToVid(dst, dstWidth, src, srcWidth, w, h)
   int  dst;
   int  dstWidth;
   char *src;
   int  srcWidth;
   int  w;
   int  h;
#endif
{
    unsigned char *curvm;
    unsigned int offset;
    unsigned int bank;
    int left;
    int count;
    Bool multiCopy = (srcWidth == w) && (dstWidth == w);

    offset = dst & (agxBankSize-1);
    bank   = (dst >> 16) & 0x3F;
    agxSetVGAPage(bank);
    curvm = (unsigned char*)agxVideoMem + offset;

    while(h>0) {
        /*
         * calc number of lines before need to switch banks
         */
        count = (agxBankSize - offset) / dstWidth;
        if (count >= h) {
           count = h;
           h = 0;
        }
        else {
           offset += count * dstWidth;
           if (offset + w  < agxBankSize) {
              count++;
              offset += dstWidth;
           }
           h -= count;
        }

        /*
         * Output line till back switch
         */
        if( multiCopy ) {
           unsigned int size = count * w;
           MemToBus( curvm, src, size );
           count = 0;
           curvm += size;
           src += size;
        }
        else {
           while(count--) {
              MemToBus( curvm, src, w);
              curvm += dstWidth;
              src += srcWidth;
           }
        }

        if (h>0) {
           if ( offset < agxBankSize) {
                h--;
                left = agxBankSize - offset;
                if( left < w )
                   MemToBus( curvm, src, left);
                else
                   MemToBus( curvm, src, w);
                bank++;
                agxSetVGAPage(bank);

                if( w > left )
                   MemToBus( agxVideoMem, src+left, w-left );
                src += srcWidth;
                offset += dstWidth;
            } else {
                bank++;
                agxSetVGAPage(bank);
            }
            offset &= (agxBankSize-1);
            curvm = (unsigned char*)agxVideoMem + offset;
        }
    }
}


static void
#if NeedFunctionPrototypes
agxImageWriteBank(
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
agxImageWriteBank(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
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
#if 1
    int offset;
    unsigned char *curvm;
    int bank;
    int bankMask = agxBankSize-1;
    int left;
    int count;
#endif

    if ((w == 0) || (h == 0))
	return;

    if (alu == MIX_DST)
	  return;

    if ( (alu != MIX_SRC) || ((planemask & agxPixMask) != agxPixMask)) {
        agxImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask);
	return;
    }

#if 0
    GE_WAIT_IDLE();
    agxPartMemToVid( AGX_PIXEL_ADJUST( x ) + y * screenStride,
                     screenStride,
                     psrc + AGX_PIXEL_ADJUST( px ) + py * pwidth, 
                     pwidth,
                     AGX_PIXEL_ADJUST( w ), h );
#else
    w = AGX_PIXEL_ADJUST( w );
    psrc += pwidth * py + AGX_PIXEL_ADJUST( px );
    offset = AGX_PIXEL_ADJUST( x ) + y * screenStride;
    bank = (offset & ~bankMask) >> 16;
    agxSetVGAPage(bank);
    offset &= bankMask;
    curvm = &((unsigned char*)agxVideoMem)[offset];

    GE_WAIT_IDLE();
    while( h-- ) {
        if ( (offset + w) <= agxBankSize) {
            MemToBus( (pointer)curvm, psrc, w );
            psrc += pwidth;
            curvm  += screenStride;
            offset += screenStride;
        }
        else {
            if (offset < agxBankSize) {
                left = agxBankSize - offset;
                MemToBus( (pointer)curvm, psrc, left);
                bank++;
                agxSetVGAPage(bank);
                MemToBus( agxVideoMem, 
                          psrc+left, 
                          w - left );
                psrc += pwidth;
                offset = (offset + screenStride) & bankMask;
                curvm = &((unsigned char*)agxVideoMem)[offset];
            } else {
                bank++;
                agxSetVGAPage(bank);
                offset &= bankMask;
                curvm = &((unsigned char*)agxVideoMem)[offset];
                MemToBus( (pointer)curvm, psrc, w );
                psrc += pwidth;
                curvm  += screenStride;
                offset += screenStride;
            }
        }
    }
#endif
}


static void
#if NeedFunctionPrototypes
agxImageWriteNoMem(
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
agxImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask)
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
    unsigned int	srcPWidth;
    unsigned int	srcBWidth;
    unsigned int	bw = AGX_PIXEL_ADJUST( w );
    int			srcBWidthShift;
    unsigned int	srcStripHeight;
    unsigned int	srcMaxLines;
    unsigned int	srcLine = 0;
    unsigned int	numVertStrips;
    unsigned int	lastVStripHeight;
    unsigned int	strip;
    unsigned int        dstCoOrd = y << 16 | x;

    /*
     * Unlike the 8514 clan, the XGA architecture does not support
     * bitblt's with the src being the input port. Instead
     * the src will need to be copied into a scratchpad map in
     * unused videomem and then used as the source of the bitblt.
     * There are a couple of complications:
     *    1) The scratchpad may not be large enough
     *    2) The AGX implementations require that the source map width
     *       be a power of 2 (there are some exceptions that I'm not
     *       exploiting yet). 
     * This results in 2 major cases:
     *    1) Fits in scratchpad.
     *    2) Doesn't fit in scratchpad.
     *       Source split into multiple horizontal strips.
     */

    srcBWidthShift = agxBytePadScratchMapPow2( w, BytesPerPixelShift );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = AGX_TO_PIXEL( srcBWidth );
    srcMaxLines = agxScratchSize >> srcBWidthShift;

    if( h <= srcMaxLines ) {
       numVertStrips = 1 ;
       lastVStripHeight = h ;
       srcStripHeight = h;
    }
    else {
       numVertStrips = h / srcMaxLines;
       lastVStripHeight = h % srcMaxLines;
       srcStripHeight = srcMaxLines;
       if( lastVStripHeight )
          numVertStrips++;
       else
          lastVStripHeight = srcStripHeight;
    }

    psrc += pwidth * py + AGX_PIXEL_ADJUST( px );

    MAP_INIT( GE_MS_MAP_B,
              agxVideoMapFormat,
              agxMemBase + agxScratchOffset,
              srcPWidth-1,
              srcStripHeight-1,
              FALSE, FALSE, FALSE );
    
    GE_WAIT_IDLE();

    MAP_SET_DST( GE_MS_MAP_A );
    MAP_SET_SRC( GE_MS_MAP_B );
    GE_SET_MAP( GE_MS_MAP_B );

    GE_OUT_B( GE_FRGD_MIX, alu );
    GE_OUT_D( GE_PIXEL_BIT_MASK, planemask );
    GE_OUT_D( GE_OP_DIM_WIDTH, (srcStripHeight-1 << 16) | w-1 );
    GE_OUT_W( GE_PIXEL_OP, 
              GE_OP_PAT_FRGD
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );

    for( strip = 1; strip <= numVertStrips; strip++ ) {

       GE_WAIT_IDLE(); 

       if( strip == numVertStrips ) {
          srcStripHeight = lastVStripHeight;
          GE_OUT_W( GE_OP_DIM_HEIGHT, srcStripHeight - 1 );
       }

       /*
        * Load map B with the current strip. 
        */
       if( numVertStrips == 1 ) { 
          agxPartMemToVid( agxScratchOffset, srcBWidth,
                           psrc, pwidth, 
                           bw, h ); 
       }
       else {
          agxPartMemToVid( agxScratchOffset, srcBWidth, 
                           psrc + (srcLine * pwidth), pwidth,
                           bw, srcStripHeight );
       }

       GE_OUT_D( GE_SRC_MAP_X, 0 );
       GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
       GE_START_CMDW( GE_OPW_BITBLT
                      | GE_OPW_FRGD_SRC_MAP
                      | GE_OPW_SRC_MAP_B
                      | GE_OPW_DEST_MAP_A   );

       srcLine  += srcStripHeight;
       dstCoOrd += srcStripHeight << 16;
    }
    GE_WAIT_IDLE_EXIT();
} 

static void
#if NeedFunctionPrototypes
agxImageReadBank(
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
agxImageReadBank(x, y, w, h, psrc, pwidth, px, py, planemask)
    int			x;
    int			y;
    int			w;
    int			h;
    char		*psrc;
    int			pwidth;     /* already adjusted to bytes */
    int			px;
    int			py;
    unsigned long	planemask;
#endif
{
    unsigned char *curvm;
    int offset;
    int bank;
    unsigned int bankMask = agxBankSize-1;
    int left;

    if ((w == 0) || (h == 0))
	return;


#if 0
    if ((planemask & 0xff) != 0xff) {
        /* ??? */
    }
#endif

    psrc += pwidth * py + AGX_PIXEL_ADJUST( px );
    offset = AGX_PIXEL_ADJUST( x ) + y * screenStride;
    bank = offset / agxBankSize;
    agxSetVGAPage(bank);
    offset &= bankMask;
    curvm = &((unsigned char*)agxVideoMem)[offset];
    w = AGX_PIXEL_ADJUST( w );

    GE_WAIT_IDLE();
    while( h-- ) {
	if (offset + w <= agxBankSize) {
            BusToMem(psrc, (pointer)curvm, w );
            psrc += pwidth;
            curvm += screenStride;
            offset += screenStride;
        }
        else {
	    if (offset < agxBankSize) {
		left = agxBankSize - offset;
		BusToMem(psrc, (pointer)curvm, left);
		bank++;
		agxSetVGAPage(bank);
		BusToMem( psrc+left, 
                          (pointer)((unsigned char*)agxVideoMem), 
                          w - left );
                psrc += pwidth;
                offset = (offset + screenStride) & bankMask;
                curvm = &((unsigned char*)agxVideoMem)[offset];
	    } else {
		bank++;
		agxSetVGAPage(bank);
		offset &= bankMask; 
		curvm = &((unsigned char*)agxVideoMem)[offset];
		BusToMem(psrc, (pointer)curvm, w );
                psrc += pwidth;
                curvm += screenStride;
                offset += screenStride;
	    }
	}
    }
}


static void
#if NeedFunctionPrototypes
agxImageFillBank(
    int                 x,
    int                 y,
    int                 w,
    int                 h,
    char	       *psrc,
    int                 pwidth,
    int                 pw,
    int                 ph,
    int                 pox,
    int                 poy,
    short               alu,
    unsigned long       planemask)
#else
agxImageFillBank(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    char	       *psrc;
    int                 pwidth;
    int                 pw;
    int                 ph;
    int                 pox;
    int                 poy;
    short               alu;
    unsigned long       planemask;
#endif
{
    int			xrot;
    int			yrot;

    if ((w == 0) || (h == 0))
        return;

    if (alu == MIX_DST)
        return;

    modulus( x-pox, pw, xrot );
    modulus( y-poy, ph, yrot );

    if ( (alu != MIX_SRC) 
         || ((planemask&agxPixMask) != agxPixMask)
         || w + xrot > pw
         || h + yrot > ph ) {
        agxImageFillNoMem(x, y, w, h, psrc, pwidth, pw, ph, pox,
                             poy, alu, planemask);
    }
    else {

       GE_WAIT_IDLE();
   
       agxPartMemToVid( AGX_PIXEL_ADJUST(x) + y * screenStride, screenStride,
                        psrc + AGX_PIXEL_ADJUST(xrot) + yrot * pwidth, pwidth,
                        AGX_PIXEL_ADJUST( w ), h );
    }
}

static void
#if NeedFunctionPrototypes
agxImageFillNoMem(
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
    unsigned long 	planemask)
#else
agxImageFillNoMem(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
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
    unsigned long 	planemask;
#endif
{
    unsigned int      srcPWidth;
    unsigned int      bw;
    unsigned int      srcBWidth;
    int  		srcBWidthShift;
    unsigned int      srcStripHeight;
    unsigned int        srcMaxLines;
    unsigned int      srcLine;
    unsigned int      numHorizTiles;
    unsigned int      firstHTileWidth = 0;
    unsigned int      lastHTileWidth = 0;
    unsigned int      firstBWidth = 0;
    unsigned int      lastBWidth = 0;
    unsigned int      numVertTiles;
    unsigned int        lastVTileHeight;
    unsigned int      numLastVTileStrips;
    unsigned int        lastVTileLastStripHeight;
    unsigned int      numVertStrips;
    unsigned int        lastVStripHeight;
    unsigned int      strip;
    unsigned int      vStripFirstY;         
    int			xBrot;
    int			xrot;
    int			yrot;
    unsigned int      newWidth;
    unsigned int      newBWidth;
    unsigned int      firstHeight;
    unsigned int      secondHeight;
    unsigned int      vTile;

    modulus( x-pox, pw, xrot );
    modulus( y-poy, ph, yrot );

    /*
     * Unlike the 8514 clan, the XGA architecture does not support
     * bitblt's with the src being the input port. Instead
     * the src will need to be copied into a scratchpad map in
     * unused videomem and then used as the source of the bitblt.
     * There are a couple of complications:
     *    1) The scratchpad may not be large enough
     *    2) The AGX implementations require that the source map width
     *       be a power of 2. This means the bitblt'er can only
     *       do tiling directly if the source width is a power of two.
     * This results in 4 major cases:
     *    1) Fits in scratchpad and it's width is a power of 2.
     *       The bitblt can be done directly including any tiling.
     *    2) Fits in scratchpad and it's width is not a power of 2.
     *       Any horizontal tiling will have to be done with multiple
     *       bitblt's.
     *    3) Doesn't fit in scratchpad and it's width is a power of 2.
     *       Source split into multiple horizontal strips. The bitblt'r 
     *       does horizontal tiling but vertical tiling is done
     *       with multiple Bitblt's. 
     *    4) Doesn't fit in scratchpad and it's width is not a power of 2.
     *       Source split into multiple horizontal strips, The bitblt'r
     *       does no tiling.
     *
     * This is further complicated by the pox and poy offsets that
     * need to be considered in the cases that the bitblt'r is unable
     * to handle tiling on its own.
     *
     * An optimization to be considered in the future, would be 
     * to split the scratchpad into two buffers, and for all cases
     * that are not trivially small, or for which hardware tiling cannot
     * be explioted in both directions, overlap the copies and 
     * the bitblt to maximize concurrency (whether the vidoe memory
     * has the bandwidh to do this without an overall loss in performance
     * has not been tested).
     *
     * Another optimization would be to seperate-out common cases that
     * don't requires SW tiling so they don't incur the computational 
     * and loop overhead of this routine.
     */

    bw = AGX_PIXEL_ADJUST( pw );
    newWidth = w > pw ? pw : w;
    newBWidth = AGX_PIXEL_ADJUST( newWidth );
    srcBWidthShift = agxBytePadScratchMapPow2( newWidth, BytesPerPixelShift );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = AGX_TO_PIXEL( srcBWidth );
    srcMaxLines = agxScratchSize >> srcBWidthShift; 

    if( w == newWidth ) {
       firstHTileWidth = w; 
       lastHTileWidth = 0; 
       numHorizTiles = 1;
    }
    else if( pw == srcPWidth ) {
       firstHTileWidth = w; 
       lastHTileWidth = 0;
       numHorizTiles = 1;
    }
    else {
       firstHTileWidth = pw;
       numHorizTiles  = w / pw;
       lastHTileWidth = w % pw; 
       if( lastHTileWidth )
          numHorizTiles++;
    } 

    if( xrot == 0 ) {
       firstBWidth = 0;
       lastBWidth = newBWidth;
    }
    else { 
       firstBWidth = AGX_PIXEL_ADJUST(pw - xrot);
       if( firstBWidth > newBWidth ) 
          firstBWidth = newBWidth; 
       lastBWidth = newBWidth - firstBWidth;
    }
    xBrot = AGX_PIXEL_ADJUST(xrot);
    
    if( h < ph ) {
       if( h <= srcMaxLines ) {
          numVertStrips = 1 ;
          lastVStripHeight = h ;
          srcStripHeight = h;
       }
       else if( h < ph ) {
          numVertStrips = h / srcMaxLines;
          lastVStripHeight = h % srcMaxLines;
          srcStripHeight = srcMaxLines;
          if( lastVStripHeight )
             numVertStrips++;
          else  
             lastVStripHeight = srcStripHeight;
       }
    }
    else if( ph <= srcMaxLines ) {
       numVertStrips = 1 ;
       lastVStripHeight = ph ;
       srcStripHeight = ph;
    }
    else {
       numVertStrips = ph / srcMaxLines;
       lastVStripHeight = ph % srcMaxLines;
       srcStripHeight = srcMaxLines;
       if( lastVStripHeight )
          numVertStrips++;
       else
          lastVStripHeight = srcStripHeight;
    }

    if( numVertStrips == 1 ) { 
       numVertTiles = 1;
       numLastVTileStrips = 1;
       lastVTileLastStripHeight = h;
    }
    else { 
       numVertTiles = h / ph;
       lastVTileHeight = h % ph;
       if( lastVTileHeight ) 
           numVertTiles++;
       else
          lastVTileHeight = ph; 

       numLastVTileStrips = lastVTileHeight / srcStripHeight; 
       lastVTileLastStripHeight = lastVTileHeight % srcStripHeight; 
       if( lastVTileLastStripHeight )
          numLastVTileStrips++;
       else
          lastVTileLastStripHeight = srcStripHeight;
    }

    srcLine = yrot; 
    vStripFirstY = y;

    MAP_INIT( GE_MS_MAP_B,
              agxVideoMapFormat,
              agxMemBase + agxScratchOffset,
              srcPWidth-1,
              srcStripHeight-1,
              FALSE, FALSE, FALSE );
   
    GE_WAIT_IDLE();

    MAP_SET_DST( GE_MS_MAP_A );
    MAP_SET_SRC( GE_MS_MAP_B );
    GE_SET_MAP( GE_MS_MAP_B );

    GE_OUT_B( GE_FRGD_MIX, alu );
    GE_OUT_D( GE_PIXEL_BIT_MASK, planemask );
    GE_OUT_W( GE_PIXEL_OP, 
              GE_OP_PAT_FRGD
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );

    for( strip = 1; strip <= numVertStrips; strip++ ) {
       unsigned int dstY = vStripFirstY;

       /*
        * Load map B with the current strip. 
        */
       firstHeight = ph - srcLine;
       if( firstHeight <= srcStripHeight && firstHeight > (unsigned)h ) {
          firstHeight = h; 
          secondHeight = 0;
       }
       else if( firstHeight < srcStripHeight ) {
          secondHeight = srcStripHeight - firstHeight;
       }
       else {
          firstHeight = srcStripHeight;
          secondHeight = 0;
       }

       GE_WAIT_IDLE();

       agxPartMemToVid( agxScratchOffset + firstBWidth, srcBWidth, 
                        psrc + srcLine * pwidth, pwidth,
                        lastBWidth, firstHeight );
       if( firstBWidth > 0 ) {
          agxPartMemToVid( agxScratchOffset, srcBWidth, 
                           psrc + srcLine * pwidth + xBrot, pwidth,
                           firstBWidth, firstHeight );
       }
     
       if( secondHeight ) {
          agxPartMemToVid( agxScratchOffset + firstHeight*srcBWidth 
                             + firstBWidth, srcBWidth,
                            psrc, pwidth,
                            lastBWidth, secondHeight );
          if( firstBWidth > 0 ) {
             agxPartMemToVid( agxScratchOffset + firstHeight*srcBWidth,
                                 srcBWidth,
                              psrc + xBrot, pwidth,
                              firstBWidth, secondHeight );
          }
       }

       if( strip == numVertStrips ) {
          srcStripHeight = lastVStripHeight;
          GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MAP_B );
          GE_OUT_W( GE_PIXEL_MAP_HEIGHT, srcStripHeight - 1 );
       }
          
       for( vTile = 1; vTile <= numVertTiles; vTile++ ) {
          register unsigned int dstX;
          unsigned int hTile;
          unsigned int dstHeight;
   
          if( vTile == numVertTiles )
             if( strip < numLastVTileStrips )
                dstHeight = srcStripHeight - 1;
             else if( strip == numLastVTileStrips )
                dstHeight = lastVTileLastStripHeight - 1;
             else
                continue;
          else
             dstHeight = srcStripHeight - 1;

          dstX = x;
          for( hTile = 1; hTile <= numHorizTiles; hTile++ ) {
             register unsigned int dstWidth;

             if( hTile == 1 ) {
                dstWidth = firstHTileWidth - 1;
             }
             else if( hTile == numHorizTiles ) {
                dstWidth = lastHTileWidth - 1;
             }
             else {
                dstWidth = pw - 1; 
             }
     
             { 
               register unsigned int dstCoOrd = dstY << 16 | dstX;
               register unsigned int opDim = dstHeight << 16 | dstWidth;
               
               GE_WAIT_IDLE();
               GE_OUT_D( GE_SRC_MAP_X,  0 );
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_MAP
                              | GE_OPW_SRC_MAP_B
                              | GE_OPW_DEST_MAP_A   );
             }
             dstX += dstWidth + 1;    
          }
          dstY += ph;
       }
       srcLine += srcStripHeight;
       if( srcLine > (unsigned)ph )
          srcLine -= ph; 
       vStripFirstY += srcStripHeight;
    }
    GE_WAIT_IDLE_EXIT();
}

void
#if NeedFunctionPrototypes
agxImageTileScrPad(
    int                 x,
    int                 y,
    int                 w,
    int                 h,
    int                 pwidth,
    int                 pw,
    int                 ph,
    int                 xrot,
    int                 yrot )
#else
agxImageTileScrPad( x, y, w, h, pwidth, pw, ph, xrot, yrot )
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    int                 pwidth;
    int                 pw;
    int                 ph;
    int                 xrot;
    int                 yrot;
#endif
{
    register unsigned int numHorizTiles;
    unsigned int          firstHTileWidth = pw - xrot;
    unsigned int          lastHTileWidth = 0;

    if( w <= firstHTileWidth ||  pw == pwidth ) {
       firstHTileWidth = w;
       lastHTileWidth = 0;
       numHorizTiles = 1;
    }
    else {
       unsigned int rem;
       rem = w - firstHTileWidth;
       numHorizTiles  = rem / pw + 1;
       lastHTileWidth = rem % pw;
       if( lastHTileWidth )
          numHorizTiles++;
       else
          lastHTileWidth = pw;
    }

    {
       unsigned int hTile;
       unsigned int dstX = x;
       unsigned int srcCoOrd = yrot << 16;
       unsigned int dstCoOrd = y << 16;
       unsigned int opDim = (h - 1) << 16;

       for( hTile = 1; hTile <= numHorizTiles; hTile++ ) {
          register unsigned int dstWidth;
          register unsigned int srcX;

          if( hTile == 1 ) {
             srcX = xrot;
             dstWidth = firstHTileWidth - 1;
          }
          else if( hTile == numHorizTiles ) {
             srcX = 0;
             dstWidth = lastHTileWidth - 1;
          }
          else {
             srcX = 0;
             dstWidth = pw - 1;
          }

          srcCoOrd = (srcCoOrd & 0xFFFF0000) | srcX;
          dstCoOrd = (dstCoOrd & 0xFFFF0000) | dstX;
          opDim    = (opDim & 0xFFFF0000) | dstWidth;

          GE_WAIT_IDLE();
          GE_OUT_D( GE_SRC_MAP_X,  srcCoOrd );
          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
          GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
          GE_START_CMDW( GE_OPW_BITBLT
                         | GE_OPW_FRGD_SRC_MAP
                         | GE_OPW_SRC_MAP_B
                         | GE_OPW_DEST_MAP_A   );
          dstX += dstWidth + 1;
       }
    }
    GE_WAIT_IDLE_EXIT();
}


void
#if NeedFunctionPrototypes
agxImageStipple(int x, int y, int w, int h, char *psrc, int pwidth, int pw,
		int ph, int pox, int poy, Pixel fgPixel, Pixel bgPixel,
		short fgAlu, short bgAlu, unsigned long planemask)
#else
agxImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy, 
                 fgPixel, bgPixel, fgAlu, bgAlu, planemask)
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
    short		fgAlu;
    short		bgAlu;
    unsigned long	planemask;
#endif
{
    unsigned int      srcPWidth = 0;
    unsigned int      srcBWidth;
    int  	      srcBWidthShift;
    unsigned int      srcStripHeight;
    unsigned int      srcMaxLines;
    unsigned int      srcLine;
    unsigned int      numHorizTiles;
    unsigned int      firstHTileWidth = 0;
    unsigned int      lastHTileWidth;
    unsigned int      firstBWidth;
    unsigned int      lastBWidth;
    unsigned int      numLastVTileStrips;
    unsigned int      lastVTileLastStripHeight;
    unsigned int      numVertTiles;
    unsigned int      lastVTileHeight;
    unsigned int      numVertStrips;
    unsigned int      lastVStripHeight;
    unsigned int      strip;
    unsigned int      vStripFirstY; 
    Bool              autoHTiling;
    int		      xrot;
    int		      yrot;
    int               xOff;
    int               xStart;
    unsigned int      vTile;
    unsigned int      dstY;
    unsigned int      newWidth;
    unsigned int      firstHeight;
    unsigned int      secondHeight;
    unsigned int      firstWidth;
    unsigned int      remWidth;

    modulus( x-pox, pw, xrot );
    modulus( y-poy, ph, yrot );

    /*
     * Unlike the 8514 clan, the XGA architecture does not support
     * bitblt's with the src being the input port. Instead
     * the src will need to be copied into a scratchpad map in
     * unused videomem and then used as the source of the bitblt.
     * There are a couple of complications:
     *    1) The scratchpad may not be large enough
     *    2) The AGX implementations require that the source map width
     *       be a power of 2. This means the bitblt'er can only
     *       do tiling directly if the source width is a power of two.
     * This results in 4 major cases:
     *    1) Fits in scratchpad and it's width is a power of 2.
     *       The bitblt can be done directly including any tiling.
     *    2) Fits in scratchpad and it's width is not a power of 2.
     *       Any horizontal tiling will have to be done with multiple
     *       bitblt's.
     *    3) Doesn't fit in scratchpad and it's width is a power of 2.
     *       Source split into multiple horizontal strips. The bitblt'r 
     *       does horizontal tiling but vertical tiling is done
     *       with multiple Bitblt's. 
     *    4) Doesn't fit in scratchpad and it's width is not a power of 2.
     *       Source split into multiple horizontal strips, The bitblt'r
     *       does no tiling.
     *
     * This is further complicated by the pox and poy offsets that
     * need to be considered in the cases that the bitblt'r is unable
     * to handle tiling on its own.
     *
     * An optimization to be considered in the future, would be 
     * to split the scratchpad into two buffers, and for all cases
     * that are not trivially small, overlap the copies and 
     * the bitblt to maximize concurrency.
     */

    newWidth = w < pw - xrot ? w : pw;
    srcBWidthShift = agxBytePadScratchMapPow2( pw+7, -3 );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = srcBWidth << 3;
    srcMaxLines = agxScratchSize >> srcBWidthShift;

    if( pw == srcPWidth && pw < w ) {
       numHorizTiles = 1;
       firstHTileWidth = pw;
       lastHTileWidth = 0;
       autoHTiling = TRUE;
       firstBWidth = 0;
       lastBWidth = pw+7 >> 3;
       xOff = 0;
    }
    else {
       autoHTiling = FALSE;
       xOff = xrot >> 3;
       xStart = xrot & 0xFFFFFFF8;
       firstWidth = pw - xrot;
       firstHTileWidth = firstWidth <= w ? firstWidth : w; 
       remWidth = w - firstHTileWidth;
       numHorizTiles  = remWidth / pw;
       lastHTileWidth = remWidth % pw;
       if( remWidth == 0 ) {
          firstBWidth = (xrot + firstHTileWidth - xStart)+7 >> 3;
          lastBWidth = 0; 
       }
       else if( lastHTileWidth > 0 && lastHTileWidth < xStart ) {
          firstBWidth = (pw+7 >> 3) - xOff; 
          lastBWidth = lastHTileWidth+7 >> 3;
       }
       else {
          firstBWidth = 0;
          lastBWidth = pw+7 >> 3;
       }
       if( firstHTileWidth > 0 )
          numHorizTiles++;
       if( lastHTileWidth > 0 )
          numHorizTiles++;
       else
          lastHTileWidth = pw;
    } 

    if( ph <= srcMaxLines ) {
       numVertStrips = 1 ;
       lastVStripHeight = ph ;
       srcStripHeight = ph;
    }
    else {
       numVertStrips = ph / srcMaxLines;
       lastVStripHeight = ph % srcMaxLines;
       srcStripHeight = srcMaxLines;
       if( lastVStripHeight )
          numVertStrips++;
       else
          lastVStripHeight = srcStripHeight;
    }
 
    if( numVertStrips == 1 ) {
       numVertTiles = 1;
       numLastVTileStrips = 1;
       lastVTileLastStripHeight = h;
    }
    else {
       numVertTiles = h / ph;
       lastVTileHeight = h % ph;
       if( lastVTileHeight )
           numVertTiles++;
       else
          lastVTileHeight = ph;

       numLastVTileStrips = lastVTileHeight / srcStripHeight;
       lastVTileLastStripHeight = lastVTileHeight % srcStripHeight;
       if( lastVTileLastStripHeight )
          numLastVTileStrips++;
       else
          lastVTileLastStripHeight = srcStripHeight;
    }

    srcLine = yrot; 
    vStripFirstY = y;

#if 0
    if( lastBWidth > srcBWidth
        || lastBWidth > (w+7>>3)
        || lastBWidth > (pw+7>>3) ) {
       ErrorF( "%s:%s - lastBWidth is too wide: %x,%x,%x,%x\n",
                __FILE__, __LINE__,
                lastBWidth, srcBWidth,
                w<<BytesPerPixelShift, pw<<BytesPerPixelShift );
    }
    if( firstBWidth > srcBWidth
        || firstBWidth > (w+7>>3)
        || firstBWidth > (pw+7>>3) ) {
       ErrorF( "%s:%s - firstBWidth is too wide: %x,%x,%x,%x\n",
                __FILE__, __LINE__,
                firstBWidth, srcBWidth,
                w<<BytesPerPixelShift, pw<<BytesPerPixelShift );
    }
    if( firstBWidth+lastBWidth > srcBWidth
        || firstBWidth+lastBWidth > (w+7>>3)
        || firstBWidth+lastBWidth > (pw+7>>3) ) {
       ErrorF( "%s:%s - firstBWidth+lastBWidth is too wide: %x,%x,%x,%x,%x\n",
                __FILE__, __LINE__,
                firstBWidth, lastBWidth, srcBWidth,
                w<<BytesPerPixelShift, pw<<BytesPerPixelShift );
    }
#endif

#if 0
    if( lastBWidth > srcBWidth
        || lastBWidth > (w+7>>3)
        || lastBWidth > (pw+7>>3) ) {
       ErrorF( "%s:%s - lastBWidth is too wide: %x,%x,%x,%x\n",
                __FILE__, __LINE__,
                lastBWidth, srcBWidth,
                AGX_PIXEL_ADJUST( w ), AGX_PIXEL_ADJUST( pw ));
    }
    if( firstBWidth > srcBWidth
        || firstBWidth > (w+7>>3)
        || firstBWidth > (pw+7>>3) ) {
       ErrorF( "%s:%s - firstBWidth is too wide: %x,%x,%x,%x\n",
                __FILE__, __LINE__,
                firstBWidth, srcBWidth,
                AGX_PIXEL_ADJUST( w ), AGX_PIXEL_ADJUST( pw ));
    }
    if( firstBWidth+lastBWidth > srcBWidth
        || firstBWidth+lastBWidth > (w+7>>3)
        || firstBWidth+lastBWidth > (pw+7>>3) ) {
       ErrorF( "%s:%s - firstBWidth+lastBWidth is too wide: %x,%x,%x,%x,%x\n",
                __FILE__, __LINE__,
                firstBWidth, lastBWidth, srcBWidth,
                AGX_PIXEL_ADJUST( w ), AGX_PIXEL_ADJUST( pw ));
    }
#endif

    MAP_INIT( GE_MS_MAP_B, 
              GE_MF_1BPP | GE_MF_INTEL_FORMAT,
              agxMemBase + agxScratchOffset, 
              srcPWidth-1, 
              srcStripHeight-1,
              FALSE, FALSE, FALSE );
   
    GE_WAIT_IDLE();

    MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
    GE_SET_MAP( GE_MS_MAP_B );   

    GE_OUT_W(GE_FRGD_MIX, bgAlu << 8 | fgAlu );  /* both fg & bg */
    GE_OUT_D(GE_FRGD_CLR, fgPixel );
    GE_OUT_D(GE_BKGD_CLR, bgPixel );
    GE_OUT_D(GE_PIXEL_BIT_MASK, planemask);
    GE_OUT_W( GE_PIXEL_OP,
              GE_OP_PAT_MAP_B
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );

    for( strip = 1; strip <= numVertStrips; strip++ ) {

       dstY = vStripFirstY;

       GE_WAIT_IDLE();

       if( strip == numVertStrips ) {
          srcStripHeight = lastVStripHeight;
          GE_OUT_B( GE_PIXEL_MAP_SEL, GE_MS_MAP_B );
          GE_OUT_W( GE_PIXEL_MAP_HEIGHT, srcStripHeight-1 );
       }

       /*
        * Load map B with the current strip. 
        */
       if( autoHTiling && numVertStrips == 1 ) { 
          agxPartMemToVid( agxScratchOffset, srcBWidth,
                           psrc, pwidth,
                           lastBWidth, ph ); 
       }
       else
       {
          firstHeight = ph - srcLine;
          secondHeight =  0;

          if( firstHeight <= srcStripHeight && firstHeight > (unsigned)h ) {
             firstHeight = h;
             secondHeight = 0;
          }
          else if( firstHeight < srcStripHeight )
             secondHeight = srcStripHeight - firstHeight;
          else
             firstHeight = srcStripHeight;

          /* load only what we need */
          if( firstBWidth > 0 ) {
             agxPartMemToVid( agxScratchOffset + xOff, srcBWidth, 
                              psrc + srcLine*pwidth + xOff, pwidth,
                              firstBWidth, firstHeight );
          }
          if( lastBWidth > 0 ) {
             agxPartMemToVid( agxScratchOffset, srcBWidth, 
                              psrc + srcLine*pwidth, pwidth,
                              lastBWidth, firstHeight );
          }
          if( secondHeight ) {
             /* load only what we need */
             if( firstBWidth > 0 ) {
                agxPartMemToVid( agxScratchOffset+firstHeight*srcBWidth+xOff,
                                    srcBWidth, 
                                 psrc + xOff, pwidth,
                                 firstBWidth, secondHeight );
             }
             if( lastBWidth > 0 ) {
                agxPartMemToVid( agxScratchOffset + firstHeight*srcBWidth,
                                    srcBWidth, 
                                 psrc, pwidth,
                                 lastBWidth, secondHeight );
             }
          }
       }

       for( vTile = 1; vTile <= numVertTiles; vTile++ ) {
          unsigned int hTile;
          unsigned int dstX;
          unsigned int srcX, srcY;
          unsigned int dstHeight;

          if( vTile == numVertTiles )
             if( strip < numLastVTileStrips )
                dstHeight = srcStripHeight - 1;
             else if( strip == numLastVTileStrips )
                dstHeight = lastVTileLastStripHeight - 1;
             else  
                continue;  
          else
             dstHeight = srcStripHeight - 1;

          dstX = x;

          GE_WAIT_IDLE();
          GE_OUT_W( GE_OP_DIM_HEIGHT, dstHeight );

          for( hTile = 1; hTile <= numHorizTiles; hTile++ ) {
            unsigned int dstWidth;

            if( autoHTiling ) {
               dstWidth = w - 1;
               srcX = xrot;
               if( numVertStrips == 1 )
                  srcY = yrot;
               else
                  srcY = 0;
            }
            else {
                if( hTile == 1 ) {
                   dstWidth = firstHTileWidth - 1;
                   srcX = xrot;
                   srcY = 0;
                }
                else if( hTile == numHorizTiles ) {
                   dstWidth = lastHTileWidth - 1;
                   srcX = 0;
                   srcY = 0;
                }
                else {
                   dstWidth = pw - 1; 
                   srcX = 0;
                   srcY = 0;
                }
            }
     
            { 
               register unsigned int srcCoOrd = srcY << 16 | srcX;
               register unsigned int dstCoOrd = dstY << 16 | dstX;
               GE_WAIT_IDLE();
               GE_OUT_D( GE_PAT_MAP_X,  srcCoOrd );
               GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
               GE_OUT_W( GE_OP_DIM_WIDTH, dstWidth );
               GE_START_CMDW( GE_OPW_BITBLT
                              | GE_OPW_FRGD_SRC_CLR
                              | GE_OPW_BKGD_SRC_CLR
                              | GE_OPW_DEST_MAP_A   );
            }
            dstX += dstWidth + 1;    
          }
          dstY += ph; 
       }
       srcLine += srcStripHeight;
       if( srcLine > (unsigned)ph )
       srcLine -= ph; 
       vStripFirstY += srcStripHeight;
    }
    GE_WAIT_IDLE_EXIT();
}

void
#if NeedFunctionPrototypes
agxImageStipScrPad(
    int                 x,
    int                 y,
    int                 w,
    int                 h,
    int                 pwidth,
    int                 pw,
    int                 ph,
    int                 xrot,
    int                 yrot )
#else
agxImageStipScrPad( x, y, w, h, pwidth, pw, ph, xrot, yrot )
    int                 x;
    int                 y;
    int                 w;
    int                 h;
    int                 pwidth;
    int                 pw;
    int                 ph;
    int                 xrot;
    int                 yrot;
#endif
{
    register unsigned int numHorizTiles;
    unsigned int          firstHTileWidth = pw - xrot;
    unsigned int          lastHTileWidth = 0;

    if( w <= firstHTileWidth ||  pw == pwidth ) {
       firstHTileWidth = w;
       lastHTileWidth = 0;
       numHorizTiles = 1;
    }
    else {
       unsigned int rem;
       rem = w - firstHTileWidth;
       numHorizTiles  = rem / pw + 1;
       lastHTileWidth = rem % pw;
       if( lastHTileWidth )
          numHorizTiles++;
       else
          lastHTileWidth = pw;
    }

    {
       unsigned int hTile;
       unsigned int dstX = x;
       unsigned int srcCoOrd = yrot << 16;
       unsigned int dstCoOrd = y << 16;
       unsigned int opDim = (h - 1) << 16;

       for( hTile = 1; hTile <= numHorizTiles; hTile++ ) {
          unsigned int dstWidth;
          unsigned int srcX;

          if( hTile == 1 ) {
             srcX = xrot;
             dstWidth = firstHTileWidth - 1;
          }
          else if( hTile == numHorizTiles ) {
             srcX = 0;
             dstWidth = lastHTileWidth - 1;
          }
          else {
             srcX = 0;
             dstWidth = pw - 1;
          }

          srcCoOrd = (srcCoOrd & 0xFFFF0000) | srcX;
          dstCoOrd = (dstCoOrd & 0xFFFF0000) | dstX;
          opDim    = (opDim & 0xFFFF0000) | dstWidth;

          GE_WAIT_IDLE();
          GE_OUT_D( GE_PAT_MAP_X,  srcCoOrd );
          GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
          GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
          GE_START_CMDW( GE_OPW_BITBLT
                         | GE_OPW_FRGD_SRC_CLR
                         | GE_OPW_BKGD_SRC_CLR
                         | GE_OPW_DEST_MAP_A   );
          dstX += dstWidth + 1;
       }
    }
    GE_WAIT_IDLE_EXIT();
}


void
#if NeedFunctionPrototypes
agxFillBoxStipple(DrawablePtr pDrawable, int nBox, BoxPtr pBox, 
                   PixmapPtr stipple, int pox, int poy,
                   Pixel fgpixel, Pixel bgpixel, 
                   short fgAlu, short bgAlu,
                   unsigned long planemask)
#else
agxFillBoxStipple( pDrawable, nBox, pBox, 
                   stipple, pox, poy,
                   fgpixel, bgpixel, 
                   fgAlu, bgAlu,
                   planemask )
    DrawablePtr     pDrawable;
    int             nBox;
    BoxPtr          pBox;
    PixmapPtr       stipple;
    int             pox;
    int             poy;
    Pixel	    fgpixel;
    Pixel	    bgpixel;
    short	    fgAlu;
    short	    bgAlu;
    unsigned long   planemask;
#endif
{
    int            w, h;
    unsigned int   srcX, srcY;
    unsigned int   pixBWidth;
    unsigned int   srcPWidth;
    unsigned int   srcBWidth;
    int            srcBWidthShift;
    unsigned int   srcMaxLines;
    unsigned int   width = stipple->drawable.width;
    unsigned int   height = stipple->drawable.height;
    
    srcBWidthShift = agxBytePadScratchMapPow2( width, -3 );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = srcBWidth << 3;
    srcMaxLines = agxScratchSize >> srcBWidthShift;
    pixBWidth = BitmapBytePad( width );

    if( nBox == 1 ) {
       int            xrot, yrot;
       modulus( pBox->x1-pox, width, xrot );
       modulus( pBox->y1-poy, height, yrot );
       w = pBox->x2 - pBox->x1;
       h = pBox->y2 - pBox->y1;
       if( (w == 0) || (h == 0) )
          return;
       if( w < width || h < height ) {
          /* box is smaller than pattern -- don't load entire pattern  */
          agxImageStipple( pBox->x1, pBox->y1,
                           w, h,
                           stipple->devPrivate.ptr, pixBWidth,
                           width, height,
                           pox, poy,
                           fgpixel, bgpixel,
                           fgAlu, bgAlu,
                           planemask );
          return;
       }
    }

    if ( srcMaxLines < height ) { 
       /* pattern doesn't fit in scratchpad */
       for (; nBox; nBox--, pBox++) {
          h = pBox->y2 - pBox->y1 - 1;
          w = pBox->x2 - pBox->x1 - 1;
   
          if ((w >= 0) && (h >= 0)) {
             agxImageStipple( pBox->x1, pBox->y1,
                              pBox->x2 - pBox->x1,
                              pBox->y2 - pBox->y1,
                              stipple->devPrivate.ptr, pixBWidth,
                              width, height, 
                              pox, poy,
                              fgpixel, bgpixel,
                              fgAlu, bgAlu,
                              planemask );
          }
       }
    }
    else {
       /* pattern fits in scratchpad */
       MAP_INIT( GE_MS_MAP_B,
                 GE_MF_1BPP | GE_MF_INTEL_FORMAT,
                 agxMemBase + agxScratchOffset,
                 srcPWidth-1,
                 height-1,
                 FALSE, FALSE, FALSE );
   
       GE_WAIT_IDLE();
       MAP_SET_DST( GE_MS_MAP_A );
       GE_SET_MAP( GE_MS_MAP_B );
       GE_OUT_W(GE_FRGD_MIX, bgAlu << 8 | fgAlu );
       GE_OUT_D(GE_FRGD_CLR, fgpixel );
       GE_OUT_D(GE_BKGD_CLR, bgpixel );
       GE_OUT_D(GE_PIXEL_BIT_MASK, planemask);
       GE_OUT_W( GE_PIXEL_OP,
                 GE_OP_PAT_MAP_B
                 | GE_OP_MASK_DISABLED
                 | GE_OP_INC_X
                 | GE_OP_INC_Y         );
   
       agxMemToVid( agxScratchOffset, srcBWidth,
                    stipple->devPrivate.ptr, pixBWidth, height );

       if( srcPWidth == width ) {
          /* pow2 -- bitblt'r can do tiling */
          unsigned int widthMask = width - 1;
          unsigned int bwidth;
   
          bwidth = width >> 3;
          if( bwidth == 0 )
             bwidth = 1;
  
          for (; nBox; nBox--, pBox++) {
             h = pBox->y2 - pBox->y1 - 1;
             w = pBox->x2 - pBox->x1 - 1;
   
             if ((w >= 0) && (h >= 0)) {
                register unsigned int patCoOrd =
                       ((pBox->y1 - poy) % height) << 16
                       | (pBox->x1 - pox) & widthMask;
                register unsigned int dstCoOrd = pBox->y1 << 16 | pBox->x1;
                register unsigned int opDim = h << 16 | w;
   
                GE_WAIT_IDLE();
                GE_OUT_D( GE_PAT_MAP_X, patCoOrd );
                GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                GE_START_CMDW( GE_OPW_BITBLT
                               | GE_OPW_FRGD_SRC_CLR
                               | GE_OPW_BKGD_SRC_CLR
                               | GE_OPW_DEST_MAP_A   );
             }
          }
          GE_WAIT_IDLE_EXIT();
       } 
       else {
          /* tiling can't be done directly */
          for (; nBox; nBox--, pBox++) {
             int            xrot, yrot;
             modulus( pBox->x1-pox, width, xrot );
             modulus( pBox->y1-poy, height, yrot );

             h = pBox->y2 - pBox->y1;
             w = pBox->x2 - pBox->x1;
             if ((w > 0) && (h > 0)) {
                 agxImageStipScrPad( pBox->x1, pBox->y1,
                                     w, h,
                                     srcPWidth,
                                     width, height,
                                     xrot, yrot );
             }
          }
       }
    }
}
 

void
#if NeedFunctionPrototypes
agxFillBoxTile(DrawablePtr pDrawable, int nBox, BoxPtr pBox, PixmapPtr tile,
	       int pox, int poy, short alu, unsigned long planemask )
#else
agxFillBoxTile( pDrawable, nBox, pBox, tile, pox, poy, alu, planemask )
    DrawablePtr     pDrawable;
    int             nBox;
    BoxPtr          pBox;
    PixmapPtr       tile;
    int             pox;
    int             poy;
    short	    alu;
    unsigned long   planemask;
#endif
{
    register int   w, h;
    unsigned int   width, height;
    unsigned int   pixBWidth;
    unsigned int   srcPWidth;
    unsigned int   srcBWidth;
    int            srcBWidthShift;
    unsigned int   srcMaxLines;

    width = tile->drawable.width;
    height = tile->drawable.height;
    
    srcBWidthShift = agxBytePadScratchMapPow2( width, BytesPerPixelShift );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = AGX_TO_PIXEL( srcBWidth );
    srcMaxLines = agxScratchSize >> srcBWidthShift;
    pixBWidth = PixmapBytePad( width, tile->drawable.depth );

    if( FALSE && nBox == 1 ) {
       int            xrot, yrot;
       modulus( pBox->x1-pox, width, xrot );
       modulus( pBox->y1-poy, height, yrot );
       w = pBox->x2 - pBox->x1;
       h = pBox->y2 - pBox->y1;
       if( (w == 0) || (h == 0) ) 
          return; 
       if( alu == GXcopy && (planemask&agxPixMask) == agxPixMask
           && (width - xrot) >= w && (height - yrot) >= h ) {
          /* copy direct */
          GE_WAIT_IDLE();
          agxPartMemToVid( AGX_PIXEL_ADJUST(pBox->x1) + pBox->y1*screenStride,
                           screenStride,
                           (char *) tile->devPrivate.ptr 
                             + AGX_PIXEL_ADJUST( xrot ) + yrot * pixBWidth,
                           pixBWidth,
                           AGX_PIXEL_ADJUST( w ), h );
          return;
       }
       if( w < width || h < height ) {
          /* box is smaller than pattern -- don't load entire pattern  */
          agxImageFillNoMem( pBox->x1, pBox->y1,
                             w, h,
                             tile->devPrivate.ptr, pixBWidth,
                             width, height,
                             pox, poy,
                             alu, planemask );
          return;
       }
    } 

    if( srcMaxLines < height ) {
       /* tile doesn't fit in scratchpad */
       if( alu == GXcopy && (planemask&agxPixMask) == agxPixMask ) {
          /* copy direct if easy */
          for (; nBox; nBox--, pBox++) {
             h = pBox->y2 - pBox->y1;
             w = pBox->x2 - pBox->x1;
             if( w > 0 && h > 0 ) {
                int            xrot, yrot;
                modulus( pBox->x1-pox, width, xrot );
                modulus( pBox->y1-poy, height, yrot );
                if( (width - xrot) >= w && (height - yrot) >= h ) {
                   GE_WAIT_IDLE();
                   agxPartMemToVid( 
                      AGX_PIXEL_ADJUST(pBox->x1) + pBox->y1*screenStride,
                      screenStride,
                      (char *) tile->devPrivate.ptr
                         + AGX_PIXEL_ADJUST( xrot ) + yrot * pixBWidth,
                      pixBWidth,
                      AGX_PIXEL_ADJUST( w ), h 
                   );
                }
                else {
                   /* in future do two direct copies */
                   agxImageFillNoMem( pBox->x1, pBox->y1,
                                      w, h,
                                      tile->devPrivate.ptr, pixBWidth,
                                      width, height,
                                      pox, poy,
                                      alu, planemask );
                }
             }
          }
       }
       else {
          for (; nBox; nBox--, pBox++) {
             h = pBox->y2 - pBox->y1;
             w = pBox->x2 - pBox->x1;
             if ((w > 0) && (h > 0)) {
                agxImageFillNoMem( pBox->x1, pBox->y1,
                                   w, h,
                                   tile->devPrivate.ptr, pixBWidth,
                                   width, height,
                                   pox, poy,
                                   alu, planemask );
             }
          }
       }
    }
    else {
       /* tile fits in scratchpad */
       /*
        * Future - should try determing extent of boxes 
        *          and limit how much tile is loaded or use direct copies
        *          if alu == GXCopy and there is no tiling 
        */
       MAP_INIT( GE_MS_MAP_B,
                 agxVideoMapFormat,
                 agxMemBase + agxScratchOffset,
                 srcPWidth-1,
                 height-1,
                 FALSE, FALSE, FALSE );

       GE_WAIT_IDLE();

       MAP_SET_DST( GE_MS_MAP_A );
       MAP_SET_SRC( GE_MS_MAP_B );
       GE_SET_MAP( GE_MS_MAP_B );
       GE_OUT_B( GE_FRGD_MIX, alu );
       GE_OUT_D( GE_PIXEL_BIT_MASK, planemask );
       GE_OUT_W( GE_PIXEL_OP,
                 GE_OP_PAT_FRGD
                 | GE_OP_MASK_DISABLED
                 | GE_OP_INC_X
                 | GE_OP_INC_Y         );
 
       agxMemToVid( agxScratchOffset, srcBWidth,
                    tile->devPrivate.ptr, pixBWidth, height );

       if( srcPWidth == width ) {
          /* pow2 -- bitblt'r can do tiling */
          unsigned int widthMask = width - 1;
          for (; nBox; nBox--, pBox++) {
             h = pBox->y2 - pBox->y1 - 1;
             w = pBox->x2 - pBox->x1 - 1;
   
             if ((w >= 0) && (h >= 0)) {
                register unsigned int srcCoOrd =
                       ((pBox->y1 - poy) % height) << 16
                       | (pBox->x1 - pox) & widthMask;
                register unsigned int dstCoOrd = pBox->y1 << 16 | pBox->x1;
                register unsigned int opDim = h << 16 | w;
   
                GE_WAIT_IDLE();
                GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
                GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                GE_OUT_D( GE_OP_DIM_WIDTH, opDim );
                GE_START_CMDW( GE_OPW_BITBLT
                               | GE_OPW_FRGD_SRC_MAP
                               | GE_OPW_SRC_MAP_B
                               | GE_OPW_DEST_MAP_A   );
             }
          }
          GE_WAIT_IDLE_EXIT();
       }
       else { 
          /* tiling can't be done directly */
      
          for (; nBox; nBox--, pBox++) {
             int            xrot, yrot;
             modulus( pBox->x1-pox, width, xrot );
             modulus( pBox->y1-poy, height, yrot );
      
             h = pBox->y2 - pBox->y1;
             w = pBox->x2 - pBox->x1;
             if ((w > 0) && (h > 0)) {
                agxImageTileScrPad( pBox->x1, pBox->y1,
                                    w, h,
                                    srcPWidth,
                                    width, height,
                                    xrot, yrot );
             }
          }
       }
    }
}

 
void
agxFSpansTile( pDrawable, nSpans, ppts, pwidth,
               tile, pox, poy,
               alu, planemask )
    DrawablePtr     pDrawable;
    int             nSpans;
    DDXPointPtr     ppts; 
    int            *pwidth;
    PixmapPtr       tile;
    int             pox;
    int             poy;
    unsigned int    alu;
    unsigned int    planemask;
{
    unsigned int   width  = tile->drawable.width;
    unsigned int   height = tile->drawable.height;
    unsigned int   pixBWidth;
    unsigned int   srcX, srcY;
    unsigned int   srcPWidth;
    unsigned int   srcBWidth;
    int            srcBWidthShift;
    unsigned int   srcMaxLines;
    unsigned int   firstLine;
    unsigned int   lastLine;
    int            xOff = 0;
    DDXPointPtr    pPts = ppts;
    int            *pWidth = pwidth;
    int            n = nSpans;
    int            minX;
    int            maxX;
    Bool           wrap = FALSE;
    int            minLine;
    int            maxLine;
    unsigned int   newWidth;
    unsigned int   firstBWidth;
    unsigned int   secondBWidth;
    char           *pixStart;
    char           *pix2Start;

    modulus(pPts->x-pox,width,minX);
    modulus(pPts->x+*pWidth-pox-1,width,maxX);
    if( maxX < minX )
       wrap = TRUE;
    modulus(pPts->y-poy,height,minLine);
    maxLine = minLine;
    /*
     * Try to load as little of the pixmap as possible.
     * We're not realigning to the pox origins so that
     * the drawing part of the routine will also work with
     * cached pixmaps in the future.
     */
    while( n-- > 0 ) {
       int tmp;
       modulus(pPts->x-pox,width,tmp);
       if( tmp < minX ) minX = tmp;
       tmp += *pWidth - 1 ;
       if( tmp >= width ) {
          modulus(tmp,width,tmp);
          if( !wrap ) {
             wrap = TRUE;
             maxX = 0;
          } 
       }
       if( wrap && tmp > maxX && tmp < minX ) {
          maxX = tmp;
       }
       else if( tmp > maxX ) {
          maxX = tmp;
       }
       modulus(pPts->y-poy,height,tmp);
       if( tmp < minLine ) minLine = tmp;
       if( tmp > maxLine ) maxLine = tmp;
       pWidth++;
       pPts++;
    }

    pPts = ppts;
    pWidth = pwidth;
    n = nSpans;

    if( wrap && maxX > minX ) {
       if( minX > 0 ) {
          maxX = minX - 1;
       }
       else {
          maxX = width - 1;
          wrap = FALSE;
       }
    }

    if( wrap ) {
       int firstWidth = width - minX;
       newWidth = firstWidth + maxX + 1;
       firstBWidth = AGX_PIXEL_ADJUST( firstWidth );
       secondBWidth = AGX_PIXEL_ADJUST( maxX + 1 );
    }
    else {
       newWidth = maxX - minX + 1;
       firstBWidth = PixmapBytePad( newWidth, tile->drawable.depth );
       secondBWidth = 0;
    }
    if( newWidth >= width ) {
       newWidth = width;
       firstBWidth = PixmapBytePad( newWidth, tile->drawable.depth );
       secondBWidth = 0;
       minX = 0;
       maxX = width - 1;
       wrap = FALSE;
    }

    pox += minX; 
    xOff = AGX_PIXEL_ADJUST( minX );
    pixStart = (char *)tile->devPrivate.ptr + xOff;
    pix2Start = (char *)tile->devPrivate.ptr;
    srcBWidthShift = agxBytePadScratchMapPow2( width, BytesPerPixelShift );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = AGX_TO_PIXEL( srcBWidth ); 
    srcMaxLines = agxScratchSize >> srcBWidthShift;
    pixBWidth = PixmapBytePad( width, tile->drawable.depth );

    firstLine = minLine;
    if( (maxLine - minLine + 1) > srcMaxLines ) {
       lastLine = firstLine + srcMaxLines - 1;
    }
    else {
       lastLine = maxLine ;
    }

    MAP_INIT( GE_MS_MAP_B,
              agxVideoMapFormat,
              agxMemBase + agxScratchOffset,
              srcPWidth-1,
              height-1,
              FALSE, FALSE, FALSE );

    GE_WAIT_IDLE();

    agxPartMemToVid( agxScratchOffset, srcBWidth,
                     pixStart + (firstLine * pixBWidth), pixBWidth,
                     firstBWidth, lastLine - firstLine + 1 );
    if( wrap ) {
       agxPartMemToVid( agxScratchOffset+firstBWidth, srcBWidth,
                        pix2Start + (firstLine * pixBWidth), pixBWidth,
                        secondBWidth, lastLine - firstLine + 1 );
    }
 
    MAP_SET_DST( GE_MS_MAP_A );
    MAP_SET_SRC( GE_MS_MAP_B );
    GE_SET_MAP( GE_MS_MAP_B );
 
    GE_OUT_B(GE_FRGD_MIX, alu );
    GE_OUT_D(GE_PIXEL_BIT_MASK, planemask);

    GE_OUT_W( GE_PIXEL_OP,
              GE_OP_PAT_FRGD 
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );


    while( nSpans-- ) {
       int firstWidth = *pwidth;   
       int lastWidth = 0; 
       int numMids = 0;
       int nextX = ppts->x;

       if (firstWidth > 0) {
          modulus(ppts->x-pox,width,srcX);
          modulus(ppts->y-poy,height,srcY);
          /* 
           * If the GE can't handle the wrap (width ! a pow of 2)
           * we'll need to handle the horz tiling ourselves.
           */
          if( srcPWidth != width && (srcX + firstWidth) > width ) {
             /* we have to handle the wrap */
             int remains; 
             firstWidth = width - srcX;
             remains = *pwidth - firstWidth;
             numMids = remains / width;
             lastWidth = remains % width;
          }

          if( srcY > lastLine || srcY < firstLine ) {
             firstLine = srcY;
             lastLine = maxLine;
             if( lastLine - firstLine >= srcMaxLines ) {
                lastLine = firstLine + srcMaxLines - 1;
             }
             GE_WAIT_IDLE();

             agxPartMemToVid( agxScratchOffset, srcBWidth,
                              pixStart + (firstLine * pixBWidth), pixBWidth,
                              firstBWidth, lastLine - firstLine + 1 );
             if( wrap ) {
                agxPartMemToVid( agxScratchOffset+firstBWidth, srcBWidth,
                                 pix2Start + (firstLine * pixBWidth),
                                    pixBWidth,
                                 secondBWidth, lastLine - firstLine + 1 );
             }
          }
          srcY -= firstLine;

          {
             register unsigned int srcCoOrd = srcY << 16 | srcX;
             register unsigned int dstCoOrd = ppts->y << 16 | nextX;

             GE_WAIT_IDLE();
             GE_OUT_D( GE_SRC_MAP_X, srcCoOrd ); 
             GE_OUT_D( GE_DEST_MAP_X, dstCoOrd ); 
             GE_OUT_D( GE_OP_DIM_WIDTH, firstWidth - 1 );
             GE_START_CMDW( GE_OPW_BITBLT
                            | GE_OPW_FRGD_SRC_MAP
                            | GE_OPW_SRC_MAP_B
                            | GE_OPW_DEST_MAP_A   );
   
             srcCoOrd &= 0xFFFF0000;   
             dstCoOrd += firstWidth;
             while( numMids-- > 0 ) { 
                GE_WAIT_IDLE();
                GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
                GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                GE_OUT_D( GE_OP_DIM_WIDTH, width-1 );
                GE_START_CMDW( GE_OPW_BITBLT
                               | GE_OPW_FRGD_SRC_MAP
                               | GE_OPW_SRC_MAP_B
                               | GE_OPW_DEST_MAP_A   );
                dstCoOrd += width;
             }
   
             if( lastWidth > 0 ) {
                GE_WAIT_IDLE();
                GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
                GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                GE_OUT_D( GE_OP_DIM_WIDTH, lastWidth - 1 );
                GE_START_CMDW( GE_OPW_BITBLT
                               | GE_OPW_FRGD_SRC_MAP
                               | GE_OPW_SRC_MAP_B
                               | GE_OPW_DEST_MAP_A   );
             }
          }
       }
       ppts++;
       pwidth++;
    }
    GE_WAIT_IDLE_EXIT();
}
    
 
void
#if NeedFunctionPrototypes
agxFSpansStipple(
    DrawablePtr		pDrawable,
    int			nSpans,
    DDXPointPtr		ppts,
    int			*pwidth,
    PixmapPtr		stipple,
    int			pox,
    int			poy,
    Pixel		fgpixel,
    Pixel		bgpixel,
    short		fgAlu,
    short		bgAlu,
    unsigned long	planemask)
#else
agxFSpansStipple( pDrawable, nSpans, ppts, pwidth, stipple, pox, poy,
                  fgpixel, bgpixel, fgAlu, bgAlu, planemask )
    DrawablePtr		pDrawable;
    int			nSpans;
    DDXPointPtr		ppts; 
    int			*pwidth;
    PixmapPtr		stipple;
    int			pox;
    int			poy;
    Pixel		fgpixel;
    Pixel		bgpixel;
    short		fgAlu;
    short		bgAlu;
    unsigned long	planemask;
#endif
{
    unsigned int   width  = stipple->drawable.width;
    unsigned int   height = stipple->drawable.height;
    unsigned int   pixBWidth;
    unsigned int   srcX, srcY;
    unsigned int   srcPWidth;
    unsigned int   srcBWidth;
    int            srcBWidthShift;
    unsigned int   srcMaxLines;
    unsigned int   firstLine;
    unsigned int   lastLine;
    DDXPointPtr    pPts = ppts; 
    int            *pWidth = pwidth;
    int            n = nSpans;
    int            minX;
    int            maxX;
    int            minLine;
    int            maxLine;
    unsigned int   newWidth;
    unsigned int   newBWidth;
    char           *pixStart;
    
    modulus(pPts->x-pox,width,minX);
    modulus(pPts->x+*pWidth-pox-1,width,maxX);
    modulus(pPts->y-poy,height,minLine);
    maxLine = minLine;
    /*
     * Try to load as little of the bitmap as possible,
     * but if we wrap it's not worth while as we have
     * to realign bits. Fortunately bitmaps are generally
     * small.
     */
    while( n-- > 0 ) {
       int tmp;
       modulus(pPts->x-pox,width,tmp);
       if( tmp < minX ) minX = tmp;
       tmp += *pWidth;
       if( tmp > width ) { 
          minX = 0;
          maxX = width - 1;
       }
       else if( tmp > maxX ) {
          maxX = tmp - 1; 
       }
       modulus(pPts->y-poy,height,tmp);
       if( tmp < minLine ) minLine = tmp;
       if( tmp > maxLine ) maxLine = tmp;
       pWidth++;
       pPts++;
    }
    pPts = ppts; 
    pWidth = pwidth;
    n = nSpans;

    minX &= 0xFFF8;
    newWidth = maxX - minX + 1;
    newBWidth = BitmapBytePad( newWidth );

    pox += minX;
    pixStart = (char *)stipple->devPrivate.ptr + (minX >> 3);
    srcBWidthShift = agxBytePadScratchMapPow2( newWidth, -3 );
    srcBWidth = 1 << srcBWidthShift;
    srcPWidth = srcBWidth << 3;
    srcMaxLines = agxScratchSize >> srcBWidthShift;
    pixBWidth = BitmapBytePad( width );

    firstLine = minLine; 
    if( (maxLine - minLine + 1) > srcMaxLines ) {
       lastLine = firstLine + srcMaxLines - 1;
    }
    else {
       lastLine = maxLine ;
    }

    MAP_INIT( GE_MS_MAP_B,
              GE_MF_1BPP | GE_MF_INTEL_FORMAT,
              agxMemBase + agxScratchOffset,
              srcPWidth-1,
              height-1,
              FALSE, FALSE, FALSE );

    GE_WAIT_IDLE();

    agxPartMemToVid( agxScratchOffset, srcBWidth,
                     pixStart + (firstLine * pixBWidth), pixBWidth, 
                     newBWidth, lastLine - firstLine + 1 );
 
    MAP_SET_SRC_AND_DST( GE_MS_MAP_A );
    GE_SET_MAP( GE_MS_MAP_B );
 
    GE_OUT_W(GE_FRGD_MIX, bgAlu << 8 | fgAlu);  /* both fg & bg */
    GE_OUT_D(GE_FRGD_CLR, fgpixel );
    GE_OUT_D(GE_BKGD_CLR, bgpixel );
    GE_OUT_D(GE_PIXEL_BIT_MASK, planemask);

    GE_OUT_W( GE_PIXEL_OP,
              GE_OP_PAT_MAP_B
              | GE_OP_MASK_DISABLED
              | GE_OP_INC_X
              | GE_OP_INC_Y         );

    while( nSpans-- ) {
       int firstWidth = *pwidth;   
       int lastWidth = 0; 
       int numMids = 0;
       int nextX = ppts->x;

       if (firstWidth > 0) {
          modulus(ppts->x-pox,width,srcX);
          modulus(ppts->y-poy,height,srcY);
          /* 
           * If the GE can't handle the wrap (width ! a pow of 2)
           * we'll need to handle the horz tiling ourselves.
           */
          if( srcPWidth != width && (srcX + firstWidth) > width ) {
             /* we have to handle the wrap */
             int remains; 
             firstWidth = width - srcX;
             remains = *pwidth - firstWidth;
             numMids = remains / width;
             lastWidth = remains % width;
          }
   
          if( srcY > lastLine || srcY < firstLine ) {
             firstLine = srcY;
             lastLine = maxLine;
             if( lastLine - firstLine >= srcMaxLines ) {
                lastLine = firstLine + srcMaxLines - 1;
             }
             GE_WAIT_IDLE();

             agxPartMemToVid( agxScratchOffset, srcBWidth,
                              pixStart + (firstLine * pixBWidth), pixBWidth, 
                              newBWidth, lastLine - firstLine + 1 );
          } 
          srcY -= firstLine;

          {
             register unsigned int srcCoOrd = srcY << 16 | srcX;
             register unsigned int dstCoOrd = ppts->y << 16 | nextX;

             GE_WAIT_IDLE();
             GE_OUT_D( GE_PAT_MAP_X,  srcCoOrd );
             GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
             GE_OUT_D( GE_OP_DIM_WIDTH, firstWidth - 1 );
             GE_START_CMDW( GE_OPW_BITBLT
                            | GE_OPW_FRGD_SRC_CLR
                            | GE_OPW_BKGD_SRC_CLR
                            | GE_OPW_DEST_MAP_A   );

             srcCoOrd &= 0xFFFF0000;   
             dstCoOrd += firstWidth;
             while( numMids-- > 0 ) {
                GE_WAIT_IDLE();
                GE_OUT_D( GE_PAT_MAP_X,  srcCoOrd );
                GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                GE_OUT_D( GE_OP_DIM_WIDTH, width-1 );
                GE_START_CMDW( GE_OPW_BITBLT
                               | GE_OPW_FRGD_SRC_CLR
                               | GE_OPW_BKGD_SRC_CLR
                               | GE_OPW_DEST_MAP_A   );
                dstCoOrd += width;
             }
   
             if( lastWidth > 0 ) {
                GE_WAIT_IDLE();
                GE_OUT_D( GE_PAT_MAP_X,  srcCoOrd );
                GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
                GE_OUT_D( GE_OP_DIM_WIDTH, lastWidth - 1 );
                GE_START_CMDW( GE_OPW_BITBLT
                               | GE_OPW_FRGD_SRC_CLR
                               | GE_OPW_BKGD_SRC_CLR
                               | GE_OPW_DEST_MAP_A   );
             }
          }
       }
       ppts++;
       pwidth++;
    }
    GE_WAIT_IDLE_EXIT();
}
