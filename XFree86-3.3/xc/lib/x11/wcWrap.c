/* $XConsortium: wcWrap.c,v 11.6 94/04/17 20:22:21 rws Exp $ */
/*

Copyright (c) 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * Copyright 1991 by the Open Software Foundation
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Open Software Foundation
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Open Software
 * Foundation makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * OPEN SOFTWARE FOUNDATION DISCLAIMS ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL OPEN SOFTWARE FOUNDATIONN BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 *		 M. Collins		OSF  
 */				

#include "Xlibint.h"
#include "Xlcint.h"

void
XwcDrawText(dpy, d, gc, x, y, text_items, nitems)
    Display            *dpy;
    Drawable            d;
    GC                  gc;
    int                 x, y;
    XwcTextItem        *text_items;
    int                 nitems;
{
    register XFontSet fs;
    register XwcTextItem *p = text_items;
    register int i = nitems;
    register int esc;

    /* ignore leading items with no fontset */
    while (i && !p->font_set) {
	i--;
	p++;
    }

    for (; --i >= 0; p++) {
	if (p->font_set)
	    fs = p->font_set;
	x += p->delta;
	esc = (*fs->methods->wc_draw_string) (dpy, d, fs, gc, x, y,
					      p->chars, p->nchars);
	if (!esc)
	    esc = fs->methods->wc_escapement (fs, p->chars, p->nchars);
	x += esc;
    }
}

#if NeedFunctionPrototypes
void
XwcDrawString(
    Display            *dpy,
    Drawable            d,
    XFontSet            font_set,
    GC                  gc,
    int                 x,
    int                 y,
    _Xconst wchar_t    *text,
    int                 text_len)
#else
void
XwcDrawString(dpy, d, font_set, gc, x, y, text, text_len)
    Display            *dpy;
    Drawable            d;
    XFontSet            font_set;
    GC                  gc;
    int                 x, y;
    wchar_t            *text;
    int                 text_len;
#endif
{
    (void)(*font_set->methods->wc_draw_string) (dpy, d, font_set, gc, x, y,
						text, text_len);
}

#if NeedFunctionPrototypes
void
XwcDrawImageString(
    Display            *dpy,
    Drawable            d,
    XFontSet            font_set,
    GC                  gc,
    int                 x,
    int                 y,
    _Xconst wchar_t    *text,
    int                 text_len)
#else
void
XwcDrawImageString(dpy, d, font_set, gc, x, y, text, text_len)
    Display            *dpy;
    Drawable            d;
    XFontSet            font_set;
    GC                  gc;
    int                 x, y;
    wchar_t            *text;
    int                 text_len;
#endif
{
    (*font_set->methods->wc_draw_image_string) (dpy, d, font_set, gc, x, y,
						text, text_len);
}

#if NeedFunctionPrototypes
int 
XwcTextEscapement(
    XFontSet            font_set,
    _Xconst wchar_t    *text,
    int                 text_len)
#else
int 
XwcTextEscapement(font_set, text, text_len)
    XFontSet            font_set;
    wchar_t            *text;
    int                 text_len;
#endif
{
    return (*font_set->methods->wc_escapement) (font_set, text, text_len);
}

#if NeedFunctionPrototypes
int
XwcTextExtents(
    XFontSet            font_set,
    _Xconst wchar_t    *text,
    int                 text_len,
    XRectangle         *overall_ink_extents,
    XRectangle         *overall_logical_extents)
#else
int
XwcTextExtents(font_set, text, text_len,
	       overall_ink_extents, overall_logical_extents)
    XFontSet            font_set;
    wchar_t            *text;
    int                 text_len;
    XRectangle         *overall_ink_extents;
    XRectangle         *overall_logical_extents;
#endif
{
    return (*font_set->methods->wc_extents) (font_set, text, text_len,
					     overall_ink_extents,
					     overall_logical_extents);
}

#if NeedFunctionPrototypes
Status
XwcTextPerCharExtents(
    XFontSet            font_set,
    _Xconst wchar_t    *text,
    int                 text_len,
    XRectangle         *ink_extents_buffer,
    XRectangle         *logical_extents_buffer,
    int                 buffer_size,
    int                *num_chars,
    XRectangle         *max_ink_extents,
    XRectangle         *max_logical_extents)
#else
Status
XwcTextPerCharExtents(font_set, text, text_len,
		      ink_extents_buffer, logical_extents_buffer,
		      buffer_size, num_chars,
		      max_ink_extents, max_logical_extents)
    XFontSet            font_set;
    wchar_t            *text;
    int                 text_len;
    XRectangle         *ink_extents_buffer;
    XRectangle         *logical_extents_buffer;
    int                 buffer_size;
    int                *num_chars;
    XRectangle         *max_ink_extents;
    XRectangle         *max_logical_extents;
#endif
{
    return (*font_set->methods->wc_extents_per_char) 
	      (font_set, text, text_len, 
	       ink_extents_buffer, logical_extents_buffer,
	       buffer_size, num_chars, max_ink_extents, max_logical_extents);
}
