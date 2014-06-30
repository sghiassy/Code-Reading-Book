/* $XFree86: xc/programs/Xserver/hw/xfree86/vga16/ibm/ppcRslvC.c,v 3.6 1996/12/23 06:53:15 dawes Exp $ */
/************************************************************
Copyright 1987 by Sun Microsystems, Inc. Mountain View, CA.

                    All Rights Reserved

Permission  to  use,  copy,  modify,  and  distribute   this
software  and  its documentation for any purpose and without
fee is hereby granted, provided that the above copyright no-
tice  appear  in all copies and that both that copyright no-
tice and this permission notice appear in  supporting  docu-
mentation,  and  that the names of Sun or X Consortium
not be used in advertising or publicity pertaining to 
distribution  of  the software  without specific prior 
written permission. Sun and X Consortium make no 
representations about the suitability of this software for 
any purpose. It is provided "as is" without any express or 
implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

********************************************************/

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

/* $XConsortium: ppcRslvC.c /main/7 1996/02/21 17:58:25 kaleb $ */

/* Generic Color Resolution Scheme
 * P. Shupak 12/31/87
 */

#include "X.h"
#include "screenint.h"
#include "scrnintstr.h"
#include "colormapst.h"	/* GJA */
#include "OScompiler.h"

/*
 * New colormap routines that can support multiple Visual types.
 */

static unsigned short defstaticpalette[16][3] = {
	/*   R       G       B   */
	{ 0x0000, 0x0000, 0x0000 },	/* black */
	{ 0xFFFF, 0xFFFF, 0xFFFF },	/* white */
	{ 0xAAAA, 0xAAAA, 0xAAAA },	/* grey */
	{ 0x0000, 0x0000, 0xAAAA },	/* dark blue */
	{ 0x0000, 0x0000, 0xFFFF },	/* medium blue */
	{ 0x0000, 0xAAAA, 0xFFFF },	/* light blue */
	{ 0x0000, 0xFFFF, 0xFFFF },	/* cyan */
	{ 0x0000, 0xAAAA, 0x0000 },	/* dark green */
	{ 0x0000, 0xFFFF, 0x0000 },	/* green */
	{ 0xAAAA, 0xFFFF, 0x5555 },	/* pale green */
	{ 0xAAAA, 0x5555, 0x0000 },	/* brown */
	{ 0xFFFF, 0xAAAA, 0x0000 },	/* light brown */
	{ 0xFFFF, 0xFFFF, 0x0000 },	/* yellow */
	{ 0xAAAA, 0x0000, 0xAAAA },	/* purple */
	{ 0xFFFF, 0x0000, 0xFFFF },	/* magenta */
	{ 0xFFFF, 0x0000, 0x0000 },	/* red */
	};

static unsigned short staticgraypalette[16][3] = {
	/*   R       G       B   */
	{ 0x0000, 0x0000, 0x0000 },
	{ 0x1000, 0x1000, 0x1000 },
	{ 0x2000, 0x2000, 0x2000 },
	{ 0x3000, 0x3000, 0x3000 },
	{ 0x4400, 0x4400, 0x4400 },
	{ 0x5400, 0x5400, 0x5400 },
	{ 0x6400, 0x6400, 0x6400 },
	{ 0x7400, 0x7400, 0x7400 },
	{ 0x8800, 0x8800, 0x8800 },
	{ 0x9800, 0x9800, 0x9800 },
	{ 0xA800, 0xA800, 0xA800 },
	{ 0xB800, 0xB800, 0xB800 },
	{ 0xCC00, 0xCC00, 0xCC00 },
	{ 0xDC00, 0xDC00, 0xDC00 },
	{ 0xEC00, 0xEC00, 0xEC00 },
	{ 0xFFFF, 0xFFFF, 0xFFFF },
	};

Bool
vga16InitializeColormap(pmap)
    register ColormapPtr	pmap;
{
    register unsigned i;
    register VisualPtr pVisual;
    unsigned lim, maxent, shift;

    pVisual = pmap->pVisual;
    lim = (1 << pVisual->bitsPerRGBValue) - 1;
    shift = 16 - pVisual->bitsPerRGBValue;
    maxent = pVisual->ColormapEntries - 1;

    switch( pVisual->class )
	{
	case StaticGray:
	    for ( i = 0 ; i < 16 ; i++ ) {
		pmap->red[i].co.local.red   = (staticgraypalette[i][0]);
		pmap->red[i].co.local.green = (staticgraypalette[i][1]);
		pmap->red[i].co.local.blue  = (staticgraypalette[i][2]);
	    }
	    break;
	case StaticColor:
	    for ( i = 0 ; i < 16 ; i++ ) {
		pmap->red[i].co.local.red   = (defstaticpalette[i][0]);
		pmap->red[i].co.local.green = (defstaticpalette[i][1]);
		pmap->red[i].co.local.blue  = (defstaticpalette[i][2]);
	    }
	    break;
	case GrayScale:
	case PseudoColor:
	    for(i=0;i<=maxent;i++) {
	        int a,b,c;
		a = i << 10;
		b = i << 12;
		c = i << 14;
		pmap->red[i].co.local.red   = a;
		pmap->red[i].co.local.green = b;
		pmap->red[i].co.local.blue  = c;
	    }
	    break;
	case TrueColor:
	case DirectColor:
	default:
	    ErrorF( "Unsupported Visual class %d\b", pVisual->class );
	    return FALSE;
	}
    return TRUE;
}

void
vga16ResolveColor( pred, pgreen, pblue, pVisual )
register unsigned short* const pred ;
register unsigned short* const pgreen ;
register unsigned short* const pblue ;
register VisualPtr const pVisual ;
{ 
    unsigned lim, maxent, shift;

    lim = (1 << pVisual->bitsPerRGBValue) - 1;
    shift = 16 - pVisual->bitsPerRGBValue;
    maxent = pVisual->ColormapEntries - 1;

    switch( pVisual->class )
	{
	case StaticGray:
	    *pred = (30L * *pred + 59L * *pgreen + 11L * *pblue) / 100;
	    *pblue = *pgreen = *pred = (((*pred >> shift) * 65535)/lim)&0xFC00;
	    break;
	case StaticColor:
	    break;
	case GrayScale:
	    *pred = (30L * *pred + 59L * *pgreen + 11L * *pblue) / 100;
	    *pblue = *pgreen = *pred = ((*pred >> shift) * 65535) / lim;
	    break;
	case PseudoColor:
	    /* rescale to rgb bits */
	    *pred = ((*pred >> shift) * 65535) / lim;
	    *pgreen = ((*pgreen >> shift) * 65535) / lim;
	    *pblue = ((*pblue >> shift) * 65535) / lim;
	    break;
	case TrueColor:
	case DirectColor:
	default:
	    ErrorF( "Unsupported Visual class %d\b", pVisual->class );
	}
}

Bool
vga16CreateDefColormap(pScreen)
    ScreenPtr pScreen;
{
    unsigned short	zero = 0, ones = 0xFFFF;
    VisualPtr	pVisual;
    ColormapPtr	cmap;
    
    for (pVisual = pScreen->visuals;
	 pVisual->vid != pScreen->rootVisual;
	 pVisual++)
	;

    if (CreateColormap(pScreen->defColormap, pScreen, pVisual, &cmap,
		       (pVisual->class & DynamicClass) ? AllocNone : AllocAll,
		       0)
	!= Success)
	return FALSE;
    if ((AllocColor(cmap, &ones, &ones, &ones, &(pScreen->whitePixel), 0) !=
       	   Success) ||
    	(AllocColor(cmap, &zero, &zero, &zero, &(pScreen->blackPixel), 0) !=
       	   Success))
    	return FALSE;
    (*pScreen->InstallColormap)(cmap);
    return TRUE;
}
