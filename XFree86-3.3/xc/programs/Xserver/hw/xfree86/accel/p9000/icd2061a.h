/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/ICD2061A.h,v 3.4 1996/12/23 06:40:26 dawes Exp $ */
#ifndef DEBUG
/* #define DEBUG */
#endif

/* Based on the number 9 Inc code */
/* Copyright (c) 1992, Number Nine Computer Corp.  All Rights Reserved.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Number Nine Computer Corp not be used 
 * in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  Number Nine Computer Corp 
 * makes no representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * NUMBER NINE COMPUTER CORP DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL NUMBER NINE COMPUTER CORP BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modifications - Copyright (c) 1994, Harry Langenbacher,All Rights Reserved. 
 * HARRY LANGENBACHER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL HARRY LANGENBACHER BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Harry Langenbacher 
 * makes no representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty.
 */
/* $XConsortium: ICD2061A.h /main/4 1996/02/21 17:31:41 kaleb $ */

extern unsigned long ICD2061ACalcClock (
#if NeedFunctionPrototypes
   long, int
#endif
);

extern int ICD2061AGCD (
#if NeedFunctionPrototypes
   int, int
#endif
);

extern void ICD2061ASetClock (
#if NeedFunctionPrototypes
   unsigned long 
#endif
);

extern unsigned int ICD2061AGetClock (
#if NeedFunctionPrototypes
   unsigned long
#endif
);

extern unsigned long CalcVCOfreq (
#if NeedFunctionPrototypes
   unsigned int, unsigned int
#endif
);


#define CRYSTAL_FREQUENCY       (14318180 * 2)
#define MIN_VCO_FREQUENCY       50000000              /* 50 Mhz */
#define NUM_PROGRAMMABLE_CLOCKS 3
