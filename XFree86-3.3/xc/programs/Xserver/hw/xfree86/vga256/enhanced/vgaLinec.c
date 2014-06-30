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

/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/enhanced/vgaLinec.c,v 3.0 1996/12/09 11:54:31 dawes Exp $ */

#include "vga256.h"
#include "miline.h"
#include "cfbrrop.h"

/*
 *  These implementations are guaranteed to be slow, as they don't special
 *  case GXcopy and GXxor and because they do a bank-change test on every
 *  pixel.  On the other hand, they do work, which is more than can be
 *  said for the previous versions (which just called into cfb and took
 *  no account of banking).
 *
 *  I expect that these implementations will only be used while people are
 *  working on accelerated versions for specific chips, and so probably
 *  won't get around to doing more efficient versions anytime soon.  If
 *  anyone else wants to take a crack at better versions, by all means do!
 *
 *  Jay Kistler, Digital Equipment Corporation
 */

/* vgabres.s */
void fastvga256BresS(
    int            alu,
    unsigned long  and,
    unsigned long  xor,
    unsigned long *addrl,
    int            nlwidth,
    register int   signdx,
    int            signdy,
    int            axis,
    int            x,
    int            y,
    register int   e,
    register int   e1,
    int            e2,
    int            len
)
{
    register int	e3 = e2-e1;
    register PixelType	*addrp;		/* Pixel pointer */

    if (len == 0)
    	return;

    /* point to first point */
    nlwidth <<= PWSH;
    addrp = (PixelType *)(addrl) + (y * nlwidth) + x;
    if (signdy < 0)
    	nlwidth = -nlwidth;
    e = e-e1;			/* to make looping easier */
    
    BANK_FLAG(addrp);

    if (axis == Y_AXIS)
    {
	int	t;

	t = nlwidth;
	nlwidth = signdx;
	signdx = t;
    }

    SETW(addrp);

    while(len--)
    { 
	*addrp = DoRRop (*addrp, and, xor);
	e += e1;
	if (e >= 0)
	{
	    addrp += nlwidth; CHECKWO(addrp); CHECKWU(addrp);
	    e += e3;
	}
	addrp += signdx; CHECKWO(addrp); CHECKWU(addrp);
    }
}

/* vgalineH.s */
int fastvga256HorzS(
    int                     alu,
    unsigned long           and,
    register unsigned long  xor,
    register unsigned long *addrl,
    int                     nlwidth,
    int                     x,
    int                     y,
    int                     len
)
{
    register int nlmiddle;
    register unsigned long startmask;
    register unsigned long endmask;

    addrl = addrl + (y * nlwidth) + (x >> PWSH);

    BANK_FLAG(addrl);

    if (((x & PIM) + len) <= PPW)
    {
	maskpartialbits(x, len, startmask);
	endmask = 0;
	nlmiddle = 0;
    }
    else
    {
	maskbits (x, len, startmask, endmask, nlmiddle);
    }

    SETW(addrl);

    if (startmask)
    {
	*addrl = DoMaskRRop (*addrl, and, xor, startmask);
	addrl++; CHECKWO(addrl);
    }

    while (nlmiddle--)
    {
	*addrl = DoRRop (*addrl, and, xor);
	addrl++; CHECKWO(addrl);
    }

    if (endmask)
	*addrl = DoMaskRRop (*addrl, and, xor, endmask);
}

/* vgalineV.s */
void fastvga256VertS(
    int            alu,
    unsigned long  and,
    unsigned long  xor,
    unsigned long *addrl,
    int            nlwidth,
    int            x,
    int            y,
    register int   len
)
{
    unsigned char *bits = (unsigned char *)addrl;

    BANK_FLAG(bits);

    nlwidth <<= PWSH;
    bits = bits + (y * nlwidth) + x;

    SETW(bits);
    while (len--) {
	*bits = (*bits & and) ^ xor;
	bits += nlwidth; CHECKWO(bits);
    }
}
