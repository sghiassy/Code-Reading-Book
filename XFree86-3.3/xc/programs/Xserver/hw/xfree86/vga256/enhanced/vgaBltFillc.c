/*
 * Copyright 1997
 * Digital Equipment Corporation. All rights reserved.
 * This software is furnished under license and may be used and copied only in 
 * accordance with the following terms and conditions.  Subject to these 
 * conditions, you may download, copy, install, use, modify and distribute 
 * this software in source and/or binary form. No title or ownership is 
 * transferred hereby.
 * 1) Any source code used, modified or distributed must reproduce and retain 
 *    this copyright notice and list of conditions as they appear in the 
 *    source file.
 *
 * 2) No right is granted to use any trade name, trademark, or logo of Digital 
 *    Equipment Corporation. Neither the "Digital Equipment Corporation" name 
 *    nor any trademark or logo of Digital Equipment Corporation may be used 
 *    to endorse or promote products derived from this software without the 
 *    prior written permission of Digital Equipment Corporation.
 *
 * 3) This software is provided "AS-IS" and any express or implied warranties,
 *    including but not limited to, any implied warranties of merchantability,
 *    fitness for a particular purpose, or non-infringement are disclaimed. In
 *    no event shall DIGITAL be liable for any damages whatsoever, and in 
 *    particular, DIGITAL shall not be liable for special, indirect, 
 *    consequential, or incidental damages or damages for lost profits, loss 
 *    of revenue or loss of use, whether such damages arise in contract, 
 *    negligence, tort, under statute, in equity, at law or otherwise, even if
 *    advised of the possibility of such damage. 
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/vgaBltFillc.c,v 3.0 1996/12/09 11:54:28 dawes Exp $ */

#include "X.h"
#include "misc.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#include "vga.h"
#include "cfb.h"

/* fBitBlt.s */

void fastBitBltCopy(
    int xdir,
    unsigned char *psrc,
    unsigned char *pdst,
    int height,
    int width,
    int srcPitch,
    int dstPitch
)
{
  if (xdir <= 0) {
#ifdef	__arm32__
      /* I wouldn't have called these variables Pitch... -JJK */
      srcPitch -= width;
      dstPitch -= width;

      /* Note that in this case the inital pdst/psrc are at
       * the upper right of the rectangle
       */
      if (width < 4)
      {
	  while(height > 0)
	  {
	      int i;
	      for(i = 1; i <= width; i++) pdst[-i] = psrc[-i];
	      pdst += dstPitch;
	      psrc += srcPitch;
	      height--;
	  }
      }
      else while(height > 0)
      {
	  /* The plan is to use 32bit reads and writes as much as possible */
	  /* word align the dst first whatever this does to the src        */
	  /* Then do word read/writes using a hold-ing register for spills */
	  /* remember on the ARM the shifts come almost free               */
	  unsigned int hold, tmp;
	  int i, wid = width;
	  unsigned int *ps, *pd;
	  i = 0;

	  /* Need to back up dst */
	  /* if dst is odd do the stray byte */
	  if ((unsigned int)pdst & 1)
	  {
	      i++;
	      pdst[-1] = psrc[-1];
	  }
	  /* pdst - i is at least 16 bit aligned */
	  if (((unsigned int)pdst-i) & 2)
	  {
	      /* Could do one less read if src is short aligned */
	      i+=2;
	      *((unsigned short *)(pdst - i)) = psrc[-i] | ((psrc[-i+1])<<8); 
	  }
	  wid -= i;
	  pd = (unsigned int *) (pdst - i);
	  /* pd points just past first word for copy */
	  ps = (unsigned int *) (((unsigned int)psrc - i) & ~3); /* align down */
	  switch (((unsigned int)psrc - i) & 3)
	  {
	      case 0:
		  /* easy, both alligned */
		  while (wid >= 4) 
		  {
		      *--pd = *--ps;
		      wid -= 4;
		  }
		  if (wid > 0) hold = ps[-1];
		  break;
	      case 1:
		  /* bottom byte valid in hold */
		  hold = *ps;
		  hold <<= 24;
		  while (wid >= 4)
		  {
		      tmp = *--ps;
		      *--pd = hold | (tmp >> 8);
		      hold = tmp<<24;
		      wid -= 4;
		  }
		  if (wid > 1) hold |= (ps[-1]>>8);
		  break;
	      case 2:
		  /* bottom two bytes valid in hold */
		  hold = *ps;
		  hold<<=16;
		  while (wid >= 4)
		  {
		      tmp = *--ps;
		      *--pd = hold | (tmp >> 16);
		      hold = tmp<<16;
		      wid -= 4;
		  }
		  if (wid > 2) hold |= (ps[-1] >> 16);
		  break;
	      case 3:		    
		  /* top byte valid in hold */
		  hold = *ps;
		  hold<<=8;
		  while (wid >= 4)
		  {
		      tmp = *--ps;
		      *--pd = hold | (tmp >> 24);
		      hold = tmp<<8;
		      wid -= 4;
		  }
		  /* Always 3 bytes in hold */
		  break;
	  }
	  /* Always have >= wid bytes in hold */
	  /* pd is word aligned */
	  if (wid > 1)
	  {
	      ((unsigned short *)pd)[-1] = hold>>16;
	      /* pd becomes a BOGUS int * */
	      pd = (unsigned int *) ((unsigned int)pd - 2);
	      wid -= 2;
	      hold <<= 16;
	  }
	  /* pd may be malformed, but we don't care */
	  if (wid > 0)
	      ((unsigned char *)pd)[-1] = hold >> 24;
	  /* next line */
	  psrc += srcPitch;
	  pdst += dstPitch;
	  height--;
      }
#else	/* !__arm32__ */
    while (height--) {
      memmove(pdst-width, psrc-width, width);
      pdst += dstPitch - width;
      psrc += srcPitch - width;
    }
#endif	/* __arm32__ */
  } else {
#ifdef	__arm32__
      /* I wouldn't have called these variables Pitch... -JJK */
      srcPitch += width;
      dstPitch += width;

      if (width < 4)
      {
	  while(height > 0)
	  {
	      int i;
	      for(i = 0; i < width; i++) pdst[i] = psrc[i];
	      pdst += dstPitch;
	      psrc += srcPitch;
	      height--;
	  }
      }
      else while(height > 0)
      {
	  /* The plan is to use 32bit reads and writes as much as possible */
	  /* word align the dst first whatever this does to the src        */
	  /* Then do word read/writes using a hold-ing register for spills */
	  /* remember on the ARM the shifts come almost free               */
	  unsigned int hold, tmp;
	  int i, wid = width;
	  unsigned int *ps, *pd;
	  i = 0;

	  /* if dst is odd do the stray byte */
	  if ((unsigned int)pdst & 1)
	      *pdst = *psrc, i++;
	  /* pdst + i is at least 16 bit aligned */
	  if (((unsigned int)pdst+i) & 2)
	  {
	      /* Could do one less read if src is short aligned */
	      *((unsigned short *)(pdst + i)) = psrc[i] | ((psrc[i+1])<<8); 
	      i+=2;
	  }
	  wid -= i;
	  pd = (unsigned int *) (pdst + i);
	  ps = (unsigned int *) (((int)psrc + i) & ~3); /* align down */
	  switch (((unsigned int)psrc + i) & 3)
	  {
	      case 0:
		  /* easy, both alligned */
		  while (wid >= 4) 
		  {
		      *pd++ = *ps++;
		      wid -= 4;
		  }
		  if (wid > 0) hold = *ps;
		  break;
	      case 1:
		  /* top three bytes valid in hold */
		  hold = *ps;
		  hold >>= 8;
		  while (wid >= 4)
		  {
		      tmp = *++ps;
		      *pd++ = hold | (tmp << 24);
		      hold = tmp>>8;
		      wid -= 4;
		  }
		  /* Never need to read more: wid < 4, have 3 valid */
		  break;
	      case 2:
		  /* top two bytes valid in hold */
		  hold = *ps;
		  hold>>=16;
		  while (wid >= 4)
		  {
		      tmp = *++ps;
		      *pd++ = hold | (tmp << 16);
		      hold = tmp>>16;
		      wid -= 4;
		  }
		  if (wid > 2) hold |= (ps[1] << 16);
		  break;
	      case 3:		    
		  /* top byte valid in hold */
		  hold = *ps;
		  hold>>=24;
		  while (wid >= 4)
		  {
		      tmp = *++ps;
		      *pd++ = hold | (tmp << 8);
		      hold = tmp>>24;
		      wid -= 4;
		  }
		  if (wid > 1) hold |= (ps[1] << 8);
		  break;
	  }
	  /* Always have > wid bytes in hold */
	  /* pd is word aligned */
	  if (wid > 1)
	  {
	      *(unsigned short *)pd = hold & 0xffff;
	      /* pd becomes a BOGUS int * */
	      pd = (unsigned int *) ((int)pd + 2);
	      wid -= 2;
	      hold >>= 16;
	  }
	  /* pd may be malformed, but we don't care */
	  if (wid > 0)
	      *(unsigned char *)pd = hold & 0xff;
	  /* next line */
	  psrc += srcPitch;
	  pdst += dstPitch;
	  height--;
      }
#else	/* !__arm32__ */
    while (height--) {
      memcpy(pdst, psrc, width);
      pdst += dstPitch + width;
      psrc += srcPitch + width;
    }
#endif	/* __arm32__ */
  }
}

/* fFillAnd.s */

unsigned char *fastFillSolidGXand(
    unsigned char *pdst,
    unsigned long fill1,
    unsigned long fill2,
    int hcount,
    int count,
    int w,
    int widthPitch
)
{
	/*
	 * NOTES: original assembly code presumes hcount > 0 to start with
	 *	New code assumes that all bytes of fill1, fill2 are
	 *	consistent.  i.e. 0xefefefef, and not 0x12345678.
	 *	This is because the caller of this routine does a PFILL()
	 *	of the [fill1, fill2] values before they get here.
	 *	For large block cases (count > 3), the original code
	 *	assumed that width == count.
	 *	Fills hcount trips of count bytes each trip through loop
	 */

  if (count == 0)
    return pdst;

  while (hcount > 0) {
	/* No special 'fast' cases here */
    int		cur_count;
    unsigned char	tmpb = fill1;
    unsigned short	tmph = fill1;
    unsigned int	tmpi = fill1;

    cur_count = count;

    /* Fiddle with leading bits up to large block */
    if (((long)pdst & 0x1) && cur_count >= 1) {
		/* To next 0mod2 */
	*(unsigned char *) pdst &= tmpb;
	pdst++;
	cur_count--;
    }

    if (((long)pdst & 0x2) && cur_count >= 2) {
		/* To next 0mod4 */
	*(unsigned short *) pdst &= tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (((long)pdst & 0x4) && cur_count >= 4) {
		/* To next 0mod8 */
	*(unsigned int *) pdst &= tmpi;
	pdst += 4;
	cur_count -= 4;
    }

	/*
	 * Perform bulk copy, knowing 0mod8 alignment
	 */

    while (cur_count >= 8 * sizeof(long)) {

	/* Hand unrolled x8, assumes scheduler does a good job */
	*(unsigned long *) ((long) pdst + 0 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 1 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 2 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 3 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 4 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 5 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 6 * sizeof(long)) &= fill1;
	*(unsigned long *) ((long) pdst + 7 * sizeof(long)) &= fill1;

	pdst += 8 * sizeof(long);
	cur_count -= 8 * sizeof(long);
    }

	/* Perform trailing bits cleanup */
    while (cur_count >= sizeof(long)) {
	*(unsigned long *) ((long) pdst + 0) &= fill1;
	pdst += sizeof(long);
	cur_count -= sizeof(long);
    }

    if (cur_count >= 4) {
		/* On 0mod4 boundary already */
	*(unsigned int *) pdst &= tmpi;
	pdst += 4;
	cur_count -= 4;
    }

    if (cur_count >= 2) {
		/* On 0mod2 boundary already */
	*(unsigned short *) pdst &= tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (cur_count >= 1) {
		/* last possible byte */
	*(unsigned char *) pdst &= tmpb;
	pdst++;
	cur_count--;
    }

	/* Loop epilogue */
/* assert(cur_count == 0); */
    pdst += widthPitch;
    hcount--;
  }

  return pdst;
}

/* fFillOr.s */

unsigned char *fastFillSolidGXor(
    unsigned char *pdst,
    unsigned long fill1,
    unsigned long fill2,
    int hcount,
    int count,
    int w,
    int widthPitch
)
{
	/*
	 * NOTES: original assembly code presumes hcount > 0 to start with
	 *	New code assumes that all bytes of fill1, fill2 are
	 *	consistent.  i.e. 0xefefefef, and not 0x12345678.
	 *	This is because the caller of this routine does a PFILL()
	 *	of the [fill1, fill2] values before they get here.
	 *	For large block cases (count > 3), the original code
	 *	assumed that width == count.
	 *	Fills hcount trips of count bytes each trip through loop
	 */

  if (count == 0)
    return pdst;

  while (hcount > 0) {
	/* No special 'fast' cases here */
    int		cur_count;
    unsigned char	tmpb = fill1;
    unsigned short	tmph = fill1;
    unsigned int	tmpi = fill1;

    cur_count = count;

    /* Fiddle with leading bits up to large block */
    if (((long)pdst & 0x1) && cur_count >= 1) {
		/* To next 0mod2 */
	*(unsigned char *) pdst |= tmpb;
	pdst++;
	cur_count--;
    }

    if (((long)pdst & 0x2) && cur_count >= 2) {
		/* To next 0mod4 */
	*(unsigned short *) pdst |= tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (((long)pdst & 0x4) && cur_count >= 4) {
		/* To next 0mod8 */
	*(unsigned int *) pdst |= tmpi;
	pdst += 4;
	cur_count -= 4;
    }

	/*
	 * Perform bulk copy, knowing 0mod8 alignment
	 */

    while (cur_count >= 8 * sizeof(long)) {

	/* Hand unrolled x8, assumes scheduler does a good job */
	*(unsigned long *) ((long) pdst + 0 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 1 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 2 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 3 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 4 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 5 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 6 * sizeof(long)) |= fill1;
	*(unsigned long *) ((long) pdst + 7 * sizeof(long)) |= fill1;

	pdst += 8 * sizeof(long);
	cur_count -= 8 * sizeof(long);
    }

	/* Perform trailing bits cleanup */
    while (cur_count >= sizeof(long)) {
	*(unsigned long *) ((long) pdst + 0) |= fill1;
	pdst += sizeof(long);
	cur_count -= sizeof(long);
    }

    if (cur_count >= 4) {
		/* On 0mod4 boundary already */
	*(unsigned int *) pdst |= tmpi;
	pdst += 4;
	cur_count -= 4;
    }

    if (cur_count >= 2) {
		/* On 0mod2 boundary already */
	*(unsigned short *) pdst |= tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (cur_count >= 1) {
		/* last possible byte */
	*(unsigned char *) pdst |= tmpb;
	pdst++;
	cur_count--;
    }

	/* Loop epilogue */
/* assert(cur_count == 0); */
    pdst += widthPitch;
    hcount--;
  }

  return pdst;
}

/* fFillXor.s */

unsigned char *fastFillSolidGXxor(
    unsigned char *pdst,
    unsigned long fill1,
    unsigned long fill2,
    int hcount,
    int count,
    int w,
    int widthPitch
)
{
	/*
	 * NOTES: original assembly code presumes hcount > 0 to start with
	 *	New code assumes that all bytes of fill1, fill2 are
	 *	consistent.  i.e. 0xefefefef, and not 0x12345678.
	 *	This is because the caller of this routine does a PFILL()
	 *	of the [fill1, fill2] values before they get here.
	 *	For large block cases (count > 3), the original code
	 *	assumed that width == count.
	 *	Fills hcount trips of count bytes each trip through loop
	 */

  if (count == 0)
    return pdst;

  while (hcount > 0) {
	/* No special 'fast' cases here */
    int		cur_count;
    unsigned char	tmpb = fill1;
    unsigned short	tmph = fill1;
    unsigned int	tmpi = fill1;

    cur_count = count;

    /* Fiddle with leading bits up to large block */
    if (((long)pdst & 0x1) && cur_count >= 1) {
		/* To next 0mod2 */
	*(unsigned char *) pdst ^= tmpb;
	pdst++;
	cur_count--;
    }

    if (((long)pdst & 0x2) && cur_count >= 2) {
		/* To next 0mod4 */
	*(unsigned short *) pdst ^= tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (((long)pdst & 0x4) && cur_count >= 4) {
		/* To next 0mod8 */
	*(unsigned int *) pdst ^= tmpi;
	pdst += 4;
	cur_count -= 4;
    }

	/*
	 * Perform bulk copy, knowing 0mod8 alignment
	 */

    while (cur_count >= 8 * sizeof(long)) {

	/* Hand unrolled x8, assumes scheduler does a good job */
	*(unsigned long *) ((long) pdst + 0 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 1 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 2 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 3 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 4 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 5 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 6 * sizeof(long)) ^= fill1;
	*(unsigned long *) ((long) pdst + 7 * sizeof(long)) ^= fill1;

	pdst += 8 * sizeof(long);
	cur_count -= 8 * sizeof(long);
    }

	/* Perform trailing bits cleanup */
    while (cur_count >= sizeof(long)) {
	*(unsigned long *) ((long) pdst + 0) ^= fill1;
	pdst += sizeof(long);
	cur_count -= sizeof(long);
    }

    if (cur_count >= 4) {
		/* On 0mod4 boundary already */
	*(unsigned int *) pdst ^= tmpi;
	pdst += 4;
	cur_count -= 4;
    }

    if (cur_count >= 2) {
		/* On 0mod2 boundary already */
	*(unsigned short *) pdst ^= tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (cur_count >= 1) {
		/* last possible byte */
	*(unsigned char *) pdst ^= tmpb;
	pdst++;
	cur_count--;
    }

	/* Loop epilogue */
/* assert(cur_count == 0); */
    pdst += widthPitch;
    hcount--;
  }

  return pdst;
}

/* fFillCopy.s */

unsigned char *fastFillSolidGXcopy(
    unsigned char *pdst,
    unsigned long fill1,
    unsigned long fill2,
    int hcount,
    int count,
    int w,
    int widthPitch
)
{
	/*
	 * NOTES: original assembly code presumes hcount > 0 to start with
	 *	New code assumes that all bytes of fill1, fill2 are
	 *	consistent.  i.e. 0xefefefef, and not 0x12345678.
	 *	This is because the caller of this routine does a PFILL()
	 *	of the [fill1, fill2] values before they get here.
	 *	For large block cases (count > 3), the original code
	 *	assumed that width == count.
	 *	Fills hcount trips of count bytes each trip through loop
	 */

  if (count == 0)
    return pdst;

  while (hcount > 0) {
	/* No special 'fast' cases here */
    int		cur_count;
    unsigned char	tmpb = fill1;
    unsigned short	tmph = fill1;
    unsigned int	tmpi = fill1;

    cur_count = count;

    /* Fiddle with leading bits up to large block */
    if (((long)pdst & 0x1) && cur_count >= 1) {
		/* To next 0mod2 */
	*(unsigned char *) pdst = tmpb;
	pdst++;
	cur_count--;
    }

    if (((long)pdst & 0x2) && cur_count >= 2) {
		/* To next 0mod4 */
	*(unsigned short *) pdst = tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (((long)pdst & 0x4) && cur_count >= 4) {
		/* To next 0mod8 */
	*(unsigned int *) pdst = tmpi;
	pdst += 4;
	cur_count -= 4;
    }

	/*
	 * Perform bulk copy, knowing 0mod8 alignment
	 */

    while (cur_count >= 8 * sizeof(long)) {

	/* Hand unrolled x8, assumes scheduler does a good job */
	*(unsigned long *) ((long) pdst + 0 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 1 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 2 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 3 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 4 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 5 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 6 * sizeof(long)) = fill1;
	*(unsigned long *) ((long) pdst + 7 * sizeof(long)) = fill1;

	pdst += 8 * sizeof(long);
	cur_count -= 8 * sizeof(long);
    }

	/* Perform trailing bits cleanup */
    while (cur_count >= sizeof(long)) {
	*(unsigned long *) ((long) pdst + 0) = fill1;
	pdst += sizeof(long);
	cur_count -= sizeof(long);
    }

    if (cur_count >= 4) {
		/* On 0mod4 boundary already */
	*(unsigned int *) pdst = tmpi;
	pdst += 4;
	cur_count -= 4;
    }

    if (cur_count >= 2) {
		/* On 0mod2 boundary already */
	*(unsigned short *) pdst = tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (cur_count >= 1) {
		/* last possible byte */
	*(unsigned char *) pdst = tmpb;
	pdst++;
	cur_count--;
    }

	/* Loop epilogue */
/* assert(cur_count == 0); */
    pdst += widthPitch;
    hcount--;
  }

  return pdst;
}

/*
 * Reverse engineered version of XFree86 code for fFillSet.s by
 * Rick Gorton (gorton@tallis.enet.dec.com)
 * This version should work well on strongly aligned RISC architectures
 * in general.  In particular, the even-odd trip performance problem
 * with 'tribbleloop' is eliminated.
 *
 * Jay, please put the original header back in here...
 */

unsigned char *fastFillSolidGXset(
    unsigned char *pdst,
    unsigned long fill1,
    unsigned long fill2,
    int hcount,
    int count,
    int w,
    int widthPitch)
{
	/*
	 * NOTES: original assembly code presumes hcount > 0 to start with
	 *	New code assumes that all bytes of fill1, fill2 are
	 *	consistent.  i.e. 0xefefefef, and not 0x12345678.
	 *	This is because the caller of this routine does a PFILL()
	 *	of the [fill1, fill2] values before they get here.
	 *	For large block cases (count > 3), the original code
	 *	assumed that width == count.
	 *	Fills hcount trips of count bytes each trip through loop
	 */

  if (count == 0)
    return pdst;

  while (hcount > 0) {
	/* No special 'fast' cases here */
    int		cur_count;
    char	tmpb;
    short	tmph;
    int		tmpi;

    cur_count = count;

    /* Fiddle with leading bits up to large block */
    if (((long)pdst & 0x1) && cur_count >= 1) {
		/* To next 0mod2 */
	tmpb = *(unsigned char *) pdst;
	tmpb = (tmpb & fill1) ^ fill2;
	*(unsigned char *) pdst = tmpb;
	pdst++;
	cur_count--;
    }

    if (((long)pdst & 0x2) && cur_count >= 2) {
		/* To next 0mod4 */
	tmph = *(unsigned short *) pdst;
	tmph = (tmph & fill1) ^ fill2;
	*(unsigned short *) pdst = tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (((long)pdst & 0x4) && cur_count >= 4) {
		/* To next 0mod8 */
	tmpi = *(unsigned int *) pdst;
	tmpi = (tmpi & fill1) ^ fill2;
	*(unsigned int *) pdst = tmpi;
	pdst += 4;
	cur_count -= 4;
    }

	/*
	 * Perform bulk copy, knowing 0mod8 alignment
	 */

    while (cur_count >= 8 * sizeof(long)) {
	unsigned long	tmp_1, tmp_2, tmp_3, tmp_4;
	unsigned long	tmp_5, tmp_6, tmp_7, tmp_8;

	/* Hand unrolled x8, assumes scheduler does a good job */
	tmp_1 = *(unsigned long *) ((long) pdst + 0 * sizeof(long));
	tmp_2 = *(unsigned long *) ((long) pdst + 1 * sizeof(long));
	tmp_3 = *(unsigned long *) ((long) pdst + 2 * sizeof(long));
	tmp_4 = *(unsigned long *) ((long) pdst + 3 * sizeof(long));
	tmp_5 = *(unsigned long *) ((long) pdst + 4 * sizeof(long));
	tmp_6 = *(unsigned long *) ((long) pdst + 5 * sizeof(long));
	tmp_7 = *(unsigned long *) ((long) pdst + 6 * sizeof(long));
	tmp_8 = *(unsigned long *) ((long) pdst + 7 * sizeof(long));

	tmp_1 = (fill1 & tmp_1) ^ fill2;
	tmp_2 = (fill1 & tmp_2) ^ fill2;
	tmp_3 = (fill1 & tmp_3) ^ fill2;
	tmp_4 = (fill1 & tmp_4) ^ fill2;
	tmp_5 = (fill1 & tmp_5) ^ fill2;
	tmp_6 = (fill1 & tmp_6) ^ fill2;
	tmp_7 = (fill1 & tmp_7) ^ fill2;
	tmp_8 = (fill1 & tmp_8) ^ fill2;

	*(unsigned long *) ((long) pdst + 0 * sizeof(long)) = tmp_1;
	*(unsigned long *) ((long) pdst + 1 * sizeof(long)) = tmp_2;
	*(unsigned long *) ((long) pdst + 2 * sizeof(long)) = tmp_3;
	*(unsigned long *) ((long) pdst + 3 * sizeof(long)) = tmp_4;
	*(unsigned long *) ((long) pdst + 4 * sizeof(long)) = tmp_5;
	*(unsigned long *) ((long) pdst + 5 * sizeof(long)) = tmp_6;
	*(unsigned long *) ((long) pdst + 6 * sizeof(long)) = tmp_7;
	*(unsigned long *) ((long) pdst + 7 * sizeof(long)) = tmp_8;

	pdst += 8 * sizeof(long);
	cur_count -= 8 * sizeof(long);
    }

	/* Perform trailing bits cleanup */
    while (cur_count >= sizeof(long)) {
	unsigned long	tmpl;

	tmpl = *(unsigned long *) ((long) pdst + 0);
	tmpl = (tmpl & fill1) ^ fill2;
	*(unsigned long *) ((long) pdst + 0) = tmpl;
	pdst += sizeof(long);
	cur_count -= sizeof(long);
    }

    if (cur_count >= 4) {
		/* On 0mod4 boundary already */
	tmpi = *(unsigned int *) pdst;
	tmpi = (tmpi & fill1) ^ fill2;
	*(unsigned int *) pdst = tmpi;
	pdst += 4;
	cur_count -= 4;
    }

    if (cur_count >= 2) {
		/* On 0mod2 boundary already */
	tmph = *(unsigned short *) pdst;
	tmph = (tmph & fill1) ^ fill2;
	*(unsigned short *) pdst = tmph;
	pdst += 2;
	cur_count -= 2;
    }

    if (cur_count >= 1) {
		/* last possible byte */
	tmpb = *(unsigned char *) pdst;
	tmpb = (tmpb & fill1) ^ fill2;
	*(unsigned char *) pdst = tmpb;
	pdst++;
	cur_count--;
    }

	/* Loop epilogue */
/* assert(cur_count == 0); */
    pdst += widthPitch;
    hcount--;
  }

  return pdst;
}

