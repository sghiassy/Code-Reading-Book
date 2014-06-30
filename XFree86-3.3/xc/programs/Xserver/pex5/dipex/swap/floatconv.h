/* $XConsortium: floatconv.h,v 5.5 91/12/30 18:23:15 hersh Exp $ */

/*
 * Copyright (c) 1988-1991 by Sun Microsystems and the X Consortium.
 */

/* 
 * floatconv.h - Functions and macros to convert between FloatTypes
 * 
 * Copyright 1988
 * Center for Information Technology Integration (CITI)
 * Information Technology Division
 * University of Michigan
 * Ann Arbor, Michigan
 *
 *                         All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that copyright notice and this permission
 * notice appear in supporting documentation, and that the names of
 * CITI or THE UNIVERSITY OF MICHIGAN not be used in advertising or
 * publicity pertaining to distribution of the software without
 * specific, written prior permission.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS." CITI AND THE UNIVERSITY OF
 * MICHIGAN DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
 * NO EVENT SHALL CITI OR THE UNIVERSITY OF MICHIGAN BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
#ifndef FLOAT_CON_H
#define FLOAT_CON_H

#include <X11/Xmd.h>
#include "X.h"
#include "Xproto.h"
#include "dipex.h"

#ifdef vax			/* probably not good enough yet */
#define MyFloatType DEC_F_Floating
#else
#define MyFloatType Ieee_754_32
#endif

extern void ConvertIEEEtoVax();
extern void ConvertVaxToIEEE();

#endif /* FLOAT_CON_H */
