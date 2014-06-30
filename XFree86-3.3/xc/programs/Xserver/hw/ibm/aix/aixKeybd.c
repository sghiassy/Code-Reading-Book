/*
 * $XConsortium: aixKeybd.c /main/7 1996/01/14 16:45:10 kaleb $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

#include <stdio.h>
#define NEED_EVENTS
#include "X.h"
#include "aixInput.h"
#include "Xmd.h"
#include "input.h"
#include "miscstruct.h"
#include "scrnintstr.h"
#include "keysym.h"
#include "cursorstr.h"
#include "inputstr.h"

#include "aixKeymap.h"

#include "ibmIO.h"
#include "ibmKeybd.h"
#include "ibmScreen.h"
#include "ibmTrace.h"


int kbdType;

#ifdef XKB
#include "XKBsrv.h"
extern Bool noXkbExtension;
#endif

#ifndef _IBM_LFT
/*---------------------------------- HFT case ------------------------------*/
#include <sys/hft.h>
#include "hftUtils.h"

/***============================================================***/

static void
rtChangeKeyboardControl(pDevice,ctrl)
    DevicePtr pDevice;
    KeybdCtrl *ctrl;
{
    int volume;

    TRACE(("rtChangeKeyboardControl(pDev=0x%x,ctrl=0x%x)\n",pDevice,ctrl));

    ibmKeyClick =   ctrl->click;
    hftSetKeyClick(ctrl->click);

    /* X specifies duration in milliseconds, RT in 1/128th's of a second */
    ibmBellPitch=	ctrl->bell_pitch;
    ibmBellDuration= 	((double)ctrl->bell_duration)*(128.0/1000.0);

    hftSetLEDS(0x7,(ibmLEDState=ctrl->leds));

    if (ibmKeyRepeat= ctrl->autoRepeat)
	hftSetTypematicDelay( 400 );
    else
	hftSetTypematicDelay( 600 );

    return;
}

/***============================================================***/

static void
rtBell(loud, pDevice, ctrl, what)
    int loud;
    DeviceIntPtr pDevice;
    pointer ctrl;
    int what;
{

    TRACE(("rtBell(loud= %d, pDev= 0x%x)\n",loud,pDevice));
    hftSound(loud,ibmBellDuration,ibmBellPitch);
    return;
}


/***============================================================***/

extern int ibmUsePCKeys;

rtGetKbdMappings(pKeySyms, pModMap)
KeySymsPtr pKeySyms;
CARD8 *pModMap;
{
    register int i;
    TRACE(("rtGetKbdMappings( pKeySyms= 0x%x, pModMap= 0x%x )\n",
							pKeySyms,pModMap));
    for (i = 0; i < MAP_LENGTH; i++)
       pModMap[i] = NoSymbol;

    if (ibmUsePCKeys) {
	pModMap[ Aix_Control_L ] = LockMask;
	pModMap[ Aix_Caps_Lock ] = ControlMask;
    }
    else {
	pModMap[ Aix_Caps_Lock ] = LockMask;
	pModMap[ Aix_Control_L ] = ControlMask;
    }

    pModMap[ Aix_Shift_L ] = ShiftMask;
    pModMap[ Aix_Shift_R ] = ShiftMask;
    pModMap[ Aix_Num_Lock ] = NumLockMask;

    switch ( kbdType = hftGetKeyboardID() )
    {
    case HF101KBD :
       pModMap[ Aix_Alt_L ] = Mod1Mask;
       pModMap[ Aix_Alt_R ] = Mod1Mask;
	break;
    case HF102KBD :
       pModMap[ Aix_Alt_L ] = Mod1Mask;
       pModMap[ Aix_Alt_R ] = Mod2Mask;
	break;
    case HF106KBD :
       pModMap[ Aix_Alt_R ] = Mod1Mask;
    case HFT_ILLEGAL_KEYBOARD:
    default :
       pModMap[ Aix_Alt_L ] = Mod1Mask;
       pModMap[ Aix_Alt_R ] = Mod1Mask;
	break;
    }

    pKeySyms->minKeyCode=       AIX_MIN_KEY;
    pKeySyms->maxKeyCode=       AIX_MAX_KEY;
    pKeySyms->mapWidth=         AIX_GLYPHS_PER_KEY;
    pKeySyms->map=              aixmap;
}

/***============================================================***/

int
AIXKeybdProc(pDev, onoff)
    DevicePtr 	 pDev;
    int 	 onoff;
{
    KeySymsRec		keySyms;
    CARD8 		modMap[MAP_LENGTH];

    TRACE(("AIXKeybdProc( pDev= 0x%x, onoff= 0x%x )\n",pDev,onoff));

    switch (onoff)
    {
	case DEVICE_INIT: 
	    ibmKeybd = pDev;
	    rtGetKbdMappings( &keySyms, modMap );
#ifdef XKB
	    if (!noXkbExtension) {
		XkbComponentNamesRec	names;
		names.keymap= NULL;
		names.keycodes= "ibm(aix101)";
		names.types= "complete";
		names.compat= "complete";
		names.symbols= "us(pc101)";
		names.geometry= "sgi101";
		XkbInitKeyboardDeviceStruct(ibmKeybd,&names,
			&keySyms, modMap,
			rtBell, rtChangeKeyboardControl);
	    }
	    else
#endif
	    InitKeyboardDeviceStruct(
			ibmKeybd, &keySyms, modMap, rtBell,
			rtChangeKeyboardControl);
	    break;
	case DEVICE_ON: 
	    pDev->on = TRUE;
	    break;
	case DEVICE_OFF: 
	    pDev->on = FALSE;
	    break;
	case DEVICE_CLOSE:
	    break;
    }
    return Success;
}
/*---------------------------------- HFT End ------------------------------*/
#else
/*---------------------------------- LFT Case ------------------------------*/
#include <fcntl.h>
#include "lftUtils.h"

extern Bool   permitOldBugs;

/***============================================================***/

/*
 * Turn caps lock LED indicator on or off
 */

SetCapsLockLED (pDev, onoff)
DevicePtr pDev;
int       onoff;
{
    InputDevPrivate  *pPriv = &lftInputInfo[KEYBOARD_ID];
    int arg;

    if (onoff == 1)
	ibmLEDState |= KSCAPLOCK;
    else
	ibmLEDState = (ibmLEDState & ~KSCAPLOCK);

    arg = ibmLEDState;
    ioctl(pPriv->fd, KSLED, &arg);
}


/*
 * Turn num lock LED indicator on or off.
 */

SetNumLockLED (pDev, onoff)
DevicePtr pDev;
int       onoff;
{
    InputDevPrivate  *pPriv = &lftInputInfo[KEYBOARD_ID];
    int arg;

    if (onoff == 1)
	ibmLEDState |= KSNUMLOCK;
    else
	ibmLEDState = (ibmLEDState & ~KSNUMLOCK);

    arg = ibmLEDState;
    ioctl(pPriv->fd, KSLED, &arg);
}



/***============================================================***/
/**
 **  FUNCTION:
 **       Change following keyboard controls: key click, autoRepeat,
 **       bell_pitch, bell_duration, leds.
 **/
/***============================================================***/


static void
aixChangeKeybdControl(pDev, ctrl)
    DeviceIntPtr pDev;
    KeybdCtrl *ctrl;
{
    InputDevPrivate  *pPriv = &lftInputInfo[KEYBOARD_ID];
    int args;

    TRACE(("Enter aixChangeKeybdControl(pDev=0x%x,ctrl=0x%x)\n",pDev, ctrl));


    /*  X duration in milliseconds, AIX in 1/128th's of a second.
     */
    ibmBellDuration = ((double)ctrl->bell_duration)*(128.0/1000.0);
    ibmBellPitch=	ctrl->bell_pitch;
    ibmKeyRepeat = ctrl->autoRepeat;

    /*  Change keyboard LEDS. Overwrite the current keyboard LEDS state.
     *  (Is server LEDS masks!=lft LEDS masks)
     */
    args = ibmLEDState = ctrl->leds;
    if(args) args = 4/args;

    /* if device is not open at this moment, do nothing */
    if (pPriv->fd != -1)
	ioctl(pPriv->fd, KSLED, &args);


    /* change key clicker.
     * Convert X click (0-100) to dd click value.
     */

    if (ctrl->click == 0)
	ibmKeyClick = KSCLICKOFF;		/* disable click */
    else if (ctrl->click <= 33)
	ibmKeyClick = KSCLICKLOW;		/* low sound click */
    else if (ctrl->click <= 66)
	ibmKeyClick = KSCLICKMED;		/* medium sound click */
    else
	ibmKeyClick = KSCLICKHI;		/* high sound click */

    /* if device is not open at this moment, do nothing */
    if (pPriv->fd != -1)
	ioctl(pPriv->fd, KSCFGCLICK, &ibmKeyClick);


    /* change bell volume */
    if (ctrl->bell == 0)
	ibmBellVolume = KSAVOLOFF;		/* disable bell */
    else if (ctrl->bell <= 33)
	ibmBellVolume = KSAVOLLOW;		/* low bell */
    else if (ctrl->bell <= 66)
	ibmBellVolume = KSAVOLMED;		/* medium bell */
    else
	ibmBellVolume = KSAVOLHI;			/* high bell */

    /* if device is not open at this moment, do nothing */
    if (pPriv->fd != -1)
	ioctl(pPriv->fd, KSVOLUME, &ibmBellVolume);

    TRACE(("Exit aixChangeKeybdControl\n"));
    return;
}

/***============================================================***/
/**
 **  FUNCTION:
 **      Ring a bell on the keyboard.  By setting volume for the bell,
 **      ring the bell, then reset the bell with original value.
 **/
/***============================================================***/


static void
aixBell(loud, pDev, cntrl, what)
    int loud;
    DeviceIntPtr pDev;
    pointer cntrl;
    int what;
{
    uint volume;
    struct ksalarm alarm;
    KeybdCtrl pKbd = ((DeviceIntPtr) pDev)->kbdfeed->ctrl;
    InputDevPrivate  *pPriv = &lftInputInfo[KEYBOARD_ID];

    TRACE(("Enter aixBell(loud= %d, pDev= 0x%x)\n", loud, pDev));
    if (pPriv->fd == -1) {
	 /* device is not opened yet, do nothing */
	 ErrorF("aixBell. Device not opened\n");
	 return;
    }

    /* convert bell volume */
    if (loud == 0)
	volume = KSAVOLOFF;	/* disable bell */
    else if (loud <= 33)
	volume = KSAVOLLOW;	/* low bell */
    else if (loud <= 66)
	volume = KSAVOLMED;	/* medium bell */
    else
	volume = KSAVOLHI;	/* high bell */

    ioctl(pPriv->fd, KSVOLUME, &volume);

    alarm.duration = ibmBellDuration;
    alarm.frequency = ibmBellPitch;

    ioctl(pPriv->fd, KSALARM, &alarm);

    /* reset bell volume */
    if (volume != ibmBellVolume)
	ioctl(pPriv->fd, KSVOLUME, &volume);

} /* aixBell */



/***============================================================***/

int aixGetKeyboardID()
{
    InputDevPrivate  *pPriv = &lftInputInfo[KEYBOARD_ID];
    int id;

     if (pPriv->fd == -1) {
         return(FALSE);
     }

     /* aixKeybd must be defined at this point !!! */
     ioctl(pPriv->fd, KSQUERYID, &id);
     return (id);
}

aixGetKbdMappings(pKeySyms, pModMap)
KeySymsPtr pKeySyms;
CARD8 *pModMap;
{
    register int i;


    TRACE(("Enter aixGetKbdMappings( pKeySyms= 0x%x, pModMap= 0x%x )\n",
		pKeySyms,pModMap));

    for (i = 0; i < MAP_LENGTH; i++)
       pModMap[i] = NoSymbol;

    pModMap[ Aix_Caps_Lock ] = LockMask;
    pModMap[ Aix_Control_L ] = ControlMask;

    pModMap[ Aix_Shift_L ] = ShiftMask;
    pModMap[ Aix_Shift_R ] = ShiftMask;
    pModMap[ Aix_Num_Lock ] = NumLockMask;

    switch (kbdType = aixGetKeyboardID() )
    {
    case KS101:
        pModMap[ Aix_Alt_L ] = Mod1Mask;
        pModMap[ Aix_Alt_R ] = Mod1Mask;
	break;

    case KS102:
        pModMap[ Aix_Alt_L ] = Mod1Mask;
        pModMap[ Aix_Alt_R ] = Mod2Mask;
	break;

    case KS106:
        pModMap[ Aix_Alt_R ] = Mod1Mask;

    default : /* Illegal keyboard */
        pModMap[ Aix_Alt_L ] = Mod1Mask;
        pModMap[ Aix_Alt_R ] = Mod1Mask;
	break;
    }

    /*
    ** There was a bug in the X11R4 Xtoolkit where XtKeysymToKeycodeList
    ** would core dump if the maxKeyCode was 255.  This minor hack will
    ** guarantee that the maxKeyCode will be less than 255 if bug comp-
    ** atibility mode is on.  This is for defect 125343.
    */
    if ((permitOldBugs == TRUE) && (AIX_MAX_KEY == 0xff)) {
	pKeySyms->maxKeyCode=       AIX_MAX_KEY - 0x01;
    } else {
	pKeySyms->maxKeyCode=       AIX_MAX_KEY;
    }

    pKeySyms->minKeyCode=       AIX_MIN_KEY;
    pKeySyms->mapWidth=         AIX_GLYPHS_PER_KEY;
    pKeySyms->map=              aixmap;
    TRACE(("Exit aixGetKbdMappings.1\n"));
}

/***============================================================***/

/* Some new vars for lft handling */
/***============================================================***/

/* Kill Sequence for the X-Server */
CARD8  aixkillMap[3] = { Aix_Control_L, Aix_Alt_L, Aix_Backspace};
/*  The modifier flags are defined by the
 *  first 3 bits of kbd_status[0]:
 *     KBDUXSHIFT (shift), KBDUXCTRL (control), KBDUXALT (alt)
 */
uchar  aixkillMasks[3] = {
     KBDUXCTRL,			/* Ctrl */
     (KBDUXLALT|KBDUXRALT),	/* Alt */
     (KBDUXCTRL|KBDUXALT),	/* modifier flags */
     };
CARD8 aixPollPending = FALSE;

/***============================================================***/
int
AIXKeybdProc(pDev, onoff)
    DevicePtr  pDev;
    int 	  onoff;
{
    extern void aixKbdEvent();
    KeySymsRec		keySyms;
    CARD8 		modMap[MAP_LENGTH];
    static char devpath[50];			/* path to open device */
    int volume, click, rc;
    InputDevPrivate *pPriv = &lftInputInfo[KEYBOARD_ID];


    TRACE(("Enter AIXKeybdProc(pDev=0x%x, onoff=0x%x )\n",pDev,onoff));


    switch (onoff) {

       case DEVICE_INIT:

	    /* get AIX keyboard mapping */
	    ibmKeybd = pDev;
	    aixGetKbdMappings( &keySyms, modMap );
#ifdef XKB
	    if (!noXkbExtension) {
		XkbComponentNamesRec	names;
		names.keymap= NULL;
		names.keycodes= "ibm(aix101)";
		names.types= "complete";
		names.compat= "complete";
		names.symbols= "us(pc101)";
		names.geometry= "sgi101";
		XkbInitKeyboardDeviceStruct((DeviceIntPtr)ibmKeybd,&names,
			&keySyms, modMap,
			aixBell, aixChangeKeybdControl);
	    }
	    else
#endif
	    rc = InitKeyboardDeviceStruct( ibmKeybd, &keySyms, modMap,
				aixBell, aixChangeKeybdControl);


	    break;

       case DEVICE_ON:
	    {
	    extern KeybdCtrl defaultKeyboardControl;
	    struct uregring reg_ring;
	    uchar kill_arg[3];			/* for KSKAP */

	    /* has anyone open the keyboard before? */
	    if (pDev->on == TRUE) {
		TRACE(("DEVICE_ON for keyboard AGAIN!!\n"));
		break;
	    }
	    if ((pPriv->fd = open("/dev/kbd0", O_RDWR, 0)) == -1)
		FatalError("Can't open driver %s!\n", "/dev/kbd0");

	    /* enable keep alive poll */
	    kill_arg[0] = 1;			/* num of keycode */
	    kill_arg[1] = aixkillMap[2] - AIX_MIN_KEY;	/* keycode */
	    kill_arg[2] = aixkillMasks[2];	/* modifier masks */
	    ioctl(pPriv->fd, KSKAP, kill_arg);
	    /*  register input ring for keyboard. X server uses
	     *  one input ring for all devices.
	     */
	    reg_ring.ring = (caddr_t)lft_input_ring;
	    reg_ring.size = INPUT_RING_SIZE;
	    reg_ring.report_id = KEYBOARD_ID;
	    if (ioctl(pPriv->fd, KSREGRING, &reg_ring) == -1) {
	        FatalError("Keyboard input ring registration failed!\n");
	    }
	    pPriv->eventHandler = aixKbdEvent;
	    pDev->on = TRUE;
	    break;
	    }

       case DEVICE_OFF:

	    /* unregister keyboard input ring.  */
	    ioctl(pPriv->fd, KSREGRING, NULL);

	    pPriv->eventHandler = (void(*)())NoopDDA;
	    pDev->on = FALSE;
	    break;

       case DEVICE_CLOSE:
	    close (pPriv->fd);
	    break;
    } /* switch */

    TRACE(("Exit AIXKeybdProc\n"));
    return Success; /* always return Success */
}


/*---------------------------------- LFT End ------------------------------*/
#endif


/***============================================================***/

Bool LegalModifier (key, pDev)
    unsigned int key;
    DevicePtr pDev;
{
    TRACE(("LegalModifier(key= 0x%x)\n",key));
    return TRUE ;
}
