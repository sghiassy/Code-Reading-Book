
/* $XConsortium: sunFbs.c /main/9 1995/10/05 07:36:47 kaleb $ */

/*
Copyright (c) 1990, 1993  X Consortium

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
 */

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
 * Copyright (c) 1987 by the Regents of the University of California
 * Copyright (c) 1987 by Adam de Boor, UC Berkeley
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

/****************************************************************/
/* Modified from  sunCG4C.c for X11R3 by Tom Jarmolowski	*/
/****************************************************************/

#include "sun.h"
#include <sys/mman.h>

int sunScreenIndex;

static unsigned long generation = 0;

#if NeedFunctionPrototypes
pointer sunMemoryMap (
    size_t	len,
    off_t	off,
    int		fd)
#else
pointer sunMemoryMap (len, off, fd)
    size_t	len;
    off_t	off;
    int		fd;
#endif
{
    int		pagemask, mapsize;
    caddr_t	addr;
    pointer	mapaddr;

#ifdef SVR4
    pagemask = sysconf(_SC_PAGESIZE) - 1;
#else 
    pagemask = getpagesize() - 1;
#endif
    mapsize = ((int) len + pagemask) & ~pagemask;
    addr = 0;

#if !defined(__bsdi__) && !defined(_MAP_NEW)
    if ((addr = (caddr_t) valloc (mapsize)) == NULL) {
	Error ("Couldn't allocate frame buffer memory");
	(void) close (fd);
	return NULL;
    }
#endif

    /* 
     * try and make it private first, that way once we get it, an
     * interloper, e.g. another server, can't get this frame buffer,
     * and if another server already has it, this one won't.
     */
    if ((int)(mapaddr = (pointer) mmap (addr,
		mapsize,
		PROT_READ | PROT_WRITE, MAP_PRIVATE,
		fd, off)) == -1)
	mapaddr = (pointer) mmap (addr,
		    mapsize,
		    PROT_READ | PROT_WRITE, MAP_SHARED,
		    fd, off);
    if (mapaddr == (pointer) -1) {
	Error ("mapping frame buffer memory");
	(void) close (fd);
	mapaddr = (pointer) NULL;
    }
    return mapaddr;
}

#if NeedFunctionPrototypes
Bool sunScreenAllocate (
    ScreenPtr	pScreen)
#else
Bool sunScreenAllocate (pScreen)
    ScreenPtr	pScreen;
#endif
{
    sunScreenPtr    pPrivate;
    extern int AllocateScreenPrivateIndex();

    if (generation != serverGeneration)
    {
	sunScreenIndex = AllocateScreenPrivateIndex();
	if (sunScreenIndex < 0)
	    return FALSE;
	generation = serverGeneration;
    }
    pPrivate = (sunScreenPtr) xalloc (sizeof (sunScreenRec));
    if (!pPrivate)
	return FALSE;

    pScreen->devPrivates[sunScreenIndex].ptr = (pointer) pPrivate;
    return TRUE;
}

#if NeedFunctionPrototypes
Bool sunSaveScreen (
    ScreenPtr	pScreen,
    int		on)
#else
Bool sunSaveScreen (pScreen, on)
    ScreenPtr	pScreen;
    int		on;
#endif
{
    int		state;

    if (on != SCREEN_SAVER_FORCER)
    {
	if (on == SCREEN_SAVER_ON)
	    state = 0;
	else
	    state = 1;
	(void) ioctl(sunFbs[pScreen->myNum].fd, FBIOSVIDEO, &state);
    }
    return( TRUE );
}

static Bool closeScreen (i, pScreen)
    int		i;
    ScreenPtr	pScreen;
{
    SetupScreen(pScreen);
    Bool    ret;

    (void) OsSignal (SIGIO, SIG_IGN);
    sunDisableCursor (pScreen);
    pScreen->CloseScreen = pPrivate->CloseScreen;
    ret = (*pScreen->CloseScreen) (i, pScreen);
    (void) (*pScreen->SaveScreen) (pScreen, SCREEN_SAVER_OFF);
    xfree ((pointer) pPrivate);
    return ret;
}

#if NeedFunctionPrototypes
Bool sunScreenInit (
    ScreenPtr	pScreen)
#else
Bool sunScreenInit (pScreen)
    ScreenPtr	pScreen;
#endif
{
    SetupScreen(pScreen);
    extern void   sunBlockHandler();
    extern void   sunWakeupHandler();
    static ScreenPtr autoRepeatScreen;
    extern miPointerScreenFuncRec   sunPointerScreenFuncs;

    pPrivate->installedMap = 0;
    pPrivate->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = closeScreen;
    pScreen->SaveScreen = sunSaveScreen;
#ifdef XKB
    if (noXkbExtension) {
#endif
    /*
     *	Block/Unblock handlers
     */
    if (sunAutoRepeatHandlersInstalled == FALSE) {
	autoRepeatScreen = pScreen;
	sunAutoRepeatHandlersInstalled = TRUE;
    }

    if (pScreen == autoRepeatScreen) {
        pScreen->BlockHandler = sunBlockHandler;
        pScreen->WakeupHandler = sunWakeupHandler;
    }
#ifdef XKB
    }
#endif
    if (!sunCursorInitialize (pScreen))
	miDCInitialize (pScreen, &sunPointerScreenFuncs);
    return TRUE;
}

#if NeedFunctionPrototypes
Bool sunInitCommon (
    int		scrn,
    ScreenPtr	pScrn,
    off_t	offset,
    Bool	(*init1)(),
    void	(*init2)(),
    Bool	(*cr_cm)(),
    Bool	(*save)(),
    int		fb_off)
#else
Bool sunInitCommon (scrn, pScrn, offset, init1, init2, cr_cm, save, fb_off)
    int		scrn;
    ScreenPtr	pScrn;
    off_t	offset;
    Bool	(*init1)();
    void	(*init2)();
    Bool	(*cr_cm)();
    Bool	(*save)();
    int		fb_off;
#endif
{
    unsigned char*	fb = sunFbs[scrn].fb;

    if (!sunScreenAllocate (pScrn))
	return FALSE;
    if (!fb) {
	if ((fb = sunMemoryMap ((size_t) sunFbs[scrn].info.fb_size, 
			     offset, 
			     sunFbs[scrn].fd)) == NULL)
	    return FALSE;
	sunFbs[scrn].fb = fb;
    }
    /* mfbScreenInit() or cfbScreenInit() */
    if (!(*init1)(pScrn, fb + fb_off,
	    sunFbs[scrn].info.fb_width,
	    sunFbs[scrn].info.fb_height,
	    monitorResolution, monitorResolution,
	    sunFbs[scrn].info.fb_width,
	    sunFbs[scrn].info.fb_depth))
	    return FALSE;
    /* sunCGScreenInit() if cfb... */
    if (init2)
	(*init2)(pScrn);
    if (!sunScreenInit(pScrn))
	return FALSE;
    (void) (*save) (pScrn, SCREEN_SAVER_OFF);
    return (*cr_cm)(pScrn);
}

