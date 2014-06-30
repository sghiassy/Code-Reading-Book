/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3/s3im.c,v 3.40.2.1 1997/05/06 13:25:44 dawes Exp $ */
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
/* $XConsortium: s3im.c /main/19 1996/10/28 05:30:59 kaleb $ */


#include "misc.h"
#include "xf86.h"
#include "s3.h"
#include "regs3.h"
#include "s3im.h"
#include "scrnintstr.h"
#include "cfbmskbits.h"
#include "s3linear.h"
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

#ifdef __alpha__
extern Bool isJensen;
#define MemToBusBase(Base,dst,src,count) { if(isJensen) JensenMemToBus(Base,dst,src,count); else MemToBus(&Base[dst],src,count); }
#define BusToMemBase(dst,Base,src,count) { if(isJensen) JensenBusToMem(Base,dst,src,count); else BusToMem(dst,&Base[src],count); }
#else
#define MemToBusBase(Base,dst,src,count) MemToBus(&Base[dst],src,count)
#define BusToMemBase(dst,Base,src,count) BusToMem(dst,&Base[src],count)
#endif

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
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static void s3ImageFill (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
static void s3ImageReadNoMem (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
void s3ImageWriteNoMem (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static void s3ImageFillNoMem (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
static void s3ImageReadBanked (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, unsigned long
#endif
);
static void s3ImageWriteBanked (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, short, unsigned long
#endif
);
static void s3ImageFillBanked (
#if NeedFunctionPrototypes
    int, int, int, int, char *, int, int, int, int, int, short, unsigned long
#endif
);
static char old_bank = -1;
extern char s3Mbanks;
extern ScrnInfoRec s3InfoRec;
Pixel s3BppPMask;

void
s3ImageInit ()
{
   int   i;
   static Bool reEntry = FALSE;
      
   if (reEntry) {
      old_bank=-1;
      if (s3InfoRec.videoRam > 1024)
	 s3Mbanks = -1;
      return;
   }
   
   reEntry = TRUE;
   s3Bpp = s3InfoRec.bitsPerPixel / 8;

   s3BppDisplayWidth = s3Bpp * s3DisplayWidth;
   s3BppPMask = (1UL << s3InfoRec.bitsPerPixel) - 1;

   for (i = 0; i < 256; i++) {
      reorder (i, s3SwapBits[i]);
   }
   
   if (OFLG_ISSET(OPTION_NO_MEM_ACCESS, &s3InfoRec.options)) {
      s3ImageReadFunc = s3ImageReadNoMem;
      s3ImageWriteFunc = s3ImageWriteNoMem;
      s3ImageFillFunc = s3ImageFillNoMem;
      ErrorF ("%s %s: Direct memory accessing has been disabled\n",
	      OFLG_ISSET(XCONFIG_NOMEMACCESS, &s3InfoRec.xconfigFlag) ?
		XCONFIG_PROBED : XCONFIG_GIVEN, s3InfoRec.name);
      return;
   }


  if (s3LinearAperture &&
      !OFLG_ISSET(OPTION_NOLINEAR_MODE, &s3InfoRec.options)) {
	 s3ImageReadFunc = s3ImageRead;
	 s3ImageWriteFunc = s3ImageWrite; 
	 s3ImageFillFunc = s3ImageFill;            
   } else {
      if (!(s3BankSize % s3BppDisplayWidth)) {
	 s3ImageReadFunc = s3ImageRead;
	 s3ImageWriteFunc = s3ImageWrite; 
	 s3ImageFillFunc = s3ImageFill;
      } else {
	 s3ImageReadFunc = s3ImageReadBanked;
	 s3ImageWriteFunc = s3ImageWriteBanked;
	 s3ImageFillFunc = s3ImageFillBanked;
      }
   }


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

static void
#if NeedFunctionPrototypes
s3ImageWriteBanked (
     int   x,
     int   y,
     int   w,
     int   h,
     char *psrc,
     int   pwidth,
     int   px,
     int   py,
     short alu,
     unsigned long planemask)
#else
s3ImageWriteBanked (x, y, w, h, psrc, pwidth, px, py, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   px;
     int   py;
     short alu;
     unsigned long planemask;
#endif
{
   int   j, offset;
   char  bank;
   char *videobuffer;

   if (alu == MIX_DST)
      return;

   if ((alu != MIX_SRC) || ((planemask & s3BppPMask) != s3BppPMask)) {
      s3ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask);
      return;
   }

   videobuffer = (char *) s3VideoMem;
            
   if (w == 0 || h == 0)
      return;

   BLOCK_CURSOR;
   /* hmm this historic junk? */
#if 0
   WaitQueue16_32(2,3);
   outw (FRGD_MIX, FSS_PCDATA | alu);
   outw32(WRT_MASK, planemask);

#endif

   psrc += pwidth * py + px * s3Bpp;
   offset = (y * s3BppDisplayWidth) + x*s3Bpp;
   bank = offset / s3BankSize;
   offset %= s3BankSize;
   w *= s3Bpp;

   WaitIdleEmpty();
   s3EnableLinear();
 /*
  * if we do a bank switch here, is _not_ possible to do one in the loop
  * before some data has been copied; for that situation to occur it would be
  * necessary that offset == s3BankSize; and by the above initialisation of
  * offset, we know offset < s3BankSize
  */
   if (old_bank != bank) {
      s3BankSelect(bank);
   }

   for (j = 0; j < h; j++, psrc += pwidth, offset += s3BppDisplayWidth) {
      if (offset + w > s3BankSize) {
	 int   partwidth;

       /* do the copy in two parts with a bank switch inbetween */
	 partwidth = s3BankSize - offset;
	 if (partwidth > 0)
	    MemToBusBase (videobuffer, offset, psrc, partwidth);

       /* bank switch to the next bank */
	 bank++;
	s3BankSelect(bank);

       /* adjust the offset by 1 banks worth */
	 offset -= s3BankSize;

       /* for a partial copy, copy the bit that was left over only */
	 if (partwidth > 0) {
	    MemToBusBase (videobuffer, 0, psrc + partwidth, w - partwidth);
	    continue;
	 }
       /* drop through to the `normal' copy */
      }
      MemToBusBase (videobuffer, offset, psrc, w);
   }
   old_bank = bank;

   s3DisableLinear();
#if 0
   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
#endif
   UNBLOCK_CURSOR;
}

static void
#if NeedFunctionPrototypes
s3ImageReadBanked (
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
s3ImageReadBanked (x, y, w, h, psrc, pwidth, px, py, planemask)
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
   int   i,j,w0;
   int   offset;
   int   bank;
   char *videobuffer;
   unsigned long l_planemask = 0;

   if (w == 0 || h == 0)
      return;

   if ((planemask & s3BppPMask) != s3BppPMask && !S3_x64_SERIES(s3ChipId)) {
      s3ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask);
      return;
   }

   videobuffer = (char *) s3VideoMem;

 
#if 0
   outw (FRGD_MIX, FSS_PCDATA | MIX_SRC);
#endif
 
   psrc += pwidth * py + px * s3Bpp;
   offset = (y * s3BppDisplayWidth) + x * s3Bpp;
   bank = offset / s3BankSize;
   offset %= s3BankSize;
   if ((planemask & s3BppPMask) != s3BppPMask) {
      w0 = w;
      l_planemask = planemask &= s3BppPMask;
      for (i=s3Bpp; i<sizeof(long); i+=s3Bpp)
	 l_planemask = (l_planemask << (s3Bpp<<3)) | planemask;
#ifdef __alpha__
      if (s3Bpp == 3)
	 l_planemask |= (1<<(24*(sizeof(long)/3))) - 1;
#endif
      planemask |= ~s3BppPMask;
   }
   else 
      w0 = 0;

   w *= s3Bpp;

   BLOCK_CURSOR;
   WaitIdleEmpty ();
   s3EnableLinear();
   if (old_bank != bank) {
      s3BankSelect(bank);
   }

   for (j = 0; j < h; j++, psrc += pwidth, offset += s3BppDisplayWidth) {
      if (offset + w > s3BankSize) {
	 int   partwidth;

       /* do the copy in two parts with a bank switch inbetween */
	 partwidth = s3BankSize - offset;
	 if (partwidth > 0)
	    BusToMemBase (psrc, videobuffer, offset, partwidth);

       /* bank switch to the next bank */
	 bank++;
         s3BankSelect(bank);

       /* adjust the offset by 1 banks worth */
	 offset -= s3BankSize;

	 if (partwidth > 0) {
	    BusToMemBase (psrc + partwidth, videobuffer, 0, w - partwidth);
	    continue;
	 }
       /* drop through to the `normal' copy */
      }
      BusToMemBase (psrc, videobuffer, offset, w);
      if (w0) {
	 char *p = psrc;

	 switch (s3Bpp) {
	 case 1: 
	    for (i=w0; i>=sizeof(long); i-=sizeof(long),p+=sizeof(long))
	       *((long*)p) &= l_planemask;
	    for (; i; i--)
	       *p++ &= planemask;
	    break;
	 case 2: 
	    for (i=w0; i>=sizeof(long); i-=sizeof(long),p+=sizeof(long))
	       *((long*)p) &= l_planemask;
	    for (; i; i--,p+=sizeof(short))
	       *((short*)p) &= planemask;
	    break;
	 case 3:  /* XXX depends on byte sex! __BYTE_ORDER == 1234 assumed */
	    i=w0;
#ifdef __alpha__
	    for (; i>=1; i-=2,p+=6)
	       *((long*)p) &= l_planemask ;
#endif
	    for (; i; i--,p+=3) 
	       *((int*)p) &= planemask;
	    break;
	 case 4:
	    for (i=w0; i>=sizeof(long); i-=sizeof(long),p+=sizeof(long))
	       *((long*)p) &= l_planemask;
	    for (; i; i--,p+=sizeof(int))
	       *((int*)p) &= planemask;
	    break;
	 }
      }
   }
   old_bank = bank;
   s3DisableLinear();
#if 0
   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
#endif
   UNBLOCK_CURSOR;
}

static void
#if NeedFunctionPrototypes
s3ImageFillBanked (
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
     short alu,
     unsigned long planemask)
#else
s3ImageFillBanked (x, y, w, h, psrc, pwidth, pw, ph, pox, poy, alu, planemask)
     int   x;
     int   y;
     int   w;
     int   h;
     char *psrc;
     int   pwidth;
     int   pw, ph, pox, poy;
     short alu;
     unsigned long planemask;
#endif
{
   int   j;
   char *pline;
   int   ypix, xpix0, offset0;
   int   cxpix;
   char *videobuffer;
   char  bank;

   if (alu == MIX_DST)
      return;

   if ((alu != MIX_SRC) || ((planemask & s3BppPMask) != s3BppPMask)) {
      s3ImageFillNoMem(x, y, w, h, psrc, pwidth,
                     pw, ph, pox, poy, alu, planemask);
      return;
   }
   videobuffer = (char *) s3VideoMem;

   if (w == 0 || h == 0)
      return;
      
   BLOCK_CURSOR;
#if 0
   WaitQueue16_32(2,3);
   outw (FRGD_MIX, FSS_PCDATA | alu);
   outw32(WRT_MASK, planemask);

   WaitQueue(8);
#endif
   pw *= s3Bpp;
   modulus ((x - pox) * s3Bpp, pw, xpix0);
   cxpix = pw - xpix0;

   modulus (y - poy, ph, ypix);
   pline = psrc + pwidth * ypix;

   offset0 = (y * s3BppDisplayWidth) + x * s3Bpp;
   bank = offset0 / s3BankSize;
   offset0 %= s3BankSize;

   w *= s3Bpp;

   WaitIdleEmpty();
   s3EnableLinear();
   if (bank != old_bank) {
      s3BankSelect(bank);
   }

   for (j = 0; j < h; j++, offset0 += s3BppDisplayWidth) {
      int   offset, width, xpix;

      width = (w <= cxpix) ? w : cxpix;
      xpix = xpix0;
      offset = offset0;

      if (offset + width >= s3BankSize) {
	 int   partwidth;

	 partwidth = s3BankSize - offset;
	 offset0 -= s3BankSize;

	 if (partwidth > 0) {
	    MemToBusBase (videobuffer, offset, pline + xpix, partwidth);
	    width -= partwidth;
	    xpix += partwidth;
	    offset = 0;
	 } else
	    offset = offset0;

	 bank++;
         s3BankSelect(bank);      
      }
      MemToBusBase (videobuffer, offset, pline + xpix, width);

      offset += width;
      for (width = w - cxpix; width >= pw; width -= pw, offset += pw) {
       /* identical to ImageWrite() */
	 if (offset + pw > s3BankSize) {
	    int   partwidth;

	    partwidth = s3BankSize - offset;
	    if (partwidth > 0)
	       MemToBusBase (videobuffer, offset, pline, partwidth);

	    bank++;
	    s3BankSelect(bank);     
	    offset -= s3BankSize;
	    offset0 -= s3BankSize;	     

	    if (partwidth > 0) {
	       MemToBusBase (videobuffer, 0, pline + partwidth, pw - partwidth);
	       continue;
	    }
	 }
	 MemToBusBase (videobuffer, offset, pline, pw);
      }

    /* at this point: 0 <= width < pw */
      xpix = 0;    
      if (width > 0) {
	 if (offset + width > s3BankSize) {
	    int   partwidth;

	    partwidth = s3BankSize - offset;

	    if (partwidth > 0) {
	       MemToBusBase (videobuffer, offset, pline, partwidth);
	       width -= partwidth;
	       xpix = partwidth;
	       offset = 0;
	    } else {
	       offset -= s3BankSize;
	    }

	    offset0 -= s3BankSize;
	    bank++;
	    s3BankSelect(bank);
	 }
	 MemToBusBase (videobuffer, offset, pline + xpix, width);
      }
      if ((++ypix) == ph) {
	 ypix = 0;
	 pline = psrc;
      } else
	 pline += pwidth;
   }
   old_bank = bank;

   s3DisableLinear();
   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;
}
/* BL */
#ifdef S3_NEWMMIO
#undef s3EnableLinear
#undef s3DisableLinear
#undef s3BankSelect
#define s3EnableLinear()
#define s3DisableLinear()
#define s3BankSelect(X)
#undef  SET_MULT_MISC
#define SET_MULT_MISC(val) outw(MULTIFUNC_CNTL, MULT_MISC) /* for WaitQueue16_32 */
#endif
/* end BL */

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
     short alu,
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
     short alu;
     unsigned long planemask;
#endif
{
   int   j, offset;
   char  bank;
   char *videobuffer;

   if (alu == MIX_DST)
      return;

   if ((alu != MIX_SRC) || ((planemask & s3BppPMask) != s3BppPMask)) {
      s3ImageWriteNoMem(x, y, w, h, psrc, pwidth, px, py, alu, planemask);
      return;
   }

   videobuffer = (char *) s3VideoMem;

   if (w == 0 || h == 0)
      return;
      
   BLOCK_CURSOR;
#if 0
   WaitQueue16_32(2,3);
   outw (FRGD_MIX, FSS_PCDATA | alu);
   outw32(WRT_MASK, planemask);
#endif

   psrc += pwidth * py + px * s3Bpp;
   offset = (y * s3BppDisplayWidth) + x *s3Bpp;
   bank = offset / s3BankSize;
   offset %= s3BankSize;

   w *= s3Bpp;
   WaitIdleEmpty();
   s3EnableLinear();   
 /*
  * if we do a bank switch here, is _not_ possible to do one in the loop
  * before some data has been copied; for that situation to occur it would be
  * necessary that offset == s3BankSize; and by the above initialisation of
  * offset, we know offset < s3BankSize
  */
   if (old_bank != bank) {
      s3BankSelect(bank);
   }
   
   for (j = 0; j < h; j++, psrc += pwidth, offset += s3BppDisplayWidth) {
      if (offset >= s3BankSize) {
       /* bank switch to the next bank */
	 bank++;
	 s3BankSelect(bank);

       /* adjust the offset by 1 banks worth */
	 offset -= s3BankSize;
      }
      MemToBusBase (videobuffer, offset, psrc, w);
   }
   old_bank = bank;
   s3DisableLinear();
#if 0
   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
#endif
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
   int   i,j,w0;
   int   offset;
   int   bank;
   char *videobuffer;
   unsigned long l_planemask = 0;

   if (w == 0 || h == 0)
      return;

   if ((planemask & s3BppPMask) != s3BppPMask && !S3_x64_SERIES(s3ChipId)) {
      s3ImageReadNoMem(x, y, w, h, psrc, pwidth, px, py, planemask);
      return;
   }

   videobuffer = (char *) s3VideoMem;
      

#if 0
   outw (FRGD_MIX, FSS_PCDATA | MIX_SRC);
#endif
 
   psrc += pwidth * py + px * s3Bpp;
   offset = (y * s3BppDisplayWidth) + x * s3Bpp;
   bank = offset / s3BankSize;
   offset %= s3BankSize;
   if ((planemask & s3BppPMask) != s3BppPMask) {
      w0 = w;
      l_planemask = planemask &= s3BppPMask;
      for (i=s3Bpp; i<sizeof(long); i+=s3Bpp)
	 l_planemask = (l_planemask << (s3Bpp<<3)) | planemask;
#ifdef __alpha__
      if (s3Bpp == 3)
	 l_planemask |= (1<<(24*(sizeof(long)/3))) - 1;
#endif
      planemask |= ~s3BppPMask;
   }
   else 
      w0 = 0;

   w *= s3Bpp;

   BLOCK_CURSOR;
   WaitIdleEmpty ();
   s3EnableLinear();
   if (old_bank != bank) {
      s3BankSelect(bank);
   }

   for (j = 0; j < h; j++, psrc += pwidth, offset += s3BppDisplayWidth) {
      if (offset >= s3BankSize) {
       /* bank switch to the next bank */
	 bank++;
	 s3BankSelect(bank);

       /* adjust the offset by 1 banks worth */
	 offset -= s3BankSize;
      }
      BusToMemBase (psrc, videobuffer, offset, w);
      if (w0) {
	 char *p = psrc;

	 switch (s3Bpp) {
	 case 1: 
	    for (i=w0; i>=sizeof(long); i-=sizeof(long),p+=sizeof(long))
	       *((long*)p) &= l_planemask;
	    for (; i; i--)
	       *p++ &= planemask;
	    break;
	 case 2: 
	    for (i=w0; i>=sizeof(long); i-=sizeof(long),p+=sizeof(long))
	       *((long*)p) &= l_planemask;
	    for (; i; i--,p+=sizeof(short))
	       *((short*)p) &= planemask;
	    break;
	 case 3:  /* XXX depends on byte sex! __BYTE_ORDER == 1234 assumed */
	    i=w0;
#ifdef __alpha__
	    for (; i>=1; i-=2,p+=6)
	       *((long*)p) &= l_planemask ;
#endif
	    for (; i; i--,p+=3) 
	       *((int*)p) &= planemask;
	    break;
	 case 4:
	    for (i=w0; i>=sizeof(long); i-=sizeof(long),p+=sizeof(long))
	       *((long*)p) &= l_planemask;
	    for (; i; i--,p+=sizeof(int))
	       *((int*)p) &= planemask;
	    break;
	 }
      }
   }
   old_bank = bank;

   s3DisableLinear();
#if 0
   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
#endif
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
     short alu,
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
     short alu;
     unsigned long planemask;
#endif
{
   int   j;
   char *pline;
   int   ypix, xpix, offset0;
   int   cxpix;
   char *videobuffer;
   char  bank;

   if (alu == MIX_DST)
      return;

   if ((alu != MIX_SRC) || ((planemask & s3BppPMask) != s3BppPMask)) {
     s3ImageFillNoMem(x, y, w, h, psrc, pwidth,
                    pw, ph, pox, poy, alu, planemask);
     return;
   }

   videobuffer = (char *) s3VideoMem;

   if (w == 0 || h == 0)
      return;
      
 
#if 0
   WaitQueue16_32(2,3);
   outw (FRGD_MIX, FSS_PCDATA | alu);
   outw32(WRT_MASK, planemask);
#endif

   w  *= s3Bpp;
   pw *= s3Bpp;

   modulus ((x - pox) * s3Bpp, pw, xpix);
   cxpix = pw - xpix ;

   modulus (y - poy, ph, ypix);
   pline = psrc + pwidth * ypix;

   offset0 = (y * s3BppDisplayWidth) + x * s3Bpp;
   bank = offset0 / s3BankSize;
   offset0 %= s3BankSize;

   BLOCK_CURSOR;
   WaitIdleEmpty();
   s3EnableLinear();
   if (bank != old_bank) {
      s3BankSelect(bank);
   }

   for (j = 0; j < h; j++, offset0 += s3BppDisplayWidth) {
      if (offset0 >= s3BankSize) {
	 bank++;
	 s3BankSelect(bank);
	 offset0 -= s3BankSize;
      }
      if (w <= cxpix) {
	 MemToBusBase (videobuffer, offset0, pline + xpix, w);
      } else {
	 int   width, offset;

	 MemToBusBase (videobuffer, offset0, pline + xpix, cxpix);

	 offset = offset0 + cxpix;
	 for (width = w - cxpix; width >= pw; width -= pw, offset += pw)
	    MemToBusBase (videobuffer, offset, pline, pw);

       /* at this point: 0 <= width < pw */
	 if (width > 0)
	    MemToBusBase (videobuffer, offset, pline, width);
      }

      if ((++ypix) == ph) {
	 ypix = 0;
	 pline = psrc;
      } else
	 pline += pwidth;
   }
   old_bank = bank;
   s3DisableLinear();
#if 0
   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
#endif
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
     short alu,
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
     short alu;
     unsigned long planemask;
#endif
{
   int   i, j;

   if (alu == MIX_DST)
      return;

   if (w == 0 || h == 0)
      return;
      
   BLOCK_CURSOR;
   WaitQueue16_32(3,4);
   outw (FRGD_MIX, FSS_PCDATA | alu);
   outw32 (WRT_MASK, planemask);
   outw (MULTIFUNC_CNTL, PIX_CNTL | 0);

   WaitQueue(5);
   outw (CUR_X, (short) x);
   outw (CUR_Y, (short) y);
   outw (MAJ_AXIS_PCNT, (short) w - 1);
   outw (MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h - 1));
   WaitIdle();
   outw (CMD, CMD_RECT | BYTSEQ | _16BIT | INC_Y | INC_X | DRAW | PCDATA
	  | WRTDATA);

   w *= s3Bpp;
   psrc += pwidth * py;

   for (j = 0; j < h; j++) {
      /* This assumes we can cast odd addresses to short! NOT!! */
      short *psrcs = (short *)&psrc[px*s3Bpp];
      for (i = 0; i < (w & ~1); ) {
	 if (s3InfoRec.bitsPerPixel == 32) {
	    outl (PIX_TRANS, ldl_u((unsigned int *)psrcs));
	    psrcs+=2;
	    i += 4;
	 }
	 else {
	    outw (PIX_TRANS, ldw_u(psrcs));
	    psrcs++;
	    i += 2;
	 }
      }
      if (i < w)  /* can't happen for 32bpp */
	 outw (PIX_TRANS, *((char*)psrcs));
      psrc += pwidth;
   }
#if 0
   WaitQueue16_32(2,3);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   outw32(WRT_MASK, ~0);
#endif
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
   int   i, j;

   if (w == 0 || h == 0)
      return;
      
   BLOCK_CURSOR;
   WaitIdleEmpty ();
   WaitQueue(7);
   outw (MULTIFUNC_CNTL, PIX_CNTL);
   outw (FRGD_MIX, FSS_PCDATA | MIX_SRC);
   outw (CUR_X, (short) x);
   outw (CUR_Y, (short) y);
   outw (MAJ_AXIS_PCNT, (short) w - 1);
   outw (MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h - 1));
   outw (CMD, CMD_RECT  | BYTSEQ | _16BIT | INC_Y | INC_X | DRAW |
	  PCDATA);

   WaitQueue16_32(1,2);

   outw32(RD_MASK, planemask);

   WaitQueue(8);

 /* wait for data to be ready */
   if (!S3_x64_SERIES(s3ChipId)) {
     while ((inw (GP_STAT) & 0x100) == 0) ;
   }
   else {
     /* x64: GP_STAT bit 8 is reserved for 864/964 */
   }

   w *= s3Bpp;
   psrc += pwidth * py;

   for (j = 0; j < h; j++) {
      short *psrcs = (short *)&psrc[px*s3Bpp]; 
      for (i = 0; i < (w & ~1); ) {
	 if (s3InfoRec.bitsPerPixel == 32) {
	    *((long*)(psrcs)) = inl(PIX_TRANS);
	    psrcs += 2;
	    i += 4;
	 } else {
	    *psrcs++ = inw(PIX_TRANS);
	    i += 2;
	 }
      }
      if (i < w)  /* can't happen for 32bpp */
	 *((char*)psrcs) = inw(PIX_TRANS);
      psrc += pwidth;
   }
   WaitQueue16_32(2,3);
   outw32(RD_MASK, ~0);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);
   UNBLOCK_CURSOR;
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
     short alu,
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
     short alu;
     unsigned long planemask;
#endif
{
   int   i, j;
   char *pline;
   int   mod;

   if (alu == MIX_DST)
      return;

   if (w == 0 || h == 0)
      return;

   BLOCK_CURSOR;  
   WaitQueue16_32(7,8);
   outw (FRGD_MIX, FSS_PCDATA | alu);
   outw32 (WRT_MASK, planemask);
   outw (CUR_X, (short) x);
   outw (CUR_Y, (short) y);
   outw (MAJ_AXIS_PCNT, (short) w - 1);
   outw (MULTIFUNC_CNTL, MIN_AXIS_PCNT | (h - 1));
   WaitIdle();
   outw (CMD, CMD_RECT | BYTSEQ|_16BIT | INC_Y | INC_X | DRAW |
	 PCDATA | WRTDATA);

   for (j = 0; j < h; j++) {
      unsigned short wrapped;
      unsigned short *pend;
      unsigned short *plines;

      modulus (y + j - poy, ph, mod);
      pline = psrc + pwidth * mod;
      pend = (unsigned short *)&pline[pw*s3Bpp];
      wrapped = (pline[0] << 8) + (pline[pw-1] << 0); /* only for 8bpp */

      modulus (x - pox, pw, mod);

      plines = (unsigned short *) &pline[mod*s3Bpp];

      for (i = 0; i < w * s3Bpp;)  {

         /* arrg in 8BPP we need to check for wrap round */
         if (plines + 1 > pend) {
	    outw (PIX_TRANS, wrapped);
            plines = (unsigned short *)&pline[1]; i += 2;
	 } else {
	    if (s3InfoRec.bitsPerPixel == 32) {
	       outl (PIX_TRANS, ldl_u((unsigned int *)(plines)));
	       plines += 2;
	       i += 4;
	    }
	    else {
	       outw (PIX_TRANS, ldw_u(plines++));
	       i += 2;
	    }
	 }
	 if (plines == pend)
	    plines = (unsigned short *)pline;
	 
      }
   }

   WaitQueue(1);
   outw (FRGD_MIX, FSS_FRGDCOL | MIX_SRC);   
   UNBLOCK_CURSOR;
}

static int _internal_s3_mskbits[17] =
{
   0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff, 0x1ff, 0x3ff, 0x7ff,
   0xfff, 0x1fff, 0x3fff, 0x7fff, 0xffff
};

#define MSKBIT(n) (_internal_s3_mskbits[(n)])

#ifdef S3_NEWMMIO
#undef  SET_MULT_MISC
#define SET_MULT_MISC(val)	((mmtr)s3MmioMem)->pk_enh_regs.regs.mult_misc = (val)
#endif

static void
s3RealImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
		   fgPixel, bgPixel, alu, planemask, opaque)
    int			x;
    int			y;
    int			w;
    int			h;
    unsigned char	*psrc;
    int			pw, ph, pox, poy;
    int			pwidth;
    Pixel		fgPixel;
    Pixel		bgPixel;
    short		alu;
    Pixel		planemask;
    int			opaque;
{
    int			srcx, srch, dstw;
    unsigned short	*ptmp;


    if (alu == MIX_DST || w == 0 || h == 0)
	return;

    BLOCK_CURSOR;
    WaitQueue16_32(5,8);
    SET_WRT_MASK(planemask);
    SET_FRGD_MIX(FSS_FRGDCOL | alu);
    if( opaque ) {
      SET_BKGD_MIX(BSS_BKGDCOL | alu);
      SET_BKGD_COLOR(bgPixel);
    }
    else {
       if (s3_968_DashBug) {
	  SET_BKGD_COLOR(0);
	  SET_BKGD_MIX(BSS_BKGDCOL | MIX_OR);
       }
       else {
	  SET_BKGD_MIX(BSS_BKGDCOL | MIX_DST);
       }
    }

    SET_FRGD_COLOR(fgPixel);
    WaitQueue(6);
    SET_PIX_CNTL(MIXSEL_EXPPC | COLCMPOP_F);
    SET_AXIS_PCNT((short) (w - 1), (short)(h-1));
    SET_CURPT((short) x, (short) y);
    WaitIdle();
    SET_CMD(CMD_RECT | PCDATA | _16BIT | INC_Y | INC_X |
	     DRAW | PLANAR | WRTDATA | BYTSEQ);
    modulus(x - pox, pw, x);
    modulus(y - poy, ph, y);
    /*
     * When the source bitmap is properly aligned, max 16 pixels wide
     * and nonrepeating, use this faster loop instead.
     */
    if( (x & 7) == 0 && w <= 16 && x+w <= pw && y+h <= ph ) {
	unsigned short pix;
	unsigned char *pnt;

	pnt = (unsigned char *)(psrc + pwidth * y + (x >> 3));
	while( h-- > 0 ) {
	    pix = ldw_u(((unsigned short *)(pnt)));
	    SET_PIX_TRANS_W(s3SwapBits[ pix & 0xff ] | 
			       s3SwapBits[ ( pix >> 8 ) & 0xff ] << 8);
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
		      pix = (unsigned short)(ldl_u((unsigned int *)(pnt)) >> x2);
		    }
		    else if( pw >= 16 ) {
		      pix = (unsigned short)((ldl_u((unsigned int *)(pnt)) >> x2)
					     & MSKBIT(np)) | (ldw_u(ptmp) << np);
		    }
		    else if( pw >= 8 ) {
			pix = ((ldw_u(pnt) >> x2) & MSKBIT(np)) 
			   | ((ldw_u(ptmp) & MSKBIT(pw)) << np) | (ldw_u(ptmp) << (np+pw));
		    }
		    else {
			pix = (ldw_u(pnt) >> x2) & MSKBIT(np);
			while( np < 16 && np < dstw ) {
			    pix |= (ldw_u(ptmp) & MSKBIT(pw)) << np;
			    np += pw;
			}
		    }
		    SET_PIX_TRANS_W(s3SwapBits[ pix & 0xff ] | 
				       s3SwapBits[ ( pix >> 8 ) & 0xff ] << 8);
		    srcx += 16;
		    while (srcx >= pw)
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
    WaitQueue(4);
    SET_MIX(FSS_FRGDCOL | MIX_SRC, BSS_BKGDCOL | MIX_SRC);
    SET_PIX_CNTL(MIXSEL_FRGDMIX | COLCMPOP_F);
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
     short alu,
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
     short alu;
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
     short alu,
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
     short alu;
     unsigned long planemask;
#endif
{

    s3RealImageStipple(x, y, w, h, psrc, pwidth, pw, ph, pox, poy,
		       fgPixel, bgPixel, alu, planemask, 1);
}
