/***********************************************************

Copyright (c) 1990  X Consortium

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


Copyright 1990 by Digital Equipment Corporation, Maynard, Massachusetts.

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
 * This file holds the hook routines for adding extension input devices.
 *
 *	Jeffrey Hsu, Digital Equipment Corporation,
 *
 */

/* $XConsortium: ext_device.c,v 1.3 94/04/17 20:29:54 keith Exp $ */

#ifdef XINPUT

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/tty.h>
#include <sys/devio.h>

#include "misc.h"
#include "X.h"
#define NEED_EVENTS
#include "Xproto.h"
#include "scrnintstr.h"
#include "pixmap.h"
#include "input.h"
#include "cursorstr.h"
#include "regionstr.h"
#include "resource.h"
#include "dixstruct.h"
#include <sys/workstation.h>
#include <sys/inputdevice.h>
#include <sys/wsdevice.h>
#include "ws.h"

#include <sys/tablet.h>
#include <sys/pcm.h>
#include <sys/termio.h>

#include "XI.h"			/* for Absolute and Relative #define */
#include "XIproto.h"		/* for deviceKeyButtonPointer */

/* XXX copied from ws_io.c, get it into some header file somewhere */
#define MOTION_BUFFER_SIZE 100
extern int wsGetMotionEvents();

extern Bool commandLinePairMatch();	/* secret function */

extern int DeviceButtonPress, DeviceButtonRelease, DeviceButtonRelease,
	DeviceMotionNotify, ProximityIn, ProximityOut, DeviceValuator;

static DevicePtr        wsTablet, wsButtonBox, wsDialBox;

static void wsButtonBoxControl(), wsDialBoxControl();
static int wsButtonBoxProc(), wsDialBoxProc(), wsTabletProc();

int tbFd;	/* tablet file descriptor */
int pcmFd;

/* return TRUE if recognized device type */
Bool
ExtProcessInputEvents(x, e)
xEvent *x;
ws_event *e;
{

	/*
	 * X Input Extension events can have more than one event structure per
	 * device event
	 */
	xEvent eventarray[3];	/* make this 3 until some device needs more */
	deviceValuator *vev;
	deviceValuator valuatorevent;
	int eventcount;

	deviceKeyButtonPointer *inputEv = (deviceKeyButtonPointer *) x;
	tablet_event *tabev = (tablet_event *) e;
	dial_event *dialev = (dial_event *) e;

	switch (e->device_type) {
	  case STABLET_DEVICE:
	    switch (e->type) {
		case BUTTON_DOWN_TYPE: 
		    x->u.u.type = DeviceButtonPress;
		    break;
		case BUTTON_UP_TYPE: 
		    x->u.u.type = DeviceButtonRelease;
		    break;
		case MOTION_TYPE: 
		    x->u.u.type = DeviceMotionNotify;
		    break;
		case PROXIMITY_IN:
		    x->u.u.type = ProximityIn;
		    break;
		case PROXIMITY_OUT:
		    x->u.u.type = ProximityOut;
		    break;
		default: 
		    printf("Unknown tablet event = %d\n", e->type);
	    	    return(TRUE);
	    }
	    vev = (deviceValuator *) &eventarray[1];
	    vev->type = DeviceValuator;
	    vev->num_valuators = 2;
	    vev->first_valuator = 0;
	    vev->valuator0 = tabev->axis_data[0];
	    vev->valuator1 = tabev->axis_data[1];
	    vev->device_state = e->e.pointer.buttons;
	    vev->deviceid = e->device;
	    eventcount = 2;
	    inputEv->deviceid = e->device | MORE_EVENTS;
	    eventarray[0] = *x;
	    (*wsTablet->processInputProc) (eventarray, wsTablet, eventcount);
	    break;
	  case BUTTONBOX_DEVICE:
	    switch (e->type) {
		case BUTTON_DOWN_TYPE: 
		    x->u.u.type = DeviceButtonPress;
		    break;
		case BUTTON_UP_TYPE: 
		    x->u.u.type = DeviceButtonRelease;
		    break;
		default: 
		    printf("Unknown button box event = %d\n", e->type);
	    	    return(TRUE);
	    }
	    (*wsButtonBox->processInputProc) (x, wsButtonBox, 1);
	    break;
	  case KNOBBOX_DEVICE:
	    switch (e->type) {
		case MOTION_TYPE:
		    x->u.u.type = DeviceMotionNotify;
		    break;
		default: 
		    printf("Unknown dial box event = %d\n", e->type);
	    	    return(TRUE);
	    }

	    vev = (deviceValuator *) &eventarray[1];
	    vev->type = DeviceValuator;
	    vev->num_valuators = 6;
	    vev->first_valuator = 0;
	    vev->valuator0 = dialev->axis_data[0];
	    vev->valuator1 = dialev->axis_data[1];
	    vev->valuator2 = dialev->axis_data[2];
	    vev->valuator3 = dialev->axis_data[3];
	    vev->valuator4 = dialev->axis_data[4];
	    vev->valuator5 = dialev->axis_data[5];
	    vev->device_state = e->e.pointer.buttons;
	    vev->deviceid = e->device;

	    vev = (deviceValuator *) &eventarray[2];
	    vev->type = DeviceValuator;
	    vev->num_valuators = 2;
	    vev->first_valuator = 6;
	    vev->valuator0 = dialev->axis_data[6];
	    vev->valuator1 = dialev->axis_data[7];
	    vev->device_state = e->e.pointer.buttons;
	    vev->deviceid = e->device;

	    eventcount = 3;
	    inputEv->deviceid = e->device;
	    eventarray[0] = *x;
	    (*wsDialBox->processInputProc) (eventarray, wsDialBox, eventcount);
	    break;
	  default:
	    return FALSE;
	}
	return TRUE;
}

int
ExtddxProcessArgument (argc, argv, i)
register int argc;
register char *argv[];
register int i;
{

	int argind=i;
	int skip = 0;

	if (strcmp(argv[argind], "-tb") == 0) {
	  if (++argind < argc)  {
	    /* defer processing to InitInput */
	    /* should think up cleaner solution XXX */
	    skip = 2;
	  }
	  else return 0;
	} else if (strcmp(argv[argind], "-pcm") == 0) {
	  if (++argind < argc)  {
	    /* defer processing to InitInput */
	    /* should think up cleaner solution XXX */
	    skip = 2;
	  }
	  else return 0;
	}

	return skip;

}

void
ExtddxUseMsg()
{
	ErrorF("-tb <tty number n>	open /dev/tty0n for tablet\n");
	ErrorF("-pcm <tty number n>	open /dev/tty0n for pcm\n");
}

void
ExtInitInput(argc, argv)
{

	DevicePtr t;
	char *tabletline, buf[256], *pcmline;
	int linenumber;
	int i;
	struct termio termio;

	if (commandLinePairMatch( argc, argv, "-tb", &tabletline)) {
	  sscanf(tabletline, "%d", &linenumber);
	  sprintf(buf, "/dev/tty0%d", linenumber);
	  if ((tbFd = open(buf,  O_RDWR, 0)) < 0) {
	    Error("couldn't open device");
	    return;
	  }

	  if (ioctl(tbFd, TCGETA, &termio) < 0) {
	    Error("TCGETA");
	    return;
	  }

	  /* someday I will do this in the line discpline open XXX */
	  termio.c_cflag = (B4800 | PARENB | PARODD | CREAD | CS8);
	  termio.c_iflag &= ~ISTRIP;

	  if (ioctl(tbFd, TCSETA, &termio) < 0) {
	    Error("TCSETA");
	    return;
	  }

	  i = TABLDISC;
	  if (ioctl(tbFd, TIOCSETD, &i) < 0) {
	    Error("couldn't set line discipline");
	    return;
	  }

	  i = 0;
	  if (ioctl(tbFd, BIOSTYPE, &i) < 0) {
	    Error("couldn't set line discipline");
	    return;
	  }

	  t = AddInputDevice(wsTabletProc, TRUE);

	  RegisterOtherDevice(t);

	}

	if (commandLinePairMatch( argc, argv, "-pcm", &pcmline)) {
	  sscanf(pcmline, "%d", &linenumber);
	  sprintf(buf, "/dev/tty0%d", linenumber);
	  if ((pcmFd = open(buf,  O_RDWR, 0)) < 0) {
	    Error("couldn't open device");
	    return;
	  }

	  if (ioctl(pcmFd, TCGETA, &termio) < 0) {
	    Error("TCGETA");
	    return;
	  }

	  /* someday I will do this in the line discpline open XXX */
	  termio.c_cflag =  (B9600 | CREAD | CS8 | CLOCAL);
	  termio.c_iflag &= ~ISTRIP;
	  termio.c_iflag |= IXOFF;

	  if (ioctl(pcmFd, TCSETA, &termio) < 0) {
	    Error("TCSETA");
	    return;
	  }

	  i = PCMDISC;
	  if (ioctl(pcmFd, TIOCSETD, &i) < 0) {
	    Error("couldn't set line discipline");
	    return;
	  }

	  t = AddInputDevice(wsButtonBoxProc, TRUE);
	  RegisterOtherDevice(t);

	  t = AddInputDevice(wsDialBoxProc, TRUE);
	  RegisterOtherDevice(t);
	}

}

void
ExtInitOutput(screenInfo, argc, argv)
ScreenInfo *screenInfo;
int argc;
char **argv;
{
/* nothing to do yet */
}

static void
wsButtonBoxControl(device, ctrl)
DevicePtr device;
LedCtrl *ctrl;
{
	int mask = ctrl->led_values;

	if (ioctl(pcmFd, PIOSETLED, &mask) < 0) {
	  Error("wsButtonBoxControl");
	}
}

static int
wsButtonBoxProc(pDev, onoff, argc, argv)
DevicePtr pDev;
int onoff, argc;
char *argv[];
{
	BYTE map[33];

	int i;
	Atom typeatom;

	switch (onoff)
	{
	  case DEVICE_INIT: 
	    wsButtonBox = pDev;
	    for (i=1; i<=32; i++)
	      map[i] = i;
	    InitButtonClassDeviceStruct(pDev, 32, map);
	    InitLedFeedbackClassDeviceStruct(pDev, wsButtonBoxControl);
	    InitFocusClassDeviceStruct(pDev);
	    typeatom = MakeAtom("BUTTONBOX", 9, FALSE);
	    AssignTypeAndName(pDev, typeatom, "BUTTONBOX");
	    break;
	  case DEVICE_ON: 
	    pDev->on = TRUE;
	    break;
	  case DEVICE_OFF: 
	    pDev->on = FALSE;
	    break;
	  case DEVICE_CLOSE:
	    if (pcmFd) {
	      close(pcmFd);
	      pcmFd = 0;
	    }
	    break;
	}
	return Success;
}

static void
wsDialBoxControl(device, ctrl)
DevicePtr device;
PtrCtrl *ctrl;
{
	struct dial_control control;

	control.smoothing = ctrl->num / ctrl->den;
	control.threshold = ctrl->threshold;

	/*
	   InitPtrFeedbackClassDeviceStruct should not call the control
	     routine for extension devices with defaultPointerControl values,
	     which may be wrong, don't think there is a fix, so live w/ it
	*/

	/* hardware should not freak out when given a threshold of 0
	     and a smoothing factor of 0 */
	if (!control.smoothing)
	  control.smoothing = 1;
	if (!control.threshold)
	  control.threshold = 1;

	if (ioctl(pcmFd, PIOSETKNOB, &control) < 0) {
	  Error("wsDialBoxControl");
	}
}

static int
wsDialBoxProc(pDev, onoff, argc, argv)
DevicePtr pDev;
int onoff, argc;
char *argv[];
{
	Atom typeatom;

	switch (onoff)
	{
	  case DEVICE_INIT: 
	    wsDialBox = pDev;
	    InitValuatorClassDeviceStruct(pDev, 8, wsGetMotionEvents,
		MOTION_BUFFER_SIZE, Relative);
	    InitValuatorAxisStruct(pDev, 0, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 1, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 2, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 3, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 4, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 5, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 6, -32768, 32767, 1);
	    InitValuatorAxisStruct(pDev, 7, -32768, 32767, 1);
	    InitPtrFeedbackClassDeviceStruct(pDev, wsDialBoxControl);
	    InitFocusClassDeviceStruct(pDev);
	    typeatom = MakeAtom("KNOB_BOX", 8, FALSE);
	    AssignTypeAndName(pDev, typeatom, "KNOB_BOX");
	    break;
	  case DEVICE_ON: 
	    pDev->on = TRUE;
	    break;
	  case DEVICE_OFF: 
	    pDev->on = FALSE;
	    break;
	  case DEVICE_CLOSE:
	    if (pcmFd) {
	      close(pcmFd);
	      pcmFd = 0;
	    }
	    break;
	}
	return Success;
}

static int
wsTabletProc(pDev, onoff, argc, argv)
DevicePtr pDev;
int onoff, argc;
char *argv[];
{

	BYTE    map[5];
	Atom typeatom;

	switch (onoff)
	{
	  case DEVICE_INIT: 
	    wsTablet = pDev;
	    map[1] = 1;
	    map[2] = 2;
	    map[3] = 3;
	    map[4] = 4;
	    /* InitPointerDeviceStruct(
		pDev, map, 4, wsGetMotionEvents,
		wsChangeTabletControl, MOTION_BUFFER_SIZE); */
	    InitButtonClassDeviceStruct(pDev, 4, map);
	    InitValuatorClassDeviceStruct(pDev, 2, wsGetMotionEvents,
		MOTION_BUFFER_SIZE, Absolute);
	    InitValuatorAxisStruct(pDev, 0, 0, 2199, 7874);
	    InitValuatorAxisStruct(pDev, 1, 0, 2199, 7874);
	    					/* 200 * 39.37 XXX */
/*	    InitPtrFeedbackClassDeviceStruct(pDev, wsTabletControl); */
	    InitProximityClassDeviceStruct(pDev);
	    InitFocusClassDeviceStruct(pDev);
	    typeatom = MakeAtom("TABLET", 6, FALSE);
	    AssignTypeAndName(pDev, typeatom, "TABLET");
	    break;
	  case DEVICE_ON: 
	    pDev->on = TRUE;
/* only need to select off mouse file descriptor */
/*	    AddEnabledDevice(tbFd);  */
	    break;
	  case DEVICE_OFF: 
	    pDev->on = FALSE;
/*	    RemoveEnabledDevice(tbFd); */
	    break;
	  case DEVICE_CLOSE:
	    close(tbFd);
	    break;
	}
	return Success;
}

#ifdef notdef
void
wsChangeTabletControl(device, ctrl)
    DevicePtr device;
    TabletCtrl   *ctrl;
{
/* XXX */
}
#endif
#endif
