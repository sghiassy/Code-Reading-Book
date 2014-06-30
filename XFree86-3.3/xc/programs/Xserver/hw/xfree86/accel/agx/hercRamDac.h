/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/hercRamDac.h,v 3.4 1996/12/23 06:33:05 dawes Exp $ */
/*
 * Copyright 1994 by Henry Worth <haw30@eng.amdahl.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry A. Worht not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * HENRY A. WORTH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: hercRamDac.h /main/5 1996/02/21 17:18:46 kaleb $ */

#include "compiler.h"
#include <X11/Xfuncproto.h>

extern unsigned int  hercBrdIO;
extern Bool hercSmallDAC;
extern Bool hercBigDAC;
extern unsigned int  hercMaxClockSmallDac;
extern unsigned int  hercMinClockBigDac;
extern unsigned int  hercMaxClockBigDac;

_XFUNCPROTOBEGIN

extern void hercOutRamDacReg(
#if NeedFunctionPrototypes
	unsigned short,
	unsigned char,
	unsigned char
#endif
);

extern void hercOutRamDacData(
#if NeedFunctionPrototypes
	unsigned short,
	unsigned char
#endif
);

extern unsigned char hercInRamDacReg(
#if NeedFunctionPrototypes
	unsigned short
#endif
);

void
hercProbeRamDac(
#if NeedFunctionPrototypes
	void	
#endif
);

void
hercSwitchToBigDac(
#if NeedFunctionPrototypes
      Bool
#endif
);

void
hercPowerDownBigDac(
#if NeedFunctionPrototypes
      void
#endif
);

void
hercSwitchToLittleDac(
#if NeedFunctionPrototypes
      void
#endif
);

void hercAddDoubledClocks(
#if NeedFunctionPrototypes
    ScrnInfoRec *
#endif
);

void hercValidateClocks(
#if NeedFunctionPrototypes
    ScrnInfoRec *
#endif
);



_XFUNCPROTOEND
