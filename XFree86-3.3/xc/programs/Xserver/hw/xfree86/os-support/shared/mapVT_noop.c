/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/shared/mapVT_noop.c,v 3.1 1996/12/23 06:51:00 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@XFree86.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: mapVT_noop.c /main/3 1996/02/21 17:53:42 kaleb $ */

/*
 * These routines are currently only needed for OSs with true USL-style VTs.
 * All other OSs get no-ops.
 */

/* ARGSUSED */
void xf86MapDisplay(ScreenNum, Region)
int ScreenNum;
int Region;
{
        return;
}

/* ARGSUSED */
void xf86UnMapDisplay(ScreenNum, Region)
int ScreenNum;
int Region;
{
        return;
}
