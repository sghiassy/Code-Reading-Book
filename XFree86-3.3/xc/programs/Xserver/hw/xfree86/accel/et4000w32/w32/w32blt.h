/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/w32blt.h,v 3.7 1996/12/23 06:35:22 dawes Exp $ */  
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
4/6/94
*******************************************************************************/
/* $XConsortium: w32blt.h /main/7 1996/10/23 18:41:06 kaleb $ */

#ifndef W32BLT_H
#define W32BLT_H

#include "w32.h"


/* initizliae a bitblt operation */
#define W32_INIT_WINWIN(OP, MASK, SRC_OFFSET, DST_OFFSET, XDIR, YDIR) \
{ \
    int i; \
\
    *ACL_SOURCE_WRAP	= 0x77; \
    if (MASK == 0xffffffff) \
	*ACL_FOREGROUND_RASTER_OPERATION	= W32OpTable[OP]; \
    else \
    { \
	*ACL_FOREGROUND_RASTER_OPERATION	= \
	    (0xf0 & W32OpTable[OP]) | 0x0a; \
	*ACL_PATTERN_ADDRESS			= W32Pattern; \
	*MBP0 					= W32Pattern; \
	*(LongP)W32Buffer 			= MASK; \
	if (W32p || W32et6000) \
	    *ACL_PATTERN_WRAP			= 0x02; \
	else \
	{ \
	    *(LongP)(W32Buffer + 4)		= MASK; \
	    *ACL_PATTERN_WRAP			= 0x12; \
	    *ACL_PATTERN_Y_OFFSET		= 0x3; \
	} \
    } \
    i = 0; \
    if (XDIR == -1) i |= 0x1; \
    if (YDIR == -1) i |= 0x2; \
    *ACL_XY_DIRECTION			= i; \
    *ACL_DESTINATION_Y_OFFSET		= DST_OFFSET; \
    *ACL_SOURCE_Y_OFFSET		= SRC_OFFSET; \
    if (W32et6000) \
	*ACL_MIX_CONTROL                = 0x33; \
    else \
	*ACL_ROUTING_CONTROL            = 0x00; \
}


#define W32_WINWIN(SRC, DST, X, Y) \
if (((X) | (Y) != 0)) \
{ \
    SET_XY(X, Y) \
    *(ACL_SOURCE_ADDRESS) = SRC; \
    START_ACL(DST) \
}


#define W32_INIT_PIXWIN(OP, MASK, DST_OFFSET) \
{ \
    if (MASK == 0xffffffff) \
	*ACL_FOREGROUND_RASTER_OPERATION	= W32OpTable[OP]; \
    else \
    { \
	*ACL_FOREGROUND_RASTER_OPERATION	= \
	    (0xf0 & W32OpTable[OP]) | 0x0a; \
	*ACL_PATTERN_ADDRESS			= W32Pattern; \
	*MBP0 					= W32Pattern; \
	*(LongP)W32Buffer 			= MASK; \
	if (W32p || W32et6000) \
	    *ACL_PATTERN_WRAP			= 0x02; \
	else \
	{ \
	    *(LongP)(W32Buffer + 4)		= MASK; \
	    *ACL_PATTERN_WRAP			= 0x12; \
	    *ACL_PATTERN_Y_OFFSET		= 0x3; \
	    *ACL_VIRTUAL_BUS_SIZE		= 0x2; \
	} \
    } \
    *ACL_XY_DIRECTION			= 0; \
    *ACL_DESTINATION_Y_OFFSET		= DST_OFFSET; \
    *ACL_ROUTING_CONTROL 		= 0x1; \
}


#define W32_PIXWIN(SRC, DST, X, Y) \
if (((X) | (Y) != 0)) \
{ \
    SET_XY(X, Y) \
    W32InitPixWin(SRC, X); \
    START_ACL(DST) \
    W32PixWin(SRC, Y); \
}


#endif /* W32BLT_H */
