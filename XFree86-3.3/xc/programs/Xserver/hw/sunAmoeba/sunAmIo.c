/* $XConsortium: sunAmIo.c,v 1.1 94/04/01 17:54:49 dpw Exp $ */
/*-
 * sunAmIo.c --
 *	Amoeba implementation of functions to handle input from
 *	the keyboard and mouse.
 *
 * Copyright (c) 1994 by the Vrije Universiteit, Amsterdam
 * Copyright (c) 1994 by the Regents of the University of California
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The Vrije Universiteit and
 * the University of California make no representations about
 * the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 */

#define NEED_EVENTS
#include    "sun.h"

#include    "amoeba.h"
#include    "server/iop/iop.h"

static int lastEventTime;

int
TimeSinceLastInputEvent()
{
    /* Return the time in milliseconds since there last was any input */
    register long   now;

    now = GetTimeInMillis();
    if (lastEventTime == 0)
        lastEventTime = now;
    return (now - lastEventTime);
}

void
SetTimeSinceLastInputEvent()
{
    lastEventTime = GetTimeInMillis();
}

/*-
 *-----------------------------------------------------------------------
 * ProcessInputEvents --
 *	Retrieve all waiting input events and pass them to DIX in their
 *	correct chronological order. Only reads from the system pointer
 *	and keyboard.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Events are passed to the DIX layer.
 *
 *-----------------------------------------------------------------------
 */
void
ProcessInputEvents ()
{
    DevicePtr               pPointer;
    DevicePtr               pKeyboard;
    register sunPtrPrivPtr  ptrPriv;
    register sunKbdPrivPtr  kbdPriv;
    int                     i;
    register IOPEvent       *e, *elast;
    IOPEvent                events[MAXEVENTS];
    xEvent                  x;
    static int              firsttime = TRUE;

    pPointer = LookupPointerDevice();
    pKeyboard = LookupKeyboardDevice();
    ptrPriv = (sunPtrPrivPtr)pPointer->devicePrivate;
    kbdPriv = (sunKbdPrivPtr)pKeyboard->devicePrivate;

    while ((i = AmoebaGetEvents(events, MAXEVENTS)) > 0) {
        for (e = &events[0], elast = &events[i]; e < elast; e++) {
            x.u.keyButtonPointer.time = lastEventTime = e->time;
            switch (e->type) {
            case EV_PointerDelta:
                x.u.u.type = MotionNotify;
                x.u.keyButtonPointer.rootX = e->x;
                x.u.keyButtonPointer.rootY = e->y;
                sunMouseProcessEvent(pPointer, &x);
                break;
             case EV_ButtonPress:
                x.u.u.type = ButtonPress;
                x.u.u.detail = e->keyorbut;
                sunMouseProcessEvent(pPointer, &x);
                break;
             case EV_ButtonRelease:
                x.u.u.type = ButtonRelease;
                x.u.u.detail = e->keyorbut;
                sunMouseProcessEvent(pPointer, &x);
                break;
             case EV_KeyPressEvent:
                /* device dependent code goes here */
                x.u.u.type = KeyPress;
                x.u.u.detail = e->keyorbut;
                sunKbdProcessEvent(pKeyboard, &x);
                break;
            case EV_KeyReleaseEvent:
                x.u.u.type = KeyRelease;
                x.u.u.detail = e->keyorbut;
                sunKbdProcessEvent(pKeyboard, &x);
                /* device dependent code goes here */
                break;
            default:
                /* this shouldn't happen */
                ErrorF("stray event %d (%d,%d) %x\n",
                    e->type, e->x, e->y, e->keyorbut);
                break;
            }
        }

        sunMouseDoneEvents(pPointer, TRUE);
    }

    mieqProcessInputEvents();
    miPointerUpdate();
}

/*
 * DDX - specific abort routine.  Called by AbortServer().
 */
void AbortDDX()
{
    int         i;
    ScreenPtr   pScreen;

    for (i = 0; i < screenInfo.numScreens; i++)
    {
	pScreen = screenInfo.screens[i];
	(*pScreen->SaveScreen) (pScreen, SCREEN_SAVER_OFF);
	sunDisableCursor (pScreen);
    }
}

/* Called by GiveUp(). */
void
ddxGiveUp()
{
    AbortDDX ();
}

void
ddxUseMsg()
{
    ErrorF("-swapLkeys          swap keysyms on L1..L10\n");
    ErrorF("-mono               force monochrome-only screen\n");
    ErrorF("-flipPixels         flip black and white pixels\n");
}

int
ddxProcessArgument (argc, argv, i)
    int argc;
    char *argv[];
    int i;
{
    if (strcmp (argv[i], "-swapLkeys") == 0) {  /* -swapLkeys */
	sunSwapLkeys = TRUE;
	return 1;
    }
    if (strcmp (argv[i], "-mono") == 0) {       /* -mono */
	return 1;
    }
    if (strcmp (argv[i], "-flipPixels") == 0) { /* -flipPixels */
	sunFlipPixels = TRUE;
	return 1;
    }
    return 0;
}

