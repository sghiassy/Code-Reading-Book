/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/mach32/ativga.h,v 3.1 1996/12/23 06:38:19 dawes Exp $ */

/***************************************************************************
 * Start of VGA font saving and restoration code.
 * Created: Sun Jun 27 12:50:09 1993 by faith@cs.unc.edu
 *
 * Reference used:
 *   "VGA WONDER Programmer's Reference,"
 *      ATI Technologies, 1991.
 *      Release 1.2 -- Reference #PRG28800
 *      (Part No. 10709B0412)
 *
 * Some code from xf86/vga256/drivers/ati/driver.c, which was written by:
 * Thomas Roell (roell@informatik.tu-muenchen.de), Per Lindqvist
 * (pgd@compuram.bbt.se), Doug Evans (dje@sspiff.UUCP), and Rik Faith
 * (faith@cs.unc.edu).
 *
 * Copyright 1990,91 by Thomas Roell, Dinkelscherben, Germany.
 * Copyright 1993 by Rickard E. Faith
 * Copyright 1993 by Kevin E. Martin, Chapel Hill, North Carolina.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Thomas Roell not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Thomas Roell makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * THOMAS ROELL, PER LINDQVIST, DOUG EVANS, RICKARD E. FAITH AND KEVIN
 * E. MARTIN DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THOMAS ROELL, PER LINDQVIST, DOUG EVANS, RICKARD E. FAITH
 * OR KEVIN E. MARTIN BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 *
 * Modified for the Mach32 by Kevin E. Martin (martin@cs.unc.edu)
 */
/* $XConsortium: ativga.h /main/3 1996/02/21 17:25:53 kaleb $ */

#include <X11/Xfuncproto.h>

_XFUNCPROTOBEGIN

extern void mach32SaveVGAInfo(
#if NeedFunctionPrototypes
  int
#endif
);
extern void mach32RestoreVGAInfo(
#if NeedFunctionPrototypes
  void
#endif
);

_XFUNCPROTOEND
