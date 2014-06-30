/* $XConsortium: miRndText.c,v 5.2 94/04/17 20:36:57 rws Exp $ */
/* $XFree86: xc/programs/Xserver/PEX5/ddpex/mi/level1/miRndText.c,v 3.0 1996/03/29 22:09:28 dawes Exp $ */
/*

Copyright (c) 1989, 1990, 1991  X Consortium

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


Copyright (c) 1989, 1990, 1991 by Sun Microsystems, Inc.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Sun Microsystems,
not be used in advertising or publicity pertaining to distribution
of the software without specific, written prior permission.

SUN MICROSYSTEMS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
SHALL SUN MICROSYSTEMS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

*/

#define NEED_EVENTS
#include "miRender.h"
#include "misc.h"
#include "miscstruct.h"
#include "PEXErr.h"
#include "extnsionst.h"
#include "gcstruct.h"
#include "ddpex2.h"

#include <stdio.h>

/*++
 |
 |  Function Name:	miRenderText
 |
 |  Function Description:
 |	 Renders Text to the screen.
 |
 |  Note(s):
 |
 --*/

ddpex3rtn
miRenderText(pRend, pddc, input_list)
/* in */
    ddRendererPtr       pRend;          /* renderer handle */
    miDDContext         *pddc;          /* dd context handle */
    miListHeader        *input_list;    /* fill area data */
{
/* calls */
      ddpex3rtn		miFilterPath();

/* Local variable definitions */
      miListHeader	*temp_list;
      listofddPoint	*sp;
      int		j;
      ddpex3rtn		err = Success;

      /* Remove all data  but vertex coordinates */
      if ((DD_IsVertNormal(input_list->type)) ||
	  (DD_IsVertEdge(input_list->type)) ||
	  (DD_IsVertColour(input_list->type)) ) {
	err = miFilterPath(pddc, input_list, &temp_list, 1);
	if (err) return (err);
	input_list = temp_list;
      }

      /*
       * Update the text GC to reflect the current 
       * text attributes 
       */
      if (pddc->Static.misc.flags & TEXTGCFLAG)
	miDDC_to_GC_text(pRend, pddc, pddc->Static.misc.pTextGC);

      /* Validate GC prior to start of rendering */

      if (pddc->Static.misc.pTextGC->serialNumber != 
	  pRend->pDrawable->serialNumber)
        ValidateGC(pRend->pDrawable, pddc->Static.misc.pTextGC);

      /* We should have DC paths here; Render them */

      for (j = 0, sp = input_list->ddList; 
	   j < input_list->numLists; j++, sp++) {
        if (sp->numPoints > 0) {

	  /* Call ddx to render the polylines */
	  (*GetGCValue(pddc->Static.misc.pTextGC, ops->Polylines)) 
			(pRend->pDrawable,
			 pddc->Static.misc.pTextGC, 
			CoordModeOrigin, 
			sp->numPoints, 
			sp->pts.p2DSpt);
        }
      }
}
