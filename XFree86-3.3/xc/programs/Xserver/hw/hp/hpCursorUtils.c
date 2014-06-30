/* $XConsortium: hpCursorUtils.c,v 1.3 95/01/24 01:43:05 dpw Exp $ */
/*************************************************************************
 * 
 * (c)Copyright 1992 Hewlett-Packard Co.,  All Rights Reserved.
 * 
 *                          RESTRICTED RIGHTS LEGEND
 * Use, duplication, or disclosure by the U.S. Government is subject to
 * restrictions as set forth in sub-paragraph (c)(1)(ii) of the Rights in
 * Technical Data and Computer Software clause in DFARS 252.227-7013.
 * 
 *                          Hewlett-Packard Company
 *                          3000 Hanover Street
 *                          Palo Alto, CA 94304 U.S.A.
 * 
 * Rights for non-DOD U.S. Government Departments and Agencies are as set
 * forth in FAR 52.227-19(c)(1,2).
 *
 *************************************************************************/

/*''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''
Copyright (c) 1988 by Hewlett-Packard Company
Copyright (c) 1987, 1988 by Digital Equipment Corporation, Maynard, 
              Massachusetts

Permission to use, copy, modify, and distribute this software 
and its documentation for any purpose and without fee is hereby 
granted, provided that the above copyright notice appear in all 
copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the names of 
Hewlett-Packard or Digital not be used in advertising or 
publicity pertaining to distribution of the software without specific, 
written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.


Copyright (c) 1987, 1988  X Consortium

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

'''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''''*/


#include "hpext.h"
#include "hildef.h"
#include "scrnintstr.h"
#include "cursorstr.h"
#include "regionstr.h"
#include "inputstr.h"
#include "opaque.h"
#include "hppriv.h"

extern int hpActiveScreen;		/* Stacked mode, >1 head */
extern WindowPtr *WindowTable;		/* Defined by DIX */

static CursorPtr currentCursors[MAXSCREENS];

void hpBlottoCursors()
{
  int j;
  for (j = MAXSCREENS; j--; ) currentCursors[j] = NULL;
}

/************************************************************
 * hpCursorLimits
 *      Return a box within which the given cursor may move on the given
 *      screen. We assume that the HotBox is actually on the given screen,
 *      since dix knows that size.
 *
 * Results:
 *      A box for the hot spot corner of the cursor.
 ************************************************************/

void
hpCursorLimits( pScreen, pCursor, pHotBox, pResultBox)
ScreenPtr pScreen;       /* Screen on which limits are desired */
CursorPtr pCursor;       /* Cursor whose limits are desired */
BoxPtr    pHotBox;       /* Limits for pCursor's hot point */
BoxPtr    pResultBox;    /* RETURN: limits for hot spot */
{
    *pResultBox = *pHotBox;
    pResultBox->x2 = min(pResultBox->x2,pScreen->width);
    pResultBox->y2 = min(pResultBox->y2,pScreen->height);
}

/************************************************************
 *
 * hpSetCursorPosition
 *
 *      This routine is called from DIX when the X11 sprite/cursor is warped.
 *
 ************************************************************/

Bool 
hpSetCursorPosition(pScreen, xhot, yhot, generateEvent)
ScreenPtr pScreen;
short	  xhot;
short	  yhot;
Bool	  generateEvent;
{
    HPInputDevice	*InDev;			/* Input device structure */
    hpPrivPtr		php;			/* XOS private structure */

    php 	= (hpPrivPtr) pScreen->devPrivate;

    /* Must Update the Input Driver's Variables: */
    InDev = GET_HPINPUTDEVICE((DeviceIntPtr)LookupPointerDevice());
    InDev->pScreen = pScreen;
    InDev->coords[0] = xhot;
    InDev->coords[1] = yhot;

    /* Do the move now */
    (*php->MoveMouse)(pScreen, xhot, yhot, 0);

    if (generateEvent)
    {
        queue_motion_event(InDev);  /* Enqueue motion event, in x_hil.c */
        isItTimeToYield++;          /* Insures client get the event! */
    }

    return(TRUE);

} /* hpSetCursorPosition() */
