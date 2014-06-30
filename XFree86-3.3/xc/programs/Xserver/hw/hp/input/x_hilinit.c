/* $XConsortium: x_hilinit.c,v 8.205 95/01/27 19:02:07 gildea Exp $ */
/* $XFree86: xc/programs/Xserver/hw/hp/input/x_hilinit.c,v 3.0 1996/03/29 22:15:17 dawes Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.


Copyright (c) 1988 by Hewlett-Packard Company

Permission to use, copy, modify, and distribute this software 
and its documentation for any purpose and without fee is hereby 
granted, provided that the above copyright notice appear in all 
copies and that both that copyright notice and this permission 
notice appear in supporting documentation, and that the name of 
Hewlett-Packard not be used in advertising or publicity 
pertaining to distribution of the software without specific, written 
prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.

*/
#define	NEED_EVENTS
#define NITEMS(array) (sizeof(array)/sizeof(array[0]))
#define BEEPER_DEVICE   "/dev/rhil"
#define UNINITIALIZED	1
#define EXPLICIT	0x80
#define XEXTENSION	3
#define XPTR_USE 	(1 << XPOINTER)
#define XKBD_USE 	(1 << XKEYBOARD)
#define XOTH_USE 	(1 << XOTHER)
#define XEXT_USE 	(1 << XEXTENSION)
#define DIN_KBD_DRVR	"hp7lc2k.sl"
#define DIN_KBD_INIT	"hp7lc2k_Init"
#define DIN_KBD_PATH	"/dev/ps2kbd"
#define DIN_MOUSE_DRVR	"hp7lc2m.sl"
#define DIN_MOUSE_INIT	"hp7lc2m_Init"
#define DIN_MOUSE_PATH	"/dev/ps2mouse"

#ifndef LIBDIR
#if OSMAJORVERSION >= 10
#define LIBDIR "/etc/X11"
#else
#define LIBDIR "/usr/lib/X11"
#endif
#define DRVRLIBDIR "/usr/lib/X11/extensions"
#endif

#ifndef DRVRLIBDIR
#define DRVRLIBDIR LIBDIR/**/"/extensions"
#endif

#include <stdio.h>
#include <errno.h>
#include <dl.h>
#include <sys/fcntl.h>
#include "ps2io.h"

#include "X.h"
#include "Xproto.h"
#include "hildef.h"
#include "XHPproto.h"				/* extension constants	*/
#include "XHPlib.h"				/* DEFAULT_DIRECTORY  	*/
#include "x_hilinit.h"
#include "x_hil.h"
#include "x_serialdrv.h"
#include "inputstr.h"
#include "../../../os/osdep.h"

#ifdef XINPUT
#include "XI.h"
#include "XIproto.h"
#include "XIstubs.h"
#else
#define Relative 0
#define Absolute 1
#endif /* XINPUT */

#include "extnsionst.h"
#include "extinit.h"			/* LookupDeviceIntRec */

/******************************************************************
 *
 * Externs and global variables that may be referenced by other files.
 *
 */

char	*mflg="TRUE";
HPInputDevice	*hpKeyboard;
HPInputDevice	*hpPointer;
HPInputDevice	*hptablet_extension;

#ifdef XINPUT
extern	int	BadDevice;
extern	int	BadMode;
extern	int	IReqCode;
extern	int	DeviceKeyPress;
extern	int	DeviceKeyRelease;
extern	int	DeviceButtonPress;
extern	int	DeviceButtonRelease;
extern	int	DeviceMotionNotify; 
XID		hp_device_ids[MAX_DEVICES];
XID		x_device_ids[MAX_DEVICES];
#endif  /* XINPUT */

extern  u_char	mv_mods, ptr_mods, rs_mods, bw_mods;
extern  u_char	pointer_amt_bits[];
extern  char   	*display;		/* display number as a string */
extern  int	queue_events_free;
extern  struct	x11EventQueue *events_queue;
extern 	InputInfo 	inputInfo;

int	hpddxScreenPrivIndex;
u_char	identity_map[256];
int 	axes_changed = FALSE;
int 	keyboard_click;
int	allocated_dev_names = FALSE;
int	x_axis, y_axis;

int  otherndx;
int  max_input_fd;
unsigned char	xhp_kbdid;
unsigned tablet_xlimit;
unsigned tablet_ylimit;
unsigned tablet_xorg;
unsigned tablet_yorg;

HPInputDevice	l_devs[MAX_DEVICES];
DeviceIntPtr	tablet_extension_device;
DeviceIntPtr	screen_change_dev;

int     HPType;

/******************************************************************
 *
 * Variables that are global to this file only.
 *
 */

static DevicePtr hpAddInputDevice();
static void RecordOpenRequest();
static void SetInputDevice();
static void mask_from_kcodes();

void		ProcessOtherEvent();

static	xHPEvent	events_array[MAX_EVENTS];	/* input event buffer*/
static	struct		x11EventQueue ev_queue;

recalculate_x_name () {}

void 	hpBell(percent, dev, ctrl, foo)
    int percent, foo;
    DeviceIntPtr dev;
    KeybdCtrl *ctrl;
{
    HPKeyboardFeedbackControl	dc;
    HPInputDevice *d = GET_HPINPUTDEVICE ((DeviceIntPtr) dev);

    dc.class = KbdFeedbackClass;
    dc.bell_pitch = ctrl->bell_pitch;
    dc.bell_duration = ctrl->bell_duration;

    dc.bell_percent = percent;
    if (!percent)
      dc.click = ctrl->click;
    else
      dc.click = 0;
    (*(d->s.write)) (d->d.file_ds, _XBell, &dc);
}

/****************************************************************************
 *
 * Procedures to control Integer and String feedbacks.  
 * These are not used by currently supported devices, but serial input devices
 * could use them.
 */

static hpChangeIntegerControl(pDevice, ctrl)
    DevicePtr pDevice;
    IntegerCtrl *ctrl;
    {
    HPIntegerFeedbackControl	dc;
    HPInputDevice *d = GET_HPINPUTDEVICE ((DeviceIntPtr) pDevice);

    dc.class = IntegerFeedbackClass;
    dc.resolution = ctrl->resolution;
    dc.max_value = ctrl->max_value;
    dc.integer_displayed = ctrl->integer_displayed;
    (*(d->s.write)) (d->d.file_ds, _XChangeFeedbackControl, &dc);
    }

static hpChangeStringControl(pDevice, ctrl)
    DevicePtr pDevice;
    StringCtrl *ctrl;
    {
    HPStringFeedbackControl	dc;
    HPInputDevice *d = GET_HPINPUTDEVICE ((DeviceIntPtr) pDevice);

    dc.class = StringFeedbackClass;
    dc.max_symbols = ctrl->max_symbols;
    dc.num_symbols_supported = ctrl->num_symbols_supported;
    dc.num_symbols_displayed = ctrl->num_symbols_displayed;
    dc.symbols_supported = (int *) ctrl->symbols_supported;
    dc.symbols_displayed = (int *) ctrl->symbols_displayed;
    (*(d->s.write)) (d->d.file_ds, _XChangeFeedbackControl, &dc);
    }

static hpChangeBellControl(pDevice, ctrl)
    DevicePtr pDevice;
    BellCtrl *ctrl;
    {
    HPBellFeedbackControl	dc;
    HPInputDevice *d = GET_HPINPUTDEVICE ((DeviceIntPtr) pDevice);

    dc.class = BellFeedbackClass;
    dc.percent = ctrl->percent;
    dc.pitch = ctrl->pitch;
    dc.duration = ctrl->duration;
    (*(d->s.write)) (d->d.file_ds, _XChangeFeedbackControl, &dc);
    }

/****************************************************************************
 *
 * Change acceleration & threshold.
 * The DIX routine that handles the ChangePointerControl request has
 * already validity checked the values and copied them into the
 * DeviceIntRec.  This routine just copies them into fields that are
 * the same no matter what kind of device we're dealing with.
 *
 */

static hpChangePointerControl(pDevice, ctrl)
    DevicePtr pDevice;
    PtrCtrl *ctrl;
    {
    HPInputDevice *d;
    HPPointerFeedbackControl	dc;
#ifdef XINPUT
    PtrFeedbackPtr b;

    /* Set the default initial acceleration to 1 if this isn't the X pointer */

    if ((DeviceIntPtr) pDevice != inputInfo.pointer && !pDevice->on)
	{
	ctrl->num = 1;
	ctrl->den = 1;
	}

    b = ((DeviceIntPtr) pDevice)->ptrfeed;

    b->ctrl = *ctrl;
#else
    extern int threshold;
    extern float acceleration;

    threshold = ctrl->threshold;
    acceleration = (float) ctrl->num / (float) ctrl->den;
    if (acceleration <= 0)
	acceleration = 1;
#endif /* XINPUT */

    d = GET_HPINPUTDEVICE ((DeviceIntPtr) pDevice);
    dc.class = PtrFeedbackClass;
    dc.num = ctrl->num;
    dc.den = ctrl->den;
    dc.threshold = ctrl->threshold;
    (*(d->s.write)) (d->d.file_ds, _XChangeFeedbackControl, &dc);
    }

/****************************************************************************
 *
 * The members of the ledCtrl structure have the following values:
 *
 * mask:	1 bit per LED.
 * value:	if mask set, turn it on or off.
 *
 */

static hpChangeLedControl(pDevice, ctrl)
    DevicePtr pDevice;
    LedCtrl *ctrl;
    {
    HPInputDevice	*d;
    HPLedFeedbackControl	dc;

    d = GET_HPINPUTDEVICE ((DeviceIntPtr) pDevice);
    dc.class = LedFeedbackClass;
    dc.led_values = ctrl->led_values;
    dc.led_mask = ctrl->led_mask;
    (*(d->s.write)) (d->d.file_ds, _XChangeFeedbackControl, &dc);
    }

/****************************************************************************
 *
 * The members of the keybdCtrl structure have the following values:
 *
 * click:	0(off) - 100 (loud);	-1 => default;
 * bell:	0(off) - 100 (loud); 	-1 => default;
 * bell_pitch:  Pitch of the bell in Hz;-1 => default;
 * bell_duration: in miliseconds;	-1 => default;
 *
 * keyboard_click is checked whenever a key is pressed, in x_hil.c.
 */

static hpChangeKeyboardControl(pDevice, ctrl)
    DevicePtr pDevice;
    KeybdCtrl *ctrl;
    {
    HPInputDevice	*d;
    HPKeyboardFeedbackControl	dc;

    if (inputInfo.keyboard &&
        ((DeviceIntPtr) pDevice)->id==inputInfo.keyboard->id)
        keyboard_click = (int)((double)(ctrl->click) * 15.0 / 100.0);

    d = GET_HPINPUTDEVICE ((DeviceIntPtr) pDevice);
    copy_kbd_ctrl_params (&dc, ctrl);
    (*(d->s.write)) (d->d.file_ds, _XChangeFeedbackControl, &dc);
    }

copy_kbd_ctrl_params (dctrl, ctrl)
    HPKeyboardFeedbackControl   *dctrl;
    KeybdCtrl *ctrl;
    {
    dctrl->class = KbdFeedbackClass;
    dctrl->click = ctrl->click;
    dctrl->bell_percent = ctrl->bell;
    dctrl->bell_pitch = ctrl->bell_pitch;
    dctrl->bell_duration = ctrl->bell_duration;
    dctrl->autoRepeat = ctrl->autoRepeat;
    memmove(dctrl->autoRepeats, ctrl->autoRepeats, 32);
    dctrl->leds = ctrl->leds;
    }

/****************************************************************************
 *
 * hpGetDeviceMotionEvents.
 *
 */
static int hpGetDeviceMotionEvents (dev, coords, start, stop, pScreen)
    DeviceIntPtr  dev;
    CARD32 start, stop;
    xTimecoord *coords;
    ScreenPtr pScreen;
    {
    HPInputDevice 	*pHP = (HPInputDevice *) dev->public.devicePrivate;
    int			i, evcount = 0;
    int			size = pHP->d.ax_num + 1;
    int 		*first, *last, 	*curr;
    int 		*buffp = (int *) coords;
    int 		*pmBuf = pHP->dpmotionBuf;
    int 		*hmBuf = pHP->dheadmotionBuf;

    if (pmBuf == hmBuf)
	{
        if (*pmBuf == 0)			/* no events yet           */
	    {
	    return 0;
	    }
	else
	    last = hmBuf + (99 * size);
	}
    else
	last = pmBuf-size;

    if (*pmBuf == 0)				/* haven't wrapped yet	    */
	first = hmBuf;
    else
	first = pmBuf;

    if (start > *last)				/* start time > last time    */
        return 0;
    else
	{
	curr = first;
	while (*curr < start)
	    {
	    curr += size;
	    if (curr == hmBuf+(100*size))
		curr = hmBuf;
	    if (curr == first)
		return 0;
	    }
	while (*curr <= stop && *curr != 0)
	    {
	    if (dev == inputInfo.pointer)	/*X pointer is 16 bits/axis */
		{
	        *buffp++ = *curr++;		/* copy the time */
	        *buffp++ = *curr << 16 | *(curr+1); /* copy data for 2 axes */
		curr += 2;
		}
	    else				/* other devices are 32 bits */
		for (i=0; i<size; i++)
		    *buffp++ = *curr++;
	    evcount++;
	    if (curr == hmBuf+(100*size))
		curr = hmBuf;
	    if (curr == first)
		break;
	    }
	}
    return (evcount);
    }

/****************************************************************************
 *
 * NOTE: The first parameter passed to this routine is really a DeviceIntPtr.
 *       The declaration used here works because the first element of the    
 *	 structure pointed to by the DeviceIntPtr is a DeviceRec.
 *
 */

#define DIN_MINKEYCODE	16
unsigned char DIN_AUTOREPEATS[] = 
{0xff,0xff,0xff,0xf3,0xfb,0xff,0xff,0xff, 
 0xfb,0xff,0xff,0xff,0xf9,0xff,0xff,0xff, 
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

#define HIL_MINKEYCODE	8
#define HIL_MINKEYCODE2	10
unsigned char HIL_AUTOREPEATS[] = 
{0x00,0x82,0xff,0xff,0xff,0xff,0xff,0xff, 
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, 
 0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};

static Bool hpDeviceProc(pDev, onoff)
    DevicePtr pDev;
    int onoff;
    {
    KeySymsRec		*key_syms, keysym_rec;
    CARD8		*the_modmap;
    DeviceIntPtr	dev = 		(DeviceIntPtr) pDev;
    HPInputDevice 	*pHP = 	(HPInputDevice *) pDev->devicePrivate;
    HPInputDeviceHeader	*pd = 	&pHP->d;
    int			i, j;
    int			mbufsiz =(pHP->d. ax_num * sizeof(int) + sizeof(Time)) *
				   MOTION_BUFFER_SIZE;
    unsigned char 	*ptrf;
    HPStrF		*ptrs;
    char		*x_basename();
#ifdef XINPUT
    char		*strchr();
#endif /* XINPUT */

    switch (onoff)
        {
	case DEVICE_INIT: 
	    pDev->on = FALSE;
	    pHP->pScreen = screenInfo.screens[0];
    	    AssignTypeAndName (pDev, pHP->x_atom, x_basename(pd->x_name));

	    if (pd->num_keys > 0)
		{
		if (!HPKget_kb_info_by_name(pd->keymap_file, 
		    pd->keymap_name, &keysym_rec, &the_modmap))
		    FatalError ("Can't find a keymap in the %s/XHPKeymaps file for device %s.\n", 
			LIBDIR, pd->x_name);
		key_syms = &keysym_rec;
		if (dev->id == inputInfo.keyboard->id)
		    {
		    KeyCode tmp;
    		    extern KeyCode xtest_command_key; /* see xtestext1dd.c */

		    InitKeyboardDeviceStruct(pDev, key_syms, the_modmap, 
			(BellProcPtr) hpBell, 
			(KbdCtrlProcPtr) hpChangeKeyboardControl);
		    get_pointerkeys();
		    fix_modifierkeys();
		    if ((tmp=HPKeySymToKeyCode (dev, XK_F9)))
    		        xtest_command_key = tmp;
		
		    }
#ifdef XINPUT
		else
		    {
		    InitKeyClassDeviceStruct (dev, key_syms, the_modmap);
		    InitKbdFeedbackClassDeviceStruct (dev, 
		    (BellProcPtr) hpBell,
			(KbdCtrlProcPtr) hpChangeKeyboardControl);
		    InitFocusClassDeviceStruct (dev);
		    }
		if (dev->key->curKeySyms.minKeyCode==DIN_MINKEYCODE)
		    memmove(dev->kbdfeed->ctrl.autoRepeats, DIN_AUTOREPEATS, 32);
		else if (dev->key->curKeySyms.minKeyCode==HIL_MINKEYCODE ||
		         dev->key->curKeySyms.minKeyCode==HIL_MINKEYCODE2)
		    memmove(dev->kbdfeed->ctrl.autoRepeats, HIL_AUTOREPEATS, 32);
		}

	    for (j=0, ptrf=pd->feedbacks; j<pd->num_fdbk; j++,ptrf++)
		if (*ptrf == IntegerFeedbackClass)
		    InitIntegerFeedbackClassDeviceStruct ( dev, 
			(IntegerCtrlProcPtr) hpChangeIntegerControl);
		else if (*ptrf == BellFeedbackClass)
		    InitBellFeedbackClassDeviceStruct ( dev, 
		        (BellProcPtr) hpBell, 
			(BellCtrlProcPtr) hpChangeBellControl);
		else if (*ptrf == KbdFeedbackClass)
		    InitKbdFeedbackClassDeviceStruct( dev, 
			(BellProcPtr) hpBell, 
			(KbdCtrlProcPtr) hpChangeKeyboardControl);
		else if (*ptrf == PtrFeedbackClass)
		    InitPtrFeedbackClassDeviceStruct( dev, 
			(PtrCtrlProcPtr) hpChangePointerControl);
	    for (j=0, ptrf=pd->ledf; j<pd->num_ledf; j++,ptrf++)
		InitLedFeedbackClassDeviceStruct ( dev, 
		(LedCtrlProcPtr) hpChangeLedControl);
	    for (j=0, ptrs=pd->strf; j<pd->num_strf; j++, ptrs++)
		InitStringFeedbackClassDeviceStruct ( dev, 
		    (StringCtrlProcPtr) hpChangeStringControl, 
		    ptrs->max_symbols,
		    ptrs->num_symbols_supported, 
		    (KeySym *) ptrs->symbols_supported);
#endif /* XINPUT */

	    if (pd->ax_num)
		{
		if (dev->id == inputInfo.pointer->id)
		    {
		    if (pHP->dev_type == NULL_DEVICE)
			{
	        	pHP->coords[0] = pHP->pScreen->width;
	        	pHP->coords[1] = pHP->pScreen->height;
			}
		    else
			{
	        	pHP->coords[0] = pHP->pScreen->width / 2;
	        	pHP->coords[1] = pHP->pScreen->height / 2;
			}
		    InitPointerDeviceStruct (pDev, ptr_button_map, 
			(pd->num_buttons ? pd->num_buttons : 3),
			hpGetDeviceMotionEvents, 
			(PtrCtrlProcPtr) hpChangePointerControl, 
			    MOTION_BUFFER_SIZE);
		    }
#ifdef XINPUT
		else
		    {
		    InitFocusClassDeviceStruct (dev);
		    if (pHP->iob & HILIOB_PIO)
			InitProximityClassDeviceStruct (dev);
		    InitValuatorClassDeviceStruct (dev, pd->ax_num, 
			hpGetDeviceMotionEvents, 100, 
			(pHP->hpflags & ABSOLUTE_DATA)?1:0);
		    InitPtrFeedbackClassDeviceStruct (dev, 
			(PtrCtrlProcPtr) hpChangePointerControl);
		    }
		InitValuatorAxisStruct (dev, 0, 0, (u_int) pd->max_x, 
		    (u_int) pd->resolution, 0, (u_int) pd->resolution);
		if (pd->ax_num > 1)
		    InitValuatorAxisStruct (dev, 1, 0, (u_int) pd->max_y,
		    (u_int) pd->resolution, 0, (u_int) pd->resolution);
		if (dev->id != inputInfo.pointer->id)
		    for (i=2; i < (u_char) pd->ax_num; i++)
			InitValuatorAxisStruct (dev, i, 0, (u_int) pd->max_x,
			(u_int) pd->resolution, 0, (u_int) pd->resolution);

		pHP->dpmotionBuf = (int *) Xalloc (mbufsiz);
		memset (pHP->dpmotionBuf, 0, mbufsiz);
		pHP->dheadmotionBuf = pHP->dpmotionBuf;
		}

	    if (pd->num_buttons && dev->id != inputInfo.pointer->id)
    		InitButtonClassDeviceStruct(dev, pd->num_buttons, identity_map);
#endif /* XINPUT */
 	    break;
	case DEVICE_ON: 
	    pDev->on = TRUE;
	    if (pHP != NULL) 
		{
        	if (pHP->dev_type != NULL_DEVICE)
		    AddEnabledDevice (pd->file_ds);
		if (pd->ax_num)
		    set_scale_and_screen_change (dev, pHP, pHP->pScreen);
	        }
	    break;
	case DEVICE_OFF:
	    pDev->on = FALSE;
	    if (dev->button) {
		memset(dev->button->down, 0, DOWN_LENGTH);
		dev->button->state = 0;
		dev->button->buttonsDown = 0;
		pHP->button_state = 0;
	    }
	    if (dev->key) {
		memset(dev->key->down, 0, DOWN_LENGTH);
		dev->key->state = 0;
		dev->key->prev_state = 0;
	    }
	    if (dev->id != inputInfo.pointer->id &&
		pd->file_ds == hpPointer->d.file_ds)
		break;
	    if (pHP != NULL && pd->file_ds>= 0)
		{
		RemoveEnabledDevice(pd->file_ds);
    	        close_device (pHP);
		}
	    break;
	case DEVICE_CLOSE: 
	    if ( pHP != NULL && pd->file_ds >= 0)
		{
		RemoveEnabledDevice (pd->file_ds);
    	        close_device (pHP);
		}
#ifdef XINPUT
	    if (pHP->dheadmotionBuf)
		{
		Xfree (pHP->dheadmotionBuf);
		pHP->dheadmotionBuf = NULL;
		pHP->dpmotionBuf = NULL;
		}
#endif /* XINPUT */
	    break;
	}
    return(Success);
    }

/****************************************************************************
 *
 * InitInput --
 *	Initialize pointer and keyboard devices.
 *
 */

void InitInput(argc, argv)
    int     	  argc;
    char    	  **argv;
    {
    int	i, j;
    DeviceIntPtr x_init_device();
    int CheckInput();

    mflg = (char *)getenv("XHPPRINTDEBUGMSG");
    max_input_fd = 0;
    x_axis = 0;
    y_axis = 1;
    axes_changed = FALSE;
    hpPointer = NULL;
    hpKeyboard = NULL;
    hptablet_extension = NULL;
    tablet_width = 0;
    otherndx = 2;
    for (i=0; i<256; i++)
	identity_map[i]=i;

    RegisterBlockAndWakeupHandlers ((BlockHandlerProcPtr) NoopDDA, 
	(WakeupHandlerProcPtr) CheckInput, (pointer) NULL);
    get_pointerkeys();
    for (i=0; i<MAX_LOGICAL_DEVS; i++)
        clear_device_private(&l_devs[i]);
    init_dynamic_devs (l_devs); 		/* load input drivers. */

    if (hpPointer->x_type == KEYBOARD)
	{
	hpPointer->d.num_buttons = 8;
	hpPointer->d.ax_num = 2;
	}

    init_events_queue ( &ev_queue);
    /*
     * Now initialize the devices as far as X is concerned.
     */

    for (i=0, j=0; i<MAX_DEVICES && j < MAX_LOGICAL_DEVS; j++) 
	{
	if (l_devs[j].id == 1 ||		/* inaccessible device*/
	    (l_devs[j].dev_type == NULL_DEVICE && 
	     !(l_devs[j].hpflags & OPEN_THIS_DEVICE)))
		continue;
	if (l_devs[j].d.file_ds != -1)
	    (void) x_init_device (&l_devs[j], TRUE);
	else
	    (void) x_init_device (&l_devs[j], FALSE);
	i++;
	}
    }

HPInputDevice *next_device_private()
  {
  int i;
  HPInputDevice *d = l_devs;

  for (i=0; i<MAX_DEVICES; i++, d++)
    if (d->id == 1)
	return d;

  for (i=0,--d; i<MAX_DEVICES; i++, d--)
    if (d->d.file_ds <= 1 && d->driver_name[0] == '\0')
	return d;

  return NULL;
  }

/***********************************************************
 *
 * Set up the tablet for subsetting.
 *
 */
fix_tablet_subsetting()
    {
    int i;
    HPInputDevice *d;

    if (hpPointer->d.x_name != NULL && !strstr (hpPointer->d.x_name, "TABLET"))
	{
	for (i=MAX_LOGICAL_DEVS-1; i>=0; i--)
	    if (l_devs[i].d.x_name != NULL &&
	        strstr(l_devs[i].d.x_name, "TABLET"))
		break;
	if (i>=0)
    	    {
	    if (hpPointer->hpflags & SECOND_LOGICAL_DEVICE)
		{
		hpPointer->hpflags &= ~SECOND_LOGICAL_DEVICE;
		hpPointer->d.file_ds = -1;
		}
	    else
		close_device(hpPointer);
	    hpPointer = &l_devs[i];
	    open_device(hpPointer);
	    }
	else
	    return;
	}
    if (d = next_device_private())
      {
      *d = *hpPointer; /* will also be an extension device */
      d->hpflags |= SECOND_LOGICAL_DEVICE;
      }
    }

/***********************************************************
 *
 * Perform X initialization for the device.
 *
 */

DeviceIntPtr
x_init_device (dev, start_it)
    HPInputDevice *dev;
    Bool start_it;
    {
    DevicePtr	pXDev;

    pXDev = hpAddInputDevice(hpDeviceProc, start_it, dev);
    if (dev==hpKeyboard)
	{
	RegisterKeyboardDevice(pXDev);
        if (dev->dev_type == KEYBOARD)
	    xhp_kbdid = dev->id - 0xA0;
	}
    if (dev==hpPointer)
	{
	RegisterPointerDevice(pXDev);
	if (dev->x_type == KEYBOARD)
	    InitKbdFeedbackClassDeviceStruct ((DeviceIntPtr) pXDev, 
		(BellProcPtr) hpBell,
		(KbdCtrlProcPtr) hpChangeKeyboardControl);
	screen_change_dev = (DeviceIntPtr) pXDev;
	if (screen_change_amt == SCREEN_CHANGE_DEFAULT)
	    if (dev->hpflags & ABSOLUTE_DATA)
		screen_change_amt = 0;
	    else
		screen_change_amt = 30;
	}
#ifdef XINPUT
    if (dev != hpPointer && dev != hpKeyboard)
    	{
	RegisterOtherDevice(pXDev);
	if (tablet_width && dev->d.file_ds==hpPointer->d.file_ds)
	    {
	    tablet_extension_device = (DeviceIntPtr) pXDev;
	    hptablet_extension = dev;
	    screen_change_dev = tablet_extension_device;
	    }
	}
#endif /* XINPUT */

    return ((DeviceIntPtr) pXDev);
    }

/***********************************************************************
 *
 * Initialize default input devices.
 * If this machine supports HIL, use the last keyboard and mouse.
 * If none are present, use the last key and pointer device.
 *
 */

clear_device_private(d)
    HPInputDevice *d;
    {
	
    memset(d, 0, sizeof(HPInputDevice));
    d->id = UNINITIALIZED;
    d->d.keymap_name = "";
    d->d.file_ds = -1;
    d->driver_name[0] = '\0';
    d->d.path[0] = '\0';
    d->d.file_ds = HP_VENDOR_RELEASE_NUMBER;
    d->d.button_chording = 0;
    if (button_chording == CHORDING_ON)
	{
	d->d.button_chording = 100;
	d->d.reserved[0] = button_latching;
	}
    }

/****************************************************************************
 *
 * open_device opens one of the input devices.
 * The path is filled in by device_files(), or is the default.
 * If the open fails, it may be because the keyboard and pointer
 * are the same device, and the device is already open.
 *
 */

open_device (d)
    HPInputDevice	*d;
    {

    d->led[NLOCK] = LockMask;
    d->d.reserved[0] = 0;
    d->d.reserved[2] = 0;
    d->d.file_ds = HP_VENDOR_RELEASE_NUMBER;
    if (button_chording == CHORDING_ON)
	    {
	    d->d.button_chording = 100;
	    d->d.reserved[0] = button_latching;
	    }
    if ((*(d->s.configure)) (&d->d) != INIT_SUCCESS)
	return(-1);
    init_device_private (d, FALSE);
    return (d->d.file_ds);
    }

/* ******************************************************************** */
/* ************************* Parse X*devices ************************** */
/* ******************************************************************** */

static struct _X_devices
    {
    char *x_name; 
    int dev_type;
    } X_devices[] = {{XI_KEYBOARD, KEYBOARD},
		     {"NULL", NULL_DEVICE}};

static char *h_position[] = 
{
  "FIRST",
  "SECOND",
  "THIRD",
  "FOURTH",
  "FIFTH",
  "SIXTH",
  "SEVENTH"
  };

static void uppercase(str) char *str;	/* convert str to upper case */
  { for (; *str; str++) *str = toupper(*str); }


/* 
 * Formats:
 *   |#comment
 *   |   #comment
 *   |word [#comment]
 *   |  word [#comment]
 *   |word word  [#comment]
 *   |word word word [#comment]
 *   |word word word word [#comment]
 */

static int is_null_needed(null_needed, keyboard_is_pointer)
    Bool *null_needed;
    int *keyboard_is_pointer;
    {
    FILE *fp;
    int i, parms, ipos, itype, use_as;
    char fname[256], buf[256], pos[256], x_name[256], use[256];

    sprintf(fname, "%s/X%sdevices",LIBDIR, display);
    fp = fopen (fname, "r");
    if (fp == NULL)
	return;
    while (fgets(buf,MAXNAMLEN+1,fp) != NULL)
	{
	buf[strlen(buf)-1] = pos[0] = x_name[0] = use[0] = '\0';

	parms = sscanf(buf, "%s%s%s", pos, x_name, use);
	if (parms != 3 || pos[0] == '#' || x_name[0] == '#' || use[0] == '#')
	    continue; 				/* blank or comment, skip it */
	              				/* or not the syntax we want */

    /* Parse lines with 3 parameters, such as:
     *   first keyboard pointer
     *   first null     keyboard
     *   first null     pointer
     */

	uppercase(pos);
	for (i=0; i<NITEMS(h_position); i++)
	    if (0 == strcmp(h_position[i], pos))
		{
		ipos = i + 1;	/* h_position[0] = "FIRST" == 1 */
		break;
		}
	if (i >= 7)                  /* failed, skip this entry */
	    continue;

	uppercase(x_name);
	for (i=0; i<NITEMS(X_devices); i++)
	    if (0 == strcmp(X_devices[i].x_name,x_name))
		{
		itype = X_devices[i].dev_type;
		break;
		}
	if (i == NITEMS(devices)) /* failed, skip this entry */
	    continue;

	uppercase(use);
	if      (0 == strcmp(use, "POINTER"))  use_as = XPTR_USE;
	else if (0 == strcmp(use, "KEYBOARD")) use_as = XKBD_USE;
	else if (0 == strcmp(use, "OTHER"))    use_as = XOTH_USE;
	else
	    continue;

	if (itype == NULL_DEVICE && use_as == XKBD_USE)
	    *null_needed = TRUE;
	if (itype == NULL_DEVICE && use_as == XPTR_USE)
	    *null_needed = TRUE;
	if (itype == KEYBOARD && use_as == XPTR_USE)
	    *keyboard_is_pointer = TRUE;
	}
    fclose(fp);
    }

/********************************************************************
 *
 * check_for_duplicates().
 * 
 * Check to see if a device that is already open has been specified.
 *
 */

static Bool check_for_duplicates(d)
    HPInputDevice *d;
    {
    int i;
    HPInputDevice *tmp;

    for (i=0,tmp=l_devs; i<NITEMS(l_devs); i++,tmp++)
	if (strcmp(tmp->d.path, d->d.path)==0 && tmp->d.file_ds >= 0 &&
	    tmp->d.file_ds != HP_VENDOR_RELEASE_NUMBER)
	    {
	    if (strcmp(d->entry, DIN_KBD_INIT)==0 && (d->use & XPTR_USE))
		{
		*d = *tmp;
		tmp->hpflags |= SECOND_LOGICAL_DEVICE;
	   	hpPointer = d;
		hpPointer->use = XPTR_USE;
		return FALSE;
		}
	    d->d.path[0] = '\0';
	    d->driver_name[0] = '\0';
	    d->entry[0] = '\0';
	    d->use = 0;
	    return FALSE;
	    }
    return TRUE;
    }

/********************************************************************
 *
 *
 *
 *
 */

get_next_device(fd, d)
    FILE	*fd;
    HPInputDevice *d;
    {
    int	len;
    char buf[256], key[256], var[64];
    char *fgets();
    char pos[MAXNAMLEN+1];
    int parms;

    if (d->driver_name[0] != '\0')	
	return TRUE;
    d->entry[0]='\0';
    while (fgets(buf,MAXNAMLEN+1,fd) != NULL)
	{
	buf[strlen(buf)-1] = '\0';
	pos[0] = '\0';

	if ((sscanf(buf,"%s",pos) == EOF) || pos[0] == '#')
	    continue; 				/* blank or comment, skip it */

	uppercase(pos);
	if (strcmp (pos,"BEGIN_DEVICE_DESCRIPTION") != 0)
	    continue;

        while (fgets(buf,256,fd) != NULL) {
	    if (((parms = sscanf(buf,"%s%s",key,var)) == EOF) || key[0] == '#')
		continue; 			/* blank or comment, skip it */

	    uppercase(key);
	    if (parms == 1 && strcmp (key,"END_DEVICE_DESCRIPTION")==0)
		return (check_for_duplicates(d));

	    if (strcmp(key,"PATH") == 0)
		strcpy(d->d.path, var);
	    else if (strcmp(key,"NAME") == 0) {
		sprintf(d->driver_name, "%s/%s", DRVRLIBDIR, var);
		if (*d->entry == '\0') {
		    len = strcspn (var,".");
		    strncpy (d->entry, var, len);
		    d->entry[len] = '\0';
		    strcat (d->entry, "_Init");
		}
	    }
	    else if (strcmp(key,"ENTRYPOINT") == 0)
		strcpy(d->entry, var);
	    else if (strcmp(key,"USE") == 0) {
		uppercase(var);
		if (!strcmp(var,"POINTER"))
		    d->use = EXPLICIT | XPTR_USE;
		else if (!strcmp(var,"KEYBOARD"))
		    d->use = EXPLICIT | XKBD_USE;
		else if (!strcmp(var,"OTHER"))
		    d->use = XOTH_USE;
		else	/* assume used as extension device */
		    d->use = XEXT_USE;
	     }
	 }
    }
    return FALSE;
}

/********************************************************************
 *
 * get_codes()
 * Used to assign codes to keys used to move the pointer.
 * Also to assign numbers to the amount to move the pointer.
 * This routine uses the index into the file to determine the keycode.
 * The down keycode is (index * 2), the up keycode is that plus 1.
 * If the type is NUMBER, the key string is assumed to be an ascii
 * representation of a number.
 * This is used as the increment to move the pointer.
 *
 */

static get_codes (key, code, type, makeupper)
    char	*key;
    int		*code;
    int		type;
    Bool	makeupper;
    {
    int		i;

    if (makeupper)
	for (i=0; i<strlen(key); i++)
	    *(key+i) = toupper(*(key+i));

    if (type == UCHAR_NUMBER || type == USHORT_NUMBER || type == UINT_NUMBER)
	{
	*code = atoi (key);
	return (0);
	}
    else if (type == STRING)
	for (i=0; i<MAX_STRINGS; i++)
	    if (strcmp (key, strings[i].string) == 0)
		{
		*code = strings[i].value;
		return (0);
		}
    for (i=0; *keyset1[i].keystr; i++)
        if (strcmp (key, keyset1[i].keystr) == 0)
	    break;
    if (!inputInfo.keyboard)
	return(0);
    *code = HPKeySymToKeyCode(inputInfo.keyboard, keyset1[i].sym);
    if (*code)
	return(0);
    return (-1);
    }

int
HPKeySymToKeyCode (dev, sym)
    DeviceIntPtr dev;
    KeySym sym;
{
    int i;
    register KeySym *syms;
    KeySymsPtr rec;

    rec = &dev->key->curKeySyms;
    syms = rec->map;
    for (i=0; i<rec->maxKeyCode - rec->minKeyCode + 1; i++)
	{
	if (*syms == sym)
	    return (i + rec->minKeyCode);
	syms += rec->mapWidth;
	}
    return(0);
}

/********************************************************************
 *
 * get_vars()
 * get the address of variables to contain keycodes for pointer functions.
 *
 */

static get_vars (func, codevar, index)
    char	*func;
    u_char	**codevar;
    int		*index;
    {
    int		i;
    
    for (i=0; i<strlen(func); i++)
	*(func+i) = toupper(*(func+i));

    for (i=0; i<MAX_POINTER_FUNCS; i++)
        if (strcmp (func, pointerfunc[i].name) == 0)
	    {
	    *codevar = pointerfunc[i].code;
	    *index = i;
	    return (0);
	    }
    return (-1);
    }

/********************************************************************
 *
 * get_pointerkeys().
 * This routine provides the ability to configure keyboard keys to 
 * move the pointer and act like buttons on the pointer device.
 * The file processed is the X*pointerkeys file, which consists
 * of pairs.  The form is:
 *
 * 	function	key, modifier, or value
 *
 * Look at the pointerfunc table in x_hilinit.h to understand this code.
 * There are 3 types of assignment done:
 * 	1). keys - have both a down and an up code to assign.
 *	2). modifiers - are a bit position in a mask.
 *	3). values - are a single integer number.
 * Possible errors:
 *	1). only 1 of the pair was specified.
 *	2). an invalid function was specified.
 *	3). an invalid key or modifier was specified.
 */

get_pointerkeys()
    {
    char	fname[MAXNAMLEN+1];
    FILE	*fp;
    int 	len;
    int 	cret;
    int 	vret;
    int 	ret2;
    int 	index;
    char 	buf[MAXNAMLEN+1];
    char 	function[MAXNAMLEN+1];
    char 	key[MAXNAMLEN+1];
    char 	*fgets();
    int		code;
    union
	{
	u_char 	*cptr;
	u_short *sptr;
	u_int 	*iptr;
	} codevar;

    button_3 = 0;
    if (inputInfo.keyboard) {
	get_codes ("KEYPAD_2", &code, KEY, FALSE);
	cursor_down = (unsigned char) code;
	get_codes ("KEYPAD_5", &code, KEY, FALSE);
	cursor_up = (unsigned char) code;
	get_codes ("KEYPAD_1", &code, KEY, FALSE);
	cursor_left = (unsigned char) code;
	get_codes ("KEYPAD_3", &code, KEY, FALSE);
	cursor_right = (unsigned char) code;
	get_codes ("KEYPAD_*", &code, KEY, FALSE);
	button_1 = (unsigned char) code;
	get_codes ("KEYPAD_/", &code, KEY, FALSE);
	button_2 = (unsigned char) code;
	get_codes ("KEYPAD_+", &code, KEY, FALSE);
	button_3 = (unsigned char) code;
	get_codes ("KEYPAD_-", &code, KEY, FALSE);
	button_4 = (unsigned char) code;
	get_codes ("KEYPAD_7", &code, KEY, FALSE);
	button_5 = (unsigned char) code;
	}
    sprintf(fname, "%s/X%spointerkeys",LIBDIR, display);
    fp = fopen ( fname, "r");
    if (fp == NULL)
	return;

    while (fgets(buf,MAXNAMLEN+1,fp) != NULL)
	{
	ret2 = sscanf (buf,"%s%s",function,key);

	/* comments begin with a '#'.  Skip them. */

	if (function[0] == '#')		/* comment, skip it 	*/
	    continue;

	if (ret2 == 2)			/* error if < 2 items 	*/
	    {
	    vret = get_vars (function, &codevar, &index);
	    if (vret < 0)		/* invalid function     */
		{
		ErrorF ("Bad function \"%s\" skipped in X*pointerkeys file.\n",
		    function);
		continue;		/* error - skip this one*/
		}
	    cret = get_codes (key, &code, pointerfunc[index].type, TRUE);
	    if (cret < 0 &&		/* not a key or modifier*/
	        pointerfunc[index].type == KEY) /* but must be  */
		{
		ErrorF ("Bad key name \"%s\" skipped in X*pointerkeys file.\n",
		    key);
		continue;		/* error - skip this one*/
		}

	    if (pointerfunc[index].type==MODIFIER) /* modifier - compute bit*/
	        *codevar.cptr = code - 8;
	    else if (pointerfunc[index].type==UINT_NUMBER)
	        *codevar.iptr = code;		/* code for 16-bit number */
	    else if (pointerfunc[index].type==USHORT_NUMBER)
	        *codevar.sptr = code;		/* code for 16-bit number */
	    else
	        *codevar.cptr = code;		/* code for 8-bit key */
	    }
	else
	    {
	    len = strlen(buf) - 1;	/* fgets adds a newline */
	    buf[len] = '\0';
	    if (len > 0)
	        ErrorF ("Bad entry \"%s\" skipped in X*pointerkeys file.\n",
		    buf);
	    }
	}
	
    fclose (fp);
    }

/****************************************************************************
 *
 * hpAddInputDevice(deviceProc, autoStart, pHP)
 * create an X input device, then assign pHP to it's devicePrivate field.
 *
 */

static DevicePtr hpAddInputDevice(deviceProc, autoStart, pHP)
    DeviceProc deviceProc;
    Bool autoStart;
    HPInputDevice *pHP;
    {
    DevicePtr pXDev;
    int id;

    if ((pXDev = AddInputDevice(deviceProc, autoStart)) == NULL)
	FatalError ("Too many input devices - X server terminating!\n");
#ifdef XINPUT
    id = ((DeviceIntPtr) pXDev)->id;
    if (pHP == hpPointer)
	{
	hp_device_ids[id] = XPOINTER;
	x_device_ids[XPOINTER] = id;
	}
    else if (pHP == hpKeyboard)
	{
	hp_device_ids[id] = XKEYBOARD;
	x_device_ids[XKEYBOARD] = id;
	}
    else
	{
	hp_device_ids[id] = otherndx;
	x_device_ids[otherndx++] = id;
	}
#endif /* XINPUT */
    pXDev->devicePrivate = (pointer) pHP;
    return  pXDev;
    }

/****************************************************************************
 *
 * We allow any keycode to be specified as a modifer, Even one that can't
 * be generated by our keyboard.
 *
 */

LegalModifier(key, dev)
    unsigned int key;
    DevicePtr dev;
    {
    return TRUE;
    }

/****************************************************************************
 *
 * close_device closes one of the input devices.
 *
 */

close_device(d)
    HPInputDevice	*d;
    {
    int i, tmp_fd = d->d.file_ds;

    (*(d->s.close)) (d->d.file_ds);
    d->d.file_ds = -1;
    if (tmp_fd == max_input_fd) 
	{
	max_input_fd = 0;
	for (i=0; i<MAX_LOGICAL_DEVS; i++)
	    if (l_devs[i].d.file_ds != HP_VENDOR_RELEASE_NUMBER &&
	        l_devs[i].d.file_ds > max_input_fd)
		max_input_fd = l_devs[i].d.file_ds;
	}
    }

/*****************************
 *
 * init_events_queue (queue)
 * 
 */

init_events_queue(queue)
    struct  x11EventQueue	*queue;		
    {
    queue->events = events_array;	
    queue->head = 0;
    queue->tail = 0;
    queue->size = MAX_EVENTS;
    events_queue = queue;
    }

/*****************************************************************
 *
 * allocate_event ()
 *	allocates the next available event to the caller and increments
 *	the tail pointer of the events queue; sets queue_events_free as needed.
 *
 */

xHPEvent  *allocate_event ()
    {
    xHPEvent		*event;

    event = &( (events_queue->events)[events_queue->tail]);

    if ( events_queue->tail == WR_EVENTS)
	events_queue->tail = 0;
    else  (events_queue->tail)++;

    queue_events_free--;
    if (queue_events_free == 0)
	ErrorF ("Server Internal events queue is full!!!\n");
    return (event);
    }

deallocate_event (ev)
    xHPEvent		*ev;
    {
    xHPEvent		*tmp, *tail, *last, *first;

    tail = &( (events_queue->events)[events_queue->tail]);
    last = &( (events_queue->events)[WR_EVENTS]);
    first = &( (events_queue->events)[0]);

    for (tmp=ev; tmp!=tail; tmp++)
	if (tmp==last)
	    {
	    *tmp = *first;
	    tmp = first-1;
	    }
	else
	    *tmp = *(tmp+1);

    if (events_queue->tail == 0)
	events_queue->tail = WR_EVENTS;
    else
	events_queue->tail--;
    queue_events_free++;
    }


#ifdef XINPUT
void
AddOtherInputDevices ()
    {
    int i;
    HPInputDevice *hp, *tmphp;
    DeviceIntPtr dev;
    Bool found;

    for (i=0, hp=l_devs; i<MAX_LOGICAL_DEVS; hp++,i++) 
	{
	found = FALSE;
        for (dev=inputInfo.devices; dev; dev=dev->next)
	    {
	    tmphp = GET_HPINPUTDEVICE (dev);
	    if (hp == tmphp)
		{
		found = TRUE;
		break;
		}
	    }
        for (dev=inputInfo.off_devices; found==FALSE && dev; dev=dev->next)
	    {
	    tmphp = GET_HPINPUTDEVICE (dev);
	    if (hp == tmphp)
		{
		found = TRUE;
		break;
		}
	    }
	if (found == FALSE && hp->d.x_name != NULL && 
		(strcmp (hp->d.path,"/dev/null") != 0))
	    {
	    dev = x_init_device (hp, TRUE);
	    dev->inited = ((*dev->deviceProc)(dev, DEVICE_INIT) == Success);
	    }
	}
    }

int
ChangeKeyboardDevice (old_dev, new_dev)
    DeviceIntPtr	old_dev;
    DeviceIntPtr	new_dev;
    {
    CARD8		tmp;
    HPInputDevice 	*old = GET_HPINPUTDEVICE (old_dev);
    HPInputDevice 	*new = GET_HPINPUTDEVICE (new_dev);

    if (old->hpflags & OPEN_THIS_DEVICE)
	old->hpflags &= ~OPEN_THIS_DEVICE;
    tmp = hp_device_ids[new_dev->id];
    hp_device_ids[new_dev->id] = XKEYBOARD;
    hp_device_ids[old_dev->id] = tmp;
    x_device_ids[XKEYBOARD] = new_dev->id;
    x_device_ids[tmp] = old_dev->id;
    hpKeyboard = new;
    return (Success);
    }

int
#if NeedFunctionPrototypes
ChangePointerDevice (
    DeviceIntPtr	old_dev,
    DeviceIntPtr	new_dev,
    unsigned char	x,
    unsigned char	y)
#else
ChangePointerDevice (old_dev, new_dev, x, y)
    DeviceIntPtr	old_dev;
    DeviceIntPtr	new_dev;
    unsigned char	x,y;
#endif
    {
    XID			tmp;
    HPInputDevice 	*old = GET_HPINPUTDEVICE (old_dev);
    HPInputDevice 	*new = GET_HPINPUTDEVICE (new_dev);
    
    if (new_dev == tablet_extension_device)
	return (BadDevice);
    x_axis = x;
    y_axis = y;
    if (x_axis != 0 || y_axis != 1)
	axes_changed = TRUE;
    else
	axes_changed = FALSE;

    new->coords[0] = old->coords[0];
    new->coords[1] = old->coords[1];

    if (old->hpflags & OPEN_THIS_DEVICE)
	old->hpflags &= ~OPEN_THIS_DEVICE;

    screen_change_dev = new_dev;
    tmp = hp_device_ids[new_dev->id];
    hp_device_ids[new_dev->id] = XPOINTER;
    hp_device_ids[old_dev->id] = tmp;
    x_device_ids[XPOINTER] = new_dev->id;
    x_device_ids[tmp] = old_dev->id;
    hpPointer = new;
    InitFocusClassDeviceStruct(old_dev);
    return (Success);
    }

/****************************************************************************
 *
 * Turn on a non-standard device.
 *
 */

void
OpenInputDevice (dev, client, status)
    DeviceIntPtr dev;
    ClientPtr client;
    int *status;
    {
    int			mode;
    HPInputDevice 	*d; 
    DeviceClientsPtr	tmp; 

    if (*status != Success)		/* kludge - if not Success, */
	mode = (*status >> 8);		/* called from HPSetInputDevice */
    else				/* mode hidden in 2nd byte	*/
	mode = DEVICE_EVENTS | ON;

    *status = Success;

    d = GET_HPINPUTDEVICE (dev);
    if (d->d.file_ds  == -1)			/* device not yet open   */
        {
        if (open_device (d) < 0)		/* couldn't open device  */
	    {
	    *status = BadDevice;
	    return;
	    }
        }
    else
	{
        for (tmp = (DeviceClientsPtr) d->clients; tmp!=NULL; tmp=tmp->next)
    	    if (tmp->mode != mode)
		{
		*status = BadMode;
		return;
		}
	}
    SetInputDevice (d, mode);

    dev->startup = 1;
    RecordOpenRequest (client, d, dev->id, mode);
    }

/***********************************************************************
 *
 * Record a successful request from a client to open an input device.
 *
 */

static void
RecordOpenRequest (client, d, id, token)
    register ClientPtr client;
    HPInputDevice *d;
    CARD8 id;
    int token;
    {
    DeviceClientsPtr tmp;
    DeviceClientsPtr new_client;

    if (d->clients != NULL)
        {
        for (tmp = (DeviceClientsPtr) d->clients; tmp!=NULL; tmp=tmp->next)
    	if (tmp->client == client)
	    {
    	    tmp->count++;
    	    return;
	    }
    	else if (tmp->next == NULL)
    	    break;

        new_client = (DeviceClients *) Xalloc(sizeof(DeviceClients));
        tmp->next = new_client;
        }
    else
        {
        new_client = (DeviceClients *) Xalloc(sizeof(DeviceClients));
        d->clients = new_client;
        }

    memset ((char *) new_client, 0, sizeof (DeviceClients));
    new_client->resource = FakeClientID(client->index);
    new_client->client = client;
    new_client->next = NULL;
    new_client->count = 1;
    new_client->mode = token;

    AddResource(new_client->resource, HPType, (pointer) id);
    }


/***********************************************************************
 *
 * Turn off a device because a client died.
 * Also called when a client closes a device.
 *
 */

int HPShutDownDevice (deviceid, clientid)
    CARD8 deviceid;
    int	clientid;
    {
    DeviceIntPtr 	dev = NULL; 
    DeviceClientsPtr	tmp;
    DeviceClientsPtr	save;
    HPInputDevice	*d;

    if (deviceid == inputInfo.pointer->id)
	d = hpPointer;
    else if (deviceid == inputInfo.keyboard->id)
	d = hpKeyboard;
    else
	{
	dev = LookupDeviceIntRec(deviceid);
	if (dev == NULL)
	    return;
	d = GET_HPINPUTDEVICE (dev);
	}
    
    if (d->clients != NULL)
        {
        tmp = (DeviceClientsPtr) d->clients;
        if (tmp->resource == clientid)
    	    {
	    d->clients = tmp->next;
	    Xfree (tmp);
	    }
        else
           for (save=tmp,tmp=tmp->next; tmp!=NULL; save=tmp, tmp=tmp->next)
               {
               if (tmp->resource == clientid)
        	   {
        	   save->next = tmp->next;
    		   Xfree (tmp);
    		   }
    	       }
        if (dev && d->clients == NULL)
    	    {
    	    if (!(d->hpflags & MERGED_DEVICE) || 
		(d->hpflags & SECOND_LOGICAL_DEVICE))
    	        DisableDevice(dev);
	    }
	}
    }

/****************************************************************************
 *
 * Turn off an extension device.
 * This code does not allow the keyboard or pointer to be turned off.
 *
 */

void
CloseInputDevice (dev, client)
    DeviceIntPtr	dev;
    ClientPtr		client;
    {
    HPInputDevice 	*d;
    DeviceClientsPtr	tmp;

    d = GET_HPINPUTDEVICE (dev);

    for (tmp= (DeviceClientsPtr) d->clients; tmp!=NULL; tmp=tmp->next)
    	if (tmp->client == client)
	    {
	    tmp->count--;
	    if (tmp->count == 0)
		{
    	        FreeResource(tmp->resource, RT_NONE);
		return;
		}
	    }
    }

/****************************************************************************
 *
 * Change the state of a non-standard device.
 * Modes are:
 *    ON - turn the device on.
 *    OFF - turn the device off.
 *    SYSTEM_EVENTS - report the standard input events.
 *    DEVICE_EVENTS - report the extension input events.
 *
 */

static void
SetInputDevice (d, mode)
    HPInputDevice	*d;
    int			mode;
    {

    if ((mode & DEVICE_EVENTS) == DEVICE_EVENTS)
	d->hpflags &= ~MERGED_DEVICE;
    else
	{
	mode |= ABSOLUTE;
        d->hpflags |= MERGED_DEVICE;
	}

    if ((mode & ABSOLUTE) == ABSOLUTE)
	{
	d->coords[0] = hpPointer->coords[0];
	d->coords[1] = hpPointer->coords[1];
	d->hpflags |= ABSOLUTE_DATA;
	}
    else
	{
	d->coords[0] = 0;
	d->coords[1] = 0;
	if (!(d->d.flags & ABSOLUTE_DATA))
	    d->hpflags &= ~ABSOLUTE_DATA;
	}
    }

/****************************************************************************
 *
 * Change the mode of an extension device.
 * This is for devices such as graphics tablets that can report either
 * relative or absolute motion.
 * We currently do not support this.
 *
 */

int
SetDeviceMode (client, dev, mode)
    register	ClientPtr	client;
    DeviceIntPtr dev;
    int		mode;
    {
    HPInputDevice *d;

    d = GET_HPINPUTDEVICE (dev);
    if (d->dev_type == NULL_DEVICE)
	return Success;
    if ((*(d->s.write)) (d->d.file_ds, _XSetDeviceMode, &mode)==WRITE_SUCCESS)
	return Success;
    return BadMatch;
    }

/****************************************************************************
 *
 * Set the value of valuators on an extension device.
 * This is needed for some devices that can report both
 * relative and absolute motion.  Some may require that the
 * initial values be set when switching modes.
 * We currently do not support this.
 *
 */

int
SetDeviceValuators (client, dev, valuators, first_valuator, num_valuators)
    register	ClientPtr	client;
    DeviceIntPtr dev;
    int		*valuators;
    int		first_valuator;
    int		num_valuators;
    {
    int i;
    HPInputDevice *d;
    HPResolutionControl ctrl;

    d = GET_HPINPUTDEVICE (dev);
    for (i=first_valuator; i<num_valuators; i++)
	if (i>=0 && i < dev->valuator->numAxes)
	    dev->valuator->axisVal[i] = *(valuators+i);
    if (d->dev_type == NULL_DEVICE)
	return Success;
    ctrl.first_valuator = first_valuator;
    ctrl.num_valuators = num_valuators;
    ctrl.resolutions = valuators;
    if ((*(d->s.write)) (d->d.file_ds, _XSetDeviceValuators, &ctrl)==WRITE_SUCCESS)
	return Success;
    return BadMatch;
    }

/****************************************************************************
 *
 * Change the resolution of valuators on an extension device.
 * This is needed for some devices that have multiple resolutions.
 * We currently do not support this.
 *
 */

int
ChangeDeviceControl (client, dev, control)
    register	ClientPtr	client;
    DeviceIntPtr dev;
    xDeviceCtl	*control;
    {
    HPInputDevice *d;
    xDeviceResolutionCtl *dctrl = (xDeviceResolutionCtl *) control;
    HPResolutionControl c;

    d = GET_HPINPUTDEVICE (dev);
    if (d->dev_type == NULL_DEVICE)
	return Success;
    c.first_valuator = dctrl->first_valuator;
    c.num_valuators = dctrl->num_valuators;
    c.resolutions =  (int *) (dctrl+1);
    if ((*(d->s.write)) (d->d.file_ds, _XChangeDeviceControl, &c)==WRITE_SUCCESS)
	return Success;
    return BadMatch;
    }
#endif /* XINPUT */

#define	LEFT_SHIFT_CODE		0x05
#define	RIGHT_SHIFT_CODE	0x04
#define	LEFT_MOD1_CODE		0x03
#define	RIGHT_MOD1_CODE		0x02
#define	RIGHT_CONTROL_CODE	0x00
#define	LEFT_CONTROL_CODE	0x06

#define	LEFT_SHIFT_BIT		0x20
#define	RIGHT_SHIFT_BIT		0x10
#define	LEFT_MOD1_BIT		0x08
#define	RIGHT_MOD1_BIT		0x04
#define	RIGHT_CONTROL_BIT	0x01
#define	LEFT_CONTROL_BIT	0x40
#define	MAX_KEY_MODS		3

fix_modifierkeys()
    {
    u_char tmp[3];

    tmp[1] = 0xff;
    tmp[2] = 0xff;
    tmp[0] = pointer_amt_mods[0];
    mask_from_kcodes (tmp, &pointer_amt_bits[0]);
    tmp[0] = pointer_amt_mods[1];
    mask_from_kcodes (tmp, &pointer_amt_bits[1]);
    tmp[0] = pointer_amt_mods[2];
    mask_from_kcodes (tmp, &pointer_amt_bits[2]);

    mask_from_kcodes (pointer_key_mods, &ptr_mods);
    mask_from_kcodes (pointer_amt_mods, &mv_mods);
    mask_from_kcodes (reset_mods, &rs_mods);
    mask_from_kcodes (borrow_mode_mods, &bw_mods);
    mv_mods &= ~ptr_mods;
    }

static void
mask_from_kcodes (src, dst)
    u_char *src;
    u_char *dst;
    {
    int i;
    HPInputDevice *d;

    d = GET_HPINPUTDEVICE (inputInfo.keyboard);
    for (i=0; i<MAX_KEY_MODS; i++, src++)
	if (*src != 0xff)
	    *dst |= inputInfo.keyboard->key->modifierMap[(*src+8)];
    return;
    }

get_down_modifiers(dev, down_mods)
    DeviceIntPtr dev;
    unsigned char *down_mods;
    {
    u_char *kptr = dev->key->down;
    HPInputDevice *d;

    *down_mods = dev->key->state;	/* get X modifier bits 		    */
    }

/*****************************************************************************
 *
 * Dynamically load drivers to support input devices.
 *
 */

#define DYNAMIC_DEVICE 	0xffff
#define HIL_DRIVER	"hil_driver.sl"
#define HILDRVR_ENTRY	"hil_driver_Init"
FILE	*fp;

init_dynamic_devs(devs)
    HPInputDevice *devs;
{
    int i, keyboard_is_pointer = FALSE;
    HPInputDevice *d;
    Bool (*driverInit)() = NULL, null_needed = FALSE;
    char fname[MAXNAMLEN];

   /*
    * Check the X*devices file for NULL device specifications.  If they are
    * specified, we need to load the HIL driver to support them.
    *
    */

    is_null_needed(&null_needed, &keyboard_is_pointer);

   /*
    * See if we can access a DIN mouse and keyboard.  If so, we need to load
    * a driver to support them.  The DIN devices will be used as the X pointer
    * and keyboard unless some other device is explicitly specified.
    *
    */

    find_din_kbd_use(keyboard_is_pointer);
    find_din_mouse_use(&keyboard_is_pointer);

   /*
    * Check to see if this machine supports HIL devices.
    * If so, load the driver and call it to process the X0devices file
    * for old-style syntax.
    */

    sprintf(fname, "%s/X%sdevices",LIBDIR, display);
    if (((fp = fopen(BEEPER_DEVICE,"r")) != NULL) || null_needed) {
	fclose (fp);
	d = next_device_private();
        sprintf(d->driver_name, "%s/%s",DRVRLIBDIR,HIL_DRIVER);
	sprintf(d->d.path,"X*devices:Recycle:%s",fname);
        strcpy (d->entry, HILDRVR_ENTRY);

    	load_and_init_dev (d, &driverInit, TRUE);

    /*
     * The request to recycle the HIL input device state does not return
     * a valid input device.  It is normally reused, but not if there are
     * no HIL input devices attached.  Clear it to make sure it is treated 
     * as uninitialized.
     */

	clear_device_private(d);

	if (d->d.reserved[3] && (!hpPointer || hpPointer->dev_type != KEYBOARD))
	    keyboard_is_pointer = TRUE;

	for (i=0; i<MAX_DEVICES; i++) {
	    sprintf(d->d.path,"X*devices:");
    	    if (load_and_init_dev (d, &driverInit, FALSE))
		break;
	    if (!(d=next_device_private()))
	        break;
	}
    } 

   /*
    * Now process the X*devices configuration file for new-style entries.
    * These specify dynamically loaded input device drivers.
    * If the system doesn't support HIL devices, get_next_device will
    * return any explicitly specified dynamically loaded input devices.
    */

    fp = fopen ( fname, "r");
    if (fp) {
	if (d=next_device_private()) {
	  while (get_next_device (fp,d)) {
    	    driverInit = NULL;
    	    load_and_init_dev (d, &driverInit, TRUE);
	    if (!(d=next_device_private()))
	        break;
	  }
	}
      fclose(fp);
      }

    if (!hpPointer)
	FatalError ("Couldn't open X pointer device!  Is one attached?\n");
    if (!hpKeyboard)
	FatalError ("Couldn't open X keyboard!  Is one attached?\n");
    if (tablet_width)
	fix_tablet_subsetting();
}

/*****************************************************************************
 *
 * Initialize the input device private structure.
 *
 */

char *x_basename (name)
    char *name;
    {
    int i;
    char *nptr = strchr (name, '_');
    char *ordinal[] = {"FIRST", "SECOND", "THIRD", "FOURTH",
			"FIFTH", "SIXTH", "SEVENTH"};

    if (!nptr)
	return (name);

    for (i=0; i<7; i++)
	if (!strncmp(name, ordinal[i], strlen(ordinal[i])))
	    return(++nptr);
    return (name);
    }

close_default_device (thisDev, otherDev)
    HPInputDevice *thisDev, *otherDev;
    {
    int i, fd;

    if (otherDev && (otherDev->d.file_ds != thisDev->d.file_ds))
        {
        thisDev->s.close(thisDev->d.file_ds);
	for (i=0, fd=thisDev->d.file_ds; i<MAX_DEVICES; i++)
	    if (l_devs[i].d.file_ds == fd)
	        l_devs[i].d.file_ds = -1;
        }
    else
	{
        thisDev->id = UNINITIALIZED;
	thisDev->driver_name[0] = '\0';
	thisDev->d.x_name = NULL;
	}
    thisDev->d.file_ds = -1;
    }

init_device_private (d, close)
    HPInputDevice *d;
    Bool close;
    {
    int j, fd;
    HPInputDeviceHeader *dh = &(d->d);
    char *nptr;

    if (dh->file_ds == HP_VENDOR_RELEASE_NUMBER)
	{
	clear_device_private (d);
	return;
	}
    nptr = x_basename (dh->x_name);
    d->x_atom = MakeAtom (nptr, strlen(nptr),0);
    if (!d->x_atom)
        d->x_atom = MakeAtom(nptr, strlen(nptr),1);
    dh->resolution *= 100;
    if (dh->num_keys)
     	d->x_type = KEYBOARD;
    else if (dh->ax_num)
     	d->x_type = MOUSE;
    else
     	d->x_type = XOTHER;
    if (dh->num_ledf)
         d->iob |= HAS_LEDS;
    if (dh->flags & REPORTS_PROXIMITY)
        d->iob = HILIOB_PIO;
    d->hpflags = dh->flags & ~REPORTS_PROXIMITY;
    if (dh->file_ds >= 0)
        d->hpflags |= OPEN_THIS_DEVICE;
    d->hpflags |= (dh->flags & DATA_SIZE_BITS);
    d->id_detail = SERIAL;
    d->dev_type = DYNAMIC_DEVICE;
    d->id = 0;
    if (d->d.reserved[0] & HP_HIL) {
	d->id_detail = d->d.reserved[0];
	d->id = d->d.min_kcode;
	d->iob = d->d.max_kcode;
	d->dev_type = d->d.reserved[1];
	d->use = d->d.reserved[2];
    }
    if (hpPointer && ((d->use & XPTR_USE) && !(d->use & EXPLICIT)))
	d->use &= ~XPTR_USE;
    if (hpKeyboard && ((d->use & XKBD_USE) && !(d->use & EXPLICIT)))
	d->use &= ~XKBD_USE;
    if (!d->use)
	d->use = XEXT_USE;

    if (d->use & XPTR_USE)
	{
	if (hpPointer)
	    close_default_device (hpPointer, hpKeyboard);
	hpPointer = d;
	}
    if (d->use & XKBD_USE)
	{
	HPInputDevice *d2;
        if (d->use & XPTR_USE && (d2=next_device_private()))
	    {
	    *d2 = *d;
	    d->use = XKBD_USE;
	    d->hpflags |= SECOND_LOGICAL_DEVICE;
	    hpPointer = d2;
	    hpPointer->use = XPTR_USE;
	    }
	if (hpKeyboard)
	    close_default_device (hpKeyboard, hpPointer);
	hpKeyboard = d;
	if (dh->reset)
	    {
	    reset = dh->reset;
	    rs_mods = dh->reset_mods;
	    }
	}
    if ((d->use & XEXT_USE) && close)
	{
	d->s.close(dh->file_ds);
	dh->file_ds = -1;
	}
    if (d->use & XOTH_USE)
        d->hpflags |= MERGED_DEVICE;
    if (dh->file_ds > max_input_fd)
	max_input_fd = dh->file_ds;
}

/******************************************************************************
 *
 * shl_driver_load() is passed the path of a shared library to load, and the
 * name of an entry point to find.  It loads the library and returns the 
 * entry point address.
 */

Bool (*shl_driver_load( driver_path, driver_init ))()
char *driver_path, *driver_init;
{
   /*
    * pfrb = pointer to a function returning a Bool
    */
   typedef Bool (*pfrb)();

   shl_t            ldr_module_id;
   long             ldr_module_entry=0;
   int              ret_val;
   char             *dummy_handle = NULL;
#define ALL_SHLIBS_HANDLE &dummy_handle

   /**********************************************************************
    *
    * If the driver entrypoint is already visible within the current program,
    * skip the load and return the address of the routine we already have
    * loaded.
    *
    */

   ret_val = shl_findsym((shl_t *) PROG_HANDLE, driver_init, 
       TYPE_PROCEDURE, &ldr_module_entry );
   if ( ! ret_val ) {
      return( (pfrb) ldr_module_entry );
   }

   /*
    * If the driver entrypoint is already visible within a shared library we
    * are already accessing, skip the load and return the address.
    */
   ret_val = shl_findsym((shl_t *) ALL_SHLIBS_HANDLE, driver_init,
	TYPE_PROCEDURE, &ldr_module_entry );
   if ( ! ret_val ) {
      return( (pfrb) ldr_module_entry );
   }


   /**********************************************************************
    *
    * Load driver into the current VA space.
    */
   ldr_module_id = shl_load( driver_path, (BIND_DEFERRED | BIND_VERBOSE), 0L );

   if ( ldr_module_id == NULL )
	FatalError ("X server failed to load shared library %s, errno is %d\n", 
	driver_path,errno);


   /**********************************************************************
    *
    * Use the module ID to find the address of the requested entry point.
    */

   ret_val = shl_findsym( &ldr_module_id, driver_init, TYPE_PROCEDURE,
                          &ldr_module_entry );
   if (ret_val && driver_init != NULL)
	FatalError ("X server couldn't find entry point '%s' in library %s\n",
	    driver_init, driver_path);

   return( (pfrb) ldr_module_entry );
}

/***********************************************************************
 *
 * din_mouse_present
 *
 */

static Bool
din_mouse_present()
    {
    int fd;
    struct ps2_4 statbuf, idbuf;

    fd = open("/dev/ps2mouse", O_RDWR);
    if (fd < 0)
	return FALSE;
    ioctl (fd, PS2_PORTSTAT, &statbuf);
    ioctl (fd, PS2_IDENT, &idbuf);
    close (fd);
    if (statbuf.b[0] != PS2_MOUSE || idbuf.b[0] != 0)
	return FALSE;
    return TRUE;
    }

/***********************************************************************
 *
 * find_din_mouse_use
 *
 */

find_din_mouse_use(keyboard_is_pointer)
    int *keyboard_is_pointer;
    {
    HPInputDevice *d;
    Bool (*driverInit)() = NULL;

    if (!din_mouse_present())          		/* no DIN mouse attached */
	{
	if (!hpPointer || 			/* no HIL pointer device */
	     (hpPointer->dev_type != MOUSE &&	/* or it's not a motion device*/
	     !(hpPointer->use & EXPLICIT)))	/* and not explicitly named   */
	    *keyboard_is_pointer = TRUE;
	return;
	}

    if (!(d=next_device_private()))		    /* too many devices */
	return;

    if ((hpPointer && hpPointer->use & EXPLICIT) || *keyboard_is_pointer)
	d->use = XEXT_USE; 			/* explicit ptr specified  */
    else
	d->use = XPTR_USE;

    sprintf(d->driver_name, "%s/%s", DRVRLIBDIR, DIN_MOUSE_DRVR);
    strcpy (d->entry, DIN_MOUSE_INIT);
    strcpy (d->d.path, DIN_MOUSE_PATH);

    load_and_init_dev (d, &driverInit, TRUE);
    d->id_detail = PS2;
    }

/***********************************************************************
 *
 * din_kbd_present
 *
 */

static Bool
din_kbd_present()
    {
    int fd;
    struct ps2_4 statbuf, idbuf;

    fd = open("/dev/ps2kbd", O_RDWR);
    if (fd < 0)
	return FALSE;
    ioctl (fd, PS2_PORTSTAT, &statbuf);
    ioctl (fd, PS2_IDENT, &idbuf);
	close (fd);
    if (statbuf.b[0]!=PS2_KEYBD && 
	idbuf.b[0]!=0xab && idbuf.b[1]!=0x83) /* no DIN kbd*/
	return FALSE;
    return TRUE;
    }

/***********************************************************************
 *
 * find_din_kbd_use
 *
 */

find_din_kbd_use(keyboard_is_pointer)
    int keyboard_is_pointer;
    {
    Bool (*driverInit)() = NULL;
    HPInputDevice *d;

    if (!din_kbd_present())				/* no DIN kbd  */
	return;

    if (!(d=next_device_private()))		    /* too many devices */
	return;

    if (hpKeyboard && hpKeyboard->use & EXPLICIT)  /* kbd explicitly spec'd */
	d->use = XEXT_USE;
    else
	d->use = XKBD_USE;

    if (keyboard_is_pointer)
	d->use |= XPTR_USE;

    sprintf(d->driver_name, "%s/%s", DRVRLIBDIR, DIN_KBD_DRVR);
    strcpy (d->entry, DIN_KBD_INIT);
    strcpy (d->d.path, DIN_KBD_PATH);

    load_and_init_dev (d, &driverInit, TRUE);
    d->id_detail = PS2 | PC101_KBD;
    }

/***********************************************************************
 *
 * load_and_init_dev
 *
 */

load_and_init_dev (d, driverInit, fatal)
    HPInputDevice *d;
    Bool (**driverInit)();
    Bool fatal;
    {
    int ret_val;
    void *(*foo)();

    if (! *driverInit)
         *driverInit = shl_driver_load(d->driver_name, d->entry, &foo, "x");
    ret_val = (**driverInit)(&(d->s)); 		/* Initialize driver. */
    if (ret_val!=INIT_SUCCESS)
        FatalError ("Couldn't initialize input device driver %s\n", 
	    d->driver_name);
    if ((*(d->s.configure)) (&(d->d))!=INIT_SUCCESS)
        if (fatal)
            FatalError ("Couldn't configure input device driver %s\n", 
	        d->driver_name);
	else
	    {
	    clear_device_private (d);
	    return 1;
	    }
    init_device_private (d, TRUE);
    return 0;
    }
