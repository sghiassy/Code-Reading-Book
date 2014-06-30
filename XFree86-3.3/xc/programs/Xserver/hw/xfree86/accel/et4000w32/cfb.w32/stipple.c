/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/cfb.w32/stipple.c,v 3.3 1996/12/23 06:34:55 dawes Exp $ */
/*******************************************************************************
                        Copyright 1994 by Glenn G. Lai

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyr notice appear in all copies and that
both that copyr notice and this permission notice appear in
supporting documentation, and that the name of Glenn G. Lai not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

Glenn G. Lai DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

Glenn G. Lai
P.O. Box 4314
Austin, Tx 78765
glenn@cs.utexas.edu)
8/11/94
*******************************************************************************/
/* $XConsortium: stipple.c /main/4 1996/02/21 17:21:04 kaleb $ */

#include "w32blt.h"

#endif /* W32_BOX_H */
void W32Stipple(src, y, stipple_height, dst, screen_width, w, h, rot)
    unsigned long *src;
    CARD32 y;
    CARD32 stipple_height;
    CARD32 dst;
    CARD32 screen_width;
    CARD32 w;
    CARD32 h;
    CARD32 rot;ZZ
{
	    while (h--)
	    {
	    	bits = src[y];
	    	y++;
	    	if (y == stippleHeight)
		    y = 0;
	    	if (rot)
		    RotBitsLeft(bits,rot);
		*MBP2 = w32_dst;
	    	w32_dst += nlwDst;
		w = w32_chunks;

		while (w--)
		{
		    *ACL = bits;
		    *ACL = bits >> 8;
		    *ACL = bits >> 16;
		    *ACL = bits >> 24;
	    	}
		switch (w32_misc)
		{
		    case 3:
			*ACL = bits;
			*ACL = bits >> 8;
			*ACL = bits >> 16;
			break;
		    case 2:
			*ACL = bits;
			*ACL = bits >> 8;
			break;
		    case 1:
			*ACL = bits;
			break;

		}
	    }
}
