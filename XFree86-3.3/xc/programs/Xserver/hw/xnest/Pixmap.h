/* $XConsortium: Pixmap.h,v 1.1 93/07/12 15:28:41 rws Exp $ */
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

#ifndef XNESTPIXMAP_H
#define XNESTPIXMAP_H

typedef struct {
  Pixmap pixmap;
} xnestPrivPixmap;

#define xnestPixmapPriv(pPixmap) \
  ((xnestPrivPixmap *)((pPixmap)->devPrivate.ptr))

#define xnestPixmap(pPixmap) (xnestPixmapPriv(pPixmap)->pixmap)

#define xnestSharePixmap(pPixmap) ((pPixmap)->refcnt++)

PixmapPtr xnestCreatePixmap();
Bool xnestDestroyPixmap();
RegionPtr xnestPixmapToRegion();

#endif /* XNESTPIXMAP_H */
