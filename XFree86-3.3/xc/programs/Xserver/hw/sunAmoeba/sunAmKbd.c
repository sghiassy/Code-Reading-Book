/* $XConsortium: sunAmKbd.c,v 1.2 94/04/17 20:32:47 dpw Exp $ */
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

/* This is a modified version of sunKbd.c (version 5.39 94/02/23 15:55:52),
 * making it work for Amoeba.
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

#define NEED_EVENTS
#include "sun.h"
#include "keysym.h"
#include "Sunkeysym.h"

#include <amoeba.h>
#include <cmdreg.h>
#include <stdcom.h>
#include <stderr.h>
#include <server/iop/iop.h>

extern capability	iopcap;

#define SUN_LED_MASK	0x0f
#define MIN_KEYCODE	8	/* necessary to avoid the mouse buttons */
#define MAX_KEYCODE	255	/* limited by the protocol */
#ifndef KB_SUN4 
#define KB_SUN4		4
#endif

#define AUTOREPEAT_INITIATE	200
#define AUTOREPEAT_DELAY	50

extern KeySymsRec sunKeySyms[];
extern SunModmapRec *sunModMaps[];
extern int sunMaxLayout;
extern KeySym *sunType4KeyMaps[];
extern SunModmapRec *sunType4ModMaps[];

long	  	  sunAutoRepeatInitiate = 1000 * AUTOREPEAT_INITIATE;
long	  	  sunAutoRepeatDelay = 1000 * AUTOREPEAT_DELAY;

static int		autoRepeatKeyDown = 0;
static int		autoRepeatReady;
static int		autoRepeatFirst;
long			autoRepeatLastKeyDown;
long			autoRepeatDelta;

static sunKbdPrivRec  	sysKbPriv = {
    -1,			/* Type */
    -1,			/* Layout */
    0,			/* click */
    (Leds)0		/* leds */
};

static void SwapKeys(keysyms)
    KeySymsRec* keysyms;
{
    unsigned int i;
    KeySym k;

    for (i = 2; i < keysyms->maxKeyCode * keysyms->mapWidth; i++)
	if (keysyms->map[i] == XK_L1 ||
	    keysyms->map[i] == XK_L2 ||
	    keysyms->map[i] == XK_L3 ||
	    keysyms->map[i] == XK_L4 ||
	    keysyms->map[i] == XK_L5 ||
	    keysyms->map[i] == XK_L6 ||
	    keysyms->map[i] == XK_L7 ||
	    keysyms->map[i] == XK_L8 ||
	    keysyms->map[i] == XK_L9 ||
	    keysyms->map[i] == XK_L10) {
	    /* yes, I could have done a clever two line swap! */
	    k = keysyms->map[i - 2];
	    keysyms->map[i - 2] = keysyms->map[i];
	    keysyms->map[i] = k;
	}
}

static void SetLights (ctrl)
    KeybdCtrl*	ctrl;
{
    /* TODO */
}


static void ModLight (device, on, led)
    DeviceIntPtr device;
    Bool	on;
    int		led;
{
    KeybdCtrl*	ctrl = &device->kbdfeed->ctrl;
    sunKbdPrivPtr pPriv = (sunKbdPrivPtr) device->public.devicePrivate;

    if(on) {
	ctrl->leds |= led;
	pPriv->leds |= led;
    } else {
	ctrl->leds &= ~led;
	pPriv->leds &= ~led;
    }
    SetLights (ctrl);
}

/*-
 *-----------------------------------------------------------------------
 * sunBell --
 *	Ring the terminal/keyboard bell
 *
 * Results:
 *	Ring the keyboard bell for an amount of time proportional to
 *	"loudness."
 *
 * Side Effects:
 *	None, really...
 *
 *-----------------------------------------------------------------------
 */

#if NeedFunctionPrototypes
static void sunBell (
    int		    percent,
    DeviceIntPtr    device,
    pointer	    ctrl,
    int		    unused)
#else
static void sunBell (percent, device, ctrl, unused)
    int		    percent;	    /* Percentage of full volume */
    DeviceIntPtr    device;	    /* Keyboard to ring */
    pointer	    ctrl;
    int		    unused;
#endif
{
    int		    kbdCmd;   	    /* Command to give keyboard */
    KeybdCtrl*      kctrl = (KeybdCtrl*) ctrl;
 
    if (percent == 0 || kctrl->bell == 0)
 	return;

    (void) iop_ringbell(&iopcap, percent,
                        kctrl->bell_pitch, kctrl->bell_duration);
}


static void EnqueueEvent (xE)
    xEvent* xE;
{
    mieqEnqueue (xE);
}

#define XLED_NUM_LOCK    0x1
#define XLED_COMPOSE     0x4
#define XLED_SCROLL_LOCK 0x2
#define XLED_CAPS_LOCK   0x8

static KeyCode LookupKeyCode (keysym, keysymsrec)
    KeySym keysym;
    KeySymsPtr keysymsrec;
{
    KeyCode i;
    int ii, index = 0;

    for (i = keysymsrec->minKeyCode; i < keysymsrec->maxKeyCode; i++)
	for (ii = 0; ii < keysymsrec->mapWidth; ii++)
	    if (keysymsrec->map[index++] == keysym)
		return i;
}

static void pseudoKey(device, down, keycode)
    DeviceIntPtr device;
    Bool down;
    KeyCode keycode;
{
    int bit;
    CARD8 modifiers;
    CARD16 mask;
    BYTE* kptr;

    kptr = &device->key->down[keycode >> 3];
    bit = 1 << (keycode & 7);
    modifiers = device->key->modifierMap[keycode];
    if (down) {
	/* fool dix into thinking this key is now "down" */
	int i;
	*kptr |= bit;
	device->key->prev_state = device->key->state;
	for (i = 0, mask = 1; modifiers; i++, mask <<= 1)
	    if (mask & modifiers) {
		device->key->modifierKeyCount[i]++;
		device->key->state += mask;
		modifiers &= ~mask;
	    }
    } else {
	/* fool dix into thinking this key is now "up" */
	if (*kptr & bit) {
	    int i;
	    *kptr &= ~bit;
	    device->key->prev_state = device->key->state;
	    for (i = 0, mask = 1; modifiers; i++, mask <<= 1)
		if (mask & modifiers) {
		    if (--device->key->modifierKeyCount[i] <= 0) {
			device->key->state &= ~mask;
			device->key->modifierKeyCount[i] = 0;
		    }
		    modifiers &= ~mask;
		}
	}
    }
}

static void DoLEDs(device, ctrl, pPriv)
    DeviceIntPtr    device;	    /* Keyboard to alter */
    KeybdCtrl* ctrl;
    sunKbdPrivPtr pPriv;
{
    if ((ctrl->leds & XLED_CAPS_LOCK) && !(pPriv->leds & XLED_CAPS_LOCK))
	    pseudoKey(device, TRUE,
		LookupKeyCode(XK_Caps_Lock, &device->key->curKeySyms));

    if (!(ctrl->leds & XLED_CAPS_LOCK) && (pPriv->leds & XLED_CAPS_LOCK))
	    pseudoKey(device, FALSE,
		LookupKeyCode(XK_Caps_Lock, &device->key->curKeySyms));

    if ((ctrl->leds & XLED_NUM_LOCK) && !(pPriv->leds & XLED_NUM_LOCK))
	    pseudoKey(device, TRUE,
		LookupKeyCode(XK_Num_Lock, &device->key->curKeySyms));

    if (!(ctrl->leds & XLED_NUM_LOCK) && (pPriv->leds & XLED_NUM_LOCK))
	    pseudoKey(device, FALSE,
		LookupKeyCode(XK_Num_Lock, &device->key->curKeySyms));

    if ((ctrl->leds & XLED_SCROLL_LOCK) && !(pPriv->leds & XLED_SCROLL_LOCK))
	    pseudoKey(device, TRUE,
		LookupKeyCode(XK_Scroll_Lock, &device->key->curKeySyms));

    if (!(ctrl->leds & XLED_SCROLL_LOCK) && (pPriv->leds & XLED_SCROLL_LOCK))
	    pseudoKey(device, FALSE,
		LookupKeyCode(XK_Scroll_Lock, &device->key->curKeySyms));

    if ((ctrl->leds & XLED_COMPOSE) && !(pPriv->leds & XLED_COMPOSE))
	    pseudoKey(device, TRUE,
		LookupKeyCode(SunXK_Compose, &device->key->curKeySyms));

    if (!(ctrl->leds & XLED_COMPOSE) && (pPriv->leds & XLED_COMPOSE))
	    pseudoKey(device, FALSE,
		LookupKeyCode(SunXK_Compose, &device->key->curKeySyms));

    pPriv->leds = ctrl->leds & 0x0f;
    SetLights (ctrl);
}

/*-
 *-----------------------------------------------------------------------
 * sunKbdCtrl --
 *	Alter some of the keyboard control parameters
 *
 * Results:
 *	None.
 *
 * Side Effects:
 *	Some...
 *
 *-----------------------------------------------------------------------
 */

#if NeedFunctionPrototypes
static void sunKbdCtrl (
    DeviceIntPtr    device,
    KeybdCtrl*	    ctrl)
#else
static void sunKbdCtrl (device, ctrl)
    DeviceIntPtr    device;	    /* Keyboard to alter */
    KeybdCtrl*	    ctrl;
#endif
{
    sunKbdPrivPtr pPriv = (sunKbdPrivPtr) device->public.devicePrivate;

    if (ctrl->click != pPriv->click) {
	pPriv->click = ctrl->click;
	(void) iop_keyclick(&iopcap, ctrl->click);
    }
    if (pPriv->type == KB_SUN4 && pPriv->leds != ctrl->leds & 0x0f)
	DoLEDs(device, ctrl, pPriv);
}

/*-
 *-----------------------------------------------------------------------
 * sunKbdProc --
 *	Handle the initialization, etc. of a keyboard.
 *
 * Results:
 *	None.
 *
 *-----------------------------------------------------------------------
 */

#if NeedFunctionPrototypes
int sunKbdProc (
    DeviceIntPtr  device,
    int	    	  what)
#else
int sunKbdProc (device, what)
    DeviceIntPtr  device;	/* Keyboard to manipulate */
    int	    	  what;	    	/* What to do to it */
#endif
{
    int i;
    DevicePtr pKeyboard = (DevicePtr) device;
    sunKbdPrivPtr pPriv;
    KeybdCtrl*	ctrl = &device->kbdfeed->ctrl;

    static CARD8 *workingModMap = NULL;
    static KeySymsRec *workingKeySyms;

    switch (what) {
    case DEVICE_INIT:
	if (pKeyboard != LookupKeyboardDevice()) {
	    ErrorF ("Cannot open non-system keyboard\n");
	    return (!Success);
	}
	    
	/*
	 * First open and find the current state of the keyboard.
	 */
	if (!workingKeySyms) {
	    errstat err;

            if ((err = iop_kbdtype(&iopcap, &sysKbPriv.type)) != STD_OK) {
                ErrorF("Xsun: iop_kbdtype failed: %s\n", err_why(err));
                FatalError("Cannot get keyboard type\n");
            }
            if (sysKbPriv.type == KB_SUN4) {
	        /* TODO: we also need to get the layout from iop_kbdtype.
		 * For now we'll just assume layout 0 by default.
		 */
		sysKbPriv.layout = 0;
                sunKeySyms[KB_SUN4].map = sunType4KeyMaps[sysKbPriv.layout];
                sunModMaps[KB_SUN4] = sunType4ModMaps[sysKbPriv.layout];
            }

            if (sysKbPriv.type < 0 || sysKbPriv.type > KB_SUN4 ||
                sunKeySyms[sysKbPriv.type].map == NULL)
	    {
	        FatalError("Unsupported keyboard type %d\n",sysKbPriv.type);
	    }

	    /*
	     * Perform final initialization of the system private keyboard
	     * structure and fill in various slots in the device record
	     * itself which couldn't be filled in before.
	     */
	    (void) memset ((void *) defaultKeyboardControl.autoRepeats,
			   ~0, sizeof defaultKeyboardControl.autoRepeats);
	    autoRepeatKeyDown = 0;
	    /*
	     * Initialize the keysym map
	     */
	    workingKeySyms = &sunKeySyms[sysKbPriv.type];

	    /*
	     * Create and initialize the modifier map.
	     */
	    if (!workingModMap) {
		workingModMap=(CARD8 *)xalloc(MAP_LENGTH);
		(void) memset(workingModMap, 0, MAP_LENGTH);
		for(i=0; sunModMaps[sysKbPriv.type][i].key != 0; i++)
		    workingModMap[sunModMaps[sysKbPriv.type][i].key + MIN_KEYCODE] = 
			sunModMaps[sysKbPriv.type][i].modifiers;
	    }

	    if (sysKbPriv.type == KB_SUN4 && sunSwapLkeys)
		SwapKeys(workingKeySyms);
	    /*
	     * ensure that the keycodes on the wire are >= MIN_KEYCODE
	     * and <= MAX_KEYCODE
	     */
	    if (workingKeySyms->minKeyCode < MIN_KEYCODE) {
		workingKeySyms->minKeyCode += MIN_KEYCODE;
		workingKeySyms->maxKeyCode += MIN_KEYCODE;
	    }
	    if (workingKeySyms->maxKeyCode > MAX_KEYCODE)
		workingKeySyms->maxKeyCode = MAX_KEYCODE;
	}
	pKeyboard->devicePrivate = (pointer)&sysKbPriv;
	pKeyboard->on = FALSE;

	InitKeyboardDeviceStruct(pKeyboard, 
				 workingKeySyms, workingModMap,
				 sunBell, sunKbdCtrl);
	break;

    case DEVICE_ON:
	pPriv = (sunKbdPrivPtr)pKeyboard->devicePrivate;
	pKeyboard->on = TRUE;
	break;

    case DEVICE_CLOSE:
    case DEVICE_OFF:
	pPriv = (sunKbdPrivPtr)pKeyboard->devicePrivate;
	if (pPriv->type == KB_SUN4) {
	    /* dumb bug in Sun's keyboard! Turn off LEDS before resetting */
	    pPriv->leds = 0;
	    ctrl->leds = 0;
	    SetLights(ctrl);
	}
	pKeyboard->on = FALSE;
	break;
    default:
	FatalError("Unknown keyboard operation\n");
    }
    return Success;
}

/*-
 *-----------------------------------------------------------------------
 * sunKbdEnqueueEvent --
 *
 *-----------------------------------------------------------------------
 */
static xEvent	autoRepeatEvent;

static Bool DoSpecialKeys(device, xE)
    DeviceIntPtr  device;
    xEvent*       xE;
{
    int	shift_index, map_index, bit;
    KeySym ksym;
    BYTE* kptr;
    sunKbdPrivPtr pPriv = (sunKbdPrivPtr)device->public.devicePrivate;
    BYTE keycode = xE->u.u.detail;
    CARD8 keyModifiers = device->key->modifierMap[keycode];

    /* look up the present idea of the keysym */
    shift_index = 0;
    if (device->key->state & ShiftMask) 
	shift_index ^= 1;
    if (device->key->state & LockMask) 
	shift_index ^= 1;
    /* TODO: find out how to get a map_index under Amoeba.
     * For now just ignore special keys.
     */
    ksym = 0;

    if (ksym == XK_Num_Lock) {
	if (pPriv->type == KB_SUN4)
	    ModLight (device, xE->u.u.type == KeyPress, XLED_NUM_LOCK);
    } else if (ksym == XK_Scroll_Lock) {
	if (pPriv->type == KB_SUN4)
	    ModLight (device, xE->u.u.type == KeyPress, XLED_SCROLL_LOCK);
    } else if (ksym == SunXK_Compose) {
	if (pPriv->type == KB_SUN4)
	    ModLight (device, xE->u.u.type == KeyPress, XLED_COMPOSE);
    } else if (keyModifiers & LockMask) {
	if (pPriv->type == KB_SUN4)
	    ModLight (device, xE->u.u.type == KeyPress, XLED_CAPS_LOCK);
    } else if ((xE->u.u.type == KeyPress) && (keyModifiers == 0)) {
	/* initialize new AutoRepeater event & mark AutoRepeater on */
	autoRepeatEvent = *xE;
	autoRepeatFirst = TRUE;
	autoRepeatKeyDown++;
	autoRepeatLastKeyDown = GetTimeInMillis();
    }
    return FALSE;
}


void
sunKbdProcessEvent(device, xeva)
    DeviceIntPtr   device;
    xEvent        *xeva;
{
    xEvent		xE;
    BYTE		keycode;
    CARD8		keyModifiers;

    xE = *xeva;
    xE.u.u.detail &= 0x7f;
    xE.u.u.detail += MIN_KEYCODE;
    keycode = xE.u.u.detail;
    keyModifiers = device->key->modifierMap[keycode];
    if (autoRepeatKeyDown && (keyModifiers == 0) &&
	(xE.u.u.type == KeyPress || (keycode == autoRepeatEvent.u.u.detail))) {
	/*
	 * Kill AutoRepeater on any real non-modifier key down, or auto key up
	 */
	autoRepeatKeyDown = 0;
    }
    if (DoSpecialKeys(device, &xE))
	return;
    mieqEnqueue (&xE);
}

void sunEnqueueAutoRepeat ()
{
    int	delta;
    int	i, mask;
    KeybdCtrl* ctrl = &((DeviceIntPtr)LookupKeyboardDevice())->kbdfeed->ctrl;

    if (ctrl->autoRepeat != AutoRepeatModeOn) {
	autoRepeatKeyDown = 0;
	return;
    }
    i=(autoRepeatEvent.u.u.detail >> 3);
    mask=(1 << (autoRepeatEvent.u.u.detail & 7));
    if (!(ctrl->autoRepeats[i] & mask)) {
	autoRepeatKeyDown = 0;
	return;
    }

    /*
     * Generate auto repeat event.	XXX one for now.
     * Update time & pointer location of saved KeyPress event.
     */

    delta = autoRepeatDelta;
    autoRepeatFirst = FALSE;

    /*
     * Fake a key up event and a key down event
     * for the last key pressed.
     */
    autoRepeatEvent.u.keyButtonPointer.time += delta;
    autoRepeatEvent.u.u.type = KeyRelease;

    EnqueueEvent (&autoRepeatEvent);
    autoRepeatEvent.u.u.type = KeyPress;
    EnqueueEvent (&autoRepeatEvent);

    /* Update time of last key down */
    autoRepeatLastKeyDown += autoRepeatDelta;
}

/*ARGSUSED*/
Bool LegalModifier(key, pDev)
    unsigned int key;
    DevicePtr	pDev;
{
    return TRUE;
}

/*ARGSUSED*/
void sunBlockHandler(nscreen, pbdata, pptv, pReadmask)
    int nscreen;
    pointer pbdata;
    struct timeval **pptv;
    pointer pReadmask;
{
    KeybdCtrl* ctrl = &((DeviceIntPtr)LookupKeyboardDevice())->kbdfeed->ctrl;
    static struct timeval artv = { 0, 0 };	/* autorepeat timeval */

    if (!autoRepeatKeyDown)
	return;

    if (ctrl->autoRepeat != AutoRepeatModeOn)
	return;

    if (autoRepeatFirst == TRUE)
	artv.tv_usec = sunAutoRepeatInitiate;
    else
	artv.tv_usec = sunAutoRepeatDelay;
    *pptv = &artv;

}

/*ARGSUSED*/
void sunWakeupHandler(nscreen, pbdata, err, pReadmask)
    int nscreen;
    pointer pbdata;
    unsigned long err;
    pointer pReadmask;
{
    KeybdCtrl* ctrl = &((DeviceIntPtr)LookupKeyboardDevice())->kbdfeed->ctrl;
    struct timeval tv;

    if (ctrl->autoRepeat != AutoRepeatModeOn)
	return;

    if (autoRepeatKeyDown) {
        register long tv = GetTimeInMillis();

        autoRepeatDelta = tv - autoRepeatLastKeyDown;
        if ((!autoRepeatFirst && (autoRepeatDelta > (sunAutoRepeatDelay/1000)))
	    || (autoRepeatDelta > (sunAutoRepeatInitiate/1000)))
	{
                autoRepeatReady++;
	}
    }
    
    if (autoRepeatReady)
    {
	sunEnqueueAutoRepeat ();
	autoRepeatReady = 0;
    }
}
