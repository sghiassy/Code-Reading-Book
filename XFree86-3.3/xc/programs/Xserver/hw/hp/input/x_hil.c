/* $XConsortium: x_hil.c /main/8 1996/01/14 08:48:33 kaleb $ */
/* $XFree86: xc/programs/Xserver/hw/hp/input/x_hil.c,v 3.0 1996/03/29 22:15:14 dawes Exp $ */

/*******************************************************************
**
**    *********************************************************
**    *
**    *  File:          ddx/hp/hp/x_hil.c
**    *
**    *  Contents:      Input event procedures for the 
**    *                 X/Starbase Merged Server
**    *
**    *  Created:       4/28/88
**    *
**    *  Last Change:   12/05/88
**    *
**    *  Last Release:  IC2
**    *
**    *  Revision:      A.01.00
**    *
**    *  Author:        --gms
**    *
**    *  Copyright:     (c) 1988 Hewlett-Packard Company
**    *
**    *********************************************************
** 
********************************************************************/

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

#define	 NEED_EVENTS
#include "X.h"
#include "XI.h"
#include "Xproto.h"
#include "Xpoll.h"
#include "hildef.h"
#include "XHPproto.h"
#include "x_hil.h"
#include "x_serialdrv.h"
#include "hpkeys.h"
#include "windowstr.h"
#include "inputstr.h"
#include "hppriv.h"
#include <sys/times.h>
#include <sys/hilioctl.h>
#ifdef XKB
#include "XKBsrv.h"
extern Bool noXkbExtension;
#endif
#ifdef XINPUT
#include "extnsionst.h"
#include "extinit.h"			/* LookupDeviceIntRec */
#endif

#define	FIRST_EXTENSION_EVENT	64
#define	MIN_KEYCODE		8	

#define ENQUEUE_EVENT(ev) \
    {if (xE.b.u.u.type != 0)	/* at least 1 motion event */ \
        {ev = allocate_event();/* get current queue pointer*/ \
        *ev = xE;		/* copy from global struct  */ \
        xE = zxE;}}		/* mark it as processed	    */ 

#ifdef XTESTEXT1
/*
 * defined in xtestext1di.c
 */
extern int	on_steal_input;
extern int	exclusive_steal;
#endif /* XTESTEXT1 */

/******************************************************************
 *
 * Externs and variables referenced from other files.
 *
 */

extern char	*mflg;
xEvent	*format_ev();
xHPEvent  xE, zxE;

#ifdef	XINPUT
extern	int		DeviceMotionNotify;
extern	int		DeviceKeyPress;
extern	int		DeviceKeyRelease;
extern	int		DeviceButtonPress;
extern	int		DeviceButtonRelease;
extern	int		DeviceValuator;
extern	int		ProximityIn;
extern	int		ProximityOut;
#endif	/* XINPUT */
extern	int 		axes_changed;
extern	int 		keyboard_click;
extern	int 		screenIsSaved;
extern	int		x_axis, y_axis;
extern	int		max_input_fd;
extern	HPInputDevice	l_devs[MAX_LOGICAL_DEVS];
extern	HPInputDevice	*hpKeyboard;
extern	HPInputDevice	*hpPointer;
extern	HPInputDevice	*hptablet_extension;
extern	WindowPtr 	*WindowTable;
extern	InputInfo 	inputInfo;
extern	DeviceIntPtr	screen_change_dev;
extern	DeviceIntPtr	tablet_extension_device;

extern	unsigned	tablet_xorg;
extern	unsigned	tablet_yorg;
extern	unsigned	tablet_xlimit;
extern	unsigned	tablet_ylimit;
extern	u_int		tablet_width;

static	Bool		in_tablet_extension = FALSE;
static  BoxRec 		LimitTheCursor;


u_char		pointer_amt_bits[3];
u_char		ptr_mods, mv_mods, rs_mods, bw_mods;
Bool		screen_was_changed = FALSE;
Bool		reset_enabled = TRUE;
int 		hpActiveScreen = 0; 		/* active screen ndx (Zaphod) */
int		queue_events_free = MAX_EVENTS;
int		pending_index;
int		pending_bytes;
float		acceleration;
int		threshold;
struct		x11EventQueue *events_queue;	/* pointer to events queue.  */
xHPEvent	*allocate_event();
struct		dev_info hil_info;		/* holds hil_data */

/******************************************************************
 *
 * Variables global to this file.
 *
 */

static	DeviceIntPtr find_deviceintrec();
static	int  process_inputs();
static	void process_serial_data();
static	check_subset_and_scale();
static	move_sprite();
static	send_motion();
static	send_button();
static	move_mouse();
static	u_char	last_direction;
static	u_char	last_key;
static	int	k_down_flag[4];
static	int	k_down_incx[4];
static	int	k_down_incy[4];

/****************************************************************************
 *
 * Process all available data from the input devices and put it on the server's
 * internal events queue.  When this routine is invoked, that queue is empty 
 * since the server empties it each time through its main dispatch loop.
 * 
 * The server's internal queue can hold 256 events.  If the server is busy for
 * a long time, it is possible for the queue to fill up.  In that case we 
 * can return with unread data, or data that is left in a global buffer.
 * This routine must be prepared to handle such leftover data.
 *
 * After handling leftovers, this routine finds a file descriptor with data
 * ready to be read and calls process_inputs to handle it.
 *
 */

CheckInput (blockData, result, pReadmask)
    pointer blockData;
    int result;
    pointer pReadmask;
    {
    fd_set* LastSelectMask = (fd_set*)pReadmask;
    fd_set mask, checkmask;
    extern fd_set EnabledDevices;
    int	    i;
    int     checkfd = max_input_fd;		/* max fd valid for input*/
    int     checkword = checkfd >> 5;		/* max valid word of mask*/

    if (result <= 0)
	return;
    XFD_ANDSET(&mask, LastSelectMask, &EnabledDevices);
    if (!XFD_ANYSET(&mask)) 
	return;

    FD_ZERO(&checkmask);

    FD_SET(checkfd, &checkmask);		/* corresponding mask	*/

    for (i=checkword; i>=0; i--)		/* for all mask words   */
	{
	while (mask.fds_bits[i] && 
	       checkmask.fds_bits[i])		/* while input available*/
	    {
	    if (mask.fds_bits[i] & checkmask.fds_bits[i]) /* current fd valid  */
		{
		mask.fds_bits[i] &= ~checkmask.fds_bits[i];
		process_inputs(checkfd);	/* process its input	*/
		}
	    checkfd--;
	    checkmask.fds_bits[i] = checkmask.fds_bits[i] >> 1;
            }
	if (i>0)
	    {
	    checkfd = (i-1) * 32 + 31;
	    FD_SET(checkfd, &checkmask);	/* corresponding mask	*/
	    }
        }
    }
		
/****************************************************************************
 *
 * Find the device data structure that matches the file descriptor from which
 * data will be read.  Read up to 2000 bytes (HIL driver buffer is only 512)
 * from that file descriptor.  From the data read, get 1 HIL data packet.
 * That packet may contain up to 8 keycodes and 1 motion event.  In the case
 * of a barcode reader in ASCII mode, each keycode may generate up to 6
 * X input events.  The worst case is therefore 49 X events from 1 HIL data
 * packet.
 *
 */

static int process_inputs (file_ds)
    int	file_ds;			/* file_ds to read from      	 */
    {
    int			i;
    u_char		*hil_ptr;
    HPInputDevice 	*indevice = NULL; 
    DeviceIntPtr 	dev;
    xHPEvent		*xHP;
    Bool 		done = FALSE;

    for (i=0; i<MAX_LOGICAL_DEVS; i++)
	{
	if (file_ds == l_devs[i].d.file_ds) 
	    {
	    indevice = &(l_devs[i]);
	    break;
	    }
	}
	
    dev = find_deviceintrec(indevice);

    while (!done)				/* data yet to be processed */
	{
	if (queue_events_free <= MAXHILEVENTS)	/* no room on server queue  */
	    return;
	hil_ptr = (unsigned char *) &hil_info;/* place to copy packet to*/
	done = get_serial_event (hil_ptr);
	process_serial_data (dev, hil_info.hil_dev, &(hil_info));
	}
    if (xE.b.u.u.type != 0)			/* at least 1 motion event */
	{
        xHP = allocate_event();			/* get current queue pointer*/
        *xHP = xE;				/* copy from global struct  */
        xE = zxE;				/* mark it as processed	    */
	}
    }

/***************************************************************************
 *
 * Given the HP device structure, find the DIX device structure that
 * logically corresponds to it.  There is a one-to-one correspondence,
 * expect when the keyboard is also the X pointer, a tablet is subsetted,
 * or an input device is merged with the X pointer or X keyboard.
 *
 * Callers: process_inputs(), read_shmhil().
 *
 */

static
DeviceIntPtr find_deviceintrec (indevice)
    HPInputDevice *indevice;
    {
    PtrFeedbackPtr	p;
    DeviceIntPtr dev = NULL;

    if (indevice != NULL)
	{
	hil_info.hil_dev = indevice;		/* input device struct ptr  */
	if (hptablet_extension &&
	    indevice->d.file_ds==hptablet_extension->d.file_ds && 
	    in_tablet_extension)
	    {
	    hil_info.hil_dev = hptablet_extension;
	    dev = tablet_extension_device;
	    }
	else if (indevice==hpKeyboard || (indevice->x_type==KEYBOARD && 
		(indevice->hpflags & MERGED_DEVICE)))
	    dev = inputInfo.keyboard;
	else if (indevice==hpPointer || (indevice->x_type==MOUSE && 
		(indevice->hpflags & MERGED_DEVICE)))
	    dev = inputInfo.pointer;
#ifdef XINPUT
	else{
	    for (dev = inputInfo.devices;
		dev && ((HPInputDevice *)dev->public.devicePrivate != indevice);
		dev = dev->next)
		;
	    if (!dev)
		FatalError ("X server couldn't find current input device - Aborting.\n");
	}

    	p = dev->ptrfeed;
	if (p != NULL)
	    {
	    threshold = p->ctrl.threshold;
	    acceleration = (float) p->ctrl.num / (float) p->ctrl.den;
	    }
#endif /* XINPUT */
	if (acceleration == 0)
	    acceleration = 1;
	}
    else
	FatalError ("X server couldn't find current input device - Aborting.\n");
    return (dev);
    }

/****************************************************************************
 *
 * process the serial data packet and generate X input events as needed.
 *
 */

static u_int	s_code[2];

static void process_serial_data (dev, phys, info)
    DeviceIntPtr 	dev;
    HPInputDevice	*phys;
    struct		dev_info	*info;
    {
    xEvent		*ev;
    u_int		*hil_code;
    u_char		type, kcode;

    while (pending_index < pending_bytes ) 
	{  
	if (info->poll_hdr & MOTION_DATA)
	    {
	    handle_motion_event (dev, phys, info);
	    if (pending_index >= pending_bytes)
		return;
	    }
    
	hil_code = s_code;

	if (phys->hpflags & DATA_IS_8_BITS)
	    *hil_code = (info->dev_data)[pending_index++];
	else if (phys->hpflags & DATA_IS_16_BITS)
	    {
	    *hil_code = ((info->dev_data)[pending_index+1] << 8) |
		(info->dev_data)[pending_index];
	    pending_index += 2;
	    }
	else if (phys->hpflags & DATA_IS_32_BITS)
	    {
	    *hil_code = ((info->dev_data)[pending_index+3] << 24) |
		(info->dev_data)[pending_index+2] << 16 |
		(info->dev_data)[pending_index+1] << 8 |
		(info->dev_data)[pending_index];
	    pending_index += 4;
	    }
	else
	    {
	    pending_index = pending_bytes;
	    return;
	    }

	if (info->poll_hdr & KEY_DATA)
	    {
	    /* Check to see if this is a "down" keycode for a key that is
	       already down.  If so, and autorepeat has been disabled for
	       this key, ignore the key and return.
	       */

	    kcode = (u_char) (*hil_code >> 1); /* same code up & down */
	    kcode += MIN_KEYCODE;		        /* avoid mouse codes. */

	    if (*hil_code & UP_MASK) 
		if (dev==inputInfo.keyboard || dev==inputInfo.pointer)
		    type = KeyRelease;
		else
		    type = DeviceKeyRelease;
	    else
		{
		if (KeyIsDown(dev,kcode) && !KeyIsRepeating(dev,kcode))
		    return;
		if (dev==inputInfo.keyboard || dev==inputInfo.pointer)
		    type = KeyPress;
		else
		    type = DeviceKeyPress;
		}

	    ev= format_ev (dev, type, kcode, info->timestamp, phys, NULL);
	    parse_keycode (dev, phys, ev);
	    }
	else if (info->poll_hdr & BUTTON_DATA)
	    {

	    if (dev==inputInfo.pointer)
		type = (*hil_code & UP_MASK) ? ButtonRelease : ButtonPress;
	    else
		type = (*hil_code & UP_MASK) ? DeviceButtonRelease 
					     : DeviceButtonPress;

	    ev= format_ev (dev, type,(*hil_code >>1) + 1,info->timestamp,phys,NULL);
#ifdef	XTESTEXT1
	    {
	    extern int	on_steal_input;	/* steal input mode is on.	*/
	    extern int	exclusive_steal;

	    if (on_steal_input)
		XTestStealKeyData(ev->u.u.detail, ev->u.u.type, MOUSE, 
		    phys->coords[0], phys->coords[1]);

	    if (exclusive_steal)
		deallocate_event(ev);
	    }
#endif	/* XTESTEXT1 */
	    }
#ifdef XINPUT
	else if (info->poll_hdr & PROXIMITY_DATA)
	    {
	    /* proximity HIL codes cause a different event type.
	       However, proximity is not reported for devices being
	       used as the X pointer, unless they have no buttons
	       (like a touchscreen), in which case the proximity is
	       treated as button 1.
	       */
	    if (dev!=inputInfo.pointer)
		{
		type = (*hil_code & UP_MASK) ? ProximityOut : ProximityIn;
		ev= format_ev (dev, type, 0, info->timestamp, phys, NULL);
		return;
		}
	    else if (phys->d.num_buttons == 0)
		{
		type = (*hil_code & UP_MASK) ? ButtonRelease : ButtonPress;
	        ev= format_ev (dev, type, 1, info->timestamp, phys, NULL);
		}
	    else
		return;	/* proximity not reported for X pointer */
	    }
#endif /* XINPUT */
	else
	    {
	    pending_index = pending_bytes;
	    return;
	    }
	}
   }

/*******************************************************************
 *
 * handle_motion_event()
 *
 */

handle_motion_event (dev, phys, info)
    DeviceIntPtr dev;
    HPInputDevice *phys;
    struct dev_info *info;
    {
    int			i, type, bytes_coord;
    int			tmp, coords[MAX_AXES];
    char  		*sdata;
    u_char  		*udata;
    HPInputDevice	*log;

    if (dev==inputInfo.pointer)
	{
	type = MotionNotify;
	log = hpPointer;
	}
    else
	{
	type = DeviceMotionNotify;
	log = phys;
	}
    if (phys->hpflags & DATA_IS_32_BITS)
	bytes_coord = 4;
    else if (phys->hpflags & DATA_IS_16_BITS)
	bytes_coord = 2;
    else
	bytes_coord = 1;

    pending_index += phys->d.ax_num * bytes_coord;

    if (phys->hpflags & ABSOLUTE_DATA)		/* absolute device */
	{
	udata = info->dev_data; 
	for (i=0; i < (u_char) phys->d.ax_num; i++, udata+=bytes_coord)
	    if (bytes_coord == 1)
		coords[i] = *udata;
	    else if (bytes_coord == 2)
		coords[i] = *udata | *(udata+1) << 8;
	    else
		coords[i] = *udata | (*(udata+1) << 8) | (*(udata+2) << 16) | 
			    (*(udata+3) << 24);

	if (!check_subset_and_scale (&dev, phys, &log, coords))
	    return;
	}
    else
	{
	sdata = (char *) info->dev_data; 
	for (i=0; i < (u_char) phys->d.ax_num; i++, sdata+=bytes_coord)
	    if (bytes_coord == 1)
		coords[i] = *sdata;
	    else if (bytes_coord == 2)
		coords[i] = *(sdata+1) << 8 | (*sdata & 0x0ff);
	    else 
		coords[i] = (*(sdata+3) << 24) | ((*(sdata+2) << 16) & 0x0ff)|
			    ((*(sdata+1) << 8) & 0xff) | (*sdata & 0x0ff);
	}

    if (phys==hpPointer && axes_changed)
	{
	tmp = coords[0];
	coords[0] = coords[x_axis];
	if (y_axis==0)
	    coords[1] = tmp;
	else
	    coords[1] = coords[y_axis];
	}
    process_motion (dev, phys, log, coords, info->timestamp);
    (void) format_ev (dev, type, 0, info->timestamp, log, &xE);
    }

/*******************************************************************
 *
 * check_subset_and_scale()
 * all we care about is the x and y coordinates.
 *
 */

static
check_subset_and_scale (dev, phys, log, c)
    DeviceIntPtr	*dev;
    HPInputDevice	*phys;
    HPInputDevice	**log;
    int			c[];
    {	
    extern u_char screen_change_amt;
    int i, n_axes = phys->d.ax_num;

    if (tablet_width && hptablet_extension)
	if (c[0]< tablet_xorg || c[0] > tablet_xlimit ||
	    c[1]> tablet_yorg || c[1] < tablet_ylimit)
	    {
	    in_tablet_extension = TRUE;
	    *dev = tablet_extension_device;
	    *log = hptablet_extension;
	    }
	else
	    {
	    in_tablet_extension = FALSE;
	    }
		
    if (*log == hpPointer)
	{
	if (*dev == screen_change_dev)
	    c[0] = (float) (c[0]-tablet_xorg) * phys->scaleX-screen_change_amt;
	else
	    c[0] = (float) (c[0]-tablet_xorg) * phys->scaleX;
        c[1] = (*log)->pScreen->height -
		((float) (c[1]-tablet_ylimit) * phys->scaleY);
	}
    else
	{
	if (*dev == screen_change_dev)
	    c[0] -= screen_change_amt;
        c[1] = phys->d.max_y - c[1]; /* Y-coord  reversed.*/
	}
    if (c[0]==(*log)->coords[0] && c[1]==(*log)->coords[1])
	return (FALSE);
    for (i=0; i<n_axes; i++)
	c[i] -= (*log)->coords[i];
    return (TRUE);
    }

/****************************************************************************
 *
 * parse_keycode (dev, phys, ev, x_type)
 *   Parse keycode information.
 *   Buttons from a three-button mouse also end up here.
 *
 */

#define FIX_LED_CTRL(dev, d, ev, index, led) \
	{if (KeyUpEvent(ev) && dev->key->modifierKeyCount[index] <= 1) { \
	    d.leds &= ~led; \
	    dev->kbdfeed->ctrl.leds &= ~led; } \
	else { \
	    d.leds |= led; \
	    dev->kbdfeed->ctrl.leds |= led; }}

int parse_keycode (dev, phys, ev)
    DeviceIntPtr 	dev;
    HPInputDevice	*phys;
    xEvent		*ev;
    {
#ifdef	XTESTEXT1
    extern u_char	xtest_command_key;	 /* defined in xtestext1dd.c */
#endif	/* XTESTEXT1 */
    u_char	down_mods;
    u_char 	key = ev->u.u.detail;

    if (hpPointer->x_type == KEYBOARD)
	if (hpKeyboard->hpflags & SECOND_LOGICAL_DEVICE &&
	    ((ev->u.keyButtonPointer.pad1==inputInfo.keyboard->id ||
	      ev->u.keyButtonPointer.pad1==inputInfo.pointer->id ) &&
	     move_sprite (dev, hpPointer, ev)))
		return;
	else
	    if (ev->u.keyButtonPointer.pad1==inputInfo.pointer->id &&
		(move_sprite (dev, hpPointer, ev)))
		return;

    /* allow reset only from the X system keyboard,
	   and only if reset is enabled.		*/

    if (key==reset && reset_enabled)
	if ((hpKeyboard->hpflags & SECOND_LOGICAL_DEVICE &&
	    (ev->u.keyButtonPointer.pad1==inputInfo.keyboard->id ||
	     ev->u.keyButtonPointer.pad1==inputInfo.pointer->id)) ||
	    ev->u.keyButtonPointer.pad1==inputInfo.keyboard->id)
	{
	get_down_modifiers (inputInfo.keyboard, &down_mods);
	if ((rs_mods & down_mods) == rs_mods)
	    GiveUp(0);
	}

    /* Special case handling for toggling keys.  These are keys
       that are bound to one of the X modifiers, and have been
       made into toggling keys.  This currently only includes Caps Lock.
       If a key is pressed that is bound to a toggling key,   
       turn on the appropriate LED and treat the key as a toggle.
     */

#ifdef XKB
    if (noXkbExtension) {
#endif

    if (IsToggleKey(dev, phys, key))     	/* toggling key pressed */
	{
	if (KeyIsIgnored(phys,key))
	    {
	    if (KeyDownEvent(ev))
	        UnignoreKey(phys,key);
	    ExpectUpKey(phys,key);		/* for handling autorepeat */
	    deallocate_event (ev);
	    return;
	    }
	else if (KeyDownEvent(ev))
	    {
	    if (UpIsExpected(phys,key))		/* key is autorepeating */
		{
	        deallocate_event (ev);
		return;
		}
	    IgnoreKey(phys,key);
	    }
	else if (KeyUpEvent(ev))
	    DontExpectUpKey(phys,key);

        if (DeviceHasLeds(phys))
	    {
	    int led, index;
	    HPKeyboardFeedbackControl	d;

	    copy_kbd_ctrl_params (&d, &dev->kbdfeed->ctrl);
	    if (index = IsLockKey(dev, key))
	       FIX_LED_CTRL(dev, d, ev, index, CAPSLOCK_LED);

	    (*(phys->s.write)) (phys->d.file_ds, _XChangeFeedbackControl, &d);
	    }
	}
#ifdef XKB
    }
#endif

#ifdef	XTESTEXT1
    if (on_steal_input)
	{ 
	XTestStealKeyData(key, ev->u.u.type, phys->x_type, 
	    ev->u.keyButtonPointer.rootX, ev->u.keyButtonPointer.rootY);
	if (exclusive_steal)
	    { 
	    if (key != xtest_command_key)
		deallocate_event (ev);
	    }
	else if (key == xtest_command_key)
	    deallocate_event (ev);
	}
#endif /* XTESTEXT1 */
    }

/************************************************************************
 *
 * This routine checks to see if the key should be interpreted as a 
 * sprite movement or a button.
 *
 */

static move_sprite (dev, phys, ev)
    DeviceIntPtr	dev;
    HPInputDevice	*phys;
    xEvent		*ev;
    {
    u_char      down_mods;
    u_char      key = ev->u.u.detail;
    u_char      type = ev->u.u.type;
    int		inc;
    Bool	motion_mods;

    get_down_modifiers (dev, &down_mods);
    if (down_mods & (~ptr_mods & ~mv_mods))
	motion_mods = FALSE;
    else if ((down_mods & ptr_mods) == ptr_mods)
	motion_mods = TRUE;
    else
	motion_mods = FALSE;

    if (!(down_mods & mv_mods))
        inc = pointer_move;
    else if ((down_mods & mv_mods) == pointer_amt_bits[0])
        inc = pointer_mod1_amt;
    else if ((down_mods & mv_mods) == pointer_amt_bits[1])
        inc = pointer_mod2_amt;
    else if ((down_mods & mv_mods) == pointer_amt_bits[2])
        inc = pointer_mod3_amt;
    else
	motion_mods = FALSE;

    k_down_incy[DOWN] = inc;
    k_down_incx[RIGHT] = inc;
    k_down_incy[UP] = inc * -1;
    k_down_incx[LEFT] = inc * -1;

    if (key==cursor_down && type==KeyPress && motion_mods)
        return (send_motion (phys, ev, 0, inc, DOWN));
    else if (key==cursor_down && type==KeyRelease)
        {
        k_down_flag[DOWN] = 0;
        return (1);
        }
    else if (key==cursor_left && type==KeyPress && motion_mods)
        return (send_motion (phys, ev, inc * -1, 0, LEFT));
    else if (key==cursor_left && type==KeyRelease)
        {
        k_down_flag[LEFT] = 0;
        return (1);
        }
    else if (key==cursor_right && type==KeyPress && motion_mods)
        return (send_motion (phys, ev, inc, 0, RIGHT));
    else if (key==cursor_right && type==KeyRelease)
        {
        k_down_flag[RIGHT] = 0;
        return (1);
        }
    else if (key==cursor_up && type==KeyPress && motion_mods)
        return (send_motion (phys, ev, 0, inc * -1, UP));
    else if (key==cursor_up && type==KeyRelease)
        {
        k_down_flag[UP] = 0;
        return (1);
        }
    else 
	{
	if (type==KeyPress)
	    type = ButtonPress;
	if (type==KeyRelease)
	    type = ButtonRelease;

	if (key == button_1)
	    return (send_button (ev, type, 1));
	else if (key == button_2)
	    return (send_button (ev, type, 2));
	else if (key == button_3)
	    return (send_button (ev, type, 3));
	else if (key == button_4)
	    return (send_button (ev, type, 4));
	else if (key == button_5)
	    return (send_button (ev, type, 5));
	else if (key == button_6)
	    return (send_button (ev, type, 6));
	else if (key == button_7)
	    return (send_button (ev, type, 7));
	else if (key == button_8)
	    return (send_button (ev, type, 8));
	}
    return (0);
    }

/****************************************************************************
 *
 * Send motion information from the keyboard, when it is the pointer device.
 *
 */

static send_motion (phys, ev, x, y, which)
    HPInputDevice	*phys;
    xEvent		*ev;
    int 		x, y, which;
    {
    int 		coords[MAX_AXES];
    unsigned int	time;
    int	i;

    for (i=0; i<4; i++)
        if (i != which && k_down_flag[i] != 0)
            {
            x += k_down_incx[i];
            y += k_down_incy[i];
            }
    coords[0] = x;
    coords[1] = y;
    k_down_flag[which] = 1;
    time = ev->u.keyButtonPointer.time;
    deallocate_event(ev);
    process_motion (inputInfo.pointer, phys, hpPointer, coords, time);
    ev = format_ev (inputInfo.pointer, MotionNotify, 0, time, hpPointer, &xE);
    return (1);
    }

/****************************************************************************
 *
 * Send button information from the keyboard, when it is the pointer device.
 *
 */

static send_button (ev, direction, bcode)
    xEvent *ev;
    u_char direction, bcode;
    {

    if (bcode == last_key && direction == last_direction)
	deallocate_event(ev);
    else
	{
	ev->u.u.type = direction;
	ev->u.u.detail = bcode;
	last_key = bcode;
	last_direction = direction;
#ifdef XTESTEXT1
	if (on_steal_input)
	    XTestStealKeyData(ev->u.u.detail, ev->u.u.type, MOUSE, 
		ev->u.keyButtonPointer.rootX, ev->u.keyButtonPointer.rootY);
#endif /* XTESTEXT1 */
	}

    return (1);
    }

/****************************************************************************
 *
 * process_motion (hil_info)
 *
 * This function may also be called from x_threebut.c and x_tablet.c.
 * It requires the motion passed to be a relative amount.
 * dev_hp and dev are the logical devices, phys is the actual device.
 *
 */

#define DEF_ACCELERATION	1
#define EDGE_L			1 << 0
#define EDGE_R			1 << 1
#define EDGE_T			1 << 2
#define EDGE_B			1 << 3

/*
 * Use the change_xmax and change_amt from the physical device.
 * This is needed for the case where an absolute device like a tablet
 * has its input merged with a relative device like a mouse.
 *
 */

#define OffRightEdge(log, phys)  (log->coords[0] > (phys->change_xmax + \
			    (int) phys->change_amt) ? EDGE_R : 0)
#define OffLeftEdge(log, phys)   (log->coords[0] < (phys->change_xmin - \
			    (int) phys->change_amt) ? EDGE_L : 0) 
#define OffTopEdge(log, phys)    (log->coords[1] < (phys->change_ymin - \
			    (int) phys->change_amt) ? EDGE_T : 0) 
#define OffBottomEdge(log, phys) (log->coords[1] > (phys->change_ymax + \
			    (int) phys->change_amt) ? EDGE_B : 0) 

process_motion (dev, phys, log, c, timestamp)
    DeviceIntPtr dev;
    HPInputDevice *phys, *log;
    int	c[];
    unsigned int timestamp;
    {
    int		i;
    unsigned int state = 0;
    extern int playback_on;
    ScreenPtr newScreen = log->pScreen;
   
    /* Compute x,y taking care of desired threshold and acceleration
     * No acceleration if we're playing back a recorded test script.
     * No acceleration for absolute pointing devices.
     * No acceleration if we're using the default (1) acceleration.
     */

    if (!playback_on)
	{
	if (!(phys->hpflags & ABSOLUTE_DATA) && 
	    (acceleration != DEF_ACCELERATION))
	    {
	    for (i=0; i < (u_char) log->d.ax_num; i++)
	        if ( (c[i] - threshold) > 0)
		    c[i] = threshold + (c[i] - threshold) * acceleration;
	        else if ( (c[i] + threshold) < 0)
		    c[i] = (c[i] + threshold) * acceleration - threshold;
	    }
	}

    /*
     * If this is the pointer or a device whose input is merged
     * with the pointer, accumulate the motion and maintain a current position.
     * If this is a relative device, save the current movement.
     */

    if (log == hpPointer || (phys->hpflags & ABSOLUTE_DATA))
	for (i=0; i< (int) log->d.ax_num; i++)
	    log->coords[i] = log->coords[i] + c[i];
    else
	for (i=0; i< (u_char) log->d.ax_num; i++)
	    log->coords[i] = c[i];

    /*
     * Change the screen if we have more than one screen,
     * and the screen change device has gone off one of the edges,
     * and the device is not grabbed and confined.
     */

    if ( screenInfo.numScreens > 1 && 
	 dev->id == screen_change_dev->id &&
        (!dev->grab || !dev->grab->confineTo))
	{
	if ((state=OffRightEdge(log,phys)) || (state=OffLeftEdge(log,phys)) || 
	    (state = OffTopEdge(log,phys)) || (state = OffBottomEdge(log,phys)))
	    {
	    if (!screen_was_changed)
		change_the_screen (dev, phys, log, state, &newScreen);
	    }
	else 
	    /*
	     * Needed for the case where a tablet is the X pointer device.
	     * Once we change screens, we want to avoid immediately changing
	     * back.  We change when we enter the screen change area and 
	     * do not change again until after we have left it.
	     */
	    screen_was_changed = FALSE;
	}
    if (phys == hptablet_extension && phys->clients == NULL)
	return;
    /*
     * Clip the cursor to stay within the bound of screen.
     */
    if (log == hpPointer)
	{
	log->coords[0] = 
	    max( LimitTheCursor.x1, min( LimitTheCursor.x2,log->coords[0]));
	log->coords[1] = 
	    max( LimitTheCursor.y1, min( LimitTheCursor.y2,log->coords[1]));
	}
    move_mouse (log, timestamp);
    }

/****************************************************************************
 *
 * change_the_screen()
 * We have more than one screen, and the screen_change_device has been moved 
 * off one of the edges.  Change to another screen.
 *
 */

#define INCREMENT_SCREEN_BY_ONE(p,l) (screenInfo.screens[(p->myNum+1) % \
    screenInfo.numScreens])

#define DECREMENT_SCREEN_BY_ONE(p,l) (p->myNum != 0 ? \
    screenInfo.screens[p->myNum-1] : \
    screenInfo.screens[screenInfo.numScreens - 1])

#define INCREMENT_SCREEN_BY_TWO(p,l) (screenInfo.screens[(p->myNum+2) % \
    screenInfo.numScreens])

#define DECREMENT_SCREEN_BY_TWO(p,l) (p->myNum > 1 ? \
    screenInfo.screens[p->myNum-2] : \
    screenInfo.screens[p->myNum + screenInfo.numScreens - 2])

change_the_screen (dev, phys, log, state, newScreen)
    DeviceIntPtr dev;
    HPInputDevice *phys, *log;				/* logical device */
    unsigned int state;
    ScreenPtr *newScreen;
    {
    ScreenPtr  	oldScreen = log->pScreen;

    if (screen_col_wrap == DEFAULT)
	{
	if (screen_orientation == VERTICAL)
	    screen_col_wrap = WRAP;
	else
	    screen_col_wrap = NOWRAP;
	}

    if (screen_row_wrap == DEFAULT)
	{
	if (screen_orientation == HORIZONTAL)
	    screen_row_wrap = WRAP;
	else
	    screen_row_wrap = NOWRAP;
	}


    switch (state)
	{
	case EDGE_L:
	    if (screen_row_wrap == NOWRAP &&
		(screen_orientation == VERTICAL ||
	        (oldScreen->myNum == 0  ||
	        (oldScreen->myNum == 2 && screen_orientation == MATRIX ))))
		return;

	    if (screen_orientation == VERTICAL)
		{
		if (screen_row_wrap == CHANGE_BY_TWO)
		    {
		    *newScreen = DECREMENT_SCREEN_BY_TWO(oldScreen,log);
		    }
		}
	    else if (screen_orientation == HORIZONTAL)
		{
		*newScreen = DECREMENT_SCREEN_BY_ONE(oldScreen,log);
		}
	    else 					/* MATRIX */
		{
		if (oldScreen->myNum % 2)
		    {
		    *newScreen = DECREMENT_SCREEN_BY_ONE(oldScreen,log);
		    }
	        else if (screen_row_wrap == WRAP)
		    {
		    if (!(screenInfo.numScreens == 3 && oldScreen->myNum == 2))
	    	        {
		        *newScreen = INCREMENT_SCREEN_BY_ONE(oldScreen,log);
		        }
		    }
		else
		    break;
		}
    
	    if (!(phys->hpflags & ABSOLUTE_DATA))
		log->coords[0] += (oldScreen->width - log->change_xmin);
	    break;
	case EDGE_R:
	    if (screen_row_wrap == NOWRAP &&
		(screen_orientation == VERTICAL ||
	        (oldScreen->myNum == (screenInfo.numScreens-1)  ||
	        (oldScreen->myNum == 1 && screen_orientation == MATRIX ))))
		return;

	    if (screen_orientation == VERTICAL)
		{
		if (screen_row_wrap == CHANGE_BY_TWO)
		    {
		    *newScreen = INCREMENT_SCREEN_BY_TWO(oldScreen,log);
		    }
		}
	    else if (screen_orientation == HORIZONTAL)
		{
		*newScreen = INCREMENT_SCREEN_BY_ONE(oldScreen,log);
		}
	    else 					/* MATRIX */
		{
	        if (oldScreen->myNum % 2)
		    {
	            if (screen_row_wrap == WRAP)
		        {
		        *newScreen = DECREMENT_SCREEN_BY_ONE(oldScreen,log);
		        }
		    else
			break;
		    }
		else if (!(screenInfo.numScreens == 3 && oldScreen->myNum == 2))
		    {
		    *newScreen = INCREMENT_SCREEN_BY_ONE(oldScreen,log);
		    }
		else if (screen_row_wrap != WRAP)
		    break;
		}

	    if (!(phys->hpflags & ABSOLUTE_DATA))
		log->coords[0] -= (oldScreen->width);
	    break;
	case EDGE_T:
	    if (screen_col_wrap == NOWRAP &&
		(screen_orientation == HORIZONTAL ||
	        (oldScreen->myNum == (screenInfo.numScreens-1)  ||
	        (oldScreen->myNum == 2 && screen_orientation == MATRIX ))))
		return;

	    if (screen_orientation == HORIZONTAL)
		{
		if (screen_col_wrap == CHANGE_BY_TWO)
		    {
		    *newScreen = INCREMENT_SCREEN_BY_TWO(oldScreen,log);
		    }
		}
	    else if (screen_orientation == VERTICAL)
		{
		*newScreen = INCREMENT_SCREEN_BY_ONE(oldScreen,log);
		}
	    else 						/* MATRIX */
	        {
		if (oldScreen->myNum >= 2) 
		    {
		    if (screen_col_wrap == WRAP)
		        {
		        *newScreen = DECREMENT_SCREEN_BY_TWO(oldScreen,log);
		        }
		    else
			break;
		    }
		else if (!(screenInfo.numScreens == 3 && oldScreen->myNum == 1))
		    {
		    *newScreen = INCREMENT_SCREEN_BY_TWO(oldScreen,log);
		    }
		else if (screen_col_wrap != WRAP)
		    break;
		}
    
	    if (!(phys->hpflags & ABSOLUTE_DATA))
		log->coords[1] += (oldScreen->height);
	    break;
	case EDGE_B:
	    if (screen_col_wrap == NOWRAP &&
		(screen_orientation == HORIZONTAL ||
	        (oldScreen->myNum == 0  ||
	        (oldScreen->myNum == 1 && screen_orientation == MATRIX))))
		return;

	    if (screen_orientation == HORIZONTAL)
		{
		if (screen_col_wrap == CHANGE_BY_TWO)
		    {
		    *newScreen = DECREMENT_SCREEN_BY_TWO(oldScreen,log);
		    }
		}
	    else if (screen_orientation == VERTICAL)
		{
		*newScreen = DECREMENT_SCREEN_BY_ONE(oldScreen,log);
		}
	    else 						/* MATRIX */
	        {
		if (oldScreen->myNum >= 2) 
		    {
		    *newScreen = DECREMENT_SCREEN_BY_TWO(oldScreen,log);
		    }
		else if (screen_col_wrap == WRAP)
		    {
		    if (! (screenInfo.numScreens == 3 && oldScreen->myNum == 1))
		        {
		        *newScreen = INCREMENT_SCREEN_BY_TWO(oldScreen,log);
		        }
		    }
		else
		    break;
		}

	    if (!(phys->hpflags & ABSOLUTE_DATA))
		log->coords[1] -= (oldScreen->height);
	    break;
	}

    hpMoveCursorToNewScreen(*newScreen, log);
    }

/****************************************************************************
 *
 * hpMoveCursorToNewScreen()
 * Turn the cursor off on the old screen.
 * Call the display driver ChangeScreen routine.
 * This routine is also called from the display driver SetCursorPosition
 * routine.
 *
 */

#include <stdio.h>

hpMoveCursorToNewScreen(newScreen, InDev)
    ScreenPtr newScreen;
    HPInputDevice *InDev;
    {
    WindowPtr	pRootWin;
    int tx, ty;
    hpPrivPtr php = (hpPrivPtr) InDev->pScreen->devPrivate;

    php->CursorOff (InDev->pScreen);
    set_scale_and_screen_change (inputInfo.pointer, InDev, newScreen);
    (*((hpPrivPtr)(newScreen->devPrivate))->ChangeScreen) (newScreen);
    InDev->pScreen = newScreen;
    if (InDev == hptablet_extension)
	{
	tx = InDev->coords[0] < tablet_xorg ? 0 : newScreen->width;
	ty = (float) InDev->coords[1] * InDev->scaleY;
	NewCurrentScreen(newScreen, tx, ty);
	}
    else
	NewCurrentScreen(newScreen, InDev->coords[0], InDev->coords[1]);
    hpActiveScreen = newScreen->myNum;
    screen_was_changed = TRUE;

    if (inputInfo.pointer->grab && inputInfo.pointer->grab->cursor)
	newScreen->DisplayCursor(newScreen,inputInfo.pointer->grab->cursor);
    else if (!(pRootWin = WindowTable[newScreen->myNum]))
	newScreen->DisplayCursor(newScreen,(CursorPtr) NULL);
    else
	newScreen->DisplayCursor(newScreen,pRootWin->optional->cursor);
    }

/****************************************************************************
 *
 * move_mouse ()
 * move the sprite, if the device is the pointer.
 * Also move it if some other device is sending MotionNotify events.
 * In any case, send a motion event to dix.
 *
 * This routine may also be called from xtest1dd.c
 *
 */

static
move_mouse (log, event_time)
    HPInputDevice	*log;			/* logical device  */
    int	event_time;				/* event timestamp */
    {
    int			i;
    int			axes = log->d.ax_num;

    if (log == hpPointer)
	(*log->pScreen->SetCursorPosition) (log->pScreen, log->coords[0], log->coords[1], FALSE);

    *log->dpmotionBuf++ = event_time;
    for (i=0; i<axes; i++)
	*log->dpmotionBuf++ = log->coords[i];
    
    if((log->dheadmotionBuf + 100 * (axes+1)) == log->dpmotionBuf)
	log->dpmotionBuf = log->dheadmotionBuf;
    }

/**************************************************************************
 *
 * Called by: hpMouseProc during device initialization, process_motion
 * 	whenever we change screens.
 *
 * This routine sets the scaling factor to be used for absolute pointing
 * devices like graphics tablets.  Input from these devices is scaled to
 * the screen size.  If we have a multi-screen environment, the scaling
 * factor must be changed whenever the screen changes.
 *
 * This routine also sets the margin at the screen edge that will be used
 * to change screens.  For tablets, this is initially 0, allowing the 
 * entire tablet surface to be used by the application.  If a tablet is 
 * the X pointer and a multi-screen environment is being used, the 
 * screen_change_amt variable should be initialized to some value (like 30)
 * to define a area at the tablet edges that will cause the screen to change.
 *
 * Tablet subsetting adds more complications.  The user can define a subset
 * area that used as the X pointer, while the remainder of the tablet surface
 * is treated as a second logical device.  It is this second logical device
 * that controls screen changes.
 *
 */
set_scale_and_screen_change (dev,d,newScreen)
    DeviceIntPtr dev;
    HPInputDevice *d;
    ScreenPtr newScreen;
    {
    int tmp, res_mm;

    /* Absolute device: graphics tablet or touchscreen */

    if (d->hpflags & ABSOLUTE_DATA)
	{
	res_mm = d->d.resolution / 1000;

    	/* Tablet subsetting enabled and this is the pointer region.
    	   This is called only during initialization, since when
    	   we change screens, the device is the second logical device. */

	if (tablet_width && dev->id == inputInfo.pointer->id)
	    {
	    tablet_xorg = tablet_xorigin * res_mm;
	    tablet_xlimit = tablet_xorg + tablet_width * res_mm;
	    tmp  = d->d.max_y - (tablet_yorigin * res_mm);
	    tablet_yorg = tmp > 0 ? tmp : 0;
	    tmp = tablet_yorg - (tablet_height * res_mm);
	    if (tmp > 0)
		tablet_ylimit = tmp;
	    else
		{
		tablet_ylimit = 0;
		tablet_height = tablet_yorg / res_mm;
		if (!tablet_height)
		    tablet_height = 1;
		}
	    d->scaleX = ((float) newScreen->width) /
	        ((float)tablet_width * res_mm );
	    d->scaleY = ((float) newScreen->height) /
		((float)tablet_height * res_mm );
	    d->change_xmin = 0;
	    d->change_xmax = newScreen->width;
	    d->change_amt = 0;
	    }
	else
	
	/* This code is called if we are initializing the second logical
	   device, or if tablet subsetting is not enabled.  It is also
	   called when we are changing screens with a tablet as the X
	   pointer device.

	 */
	    {
	    /* 
	      Set scale for the case where the tablet is the X pointer.
	      The scale is also returned to clients via XHPListInputDevices.
	     */
	    d->scaleX = ((float) (newScreen->width+2*screen_change_amt)) /
		((float)d->d.max_x);
	    d->scaleY = ((float)newScreen->height) /
		((float)d->d.max_y);
	    if (tablet_width)
		{
		/* If this is the second logical device, we must also
		   change the scale of the X pointer.  Since input from
		   absolute extension devices is not scaled, the
		   screen change amounts units are tablet counts.
		 */
		hpPointer->scaleX = ((float) newScreen->width) /
	            ((float)tablet_width * res_mm );
		hpPointer->scaleY = ((float) newScreen->height) /
		    ((float)tablet_height * res_mm );
		d->change_xmin =  res_mm * screen_change_amt;
		d->change_xmax = d->d.max_x - d->change_xmin;
		d->change_ymin =  res_mm * screen_change_amt;
		d->change_ymax = d->d.max_y - d->change_xmin;
		d->change_amt = 0;
		}
	    else
		/* The tablet is the X pointer.  Screen change units
		   are in pixels, since the input will be scaled.
		 */
		{
		d->change_xmin =  1;
		d->change_xmax = newScreen->width - 2;
		d->change_ymin =  1;
		d->change_ymax = newScreen->height - 2;
		d->change_amt = 0;
		}
	    }
	}
    else

    /* This code is called when a relative device is initialized,
       and when we are changing screens with a relative device.
       These devices  (mice, trackballs, dialboxes, spaceballs)
       cause us to change screens by generating values that are
       beyond the edge of the screen.
     */

	{
	d->change_xmin = 0;
	d->change_xmax = newScreen->width;
	d->change_ymin = 0;
	d->change_ymax = newScreen->height;
	d->change_amt = screen_change_amt;
	}
    }

/****************************************************************************
 *
 *  queue_motion_event ()
 *  This is a convenience routine for xosSetCursorPosition.
 *  It is used to artifically generate a motion event when WarpPointer
 *  request is made.
 *
 */

queue_motion_event (dev_p)
    HPInputDevice	*dev_p;
    {
    int			i;
    int			axes = dev_p->d.ax_num;
    extern		TimeStamp currentTime;
    int			ev_time = currentTime.milliseconds;

    *dev_p->dpmotionBuf++ = ev_time;
    for (i=0; i<axes; i++)
	*dev_p->dpmotionBuf++ = dev_p->coords[i];
    
    if((dev_p->dheadmotionBuf + 100 * (axes+1)) == dev_p->dpmotionBuf)
	dev_p->dpmotionBuf = dev_p->dheadmotionBuf;

    (void) format_ev (inputInfo.pointer, MotionNotify, 0, ev_time, dev_p, NULL);
    xE.b.u.u.type = 0;
    }

/****************************************************************************
 *
 * format_ev ( )
 *	format one or more key, button, motion or proximity xEvents.
 *	This routine assumes devices have less than 6 axes, or report only
 *      one axis per event.
 */

#define AXES_PER_EVENT 6

xEvent *
format_ev (dev, type, detail, event_time, log, event)
    DeviceIntPtr	dev;
    u_char		type;
    u_char		detail;
    unsigned  int 	event_time;
    HPInputDevice	*log;
    xHPEvent		*event;
    {
    int first_val = 0, val_data = 0;
    int i, j;
    int n_axes = log->d.ax_num;
    INT32 *ip;
    xEvent *ret = NULL;
    Bool compressing = FALSE;

    if (log->hpflags & NON_CONTIGUOUS_DATA)
	for (j=0; j < (u_char) log->d.ax_num; j++)
	    if (log->coords[j]!=0)
		{
		first_val = j;
		val_data = log->coords[j];
		break;
		}

    for (i=0; (i==0 || i<n_axes); i+=AXES_PER_EVENT)
	{
	if (event==NULL)
	    {
	    ENQUEUE_EVENT(event);
	    event = allocate_event();
	    }
	else if ((event->b.u.keyButtonPointer.pad1 & 0x7f) != dev->id ||
	    ((log->hpflags & NON_CONTIGUOUS_DATA) &&
	    event->x.first_valuator != first_val))
	    {
	    ENQUEUE_EVENT(event);
	    event = &xE;
	    }
	else
	    compressing = TRUE;
	if (!ret)
	    ret = (xEvent *) event;

	event->b.u.u.type = type;
	event->b.u.u.detail = detail;
	event->b.u.keyButtonPointer.time = event_time;
	event->b.u.keyButtonPointer.rootX = hpPointer->coords[0];
	event->b.u.keyButtonPointer.rootY = hpPointer->coords[1];
	event->b.u.keyButtonPointer.pad1 = dev->id;
#ifdef XINPUT
	if (type >= FIRST_EXTENSION_EVENT)
	    {
	    event->b.u.keyButtonPointer.pad1 |= MORE_EVENTS;
	    event->x.type = DeviceValuator;
	    event->x.deviceid = dev->id;

	    if (log->hpflags & NON_CONTIGUOUS_DATA)
		{
		event->x.num_valuators = 1;
		event->x.first_valuator = first_val;
		if (compressing && !(log->hpflags & ABSOLUTE_DATA))
		    event->x.valuator0 += val_data;
		else
		    event->x.valuator0 = val_data;
		return (ret);
		}
	    else
		{
		event->x.num_valuators = 
		    log->d.ax_num < AXES_PER_EVENT ?
		    log->d.ax_num : i==0 ? AXES_PER_EVENT:
		    log->d.ax_num - AXES_PER_EVENT;

		event->x.first_valuator = i;
		ip = &event->x.valuator0;
		for (j=i; j<i+6; j++)
		    if ( j < (u_char) log->d.ax_num)
			if (compressing &&
			   !(log->hpflags & ABSOLUTE_DATA))
			    *ip++ +=  log->coords[j];
			else
			    *ip++ =  log->coords[j];
		    else
			*ip++ =  0;
		event = NULL;
		}
	    }
#endif /* XINPUT */
	}

    return (ret);
    }

/********************************************************************
 *
 * ProcessInputEvents()
 * This routine is invoked from the dispatcher to route events.  
 * It invokes the dix routines to do this.
 *
 */

void ProcessInputEvents()
    {
    int	id, i;
    INT32 *ip;
    int	count;
    xHPEvent	*event;
    DeviceIntPtr	dev;
    Bool checkedscreensave = FALSE;

    while ( events_queue->head != events_queue->tail) 
	{
	if (!checkedscreensave)
	    {
	    if (screenIsSaved==SCREEN_SAVER_ON)
		SaveScreens (SCREEN_SAVER_OFF, ScreenSaverReset);
	    checkedscreensave = TRUE;
	    }
        event =  &((events_queue->events)[(events_queue->head)]);

	switch (event->b.u.u.type) 
	    {
	    case KeyPress:
	        if (keyboard_click)
		    {
    		    KeybdCtrl ctrl;
		    dev = (DeviceIntPtr) LookupKeyboardDevice ();
		    ctrl.click = dev->kbdfeed->ctrl.click;
		    ctrl.bell = 0;
		    hpBell(0, dev, &ctrl, 0);
		    }
	    case KeyRelease:
		dev = (DeviceIntPtr) LookupKeyboardDevice ();
		(*dev->public.processInputProc) ((xEventPtr) event, dev, 1);
	        break;
	    case ButtonPress:
	    case ButtonRelease:
	    case MotionNotify:
		    dev = (DeviceIntPtr) LookupPointerDevice ();
		    (*dev->public.processInputProc) ((xEventPtr) event, dev, 1);
		    break;
		default:
    #ifdef XINPUT
		    id = event->b.u.keyButtonPointer.pad1 & DEVICE_BITS;
		    if (!(event->b.u.keyButtonPointer.pad1 & MORE_EVENTS))
			count=1;
		    else
			count=2;
		    dev = LookupDeviceIntRec (id);
		if (dev == NULL)
		    break;
		if (event->b.u.u.type == DeviceKeyPress)
		    {
		    if (dev->kbdfeed && dev->kbdfeed->ctrl.click)
		        {
    		        KeybdCtrl ctrl;
		        ctrl.click = dev->kbdfeed->ctrl.click;
		        ctrl.bell = 0;
		        hpBell(0, dev, &ctrl, 0);
		        }
		    }
		else if (event->b.u.u.type == DeviceMotionNotify)
		    {
		    ip = &event->x.valuator0;
		    for (i=0; i < (u_char) event->x.num_valuators; i++)
			dev->valuator->axisVal[i] = *(ip+i);
		    }
		(*dev->public.processInputProc) ((xEventPtr) event, dev, count);
#endif /* XINPUT */
	    break;
	    }

	if (events_queue->head == WR_EVENTS)
	    events_queue->head = 0;
	else
	    events_queue->head++;

        }
    queue_events_free	= WR_EVENTS;
    }

Bool
get_serial_event (hil_ptr)
    struct dev_info *hil_ptr;				/* holds hil_data */
    {
    HPInputDevice *d = hil_ptr->hil_dev;
    int status;

    hil_ptr->timestamp = GetTimeInMillis();
    hil_ptr->poll_hdr = 0;
    pending_index=0;
    pending_bytes=0;
    bzero (hil_ptr->dev_data, 36);
    status = (*(d->s.read))
	(d->d.file_ds, hil_ptr->dev_data, &hil_ptr->poll_hdr, &pending_bytes);
    if (status==READ_SUCCESS)
	return(FALSE);
    else
	return(TRUE);
    }

/************************************************************
 * hpConstrainCursor
 *
 * This function simply sets the box to which the cursor 
 * is limited.  
 * 
 * A single BoxRec is used for recording the cursor limits, 
 * instead of one per screen.  This is ok because DIX will
 * call this routine to establish new limits anytime the 
 * cursor leaves one screen for another.
 *
 ************************************************************/

void 
hpConstrainCursor (pScreen,pBox)
ScreenPtr pScreen;    /* Screen to which it should be constrained */
BoxPtr   pBox;        /* Box in which... */
{
    HPInputDevice *d;

    LimitTheCursor = *pBox;
    d = GET_HPINPUTDEVICE (inputInfo.pointer);
    if (d->pScreen !=pScreen)
	hpMoveCursorToNewScreen(pScreen, d);
}
