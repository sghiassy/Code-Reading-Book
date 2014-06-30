/* $XConsortium: hil_driver.c,v 1.2 95/01/25 17:24:43 gildea Exp $ */
/************************************************************

Copyright (c) 1993 by Hewlett-Packard Company, Palo Alto, California

			All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Hewlett-Packard not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
HEWLETT-PACKARD BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

********************************************************/
/* 
cc -DTEST -g hil_driver.c -Aa -D_HPUX_SOURCE -I.. -I/usr/include/X11R5/X11 -I/usr/include/X11R5/X11/extensions

cc +z -c -O hil_driver.c -Aa -D_HPUX_SOURCE -I.. -I/usr/include/X11R5/X11 -I/usr/include/X11R5/X11/extensions
ld -b hil_driver.o -o hil_driver.sl
cp hil_driver.sl /usr/lib/X11/extensions

In /etc/X11/X0devices:
   first NULL keyboard
   Begin_Device_Description
   name hil_driver.sl
   use keyboard
   path keyboard
   End_Device_Description

   Begin_Device_Description
   name hil_driver.sl
   use pointer
   path mouse
   End_Device_Description

 */

static char what[] = "@(#)hil_driver : HIL device driver for X v1.0";
#define WHAT (&what[4])

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/hilioctl.h>
#include "x_serialdrv.h"
#include "X.h"
#include "XI.h"

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#define XOTHER		2	/* not defined in XI.h */
#define XEXTENSION	3	/* not defined in XI.h */

#define XPTR_USE	(1<<XPOINTER)
#define XKBD_USE	(1<<XKEYBOARD)
#define XOTH_USE	(1<<XOTHER)
#define XEXT_USE	(1<<XEXTENSION)
#define EXPLICIT	0x80


/* ******************************************************************** */
/* ******************* Misc Routines and Constants ******************** */
/* ******************************************************************** */

static int imin(a,b) register int a,b; { return (a < b) ? a : b; }
static int imax(a,b) register int a,b; { return (a > b) ? a : b; }

static void uppercase(str) char *str;	/* convert str to upper case */
  { for (; *str; str++) *str = toupper(*str); }

#define NITEMS(array) (sizeof(array)/sizeof(array[0]))

static int remap[] = {0,1,4,5,2,3,6,7,8,9,10,11,12,13,14,15};

static char *h_position[] = 
{
  "FIRST",
  "SECOND",
  "THIRD",
  "FOURTH",
  "FIFTH",
  "SIXTH",
  "SEVENTH",
  "EIGHTH",
  "NINTH",
  "TENTH",
  "ELEVENTH",
  "TWELFTH",
  "THIRTEENTH",
  "FOURTEENTH",
  "FIFTEENTH",
  "SIXTEENTH",
  "SEVENTEENTH",
  "EIGHTEENTH",
  "NINETEENTH",
  "TWENTIETH",
  "LAST"
};

/* ******************************************************************** */
/* ************************** HIL Constants *************************** */
/* ******************************************************************** */

typedef struct
{
  int hil_id_low, hil_id_high;
  char *X_name;
  int dev_type, x_type;
  int  (*read_device)();
  int reverse_y_axes;
  int  (*write_device)();
} HIL_DeviceType;

#define NULL_DEVICE		0
#define MOUSE			1
#define TABLET			2
#define KEYBOARD		3
#define QUADRATURE		4
#define TOUCHSCREEN		5
#define TOUCHPAD		6
#define BUTTONBOX		7
#define BARCODE			8
#define ONE_KNOB    		9
#define TRACKBALL		10
#define KEYPAD   		11
#define NINE_KNOB    		12
#define ID_MODULE 		13

#ifdef DEBUG
#define ONE_AXIS		14
#define NO_IOB_REL		15
#define ERROR_DEVICE		16
#define NO_AXES 		17
#define THREE_AXES 		18
#define XI_ONE_AXIS		"ONE_AXIS"
#define XI_NO_IOB_REL		"NO_IOB_REL"
#define XI_ERROR_DEVICE		"ERROR_DEVICE"
#define XI_NO_AXES     		"NO_AXES"
#define XI_THREE_AXES     	"THREE_AXES"
#endif /* DEBUG */


#define	NINE_KNOB_ID		0x61
#define	QUAD_ID			0x62	/* one of the quadrature ids */

static int
  read_keyboard(),  read_nothing(), read_mouse(), read_barcode(),
  write_keyboard(), write_nothing();

	/* Notes:
	 *   Only support button boxes with HIL ids of 0x30.  Ignore the
	 *     rest of them because I don't think they exist.
	 */
static HIL_DeviceType devices[] =
{
  {	/* !!!??? */	/* This MUST remain the first device in this list! */
    -1,-1,     "NULL",		NULL_DEVICE,	XOTHER,
    read_nothing,	FALSE, write_nothing,
  },
  {
    0x30,0x30, XI_BUTTONBOX,	BUTTONBOX,	KEYBOARD,
    read_keyboard,	TRUE, write_keyboard,
  },
  {
    0x34,0x34, XI_ID_MODULE,	 ID_MODULE,	KEYBOARD,
    read_nothing,	FALSE, write_nothing,
  },
  {
    0x5c,0x5f, XI_BARCODE,	BARCODE,	KEYBOARD,
    read_barcode,	FALSE, write_nothing,
  },
  {
    0x60,0x60, XI_ONE_KNOB,	ONE_KNOB,	MOUSE,
    read_mouse,		FALSE, write_nothing,
  },
  {
    0x61,0x61, XI_NINE_KNOB,	NINE_KNOB,	MOUSE,
    read_mouse, 	FALSE, write_nothing,
  },
  {
    0x62,0x67, XI_QUADRATURE,	QUADRATURE,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x68,0x6b, XI_MOUSE,	MOUSE,		MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x6c,0x6f, XI_TRACKBALL,	TRACKBALL,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
#ifdef DEBUG
  {
    0x70,0x70, XI_ONE_AXIS,	ONE_AXIS,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x71,0x71, XI_NO_IOB_REL,	NO_IOB_REL,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x72,0x72, XI_MOUSE,	MOUSE,		MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x73,0x73, XI_ERROR_DEVICE,	ERROR_DEVICE,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x74,0x74, XI_NO_AXES,	NO_AXES,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
  {
    0x75,0x75, XI_THREE_AXES,	THREE_AXES,	MOUSE,
    read_mouse,		TRUE, write_nothing,
  },
#endif /* DEBUG */
  {
    0x8c,0x8f, XI_TOUCHSCREEN,	TOUCHSCREEN,	MOUSE,
    read_mouse,		FALSE, write_nothing,
  },
  {
    0x90,0x97, XI_TABLET,	TABLET,		MOUSE,
    read_mouse,		FALSE, write_nothing,
  },
  {
    0xC0,0xDF, XI_KEYBOARD,	KEYBOARD,	KEYBOARD,
    read_keyboard,	FALSE, write_keyboard,
  },
};


	/* HIL Poll Header bits: */
#define KEY_DATA_MASK 		0x70
#define MOTION_MASK 		0x0F	/* !!!??? 0x3??? */
#define MOTION_Y 		0x02	/* Axes Y reporting */
#define MOTION_XY 		0x02	/* Axes X and Y reporting */
#define MOTION_XYZ 		0x03	/* Axes X, Y and Z reporting */

#define KEY_DOWN(key)		((key) & ~1)
#define KEY_UP(key)		((key) | 1)
#define KEY_IS_DOWN(key)	(0 == ((key) & 1))	/* down is even */
#define KEY_IS_UP(key)		!KEY_IS_DOWN(key)

#define BUTTON_BASE 		0x80	/* buttons are keys 0x80 - 0x8D */
#define BUTTON_MAX 		0x8D	/* buttons are keys 0x80 - 0x8D */
#define BUTTON_DOWN(button)	KEY_DOWN(button)	/* same as keys */
#define BUTTON_UP(button)	KEY_UP(button)		/* same as keys */
#define BUTTON_IS_DOWN(button)	KEY_IS_DOWN(button)	/* same as keys */
#define BUTTON_IS_UP(button)	KEY_IS_UP(button)	/* same as keys */
#define NEXT_BUTTON(button)	(BUTTON_DOWN(button) + 2)
#define REDUCE_BUTTON(button)	((button) / 2)
#define EXPAND_BUTTON(button)	((button) * 2)
#define HIL_TO_BUTTON(hil,type,buttons) ((type==TABLET && buttons==4) ? \
    remap[((hil) - BUTTON_BASE)] : ((hil) - BUTTON_BASE))
#define PROXIMITY_IN		0x0E	/* Actually 0x8E */
#define PROXIMITY_OUT		0x0F	/* Actually 0x8F */
#define BUTTON_BIT(button) (1 << REDUCE_BUTTON(button))
#define BUTTON_IS_LATCHED(d,button) (d->kb_latched & (BUTTON_BIT(button)))
#define BUTTON_IS_IGNORED(d,button) (d->kb_ignore & (BUTTON_BIT(button)))
#define IGNORE_BUTTON(d,button) (d->kb_ignore |= (BUTTON_BIT(button)))
#define UNIGNORE_BUTTON(d,button) (d->kb_ignore &= ~(BUTTON_BIT(button)))


#define	HIL_ABSOLUTE	0x40	/* Device has absolute positioning data */
#define HIL_16_BITS 	0x20	/* Device has position data 16 bit accuracy */
#define HIL_IOB		0x10	/* Device has I/O description byte */
#define HIL_NUM_AXES	0x03	/* Number of axes supported */

#define HAS_LEDS	0xf0	/* Device has leds                        */
#define HILIOB_PAA	0x80	/* Device supports prompt and acknowledge */
#define HILIOB_NPA	0x70	/* Number of prompts & acknowledges supported */
#define HILIOB_PIO	0x08	/* Device supports Proximity In/Out */
#define HILIOB_BUTTONS	0x07	/* Number of buttons on device */
#define HP_HIL 		0x40
#define HP_ITF_KBD	0x01
#define PC101_KBD	0x02


/* ******************************************************************** */
/* ****************** Convert HIL ID to Keymap Name ******************* */
/* ******************************************************************** */

    /*
     * Notes:
     *   See the manual on using hp-hil devices with HP-UX for the keyboard
     *     nationality codes; they are the low order 6 bits of the device
     *     id; 0x1f is United States, so we'll subtract from 0x1f to give
     *     the U.S.  a keyId of zero; The PS2 keyboards have hil ids E0-FF.
     *   6 bits == a max of 64 different keyboards.  32 extended and 32 PS2.
     *   George says to use 7 bits:  HIL ids in the range A0-FF.
     *     A0-BF  Compressed keyboard.  Not used (yet).
     *     C0-DF  Extended (ITF) keyboard
     */
static char *hil_id_to_keymap(hil_id, might_be_PS2) int hil_id, might_be_PS2;
{
  static char *base_table[] =
  {
    "31",			/* HIL=00h Undefined keyboard */
    "HIL_JIS",			/* HIL=01h Undefined keyboard */
    "Japanese",			/* HIL=02h */
    "Swiss_French",		/* HIL=03h */
    "29",			/* HIL=04h No keysym support for Portugues */
    "28",			/* HIL=05h No keysym support for Arabic    */
    "27",			/* HIL=06h No keysym support for Hebrew    */
    "Canada_English",		/* HIL=07h */
    "26",			/* HIL=08h No keysym support for Turkish   */
    "25",			/* HIL=09h No keysym support for Greek     */
    "24",			/* HIL=0Ah No keysym support for Thai      */
    "Italian",			/* HIL=0Bh */
    "Korean",			/* HIL=0Ch */
    "Dutch",			/* HIL=0Dh */
    "Swedish",			/* HIL=0Eh */
    "German",			/* HIL=0Fh */
    "S_Chinese",		/* HIL=10h */
    "T_Chinese",		/* HIL=11h */
    "Swiss_French2",		/* HIL=12h */
    "Euro_Spanish",		/* HIL=13h */
    "Swiss_German2",		/* HIL=14h */
    "Belgian",			/* HIL=15h */
    "Finnish",			/* HIL=16h */
    "UK_English",		/* HIL=17h */
    "Canada_French",		/* HIL=18h */
    "Swiss_German",		/* HIL=19h */
    "Norwegian",		/* HIL=1Ah */
    "French",			/* HIL=1Bh */
    "Danish",			/* HIL=1Ch */
    "Katakana",			/* HIL=1Dh */
    "Latin_Spanish",		/* HIL=1Eh */
    "US_English",		/* HIL=1Fh */
  };

  if (hil_id == 0x30) return "HP46086A_Button_Box";

  if (hil_id == 0x5c) return "ITF_US_English";	/* Barcode reader */

  if (hil_id == 0x34) return "ITF_US_English";	/* ID Module      */

  if (0xC0 <= hil_id && hil_id <= 0xDF)		/* Keyboard: 0xC0 - 0xDF */
  {
    static char buf[32];

    if (might_be_PS2) strcpy(buf, "PS2_");
    else	      strcpy(buf, "ITF_");
    strcat(buf, base_table[hil_id & 0x1f]);		/* 0 - 31 */
    return buf;
  }

    /* A device that is not a key device, is unknown or not yet supported
     *   (such as a nonkbd device (like the ID module)) or Standard or
     *   Compressed keyboards.
     */
  return "";
}

/* ******************************************************************** */
/* ************************ General HIL stuff ************************* */
/* ******************************************************************** */

#define MAX_HIL_BUTTONS		7

typedef struct
{
	/* Stuff for all devices */
  HIL_DeviceType *device;
  char
    long_name[50],	/* eg FIRST_KEYBOARD, SECOND_POINTER, etc */
    keymap[30],		/* "" or the keymap name */
    file_name[100];	/* "/dev/hil1", "/tmp/foo", etc  */
  int
    fd,				/* File descriptor */
    hil_id, use_as,
    data_size,			/* DATA_IS_16_BITS, etc		*/
    num_axes,			/* number of axes (motion only)	*/
    error;			/* for device specific error handling */
  unsigned char
    *ptr;			/* for whatever */

	/* Motion device specific stuff */
  int
    res_x, res_y,		/* counts / meter for x and y axis	*/
    max_x, max_y,		/* maximum x and y value  	*/
    latching_enabled, 		/* 0 if not enabled  */
    latching_on,      		/* 0 if not on       */
    chording_interval,		/* 0 if not chording */
	/* Button chording state variables*/
    chorded_button_up,
    ignore_button1, ignore_button2;
  char
    kb_latched,
    kb_ignore,
    ignored_buttons,			/* bit 1 for ignore_button1, etc */
    button_down[MAX_HIL_BUTTONS];	/* TRUE if button n is down */

  unsigned char
    device_exists,		/* TRUE if device exists and is openable */
    flags, 			/* device characteristics	*/
    iob,			/* I/O descriptor Byte 		*/
    num_buttons,		/* count of physical buttons 	*/
    num_keys,			/* number of keys            	*/
    num_leds;			/* number of leds            	*/
} HIL_Device;


    /* 
     * Notes:
     *   The nine knob box has the same HIL id as the quadrature port.
     *   The nine knob box is 3 HIL devices, each with 3 axes of motion.
     */
static int stat_device(ptr, fd) HIL_Device *ptr;
{
  unsigned char describe[32];
  int i, id, num_axes;

  memset(ptr, 0, sizeof(HIL_Device));	/*  */

  if (-1 == ioctl(fd, HILID, describe))  /* hopefully the NULL device */
    return FALSE;

  id = describe[0];
#if 0
  printf("fd is %d errno is %d id is %x\n", fd, errno, id);	/*  */
#endif

  num_axes = (describe[1] & HIL_NUM_AXES);
  if (id == NINE_KNOB_ID && num_axes != 3) id = QUAD_ID;

  for (i = 0; i < NITEMS(devices); i++)
  {
    HIL_DeviceType *device = &devices[i];
    int iob;

    if (id < device->hil_id_low || device->hil_id_high < id) continue;

    ptr->device = device;

    ptr->hil_id = id;
    ptr->flags = describe[1];
    ptr->num_axes = num_axes;

    ptr->data_size = DATA_IS_8_BITS;

    iob = 0;

      /* If # of axes (of motion) indicate it is a positional device then
       *   gather resolution.
       * If 16 bits of information are reported, resolution is in
       *   counts/cm.  In this case, convert to counts/meter.
       */
    if (num_axes)
    {
      int lo_resol = describe[2], hi_resol = describe[3];

      ptr->res_x = ptr->res_y = ((hi_resol << 8) + lo_resol);

      if (ptr->flags & HIL_16_BITS)
      {
	ptr->data_size = DATA_IS_16_BITS;
	ptr->res_x = ptr->res_y = (ptr->res_x * 100);
      }

	    /* If it is an absolute device, gather size */
      if (ptr->flags & HIL_ABSOLUTE)
      {
	switch(num_axes)
	{
	  case 2:
	    ptr->max_y = (int)describe[6] | ((int)describe[7] << 8);
	    /* FALLTHOUGH */
	  case 1:
	    ptr->max_x = (int)describe[4] | ((int)describe[5] << 8);
	}
	iob = describe[4 + 2 * num_axes];
      }
      else
      {
        if (ptr->flags & HIL_IOB) iob = describe[4];
      }
    }
    else		/* Not a motion device */
    {
      if (ptr->flags & HIL_IOB) iob = describe[2];
      ptr->res_x = ptr->res_y = 0;
    }

    ptr->iob = iob;

    if (iob & HILIOB_BUTTONS)
    {
      ptr->num_buttons = (iob & HILIOB_BUTTONS);
    }

    if (iob & HAS_LEDS)
	ptr->num_leds = imax(1, ((iob & HILIOB_NPA) >> 4));

    strcpy(ptr->keymap, hil_id_to_keymap(id, (3 == ptr->num_leds)));

    break;
  }
  return TRUE;
}

static int open_device(name) char *name;
{
  return open(name, O_RDWR | O_NDELAY);
}

static char hil_file_base_name[256] = "/dev/hil";

static int hil_open(n)	/* n in (1,7) */
{
  char name[128];

  sprintf(name, "%s%d", hil_file_base_name, n);
  return open_device(name);
}

	/* hil1 .. hil7 + a bunch of XOTHER devices */
#define MAX_HIL_DEVICES 20
static HIL_Device loop_device_list[MAX_HIL_DEVICES];

static HIL_Device *next_device(reset)
{
  static int z;

  if (reset) { z = 0; return NULL; }
  if (z == MAX_HIL_DEVICES) return NULL;
  return &loop_device_list[z++];
}

    /* Figure out the long name of all the devices on the loop.  This is for
     *   the list devices extension.
     * Format:
     *   nth_name
     *   where:
     *     nth is 0 .. 6 for FIRST, SECOND, ... SEVENTH
     *     name is KEYBOARD, MOUSE, BARCODE, etc
     * Notes:
     *   Do this after catalog_loop().
     */
static void make_name(long_name, nth, x_name) char *long_name, *x_name;
{
  strcat(strcat(strcpy(long_name, h_position[nth]), "_"), x_name);
}

static void name_loop()
{
  int nth, hil, i;

  for (hil = 0; hil < NITEMS(loop_device_list); hil++)
  {
    HIL_Device *ptr = &loop_device_list[hil];

    nth = 0;
    if (ptr->device_exists)
    {
      for (i = 0; i < hil; i++)
	if ( loop_device_list[i].device_exists	&&
	    (loop_device_list[i].device->dev_type == ptr->device->dev_type))
	{
	  nth++;
	}
      make_name(ptr->long_name, nth, ptr->device->X_name);
    }
  }
}

    /* The HIL loop is cataloged every time configure() is called.  This is
     *   because I can't tell if the loop has changed.  This is probably
     *   only a problem for people using the HP Input Extension - somebody
     *   writing a client using the extension might be pluging and unpluging
     *   devices and running their client.  If I didn't catalog, they would
     *   have to restart the X server.  Note that changing one of the core
     *   devices (while X is running) or moving it around the loop is likely
     *   to hose X.
     * If a device is open, assume it is OK (ie don't recatalog it).
     * This should be quick - only have to open(), read(small amount of
     *   data), close() and look at data.
     */
static void catalog_loop()
{
  HIL_Device *ptr;
  int fd, id, hil, i;

  ptr = loop_device_list;
  for (hil = 1; hil <= 7; hil++, ptr++)
  {
    if (ptr->device_exists && ptr->fd != -1) continue;

    fd = hil_open(hil);
    if (fd == -1) continue;		/* Couldn't open that device */

    if (stat_device(ptr, fd))
    {
      sprintf(ptr->file_name, "%s%d", hil_file_base_name, hil);
      ptr->fd = -1;
      ptr->device_exists = TRUE;
    }
    close(fd);
  }
}

    /* 
     * Input:
     *   path: Device file name.  Eg "/dev/hil8"
     *   null_device:  TRUE if this is the null device, ie if path is
     *     "/dev/null" ie "first NULL keyboard"
     *   use_as: XKEYBOARD, XPOINTER or XOTHER
     * Notes:
     *   Always need to stat the device because stat_device() resets things.
     */
static HIL_Device *add_device(path, null_device, use_as) char *path;
{
  HIL_Device *ptr, *qtr;
  int fd, n, device_exists;

  ptr = NULL;
  for (n = 7; n < NITEMS(loop_device_list); n++)
  {
    qtr = &loop_device_list[n];
    device_exists = qtr->device_exists;
    if (!device_exists || (device_exists && qtr->fd == -1))
    {
      ptr = qtr;
      break;
    }
  }

  if (!ptr) return NULL;	/* no open slots */

  if (-1 == (fd = open_device(path))) return NULL;
  if (!stat_device(ptr, fd) && !null_device)
  {
    close(fd);
    return NULL;
  }
  ptr->fd = fd;
  ptr->device_exists = TRUE;
  ptr->use_as |= use_as;
  strcpy(ptr->file_name, path);

  return ptr;
}

static HIL_Device *find_null_device(use_as) /* and open it */
{
  HIL_Device *ptr = &loop_device_list[0];
  int hil, fd;

  for (hil = 0; hil < NITEMS(loop_device_list); hil++, ptr++)
    if (ptr->device_exists && ptr->device->dev_type == NULL_DEVICE)
    {
    ptr->use_as |= use_as;
    if (ptr->fd == -1 && (fd = open_device("/dev/null")) != -1)
      ptr->fd = fd;
    return ptr;
    }
  return NULL;
}

    /* Input:
     *   type:  KEYBOARD, BARCODE, MOUSE, etc
     *   which:  1 ... 7 for FIRST, SECOND ...  Find the nth device of type.
     * Notes:
     *   If which is out of range (ie, bigger than last device of type), the
     *     last device of type is used.
     */
static HIL_Device *find_device_by_type(type, which, use_as, x, in_use) /* and open it */
{
  HIL_Device *savptr = NULL, *ptr = &loop_device_list[0];
  int hil, savhil, fd, find_type;

  for (hil = 0; hil < NITEMS(loop_device_list); hil++, ptr++)
  {
    int count = 0;

    if (!ptr->device_exists)
	continue;
    if (x)
	find_type = ptr->device->x_type;
    else
	find_type = ptr->device->dev_type;

    if (find_type == type) 
    {
      count++;
      if (!in_use && ptr->use_as)
	  continue;
      savptr = ptr;
      savhil = hil + 1;
      if (count == which) break;
    }
  }

	/* Opening the device for the first time? */
  if (savptr && savptr->fd == -1 && (fd = open_device(savptr->file_name)) != -1)
    {
      savptr->fd = fd;
      savptr->use_as = use_as;
      return savptr;
    }

	/* Opening the device for the nth time? */
  if (savptr && savptr->fd != -1)
    {
      savptr->use_as |= use_as;
      return savptr;
    }

  return NULL;
}

static HIL_Device *use_device_n(n, use_as)	/* n in [1,7] */
{
  HIL_Device *ptr = &loop_device_list[n - 1];
  int fd;

  if (ptr->device_exists && ptr->fd == -1 && (fd = hil_open(n)) != -1)
  {
    ptr->fd = fd;
    ptr->use_as |= use_as;
    return ptr;
  }
  return NULL;
}

static HIL_Device *find_device_by_fd(fd)
{
  HIL_Device *ptr = loop_device_list;
  int hil;

  for (hil = NITEMS(loop_device_list); hil--; ptr++)
    if (ptr->fd == fd) return ptr;

  return NULL;
}

static HIL_Device *find_device_by_use(use_as)
{
  HIL_Device *ptr = loop_device_list;
  int hil;

  for (hil = NITEMS(loop_device_list); hil--; ptr++)
    if (ptr->use_as & use_as) return ptr;

  return NULL;
}

/**************************************************************************
 *
 * This routine is called by the X server to close an input device.
 *
 */
static int close_device(fd) int fd;
{
  HIL_Device *ptr;
  int i;

  if (!(ptr = find_device_by_fd(fd)))	/* !!! I hope this never happens! */
	return CLOSE_SUCCESS;

  close(fd);
  ptr->fd = -1;

  return CLOSE_SUCCESS;
}

/* ******************************************************************** */
/* ************************* Read HIL Devices ************************* */
/* ******************************************************************** */

typedef struct
{
  unsigned char	len;
  unsigned char	timestamp[4];
  unsigned char	poll_hdr;
  unsigned char	data[20];
} HilPacket;

#define MAX_PACKET_SIZE		sizeof(HilPacket)	/* bytes */

#define BUFFER_SIZE	600	/* Must be bigger than a (single) HIL packet */
#define MAX_RETRIES	10

static unsigned char hil_buffer[BUFFER_SIZE];
static int data_start, bytes_left;

static int read_hil(fd, force_read)
{
  if (fd == -1) return bytes_left;		/* buffer check */
  if (!force_read && bytes_left) return TRUE;	/* got data in buffer */

	/* buffer empty or not enough data in buffer */
  {
    int n;

    if (bytes_left == 0) data_start = 0;

    n = read(fd, hil_buffer + data_start + bytes_left,
		 BUFFER_SIZE - bytes_left);
    /* !!! error check */
    bytes_left += n;
  }

  return bytes_left;
}

    /* Read (at least) enough data to fill a packet.
     * Returns:
     *   0 : No can do.
     *   n : packet size
     */
static int read_packet(fd)
{
  int packet_size;

  read_hil(fd,FALSE);
  if (bytes_left == 0) return 0;	/* no packet available */

  packet_size = hil_buffer[data_start];
/*  if (packet_size > MAX_PACKET_SIZE) ???
/* error check size!!! */
  if (packet_size <= bytes_left) return packet_size;

  if (fd == -1) return 0;

	/* Don't have a complete packet */
  {
    int i;

    if (BUFFER_SIZE < (data_start + packet_size))
    {	/* packet won't fit in buffer */
      memcpy(hil_buffer, hil_buffer + data_start, bytes_left);
      data_start = 0;
    }
    for (i = MAX_RETRIES; i--; )
    {
      if (!read_hil(fd, TRUE)) return 0;	/* !!!??? */
      if (packet_size <= bytes_left)   /* got (at least) the complete packet */
	return packet_size;
  /* !!! sleep??? */
    }
  }
    /* !!! big bad errors! */
  return 0;
}

static HilPacket a_packet;
static int packet_waiting = FALSE;

static void pop_packet() { packet_waiting = FALSE; }

static void *get_packet(fd)
{
  if (!packet_waiting)
  {
    int packet_size;

    if (!(packet_size = read_packet(fd))) return NULL;
/* !!! ick - two data copies */
    memcpy(&a_packet, hil_buffer + data_start, packet_size);
    data_start += packet_size; bytes_left -= packet_size;
    packet_waiting = TRUE;
  }
  return &a_packet;
}

    /* Look though the buffered HIL packets looking for a button down.  If a
     *   button down is found, all the packets before it are thrown away.
     * This is used for button chording.  Motion devices can send different
     *   kinds of events in a single packet and there can be many motion
     *   events between button presses (even if the user is trying to hold
     *   still) - the tablet is especially bad in this reguard.
     * Notes:
     *   This routine has its fingers in too many things.  It has to know
     *     too much about HIL packets.
     * Input:
     *   device : The motion device that has buffered data.
     *   b1, b2:  Buttons to look for.
     *   button : Pointer to int to be filled in if a button is found.
     * Output:
     *   button
     *   The current packet is the button if TRUE returned, else unknown.
     * Returns:
     *   TRUE : If a button down is found.
     */
static int look_for_button(device, b1, b2, button)
  HIL_Device *device;
  int b1, b2, *button;
{
  int poll_hdr, b, z, n, save_data_start, save_bytes_left;

  save_data_start = data_start;
  save_bytes_left = bytes_left;

  n = 1 + (device->data_size == DATA_IS_16_BITS);

  while (TRUE)
  {
    if (!get_packet(-1)) break;		/* incomplete packet */
    pop_packet();

    poll_hdr = a_packet.poll_hdr;
    if (0 == (poll_hdr & KEY_DATA_MASK)) continue;	/* not a button */

    z = 0;
    if (poll_hdr & MOTION_MASK)		/* motion AND key data in one packet */
    {
      if (poll_hdr  & MOTION_XYZ)		 z += n;	/* X axes */
      if (poll_hdr  & MOTION_Y)			 z += n;	/* Y axes */
      if ((poll_hdr & MOTION_XYZ) == MOTION_XYZ) z += n;	/* Z axes */
    }

    b = HIL_TO_BUTTON(a_packet.data[z], device->device->dev_type,
	device->num_buttons);
    if (BUTTON_IS_UP(b)) break;
    if (BUTTON_MAX < a_packet.data[z])  break;		/* probably proximity */

    b = REDUCE_BUTTON(b);
#if 1
    *button = b;
    if (b == b1 || b == b2)
      return TRUE;
#else
    if (b == b1 || b == b2)
    {
      *button = b;
      return TRUE;
    }
#endif

    break;			/* wrong button, bail */
  }

	/* No button down in buffer, restore buffer */
  data_start = save_data_start;
  bytes_left = save_bytes_left;

  return FALSE;
}


/* ******************************************************************** */
/* ****************** General HIL Turn Things On/Off ****************** */
/* ******************************************************************** */

    /* Turn on or off LEDs.
     * Input:
     *   fd :  File descriptor of the HIL device.
     *   led:  LED to turn on or off:
     *         0 : general prompt.
     *         1 : LED 1.
     *         etc
     *   on : TRUE if turn on the LED.
     */
  int
    led_on[]  = { HILP, HILP1, HILP2, HILP3, HILP4, HILP5, HILP6, HILP7 },
    led_off[] = { HILA, HILA1, HILA2, HILA3, HILA4, HILA5, HILA6, HILA7 };
static void set_LED(fd, led, on)
{

  if (0 <= led && led <= 7)
    if (on) ioctl(fd, led_on [led], (char *)NULL);
    else    ioctl(fd, led_off[led], (char *)NULL);
}

    /* HILER1 == Key repeat every 1/30 sec
     * HILER2 == Key repeat every 1/60 sec
     */
static void set_autorepeat(fd, mode)
{
  switch (mode)
  {
    case AutoRepeatModeOff:
      ioctl(fd, HILDKR, (char *)NULL);
      break;
    case AutoRepeatModeOn:
    case AutoRepeatModeDefault:
      ioctl(fd, HILER2, (char *)NULL);
      break;
    default:
      ioctl(fd, HILER1, (char *)NULL);
      break;
  }
}

/* ******************************************************************** */
/* ************************* Button Chording ************************** */
/* ******************************************************************** */

/* Button chording can allow a N button device generate 2N-1 buttons.
 * Buttons are numbered 0,1, ...
 * Simultaneously pressing buttons A and B (where A in [1,N) and B == A+1),
 *   generates button N+A.
 * Only a total of 5 buttons are supported.  This is because of motion
 *   events and other things that George knows more about.
 * If the device has 4 buttons, only buttons 0 and 1 can chord.
 * If the device has 5 or more buttons, no chording is possible.  Because of
 *   the max number of buttons.
 * Only chord adjacent buttons ie don't chord buttons 0 and 3.  My guess is
 *   this is historical and with 5 buttons, you don't need all those
 *   chordings.
 * Algorithm:
 *   if num_buttons > 4, no chording, done.
 *   if num_buttons <= 3, Z = [0,2]
 *   if num_buttons == 4, Z = [0,1]
 *   Button in Z is pressed.  X = button.
 *   Wait E seconds (E is passed in).  Don't just wait for the device to
 *     send data within E seconds.  This is in case the device sends (for
 *     example) motion data before the other button.
 *   Look though the data to see if there is a button (Y):
 *     No:  Send button X.
 *     Yes:
 *       If new button (Y) is (X+1 or X-1) and Y != 3:
 *         Yes:
 *           Send button (N - 1) + (X + Y + 1)/2.
 *              (N - 1) because buttons start at zero.
 *           Discard the motion events between the buttons.
 *         No:
 *           Send X, and process the other data "normally".
 * Notes:
 *   The hard part is keeping track of the ups and downs and keeping things
 *     in sync.
 *   If the chording interval is long, it is possible for the buffer in the
 *     HIL device (or kernel) to overflow while waiting.  Too bad.
 */

#define MAX_X_BUTTONS			5
#define MAX_BUTTONS_FOR_CHORDING	4

    /* Process a button, chording if chording turned on.
     * Input:
     *   button : reduced button.
     * Returns:
     *   TRUE  : Button chorded
     */
static int chord_button(device, button,
			chorded_button, ignore_button1, ignore_button2)
  HIL_Device *device;
  int *chorded_button, *ignore_button1, *ignore_button2;
{
  int x,y,ret, max_b;

  if (!device->chording_interval) return FALSE;

  max_b = 3;
  if (3 < device->num_buttons) max_b = 1;

  x = REDUCE_BUTTON(button);
  if (
    BUTTON_IS_DOWN(button)			&&
    device->chording_interval			&&
    0 <= x && x <= max_b)
  {
    int x1;
    struct timeval timeout;

    timeout.tv_sec = 0;
    timeout.tv_usec = device->chording_interval * 1000;
    select(0, (int *)NULL, (int *)NULL, (int *)NULL, &timeout);

	/* No response in allotted time */
    if (!read_hil(device->fd, FALSE)) return 0;

    if (3 == (x1 = x + 1)) x1 = 100;
	/* No button => not a chording combination */
    ret = look_for_button(device, x - 1, x1, &y);
#if 1
    if (device->latching_enabled)
    {
	if (!ret && 
		(x == 0 && look_for_button(device, 100,2,&y)) ||
		(x == 2 && look_for_button(device, 100,0,&y)))
	{
	    device->latching_on = ~device->latching_on;
	    if (device->latching_on)
		device->kb_latched = 0xff;
	    else
		device->kb_latched = 0;
            *chorded_button = -1;
    	    pop_packet();
    	    *ignore_button1 = BUTTON_UP(EXPAND_BUTTON(x));
    	    *ignore_button2 = BUTTON_UP(EXPAND_BUTTON(y));
	    return TRUE;
	}
    }

    if (!ret) return FALSE;
#else
    if (!ret) return FALSE;
#endif
    *chorded_button = EXPAND_BUTTON(device->num_buttons - 1 + (x + y + 1)/2);
    pop_packet();
    *ignore_button1 = BUTTON_UP(EXPAND_BUTTON(x));
    *ignore_button2 = BUTTON_UP(EXPAND_BUTTON(y));
    return TRUE;
  }
  return FALSE;		/* Not a chording button */
}

/* ******************************************************************** */
/* **************************** Read Mouse **************************** */
/* ******************************************************************** */

static int find_button_down(), ignore_check(), error_check_button();
static void set_button_down();

static int get_data(data, two_bytes) unsigned char *data;
{
  int z;

  z = data[0];
  if (two_bytes) z |= (data[1] << 8);

  return z;
}

static void put_data(z, data, two_bytes) unsigned char *data;
{
  data[0] = z;			/* the lower byte */
  if (two_bytes) data[1] = (z >> 8);
}

#define APPEND_DATA(type, z)			\
  *data_type |= (type);				\
  put_data((z), &data[num_bytes], two_bytes);	\
  num_bytes += step;				\
  *pending = num_bytes;


    /* Read from a motion device:  motion and button presses.
     * Mouse, Nine Knob box, Tablet.
     * Handles absolute devices, 1 and 2 byte data.
     * Notes:
     *   Mice need to have the Y axes motion data negated so the sprite
     *     moves when the mouse moves up.  Tablets and the nine knob box
     *     don't.
     *   I really hate having to keep so much state information for chording
     *     but I think I have to:  If you chord two buttons and then
     *     repeatedly press one while keeping the other pressed, you need a
     *     lot of state info to keep track.
     *   Can only chord one button pair at a time.  This means that holding
     *     down the middle button, then pressing the buttons to the right
     *     and left of it (while holding down the middle button) will only
     *     chord one of the pairs.
     *   Motion devices can generate a LOT of motion data quickly.  For
     *     example, if the X server takes a long time to render something
     *     (like a wide dashed line) and you are moving the mouse around,
     *     the HIL buffer (in the kernel) can overflow and data will be
     *     lost.  This is more-or-less OK (can't do anything about it)
     *     except in the case where a button was down (before the overflow)
     *     and then went up after the overflow.  In this case, a button up
     *     will never be generated.  To check for this, when there is a
     *     button down, check to see if the button is already down and if it
     *     is, generate ups for all down buttons.  The server sends
     *     extraneous ups to clients but I hope that doesn't cause problems
     *     because it is quite painful to track chording in this case.
     */
static int read_mouse(mouse, data, data_type, pending)
  HIL_Device *mouse;
  unsigned char *data, *data_type;
  int *pending;
{
  int poll_hdr, num_bytes, two_bytes, step;

  if (!get_packet(mouse->fd))
  {
    *pending = 0;
    return READ_FAILURE;
  }

  two_bytes = (mouse->data_size == DATA_IS_16_BITS);
  step = 1 + two_bytes;
  num_bytes = 0;

  if (mouse->error)	/* Buffer overflow, sending button ups */
  {
    int button;

    button = find_button_down(mouse, -1);
    if (button == -1)		/* No downs left */
    {
      mouse->error = FALSE;

      *pending = 0;
      return READ_SUCCESS;
    }

    set_button_down(mouse, button, FALSE);
    button = BUTTON_UP(button);
    APPEND_DATA(BUTTON_DATA, button);
    return READ_SUCCESS;
  }

  poll_hdr = a_packet.poll_hdr;

  if (poll_hdr & MOTION_MASK)		/* mouse movement */
  {
    int motion;

    *data_type |= MOTION_DATA;

    if (poll_hdr & MOTION_XYZ)			/* X axes */
    {
      motion = get_data(&a_packet.data[num_bytes], two_bytes);
      put_data(motion, &data[num_bytes], two_bytes);
      num_bytes += step;
    }
    if (poll_hdr & MOTION_Y)			/* Y axes */
    {
      motion = get_data(&a_packet.data[num_bytes], two_bytes);

      if (mouse->device->reverse_y_axes) motion = -motion;

      put_data(motion, &data[num_bytes], two_bytes);
      num_bytes += step;
    }
    if ((poll_hdr & MOTION_XYZ) == MOTION_XYZ)	/* Z axes */
    {
      motion = get_data(&a_packet.data[num_bytes], two_bytes);
      put_data(motion, &data[num_bytes], two_bytes);
      num_bytes += step;
    }
  }

  if (poll_hdr & KEY_DATA_MASK)		/* button press */
  {
    int button;

    pop_packet();

    button = HIL_TO_BUTTON(a_packet.data[num_bytes], mouse->device->dev_type,
	mouse->num_buttons);

    if (button == PROXIMITY_IN || button == PROXIMITY_OUT)
    {
      *data_type |= PROXIMITY_DATA;

      put_data(
	((button == PROXIMITY_IN) ? IN_PROXIMITY : OUT_OF_PROXIMITY),
	&data[num_bytes], two_bytes);
      num_bytes += step;
    }
    else		/* Must? be a "regular" button */
    {
      if (1) /*!error_check_button(mouse, button))*/
      {
	if (mouse->chorded_button_up)	/* Button chord in progress */
	{
	  if (BUTTON_IS_UP(button))
	  {
	    button = ignore_check(mouse, button, mouse->ignore_button1, 0x1);
	    button = ignore_check(mouse, button, mouse->ignore_button2, 0x2);
	    if (0x3 == mouse->ignored_buttons)
	    {
	      button = mouse->chorded_button_up;
	      mouse->chorded_button_up = 0;	/* turn off chord */
	    }
	  }
	}
	else				/* not chording */
	  if (chord_button(mouse, button, &button,
			   &mouse->ignore_button1, &mouse->ignore_button2))
	  {
	    if (button == -1)
	    {
	        mouse->chorded_button_up = -1;
		num_bytes = 0;
	    }
	    else
	        mouse->chorded_button_up = BUTTON_UP(button);
	    mouse->ignored_buttons = 0;
	    set_button_down(mouse, mouse->ignore_button1, TRUE);
	    set_button_down(mouse, mouse->ignore_button2, TRUE);
	  }

	  if (mouse->num_buttons==2 && button>=0 && button <=15)
	      button = remap[button];
	      
	  if (button != -1 && !BUTTON_IS_IGNORED(mouse, button))
	  {
	    APPEND_DATA(BUTTON_DATA, button);
	    set_button_down(mouse, button, BUTTON_IS_DOWN(button));
	  }
      }

    if (button != -1)
    {
      if (BUTTON_IS_LATCHED(mouse, button)) 
        if (BUTTON_IS_IGNORED(mouse, button))
        {
	  if (BUTTON_IS_DOWN(button))
	    UNIGNORE_BUTTON(mouse, button);
        }
        else if (BUTTON_IS_DOWN(button))
	  IGNORE_BUTTON(mouse, button);
    }
    }
  }

  pop_packet();

  *pending = num_bytes;  /* might be zero in case of chording or error */
  return READ_SUCCESS;
}

static int ignore_check(mouse, button, ignore_button, ignored_button_bit)
  HIL_Device *mouse;
  int button, ignore_button, ignored_button_bit;
{
  if (button == ignore_button)
  {
    if (0 == (mouse->ignored_buttons & ignored_button_bit))
    {
      mouse->ignored_buttons |= ignored_button_bit;
      set_button_down(mouse, button, FALSE);
      return -1;		/* ignore this button */
    }
  }
  return button;
}

static void set_button_down(mouse, button, state) HIL_Device *mouse;
{
  mouse->button_down[REDUCE_BUTTON(button)] = state;
}

static int find_button_down(mouse, button) HIL_Device *mouse;
{
  int b, z;

  if (-1 != button)
  {
    b = REDUCE_BUTTON(button);
    z = mouse->button_down[b];
  }
  else
    for (b = 0; b < MAX_HIL_BUTTONS; b++)
      if (z = mouse->button_down[b]) break;

  if (z) return EXPAND_BUTTON(b);

  return -1;
}

    /* Notes:
     *   If there is an error (because there is a down for a button that is
     *     already down), turn off that button so I don't send two ups (got
     *     a down so will get an up).
     *   Turn off chording.
     *   Trying not to send extraneous ups when chording is just too painful
     *     to worry about.  The client will just have to live with extra
     *     ups.
     */
static int error_check_button(mouse, button) HIL_Device *mouse;
{
  if (BUTTON_IS_DOWN(button))	/* error check */
  {
    if (-1 != find_button_down(mouse,button))
    {
      mouse->error = TRUE;
      mouse->chorded_button_up = 0;		/* turn off chording */
      set_button_down(mouse, button, FALSE);
      return TRUE;
    }
  }
  return FALSE;		/* no problem */
}

/* ******************************************************************** */
/* ***************************** Nothing ****************************** */
/* ******************************************************************** */

static int read_nothing(device, data, data_type, pending)
  HIL_Device *device;
  unsigned char *data, *data_type;
  int *pending;
{
  *pending = 0;
  return READ_FAILURE;
}

static int write_nothing(device, request, data)
  HIL_Device *device;
  int request;
  char *data;
{
  return WRITE_FAILURE;
}

/* ******************************************************************** */
/* ************************** Read Keyboard *************************** */
/* ******************************************************************** */

#define REPEAT_CURSOR		0x02

#define ARROW_LEFT		0xf8
#define ARROW_DOWN		0xfa
#define ARROW_UP		0xfc
#define ARROW_RIGHT		0xfe

static int last_key;		/* for arrow auto repeat */

static int read_keyboard(keyboard, data, data_type, pending)
  HIL_Device *keyboard;
  unsigned char *data, *data_type;
  int *pending;
{
  if (get_packet(keyboard->fd))
  {
    int poll_hdr = a_packet.poll_hdr;

    pop_packet();

    if (poll_hdr & KEY_DATA_MASK)	/* button press */
    {
      int key;

      key = a_packet.data[0];

      if (key == REPEAT_CURSOR) key = last_key;
      if (KEY_IS_DOWN(key)) last_key = key;

      *pending = 1;
      *data_type = KEY_DATA;
      *data = key;

      return READ_SUCCESS;
    }
  }

  *pending = 0;
  return READ_FAILURE;
}

/* ******************************************************************** */
/* *************************** Bell/Beeper **************************** */
/* ******************************************************************** */

static unsigned char bell_data[4];

static int bell_percent, bell_pitch, bell_duration, beeper_fd = -1;

#define BEEPER_DEVICE   "/dev/rhil"

static void close_beeper()
{
  if (beeper_fd != -1) close(beeper_fd);
  beeper_fd = -1;
}

static void open_beeper()
{
  if (beeper_fd > 0)
      close_beeper();
  if ((beeper_fd = open(BEEPER_DEVICE,O_RDWR)) < 0)
    ; /* Don't complain, because we might be on a non-HIL machine */
      /* and be using this driver to support NULL input devices   */
#if 0
    fprintf(stderr, "Unable to open beeper device \"%s\".\n",BEEPER_DEVICE);
#endif
}

/*
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; File:         beeper.c
; SCCS:         %A% %G% %U%
; Description:  Access Gator/Bobcat beeper
; Author:       Andreas Paepcke, HPLabs/ATL
; Created:      2-Aug-85
; Modified:     Thu Oct 15 12:53:00 1987 (Don Bennett) bennett@hpldpb
;		George and Craig
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
*/

/*
   We offer three voices and a noise source.  Each sound is controllable
   in pitch, volume and duration.  Pitch goes from 0 to 1023, volume
   goes from 0 to 15, duration is between 0 and 255 10msec intervals.  A
   duration of 0 turns the voice on continuously.  A volume of 0 turns
   it off.

   The manufacturing specs give details on the programming interface.
   Here is a summary:

   The beeper is accessed through ioctl calls.  The request argument is
   either "Send data to beeper" or "Read voice values from beeper".  The
   argument is a pointer to a 4 byte buffer.  These four bytes are
   defined here.

   R0-R3: Register address field. In the order R2, R1, R0:
     
     0 0 0: Voice 1 frequency
     0 0 1: Voice 1 attenuation
     0 1 0: Voice 2 frequency
     0 1 1: Voice 2 attenuation
     1 0 0: Voice 3 frequency
     1 0 1: Voice 3 attenuation
     1 1 0: Noise control
     1 1 1: Noise attentuation

  F0-F9: 10 bits pitch
  A0-A3: Attenuation
  D0-D7: Duration in 10msec's

  The placement of data in the buffer is a bit screwy:

  Byte 0 (Frequency 1):  1 R2 R1 R0 F3 F2 F1 F0     LSB
  Byte 1 (Frequency 2):  0  0 F9 F8 F7 F6 F5 F4
  Byte 2 (Attenuator) :  1 R2 R1 R0 A3 A2 A1 A0
  Byte 3 (Duration)   : D7 D6 D5 D4 D3 D2 D1 D0

  The volume is inversely proportional to the attenuation.  In order to
  provide rising numbers for rising loudness to the user, we expect a
  volume and modify to get the attenuation.  The same goes for the
  pitch.  In order to calculate frequency of the pitch, use:

           83333/(1023-pitch)

  It is possible at any time to request the time any voice has left to
  run.  This is done by:
 
  F4: Read voice1 timer
  F5: Read voice2 timer
  F6: Read voice3 timer
  F7: Read voice4 timer (noise)

  Noise is generated using a shift register.  The following controls are
    possible for noise:
 
  - Attenuation
  - Duration
  - Periodic or white noise
  - 3 shift rates or output of voice 4 as shift rate

  Bytes 0 and 1 of the data buffer must both have identical contents to
  control the noise.  Attenuation and duration are as in the other
  voices.  Bytes 0 and 1 should look like this:

  1 R2 R1 R0 0 FB NF1 NF0   LSB

  R2, R1 and R0 must be 1, 1 and 0.  If FB is 0, periodic noise is
  generated.  If FB is 1, white noise is produced.

  NF1 and NF2 control the shift rate of the noise generator:

  NF1     NF2     Shift Rate
  --------------------------
  0       0       M/64
  0       1       M/128
  1       0       M/256
  1       1       Uses tone generator 3 output

  M is related to the clock rate.

  The voice start routines return 0 if all is well, -1 if we had trouble
  accessing the device file for the beeper and -2 if given parameters
  were out of range.
*/

#define ALL_OK           0
#define ACCESS_PROBLEM  -1
#define BAD_RANGE       -2

#define VOICE1_FREQ_REG 0x80	/* Top nibbles for byte0 for all voices: */
#define VOICE2_FREQ_REG 0xA0
#define VOICE3_FREQ_REG 0xC0
#define NOISE_FREQ_REG  0xE0

#define VOICE1_VOL_REG  0x90	/* Top nibbles for byte2 for all voices: */
#define VOICE2_VOL_REG  0xB0
#define VOICE3_VOL_REG  0xD0
#define NOISE_VOL_REG   0xF0

#define MIN_VOICE	   1	/* Legal ranges for parms from user: */
#define MAX_VOICE	   3
#define MIN_PITCH	   0
#define MAX_PITCH	1023
#define MIN_DURATION	   0
#define MAX_DURATION	 255
#define MIN_VOLUME	   0
#define MAX_VOLUME	  15
#define MIN_TYPE	   0
#define MAX_TYPE	   1
#define MIN_RATE	   0
#define MAX_RATE	   3

static void set_bell_attributes(volume, pitch, duration)
{
  int attenuation, loud;

	/* Map input range of 0 - 100 to hardware attenuation range of 15 -
	 * 0 we use a temp variable (loud) because of C's penchant for
	 * strange orders of evaluation ie to force the multiply before the
	 * divide.
	 */
  volume = imin(volume,100);
  loud = volume * MAX_VOLUME;
  attenuation = (char) MAX_VOLUME - loud / 100;

  duration = duration / 10;
  pitch = (pitch == 0) ? 1023 : (83333 / pitch);
  pitch = imax(imin(pitch, 1023), 0);

  bell_data[0] = (VOICE1_FREQ_REG | (pitch & 0x0f));
  bell_data[1] = (0x03f & (pitch >> 4));
  bell_data[2] = (VOICE1_VOL_REG | attenuation);
  bell_data[3] = imax(duration, 1);
}


    /* We formerly used all three voices to get a reasonably loud volume.
     * However, s700 audio hardware support tries to emulate the EFTSBP
     * ioctl, and has only one voice.  The result is that distinct beeps
     * (usually two) are heard if the duration is 100ms or less.  To avoid
     * this, we will use only one voice.
     */
static void ding(click, percent)
{
  int pitch;
  unsigned char buffer[4];

  if (beeper_fd < 0) return;
  if (!click)
  {
    ioctl(beeper_fd, EFTSBP, bell_data);
    return;
  }

  click = (int)((double)(percent) * 15.0 / 100.0);

  click = MAX_VOLUME - click;
  pitch  = MAX_PITCH  - 800;
  buffer[0] = VOICE2_FREQ_REG | (pitch & 0x0000000f);
  buffer[2] = VOICE2_VOL_REG  | (percent & 0x0000000f);
	/* The high 6 bits of the pitch go into byte 1: */
  buffer[1] = 0x0000003f & (pitch >> 4);
  buffer[3] = 1;
  ioctl(beeper_fd,EFTSBP,buffer);
  return;
}

/* ******************************************************************** */
/* ************************** Write Keyboard ************************** */
/* ******************************************************************** */

/* !!!!!!!!!!!!!!!
 * keyclick - in server or here?
 * if key autorepeats but autorepeat is turned off for that key, don't click
 *   ie down == last key && autorepeat on for key && keyclick on => click
 * only click for downs
 */

static int write_keyboard(keyboard,request,data)
  HIL_Device *keyboard;
  int request;
  char *data;
{
  int fd = keyboard->fd;

/*fprintf(stderr,"write keyboard, request is %d\n",request);		/*  */
  switch (request)
  {
    case _XChangeFeedbackControl:
    {
      HPKeyboardFeedbackControl *kctrl = (HPKeyboardFeedbackControl *)data;

      if (kctrl->class == KbdFeedbackClass)
      {
	int mask = kctrl->leds;

	if (keyboard->device->reverse_y_axes)	/* its a button box */
	{
	  set_LED(fd, 0, mask & 1);
	}
	else		/* a keyboard */
	{
	  set_LED(fd, 1, mask & SCROLLLOCK_LED);
	  set_LED(fd, 2, mask & NUMLOCK_LED);
	  set_LED(fd, 3, mask & CAPSLOCK_LED);
	}

	set_autorepeat(fd, kctrl->autoRepeat);

	set_bell_attributes(
	  kctrl->bell_percent, kctrl->bell_pitch, kctrl->bell_duration);

	return WRITE_SUCCESS;
      }
      break;
    }
    case _XBell:
    {
      HPKeyboardFeedbackControl *kctrl = (HPKeyboardFeedbackControl *)data;

      if (kctrl->click)
        ding(TRUE, kctrl->click);
      else
        ding(FALSE, kctrl->bell_percent);
      return WRITE_SUCCESS;
    }
    case _XSetDeviceValuators:
    case _XChangeDeviceControl:
    case _XSetDeviceMode:
    default:
      break;
  }
  return WRITE_FAILURE;
}

/* ******************************************************************** */
/* ************************** BarCode Reader ************************** */
/* ******************************************************************** */

static unsigned char ascii_to_code[128][7] =
{
  {0x0c,0x0a,0x7a,0x7b,0x0b,0x0d,0},	/* 0x00 : cntl - @ */
  {0x0c,0x5a,0x5b,0x0d,0,0,0},		/* 0x01 : cntl - a */
  {0x0c,0x30,0x31,0x0d,0,0,0},		/* 0x02 : cntl - b */
  {0x0c,0x34,0x35,0x0d,0,0,0},		/* 0x03 : cntl - c */
  {0x0c,0x56,0x57,0x0d,0,0,0},		/* 0x04 : cntl - d */
  {0x0c,0x68,0x69,0x0d,0,0,0},		/* 0x05 : cntl - e */
  {0x0c,0x54,0x55,0x0d,0,0,0},		/* 0x06 :  cntl - f */
  {0x0c,0x52,0x53,0x0d,0,0,0},		/* 0x07 :  cntl - g */
  {0x0c,0x50,0x51,0x0d,0,0,0},		/* 0x08 :  cntl - h */
  {0x0c,0xc0,0xc1,0x0d,0,0,0},		/* 0x09 :  cntl - i */
  {0x0c,0xd0,0xd1,0x0d,0,0,0},		/* 0x0a :  cntl - j */
  {0x0c,0xd2,0xd3,0x0d,0,0,0},		/* 0x0b :  cntl - k */
  {0x0c,0xd4,0xd5,0x0d,0,0,0},		/* 0x0c :  cntl - l */
  {0x0c,0xe0,0xe1,0x0d,0,0,0},		/* 0x0d :  cntl - m */
  {0x0c,0xf0,0xf1,0x0d,0,0,0},		/* 0x0e :  cntl - n */
  {0x0c,0xc2,0xc3,0x0d,0,0,0},		/* 0x0f :  cntl - o */
  {0x0c,0xc4,0xc5,0x0d,0,0,0},		/* 0x10 :  cntl - p */
  {0x0c,0x6c,0x6d,0x0d,0,0,0},		/* 0x11 :  cntl - q */
  {0x0c,0x66,0x67,0x0d,0,0,0},		/* 0x12 :  cntl - r */
  {0x0c,0x58,0x59,0x0d,0,0,0},		/* 0x13 :  cntl - s */
  {0x0c,0x64,0x65,0x0d,0,0,0},		/* 0x14 :  cntl - t */
  {0x0c,0x60,0x61,0x0d,0,0,0},		/* 0x15 :  cntl - u */
  {0x0c,0x32,0x33,0x0d,0,0,0},		/* 0x16 :  cntl - v */
  {0x0c,0x6a,0x6b,0x0d,0,0,0},		/* 0x17 :  cntl - w */
  {0x0c,0x36,0x37,0x0d,0,0,0},		/* 0x18 :  cntl - x */
  {0x0c,0x62,0x63,0x0d,0,0,0},		/* 0x19 :  cntl - y */
  {0x0c,0x38,0x39,0x0d,0,0,0},		/* 0x1a :  cntl - z */
  {0x0c,0xc6,0xc7,0x0d,0,0,0},		/* 0x1b :  cntl - [ */
  {0x0c,0xca,0xcb,0x0d,0,0,0},		/* 0x1c :  cntl - \ */
  {0x0c,0xc8,0xc9,0x0d,0,0,0},		/* 0x1d :  cntl - ] */
  {0x0c,0x0a,0x72,0x73,0x0b,0x0d,0},	/* 0x1e :  cntl - ^ */
  {0x0c,0x0a,0xb6,0xb7,0x0b,0x0d,0},	/* 0x1f :  cntl - _ */
  {0xf2,0xf3,0,0,0,0,0},        	/* 0x20 :  space    */
  {0x0a,0x7c,0x7d,0x0b,0,0,0},		/* 0x21 :  !        */
  {0x0a,0xd8,0xd9,0x0b,0,0,0},		/* 0x22 :  "        */
  {0x0a,0x78,0x79,0x0b,0,0,0},		/* 0x23 :  #        */
  {0x0a,0x76,0x77,0x0b,0,0,0},		/* 0x24 :  $        */
  {0x0a,0x74,0x75,0x0b,0,0,0},		/* 0x25 :  %        */
  {0x0a,0x70,0x71,0x0b,0,0,0},		/* 0x26 :  &        */
  {0xd8,0xd9,0,0,0,0,0},        	/* 0x27 :  '        */
  {0x0a,0xb2,0xb3,0x0b,0,0,0},		/* 0x28 :  (        */
  {0x0a,0xb4,0xb5,0x0b,0,0,0},		/* 0x29 :  )        */
  {0x0a,0xb0,0xb1,0x0b,0,0,0},		/* 0x2a :  *        */
  {0x0a,0xb8,0xb9,0x0b,0,0,0},		/* 0x2b :  +        */
  {0xe2,0xe3,0,0,0,0,0},        	/* 0x2c :  ,        */
  {0xb6,0xb7,0,0,0,0,0},        	/* 0x2d :  -        */
  {0xe4,0xe5,0,0,0,0,0},        	/* 0x2e :  .        */
  {0xe6,0xe7,0,0,0,0,0},        	/* 0x2f :  /        */
  {0xb4,0xb5,0,0,0,0,0},        	/* 0x30 :  0        */
  {0x7c,0x7d,0,0,0,0,0},        	/* 0x31 :  1        */
  {0x7a,0x7b,0,0,0,0,0},        	/* 0x32 :  2        */
  {0x78,0x79,0,0,0,0,0},        	/* 0x33 :  3        */
  {0x76,0x77,0,0,0,0,0},        	/* 0x34 :  4        */
  {0x74,0x75,0,0,0,0,0},        	/* 0x35 :  5        */
  {0x72,0x73,0,0,0,0,0},        	/* 0x36 :  6        */
  {0x70,0x71,0,0,0,0,0},        	/* 0x37 :  7        */
  {0xb0,0xb1,0,0,0,0,0},        	/* 0x38 :  8        */
  {0xb2,0xb3,0,0,0,0,0},        	/* 0x39 :  9        */
  {0x0a,0xd6,0xd7,0x0b,0,0,0},		/* 0x3a :  :        */
  {0xd6,0xd7,0,0,0,0,0},        	/* 0x3b :  ;        */
  {0x0a,0xe2,0xe3,0x0b,0,0,0},		/* 0x3c :  <        */
  {0xb8,0xb9,0,0,0,0,0},        	/* 0x3d :  =        */
  {0x0a,0xe4,0xe5,0x0b,0,0,0},		/* 0x3e :  >        */
  {0x0a,0xe6,0xe7,0x0b,0,0,0},		/* 0x3f :  ?        */
  {0x0a,0x7a,0x7b,0x0b,0,0,0},		/* 0x40 :  @        */
  {0x0a,0x5a,0x5b,0x0b,0,0,0},		/* 0x41 :  A        */
  {0x0a,0x30,0x31,0x0b,0,0,0},		/* 0x42 :  B        */
  {0x0a,0x34,0x35,0x0b,0,0,0},		/* 0x43 :  C        */
  {0x0a,0x56,0x57,0x0b,0,0,0},		/* 0x44 :  D        */
  {0x0a,0x68,0x69,0x0b,0,0,0},		/* 0x45 :  E        */
  {0x0a,0x54,0x55,0x0b,0,0,0},		/* 0x46 :  F        */
  {0x0a,0x52,0x53,0x0b,0,0,0},		/* 0x47 :  G        */
  {0x0a,0x50,0x51,0x0b,0,0,0},		/* 0x48 :  H        */
  {0x0a,0xc0,0xc1,0x0b,0,0,0},		/* 0x49 :  I        */
  {0x0a,0xd0,0xd1,0x0b,0,0,0},		/* 0x4a :  J        */
  {0x0a,0xd2,0xd3,0x0b,0,0,0},		/* 0x4b :  K        */
  {0x0a,0xd4,0xd5,0x0b,0,0,0},		/* 0x4c :  L        */
  {0x0a,0xe0,0xe1,0x0b,0,0,0},		/* 0x4d :  M        */
  {0x0a,0xf0,0xf1,0x0b,0,0,0},		/* 0x4e :  N        */
  {0x0a,0xc2,0xc3,0x0b,0,0,0},		/* 0x4f :  O        */
  {0x0a,0xc4,0xc5,0x0b,0,0,0},		/* 0x50 :  P        */
  {0x0a,0x6c,0x6d,0x0b,0,0,0},		/* 0x51 :  Q        */
  {0x0a,0x66,0x67,0x0b,0,0,0},		/* 0x52 :  R        */
  {0x0a,0x58,0x59,0x0b,0,0,0},		/* 0x53 :  S        */
  {0x0a,0x64,0x65,0x0b,0,0,0},		/* 0x54 :  T        */
  {0x0a,0x60,0x61,0x0b,0,0,0},		/* 0x55 :  U        */
  {0x0a,0x32,0x33,0x0b,0,0,0},		/* 0x56 :  V        */
  {0x0a,0x6a,0x6b,0x0b,0,0,0},		/* 0x57 :  W        */
  {0x0a,0x36,0x37,0x0b,0,0,0},		/* 0x58 :  X        */
  {0x0a,0x62,0x63,0x0b,0,0,0},		/* 0x59 :  Y        */
  {0x0a,0x38,0x39,0x0b,0,0,0},		/* 0x5a :  Z        */
  {0xc6,0xc7,0,0,0,0,0},        	/* 0x5b :  [        */
  {0xca,0xcb,0,0,0,0,0},        	/* 0x5c :  \        */
  {0xc8,0xc9,0,0,0,0,0},        	/* 0x5d :  ]        */
  {0x0a,0x72,0x73,0x0b,0,0,0},		/* 0x5e :  ^        */
  {0x0a,0xb6,0xb7,0x0b,0,0,0},		/* 0x5f :  _        */
  {0x7e,0x7f,0,0,0,0,0},        	/* 0x60 :  `        */
  {0x5a,0x5b,0,0,0,0,0},		/* 0x61 :  a        */
  {0x30,0x31,0,0,0,0,0},		/* 0x62 :  b        */
  {0x34,0x35,0,0,0,0,0},		/* 0x63 :  c        */
  {0x56,0x57,0,0,0,0,0},		/* 0x64 :  d        */
  {0x68,0x69,0,0,0,0,0},		/* 0x65 :  e        */
  {0x54,0x55,0,0,0,0,0},		/* 0x66 :  f        */
  {0x52,0x53,0,0,0,0,0},		/* 0x67 :  g        */
  {0x50,0x51,0,0,0,0,0},		/* 0x68 :  h        */
  {0xc0,0xc1,0,0,0,0,0},		/* 0x69 :  i        */
  {0xd0,0xd1,0,0,0,0,0},		/* 0x6a :  j        */
  {0xd2,0xd3,0,0,0,0,0},		/* 0x6b :  k        */
  {0xd4,0xd5,0,0,0,0,0},		/* 0x6c :  l        */
  {0xe0,0xe1,0,0,0,0,0},		/* 0x6d :  m        */
  {0xf0,0xf1,0,0,0,0,0},		/* 0x6e :  n        */
  {0xc2,0xc3,0,0,0,0,0},		/* 0x6f :  o        */
  {0xc4,0xc5,0,0,0,0,0},		/* 0x70 :  p        */
  {0x6c,0x6d,0,0,0,0,0},		/* 0x71 :  q        */
  {0x66,0x67,0,0,0,0,0},		/* 0x72 :  r        */
  {0x58,0x59,0,0,0,0,0},		/* 0x73 :  s        */
  {0x64,0x65,0,0,0,0,0},		/* 0x74 :  t        */
  {0x60,0x61,0,0,0,0,0},		/* 0x75 :  u        */
  {0x32,0x33,0,0,0,0,0},		/* 0x76 :  v        */
  {0x6a,0x6b,0,0,0,0,0},		/* 0x77 :  w        */
  {0x36,0x37,0,0,0,0,0},		/* 0x78 :  x        */
  {0x62,0x63,0,0,0,0,0},		/* 0x79 :  y        */
  {0x38,0x39,0,0,0,0,0},		/* 0x7a :  z        */
  {0x0a,0xc6,0xc7,0x0b,0,0,0},		/* 0x7b :  {        */
  {0x0a,0xca,0xcb,0x0b,0,0,0},		/* 0x7c :  |        */
  {0x0a,0xc6,0xc9,0x0b,0,0,0},		/* 0x7d :  }        */
  {0x0a,0x7e,0x7f,0x0b,0,0,0},		/* 0x7e :  ~        */
  {0x0a,0x3e,0x3f,0x0b,0,0,0},		/* 0x7f :  delete   */
};


static int read_barcode(device, data, data_type, pending)
  HIL_Device *device;
  unsigned char *data, *data_type;
  int *pending;
{
  unsigned char *ptr;

  ptr = device->ptr;

  if (ptr)			/* In the middle of process data */
  {
    if (!*ptr)
    {
      device->ptr = NULL;
      return READ_SUCCESS;
    }

    *pending = 1;
    *data_type = KEY_DATA;
    *data = *ptr++;

    device->ptr = ptr;

    return READ_SUCCESS;
  }

  if (get_packet(device->fd))
  {
    int poll_hdr = a_packet.poll_hdr;

    pop_packet();

    if (poll_hdr & KEY_DATA_MASK)
    {
      device->ptr = ascii_to_code[a_packet.data[0]];
      return read_barcode(device, data, data_type, pending);
    }
  }

  *pending = 0;
  return READ_FAILURE;
}

/* ******************************************************************** */
/* ************************* Parse X*devices ************************** */
/* ******************************************************************** */

/* 
  X*devices:
    position device_name X_use
      eg:
      first keyboard keyboard
      first mouse    pointer
      first tablet   other

    path X_use
      eg:
        /dev/hil3    keyboard
	/tmp/foo     pointer
	/dev/hil7    other

    path  hil_path
      Use path instead of "/dev/hil".
 */



static int parse_1_parm(use, use_as) char *use; int *use_as;
{
  if      (0 == strcmp(use, "POINTER"))  *use_as = XPTR_USE;
  else if (0 == strcmp(use, "KEYBOARD")) *use_as = XKBD_USE;
  else if (0 == strcmp(use, "OTHER"))    *use_as = XOTH_USE;
  else
  {
    fprintf(stderr,
      "Bad device use \"%s\" in X*devices file, entry skipped.\n", use);
    return FALSE;
  }
  return TRUE;
}

/***********************************************************************
 *
 * This routine is invoked when two parameters are specified.
 * Either they are a device path and intended use, or a device loop path.
 */

static int parse_2_parms(dev, use, use_as)
    char *dev, *use;
    int *use_as;
    {
    uppercase(use);

    if (0 == strcmp (use,"HIL_PATH"))
	{
	strcpy (hil_file_base_name,dev);
	return FALSE;		/* Fake out configure() */
	}

    return parse_1_parm(use, use_as);
    }

    /* Parse lines with 3 parameters, such as:
     *   first keyboard keyboard
     * Params are a position, a device type, and its intended use.
     * Input:
     *   pos : first param
     *   x_name : second param
     *   use : third param
     *   ipos : 
     *   itype : dev_type
     *   use_as : Pointer to int, filled in with what third parm is.
     * Returns:
     */
static int parse_three_parms(pos, x_name, use, ipos, itype, use_as)
    char *pos, *x_name, *use;
    int *ipos, *itype, *use_as;
{
    int i;

    uppercase(pos);
    uppercase(use);

    for (i=0; i<NITEMS(h_position); i++)
	if (0 == strcmp(h_position[i], pos))
	{
	  *ipos = i + 1;	/* h_position[0] = "FIRST" == 1 */
	  break;
	}

    if (i >= 7)                  /* failed, skip this entry */
    {
	fprintf(stderr,
	    "Bad ordinal \"%s\" in X*devices file, entry skipped.\n",
	    pos);
	return FALSE;
    }

    uppercase(x_name);

    for (i=0; i<NITEMS(devices); i++)
	if (0 == strcmp(devices[i].X_name,x_name))
	{
	  *itype = devices[i].dev_type;
	  break;
	}

    if (i == NITEMS(devices)) /* failed, skip this entry */
    {
	fprintf(stderr,
	    "Bad device type \"%s\" in X*devices file, entry skipped.\n",
	    x_name);
	return FALSE;
    }

    return parse_1_parm(use, use_as);
}

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
static int process_path(path, itype, ipos, parm1, use_as)
  char *path, *parm1;
  int *ipos, *itype, *use_as;
{
  int parms;
  char parm2[256], parm3[256];

  parm1[0] = parm2[0] = parm3[0] = '\0';

  parms = sscanf(path, "%s%s%s", parm1, parm2, parm3);

       if (*parm1 == '#') return FALSE;	/* comment line */
  else if (*parm2 == '#') parms = 1;	/* error */
  else if (*parm3 == '#') parms = 2;

  if (2 == parms)	/* device name specified */
    return parse_2_parms (parm1, parm2, use_as);
  if (3 == parms)
    return parse_three_parms (parm1, parm2, parm3, ipos, itype, use_as);

  fprintf(stderr,
    "Bad line in X*devices file, entry skipped:\n   %s", path);

  return FALSE;
}

#define MAX_X0devices 20
typedef struct
{
  int live, dev_type, position, use_as, a_path;
  char path[256];
} X0Device;

static void set_X0device(ptr, type, position, use_as, path)
  X0Device *ptr; char *path;
{
  ptr->live = TRUE;
  ptr->dev_type = type;
  ptr->position = position;
  ptr->use_as |= use_as;
  if (use_as == XKBD_USE || use_as == XPTR_USE)
      ptr->use_as |= EXPLICIT;
  if (type == -1)	/* 2 params, ie got a path */
  {
    ptr->a_path = TRUE;
    strcpy(ptr->path, path);
  }
}

static void pick_default_device(live, type, use_as, x, in_use)
{

  if (live) return;	/* device specified, no need to pick a default */
	/* return last device of type */
  find_device_by_type(type, 100, use_as, x, in_use);
  /* else no default */
}

static HIL_Device *do_path(path, use_as) char *path;
{	/* "/dev/hil3 keyboard" or "/tmp/keys keyboard" */
  int nth, len;

  len = strlen(hil_file_base_name);

      /* if "/dev/hilx", use existing loop stuff */
  nth = path[len] - '0';	/* '1' => 0x1 */
  if (
      (len + 1) == strlen(path)				&&
      (0 == strncmp(path, hil_file_base_name, len))	&&
      (1 <= nth && nth <= 7))
  {
    return use_device_n(nth, use_as);
  }
	/* "/tmp/keys" or "/dev/HIL" or "/dev/hil8" */
  return add_device(path, FALSE, use_as);
}

#define RECYCLE_CMD  	"X*devices:Recycle"

static int process_X0devices(d) HPInputDeviceHeader *d;
{
  FILE *fptr;
  char *path;
  int n, use_as;
  X0Device X0devices[MAX_X0devices], *ptr;

  path = d->path + sizeof(RECYCLE_CMD);
  memset(X0devices, 0, sizeof(X0devices));
  memset(loop_device_list, 0, sizeof(loop_device_list));

  if (fptr = fopen(path, "r"))
  {
    char buf[256], word[256], *keyword;
    int num_X0devices = 2, ignoring, itype, ipos;

    buf[0] = '\0';
    ignoring = FALSE; keyword = "BEGIN_DEVICE_DESCRIPTION";

    while (fgets(buf, 250, fptr))
    {
      if (*buf == '#' || *buf == '\n') continue;  /* quick speed ups */

      if (1 == sscanf(buf, "%s", word))
      {
	uppercase(word);
	if (0 == strcmp(keyword, word))
	{
	  keyword = ignoring ?
		"BEGIN_DEVICE_DESCRIPTION" : "END_DEVICE_DESCRIPTION";
	  ignoring = !ignoring;
	  continue;
	}
      }

      itype = -1;
      if (ignoring || !process_path(buf, &itype, &ipos, word, &use_as))
	continue;

      switch(use_as)
      {
	case XKBD_USE: n = XKEYBOARD; break;
	case XPTR_USE: n = XPOINTER; break;
	case XOTH_USE:
	  if (MAX_X0devices == num_X0devices)
	  {
	    fprintf(stderr, "Too many \"other\" devices in X*devices\n");
	    continue;
	  }
	  n = num_X0devices++;
	  break;
      }
      set_X0device(&X0devices[n], itype, ipos, use_as, word);
    }

    fclose(fptr);
  }

  catalog_loop();

  for (n = MAX_X0devices, ptr = X0devices; n--; ptr++)
  {
    if (!ptr->live) continue;
    if (ptr->a_path) do_path(ptr->path, ptr->use_as);
    else
    {
      if (ptr->dev_type == NULL_DEVICE)
      {
	HIL_Device *device;

        if (find_null_device(ptr->use_as) == NULL)
	  if (device = add_device("/dev/null", TRUE, ptr->use_as))
	     device->device = &devices[0];
      }
      else
        find_device_by_type(ptr->dev_type, ptr->position, ptr->use_as, FALSE, TRUE);
    }
  }

  pick_default_device(X0devices[XKEYBOARD].live, KEYBOARD, XKBD_USE, FALSE, FALSE);
  if (!find_device_by_use(XKBD_USE))		/* still no keyboard */
    pick_default_device(FALSE, KEYBOARD, XKBD_USE, TRUE, FALSE);
  if (!find_device_by_use(XKBD_USE))		/* still no keyboard */
    pick_default_device(X0devices[XKEYBOARD].live, KEYBOARD, XKBD_USE, FALSE, TRUE);
  if (!find_device_by_use(XKBD_USE))		/* still no keyboard */
    pick_default_device(FALSE, KEYBOARD, XKBD_USE, TRUE, TRUE);

  pick_default_device(X0devices[XPOINTER].live, MOUSE,    XPTR_USE, FALSE, FALSE);
  if (!find_device_by_use(XPTR_USE))		/* still no pointer */
    pick_default_device(FALSE, MOUSE, XPTR_USE, TRUE, FALSE);
  if (!find_device_by_use(XPTR_USE))		/* still no pointer */
    pick_default_device(X0devices[XPOINTER].live, MOUSE, XPTR_USE, FALSE, TRUE);
  if (!find_device_by_use(XPTR_USE))		/* still no pointer */
    pick_default_device(FALSE, MOUSE, XPTR_USE, TRUE, TRUE);
  if (!find_device_by_use(XPTR_USE))		/* still no pointer */
  {					/* use X keyboard (if it exists) */
    HIL_Device *device;
    if ((device = find_device_by_use(XKBD_USE)) && 
	 device->device->dev_type != NULL_DEVICE)
	 device->use_as |= XPTR_USE;
  }

  name_loop();

  if (X0devices[XPOINTER].position == 1 &&	/* position is "FIRST"  */
      X0devices[XPOINTER].dev_type == KEYBOARD) /* type is KEYBOARD     */
      d->reserved[3] = 1;

  return TRUE;
}

/* ******************************************************************** */
/* ************************ X Server Interface ************************ */
/* ******************************************************************** */

#define PARSE_KEYWORD	"X*devices:"

    /* 
     * Notes:
     *   The "path" is passed in unchanged from X0devices.  I overload this:
     *     <device_name>
     *     <device_name>:<keymap_name>
     *     you_pick
     *     you_pick:<keymap_name>
     */
static int configure(d) HPInputDeviceHeader *d;
{
  HIL_Device *ptr;

  if (0 == strncmp(d->path, RECYCLE_CMD, sizeof(RECYCLE_CMD) - 1))
  {
    open_beeper();
    process_X0devices(d);
    next_device(TRUE);

    return INIT_SUCCESS;
  }
  else if (0 == strcmp(d->path, PARSE_KEYWORD))
  {
    while ((ptr = next_device(FALSE)) && !ptr->device_exists)  ;
    if (!ptr) return INIT_FAILURE;
    strcpy(d->path,ptr->file_name);
  }
  else
  {
    char *path, dev_path[255];
    int i, fd, ipos, itype, use_as;

    catalog_loop();

    ptr = do_path(d->path, XEXT_USE);

    if (!ptr) return INIT_FAILURE;

    ptr->use_as &= ~EXPLICIT;

    name_loop();

#ifdef VERBOSE
printf("Configure: %d \"%s\" %s %d %s\n",
  ptr->fd, path, ptr->keymap, d->button_chording, ptr->long_name); 	/*  */
#endif /* VERBOSE */
  }

  d->min_kcode = (ptr->hil_id == QUAD_ID ? NINE_KNOB_ID : ptr->hil_id);
  d->max_kcode = ptr->iob;
  ptr->latching_enabled = d->reserved[0];
  d->reserved[0] = HP_HIL;
  d->reserved[1] = ptr->device->dev_type;
  if (ptr->use_as & XKBD_USE || ptr->use_as & XPTR_USE)
      ptr->use_as &= (XKBD_USE | XPTR_USE | EXPLICIT);
  d->reserved[2] = ptr->use_as;
  d->x_name = ptr->long_name;
  d->file_ds = ptr->fd;
  switch (ptr->device->x_type)
  {
    case KEYBOARD:
      if (ptr->iob & HILIOB_NPA)
        d->reserved[0] |= PC101_KBD;
      else
        d->reserved[0] |= HP_ITF_KBD;
      set_autorepeat(ptr->fd, AutoRepeatModeOn);

      d->keymap_name = ptr->keymap;
      d->flags = DATA_IS_8_BITS;
      d->num_keys = 1;

      d->num_ledf = 1;	                 /* number of led feedbacks */
      d->ledf = &ptr->num_leds;	         /* number of leds          */

      d->reset = 0xf;		/* Reset keycode on HIL keyboards*/
      d->reset_mods = (ShiftMask | ControlMask);	/* Shift Control */

      break;
    case MOUSE:
      d->keymap_name = ptr->keymap;
      d->flags = ptr->data_size;
      if (ptr->flags & HIL_ABSOLUTE)
	d->flags |= (ABSOLUTE_DATA | REPORTS_PROXIMITY);
      d->num_keys = 0;
      d->ax_num = ptr->num_axes;

      ptr->error = 0;
      ptr->chorded_button_up = 0;

      {
	int buttons = ptr->num_buttons;

	if (1 < buttons && buttons <= MAX_BUTTONS_FOR_CHORDING)/* we can chord*/
	{
	  if (buttons==2 && d->button_chording==0)
	      d->button_chording=100;
	  ptr->chording_interval = d->button_chording;
	  if (d->button_chording) buttons = (2 * buttons - 1);
	}
	if (MAX_X_BUTTONS < buttons) buttons = MAX_X_BUTTONS;
	d->num_buttons = buttons;
      }

      d->num_keys = 0;     		/* no keys                 */
      d->resolution = ptr->res_x / 100;	/* resolution in counts/cm */
      d->max_x = ptr->max_x;		/* maximum x value in counts */
      d->max_y = ptr->max_y;		/* maximum y value in counts */

      break;
    case XOTHER:	/* !!!??? the NULL device */
      d->ax_num = 2;
      d->max_x = d->max_y = 1000;
/*      d->res_x = d->res_y = 1000;	/*  */
      d->resolution = 10;
      d->num_keys = 1;
      d->num_buttons = 3;
      d->min_kcode = 0xd5;
      d->keymap_name = "ITF_US_English";
      break;
    default: return INIT_FAILURE;
  }

  return INIT_SUCCESS;
}

/**************************************************************************
 *
 * This routine is called by the X server to read from the device driver.
 *
 */

static int read_device(fd, data, data_type, pending)
  int fd, *pending;
  unsigned char *data, *data_type;
{
  HIL_Device *device;

  device = find_device_by_fd(fd);

/*fprintf(stderr,"read_device(%d)\n", fd);	/*  */

  if (device)
	return device->device->read_device(device, data, data_type, pending);

  *pending = 0;
  return READ_FAILURE;
}

/**************************************************************************
 *
 * This routine is called by the X server to write to the device driver.
 *
 */

static int write_to_device(fd, request, data)
  int fd, request;
  char *data;
{
  HIL_Device *device;

  device = find_device_by_fd(fd);

/*fprintf(stderr,"write_device(%d)\n", fd);	/*  */

  if (device) return device->device->write_device(device, request, data);

  return WRITE_FAILURE;
}

/**************************************************************************
 *
 * This routine is called by the X server to initialize the device driver.
 *
 */

int hil_driver_Init(serialproc) SerialProcs *serialproc;
{
  serialproc->configure	= configure;
  serialproc->read	= read_device;
  serialproc->write	= write_to_device;
  serialproc->close	= close_device;

  return INIT_SUCCESS;
}

#ifdef TEST

/* ******************************************************************** */
/* ***************************** TESTing ****************************** */
/* ******************************************************************** */

void read_dump(device) HIL_Device *device;
{
  unsigned char data[20], data_type = 0;
  int pending;

  if (!device) return;

  while (READ_SUCCESS == read_device(device->fd, &data, &data_type, &pending))
  {
    int i;

if (pending == 0) continue;
    printf("%d %d ", device->fd, pending);
    if (data_type & MOTION_DATA)	printf("Motion ");
    if (data_type & PROXIMITY_DATA)	printf("Proximity ");
    if (data_type & BUTTON_DATA)	printf("Button ");
    if (data_type & KEY_DATA)		printf("Key ");
    printf(": ");
    switch (device->data_size)
    {
      case DATA_IS_8_BITS:
        for (i = 0; i < pending; i++) printf("%x ", data[i]);
	break;
      case DATA_IS_16_BITS:
        for (i = 0; i < pending; i += 2)
	  printf("%x ", get_data(data + i, TRUE));
        break;
      default:
        printf("read_dump():  Unknown data size: %d", device->data_size);
	break;
    }
    printf("\n");
    data_type = 0;
  }
}

main()
{
  HPInputDeviceHeader d;
  HIL_Device *keyboard = NULL, *mouse = NULL;
  int s;

#if 1
  strcpy(d.path,"X*devices:Recycle:/etc/X11/X0devices");
  configure(&d);
  strcpy(d.path,"X*devices:");
  while(INIT_SUCCESS == configure(&d))
  {
    if (d.file_ds != -1)
    {
      HIL_Device *ptr;

      ptr = find_device_by_fd(d.file_ds);
      switch (ptr->device->x_type)
      {
	case KEYBOARD: keyboard = ptr; break;
	case MOUSE:    mouse = ptr;    break;
      }
    }
    strcpy(d.path,"X*devices:");
  }
#else
  strcpy(d.path,"second keyboard keyboard");	/*  */
/*  strcpy(d.path,"third keyboard keyboard");	/*  */
/*  strcpy(d.path,"last keyboard keyboard");	/*  */
/*  strcpy(d.path,"/dev/hil4 keyboard");	/*  */
  s = configure(&d);
  if (s == INIT_SUCCESS) keyboard = find_device_by_fd(d.file_ds);
  else fprintf(stderr,"Couldn't find a keyboard\n");

  strcpy(d.path,"first mouse pointer");
  d.button_chording = 100;
  s = configure(&d);
  if (s == INIT_SUCCESS) mouse = find_device_by_fd(d.file_ds);
  else fprintf(stderr,"Couldn't find a mouse\n");
#endif

  while (1)
  {
    read_dump(keyboard);
    read_dump(mouse);
  }
}

#endif	/* TEST */
#ifdef DEBUG
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/termio.h>
#include <sys/ptyio.h>
#include <sys/hilioctl.h>
#include "HilCmds.h"
#include "HilData.h"

#define max( a, b ) (( a > b ) ? a : b )
#define min( a, b ) (( a > b ) ? b : a )

int BaseFiledescriptor = 99;
int MaxOpen;
int SelectMask;
void SignalHandler();
void CloseAllHilDevices();
void HandshakeDevice();
#define HOSTBUFFERSIZE 1024 
char Hostname[ HOSTBUFFERSIZE ];
#define PTY_DIR		"/tmp/"
#define NUM_DEVS 9

/*
 * the following strings are returned by the HIL driver
 * describing the associated HIL device in reponse to 
 * an HILID ioctl
 */
unsigned char three_axes[11] =
  { 0x75,0x13,0x61,0x0F,0x03,   0,    0,   0,   0,   0,   0 };
unsigned char no_axes[11] =
  { 0x74,0x00, 0x0b,   0,   0,   0,    0,   0,   0,   0,   0 };
unsigned char error_device[11] =
  { 0x73,0x00, 0x0f,   0,   0,   0,    0,   0,   0,   0,   0 };
unsigned char no_iob_rel[11] =
  { 0x71,0x01, 0x90,   0x01,   0,   0,    0,   0,   0,   0,   0 };
unsigned char one_axis[11] =
  { 0x70,0x41, 0x90,   0x01,   0x03,   0x04,    0x03,   0,   0,   0,   0 };
unsigned char pc_keyboard[11] =
  { 0xde,0x10, 0x30,   0,   0,   0,    0,   0,   0,   0,   0 };
unsigned char us_keyboard[11] =
  { 0xdf,   0,0xc2,   0,   0,   0,    0,   0,   0,   0,   0 };
unsigned char asize_tablet[11] =
  { 0x93,0x72,0x90,0x01,0x80,0x2e,0xd4,0x21,0x0c,   0,   0 };
unsigned char bsize_tablet[11] =
  { 0x94,0x72,0x90,0x01,0x90,0x43,0xd4,0x2e,0x0b,   0,   0 };
unsigned char three_button_mouse[11] =
  { 0x72,0x12,0xC2,0x1E,0x03,0x43,    0,   0,   0,   0,   0 };
unsigned char two_button_mouse[11] =
  { 0x68,0x12,0xC2,0x1E,0x02,0x43,    0,   0,   0,   0,   0 };
unsigned char buttonbox[11]=
  { 0x30,0x10,0x80,0x1E,0x00,   0,    0,   0,   0,   0,   0 };
unsigned char id_module[11]=
  { 0x34,0x10,0x00,0x00,0x00,   0,    0,   0,   0,   0,   0 };
unsigned char nineknobbox[11] =
  { 0x61,0x13,0x61,0x0F,0x00,   0,    0,   0,   0,   0,   0 };
unsigned char oneknobbox[11] =
  { 0x60,0x11,0x61,0x0F,0x00,   0,    0,   0,   0,   0,   0 };
unsigned char barcode[11] =
  { 0x5c,0,0x80,   0,   0,   0,    0,   0,   0,   0,   0 };
unsigned char quadrature[11] =
  { 0x61,0x12,0x61,0x0f,0x03,0x43,    0,   0,   0,   0,   0 };
unsigned char touchscreen[11] =
  { 0x8c,0x52,0x0a,0x01,0x38,0,0x2a,0,0x08,   0,   0 };
unsigned char bogus[11]=  
  {    0,   0,    0,   0,   0,   0,    0,   0,   0,   0,   0 };

struct	HilDeviceInfo {
    int			id;
    char		* name;
    unsigned char	* pHilIdString;
    unsigned char	num_keys;
    unsigned char	min_kcode;
    unsigned char	max_kcode;
};
struct	 HilDeviceInfo DeviceTable[] = {
    { X_BUTTONBOX,   "BUTTONBOX",    buttonbox, 32,8,39 },
    { X_ID_MODULE,   "ID_MODULE",    id_module,      0,0,0 },
    { X_BARCODE,     "BARCODE",      barcode,  109,8,135 },
    { X_NO_AXES,     "NO_AXES",      no_axes,      0,0,0 },
    { X_THREE_AXES,   "THREE_AXES",  three_axes,      0,0,0 },
    { X_ONE_AXIS,    "ONE_AXIS",     one_axis,      0,0,0 },
    { X_NO_IOB_REL,  "NO_IOB_REL",   no_iob_rel,    0,0,0 },
    { X_ERROR_DEVICE,"ERROR_DEVICE", error_device,  0,0,0 },
    { X_ONE_KNOB,    "ONE_KNOB",     oneknobbox,      0,0,0 },
    { X_NINE_KNOB,   "NINE_KNOB",    nineknobbox,  0,0,0 },
    { X_QUADRATURE,  "QUADRATURE",   quadrature,  0,0,0 },
    { X_MOUSE_2B,    "MOUSE",        two_button_mouse,  0,0,0 },
    { X_MOUSE_3B,    "MOUSE",        three_button_mouse,  0,0,0 },
    { X_TOUCHSCREEN, "TOUCHSCREEN",  touchscreen,  0,0,0 },
    { X_TABLET_ASIZE,"TABLET_ASIZE", asize_tablet,  0,0,0 },
    { X_TABLET_BSIZE,"TABLET_BSIZE", bsize_tablet,  0,0,0 },
    { X_KB_US,       "KEYBOARD",     us_keyboard,109,8,135 },
    { X_KB_PC,       "KEYBOARD",     pc_keyboard,109,8,135 },
    { 0x00,        "NULL",         bogus,  0,0,0 } 
};

struct HilLoopInfo {
#define MAXNAMLEN	1024
	char PtyName[MAXNAMLEN];	/* device name to link the pty to  */
	char SlaveName[MAXNAMLEN];	/* pty slave name		   */
	int  Pty;		/* fd of master side of device pty */
	int  PtyMask; 		/* select mask for same		   */
	int  PtyCount;		/* number of bytes written to pty  */
	struct HilDeviceInfo *pHilDeviceInfo;
	struct HilCommandLog *pNext;
	struct HilCommandLog *pLast;
} hil[NUM_DEVS];

struct HilCommandLog {
        int  Command;
	struct HilCommandLog *pNext;
};

unsigned char tcget_info[17] =
  { 0,0,0,0,0x09,0x67,0,0,0,0x7f,0x1c,0x23,0x40,0x04,0,0,0 };

/***********************************************************
 *  
 * Main entry point.  This program will fork into two processes.
 * The first is a test program that will invoke the entry points
 * provided by this input device driver.  The second is a daemon
 * that will handle ioctls from the driver.
 *  
 * Functionality to be tested:
 *
 *   driver initialization entry point
 *
 *  - touch test
 *
 *   configuration entry point
 *
 *   - default configuration with no X*devices specification
 *	- only a non-keyboard key device
 *	- only a keyboard
 *	- keyboard and mouse
 *	- non-keyboard key device and mouse
 *	- keyboard and non-mouse pointer
 *	- multiple keyboards and mouse
 *	- multiple key devices and mouse
 *	- keyboard and multiple pointing devices
 *	- multiple key devices and multiple pointing devices
 *	- maximum number of input devices
 *   - configuration with an X0devices file
 *	- non-default keyboard device
 *	- non-default pointer device
 *	- null keyboard and null pointer
 *	- null keyboard and mouse
 *	- keyboard and null mouse
 *	- specify "other" devices
 *	- specify non-default ordinal
 *	- specify invalid ordinal
 *	- specify invalid device
 *	- specify invalid use
 *	- specify too many input devices
 *	- specify too many input parameters
 *	- specify not enough input parameters
 *
 *   read entry point
 *  
 *   - read keyboard data  
 *   - read buttonbox data 
 *   - read barcode data 
 *   - read mouse motion/button data
 *   - read tablet motion/button data
 *   - read nine-knob motion data
 *   - button chording
 *
 *   write entry point
 *  
 *   - turn on LEDS
 *   - turn autorepeat on/off
 *   - turn keyclick on/off
 *   - sound the bell
 *  
 *   close entry point
 *  
 *   - touch test  
 *  
 ***********************************************************/
main(argc,argv)
    char *argv[];
    int argc;
    {
    int i, pid;

    for (i=0; i<NUM_TEST_CASES; i++)
	{
        if ((pid = fork()) == 0)	/* This is the child process    */
#if 0
        if (argc > 1)
#endif
	    {
	    do_child_process(i);
	    exit(0);
	    }
        else			
	    {
	    usleep(50000, 0, NULL);
	    do_parent_process(i);
	    waitpid (pid, NULL, 0);
	    }
        }
    }

/***********************************************************
 *  do_child_process
 *
 * This is the process that handles ioctls from the driver.
 * It sets up an array of input devices that will be listed
 * by the driver.  How they are used will be determined by
 * the information in the X*devices file.
 *
 */

do_child_process(i)
    int i;
    {

    signal( SIGINT, SignalHandler );
    signal( SIGHUP, SignalHandler );
    signal( SIGTERM, SignalHandler );
    gethostname( Hostname, HOSTBUFFERSIZE );

    SetupHILDevices (tc[i].devs);

    /*
     * loop, servicing the ptys when they so indicate via the select mask.
     */

    for ( ;; )
	{
	int j, ret;
	int ExceptionMask = SelectMask;

	ret = select( MaxOpen + 1,  NULL, NULL, &ExceptionMask,  0 );

	/*
	 * if the pty's emulating the HIL devices need
	 * to be serviced, then handshake them.
	 */
	if ( ExceptionMask )
	    {
	    for ( j = 0; j < 32; j++ )
		{
		if ( ExceptionMask & 1 )
		    HandshakeDevice( j - BaseFiledescriptor );
		ExceptionMask >>= 1;
		}
	    }
	}
    }

/***********************************************************
 *  do_parent_process
 *
 * This is the parent process.  It performs the following functions:
 *     - Set up the X0devices file to read from the daemon ptys
 *     - Invoke the driver initialiation and device initialization routines.
 *     - Invoke the read routines to read data from the test input devices.
 */

struct mydata
    {
    int valid;
    int	type;
    } ;

#define TPSENDD	 _IOW('Z', 1, struct mydata)
#define TPSTOPIT _IO('Z', 2)

do_parent_process(i)
    int i;
    {
    int status, fd, count;
    char buffer[128];
    SerialProcs s;
    HPInputDeviceHeader d[10], *dptr = d;
    HIL_Device *lptr;
    HPKeyboardFeedbackControl kctl;
    char TruncHost[1024];

    gethostname( Hostname, HOSTBUFFERSIZE );
    strcpy (TruncHost, Hostname);
    strtok (TruncHost, ".");

    for (count=0; count<9; count++)
	{
	d[count].path[0]='\0';
        d[count].button_chording = 100;
	}
    fd = creat("./X0devices.test", 0777);
    sprintf (buffer,"/tmp/%s:hil hil_path\n",TruncHost);
    write (fd, buffer,strlen(buffer));
    if (strlen(tc[i].devices_file))
        write (fd, tc[i].devices_file, strlen(tc[i].devices_file));
    else 
	{
        sprintf (buffer,"/tmp/%s:hil7 other\n",TruncHost);
        write (fd, buffer,strlen(buffer));
	}
    close(fd);

    if ((status = hil_driver_Init(&s)) != INIT_SUCCESS)
	{
	printf("Driver initialization failure: %x\n",status);
	CloseAllHilDevices();
	exit(1); 
	}

    strcpy(d[0].path,"X*devices:Recycle:./X0devices.foobar");
    status = (*(s.configure)) (d);
    for (lptr=loop_device_list,count=0; count<10; count++,lptr++)
	if (lptr->fd > 0)
            (*(s.close)) (lptr->fd);
    fd = creat("./X0devices.empty", 0777);
    close(fd);
    strcpy(d[0].path,"X*devices:Recycle:./X0devices.empty");
    status = (*(s.configure)) (d);
    for (lptr=loop_device_list,count=0; count<10; count++,lptr++)
	if (lptr->fd > 0)
            (*(s.close)) (lptr->fd);
    test_configure_entry_point(&s, d, i);
    for (dptr=d,i=0; i<10; i++,dptr++)
	{
        unsigned char device_type = dptr->min_kcode;
	if (dptr->file_ds > 0 || *dptr->path=='\0')
	    continue;
	if (device_type == X_NINE_KNOB)
	    dptr->button_chording = 0;
        if ((status = (*(s.configure)) (dptr)) != INIT_SUCCESS)
	    {
	    printf("Device '%s' initialization failure status: %x\n",
		dptr->path, status);
	    CloseAllHilDevices();
	    exit(1); 
	    }
	}
    for (dptr=d,i=0; i<10; i++,dptr++)
	{
	if (dptr->file_ds <= 0 || *dptr->path=='\0')
	    continue;
	if (strcmp(dptr->x_name, "FIRST_NULL")!=0)
            test_read_entry_point(&s, dptr);
	if (i==0)
	    {fd = beeper_fd; beeper_fd=-1;}
        test_write_entry_point(&s, dptr);
	if (i==0)
	    beeper_fd = fd;
	if (strcmp(dptr->x_name, "FIRST_NULL")!=0)
            test_read_entry_point(&s, dptr);
	}
    for (dptr=d,i=0; i<10; i++,dptr++)
	if (dptr->file_ds > 0)
  	    ioctl(dptr->file_ds, TPSTOPIT, buffer);	/* terminate child */
    for (dptr=d,i=0; i<10; i++,dptr++)
	{
	if (dptr->file_ds <= 0)
	    continue;
        if ((status = (*(s.close)) (dptr->file_ds)) != CLOSE_SUCCESS)
	    {
	    printf("Device '%s' close failure status: %x\n",dptr->path, status);
	    CloseAllHilDevices();
	    exit(1); 
	    }
	}
    close_beeper();
    close_beeper();
    }

/***********************************************************
 *  test_configure_entry_point
 *
 *  Test the configure() entry point of the HIL device driver.
 */

test_configure_entry_point(s, dptr, i)
    SerialProcs *s;
    HPInputDeviceHeader *dptr;
    {
    int fail, status;
    ExpectedData *expected = tc[i].expect;

    strcpy(dptr->path,"X*devices:Recycle:./X0devices.test");
    if ((status = (*(s->configure)) (dptr)) != INIT_SUCCESS)
	{
	printf("Device '%s' initialization failure status: %x\n",
		dptr->path, status);
	CloseAllHilDevices();
	exit(1); 
	}
    strcpy(dptr->path,"X*devices:");
    fail=0;
    while ((status = (*(s->configure)) (dptr)) == INIT_SUCCESS)
	{
	if (strcmp (dptr->x_name, expected->name)!=0 ||
	    dptr->file_ds != expected->fd ||
	    dptr->reserved[2] != expected->use)
	    {
	    printf ("expected data did not match\n");
	    printf ("a=%s e=%s a=%d e=%d a=%d e=%d\n", 
	    dptr->x_name, expected->name,
	    dptr->file_ds, expected->fd, dptr->reserved[2], expected->use);
#ifdef VERBOSE
	    printf("Found: %s,  fd: %d,  use as: %d, path: %s\n", 
		dptr->x_name, dptr->file_ds, dptr->reserved[2], dptr->path);
#endif /* VERBOSE */
	    fail++;
	    }
#ifdef VERBOSE
	else
	    printf("Found: %s,  fd: %d,  use as: %d, path: %s\n", 
		dptr->x_name, dptr->file_ds, dptr->reserved[2], dptr->path);
#endif /* VERBOSE */
	expected++;
	dptr++;
        strcpy(dptr->path,"X*devices:");
	}
    *dptr->path = '\0';
    if (fail==0)
	{
	printf("Test case %d succeeded:\n%s\n",i,tc[i].message);
	}
    else
	printf("Test case %d failed: \n%s\n",i,tc[i].message);
    }

/***********************************************************
 *  find_valid_data
 *
 *  Validate the data returned by the read routine.
 */

struct valid *find_valid_data(device_type)
    {
    struct valid *v = NULL;

    for (v=all_valid; (v->id != 0 && v->id != device_type); v++)
	;
    return v;
    }

/***********************************************************
 *  test_read_entry_point
 *
 *  Test the read() entry point of the HIL device driver.
 */

test_read_entry_point(s, d)
    SerialProcs *s;
    HPInputDeviceHeader *d;
    {
    int i, size, ndx, ret, status, nbytes=0, count=0, mask;
    unsigned char buf[128];
    unsigned char data_type=0, device_type = d->min_kcode;
    struct valid *v = find_valid_data(device_type);

    generate_test_data (d->file_ds, FALSE, device_type);
    status = (*(s->read)) (d->file_ds, buf, &data_type, &nbytes);
    if (status != READ_FAILURE)
        if (device_type == X_KB_US || device_type == X_BUTTONBOX)
	    printf("No READ_FAILURE w/bogus data for device %s\n",d->x_name);
	else if (nbytes != 0)
	    printf("No READ_FAILURE w/bogus data for device %s\n",d->x_name);
    (*(s->close)) (d->file_ds);
    status = (*(s->read)) (d->file_ds, buf, &data_type, &nbytes);
    (*(s->configure)) (d);
    generate_test_data (d->file_ds, TRUE, device_type);
    mask = 1 << d->file_ds;
    ndx = 0;
    while ((ret = usleep (1000, (d->file_ds + 1), &mask)) > 0)
        while ((ret = (*(s->read)) (d->file_ds, buf, &data_type, &nbytes))==READ_SUCCESS)
	{
	if (nbytes == 0)
	    continue;
	size = v->size ? v->size : v->vdata[ndx++];

	if (!(data_type & v->dtype) || nbytes != size || 
	    !validate_data (buf, &v->vdata[ndx], nbytes))
	    {
	    printf("Invalid data: type = %x (%x), size = %d (%d) data = ",
		data_type, v->dtype, nbytes, size);
	    for (i=0; i<nbytes; i++)
		printf ("%x (%x) ", buf[i], v->vdata[ndx+i]);
	    printf ("\n");
	    }
#ifdef VERBOSE
	else
	    printf("data matched\n");
#endif /* VERBOSE */
	count++;
	ndx += nbytes;
	data_type = nbytes = buf[0] = 0;
	}
    }

/***********************************************************
 *  validate_data
 *
 *  Validate the data returned by the read routine.
 */

validate_data (actual, expected, size)
    unsigned char *actual, *expected;
    {
    int i;

    for (i=0; i<size; i++)
	if (*actual != *expected)
	    return 0;
    return 1;
    }

/***********************************************************
 *  generate_test_data
 *
 *  Cause the child process to generate test data.
 */

generate_test_data (fd, valid, data_type)
    char data_type;
    {

    struct mydata foo;

    foo.type = data_type;
    foo.valid = valid;

    ioctl(fd, TPSENDD, &foo);
    usleep(50000, 0, NULL);
    }


/***********************************************************
 *  test_write_entry_point
 *
 *  Test the write() entry point of the HIL device driver.
 */

test_write_entry_point(s, dptr)
    SerialProcs *s;
    HPInputDeviceHeader *dptr;
    {
    int status;
    HPKeyboardFeedbackControl kctl;
    HPPointerFeedbackControl  pctl;
    HPBell                    bctl;
    HPValuatorControl         vctl;
    HPResolutionControl       rctl;

    /* Test XChangeFeedbackControl */

    kctl.class = 0xff;
    kctl.bell_percent = 100;
    kctl.bell_pitch = 0;
    kctl.bell_duration = 400;
    kctl.leds = -1;
    kctl.autoRepeat = AutoRepeatModeOff;
    status = (*(s->write)) (dptr->file_ds, _XChangeFeedbackControl, &kctl);
    kctl.class = KbdFeedbackClass;

    kctl.class = KbdFeedbackClass;
    kctl.click = 100;
    status = (*(s->write)) (dptr->file_ds, _XChangeFeedbackControl, &kctl);
    kctl.bell_pitch = 100;
    kctl.autoRepeat = AutoRepeatModeDefault;
    kctl.leds = 0;
    status = (*(s->write)) (dptr->file_ds, _XChangeFeedbackControl, &kctl);
    kctl.autoRepeat = AutoRepeatModeOn;
    status = (*(s->write)) (dptr->file_ds, _XChangeFeedbackControl, &kctl);
    kctl.autoRepeat = -2;
    status = (*(s->write)) (dptr->file_ds, _XChangeFeedbackControl, &kctl);

    /* Test XBell                  */

    bctl.class = BellFeedbackClass;
    bctl.bell_percent = 100;
    status = (*(s->write)) (dptr->file_ds, _XBell, &bctl);
    kctl.click = 0;
    status = (*(s->write)) (dptr->file_ds, _XBell, &kctl);

    /* Test XSetDeviceValuators    */

    vctl.first_valuator = 0;
    vctl.num_valuators = 0;
    vctl.valuators = NULL;
    status = (*(s->write)) (dptr->file_ds, _XSetDeviceValuators, &vctl);

    /* Test XChangeDeviceControl   */

    rctl.first_valuator = 0;
    rctl.num_valuators = 0;
    rctl.resolutions = NULL;
    status = (*(s->write)) (dptr->file_ds, _XChangeDeviceControl, &rctl);

    /* Test XSetDeviceMode         */

    status = (*(s->write)) (dptr->file_ds, _XSetDeviceMode, Absolute);
    status = (*(s->write)) (dptr->file_ds, _XSetDeviceMode, Relative);
    (*(s->close)) (dptr->file_ds);
    status = (*(s->write)) (dptr->file_ds, _XSetDeviceMode, Relative);
    (*(s->configure)) (dptr);

    status = (*(s->write)) (dptr->file_ds, -1, &kctl);
    }

/***********************************************************
 *  GetPty
 *
 *  Search through all of the ptys looking for one that 
 *  can be opened on the master side.  Return when one
 *  is successfully opened.
 *
 *  On the master side, ptys have names in the range
 *  /dev/ptyp0 ... /dev/ptyvf.  The last digit varies from
 *  0 ... f, and the last alphabetic character varies from
 *  p ... v.  Therefore, there are 16 * 7 = 112 ptys.
 *
 *  The names on the slave side are the same as the master
 *  side, except that they are /dev/tty rather than /dev/pty.
 ***********************************************************/

#define NUMPTYS		112

int 
GetPty(SlaveName)
char * SlaveName;
{
    int NextPty = 0;
    char MasterName[50];
    int Fd;

    for (NextPty=0; NextPty<NUMPTYS; NextPty++)
    {
        sprintf (SlaveName,"/dev/pty/tty%c%x",NextPty/16+'p',NextPty%16);
        sprintf (MasterName,"/dev/ptym/pty%c%x",NextPty/16+'p',NextPty%16);
        if (( Fd = open(MasterName,O_RDWR | O_NDELAY )) != -1)
	{
	   SelectMask |= 1 << Fd; 
	   MaxOpen = max( MaxOpen, Fd );
	   BaseFiledescriptor = min( BaseFiledescriptor, Fd );
	   return (Fd);
	}
    }
    printf( "GetPty: cannot open any of the pty's.\n");
    exit( 1 );
}


/***********************************************************
 *  SetupDevice
 *
 * This routine finds an available pty, then links
 * the pty symbolically to an X0devices name in /tmp.
 *
 * The name is created in /tmp, using the hostname and the number
 * of the HIL device being emulated.
 ***********************************************************/

void
SetupDevice( hilnum )
int hilnum;
{
    int rc;
    struct  request_info iostuff;
    char TruncHost[1024];

    strcpy (TruncHost, Hostname);
    strtok (TruncHost, ".");
    sprintf( hil[hilnum].PtyName, "%s%s:hil%d", PTY_DIR, TruncHost, hilnum+1 );
    /*
     * Find an available pty 
     */
    hil[hilnum].Pty = GetPty (hil[hilnum].SlaveName);

    /* Trap ioctls on pty.  These are HILID, HILDKR and HILER1  */

    rc = 1;			/* Trap ioctls on pty */
    if ( ioctl( hil[hilnum].Pty, TIOCTRAP, &rc) == -1)
    {
	perror( "SetupDevice: couldn't TIOCTRP pty");
	exit( 1 );
    }

    rc = 1;			/* Disable termio processing */
    if ( ioctl( hil[hilnum].Pty, TIOCTTY, &rc) == -1)
    {
	perror( "SetupDevice: couldn't TIOCTTY pty");
	exit( 1 );
    }

    /*
     * block client signals while the ioctl is being handled 
     */
    if (ioctl( hil[hilnum].Pty, TIOCSIGMODE, TIOCSIGBLOCK) == -1)
    {
	perror( "SetupDevice: couldn't TIOCSIGBLOCK pty");
	exit( 1 );
    }
    /*
     * clean up any old symlinks and link to our pty
     */
    unlink( hil[hilnum].PtyName);
    if ( symlink (hil[hilnum].SlaveName, hil[hilnum].PtyName )) 
    {
	perror( "SetupDevice: couldn't link pty");
	printf( "SetupDevice:    ln %s %s\n",
	   hil[hilnum].SlaveName, hil[hilnum].PtyName);
	exit( 1 );
    }
}

struct HilDeviceInfo * FindHilInfo();

/***********************************************************
 *  FindHilInfo
 *
 *      Search the HIL device information table for
 *  information about the given device type.
 *
 ***********************************************************/
struct HilDeviceInfo *
FindHilInfo( Type )
unsigned char Type;
{
     struct HilDeviceInfo * pInfo = DeviceTable;

     while ( pInfo->id != 0 )
     {
	 if ( pInfo->id == Type )
	     return( pInfo );
	 pInfo++;
     }
     printf( "Hil.c: FindHilInfo: requested type %x not in DeviceTable.\n", Type);
     exit( 1 );
}

/***********************************************************
 *  SetupHILDevices
 *
 *     Loop through the array that defines the 
 * HIL devices to be emulated.  For each device,
 * obtain a pty to use to emulate the device.
 * Then associate a HIL device information table entry with
 * the HIL loop position.
 *
 ***********************************************************/

SetupHILDevices( LoopDefinitionArray )
unsigned char LoopDefinitionArray[];
{
    int i;
    int Type;
    for ( i = 0; i < NUM_DEVS; i++ )
    {
	Type = LoopDefinitionArray[i];
	if ( Type == 0 )
	    break;
	SetupDevice( i );
	hil[i].pHilDeviceInfo = FindHilInfo( Type );
    }
}

/***********************************************************
 *  LogHilCommand
 *
 *     Log the command for the particular device.
 *  This log can later be scanned to see if a particular
 *  command was sent by the server.
 *
 *     The log is kept in a singly linked list.   The 
 *  initial pointer to the list is indexed by the hil number,
 *  so as to keep the logs for the devices distinct.
 *
 ***********************************************************/

LogHilCommand( hilnum, Command )
int hilnum;
int Command;
{
	if ( hil[ hilnum ].pLast == NULL )
	{
	    hil[ hilnum ].pLast = ( struct HilCommandLog * )
				  malloc( sizeof( struct HilCommandLog ));
	    hil[ hilnum ].pLast->pNext = 0;
	    hil[ hilnum ].pLast->Command = Command;

	    hil[ hilnum ].pNext = hil[ hilnum ].pLast;
	}
	else
	{
	    struct HilCommandLog * pTemp;

	    pTemp = hil[ hilnum ].pLast;
	    hil[ hilnum ].pLast = ( struct HilCommandLog * )
			           malloc( sizeof( struct HilCommandLog ));
	    hil[ hilnum ].pLast->Command = Command;
	    hil[ hilnum ].pLast->pNext = 0;
	    pTemp->pNext = hil[ hilnum ].pLast;
	}
}

/*************************************************************
 * PrintHilCommand
 *
 *   This function prints the mnemonic for an HIL command
 * or the several standard PTY commands.
 *
 *************************************************************/

void
PrintHilCommand( hilnum, cmd )
int hilnum;
int cmd;
{
    struct _hilCommands *hp;

    for ( hp = hilArray; hp->HilCommand; hp++ )
    {
        if ( hp->HilCommand == cmd )
        {
	    printf("hil: %d command: 0x%4X %s\n",
		hilnum, cmd, hp->HilCommandName );
	    break;
	}
    }
    if ( hp->HilCommand == 0 )
	printf("hil: %d command: 0x%4X %s\n", hilnum, cmd, "UNKNOWN" );

}

/***********************************************************
 *  HandShakeDevice
 *
 * Handle an ioctl request from the driver.
 * Use ioctl to send back the correct data.
 *
 ***********************************************************/

struct mydata foo;

void
HandshakeDevice( hilnum )
int hilnum;
{
    struct  request_info iostuff;
    unsigned char * pIdInfo;
    /*
     * find out what the HIL request is
     */
    ioctl (hil[hilnum].Pty, TIOCREQGET, &iostuff);

    /*PrintHilCommand( hilnum, iostuff.request ); */
    LogHilCommand( hilnum, iostuff.request );

    switch( iostuff.request )
    {
    case HILID:

	if (iostuff.argset == 0 )
	{
	    printf( "HandshakeDevice: argset is zero on HILID.\n");
	    exit( 1 );
	}

	if (hil[hilnum].pHilDeviceInfo->id == X_ERROR_DEVICE)
	    iostuff.return_value = -1;
	pIdInfo = hil[ hilnum ].pHilDeviceInfo->pHilIdString;
	ioctl (hil[hilnum].Pty, iostuff.argset, pIdInfo );


	break;
    case TCGETA:

	if (iostuff.argset == 0 )
	{
	    printf( "HandshakeDevice: argset is zero on TCGETA.\n");
	    exit( 1 );
	}

	ioctl (hil[hilnum].Pty, iostuff.argset, tcget_info );
	break;
    case TIOCOPEN:
    case TIOCCLOSE:
    case HILER1:
    case HILER2:
    case HILDKR:
    case HILP:
    case HILP1:
    case HILP2:
    case HILP3:
    case HILP4:
    case HILP5:
    case HILP6:
    case HILP7:
    case HILA:
    case HILA1:
    case HILA2:
    case HILA3:
    case HILA4:
    case HILA5:
    case HILA6:
    case HILA7:
	break;
    case TPSENDD:
	ioctl (hil[hilnum].Pty, iostuff.argget, &foo);
	send_test_data (hil[hilnum].Pty, foo.valid, foo.type);
	break;
    case TPSTOPIT:
	CloseAllHilDevices();
	exit(1);
	break;
    default:
	    printf( "HandshakeDevice: default command %x.\n",iostuff.request);
    }
    if (ioctl (hil[hilnum].Pty, TIOCREQSET, &iostuff) < 0 )
    {
      perror( "HandshakeDevice: error on TIOCREQSET" );
      printf( "HandshakeDevice: errno %d\n", errno);
      exit( 1 );
    }
}

/***********************************************************
 *  CloseAllHilDevices
 *
 *   This routine closes all the opened HIL devices
 ***********************************************************/

void
CloseAllHilDevices()
{
    int i;

    for ( i = 0; i < NUM_DEVS; i++ )
	if ( hil[ i ].Pty )
	{
            close( hil[ i ].Pty );
	    hil[ i ].Pty = 0;
	    unlink( hil[ i ].PtyName);
	}
}

/*************************************************************
 * SignalHandler
 *
 *   Handle receipt of a signal.  In current use, this generally
 * means that this process has been hit by kfork.  Kfork 
 * sends SIGTERM by default.
 *
 * ACTION:  try to shut down the server, if it is connected,
 * by sending control-shift-reset.  Then close all the pty's,
 * unlink them, shutdown and close the sockets, and exit 
 * gracefully.
 *************************************************************/

void
SignalHandler()
{
    CloseAllHilDevices();
    exit( 0 );
}

/*
 * int usleep(unsigned long microseconds, int nfds, int *readmask)
 *	This is a microsecond "sleep" routine. It uses the select(2) system
 *      call to delay for the desired number of micro-seconds.
 *
 * Arguments:
 *	unsigned long microseconds	Number of microseconds to pause.
 *
 * Return value:
 *	int		0 on successful completion, -1 otherwise.
 *
 * Side effects:
 *	Program pause based upon argument value.
 *
 * Calls:
 *	select(2).
 */
int
usleep(microseconds, nfds, readmask)
register unsigned long microseconds;
register int nfds, *readmask;

{
    register unsigned int seconds, usec;
    int writefds, exceptfds;
    struct timeval timer;

    writefds = exceptfds = 0;

    /* Convert to values select() understands */
    seconds = microseconds / (unsigned long)1000000;
    usec = microseconds % (unsigned long)1000000;

    timer.tv_sec = seconds;
    timer.tv_usec = usec;

    return (select(nfds, readmask, &writefds, &exceptfds, &timer));
}

send_test_data (pty, valid, type)
    unsigned char type;
    {
    switch (type)
	{
	case X_KB_US:
	    if (valid)
		write(pty, valid_key_data, sizeof(valid_key_data));
	    else
		write(pty, bogus_key_data, sizeof(bogus_key_data));
	    break;
	case X_MOUSE_3B:
	    if (valid)
		write(pty, valid_mouse_data, sizeof(valid_mouse_data));
	    else
		write(pty, bogus_mouse_data, sizeof(bogus_mouse_data));
	    break;
	case X_MOUSE_2B:
	    if (valid)
		write(pty, valid_mouse_data2, sizeof(valid_mouse_data2));
	    else
		write(pty, bogus_mouse_data2, sizeof(bogus_mouse_data2));
	    break;
	case X_TABLET_ASIZE:
	case X_TABLET_BSIZE:
	    if (valid)
		write(pty, valid_tablet_data, sizeof(valid_tablet_data));
	    else
		write(pty, bogus_tablet_data, sizeof(bogus_tablet_data));
	    break;
	case X_QUADRATURE:
	    if (valid)
		write(pty, valid_quad_data, sizeof(valid_quad_data));
	    else
		write(pty, bogus_quad_data, sizeof(bogus_quad_data));
	    break;
	case X_TOUCHSCREEN:
	    if (valid)
		write(pty, valid_mouse_data, sizeof(valid_mouse_data));
	    else
		write(pty, bogus_mouse_data, sizeof(bogus_mouse_data));
	    break;
	case X_BARCODE:
	    if (valid)
		write(pty, valid_barcode_data, sizeof(valid_barcode_data));
	    else
		write(pty, bogus_barcode_data, sizeof(bogus_barcode_data));
	    break;
	case X_NINE_KNOB:
	    if (valid)
		write(pty, valid_9knob_data, sizeof(valid_9knob_data));
	    else
		write(pty, bogus_9knob_data, sizeof(bogus_9knob_data));
	    break;
	case X_ONE_KNOB:
	    if (valid)
		write(pty, valid_1knob_data, sizeof(valid_1knob_data));
	    else
		write(pty, bogus_1knob_data, sizeof(bogus_1knob_data));
	    break;
	case X_ONE_AXIS:
	    if (valid)
		write(pty, valid_one_axes_data, sizeof(valid_one_axes_data));
	    else
		write(pty, bogus_one_axes_data, sizeof(bogus_one_axes_data));
	    break;
	case X_NO_AXES:
	    if (valid)
		write(pty, valid_no_axes_data, sizeof(valid_no_axes_data));
	    else
		write(pty, bogus_no_axes_data, sizeof(bogus_no_axes_data));
	    break;
	case X_THREE_AXES:
	    if (valid)
		write(pty, valid_three_axes_data, sizeof(valid_three_axes_data));
	    else
		write(pty, bogus_three_axes_data, sizeof(bogus_three_axes_data));
	    break;
        }
    }
#endif /* DEBUG */
