/* $XConsortium: sunAmMouse.c,v 1.2 94/04/17 20:32:48 dpw Exp $ */

/*-
 * Copyright (c) 1987 by the Regents of the University of California
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  The University of California
 * makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without
 * express or implied warranty.
 */

/* This is a modified version of sunMouse.c, making it work for Amoeba */

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
 * Copyright 1991, 1992, 1993 Kaleb S. Keithley
 *
 * Permission to use, copy, modify, and distribute this
 * software and its documentation for any purpose and without
 * fee is hereby granted, provided that the above copyright
 * notice appear in all copies.  Kaleb S. Keithley makes no 
 * representations about the suitability of this software for 
 * any purpose.  It is provided "as is" without express or 
 * implied warranty.
 */

#define NEED_EVENTS
#include    "sun.h"

Bool sunActiveZaphod = TRUE;

static Bool sunCursorOffScreen();
static void sunCrossScreen();
static void sunWarpCursor();

miPointerScreenFuncRec sunPointerScreenFuncs = {
    sunCursorOffScreen,
    sunCrossScreen,
    sunWarpCursor,
};

static sunPtrPrivRec sysMousePriv = {
    0,
    0,	/* Current button state */
    0,
    0,
    0
};

/*-
 *-----------------------------------------------------------------------
 * sunMouseCtrl --
 *	Alter the control parameters for the mouse. Since acceleration
 *	etc. is done from the PtrCtrl record in the mouse's device record,
 *	there's nothing to do here.
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
/*ARGSUSED*/
static 
#if NeedFunctionPrototypes
void sunMouseCtrl (
    DeviceIntPtr    device,
    PtrCtrl*	    ctrl)
#else
void sunMouseCtrl (device, ctrl)
    DeviceIntPtr    device;
    PtrCtrl*	    ctrl;
#endif
{
}

/*-
 *-----------------------------------------------------------------------
 * sunMouseProc --
 *	Handle the initialization, etc. of a mouse
 *
 * Results:
 *	none.
 *
 * Side Effects:
 *
 * Note:
 *	When using sunwindows, all input comes off a single fd, stored in the
 *	global windowFd.  Therefore, only one device should be enabled and
 *	disabled, even though the application still sees both mouse and
 *	keyboard.  We have arbitrarily chosen to enable and disable windowFd
 *	in the keyboard routine sunKbdProc rather than in sunMouseProc.
 *
 *-----------------------------------------------------------------------
 */
#if NeedFunctionPrototypes
int sunMouseProc (
    DeviceIntPtr  device,
    int	    	  what)
#else
int sunMouseProc (device, what)
    DeviceIntPtr  device;   	/* Mouse to play with */
    int	    	  what;	    	/* What to do with it */
#endif
{
    DevicePtr	  pMouse = (DevicePtr) device;
    int	    	  format;
    static int	  oformat;
    BYTE    	  map[4];
    char	  *dev;

    switch (what) {
	case DEVICE_INIT:
	    if (pMouse != LookupPointerDevice()) {
		ErrorF ("Cannot open non-system mouse");	
		return !Success;
	    }
	    pMouse->devicePrivate = (pointer) &sysMousePriv;
	    pMouse->on = FALSE;
	    map[1] = 1;
	    map[2] = 2;
	    map[3] = 3;
	    InitPointerDeviceStruct(
		pMouse, map, 3, miPointerGetMotionEvents,
 		sunMouseCtrl, miPointerGetMotionBufferSize());
	    break;

	case DEVICE_ON:
	    pMouse->on = TRUE;
	    break;

	case DEVICE_CLOSE:
	    break;

	case DEVICE_OFF:
	    pMouse->on = FALSE;
	    break;
    }
    return Success;
}
    
/*-
 *-----------------------------------------------------------------------
 * MouseAccelerate --
 *	Given a delta and a mouse, return the acceleration of the delta.
 *
 * Results:
 *	The corrected delta
 *
 * Side Effects:
 *	None.
 *
 *-----------------------------------------------------------------------
 */
static short
MouseAccelerate (device, delta)
    DeviceIntPtr  device;
    int	    	  delta;
{
    int  sgn = sign(delta);
    PtrCtrl *pCtrl;
    short ret;

    delta = abs(delta);
    pCtrl = &device->ptrfeed->ctrl;
    if (delta > pCtrl->threshold) {
	ret = 
	    (short) sgn * 
		(pCtrl->threshold + ((delta - pCtrl->threshold) * pCtrl->num) /
		    pCtrl->den);
    } else {
	ret = (short) sgn * delta;
    }
    return ret;
}

/*
 * Given an xEvent for a mouse, pass it off the the dix layer
 * properly converted ...
 */
void
sunMouseProcessEvent(pMouse, xeva)
    DevicePtr     pMouse;               /* mouse from which the event came */
    xEvent        *xeva;
{
    int                 index;          /* screen index */
    xEvent              xE;
    sunPtrPrivPtr	pPriv;          /* private data for pointer */
    register int        bmask;          /* temporary button mask */
    int                 x,y;

    xE = *xeva;
    pPriv = (sunPtrPrivPtr)pMouse->devicePrivate;

    bmask = pPriv->bmask;
    switch (xE.u.u.type) {
    case ButtonRelease:
        if (!(bmask & (1 << xE.u.u.detail))) {
	    /* This may happen when the server starts up; ignore it */
            /* ErrorF("Xsun: Ignoring spurious button release\n"); */
            return;
        }
        pPriv->bmask &= ~(1<<xE.u.u.detail);
        break;
    case ButtonPress:
        if ((bmask & (1 << xE.u.u.detail))) {
            ErrorF("Xsun: Ignoring spurious button press\n");
            return;
        }
        pPriv->bmask |= (1<<xE.u.u.detail);
        break;
    case MotionNotify:
        /*
         * When we detect a change in the mouse coordinates, we call
         * the cursor module to move the cursor. It has the option of
         * simply removing the cursor or just shifting it a bit.
         * If it is removed, DIX will restore it before we goes to sleep...
         *
         * What should be done if it goes off the screen? Move to another
         * screen? For now, we just force the pointer to stay on the
         * screen...
         */
        pPriv->dx += MouseAccelerate(pMouse, xE.u.keyButtonPointer.rootX);
        pPriv->dy += MouseAccelerate(pMouse, xE.u.keyButtonPointer.rootY);
        pPriv->mouseMoved = TRUE;
        return;
    default:
        FatalError("sunMouseProcessEvent: unrecognized id\n");
        break;
    }

    miPointerPosition(&x, &y);
    xE.u.keyButtonPointer.rootX = x;
    xE.u.keyButtonPointer.rootY = y;
    mieqEnqueue(&xE);
}

/*
 * Finish off any mouse motions we haven't done yet
 */
/*ARGSUSED*/
void
sunMouseDoneEvents(pMouse,final)
    DevicePtr     pMouse;
    Bool          final;
{
    sunPtrPrivPtr pPriv;
    xEvent        xE;
    int           dx, dy;

    pPriv = (sunPtrPrivPtr) pMouse->devicePrivate;

    if (pPriv->mouseMoved) {
        dx = pPriv->dx;
        dy = pPriv->dy;
        pPriv->dx = pPriv->dy = 0;
        pPriv->mouseMoved = FALSE;
        miPointerDeltaCursor(dx, dy, TimeSinceLastInputEvent());
    }
}

/*ARGSUSED*/
static Bool
sunCursorOffScreen (pScreen, x, y)
    ScreenPtr	*pScreen;
    int		*x, *y;
{
    extern Bool PointerConfinedToScreen();

    if (PointerConfinedToScreen()) return TRUE;
	
    /* since we support only one screen anyway: */
    return FALSE;
}

static void
sunCrossScreen (pScreen, entering)
    ScreenPtr	pScreen;
    Bool	entering;
{
    if (sunFbs[pScreen->myNum].EnterLeave)
	(*sunFbs[pScreen->myNum].EnterLeave) (pScreen, entering ? 0 : 1);
}

static void
sunWarpCursor (pScreen, x, y)
    ScreenPtr	pScreen;
    int		x, y;
{
    miPointerWarpCursor (pScreen, x, y);
}
