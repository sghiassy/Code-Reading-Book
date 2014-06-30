/* $XConsortium: hp7lc2k.c /main/2 1996/12/04 10:23:28 lehors $ */
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

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "ps2io.h"
#include <X11/X.h>
#include <X11/extensions/XI.h>
#include "x_serialdrv.h"

/**************************************************************************
 *
 * hp7lc2k.c - X server input device driver.
 *
 * This driver provides support for PS2 keyboards attached to an HP9000700LC2
 * via the internal mini-DIN interface.
 *
 * The HP-UX 9.03 operating system is required by this driver.
 *
 * If another device is used as the X keyboard, the PS2 keyboard attached to
 * the mini-DIN interface can be accessed via the X input device extension
 * by adding the following lines to the /usr/lib/X11/X0devices file:
 *
 * Begin_Device_Description
 * Name	hp7lc2k.sl     		# PS2 DIN keyboard via mini-DIN interface
 * Use	Extension		# access via input extension
 * Path	/dev/ps2kbd		# use device special file /dev/ps2kbd
 * End_Device_Description
 *
 * For documentation on using serial input devices with X, please refer
 * to the manual "Using the X Window System", Chapter 3 "Preliminary
 * Configuration", in the section titled "Using Special Input Devices".
 *
 * Sample entries for the /usr/lib/X11/X*devices file can be found in 
 * /etc/newconfig/X0devices.
 *
 */ 

#define	BUFRSIZ	2048
#define READSIZ 2000

static int hp7lc2k_left=READSIZ, hp7lc2k_datacnt=0;
static unsigned char hp7lc2k_buf[BUFRSIZ];
static unsigned char *hp7lc2k_data_ptr = hp7lc2k_buf;
typedef struct ps2_4  ps2_buffer_t;

/**************************************************************************
 *
 * Configure the PS/2 keyboard. Use scancode set 3, Turn indicator LEDs
 * off, set all keys to typematic make/break, and set autorepeat to ~30/sec
 * and delay to 250 mSec.
 */

static int
keybd_configure (
    int fd)
    {
    ps2_buffer_t kbdbuf;

    /* use scancode set 3 */

    kbdbuf.b[0] = SCANCODE_3;
    ioctl (fd, PS2_SCANCODE, &kbdbuf);

    /* turn LEDs off */

    kbdbuf.b[0] = 0;
    ioctl (fd, PS2_INDICATORS, &kbdbuf);

    /* set all keys to typematic and make/break  */

    ioctl (fd, PS2_ALL_TMAT_MKBRK, &kbdbuf);

    /* set the autorepeat rate to the maximum (~30/sec) and the delay to the
       minimum (250ms) */

    kbdbuf.b[0] = 0x00;
    ioctl (fd, PS2_RATEDELAY, &kbdbuf);

    } /* end keybd_configure() */

/**************************************************************************
 *
 * This routine is called by the X server to open and configure the device.
 * It is passed a pointer to an HPDeviceHeader struct (defined in 
 * x_serialdrv.h).  That struct points to the path name of the device file
 * to which the input device is connected.  The path is specified in the
 * X*devices configuration file described above.
 *
 * This routine must open the device file and configure the serial port.
 * It must initialize fields in the HPInputDeviceHeader struct including
 * the device file descriptor, name by which the device will be known to
 * X, the number of keys, buttons, and axes on the device, the keymap name
 * and file, if the device has keys, and a flags field that specifies the
 * size of the data returned by the device.  See the serial input device
 * specification described above for more details.
 *
 */

static int
hp7lc2kconfigure (
    HPInputDeviceHeader *d)
    {
    int ret;
    ps2_buffer_t kbdbuf;
    static unsigned char num_leds=3;
    static char hp7lc2k_namebuf[128];
    FILE *fp;

    d->file_ds = open (d->path, O_RDWR | O_NDELAY);/* non-block read */
    if (d->file_ds < 0) 
	return(INIT_FAILURE);

    keybd_configure (d->file_ds);

    /* get the keyboard nationality, or default to US_English */

    d->keymap_name = "PS2_DIN_US_English"; /* default keymap name     */
    fp = fopen (X_KEYMAP_NAME, "r");	 /* defined by x_serialdrv.h  */
    if (fp && fscanf (fp, "%s", hp7lc2k_namebuf))
	d->keymap_name = hp7lc2k_namebuf;/* international keymap name */
    fclose(fp);

    d->x_name = "PS2_DIN_KEYBOARD";  	 /* device name for X server*/
    d->keymap_file = "";	 	 /* keymap file for X server*/
    d->flags = DATA_IS_16_BITS;          /* size of data for device */
    d->num_keys = 101;	                 /* tell server we have keys*/
    d->min_kcode = 0;	                 /* server gets from keymap */
    d->max_kcode = 0;	                 /* server gets from keymap */
    d->num_ledf = 1;	                 /* number of led feedbacks */
    d->ledf = &num_leds;	         /* number of leds          */
    d->reset = 107;			 /* X server reset keycode  */
    d->reset_mods = ShiftMask | ControlMask; /* X server reset modifiers*/

    return (INIT_SUCCESS);
    }

/**************************************************************************
 *
 * Get more data and crunch the buffer if we're getting near the end.
 *
 */

static int get_more_data(
    int fd)
    {
    int count;

    /* If we don't already have at least 1 byte of data, 
     * we need to read some.  If we're getting near the end
     * of the buffer, copy the leftover data to the beginning
     * of the buffer.
     */

    if (hp7lc2k_data_ptr - hp7lc2k_buf > hp7lc2k_datacnt)
	{
	bcopy (hp7lc2k_data_ptr, hp7lc2k_buf, hp7lc2k_datacnt);
	hp7lc2k_data_ptr = hp7lc2k_buf;
	hp7lc2k_left = READSIZ - hp7lc2k_datacnt;
	}
    count = read (fd, hp7lc2k_data_ptr + hp7lc2k_datacnt, hp7lc2k_left);

    if (count >0)
	{
        hp7lc2k_datacnt += count;
        hp7lc2k_left -= count;
	}

    if (hp7lc2k_datacnt < 1)
	return(READ_FAILURE);

    return(READ_SUCCESS);
    }

/**************************************************************************
 *
 * This entry point is called when the X server detects that there is data
 * available to be read from the device.  This routine will be called
 * repeatedly until it returns READ_FAILURE.  It is expected to return
 * one "event" for each successful call.  An "event" is one key press or
 * release, one button press or release, one change in axis data, or one
 * change of proximity state.  One change in axis data can be reported
 * along with a button or proximity change.
 *
 */

static int
hp7lc2kread (
    int fd, unsigned char *data, unsigned char *data_type, int *pending)
    {
    struct timeval timeout;
    unsigned short code, keycode;
    int count;

    get_more_data(fd);
    if (hp7lc2k_datacnt < 1)
	 return(READ_FAILURE);

    /* PS2 keyboards return 1-byte scancode for keypresses, and a 1 byte
     * constant followed by that same scancode for keyreleases.
     * The X server expects key releases to have the value of the 
     * key press + 1, so we must multiply key codes by 2, and add 1 to the
     * key releases.  
     *
     * The minimum key code returned by the kernel is 7, but the minimum
     * expected by the keymap is 16.  Since the X server will also add 8 to 
     * the keycode after dividing by 2, to avoid the codes reserved by X for 
     * mouse buttons, we must add 1 before multiplying.
     */
    
    if (*hp7lc2k_data_ptr == 0xF0) {	/* this is a key release */
	if (hp7lc2k_datacnt == 1){	/* code hasn't arrived   */
	    timeout.tv_sec = 0;
	    timeout.tv_usec = 25000; 	/* wait interval */
	    select(0, (int *)NULL, (int *)NULL, (int *)NULL, &timeout);
	    get_more_data(fd);
	    if (hp7lc2k_datacnt < 1)
		return(READ_FAILURE);
	    }
	code = *(hp7lc2k_data_ptr+1);  /* keycode is second byte  */
	keycode = (code + 1) * 2 + 1;  /* add 1, double it, add 1 */
	hp7lc2k_datacnt-=2;
	hp7lc2k_data_ptr+=2;
    }
    else {
	code = *hp7lc2k_data_ptr;
	keycode = (code + 1) * 2; 	/* add 1, double it   */
	hp7lc2k_datacnt-=1;
	hp7lc2k_data_ptr+=1;
    }
    
    if (code == 0xAA) {			/* keyboard has reset itself */
	keybd_configure (fd);		/* reconfigure the keyboard  */
	hp7lc2k_datacnt=0;		/* reset the data count      */
	hp7lc2k_data_ptr = hp7lc2k_buf; /* reset the data pointer    */
        return(READ_FAILURE);
    }
    *data++ = keycode;
    *data = keycode >> 8;
    *data_type = KEY_DATA;                /* tell X it's key data */
    *pending = 2;                        /* two bytes are returned */
    return(READ_SUCCESS);
    }

/**************************************************************************
 *
 * This routine is called by the X server to write to the keyboard.
 * It is called when an X request is made that causes a write to an input 
 * device.
 *
 * See the file x_serialdrv.h for the format of the data for each request.
 *
 * The only one we will support is _XChangeFeedbackControl, which is used
 * to turn on LEDs.
 *
 */

static int
hp7lc2kwrite (
    int fd, int request, char *data)
    {
    int i;
    HPKeyboardFeedbackControl *ctrl;
    ps2_buffer_t kbdbuf;

    switch (request)
	{
	case _XChangeFeedbackControl:
	    ctrl = (HPKeyboardFeedbackControl *) data;

	    if (ctrl->class != KbdFeedbackClass)
		return(WRITE_FAILURE);
	    kbdbuf.b[0] = 0;
	    if (ctrl->leds & SCROLLLOCK_LED) {
		kbdbuf.b[0] |= SCROLL_LED;
		}
	    if (ctrl->leds & NUMLOCK_LED) {
		kbdbuf.b[0] |= NUM_LED;
		}
	    if (ctrl->leds & CAPSLOCK_LED) { 
		kbdbuf.b[0] |= CAPS_LED;
		}
	    ioctl (fd, PS2_INDICATORS, &kbdbuf);

	    if (ctrl->autoRepeat == AutoRepeatModeOff)
		ioctl (fd, PS2_ALL_MKBRK, &kbdbuf);
	    else
		ioctl (fd, PS2_ALL_TMAT_MKBRK, &kbdbuf);
	    break;
	case _XSetDeviceValuators:
	case _XChangeDeviceControl:
	case _XSetDeviceMode:
	default:
		return(WRITE_FAILURE);
	    break;
	}
    return(WRITE_SUCCESS);
    }

/**************************************************************************
 *
 * This routine is called by the X server to close an input device.
 *
 */

static int
hp7lc2kclose (
    int fd)
    {
    close (fd);
    return (CLOSE_SUCCESS);
    }

/**************************************************************************
 *
 * This routine is called to initialize the entry point vector for this
 * serial input device driver.
 *
 */

int
hp7lc2k_Init(
    SerialProcs *serialproc)
    {
    serialproc->configure = hp7lc2kconfigure;/* routine to init device   */
    serialproc->read = hp7lc2kread;          /* routine to read from dev */
    serialproc->write = hp7lc2kwrite;        /* routine to write to dev  */
    serialproc->close = hp7lc2kclose;        /* routine to close device  */
    return INIT_SUCCESS;                  /* indicate success         */
    }

#ifdef DEBUG
main()
    {
    int fd;

    makedata();				/* make the test data	*/
    fd = open ("data", O_RDWR);
    process_test_data(fd);
    close(fd);
    }

/* The PS2 keyboard returns one byte per key press and two bytes per key release.
 * The keycode is the same in each case.
 *
 * Byte 0:
 *		scancode for key press, 0xF0 for key release.
 * Byte 1:
 *		scancode for key release.
 */

makedata ()
    {
    int i, fd;
    unsigned char buf[3];

    fd = creat("data", 0777);
    for (i=7; i<256; i++)
	{
	buf[0]=i;
	buf[1]=0xF0;
	buf[2]=i;
	write (fd,buf,3);
	}
    close(fd);
    }

process_test_data(fd)
    {
    unsigned char data[32], data_type;
    int pending;

    hp7lc2kread (fd, data, &data_type, &pending);
    while (pending > 0)
	{
	printf ("%d bytes returned, type is %x, data is %x %x\n", pending, data_type, 
	    data[0], data[1]);
	pending = 0;
	data[0] = data[1] = 0;
        hp7lc2kread (fd, data, &data_type, &pending);
	}
    }
#endif /* DEBUG */
