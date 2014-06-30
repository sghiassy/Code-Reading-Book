/* $XConsortium: Drawable.h /main/2 1996/12/02 10:20:59 lehors $ */
/*

Copyright 1993 by Davor Matic

Permission to use, copy, modify, distribute, and sell this software
and its documentation for any purpose is hereby granted without fee,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation.  Davor Matic makes no representations about
the suitability of this software for any purpose.  It is provided "as
is" without express or implied warranty.

*/

#ifndef XNESTDRAWABLE_H
#define XNESTDRAWABLE_H

#include "XNWindow.h"
#include "Pixmap.h"

#define xnestDrawable(pDrawable) \
  ((pDrawable)->type == DRAWABLE_WINDOW ? \
   xnestWindow((WindowPtr)pDrawable) : \
   xnestPixmap((PixmapPtr)pDrawable))

#endif /* XNESTDRAWABLE_H */
