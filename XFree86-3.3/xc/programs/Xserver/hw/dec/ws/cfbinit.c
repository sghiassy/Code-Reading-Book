/* $XConsortium: cfbinit.c /main/9 1995/11/30 16:34:34 dpw $ */
/***********************************************************

Copyright (c) 1991  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1991 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/* 	
	Todo. 
	-----
  ****	Fix plane mask handling.  The screen devprivate should have
	a hardware specific planemask setting proc.  For now, since
	pmax is the only machine with a plane mask, so keep it global
	for now.
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/tty.h>
#include <errno.h>
#include <sys/devio.h>


#include "misc.h"
#include "X.h"
#include "scrnintstr.h"
#include "pixmap.h"
#include "input.h"
#include "cursorstr.h"
#include "regionstr.h"
#include "resource.h"
#include "dixstruct.h"

#include <sys/workstation.h>
#include <sys/inputdevice.h>
#include "ws.h"

#include "mfb.h"

extern int wsScreenPrivateIndex;
extern CARD32 lastEventTime;
extern int defaultColorVisualClass;
void wsQueryBestSize();
Bool wsRealizeCursor();
Bool wsUnrealizeCursor();
Bool wsDisplayCursor();
void wsRecolorCursor();
void wsCursorControl();
Bool wsSetCursorPosition();
void wsCursorLimits();
void wsConstrainCursor();
void wsPointerNonInterestBox();
void wsChangeKeyboardControl();
void wsChangePointerControl();
void wsClick();

extern	ws_event_queue	*queue;
#undef VSYNCFIXED
#ifdef VSYNCFIXED
#define CURRENT_TIME	queue->time
#else
#define CURRENT_TIME	GetTimeInMillis()
#endif

extern int wsScreenInit();
extern void miRecolorCursor();
extern void NewCurrentScreen();
extern int wsFd;
extern ws_screen_descriptor screenDesc[];
static ws_depth_descriptor depthDesc[MAXSCREENS];
static mapped[MAXSCREENS];
char *planemask_addr;

/*
 * XXX  this needs to get fixed to accept a screen!!!  jmg
 */
unsigned int
cfbpmaxSetPlaneMask(planemask, pScreen)
    unsigned int planemask;
    ScreenPtr *pScreen;
{
    static unsigned int currentmask = ~0;
    unsigned int result = currentmask;
    if(planemask_addr == 0)
    {
	currentmask =  planemask;
    }
    else
    {
        *planemask_addr = currentmask = planemask;

    }
    return result;
}


void
wsQueryBestSize16(class, pwidth, pheight, pScr)
    int class;
    unsigned short *pwidth;
    unsigned short *pheight;
    ScreenPtr pScr;
{
    unsigned width, test;

    if (*pwidth > 0)
    {
      switch(class)
      {
        case CursorShape:
	  *pwidth = 16;
	  *pheight = 16;
	  break;
	default: 
	  mfbQueryBestSize(class, pwidth, pheight, pScr);
	  break;
       }
    }
}


void
wsQueryBestSize64(class, pwidth, pheight, pScr)
    int class;
    unsigned short *pwidth;
    unsigned short *pheight;
    ScreenPtr pScr;
{
    unsigned width, test;

    if (*pwidth > 0)
    {
      switch(class)
      {
        case CursorShape:
	  *pwidth = 64;
	  *pheight = 64;
	  break;
	default: 
	  mfbQueryBestSize(class, pwidth, pheight, pScr);
	  break;
       }
    }
}

Bool
wsScreenClose(index, pScreen)
    int index;
    ScreenPtr pScreen;
{
    wsScreenPrivate *wsp = (wsScreenPrivate *)
		pScreen->devPrivates[wsScreenPrivateIndex].ptr;

    pScreen->CloseScreen = wsp->CloseScreen;
    Xfree(wsp);
    return (*pScreen->CloseScreen) (index, pScreen);
}


static void
colorNameToColor( pname, pred, pgreen, pblue)
    char *      pname;
    unsigned short *     pred;
    unsigned short *     pgreen;
    unsigned short *     pblue;
{
    if ( *pname == '#')
    {
        pname++;                /* skip over # */
        sscanf( pname, "%2x", pred);
        *pred <<= 8;

        pname += 2;
        sscanf( pname, "%2x", pgreen);
        *pgreen <<= 8;

        pname += 2;
        sscanf( pname, "%2x", pblue);
        *pblue <<= 8;
    }
    else /* named color */
    {
        OsLookupColor( 0 /*"screen", not used*/, pname, strlen( pname),
                pred, pgreen, pblue);
    }
}

extern Bool mfbScreenInit (), mcfbScreenInit(), cfbScreenInit();
extern Bool cfb16ScreenInit(), cfb32ScreenInit();

Bool
fbInitProc(index, pScreen, argc, argv)
    int index;
    ScreenPtr pScreen;
    int argc;
    char **argv;
{
    int		dpix, dpiy, i;
    static int  mapOnce = FALSE;
    wsScreenPrivate *wsp;
    int	    depthIndex;
    Bool    (*screenInit)();
    ws_depth_descriptor *dd;
    ws_visual_descriptor vd;
    static ws_map_control mc;
    VisualPtr	    pVisual;
    ColormapPtr	    pCmap;
#ifdef __alpha
    Pixel	    blackPixel, whitePixel;
#endif
/* for initializing color map entries */
    unsigned short blackred      = 0x0000;
    unsigned short blackgreen    = 0x0000; 
    unsigned short blackblue     = 0x0000;

    unsigned short whitered      = 0xffff;
    unsigned short whitegreen    = 0xffff;
    unsigned short whiteblue     = 0xffff;

    lastEventTime = CURRENT_TIME;

    wsp = (wsScreenPrivate *) Xalloc(sizeof(wsScreenPrivate));
    wsp->pInstalledMap = NOMAPYET;
    pScreen->devPrivates[wsScreenPrivateIndex].ptr = (pointer) wsp;

    wsp->screenDesc = &screenDesc[index];    
    wsp->args = &screenArgs[index];

    /* since driver does not support unmap (yet), only map screen once */
    if (! mapped[index]) {
	depthDesc[index].screen = screenDesc[index].screen;
	depthIndex = -1;
	for (i = 0; i < wsp->screenDesc->allowed_depths; i++) 
	{
	    extern int forceDepth;

	    depthDesc[index].which_depth = i;	
	    if (ioctl(wsFd, GET_DEPTH_INFO, &depthDesc[index]) == -1) {
		ErrorF("GET_DEPTH_INFO failed");
		exit (1);
	    }
	    if (forceDepth)
		depthDesc[index].depth = forceDepth;
	    switch (depthDesc[index].bits_per_pixel) {
	    case 1:
	    case 8:
	    case 16:
	    case 32:
		break;
	    default:
		continue;
	    }
	    depthIndex = i;
	}
	if (depthIndex == -1) return FALSE;

	mc.screen = screenDesc[index].screen;
	mc.which_depth = depthIndex;
	mc.map_unmap = MAP_SCREEN;
	if (ioctl(wsFd,  MAP_SCREEN_AT_DEPTH, &mc) == -1)    {
	    ErrorF("MAP_SCREEN_AT_DEPTH failed");
	    free ((char *) wsp);
	    return FALSE;
	}
	/* 
	 * reget the depth desc.  It now contains the user-mapped bitmap
	 * addr. 
	 */
	if (ioctl(wsFd, GET_DEPTH_INFO, &depthDesc[index]) == -1) 
	{
	    ErrorF("GET_DEPTH_INFO failed");
	    return FALSE;
	}
	if (forceDepth)
	    depthDesc[index].depth = forceDepth;
	mapped[index] = TRUE;
    }

    /* ws routines knows how to initialize many functions, so call init. */
    if (wsScreenInit(index, pScreen, argc, argv) == -1) 
	return FALSE;

    dd = &depthDesc[index];

    if (index > 0) 
	wsCursorControl(index, CURSOR_OFF);
    wsScreens[index] = pScreen;

/* 
 * this is really dumb.  The driver has the screen geometry in mm.
 * The screen wants it stored as mm, but the damn interface passes
 * inches.  mm => inches => mm.  What a waste.  Should we change cfbscrinit.c?
 * -jmg.
 */

    if (screenArgs[index].flags & ARG_DPIX)
	dpix = screenArgs[index].dpix;
    else
        dpix =  (wsp->screenDesc->width * 254 +
                    wsp->screenDesc->monitor_type.mm_width * 5)  /
                (wsp->screenDesc->monitor_type.mm_width * 10);

    if (screenArgs[index].flags & ARG_DPIY)
	dpiy = screenArgs[index].dpiy;
    else
        dpiy =  (wsp->screenDesc->height * 254 +
                    wsp->screenDesc->monitor_type.mm_height * 5) /
                (wsp->screenDesc->monitor_type.mm_height * 10);

    vd.screen = screenDesc[index].screen; 
    vd.which_visual = 0;	/* jmg ? */
     if (ioctl(wsFd, GET_VISUAL_INFO, &vd) == -1) 
     {
     	ErrorF("GET_VISUAL_INFO failed");
    	return FALSE;
    }

    defaultColorVisualClass = vd.screen_class;
    if(screenArgs[index].flags & ARG_CLASS) 
	defaultColorVisualClass = screenArgs[index].class;

    /* Might want to make this table driven  - jmg */

    switch (dd->bits_per_pixel)
    {
    case 1:
	screenInit = mfbScreenInit;
	break;
    case 8:
    case 16:
    case 32:
	screenInit = mcfbScreenInit;
	break;
    }
    if (dd->depth == 1) 
    {
	pScreen->blackPixel = 0;
	pScreen->whitePixel = 1;
    	if(screenArgs[index].flags & ARG_BLACKVALUE)
	    if((i = atoi(screenArgs[index].blackValue)) == 0 || i == 1)
	    	pScreen->blackPixel = i;
	    else
	    	wsPixelError(index);
    
    	if(screenArgs[index].flags & ARG_WHITEVALUE)
	    if((i = atoi(screenArgs[index].whiteValue)) == 0 || i == 1)
	    	pScreen->whitePixel = i;
	    else
	    	wsPixelError(index);
    }
    else
    {
    	if(screenArgs[index].flags & ARG_BLACKVALUE)
	    colorNameToColor(screenArgs[index].blackValue, &blackred,
			     &blackgreen, &blackblue); 
    
    	if(screenArgs[index].flags & ARG_WHITEVALUE)
	    colorNameToColor(screenArgs[index].whiteValue, &whitered, 
			    &whitegreen, &whiteblue);

    }

    if (!(*screenInit) (pScreen, 
#ifndef __alpha
	dd->pixmap, 
#else
	(pointer)(((char*)dd->pixmap) - 4096),
#endif
	wsp->screenDesc->width,
	wsp->screenDesc->height, 
	dpix, dpiy, 
	dd->fb_width, dd->bits_per_pixel, dd->depth))
    {
	return FALSE;
    }
 
    if (dd->depth == 1)
    {
        mfbCreateDefColormap(pScreen);
    }
    else
    {
	/* copy of cfbCreateDefColormap, except variable colors */
	for (pVisual = pScreen->visuals;
	     pVisual->vid != pScreen->rootVisual;
	     pVisual++)
	    ;

	if (CreateColormap(pScreen->defColormap, pScreen, pVisual, &pCmap,
			   (pVisual->class & DynamicClass) ? AllocNone : AllocAll,
			   0)
	    != Success)
	    return FALSE;
#ifndef __alpha
	if ((AllocColor(pCmap, &whitered, &whitegreen, &whiteblue,
			&(pScreen->whitePixel), 0) != Success) ||
	    (AllocColor(pCmap, &blackred, &blackgreen, &blackblue,
			&(pScreen->blackPixel), 0) != Success))
	{
	    return FALSE;
	}
#else
	/* 
	 * It could be argued that this is unnecessary because the DEC
	 * is little-endian and you get what you need, but it does
	 * eliminate a warning from the compiler.
	 */
	if ((AllocColor(pCmap, &whitered, &whitegreen, &whiteblue,
			&whitePixel, 0) != Success) ||
	    (AllocColor(pCmap, &blackred, &blackgreen, &blackblue,
			&blackPixel, 0) != Success))
	{
	    return FALSE;
	}
	else
	{
	    pScreen->whitePixel = whitePixel;
	    pScreen->blackPixel = blackPixel;
	}
#endif
	(*pScreen->InstallColormap)(pCmap);
    }
    planemask_addr = dd->plane_mask;
    
    /* Wrap screen close routine to avoid memory leak */
    wsp->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = wsScreenClose;

    if(wsp->screenDesc->cursor_width == 64)
	 pScreen->QueryBestSize = wsQueryBestSize64;
    else
	 pScreen->QueryBestSize = wsQueryBestSize16;
    return TRUE;
}
