/* $XConsortium: svgaIo.c,v 1.7 93/10/19 16:44:37 rws Exp $ */
/*
 * Copyright 1990,91,92,93 by Thomas Roell, Germany.
 * Copyright 1991,92,93    by SGCS (Snitily Graphics Consulting Services), USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this  permission notice appear
 * in supporting documentation, and that the name of Thomas Roell nor
 * SGCS be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 * Thomas Roell nor SGCS makes no representations about the suitability
 * of this software for any purpose. It is provided "as is" without
 * express or implied warranty.
 *
 * THOMAS ROELL AND SGCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THOMAS ROELL OR SGCS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define NEED_EVENTS
#include "svga.h"
#include "mi.h"
#include "mipointer.h"
#include "atKeynames.h"
#include "inputstr.h"
#include <sys/kd.h>

xqEventQueue *XqueQaddr = NULL;
static int          XqueSema = 0;
static int          XqueDeltaX, XqueDeltaY;
static int          XqueLastButtons = 7;
static ulong        XqueLastEventTime = 0;
static unchar       XqueScanPrefix = 0;
static Bool         XqueNumLock = FALSE;

Bool svgaInputPending = FALSE;

/*
 * ProcessInputEvents --
 *      Retrieve all waiting input events and pass them to DIX in their
 *      correct chronological order. Only reads from the system pointer
 *      and keyboard.
 */

void
ProcessInputEvents()
{
  svgaInputPending = FALSE;

  if (XqueDeltaX || XqueDeltaY) {
    miPointerDeltaCursor(XqueDeltaX, XqueDeltaY,
			 XqueLastEventTime);
    XqueDeltaX = 0;
    XqueDeltaY = 0;
  }

  mieqProcessInputEvents();
  miPointerUpdate();
}

CARD32
GetTimeInMillis()
{
  return max(XqueLastEventTime, XqueQaddr->xq_curtime);
}

/*
 * XqueRequest --
 *      Notice an i/o request from the xqueue.
 */

/* ARGSUSED */
void
XqueRequest(
    int signo
)
{
  xqEvent      *XqueEvents;
  int          XqueHead, dx, dy, id, changes;
  unchar       keycode;
  DeviceIntPtr pKeyboard, pPointer;
  PtrCtrl      *ctrl;
  xEvent       xE;
  KeyClassRec  *keyc;
  KeySym       *keysym;
  unchar       scanCode;
  Bool         down;

  XqueQaddr->xq_sigenable = 0; /* LOCK */

  XqueEvents = XqueQaddr->xq_events;
  XqueHead   = XqueQaddr->xq_head;

  pKeyboard = (DeviceIntPtr)LookupKeyboardDevice();
  pPointer = (DeviceIntPtr)LookupPointerDevice();

  keyc = pKeyboard->key;

  while (XqueHead != XqueQaddr->xq_tail)
    {
      switch(XqueEvents[XqueHead].xq_type) {
        
      case XQ_MOTION:
	dx = XqueEvents[XqueHead].xq_x;
	dy = XqueEvents[XqueHead].xq_y;

	if (dx || dy) {
	  ctrl = &(pPointer->ptrfeed->ctrl);

	  if ((abs(dx) + abs(dy)) >= ctrl->threshold) {
	    dx = (dx * ctrl->num) / ctrl->den;
	    dy = (dy * ctrl->num)/ ctrl->den;
	  }

	  XqueDeltaX += dx;
	  XqueDeltaY += dy;
	  svgaInputPending = TRUE;
	}
	/* FALLTHRU */

      case XQ_BUTTON:

	if (XqueEvents[XqueHead].xq_time <= XqueLastEventTime) {
	  xE.u.keyButtonPointer.time = ++XqueLastEventTime;
	}
	else {
	  xE.u.keyButtonPointer.time =
	    XqueLastEventTime =
	      XqueEvents[XqueHead].xq_time;
	}

	changes = (XqueEvents[XqueHead].xq_code & 7) ^ XqueLastButtons;

	while (changes)
	  {
	    id = ffs(changes);
	    changes &= ~(1 << (id-1));

	    xE.u.u.detail = (4 - id);
	    xE.u.u.type   = ((XqueEvents[XqueHead].xq_code & (1l << (id -1)))
			     ? ButtonRelease
			     : ButtonPress);
	    mieqEnqueue(&xE);

	    svgaInputPending = TRUE;
	  }

	XqueLastButtons = (XqueEvents[XqueHead].xq_code & 7);
        break;


      case XQ_KEY:

	/*
	 * First do some special scancode remapping ...
	 */
	scanCode = XqueEvents[XqueHead].xq_code & 0x7f;
	down     = (XqueEvents[XqueHead].xq_code & 0x80 ? FALSE : TRUE);

	if (XqueScanPrefix == 0) {

	  switch (scanCode) {
      
	  case KEY_Prefix0:
	  case KEY_Prefix1:
	    XqueScanPrefix = scanCode;  /* special prefixes */
	    goto skip_event;

	  default:
	    break;
	  }
#ifndef XKB
	  if ((!XqueNumLock && !ModifierDown(ShiftMask)) ||
	      (XqueNumLock && ModifierDown(ShiftMask))) {
	    /*
	     * Hardwired numlock handling ... (Some applications break if
	     * they have these keys double defined, like twm)
	     */
	    switch (scanCode) {
	    case KEY_KP_7:       scanCode = KEY_Home;   break;
	    case KEY_KP_8:       scanCode = KEY_Up;     break;
	    case KEY_KP_9:       scanCode = KEY_PgUp;   break;
	    case KEY_KP_4:       scanCode = KEY_Left;   break;
	    case KEY_KP_5:       scanCode = KEY_Begin;  break;
	    case KEY_KP_6:       scanCode = KEY_Right;  break;
	    case KEY_KP_1:       scanCode = KEY_End;    break;
	    case KEY_KP_2:       scanCode = KEY_Down;   break;
	    case KEY_KP_3:       scanCode = KEY_PgDown; break;
	    case KEY_KP_0:       scanCode = KEY_Insert; break;
	    case KEY_KP_Decimal: scanCode = KEY_Delete; break;
	    }
	  }
#endif
	}
        
	else if (XqueScanPrefix == KEY_Prefix0) {
    
	  XqueScanPrefix = 0;
          
	  switch (scanCode) {
	  case KEY_KP_7:        scanCode = KEY_Home;      break;
	  case KEY_KP_8:        scanCode = KEY_Up;        break;
	  case KEY_KP_9:        scanCode = KEY_PgUp;      break;
	  case KEY_KP_4:        scanCode = KEY_Left;      break;
	  case KEY_KP_5:        scanCode = KEY_Begin;     break;
	  case KEY_KP_6:        scanCode = KEY_Right;     break;
	  case KEY_KP_1:        scanCode = KEY_End;       break;
	  case KEY_KP_2:        scanCode = KEY_Down;      break;
	  case KEY_KP_3:        scanCode = KEY_PgDown;    break;
	  case KEY_KP_0:        scanCode = KEY_Insert;    break;
	  case KEY_KP_Decimal:  scanCode = KEY_Delete;    break;
	  case KEY_Enter:       scanCode = KEY_KP_Enter;  break;
	  case KEY_LCtrl:       scanCode = KEY_RCtrl;     break;
	  case KEY_KP_Multiply: scanCode = KEY_Print;     break;
	  case KEY_Slash:       scanCode = KEY_KP_Divide; break;
	  case KEY_Alt:         scanCode = KEY_AltLang;   break;
	  case KEY_ScrollLock:  scanCode = KEY_Break;     break;
	    /*
	     * Ignore virtual shifts (E0 2A, E0 AA, E0 36, E0 B6)
	     */
	  default:
	    goto skip_event;
	  }
	}
  
	else if (XqueScanPrefix == KEY_Prefix1) {

	  XqueScanPrefix = (scanCode == KEY_LCtrl) ? KEY_LCtrl : 0;
	  goto skip_event;
	}
  
	else if (XqueScanPrefix == KEY_LCtrl) {

	  XqueScanPrefix = 0;
	  if (scanCode != KEY_NumLock) goto skip_event;
	  scanCode = KEY_Pause;
	}

	if ((scanCode == KEY_BackSpace) && 
	    ((ModifierDown(ControlMask | AltMask)) ||
	     (ModifierDown(ControlMask | AltLangMask))))
	  {
	    GiveUp(0);
	    goto skip_event;
	  }

	/*
	 * Now map the scancodes to real X-keycodes ...
	 */
	keycode = scanCode + MIN_KEYCODE;
	keysym = (keyc->curKeySyms.map +
		  keyc->curKeySyms.mapWidth * 
		  (keycode - keyc->curKeySyms.minKeyCode));
#ifndef XKB
	/*
	 * LockKey special handling:
	 * ignore releases, toggle on & off on presses.
	 */
	if (keysym[0] == XK_Caps_Lock ||
	    keysym[0] == XK_Num_Lock)
	  {
	    if (!down) goto skip_event;
	    if (KeyPressed(keycode)) down = !down;
	    if (keysym[0] == XK_Num_Lock) XqueNumLock  = down;
	  }
#endif
	/*
	 * check for an autorepeat-event
	 */
	if ((down && KeyPressed(keycode)) &&
	    (pKeyboard->kbdfeed->ctrl.autoRepeat != AutoRepeatModeOn ||
	     keyc->modifierMap[keycode]))
	  goto skip_event;

	if (XqueEvents[XqueHead].xq_time <= XqueLastEventTime) {
	  xE.u.keyButtonPointer.time = ++XqueLastEventTime;
	}
	else {
	  xE.u.keyButtonPointer.time =
	    XqueLastEventTime =
	      XqueEvents[XqueHead].xq_time;
	}

	xE.u.u.detail = keycode;
	xE.u.u.type   = down ? KeyPress : KeyRelease;
	mieqEnqueue(&xE);

	svgaInputPending = TRUE;
        break;
        
      default:
        ErrorF("Unknown Xque Event: 0x%02x\n", XqueEvents[XqueHead].xq_type);
      }
      
    skip_event:
      if ((++XqueHead) == XqueQaddr->xq_size) XqueHead = 0;
    }

  /* reenable the signal-processing */
  XqueQaddr->xq_head = XqueQaddr->xq_tail;
  XqueQaddr->xq_sigenable = 1; /* UNLOCK */
}


/*
 * XqueEnable --
 *      Enable the handling of the Xque
 */

static Bool
XqueEnable()
{
  if (XqueSema++ == 0) {
    XqueQaddr->xq_head = XqueQaddr->xq_tail;
    XqueQaddr->xq_sigenable = 1; /* UNLOCK */
  }

  return(Success);
}

/*
 * XqueDisable --
 *      disable the handling of the Xque
 */

static int
XqueDisable()
{
  if (XqueSema-- == 1)
    XqueQaddr->xq_sigenable = 0; /* LOCK */

  return(Success);
}

/*
 * XqueGetMotionEvents --
 *      Return the (number of) motion events in the "motion history
 *      buffer" (snicker) between the given times.
 */

/* ARGSUSED */
static int
XqueGetMotionEvents(
    DeviceIntPtr pdev,
    xTimecoord *coords,
    unsigned long start,
    unsigned long stop,
    ScreenPtr pScreen
)
{
  return 0;
}


/*
 * XquePointerProc --
 *      Handle the initialization, etc. of a mouse
 */

int
XquePointerProc(
    DeviceIntPtr pPointer,
    int       what
)
{
  unchar map[4];

  switch (what) {

  case DEVICE_INIT: 
      
    pPointer->public.on = FALSE;
      
    map[1] = 1;
    map[2] = 2;
    map[3] = 3;
    InitPointerDeviceStruct((DevicePtr)pPointer, 
			    map, 
			    3, 
			    XqueGetMotionEvents, 
			    (PtrCtrlProcPtr)NoopDDA,
			    0);
    break;
      
  case DEVICE_ON:
    XqueLastButtons = 7;
    pPointer->public.on = TRUE;
    return(XqueEnable());
      
  case DEVICE_CLOSE:
  case DEVICE_OFF:
    pPointer->public.on = FALSE;
    return(XqueDisable());
  }
  
  return Success;
}


/*
 * XqueKeyboardBell --
 *      Ring the terminal/keyboard bell for an amount of time proportional to
 *      "loudness".
 */

static void
XqueKeyboardBell(
    int           loudness,         /* Percentage of full volume */
    DeviceIntPtr  pKeyboard,        /* Keyboard to ring */
    pointer	  cntrl,
    int           thing
)
{
  KeybdCtrl *ctrl = (KeybdCtrl *)cntrl;

  if (loudness && ctrl->bell_pitch) {
    (void)ioctl(svgaConsoleFd, KDMKTONE,
		((1193190 / ctrl->bell_pitch) & 0xffff) |
		(((ulong)ctrl->bell_duration * loudness / 50) << 16));

  }
}


/*
 * XqueKeyboardProc --
 *      Handle the initialization, etc. of a keyboard.
 */

int
XqueKeyboardProc(
    DeviceIntPtr pKeyboard,
    int       what
)
{
  KeySymsRec  keySyms;
  CARD8       modMap[MAP_LENGTH];
#ifndef XKB
  unchar      leds;
#endif
  unchar      i;
  xEvent      xE;
  KeyClassRec *keyc = ((DeviceIntPtr)pKeyboard)->key;
  KeySym      *map = keyc->curKeySyms.map;

  switch (what) {
      
  case DEVICE_INIT:
    
    svgaKbdGetMapping(&keySyms, modMap);

    /*
     * Perform final initialization of the system private keyboard
     * structure and fill in various slots in the device record
     * itself which couldn't be filled in before.
     */
    pKeyboard->public.on = FALSE;
    
    InitKeyboardDeviceStruct((DevicePtr)pKeyboard,
                             &keySyms,
                             modMap,
                             XqueKeyboardBell,
                             (KbdCtrlProcPtr)NoopDDA);
    
    break;
    
  case DEVICE_ON:
    pKeyboard->public.on = TRUE;

    XqueNumLock = FALSE;

    xE.u.keyButtonPointer.time = XqueLastEventTime = XqueLastEventTime++;

    /*
     * Hmm... here is the biggest hack of every time !
     * It may be possible that a switch-vt procedure has finished BEFORE
     * you released all keys neccessary to do this. That peculiar behavior
     * can fool the X-server pretty much, cause it assumes that some keys
     * were not released. TWM may stuck alsmost completly....
     * OK, what we are doing here is after returning from the vt-switch
     * exeplicitely unrelease all keyboard keys before the input-devices
     * are reenabled.
     */
    for (i = keyc->curKeySyms.minKeyCode; i < keyc->curKeySyms.maxKeyCode; i++)
      {
	if (KeyPressed(i)) {
	  xE.u.u.detail = i;
	  xE.u.u.type = KeyRelease;
	  (*pKeyboard->public.processInputProc)(&xE, pKeyboard, 1);
	}
      }

#ifndef XKB
    /*
     * Get also the initial led settings
     */
    (void)ioctl(svgaConsoleFd, KDGETLED, &leds);
    for (i = keyc->curKeySyms.minKeyCode;
	 i < keyc->curKeySyms.maxKeyCode;
	 i++, map += keyc->curKeySyms.mapWidth)
      {
	switch(*map) {

	case XK_Caps_Lock:
	case XK_Shift_Lock:
	  if (leds & LED_CAP) 
	    {
	      xE.u.u.detail = i;
	      xE.u.u.type = KeyPress;
	      (*pKeyboard->public.processInputProc)(&xE, pKeyboard, 1);
	    }
	  break;

	case XK_Num_Lock:
	  if (leds & LED_NUM)
	    {
	      xE.u.u.detail = i;
	      xE.u.u.type = KeyPress;
	      (*pKeyboard->public.processInputProc)(&xE, pKeyboard, 1);
	      XqueNumLock = TRUE;
	    }
	  break;
	}
      }
#endif
    return(XqueEnable());
    
  case DEVICE_CLOSE:
  case DEVICE_OFF:
    pKeyboard->public.on = FALSE;
    return(XqueDisable());
  }
  
  return (Success);
}
