/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/w32itext.h,v 3.7 1996/12/23 06:35:25 dawes Exp $ */ 
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
/* $XConsortium: w32itext.h /main/5 1996/10/19 17:51:42 kaleb $ */

#ifndef W32_ITEXT_H
#define W32_ITEXT_H

#include "w32.h"

/* the following needs some polishing */
void W32ImageText1();
void W32ImageText2();
void W32ImageText3();
void W32ImageText4();

void W32pImageText1();
void W32pImageText2();
void W32pImageText3();
void W32pImageText4();

#define W32_INIT_IMAGE_TEXT(FOREGROUND, BACKGROUND, DST_OFFSET, X, Y) \
{ \
    SET_XY(X, Y) \
    *ACL_FOREGROUND_RASTER_OPERATION	= 0xcc; \
    *ACL_BACKGROUND_RASTER_OPERATION	= 0xf0; \
    *ACL_ROUTING_CONTROL		= 0x2; \
    *ACL_VIRTUAL_BUS_SIZE		= 0x0; \
    *ACL_XY_DIRECTION			= 0; \
    *ACL_DESTINATION_Y_OFFSET		= DST_OFFSET; \
    *ACL_SOURCE_ADDRESS			= W32Foreground; \
    *MBP0 				= W32Foreground; \
    *ACL_PATTERN_ADDRESS		= W32Background; \
    *(LongP)W32Buffer	 		= FOREGROUND; \
    *(LongP)(W32Buffer + 4) 		= FOREGROUND; \
    *ACL_SOURCE_WRAP			= 0x12; \
    *ACL_SOURCE_Y_OFFSET		= 0x3; \
    *ACL_PATTERN_WRAP			= 0x12; \
    *ACL_PATTERN_Y_OFFSET		= 0x3; \
    *MBP0 				= W32Background; \
    *(LongP)W32Buffer	 		= BACKGROUND; \
    *(LongP)(W32Buffer + 4) 		= BACKGROUND; \
}

#define W32P_INIT_IMAGE_TEXT(FOREGROUND, BACKGROUND, DST_OFFSET, X, Y) \
{ \
    SET_XY(X, Y) \
    *ACL_FOREGROUND_RASTER_OPERATION	= 0xcc; \
    *ACL_BACKGROUND_RASTER_OPERATION	= 0xf0; \
    if (W32et6000) \
      *ACL_MIX_CONTROL			= 0x32; \
    else \
      *ACL_ROUTING_CONTROL		= 0x02; \
    *ACL_XY_DIRECTION			= 0; \
    *ACL_DESTINATION_Y_OFFSET		= DST_OFFSET; \
    *ACL_SOURCE_ADDRESS			= W32Foreground; \
    *MBP0 				= W32Foreground; \
    *(LongP)W32Buffer	 		= FOREGROUND; \
    *ACL_SOURCE_WRAP			= 0x02; \
    *ACL_PATTERN_WRAP			= 0x02; \
    *ACL_PATTERN_ADDRESS		= W32Background; \
    *MBP0 				= W32Background; \
    *(LongP)W32Buffer	 		= BACKGROUND; \
    *ACL_MIX_ADDRESS			= 0; \
    *ACL_MIX_Y_OFFSET 			= 31; \
}


#endif /* W32_ITEXT_H */
