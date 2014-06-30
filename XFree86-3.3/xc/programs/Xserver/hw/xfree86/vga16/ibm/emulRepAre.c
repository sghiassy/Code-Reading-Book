/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/emulRepAre.c,v 3.4 1996/12/23 06:52:36 dawes Exp $ */
/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that 
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/* $XConsortium: emulRepAre.c /main/5 1996/02/21 17:56:16 kaleb $ */

/* ppc Replicate Area -- A Divide & Conquer Algorithm
 * a "ppc" Helper Function For Stipples And Tiling
 * P. Shupak 1/88
 */

#include "X.h"
#include "scrnintstr.h"
#include "screenint.h"
#include "pixmapstr.h"
#include "pixmap.h"
#include "windowstr.h"	/* GJA */
#include "ppc.h"

void ppcReplicateArea( pWin, x, y, planeMask, goalWidth, goalHeight,
			currentHoriz, currentVert, pScrn )
WindowPtr pWin; /* GJA */
register int x, y, planeMask ;
int goalWidth, goalHeight ;
int currentHoriz, currentVert ;
ScreenPtr pScrn ;
{
	for ( ;
	      currentHoriz <= ( goalWidth >> 1 ) ;
	      currentHoriz <<= 1 ) {
		vgaBitBlt( pWin, GXcopy, planeMask, planeMask,
			x, y,
			x + currentHoriz, y,
			currentHoriz, currentVert ) ;
	}
	if ( goalWidth - currentHoriz )
		vgaBitBlt( pWin, GXcopy, planeMask, planeMask,
			x, y,
			x + currentHoriz, y,
			goalWidth - currentHoriz, currentVert ) ;
	for ( ;
	      currentVert <= ( goalHeight >> 1 ) ;
	      currentVert <<= 1 ) {
		vgaBitBlt( pWin, GXcopy, planeMask, planeMask,
			x, y,
			x, y + currentVert,
			goalWidth, currentVert ) ;
	}
	if ( goalHeight - currentVert )
		vgaBitBlt( pWin, GXcopy, planeMask, planeMask,
			x, y,
			x, y + currentVert,
			goalWidth, goalHeight - currentVert ) ;
return ;
}
