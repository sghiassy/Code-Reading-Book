/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3im.c,v 3.12.2.4 1997/05/28 13:12:51 dawes Exp $ */
/*
 * Copyright 1992 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Kevin E. Martin not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Kevin E. Martin makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 *
 * KEVIN E. MARTIN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */

/*
 * Modified by Amancio Hasty and extensively by Jon Tombs & Phil Richards.
 *
 */
/* $XConsortium: s3im.c /main/10 1996/10/28 05:22:54 kaleb $ */


#include "misc.h"
#include "xf86.h"
#include "s3v.h"
#include "s3im.h"
#include "scrnintstr.h"
#include "cfbmskbits.h"
#include "xf86_HWlib.h"
#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#define	reorder(a,b)	b = \
	(a & 0x80) >> 7 | \
	(a & 0x40) >> 5 | \
	(a & 0x20) >> 3 | \
	(a & 0x10) >> 1 | \
	(a & 0x08) << 1 | \
	(a & 0x04) << 3 | \
	(a & 0x02) << 5 | \
	(a & 0x01) << 7;

extern unsigned char s3SwapBits[256];
extern int s3ScreenMode;
extern int   s3DisplayWidth;
extern int   s3BankSize;
extern unsigned char s3Port51;
extern unsigned char s3Port40;
extern unsigned char s3Port54;
extern int xf86Verbose;
extern Bool s3LinearAperture;

static void s3ImageRead (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static void s3ImageWrite (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, unsigned long
#endif
);
static void s3ImageFill (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, int, unsigned long
#endif
);
static void s3ImageReadNoMem (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
void s3ImageWriteNoMem (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, unsigned long
#endif
);
static void s3ImageFillNoMem (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, int, unsigned long
#endif
);
extern ScrnInfoRec s3InfoRec;
int s3Bpp;
int s3BppDisplayWidth;
Pixel s3BppPMask;
int realS3Bpp;

void
s3ImageInit ()
{
   int   i;
   static Bool reEntry = FALSE;

   if (reEntry) {
      return;
   }

   reEntry = TRUE;
   s3Bpp = s3InfoRec.bitsPerPixel / 8;
   realS3Bpp = s3Bpp;
   if (s3Bpp == 4) realS3Bpp = 3;

   s3BppDisplayWidth = realS3Bpp * s3DisplayWidth;
   s3BppPMask = (1UL << s3InfoRec.depth) - 1;
   if (s3Bpp == 4) s3BppPMask = 0x00ffFFff;

   for (i = 0; i < 256; i++) {
      reorder (i, s3SwapBits[i]);
   }

   s3ImageReadFunc  = s3ImageRead;
   s3ImageWriteFunc = s3ImageWrite;
   s3ImageFillFunc  = s3ImageFill;

   if (xf86Verbose)
      ErrorF ("%s %s: Using a banksize of %dk, line width of %d\n",
           XCONFIG_PROBED, s3InfoRec.name, s3BankSize/1024, s3DisplayWidth);
}

/* fast ImageWrite(), ImageRead(), and ImageFill() routines */
/* there are two cases; (i) when the bank switch can occur in the */
/* middle of raster line, and (ii) when it is guaranteed not possible. */
/* In theory, s3InfoRec.virtualX should contain the number of bytes */
/* on the raster line; however, this is not necessarily true, and for */
/* many situations, the S3 card will always have 1024. */
/* Phil Richards <pgr@prg.ox.ac.uk> */
/* 26th November 1992 */
/* Bug fixed by Jon Tombs */
/* 30/7/94 began 16,32 bit support */

#undef outw
#undef outw32
#define outw(p,v)     do { /* noting*/ } while (0)
#define outw32(p,v)   do { /* noting*/ } while (0)

static void 
copy32to24(char *d, char *s, int n)
{  /* depends on byte sex, LE only !! */
  CARD32 *s32, *d32;

  for ( ; n>0 && ((long)(d) & 3); n--) {
    if ((long)(d) & 1) {
      *d++ = *s++;
      *((CARD16 *)d++) = ldw_u((CARD16*)(s++));
      d++; s++;
      s++;
    }
    else {
      *((CARD16 *)d++) = ldw_u((CARD16*)(s++));
      d++; s++;
      *d++ = *s++;
      s++;
    }
  }

  s32 = (CARD32*)s;
  d32 = (CARD32*)d;
  for ( ;n >= 4; n -= 4) {
    CARD32 p1,p2,p3,p4;
    p1 = ldl_u(s32++);
    p2 = ldl_u(s32++);
    p3 = ldl_u(s32++);
    p4 = ldl_u(s32++);
    *d32++ =  (p1 & 0xffffff)        | (p2 << 24);
    *d32++ = ((p2 & 0xffffff) >>  8) | (p3 << 16);
    *d32++ = ((p3 & 0xffffff) >> 16) | (p4 <<  8);
  }
  s = (char*)s32;
  d = (char*)d32;

  for ( ;n>0; n--) {
    if ((long)(d) & 1) {
      *d++ = *s++;
      *((CARD16 *)(d++)) = ldw_u((CARD16*)(s++));
      d++; s++;
      s++;
    }
    else {
      *((CARD16 *)(d++)) = ldw_u((CARD16*)(s++));
      d++; s++;
      *d++ = *s++;
      s++;
    }
  }
}

static void
copy24to32(char *d, char *s, int n)
{  /* depends on byte sex, LE only !! */
  CARD32 *s32, *d32;

  for ( ; n>0 && ((long)(s) & 3); n--) {
    if ((long)(d) & 1) {
      *d++ = *s++;
      *((CARD16 *)(d++)) = ldw_u((CARD16*)(s++));
      d++; s++;
      d++;
    }
    else { 
      *((CARD16 *)(d++)) = ldw_u((CARD16*)(s++));
      d++; s++;
      *d++ = *s++;
      d++;
    }
  }

  s32 = (CARD32*)s;
  d32 = (CARD32*)d;
  for (;n >= 4; n -= 4) {
    CARD32 p1,p2,p3;
    p1 = ldl_u(s32++);
    p2 = ldl_u(s32++);
    p3 = ldl_u(s32++);
    *d32++ =    p1 & 0xffffff;
    *d32++ =  ((p1 >> 24) | (p2 <<  8)) & 0xffffff;
    *d32++ =  ((p2 >> 16) | (p3 << 16)) & 0xffffff;
    *d32++ =    p3 >>  8;
  }
  s = (char*)s32;
  d = (char*)d32;

  for ( ;n>0; n--) {
    if ((long)(d) & 1) {
      *d++ = *s++;
      *((CARD16 *)(d++)) = ldw_u((CARD16*)(s++));
      d++; s++;
      d++;
    }
    else {
      *((CARD16 *)(d++)) = ldw_u((CARD16*)(s++));
      d++; s++;
      *d++ = *s++;
      d++;
    }
  }
}

static void
#if NeedFunctionPrototypes
s3ImageWrite (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   px,
     int   py,
     int   alu,
     unsigned long planemask)
#else
s3ImageWrite (x, y, w, h, psrc, pwidth, px, py, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   px;
     int   py;
     int   alu;
     unsigned long planemask;
#endif
{
   int   j, offset;
   char *videobuffer;
   int k;

   if (alu == ROP_D)
      return;

   if ((alu != ROP_S) || ((planemask & s3BppPMask) != s3BppPMask)) {
      s3ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask);
      return;
   }

   if (w == 0 || h == 0)
      return;

   videobuffer = (char *) s3VideoMem;

   psrc += pwidth * py + px * s3Bpp;
   offset = (y * s3BppDisplayWidth) + x * realS3Bpp /*s3Bpp*/;
   videobuffer += offset;

   /* set 'w' to either number of bytes per row, or number of pixels
    * per row, to copy; depending on pixel size.
   */
   if (realS3Bpp < 3) w *= s3Bpp;

   BLOCK_CURSOR;
   WaitIdleEmpty();

   for (j = 0; j < h; j++, psrc += pwidth) {

     if (realS3Bpp < 3)
       MemToBus (videobuffer, psrc, w);
     else
       copy32to24(videobuffer, psrc, w);

     videobuffer += s3BppDisplayWidth;
   }

   UNBLOCK_CURSOR;
}

static void
#if NeedFunctionPrototypes
s3ImageRead (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   px,
     int   py,
     unsigned long planemask)
#else
s3ImageRead (x, y, w, h, psrc, pwidth, px, py, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   px;
     int   py;
     unsigned long planemask;
#endif
{
   int   j;
   int   offset;
   char *videobuffer;
   int k;

   if (w == 0 || h == 0)
      return;

   if ((planemask & s3BppPMask) != s3BppPMask) {
      s3ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask);
      return;
   }

   videobuffer = (char *) s3VideoMem;

   psrc += pwidth * py + px * s3Bpp;
   offset = (y * s3BppDisplayWidth) + x * realS3Bpp /*s3Bpp*/;
   videobuffer += offset;

   if (realS3Bpp < 3) w *= s3Bpp;

   BLOCK_CURSOR;
   WaitIdleEmpty ();

   for (j = 0; j < h; j++, psrc += pwidth) {

     if (realS3Bpp < 3)
       BusToMem (psrc, videobuffer, w);
     else
       copy24to32(psrc, videobuffer, w);

     videobuffer += s3BppDisplayWidth;
   }

   UNBLOCK_CURSOR;
}

static void
#if NeedFunctionPrototypes
s3ImageFill (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   pw,
     int   ph,
     int   pox,
     int   poy,
     int   alu,
     unsigned long planemask)
#else
s3ImageFill (x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   pw;
     int   ph;
     int   pox;
     int   poy;
     int   alu;
     unsigned long planemask;
#endif
{
   int   j;
   char *pline;
   int   ypix, xpix, offset0;
   int   cxpix;
   char *videobuffer, *sp;
   int k;

   if (w == 0 || h == 0) return;
   if (alu == ROP_D) return;

   if ((alu != ROP_S) || ((planemask & s3BppPMask) != s3BppPMask)) {
     s3ImageFillNoMem(x, y, w, h, psrc, pwidth,
                    pw, ph, pox, poy, alu, planemask);
     return;
   }

   videobuffer = (char *) s3VideoMem;

   if (s3Bpp <= 3) {
     w  *= s3Bpp;
     pw *= s3Bpp;
     modulus ((x - pox) * s3Bpp, pw, xpix);
   }
   else modulus ((x - pox), pw, xpix);
   cxpix = pw - xpix ;

   modulus (y - poy, ph, ypix);
   pline = psrc + pwidth * ypix;

   offset0 = (y * s3BppDisplayWidth) + x * realS3Bpp /*s3Bpp*/;
   videobuffer += offset0;

   BLOCK_CURSOR;
   WaitIdleEmpty();

   for (j = 0; j < h; j++ /*, offset0 += s3BppDisplayWidth*/) {
      if (w <= cxpix) {
	if (realS3Bpp < 3)
	  MemToBus (videobuffer /*&videobuffer[offset0]*/, pline + xpix, w);
	else
	  copy32to24(videobuffer /*+ offset0*/, pline + xpix * s3Bpp, w);
      } else {
	 int   width, offset;

	 if (realS3Bpp < 3)
	   MemToBus (videobuffer/*&videobuffer[offset0]*/, pline + xpix, cxpix);
	 else
	   copy32to24(videobuffer /*+ offset0*/, pline + xpix * s3Bpp, cxpix);
/*
	 offset = offset0 + cxpix;
*/
	 if (realS3Bpp < 3) sp = videobuffer + cxpix;
	 else sp = videobuffer + cxpix * realS3Bpp;

	 for (width = w - cxpix; width >= pw; width -= pw) {
	   if (realS3Bpp < 3) {
	     MemToBus (sp /*&videobuffer[offset]*/, pline, pw);
	     sp += pw /*offset += pw*/;
	   }
	   else {
	     copy32to24(sp, pline , pw);
	     sp += pw * realS3Bpp;
	   }
	 }

         /* at this point: 0 <= width < pw */
	 if (width > 0) {
	   if (realS3Bpp < 3)
	     MemToBus (sp /*&videobuffer[offset]*/, pline, width);
	   else
	     copy32to24(sp, pline , width);
	 }
      }

      if ((++ypix) == ph) {
	 ypix = 0;
	 pline = psrc;
      } else
	 pline += pwidth;

      videobuffer += s3BppDisplayWidth;
   }

   UNBLOCK_CURSOR;
}

void
#if NeedFunctionPrototypes
s3ImageWriteNoMem (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   px,
     int   py,
     int   alu,
     unsigned long planemask)
#else
s3ImageWriteNoMem (x, y, w, h, psrc, pwidth, px, py, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   px;
     int   py;
     int   alu;
     unsigned long planemask;
#endif
{
   int   i, origwidth;

   if (alu == ROP_D)
      return;

   if (w == 0 || h == 0)
      return;

   BLOCK_CURSOR;
   ;outw (FRGD_MIX, FSS_PCDATA | alu);
   ;outw32 (WRT_MASK, planemask); /* SET_WRT_MASK(planemask); */
   ;outw (MULTIFUNC_CNTL, PIX_CNTL | 0);

   origwidth = w;
   w = s3CheckLSPN(w, 1);
   if (w != origwidth) {
      WaitQueue(6);
      SETB_CLIP_L_R(x, x + origwidth-1);
   }
   else {
      WaitQueue(4);
   }

   SETB_RWIDTH_HEIGHT(w - 1, h);
   SETB_RDEST_XY(x, y);
   WaitIdle();
   SETB_CMD_SET(s3_gcmd | CMD_BITBLT | INC_X | INC_Y | alu
		| CMD_ITA_DWORD | MIX_CPUDATA);

   w *= realS3Bpp /*s3Bpp*/;
   psrc += pwidth * py;

   if (alu != ROP_0 && alu != ROP_1 && alu != ROP_D && alu != ROP_Dn)
      for ( ; h--; ) {
	 CARD32 *psrcs = (CARD32 *)(psrc + px * s3Bpp);
	 for (i = 0; i < w; i+=4) {
	    *IMG_TRANS = ldl_u(psrcs++);
	    write_mem_barrier();
	 }
	 psrc += pwidth;
      }
   WaitIdle();
   if (w != origwidth) {
      SETB_CLIP_L_R(0, s3DisplayWidth-1);
   }
   UNBLOCK_CURSOR;
}


static void
#if NeedFunctionPrototypes
s3ImageReadNoMem (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   px,
     int   py,
     unsigned long planemask)
#else
s3ImageReadNoMem (x, y, w, h, psrc, pwidth, px, py, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   px;
     int   py;
     unsigned long planemask;
#endif
{
   if (1) ErrorF("s3ImageReadNoMem called, please report\n");
#ifdef very_old_S3
   int   i, j;

   if (w == 0 || h == 0)
      return;

   BLOCK_CURSOR;
   WaitIdleEmpty ();
   WaitQueue(7);
   outw (MULTIFUNC_CNTL, PIX_CNTL);
   outw (FRGD_MIX, FSS_PCDATA | ROP_S);
   outw (CUR_X, x);
   outw (CUR_Y, y);
   outw (MAJ_AXIS_PCNT, w - 1);
   outw (MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h - 1));
   outw (CMD, CMD_RECT  | BYTSEQ | _16BIT | INC_Y | INC_X | DRAW |
	  PCDATA);

   WaitQueue16_32(1,2);

   outw32(RD_MASK, planemask); /* SET_RD_MASK(planemask); */

   WaitQueue(8);

   w *= s3Bpp;
   psrc += pwidth * py;

   for (j = 0; j < h; j++) {
      short *psrcs = (short *)&psrc[px*s3Bpp];
      for (i = 0; i < w; ) {
	 if (s3InfoRec.bitsPerPixel == 32) {
	    *((long*)(psrcs)) = inl(PIX_TRANS);
	    psrcs += 2;
	    i += 4;
	 } else {
	    *psrcs++ = inw(PIX_TRANS);
	    i += 2;
	 }
      }
      psrc += pwidth;
   }
   UNBLOCK_CURSOR;
#endif
}

static void
#if NeedFunctionPrototypes
s3ImageFillNoMem (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   pw,
     int   ph,
     int   pox,
     int   poy,
     int   alu,
     unsigned long planemask)
#else
s3ImageFillNoMem (x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   pw;
     int   ph;
     int   pox;
     int   poy;
     int   alu;
     unsigned long planemask;
#endif
{
   int   i, j, origwidth;
   unsigned char *pline;
   int   mod;

   if (alu == ROP_D)
      return;

   if (w == 0 || h == 0)
      return;

   BLOCK_CURSOR;
   ;outw (FRGD_MIX, FSS_PCDATA | alu);
   ;outw32 (WRT_MASK, planemask); /* SET_WRT_MASK(planemask); */

   origwidth = w;
   w = s3CheckLSPN(w, 1);
   if (w != origwidth) {
      WaitQueue(6);
      SETB_CLIP_L_R(x, x + origwidth-1);
   }
   else {
      WaitQueue(4);
   }

   SETB_RWIDTH_HEIGHT(w - 1, h);
   SETB_RDEST_XY(x, y);
   WaitIdle();
   SETB_CMD_SET(s3_gcmd | CMD_BITBLT | INC_X | INC_Y | alu
		| CMD_ITA_DWORD | MIX_CPUDATA);


   if (alu != ROP_0 && alu != ROP_1 && alu != ROP_D && alu != ROP_Dn)
   for (j = 0; j < h; j++) {
      CARD32 wrapped1=0, *pnext1=NULL;
      CARD32 wrapped2=0, *pnext2=NULL;
      CARD32 wrapped3=0, *pnext3=NULL;
      CARD32 *pend;
      CARD32 *plines;

      modulus (y + j - poy, ph, mod);
      pline = (unsigned char *)(psrc + pwidth * mod);
      pend = (CARD32 *)(pline + pw * s3Bpp);

      wrapped1 = (ldl_u((CARD32 *)pline) << 8) | (pline[pw - 1] << 0);
      pnext1 = (CARD32 *)(pline + 3);
      wrapped2 = (ldw_u((CARD16 *)pline) << 16) | (ldw_u((CARD16*)(pline + pw - 2)) << 0);
      pnext2 = (CARD32 *)(pline + 2);
      wrapped3 = (pline[0] << 24) | ((ldl_u((CARD32*)(pline + pw - 3)) & 0xffffff) << 0);
      pnext3 = (CARD32 *)(pline + 1);

      modulus (x - pox, pw, mod);
      plines = (CARD32 *)(pline + mod * s3Bpp);

      for (i=0; i < w * realS3Bpp /*s3Bpp*/; i+=4)  {
         /* we need to check for wrap round */
         if (plines + 1 > pend) {
	    switch ((unsigned char *)(pend) - (unsigned char *)(plines)) {
	    case 1:
	       *IMG_TRANS = wrapped1;
	       write_mem_barrier();
	       plines = pnext1;
	       break;
	    case 2:
	       *IMG_TRANS = wrapped2;
	       write_mem_barrier();
	       plines = pnext2;
	       break;
	    case 3:
	    case 4:
	       *IMG_TRANS = wrapped3;
	       write_mem_barrier();
	       plines = pnext3;
	       break;
	    }
	 } else {
	    *IMG_TRANS = ldl_u(plines++);
	    write_mem_barrier();
	 }
	 if (plines == pend)
	    plines = (CARD32 *)pline;
      }
   }
   WaitIdle();
   if (w != origwidth) {
      SETB_CLIP_L_R(0, s3DisplayWidth-1);
   }

   UNBLOCK_CURSOR;
}

static unsigned int _internal_s3_mskbits[33] =
{
   0x0,
   0x1, 0x3, 0x7, 0xf,
   0x1f, 0x3f, 0x7f, 0xff,
   0x1ff, 0x3ff, 0x7ff, 0xfff,
   0x1fff, 0x3fff, 0x7fff, 0xffff,
   0x1ffff, 0x3ffff, 0x7ffff, 0xfffff,
   0x1fffff, 0x3fffff, 0x7fffff, 0xffffff,
   0x1ffffff, 0x3ffffff, 0x7ffffff, 0xfffffff,
   0x1fffffff, 0x3fffffff, 0x7fffffff, 0xffffffff
};

#define MSKBIT(n) (_internal_s3_mskbits[(n)])


static void
s3RealImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
		   fgPixel, bgPixel, alu, planemask, opaque)
     int		x;
     int		y;
     int		w;
     int		h;
     unsigned char	*psrc;
     int		pw, ph, pox, poy;
     int		pwidth;
     Pixel		fgPixel;
     Pixel		bgPixel;
     int		alu;
     Pixel		planemask;
     int		opaque;
{
   int		srcx, srch, dstw;
   CARD32	*ptmp;
   int 		origwidth;

   if (alu == ROP_D || w == 0 || h == 0)
      return;

   BLOCK_CURSOR;

   origwidth = w;
   w = s3CheckLSPN(w, 1);
   if (w != origwidth) {
      WaitQueue(6 + (opaque ? 1 : 0));
      SETB_CLIP_L_R(x, x + origwidth-1);
   }
   else {
      WaitQueue(4 + (opaque ? 1 : 0));
   }

   ;SET_WRT_MASK(planemask);
   ;SET_FRGD_MIX(FSS_FRGDCOL | alu);
   ;SET_PIX_CNTL(MIXSEL_EXPPC | COLCMPOP_F);

   SETB_SRC_FG_CLR(fgPixel);
   if (opaque) {
      SETB_SRC_BG_CLR(bgPixel);
   }
   else {
      alu |= MIX_MONO_TRANSP;
   }

   SETB_RWIDTH_HEIGHT(w - 1, h);
   SETB_RDEST_XY(x, y);
   WaitIdle();
   SETB_CMD_SET(s3_gcmd | CMD_BITBLT | INC_X | INC_Y | alu
		| CMD_ITA_DWORD | MIX_CPUDATA | MIX_MONO_SRC );

   modulus(x - pox, pw, x);
   modulus(y - poy, ph, y);
   /*
    * When the source bitmap is properly aligned, max 32 pixels wide
    * and nonrepeating, use this faster loop instead.
    */
   if (alu != ROP_0 && alu != ROP_1 && alu != ROP_D && alu != ROP_Dn)
   if ((x & 7) == 0 && w <= 32 && x+w <= pw && y+h <= ph) {
      CARD32 pix;
      unsigned char *pnt;

      pnt = (unsigned char *)(psrc + pwidth * y + (x >> 3));
      while( h-- > 0 ) {
	 pix = (CARD32)(ldl_u((CARD32 *)(pnt)));

	 *IMG_TRANS = s3SwapBits[   pix         & 0xff ]
	           | (s3SwapBits[ ( pix >>  8 ) & 0xff ] <<  8)
		   | (s3SwapBits[ ( pix >> 16 ) & 0xff ] << 16)
		   | (s3SwapBits[ ( pix >> 24 ) & 0xff ] << 24);
	 write_mem_barrier();
	 pnt += pwidth;
      }
   }
   else {
      while( h > 0 ) {
	 srch = ( y+h > ph ? ph - y : h );
	 while( srch > 0 ) {
	    dstw = w;
	    srcx = x;
	    ptmp = (CARD32 *)(psrc + pwidth * y);
	    while( dstw > 0 ) {
	       int np, x2;
	       CARD32 *pnt, pix;
	       /*
		* Assemble 32 bits and feed them to the draw engine.
		*/
	       np = pw - srcx;	/* number of pixels left in bitmap.*/
	       pnt =(CARD32 *)((unsigned char *)(ptmp) + (srcx >> 3));
	       x2 = srcx & 7;	/* Offset within byte. */
	       if (np >= 32) {
#ifdef __alpha__
		  pix =   (CARD32)(ldq_u((unsigned long *)(pnt)) >> x2);
#else
		  pix =   (CARD32)(ldl_u((unsigned int *)(pnt)) >> x2);
		  if (x2 > 0)
		     pix |= (CARD32)(ldl_u((unsigned int *)(pnt+1)) << (32-x2));
#endif
	       }
	       else if (pw >= 32) {
#ifdef __alpha__
		  pix = (CARD32)((ldq_u((unsigned long *)(pnt)) >> x2)
				 & MSKBIT(np)) | (ldl_u(ptmp) << np);
#else
		  pix =   (CARD32)(  ldl_u((unsigned int *)(pnt)) >> x2 );
		  if (x2 > 0)
		     pix |= (CARD32)(ldl_u((unsigned int *)(pnt+1)) << (32-x2));
		  pix = (pix  & MSKBIT(np)) | (ldl_u(ptmp) << np);
#endif
	       }
	       else if (pw >= 16) {
		  if (np+pw >= 32)
		     pix = ((ldl_u(pnt) >> x2) & MSKBIT(np)) | ((ldl_u(ptmp) & MSKBIT(pw)) << np);
		  else
		     pix = ((ldl_u(pnt) >> x2) & MSKBIT(np)) | ((ldl_u(ptmp) & MSKBIT(pw)) << np)
			| (ldl_u(ptmp) << (np+pw));
	       }
	       else {
		  pix = (ldl_u(pnt) >> x2) & MSKBIT(np);
		  while( np < 32 && np < dstw ) {
		     pix |= (ldl_u(ptmp) & MSKBIT(pw)) << np;
		     np += pw;
		  }
	       }
	       *IMG_TRANS = s3SwapBits[   pix         & 0xff ]
		         | (s3SwapBits[ ( pix >>  8 ) & 0xff ] <<  8)
			 | (s3SwapBits[ ( pix >> 16 ) & 0xff ] << 16)
			 | (s3SwapBits[ ( pix >> 24 ) & 0xff ] << 24);
	       write_mem_barrier();
	       srcx += 32;
	       while (srcx >= pw)
		  srcx -= pw;
	       dstw -= 32;
	    }
	    y++;
	    h--;
	    srch--;
	 }
	 y = 0;
      }
   }
   WaitIdle();

   if (w != origwidth) {
      SETB_CLIP_L_R(0, s3DisplayWidth-1);
   }

   UNBLOCK_CURSOR;
}

void
#if NeedFunctionPrototypes
s3ImageStipple (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   pw,
     int   ph,
     int   pox,
     int   poy,
     Pixel fgPixel,
     int   alu,
     unsigned long planemask)
#else
s3ImageStipple (x, y, w, h, psrc, pwidth, pw, ph, pox, poy, fgPixel, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   pw;
     int   ph;
     int   pox;
     int   poy;
     Pixel fgPixel;
     int   alu;
     unsigned long planemask;
#endif
{

    s3RealImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
		       fgPixel, 0, alu, planemask, 0);
}

#if NeedFunctionPrototypes
void
s3ImageOpStipple (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   pw,
     int   ph,
     int   pox,
     int   poy,
     Pixel fgPixel,
     Pixel bgPixel,
     int   alu,
     unsigned long planemask)
#else
void
s3ImageOpStipple (x, y, w, h, psrc, pwidth, pw,
		  ph, pox, poy, fgPixel, bgPixel, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   pw, ph, pox, poy;
     Pixel fgPixel;
     Pixel bgPixel;
     int   alu;
     unsigned long planemask;
#endif
{

    s3RealImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
		       fgPixel, bgPixel, alu, planemask, 1);
}
