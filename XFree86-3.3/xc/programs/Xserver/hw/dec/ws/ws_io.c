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

/* $XConsortium: ws_io.c /main/18 1996/03/01 16:17:00 kaleb $ */

#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/tty.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/devio.h>
#define  NEED_EVENTS
#include "misc.h"
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "pixmap.h"
#include "inputstr.h"
#include "cursorstr.h"
#include "regionstr.h"
#include "resource.h"
#include "dixstruct.h"
#include "servermd.h"
#include <sys/workstation.h>
#include <sys/inputdevice.h>
#include <sys/wsdevice.h>
#include "ws.h"
#include "keynames.h"
#ifdef XKB
#include "XKBsrv.h"
#ifdef __alpha
#include <alpha/hal_sysinfo.h> /* for GSI_KEYBOARD */
#endif
#endif

#include "mi.h"

#define MOTION_BUFFER_SIZE 100

extern ws_descriptor wsinfo;
void wsCursorControl();
static Bool wsDisplayCursor();
static Bool wsSetCursorPosition();

extern ws_screen_descriptor screenDesc[];
extern int wsFd;
int rememberedScreen;
static int shiftLock;
static short lockLed;
ScreenPtr	wsScreens[MAXSCREENS];
ScreenArgsRec	screenArgs[MAXSCREENS];
static DevicePtr	wsPointer;
static DevicePtr	wsKeyboard;
char *blackValue, *whiteValue;
extern	ws_event_queue	*queue;
CARD32 lastEventTime;
int wsNumButtons = -1;
static Bool cursorConfined = FALSE;

#define MAX_LED 3	/* only 3 LED's can be set by user; Lock LED
			   is controlled by server */

#define VSYNCFIXED
#ifdef VSYNCFIXED
#define CURRENT_TIME	queue->time
#else
#define CURRENT_TIME	GetTimeInMillis()
#endif

#define NoSuchClass -1

static int
ParseClass(className)
    char *	className;
{
    static char *names[] = {
	"StaticGray", 
	"GrayScale", 
	"StaticColor",
	"PseudoColor", 
	"TrueColor"};
    /* 
     * only the ones we support and must be in order from X.h, since
     * return value depends on index into array.
     */
    int i;
    for (i = 0; i < sizeof(names)/sizeof(char *); i++)
    {
	if (strcmp(names[i], className) == 0)
	    return i;
    }
    return NoSuchClass;
}

/* SaveScreen does blanking, so no need to worry about the interval timer */

Bool
wsSaveScreen(pScreen, on)
    ScreenPtr pScreen;
    int on;
{
    ws_video_control vc;
    vc.screen = screenDesc[pScreen->myNum].screen;

    if (on == SCREEN_SAVER_FORCER) {
	lastEventTime = CURRENT_TIME;
    } else if (on == SCREEN_SAVER_ON) {
	vc.control = SCREEN_OFF;
	if (ioctl(wsFd, VIDEO_ON_OFF, &vc) < 0)
	    ErrorF("VIDEO_ON_OFF: failed to turn screen off.\n");
    } else {
	vc.control = SCREEN_ON;
	if (ioctl(wsFd, VIDEO_ON_OFF, &vc) < 0)
	    ErrorF("VIDEO_ON_OFF: failed to turn screen on.\n");
    }
    return TRUE;
}


wsPixelError(index)
int index;
{
    ErrorF("Only 0 through 255 are acceptable pixels for device %d\n", index);
}

void
wsClick(click)
    int         click;
{
    ws_keyboard_control control;
    control.device_number = wsinfo.console_keyboard;
    control.flags = WSKBKeyClickPercent;
    control.click = click;
    if (ioctl (wsFd, SET_KEYBOARD_CONTROL, &control) == -1)
      printf ("couldn't set click\n");
    return;
}

static void
wsChangeKeyboardControl(device, ctrl)
    DeviceIntPtr device;
    KeybdCtrl *ctrl;
{
    int i;
    ws_keyboard_control control;
    control.device_number = wsinfo.console_keyboard;
    control.flags = 0;

    /* 
     * even though some of these are not implemented on the lk201/lk401,
     * we should pass these to the driver, as other hardware may not
     * lose so badly.  The new driver does do auto-repeat and up down
     * properly, however.
     */
    control.flags |=  WSKBKeyClickPercent| WSKBBellPercent | WSKBBellPitch | 
	WSKBBellDuration | WSKBAutoRepeatMode | WSKBAutoRepeats | WSKBLed;
    control.click = ctrl->click;
    control.bell = ctrl->bell;
    control.bell_pitch = ctrl->bell_pitch;
    control.bell_duration = ctrl->bell_duration;
    control.auto_repeat = ctrl->autoRepeat;
    control.leds = ctrl->leds;
    /* 
     * XXX a crock, but to have a byte interface would have implied the
     * driver did alot more work at interrupt time, so we made it 32 bits wide.
     */
    memmove(control.autorepeats, ctrl->autoRepeats, 32);
#ifdef notdef
    /* LEDs */
    for (i=1; i<=MAX_LED; i++)
        ChangeLED(i, (ctrl->leds & (1 << (i-1))));
#endif
    if (ioctl (wsFd, SET_KEYBOARD_CONTROL, &control) == -1)
      ErrorF ("couldn't set global autorepeat\n");
    return;

}

static void
wsBell(loud, pDevice, ctrl, fbclass)
    int loud;
    DeviceIntPtr pDevice;
    pointer ctrl;
    int fbclass;
{
    ws_keyboard_control control;
    control.device_number = wsinfo.console_keyboard;
    control.flags = WSKBBellPercent;
    control.bell = loud;
    ioctl(wsFd, SET_KEYBOARD_CONTROL, &control);

    ioctl(wsFd, RING_KEYBOARD_BELL, &wsinfo.console_keyboard);
}

/*
 * These serve protocol requests, setting/getting acceleration and threshold.
 * X10 analog is "SetMouseCharacteristics".
 */
void
wsChangePointerControl(device, ctrl)
    DeviceIntPtr device;
    PtrCtrl   *ctrl;
{
    ws_pointer_control pc;
    pc.device_number = wsinfo.console_pointer;
    pc.numerator = ctrl->num;
    pc.denominator = ctrl->den;
    pc.threshold = ctrl->threshold;
    if (ioctl (wsFd, SET_POINTER_CONTROL, &pc) == -1) {
	ErrorF ("error setting mouse properties\n");
    }
}

int
wsGetMotionEvents(pDevice, buff, start, stop, pScr)
    DeviceIntPtr pDevice;
    xTimecoord *buff;
    unsigned long start, stop;
    ScreenPtr pScr;
{
    register int    i;		/* Number of events left to process in ring */
    ws_motion_buffer *mb = queue->mb;
    register ws_motion_history *mh = mb->motion; /*Beginning of ring storage*/
    int		    count;      /* Number of events that match conditions   */
    CARD32 temptime;

    /* Loop through entire ring buffer.  Technically, the driver may not have
       actually queued this many motion events, but since they are initialized
       to time 0 the non-events shouldn't match.  If the mouse isn't moved for
       25 days after startup this could be a problem...but who cares? */
    count = 0;
    for (i = mb->size; i != 0; i--) {
	temptime = mh->time;
	if (start <= temptime && temptime <= stop) {
	    buff[count].time = temptime;
	    buff[count].x    = mh->axis[0];
	    buff[count].y    = mh->axis[1];
	    if (temptime == mh->time) count++;	/* paranoid */
	}
	mh = (ws_motion_history *) ((int)mh + mb->entry_size);
    } 
    return count;
}

int
wsMouseProc(pDev, onoff, argc, argv)
    DevicePtr pDev;
    int onoff, argc;
    char *argv[];
{
    int     i, numButtons;
    BYTE    map[6];
    ws_hardware_type wht;

    switch (onoff)
    {
	case DEVICE_INIT: 
	    wht.device_number = wsinfo.console_pointer;
	    ioctl(wsFd, GET_DEVICE_TYPE, &wht);
	    wsPointer = pDev;
	    pDev->devicePrivate = (pointer) &queue;
	    map[1] = 1;  /* worst case is 5 buttons  - jmg */
	    map[2] = 2;
	    map[3] = 3;
	    map[4] = 4;
	    map[5] = 5;
	    if(wsNumButtons == -1) {
#ifndef VSXXX
		numButtons = wht.buttons; /* believe the Kernel :-) */
#else
		if(wht.hardware_type == VSXXX)
		    numButtons = 3;
		else 
		    numButtons = 4;
#endif
	    } 
	    else
		numButtons = wsNumButtons;
	    InitPointerDeviceStruct(
		wsPointer, map, numButtons, wsGetMotionEvents,
		wsChangePointerControl, MOTION_BUFFER_SIZE);
	    SetInputCheck((HWEventQueuePtr)&queue->head, 
			  (HWEventQueuePtr)&queue->tail);
	    break;
	case DEVICE_ON: 
	    pDev->on = TRUE;
	    AddEnabledDevice(wsFd); 
	    break;
	case DEVICE_OFF: 
	    pDev->on = FALSE;
	    RemoveEnabledDevice(wsFd);
	    break;
	case DEVICE_CLOSE:
	    break;
    }
    return Success;
}

/* since this driver does up/down autorepeat right, any key can be a modifier*/
/*ARGSUSED*/
Bool
LegalModifier(key, pDev)
    unsigned int key;
    DevicePtr	pDev;
{
    return TRUE;
}

#ifdef XKB
static char *languages[] = {
    "danish",		/* 30 Dansk */
    "german",		/* 32 Deutsch */
    "swiss_german",	/* 34 Deutsch(Schweiz) */
    "us",		/* 36 English(American) */
    "uk",		/* 38 English(British/Irish) */
    "spanish",		/* 3a Espanol */
    "french",		/* 3c Francais */
    "canadian_french",	/* 3e Francais(Canadien) */
    "swiss_french",	/* 40 Francais(SuisseRomande) */
    "italian",		/* 42 Italiano */
    "dutch",		/* 44 Nederlands */
    "norwegian",	/* 46 Norsk */
    "portuguese",	/* 48 Portugues */
    "finnish",		/* 4a Suomi */
    "swedish",		/* 4c Svenska */
    "flemish"		/* 4e Vlaams */
};

#define ENGLISH_AMERICAN 3 /* languages[3] = us, hardcoded fallback */
#endif /* XKB */

int
wsKeybdProc(pDev, onoff, argc, argv)
    DevicePtr pDev;
    int onoff, argc;
    char *argv[];
{
    KeySymsRec keySyms;
    CARD8 modMap[MAP_LENGTH];

    switch (onoff)
    {
	case DEVICE_INIT: 
/* Note that keyclick is off by default.  The QDSS MIT server sets it
   to 20 */	
	    wsKeyboard = pDev;
	    GetKeyboardMappings( &keySyms, modMap);
#if defined(XKB) && defined(__alpha)
	    if (!noXkbExtension)
	    {
		XkbComponentNamesRec	names;
		char keyboard[8];
		char lang[4];
		char keymapname[80];
		unsigned int langindex;
		char *p;
		int i;

		/* Set the default keymap based on the console's language 
		 * environment variable.
		 */
		lang[0] = '\0';
		if (-1 == getsysinfo(GSI_PROM_ENV, lang, sizeof(lang), 0,
				     "language"))
		    lang[0] = '\0';

		/* Find the class of keyboard being used.
		 */
		keyboard[0] = '\0';
		if (-1 == getsysinfo(GSI_KEYBOARD, keyboard, sizeof(keyboard),
				     0, NULL))
		keyboard[0] = '\0';

		/* Now put together the keymap name: digital/lang(kbtype)
		 * Eventually, we'll consult a file to find the keymap
		 * name based on the keyboard class and language, and all
		 * the code below will be a fallback in case nothing was
		 * found in the file.  For now, just rely on the fallback
		 * code.
		 *
		 * All of our keymap names start with "digital/", so ...
		 */
		keymapname[0] = '\0';
		strcat(keymapname, "digital/");

		/* Next you tack on the language.  If we don't understand
		 * what getsysinfo returned, use the fallback.
		 */
		langindex = ((unsigned)lang[0] - MIN_LANG_CODE) / 2;
		if (langindex >= sizeof(languages) / sizeof(char *))
		    langindex = ENGLISH_AMERICAN;
		strcat(keymapname, languages[langindex]);

		/* Lastly you append the keyboard type.  Convert it to lower
		 * case first.  Funky special case: for non-us keyboards,
		 * change "lk443" to "lk444".  (Why not just name the
		 * keymap files consistently?)
		 */

		i = strlen(keymapname);
		keymapname[i++] = '(';
		for (p = keyboard; *p; p++) {
		    keymapname[i++] = tolower(*p);
		}
		if (!strcmp("LK443", keyboard) &&
		    langindex != ENGLISH_AMERICAN) {
		    keymapname[i-1] = '4';
		}
		keymapname[i++] = ')';
		keymapname[i++] = '\0';
#ifdef DEBUG
		ErrorF("keymap name is %s\n", keymapname);
#endif
		names.keymap= keymapname;

		/* the chosen keymap file specifies all of these */

		names.keycodes= NULL;
		names.types=    NULL;
		names.compat=   NULL;
		names.symbols=  NULL;
		names.geometry= NULL;
		XkbInitKeyboardDeviceStruct((DeviceIntPtr)wsKeyboard,
					    &names, &keySyms, modMap, wsBell,
					    wsChangeKeyboardControl);
	    }
	    else
#endif /* XKB */
	    InitKeyboardDeviceStruct(
		    wsKeyboard, &keySyms, modMap, wsBell,
		    wsChangeKeyboardControl);

    /* Free the key sym mapping space allocated by GetKeyboardMappings. */
	    Xfree(keySyms.map);  
	    break;
	case DEVICE_ON: 
	    pDev->on = TRUE;
	    AddEnabledDevice(wsFd); 
	    break;
	case DEVICE_OFF: 
	    pDev->on = FALSE;
	    RemoveEnabledDevice(wsFd);
	    break;
	case DEVICE_CLOSE: 
	    break;
    }
    return Success;
}
extern int screenIsSaved;
static CursorPtr	currentCursor;


/* The code below is for backward compatibility with DEC R3 servers.
 * Load the keyboard map pointed to by the file "/usr/lib/X11/keymap_default"
 */

#define FileNameLength 256
#define MaxLineLength 256


KeySym *LoadKeymap();

/*
 * Load the default keymap file.
 */

int GetDefaultMap(ks)
    KeySymsPtr ks;
{
    char keymap_name[FileNameLength];
    KeySym *keymap_pointer;
    int keymap_width;
    int keymap_loaded = FALSE;

    if (GetKeymapName (keymap_name)==TRUE) {
	if ((keymap_pointer = LoadKeymap (keymap_name, MIN_LK201_KEY,
					MAX_LK201_KEY, &keymap_width)) !=NULL) {
	    ks->minKeyCode = MIN_LK201_KEY;
	    ks->maxKeyCode = MAX_LK201_KEY;
	    ks->mapWidth = keymap_width;
	    ks->map = keymap_pointer;
	    keymap_loaded=TRUE;
	}
    }
    return (keymap_loaded);
}


/*
 * Check for keymap type file "/usr/lib/X11/keymap_default"
 */

int GetKeymapName (name_return)
    char *name_return;
{
    char *filename = "/usr/lib/X11/keymap_default";
    int fd;
    int len;

    if ((fd = open(filename, O_RDONLY)) == -1)
	return (FALSE);
    len = strlen(filename);
    strcpy(name_return, filename);
    name_return[len] = '\0';
    return (TRUE);
}


#define EndLine(c) (((c)=='!' || (c) =='\n' || (c) == '\0') ? TRUE : FALSE )

/*
 * load the keymap file into  keysym table
 */

KeySym *LoadKeymap(keymap_name, minkc, maxkc, return_ks_per_kc)
    char *keymap_name;
    KeyCode minkc;
    KeyCode maxkc;
    int *return_ks_per_kc;
{
    FILE *fp;
    KeySym *keymap = NULL;
    char line[MaxLineLength];

    if ((fp=fopen (keymap_name, "r")) == NULL)
	return ( (KeySym *) NULL);

    while (fgets (line, MaxLineLength, fp) != NULL) {
	if (AddLineToKeymap (line, &keymap, minkc, maxkc, return_ks_per_kc)
								==FALSE) {
	    fclose (fp);
	    if (keymap != NULL) Xfree (keymap);
	    return ( (KeySym *)NULL);
	}
    }
    fclose (fp);
    return ( keymap );
}    


/*
 * decode keycode, and keysyms from line, allocate keymap first time round.
 */

int AddLineToKeymap (line, keymap, minkc, maxkc, return_ncols)
    char *line;
    KeySym **keymap;
    KeyCode minkc;
    KeyCode maxkc;
    int *return_ncols;
{
    int pos;
    KeyCode kc;
    KeySym *offset;
    KeySym ks;
    int ncols;
    int col;
    int map_size;
    int i;

    if (isspace(line[0]) || line[0] == '!' || line[0] == '\n' || line[0] == '\0')
	return (TRUE); /* ignore blank lines and comments */

    if ( *keymap ==NULL) {
	pos=0;
	if ((kc=GetToken (line, &pos)) == -1) return (FALSE);
	ncols=0;
	while (GetToken (line, &pos) != -1)
	    ncols++;
	if (ncols < 2) ncols = 2;
	(*return_ncols) = ncols;
	map_size = (maxkc-minkc+1) * ncols;
	(*keymap) = (KeySym *) Xalloc ( map_size * sizeof (KeySym));
	for (i = 0; i < map_size; i++)
	    (*keymap)[i] = NoSymbol;

    }
    pos = 0;
    if ((kc=GetToken (line, &pos)) == -1) return (FALSE);
    if ( kc < minkc || kc > maxkc ) return (FALSE);
    offset = (*keymap) + (kc-minkc)* (*return_ncols);
    col=0;
    while (col < (*return_ncols) && ((ks=GetToken (line, &pos)) != -1)) {
	*(offset + col) = ks;
	col+=1;
    }
    return (TRUE);
}

/*
 * return hex value of next item on line (current position held in 'pos')
 */

int GetToken (line, pos)
    char *line;
    int *pos;
{
    int start;

    if (EndLine(line[*pos]) == TRUE) return (-1);
    while (isspace(line[*pos]) || EndLine(line[*pos])) {
	if (EndLine(line[*pos]) == TRUE)
		return (-1);
	(*pos)++;
    }
    start = *pos;
    while (!isspace (line[*pos]) && !EndLine (line[*pos])) {
	(*pos)++;
    }
    return (StringToHex(&line[start], (*pos)-start));
}


/*
 * convert null terminated hexadecimal string to integer
 * return 'value', or '-1' on error
 */

int StringToHex (str,nbytes)
    char *str;
    int nbytes;
{
    int i;
    int digit;
    int scale = 1;
    long value = 0;

    for (i=nbytes-1;i>=0;i--) {
	if (!isxdigit(str[i])) return (-1);
	if (isdigit(str[i]))
		digit=str[i]-'0';
	else
		digit=toupper(str[i])-'A'+10;
	value+=(digit*scale);
	scale*=16;
    }
    return(value);
}

#undef EndLine
#undef MaxLineLength
#undef FileNameLength


Bool GetKeyboardMappings(pKeySyms, pModMap)
    KeySymsPtr pKeySyms;
    CARD8 *pModMap;
{
    int i;
    ws_keyboard_definition def;
    KeySym *map;
    KeySym rawsyms[256];		/* more than we'll ever need! */
    unsigned char rawcodes[256];	/* more than we'll ever need! */
    ws_keycode_modifiers mods[32];	/* more than we'll ever need! */
    ws_keysyms_and_modifiers km;
    int min_keycode = 256, max_keycode = 0;

    def.device_number = wsinfo.console_keyboard;
    if (ioctl (wsFd, GET_KEYBOARD_DEFINITION, &def) == -1) {
	ErrorF ("error getting keyboard definition\n");
    }
    lockLed = def.lock_key_led;

    /* If it exists, load special keysym map from file instead of driver.
	This is for backward compatibility with the i18n stuff from the
	DEC R3 servers.
    */
    if (GetDefaultMap(pKeySyms) == TRUE) {
	for (i = 0; i < MAP_LENGTH; i++)
	    pModMap[i] = NoSymbol;	/* make sure it is restored */
	pModMap[ KEY_LOCK ] = LockMask;
	pModMap[ KEY_SHIFT ] = ShiftMask;
	pModMap[ KEY_SHIFT_R ] = ShiftMask;
	pModMap[ KEY_CTRL ] = ControlMask;
	pModMap[ KEY_COMPOSE ] = Mod1Mask;
	pModMap[ KEY_COMPOSE_R ] = Mod1Mask;
	pModMap[ KEY_ALT_L ] = Mod2Mask;
	pModMap[ KEY_ALT_R ] = Mod2Mask;
	return (TRUE);
    }

    km.device_number = wsinfo.console_keyboard;
    km.modifiers = mods;
    *((KeySym **)(&km.keysyms)) = rawsyms; /* XXX bad type in inputdevice.h */
    km.keycodes = rawcodes;
    if (ioctl (wsFd, GET_KEYSYMS_AND_MODIFIERS, &km) == -1) {
	ErrorF ("error getting keysyms and modifiers\n");
    }

#define KEYCODE(i) ((i) >> 24))
    for (i = 0; i < def.keysyms_present; i++) {
	if (rawcodes[i] > max_keycode) max_keycode = rawcodes[i];
	if (rawcodes[i] < min_keycode) min_keycode = rawcodes[i];
    }
	
    map = (KeySym *)Xalloc(sizeof(KeySym) * 
		    (MAP_LENGTH * def.keysyms_per_keycode));
    if (!map)
	return FALSE;

    for (i = 0; i < MAP_LENGTH; i++)
	pModMap[i] = NoSymbol;	/* make sure it is restored */

    /* first set up modifier keys */
    for (i = 0; i < def.modifier_keycode_count; i++)
	pModMap[mods[i].keycode] = mods[i].modbits;

    /* initialize the keysym array */
    for (i = 0; i < (MAP_LENGTH * def.keysyms_per_keycode); i++)
	map[i] = NoSymbol;
    pKeySyms->minKeyCode = min_keycode;
    pKeySyms->maxKeyCode = max_keycode;
    pKeySyms->mapWidth   = def.keysyms_per_keycode;
    pKeySyms->map = map;
#define INDEX(in) ((in - min_keycode) * def.keysyms_per_keycode)
    for (i = 0; i < def.keysyms_present; i++) {
	register int j;
	for (j = 0; j < def.keysyms_per_keycode; j++) {
		if (map[INDEX(rawcodes[i]) + j] == NoSymbol) {
			map[INDEX(rawcodes[i]) + j] = rawsyms[i];
			break;
		}
	}
    }
#undef INDEX
    return TRUE;
}


void
SetLockLED (on)
    Bool on;
{
    ws_keyboard_control kc;
    kc.flags = WSKBLed;
    kc.device_number = wsinfo.console_keyboard;
    if (ioctl (wsFd, GET_KEYBOARD_CONTROL, &kc) == -1) {
        ErrorF ("error getting keyboard control\n");
    }
    if(on)
        kc.leds |= 1 << (lockLed -1);
    else
	kc.leds &= ~(1 << (lockLed -1));
    kc.flags = WSKBLed;
    if (ioctl (wsFd, SET_KEYBOARD_CONTROL, &kc) == -1) {
        ErrorF ("error setting keyboard control\n");
    }
}

/*
 * The driver has been set up to put events in the queue that are identical
 * in shape to the events that the DDX layer has to deliver to ProcessInput
 * in DIX.
 */
void
ProcessInputEvents()
{
    xEvent x;
    register ws_event *e;
    ws_event etmp;
    int screen;
    DeviceIntPtr dev = (DeviceIntPtr) wsKeyboard;

    e = &etmp;
    while (queue->head != queue->tail)  {
        /* The events should be popped off the queue before they
         * are processed.  This bug surfaced because XKB indirectly
         * turned ProcessInputEvents into a recursive routine.
         */
        memmove(&etmp,
#ifdef __alpha
                (caddr_t)(queue->events) + queue->event_size * queue->head,
#else
                (int)(queue->events) + queue->event_size * queue->head,
#endif
                queue->event_size);

        if (queue->head >= queue->size - 1)
            queue->head = 0;
        else
            ++queue->head;

	if (screenIsSaved == SCREEN_SAVER_ON)
	    SaveScreens(SCREEN_SAVER_OFF, ScreenSaverReset);
   	if(e->screen != rememberedScreen)
	{
		ScreenPtr	pScreen;
		int		x, y;

		if (cursorConfined)
		{
		    /* OS doesn't work right -- we have to confine here */
		    GetSpritePosition (&x, &y);
		    pScreen = wsScreens[rememberedScreen];
		    (void) wsSetCursorPosition (pScreen, x, y, FALSE);
		    wsCursorControl(e->screen, CURSOR_OFF);
		    wsCursorControl(rememberedScreen, CURSOR_ON);
		    if (currentCursor)
			wsDisplayCursor(pScreen, currentCursor);
		}
		else
		{
		    /* assumption -- this is a motion event */
		    wsCursorControl(rememberedScreen, CURSOR_OFF);
		    wsCursorControl(e->screen, CURSOR_ON);
		    rememberedScreen = e->screen;
		    pScreen = wsScreens[e->screen];
		    if (currentCursor)
			    wsDisplayCursor (pScreen, currentCursor);
		    x = e->e.key.x;
		    y = e->e.key.y;
		    NewCurrentScreen(pScreen, x, y);
		}
		continue;
    	}

	x.u.keyButtonPointer.rootX = e->e.key.x;
	x.u.keyButtonPointer.rootY = e->e.key.y;
	x.u.keyButtonPointer.time = lastEventTime = e->time;
	x.u.u.detail = e->e.key.key;

	switch (e->device_type) {
	  case KEYBOARD_DEVICE:
		    switch (e->type) {
			case BUTTON_DOWN_TYPE: 
			    x.u.u.type = KeyPress;
#ifdef XKB
			    if (noXkbExtension)
#endif
			    {
				/* if key is a lock modifier then ... */
				if (dev->key->modifierMap[e->e.key.key] & LockMask){
				    if (shiftLock) {
					x.u.u.type = KeyRelease;
					SetLockLED (FALSE);
					shiftLock = FALSE;
				    } else {
					x.u.u.type = KeyPress;
					SetLockLED (TRUE);
					shiftLock = TRUE;
				    }
				}
			    }
			    (*wsKeyboard->processInputProc) (&x, dev, 1);
			    break;
			case BUTTON_UP_TYPE: 
#ifdef XKB
			    if (noXkbExtension)
#endif
			    {
				/* if key is a lock modifier then ignore */
				if (dev->key->modifierMap[e->e.key.key] & LockMask)
				    break;
			    }
			    x.u.u.type = KeyRelease;
			    (*wsKeyboard->processInputProc) (&x, dev, 1);
			    break;
			default: 	       /* hopefully BUTTON_RAW_TYPE */
			    break;
		    }
		    break;
	    case MOUSE_DEVICE:
	    /* someday tablet will be handled differently than a mouse */
	    case TABLET_DEVICE:
		    switch (e->type) {
			case BUTTON_DOWN_TYPE: 
			    x.u.u.type = ButtonPress;
			    break;
			case BUTTON_UP_TYPE: 
			    x.u.u.type = ButtonRelease;
			    break;
			case MOTION_TYPE: 
			    x.u.u.type = MotionNotify;
			    break;
			default: 
#ifdef DEBUG
			    printf("Unknown mouse or tablet event = %d\n",
				e->type);
#endif
			    continue;
		    }
		    (*wsPointer->processInputProc)
			(&x, (DeviceIntPtr) wsPointer, 1);
		    break;
	    /* new input devices go here (or are ignored) */
	    default:
#ifdef XINPUT
		    if (!ExtProcessInputEvents(&x, e))
#  ifdef DEBUG
		      printf("Unknown device type = %d\n",e->device_type);
#  else
		      ; /* do nothing */
#  endif
#else
#  ifdef DEBUG
		    printf("Unknown device type = %d\n",e->device_type);
#  endif
#endif
		break;
	}
    }
}

CARD32
GetTimeInMillis()
{
    struct timeval  tp;
#ifdef VSYNCFIXED
    if (queue)
	return queue->time;
#endif
    gettimeofday(&tp, 0);
    return(tp.tv_sec * 1000) + (tp.tv_usec / 1000);
}

TimeSinceLastInputEvent()
{
    if (lastEventTime == 0)
	lastEventTime = CURRENT_TIME;
    return CURRENT_TIME -  lastEventTime;
}

SetTimeSinceLastInputEvent ()
{
    lastEventTime = CURRENT_TIME;
}

/*
 * set the bounds in the device for this particular cursor
 */
static void
wsConstrainCursor( pScr, pBox)
    ScreenPtr	pScr;
    BoxPtr	pBox;
{
    ws_pointer_box wsbox;
    wsbox.screen = screenDesc[pScr->myNum].screen;
    wsbox.enable = PointerConfinedToScreen();
    cursorConfined = wsbox.enable;
    wsbox.device_number = wsinfo.console_pointer;
    wsbox.box.bottom = pBox->y2;
    wsbox.box.right = pBox->x2;
    wsbox.box.top = pBox->y1;
    wsbox.box.left = pBox->x1;

    if (ioctl(wsFd, SET_POINTER_BOX, &wsbox) == -1)
	    ErrorF("SET_POINTER_BOX: failed to set pointer box.\n");
}

static Bool
wsSetCursorPosition( pScr, newx, newy, generateEvent)
    ScreenPtr	pScr;
    unsigned int	newx;
    unsigned int	newy;
    Bool		generateEvent;
{
    ws_pointer_position pos;
    xEvent motion;

    pos.screen = screenDesc[pScr->myNum].screen;
    pos.device_number = wsinfo.console_pointer;
    pos.x = newx;
    pos.y = newy;

    /* if this is on a different screen, then we need to switch... */
    if (pos.screen != rememberedScreen) {
		wsCursorControl(rememberedScreen, CURSOR_OFF);
		wsCursorControl(pos.screen, CURSOR_ON);
		rememberedScreen = pos.screen;
    }
    if (ioctl (wsFd, SET_POINTER_POSITION, &pos) == -1) {
	ErrorF ("error warping cursor\n");
	return FALSE;
    }

    if (generateEvent) {
	if (queue->head != queue->tail)
	    ProcessInputEvents ();
	motion.u.keyButtonPointer.rootX = newx;
	motion.u.keyButtonPointer.rootY = newy;
	motion.u.keyButtonPointer.time = currentTime.milliseconds;
	motion.u.u.type = MotionNotify;
	(wsPointer->processInputProc)(&motion, (DeviceIntPtr) wsPointer, 1);
    }
    return TRUE;
}

static Bool
wsDisplayCursor( pScr, pCurs)
    ScreenPtr	pScr;
    CursorPtr	pCurs;
{
    ws_cursor_data cd;
    ws_cursor_color cc;
#ifdef __alpha
    unsigned int sourcebits[1024], maskbits[1024];
    unsigned char *pSrc, *pDst;
    int i;
    int widthBytesLineSrc, widthBytesLineDest;
#endif
    cd.screen = screenDesc[pScr->myNum].screen;
    cd.width = pCurs->bits->width;
    cd.height = pCurs->bits->height;
    cd.x_hot =  pCurs->bits->xhot;
    cd.y_hot =  pCurs->bits->yhot;
#ifdef __alpha
    /*
     * convert from an image padded on 8-byte boundaries to an
     * image padded on 4-byte boundaries for the hardware
     */
    widthBytesLineSrc = BitmapBytePad (pCurs->bits->width);
    widthBytesLineDest = BitmapBytePadProto (pCurs->bits->width);

    pSrc = (unsigned char*) pCurs->bits->source;
    pDst = (unsigned char*) sourcebits;
    for (i = 0; i < pCurs->bits->height; 
	i++, pSrc += widthBytesLineSrc, pDst += widthBytesLineDest)
	memmove((void*) pDst, (void*)pSrc, widthBytesLineDest);
    cd.cursor = sourcebits;
    pSrc = (unsigned char*) pCurs->bits->mask;
    pDst = (unsigned char*) maskbits;
    for (i = 0; i < pCurs->bits->height; 
	i++, pSrc += widthBytesLineSrc, pDst += widthBytesLineDest)
	memmove((void*) pDst, (void*)pSrc, widthBytesLineDest);
    cd.mask = maskbits;
#else
    cd.cursor = (unsigned int *) pCurs->bits->source;
    cd.mask =   (unsigned int *) pCurs->bits->mask;
#endif
    if ( ioctl( wsFd, LOAD_CURSOR, &cd) == -1)    {
	ErrorF( "error loading bits of new cursor\n");
        return FALSE;
    }
    cc.screen = screenDesc[pScr->myNum].screen;
    cc.background.red = pCurs->backRed;
    cc.background.green = pCurs->backGreen;
    cc.background.blue = pCurs->backBlue;
    cc.foreground.red = pCurs->foreRed;
    cc.foreground.green  = pCurs->foreGreen;
    cc.foreground.blue = pCurs->foreBlue;
    if ( ioctl(wsFd, RECOLOR_CURSOR, &cc) == -1) {
	ErrorF( "error writing colors of new cursor\n");
        return FALSE;
    }
    currentCursor = pCurs;
    return (TRUE);
}

void 
wsCursorControl(screen, control)
    int screen;
    int control;
{
    ws_cursor_control cc;
    cc.screen = screenDesc[screen].screen;
    cc.control = control;
    if (ioctl(wsFd, CURSOR_ON_OFF, &cc) == -1) {
	ErrorF( "error enabling/disabling cursor\n");
    }
    return;
}
static void
wsRecolorCursor (pScr, pCurs, displayed)
    ScreenPtr	pScr;
    CursorPtr	pCurs;
    Bool	displayed;
{
    ws_cursor_color cc;
    if (!displayed)
	return;
    cc.screen = screenDesc[pScr->myNum].screen;
    cc.background.red = pCurs->backRed;
    cc.background.green = pCurs->backGreen;
    cc.background.blue = pCurs->backBlue;
    cc.foreground.red = pCurs->foreRed;
    cc.foreground.green  = pCurs->foreGreen;
    cc.foreground.blue = pCurs->foreBlue;
    if ( ioctl(wsFd, RECOLOR_CURSOR, &cc) == -1)    {
	ErrorF( "error writing colors of new cursor\n");
    }
}

static Bool
wsRealizeCursor( pScr, pCurs)
    ScreenPtr	pScr;
    CursorPtr	pCurs;	/* The new driver makes this easy */
{
    return TRUE;
}

static Bool
wsUnrealizeCursor( pScr, pCurs)
    ScreenPtr	pScr;
    CursorPtr	pCurs;
{
    if (pCurs == currentCursor)
	currentCursor = 0;
    return TRUE;
}

static void
wsCursorLimits( pScr, pCurs, pHotBox, pPhysBox)
    ScreenPtr	pScr;
    CursorPtr	pCurs;
    BoxPtr	pHotBox;
    BoxPtr	pPhysBox;	/* return value */
{
    wsScreenPrivate *wsp = (wsScreenPrivate *)
		pScr->devPrivates[wsScreenPrivateIndex].ptr;
    pPhysBox->x1 = max( pHotBox->x1, 0);
    pPhysBox->y1 = max( pHotBox->y1, 0);
    pPhysBox->x2 = min( pHotBox->x2, wsp->screenDesc->width - 1);
    pPhysBox->y2 = min( pHotBox->y2, wsp->screenDesc->height - 1);
}

void
wsPointerNonInterestBox( pScr, pBox)
    ScreenPtr	pScr;
    BoxPtr	pBox;
{
    ws_pointer_box wsbox;
    wsbox.screen = screenDesc[pScr->myNum].screen;
    wsbox.device_number = wsinfo.console_pointer;
    wsbox.enable = TRUE;
    wsbox.box.top = pBox->x1;
    wsbox.box.bottom =  pBox->x2;
    wsbox.box.left = pBox->y1;
    wsbox.box.right = pBox->y2;;
    if (ioctl(wsFd, SET_ESCAPE_BOX, wsbox) == -1)
	    ErrorF("SET_ESCAPE_BOX: failed to set non interest box.\n");
}
/*
 * DDX - specific abort routine.  Called by AbortServer().
 */
void
AbortDDX()
{
}

/* Called by GiveUp(). */
void
ddxGiveUp()
{
}

int
ArgMatch(arg, template, screen)
    char *arg;
    char *template;
    int *screen;
{
    int tlen = strlen(template);
    char next = *(arg + tlen);
    if(strncmp(arg, template, tlen) == 0) {
        if(tlen == strlen(arg)) {
	    /* exact match - applies to all screens */
	    *screen = -1;
	    return TRUE;
	}
	if(isdigit(next)) {
	    /* parse off screen number */
	    *screen = atoi(arg + tlen);
	    if(*screen < MAXSCREENS)
		return TRUE;
	    else
	        return FALSE;
	}
	else
	    /* non-digit stuff at end of arg.  not ours. */
	    return FALSE;
    }
    else 
	return FALSE;
}


int
ddxProcessArgument (argc, argv, i)
    register int argc;
    register char *argv[];
    register int i;
{
    int			argind=i;
    int			skip;
    static int		Once=0;
    void		ddxUseMsg();
    int 		screen;

    skip = 0;
    if (!Once) {
        blackValue = NULL;
        whiteValue = NULL;
	Once = 1;
    }
    if (ArgMatch( argv[argind], "-dpix", &screen)) {
	if (++argind < argc) {
	    if(screen == -1) {
		for(i = 0; i < MAXSCREENS; i++) {
		    screenArgs[i].flags |= ARG_DPIX;
		    screenArgs[i].dpix = atoi(argv[argind]);
		}
	    }
	    else {
		screenArgs[screen].flags |= ARG_DPIX;
		screenArgs[screen].dpix = atoi(argv[argind]);
	    }
	    skip = 2;
	}
	else return 0;	/* failed to parse */
    }
    else if (ArgMatch( argv[argind], "-dpiy", &screen)) {
	if (++argind < argc) {
	    if(screen == -1) {
		for(i = 0; i < MAXSCREENS; i++) {
		    screenArgs[i].flags |= ARG_DPIY;
		    screenArgs[i].dpiy = atoi(argv[argind]);
		}
	    }
	    else {
		screenArgs[screen].flags |= ARG_DPIY;
		screenArgs[screen].dpiy = atoi(argv[argind]);
	    }
	    skip = 2;
	}
	else return 0;
    }
    else if (ArgMatch( argv[argind], "-dpi", &screen)) {
	if (++argind < argc) {
	    if(screen == -1) {
		for(i = 0; i < MAXSCREENS; i++) {
		    screenArgs[i].flags |= (ARG_DPI | ARG_DPIX | ARG_DPIY) ;
		    screenArgs[i].dpi = screenArgs[i].dpix =
			screenArgs[i].dpiy = atoi(argv[argind]);
		}
	    }
	    else {
		screenArgs[screen].flags |= (ARG_DPI | ARG_DPIX | ARG_DPIY);
		screenArgs[screen].dpi = screenArgs[screen].dpix  =
		       screenArgs[screen].dpiy = atoi(argv[argind]);
	    }
	    skip = 2;
	}
	else return 0;
    }
    else if(ArgMatch( argv[argind], "-bp", &screen)) {
	if (++argind < argc) {
	    if(screen == -1) {
		for(i = 0; i < MAXSCREENS; i++) {
		    screenArgs[i].flags |= ARG_BLACKVALUE;
		    screenArgs[i].blackValue = argv[argind];
		}
	    }
	    else {
		screenArgs[screen].flags |=  ARG_BLACKVALUE;
		screenArgs[screen].blackValue = argv[argind];
	    }
	    skip = 2;
	}
	else return 0;
    }
    else if (ArgMatch( argv[argind], "-wp", &screen)) {
	if (++argind < argc) {
	    if(screen == -1) {
		for(i = 0; i < MAXSCREENS; i++) {
		    screenArgs[i].flags |= ARG_WHITEVALUE;
		    screenArgs[i].whiteValue = argv[argind];
		}
	    }
	    else {
		screenArgs[screen].flags |=  ARG_WHITEVALUE;
		screenArgs[screen].whiteValue = argv[argind];
	    }
	    skip = 2;
	}
	else return 0;
    }
    else if (ArgMatch(argv[argind], "-class", &screen))  {
	if(++argind < argc)  {
	    int class = ParseClass(argv[argind]);
	    if (class == NoSuchClass)
		return 0;
	    if(screen == -1) {
		for(i = 0; i < MAXSCREENS; i++) {
		    screenArgs[i].flags |= ARG_CLASS;
		    screenArgs[i].class = class;
		}
	    }
	    else {
		screenArgs[screen].flags |=  ARG_CLASS;
		screenArgs[screen].class = class;
	    }
	    skip = 2;
        }
	else return 0;
    }
    else if (ArgMatch(argv[argind], "-edge_left", &screen))  {
	if(++argind < argc)  {
	    if(screen == -1) {
		return 0;
	    }
	    else {
		screenArgs[screen].flags |=  ARG_EDGE_L;
		screenArgs[screen].edge_left = atoi(argv[argind]);
	    }
	    skip = 2;
        }
	else return 0;
    }
    else if (ArgMatch(argv[argind], "-edge_right", &screen))  {
	if(++argind < argc)  {
	    if(screen == -1) {
		return 0;
	    }
	    else {
		screenArgs[screen].flags |=  ARG_EDGE_R;
		screenArgs[screen].edge_right = atoi(argv[argind]);
	    }
	    skip = 2;
        }
	else return 0;
    }
    else if (ArgMatch(argv[argind], "-edge_top", &screen))  {
	if(++argind < argc)  {
	    if(screen == -1) {
		return 0;
	    }
	    else {
		screenArgs[screen].flags |=  ARG_EDGE_T;
		screenArgs[screen].edge_top = atoi(argv[argind]);
	    }
	    skip = 2;
        }
	else return 0;
    }
    else if (ArgMatch(argv[argind], "-edge_bottom", &screen))  {
	if(++argind < argc)  {
	    if(screen == -1) {
		return 0;
	    }
	    else {
		screenArgs[screen].flags |=  ARG_EDGE_B;
		screenArgs[screen].edge_bottom = atoi(argv[argind]);
	    }
	    skip = 2;
        }
	else return 0;
    }

    else if (ArgMatch(argv[argind], "-monitor", &screen))  {
	if(++argind + 4 < argc)  {
	    if(strlen(argv[argind]) == 5) {
		strcpy(screenArgs[screen].monitor.type, argv[argind++]);
		screenArgs[screen].monitor.mm_width =  atoi(argv[argind++]);
		screenArgs[screen].monitor.mm_height =  atoi(argv[argind++]);
		screenArgs[screen].monitor.color_or_mono =atoi(argv[argind++]);
		screenArgs[screen].monitor.phosphor_type =atoi(argv[argind++]);
		screenArgs[screen].flags |=  ARG_MONITOR;
	        skip = 6;
	    }
	    else return 0;
        }
	else return 0;
    }
    else if (strcmp( argv[argind], "-btn") == 0)
    {
	if (++argind < argc)
	{
	    wsNumButtons = atoi(argv[argind]);
	    skip = 2;
	    if(wsNumButtons < 1 || wsNumButtons > 5)
		return 0;
	}
	else return 0;
    }
    else if (strcmp( argv[argind], "-forceDepth") == 0)
    {
	if (++argind < argc) {
	    forceDepth = atoi (argv[argind]);
	    skip = 2;
	}
    }

#ifdef XINPUT
    else
    skip = ExtddxProcessArgument(argc, argv, argind);
#endif

    return skip;
}

initEdgeAttachments(index)
    int index;
{
    ws_edge_connection wec;
    ScreenArgsRec *args = &screenArgs[index];
    int mask = args->flags;
    wec.screen = index;
    if(mask & ARG_EDGE_L)
	wec.adj_screens.left = args->edge_left;
    else
	wec.adj_screens.left = index - 1;
    if(mask & ARG_EDGE_R)
	wec.adj_screens.right =	 args->edge_right;
    else
	if(index + 1 < wsinfo.num_screens_exist)
	    wec.adj_screens.right = index + 1;	
	else
	     wec.adj_screens.right = -1;	    
    if(mask & ARG_EDGE_T)
	wec.adj_screens.top = args->edge_top;
    else
	wec.adj_screens.top = -1;
    if(mask & ARG_EDGE_B)
	wec.adj_screens.bottom = args->edge_bottom;
    else
	wec.adj_screens.bottom = -1;
    if (ioctl(wsFd, SET_EDGE_CONNECTION, &wec) == -1)
	  ErrorF("SET_EDGE_CONNECTION, failed to set edge attachment.\n");
    
}

#define ARGFILE "/etc/screens"

void getFileArguments()
{
    FILE *f =  fopen(ARGFILE, "r");
    if(f) {
        struct stat stats;
	if(stat(ARGFILE, &stats) == 0) {
	    int i, argcount = 0, skip;
	    char *ptr, *buf = (char *)Xalloc(stats.st_size);
	    char **arguments;
	    while(EOF != fscanf(f, "%s", buf)) argcount++;
	    arguments =  (char **) Xalloc(argcount * sizeof(char *));
    	    rewind(f);
	    for(ptr = buf, i = 0; i < argcount; i++) {
	        arguments[i] = ptr;
		fscanf(f, "%s", ptr);
		ptr += strlen(arguments[i]) + 1;
	    }
	    fclose(f);
	    for(i = 0; i < argcount; i++) 
		if(skip = ddxProcessArgument(argcount, arguments, i))
		    i +=  (skip - 1);
	    Xfree(arguments);
	    Xfree(buf);
	}	
    }
}


void
ddxUseMsg()
{
    ErrorF ("\n");
    ErrorF ("\n");
    ErrorF ("Device Dependent Usage\n");
    ErrorF ("Note - most ddx arguments can take an optional screen number ``s''\n");
    ErrorF ("\n");
    ErrorF ("-btn <n>              Number of buttons on pointer device\n");
    ErrorF ("-dpix[s] <n>          Dots per inch, x coordinate\n");
    ErrorF ("-dpiy[s] <n>          Dots per inch, y coordinate\n");
    ErrorF ("-dpi[s] <n>           Dots per inch, x and y coordinates\n");
    ErrorF ("                   (overrides -dpix and -dpiy above)\n");
    ErrorF ("-bp[s] #XXX           color for BlackPixel for screen\n");
    ErrorF ("-wp[s] #XXX           color for WhitePixel for screen\n");
    ErrorF ("-class[s] <classname> type of Visual for root window\n");
    ErrorF ("       one of StaticGray, StaticColor, PseudoColor,\n");
    ErrorF ("       GrayScale, or even TrueColor!\n");
    ErrorF ("-edge_left<s1> <s2> Attach left edge of screen s1 to screen s2\n");
    ErrorF ("-edge_right<s1> <s2> Attach right edge of screen s1 to screen s2\n");
    ErrorF ("-edge_top<s1> <s2> Attach top edge of screen s1 to screen s2\n");
    ErrorF ("-edge_bottom<s1> <s2> Attach bottom edge of screen s1 to screen s2\n");

#ifdef XINPUT
    ExtddxUseMsg();
#endif
}

/* ARGSUSED */
int wsScreenInit(index, pScreen, argc, argv)
    int index;
    register ScreenPtr pScreen;
    int argc;
    char **argv;
{

    pScreen->SaveScreen = wsSaveScreen;
    pScreen->RealizeCursor = wsRealizeCursor;
    pScreen->UnrealizeCursor = wsUnrealizeCursor;
    pScreen->DisplayCursor = wsDisplayCursor;
    pScreen->SetCursorPosition = wsSetCursorPosition;
    pScreen->CursorLimits = wsCursorLimits;
    pScreen->PointerNonInterestBox = wsPointerNonInterestBox;
    pScreen->ConstrainCursor = wsConstrainCursor;
    pScreen->RecolorCursor = wsRecolorCursor;
    pScreen->StoreColors = wsStoreColors;
    pScreen->InstallColormap = wsInstallColormap;
    pScreen->UninstallColormap = wsUninstallColormap;
    pScreen->ListInstalledColormaps = wsListInstalledColormaps;

    initEdgeAttachments(index);
    if(screenArgs[index].flags & ARG_MONITOR) {
	ws_monitor_type wmt;
	wmt.screen = index;
	wmt.monitor_type = screenArgs[index].monitor;
	if (ioctl(wsFd, SET_MONITOR_TYPE, &wmt) == -1)
	    ErrorF("SET_MONITOR_TYPE, failed to set monitor type.\n");
    }
    wsSaveScreen(pScreen,  SCREEN_SAVER_OFF);
    return index;
}
