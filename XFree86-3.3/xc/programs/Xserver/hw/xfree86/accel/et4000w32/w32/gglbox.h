/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/et4000w32/w32/gglbox.h,v 3.4 1996/12/23 06:35:16 dawes Exp $ */
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
/* $XConsortium: gglbox.h /main/5 1996/02/21 17:21:54 kaleb $ */

{
    CARD32 ggl_cnt, ggl_ptr;
    ggl_cnt = GGL_COUNT;
    ggl_ptr = (CARD32)GGL_PTR;
    while (ggl_cnt != 0)
    {
	GGL_PTR = (void*)ggl_ptr;
	GGL_SET(GGL_PTR)
	ggl_ptr += W32BoxHop;
	GGL_COUNT = ggl_cnt <= W32BoxCount ? ggl_cnt : W32BoxCount;
	ggl_cnt -= GGL_COUNT;
	GGL_DRAW
    }
}
