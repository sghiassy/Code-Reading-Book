/* $XConsortium: Cursor.h,v 1.1 93/07/12 15:28:04 rws Exp $ */
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

#ifndef XNESTCURSOR_H
#define XNESTCURSOR_H

typedef struct {
  Cursor cursor;
} xnestPrivCursor;

#define xnestCursorPriv(pCursor, pScreen) \
  ((xnestPrivCursor *)((pCursor)->devPriv[pScreen->myNum]))

#define xnestCursor(pCursor, pScreen) \
  (xnestCursorPriv(pCursor, pScreen)->cursor)

void xnestConstrainCursor();
void xnestCursorLimits();
Bool xnestDisplayCursor();
Bool xnestRealizeCursor();
Bool xnestUnrealizeCursor();
void xnestRecolorCursor();
Bool xnestSetCursorPosition();

#endif /* XNESTCURSOR_H */
