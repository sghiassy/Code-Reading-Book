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

/* $XConsortium: init.c /main/12 1996/01/15 14:55:16 kaleb $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/tty.h>
#include <errno.h>
#include <sys/devio.h>

#include "X.h"

#include "scrnintstr.h"
#include "servermd.h"

#include "input.h"
/* XXX */
#include <sys/workstation.h>
#ifdef __alpha
#include <machine/hal_sysinfo.h>
#endif
#include <sys/inputdevice.h>
#include "ws.h"

extern int wsMouseProc();
extern int wsKeybdProc();
extern void wsClick();
extern void wsChangePointerControl();
extern void mcfbFillInMissingPixmapDepths();

extern KeybdCtrl defaultKeyboardControl;
ws_event_queue	*queue;

static int NumFormats;
static int bitsPerDepth[33];

Bool fbInitProc();
extern int num_accelerator_types;

extern wsAcceleratorTypes types[];

ws_screen_descriptor screenDesc[MAXSCREENS];

Bool
commandLineMatch( argc, argv, pat, pmatch)
    int         argc;           /* may NOT be changed */
    char *      argv[];         /* may NOT be changed */
    char *      pat;
{
    int         ic;

    for ( ic=0; ic<argc; ic++)
        if ( strcmp( argv[ic], pat) == 0)
            return TRUE;
    return FALSE;
}

Bool
commandLinePairMatch( argc, argv, pat, pmatch)
    int         argc;           /* may NOT be changed */
    char *      argv[];         /* may NOT be changed */
    char *      pat;
    char **     pmatch;         /* RETURN */
{
    register int         ic;

    for ( ic=0; ic<argc; ic++)
        if ( strcmp( argv[ic], pat) == 0) {
            *pmatch = argv[ ic+1];
            return TRUE;
	}
    return FALSE;
}

ws_descriptor wsinfo;
int wsFd;
int ws_cpu;
int forceDepth;

int wsScreenPrivateIndex;
/* the following filth is forced by a broken dix interface */

void
InitOutput(screenInfo, argc, argv)
    ScreenInfo *screenInfo;
    int argc;
    char **argv;
{
    int i, j;
    int si = 0;
    static int inited = FALSE;

    if (!inited) {
	char *forceD;
	ws_keyboard_control control;
	struct stat statbuf;
	inited = TRUE;
	if (!stat("/dev/ws0", &statbuf)) {
	    if ((wsFd = open("/dev/ws0",  O_RDWR, 0)) < 0) {
		ErrorF("couldn't open device /dev/ws0\n");
		exit (1);
	    }
	} else {
	    if ((wsFd = open("/dev/mouse",  O_RDWR, 0)) < 0) {
		ErrorF("couldn't open device /dev/mouse\n");
		exit (1);
	    }
	}
	if (ioctl (wsFd, GET_WORKSTATION_INFO, &wsinfo) != 0) {
		FatalError("GET_WORKSTATION_INFO failed, errno %d (%s)\n",
			   errno, strerror(errno));
	}
	control.device_number = wsinfo.console_keyboard;
	if (ioctl(wsFd, GET_KEYBOARD_CONTROL, &control) == -1) {
		FatalError("GET_KEYBOARD_CONTROL failed, errno %d (%s)\n",
			   errno, strerror(errno));
	}
	memmove(defaultKeyboardControl.autoRepeats, control.autorepeats, 32);
	
    /* turn off cursors on additional screens initially */
/* 
 * jmg - have to do a lot of other stuff here dealing with visuals
 *  and depths 
 */
       /*
	* deal with arguments.  Note we don't bother until we've successfully
	* opened the device.
	*/

	if (commandLinePairMatch( argc, argv, "-forceDepth", &forceD))
		sscanf ( forceD, "%d", &forceDepth);
    }
    for (i = 1; i <= 32; i++)
	bitsPerDepth[i] = 0;

    for (i = 0; i < wsinfo.num_screens_exist; i++)
    {
	ws_screen_descriptor	screeninfo;
	ws_depth_descriptor	depthinfo;
	
	screeninfo.screen = i;
	ioctl (wsFd, GET_SCREEN_INFO, &screeninfo);
	for (j = 0; j < screeninfo.allowed_depths; j++) 
	{
	    depthinfo.screen = i;
	    depthinfo.which_depth = j;
	    ioctl (wsFd, GET_DEPTH_INFO, &depthinfo);
	    if (forceDepth)
		depthinfo.depth = forceDepth;
	    if (bitsPerDepth[depthinfo.depth] &&
		bitsPerDepth[depthinfo.depth] != depthinfo.bits_per_pixel)
	    {
		FatalError ("Screens with mismatching bpp for depth %d\n",
			    depthinfo.depth);
	    }
	    bitsPerDepth[depthinfo.depth] = depthinfo.bits_per_pixel;
	}
    }
    if (!bitsPerDepth[1])
	bitsPerDepth[1] = 1;

    mcfbFillInMissingPixmapDepths(bitsPerDepth);

    NumFormats = 0;
    for (i = 1; i <= 32; i++)
    {
	if (j = bitsPerDepth[i]) {
	    if (NumFormats >= MAXFORMATS)
		FatalError ("MAXFORMATS is too small for this machine\n");
	    screenInfo->formats[NumFormats].depth = i;
	    screenInfo->formats[NumFormats].bitsPerPixel = j;
	    screenInfo->formats[NumFormats].scanlinePad = BITMAP_SCANLINE_PAD;
	    NumFormats++;
	}
    }

    screenInfo->imageByteOrder = IMAGE_BYTE_ORDER;
    screenInfo->bitmapScanlineUnit = BITMAP_SCANLINE_UNIT;
    screenInfo->bitmapScanlinePad = BITMAP_SCANLINE_PAD;
    screenInfo->bitmapBitOrder = BITMAP_BIT_ORDER;
    screenInfo->numPixmapFormats = NumFormats;

    wsScreenPrivateIndex = AllocateScreenPrivateIndex();
#ifdef __alpha
    if (getsysinfo(GSI_CPU, (caddr_t)&ws_cpu, sizeof(ws_cpu), 0, 0) == -1) {
        fprintf(stderr,"INIT: cannot get cputype.\n");
        exit(1);
    }
#else
    ws_cpu = wsinfo.cpu;
#endif
    for(i = 0; i < wsinfo.num_screens_exist; i++) {
	int j, DECaccelerator = FALSE;
	screenDesc[si].screen = i;
	if (ioctl(wsFd,	 GET_SCREEN_INFO, &screenDesc[si]) == -1) {
	    FatalError("GET_SCREEN_INFO failed, errno %d (%s)\n",
		       errno, strerror(errno));
	}
	if (si >= MAXSCREENS) {
	    ErrorF ("Server configured for %d screens, can't configure screen %d\n", MAXSCREENS, si);
	    break;
	}
	for (j = 0; j < num_accelerator_types; j++) {
	    if (strcmp (screenDesc[si].moduleID, types[j].moduleID) == 0) {
		DECaccelerator = TRUE;
		break;
	    }
	}
        if(DECaccelerator)
	    j = AddScreen(types[j].createProc, argc, argv);
	else
	    j = AddScreen(fbInitProc, argc, argv);
	/*
	 * AddScreen either returns -1 (error) or # of screens installed
	 * (current index).  screenDesc[] is exactly parallel to
	 * screenInfo.screens[] since the initProc() arg will be called
	 * with AddScreen's index, which in turn gets used for screenDesc[].
	 */
	if (j == -1) {
		ErrorF("Could not AddScreen, ID = %s\n",
			screenDesc[si].moduleID);
	} else
	    si = j+1;
    }
#ifdef XINPUT
    ExtInitOutput(screenInfo, argc, argv);
#endif
}



void
InitInput(argc, argv)
    int argc;
    char *argv[];
{
    DevicePtr p, k;
    static int inited = FALSE;
    
    p = AddInputDevice(wsMouseProc, TRUE);

    k = AddInputDevice(wsKeybdProc, TRUE);

    RegisterPointerDevice(p);
    RegisterKeyboardDevice(k);

#ifdef XINPUT
    ExtInitInput(argc, argv);
#endif

    if (!inited) {
	inited = TRUE;
	if (ioctl(wsFd,  GET_AND_MAP_EVENT_QUEUE, &queue) == -1)  {
		FatalError("GET_AND_MAP_EVENT_QUEUE failed, errno %d (%s)\n",
			   errno, strerror(errno));
	}
    }
    SetTimeSinceLastInputEvent ();
}
