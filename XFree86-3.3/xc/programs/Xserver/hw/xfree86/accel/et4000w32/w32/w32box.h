/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/w32box.h,v 3.8 1996/12/23 06:35:23 dawes Exp $ */ 
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
glennlai@netcom.com
4/6/94
8/14/96
*******************************************************************************/
/* $XConsortium: w32box.h /main/7 1996/10/23 18:41:15 kaleb $ */

#ifndef W32_BOX_H
#define W32_BOX_H

#include "w32.h"

/* Paint a solid box */
#define W32_INIT_BOX(OP, MASK, COLOR, DST_OFFSET) \
{ \
    if (MASK == 0xffffffff) \
	*ACL_FOREGROUND_RASTER_OPERATION	= W32OpTable[OP]; \
    else \
    { /* w32p only */ \
	*ACL_FOREGROUND_RASTER_OPERATION	= \
	    (0xf0 & W32OpTable[OP]) | 0x0a; \
	*ACL_PATTERN_WRAP			= 0x02; \
	*ACL_PATTERN_Y_OFFSET			= 0x3; \
	*ACL_PATTERN_ADDRESS			= W32Pattern; \
	*MBP0 					= W32Pattern; \
	*(LongP)W32Buffer 			= MASK; \
    } \
    *ACL_DESTINATION_Y_OFFSET		= DST_OFFSET; \
    *ACL_SOURCE_ADDRESS			= W32Foreground; \
    *MBP0				= W32Foreground; \
    *(LongP)W32Buffer			= COLOR; \
    if (W32OrW32i) \
    { \
	*(LongP)(W32Buffer + 4)		= COLOR; \
	*ACL_SOURCE_WRAP		= 0x12; \
	*ACL_SOURCE_Y_OFFSET		= 0x3; \
    } \
    else /* w32p */ \
	*ACL_SOURCE_WRAP		= 0x02; \
    if (W32et6000) \
	*ACL_MIX_CONTROL                = 0x33; \
    else \
	*ACL_ROUTING_CONTROL            = 0x00; \
    *ACL_XY_DIRECTION			= 0; \
}


#define W32_INIT_HLINE(OP, MASK, COLOR) \
	W32_INIT_BOX(OP, MASK, COLOR, 0) \
	*ACL_Y_COUNT = 0;


#define W32_HLINE(DST, X) \
{ \
    if ((X) != 0) \
    { \
	*ACL_X_COUNT = (X - 1) * (PSZ >> 3); \
	START_ACL(DST) \
    } \
}

#define W32_BOX(DST, X, Y) \
{ \
    if (((X) | (Y)) != 0) \
    { \
	SET_XY(X, Y) \
	START_ACL(DST) \
    } \
}

#endif /* W32_BOX_H */
