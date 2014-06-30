/* $XConsortium: hp7lc2m.c /main/2 1996/12/04 10:23:42 lehors $ */
/************************************************************

Copyright (c) 1992 by Hewlett-Packard Company, Palo Alto, California

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
#include <X11/extensions/XI.h>
#include "x_serialdrv.h"

/**************************************************************************
 *
 * hp7lc2m.c - X server input device driver.
 *
 * This driver provides support for PS2 mice attached to an HP9000700LC2
 * via the internal mini-DIN interface.
 *
 * The HP-UX 9.03 operating system is required by this driver.
 *
 * If another device is used as the X pointer, the PS2 mouse attached to
 * the mini-DIN interface can be accessed via the X input device extension
 * by adding the following lines to the /usr/lib/X11/X0devices file:
 *
 * Begin_Device_Description
 * Name	hp7lc2m.sl     		# PS2 DIN keyboard via mini-DIN interface
 * Use	Extension		# access via input extension
 * Path	/dev/ps2mouse		# use device special file /dev/ps2mouse
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
#define PKT_SIZ 3
#define BUTTON1_AND_2	0x05
#define BUTTON2_AND_3	0x06
#define BUTTON1_AND_3	0x03
#define BUTTON_BITS	0x07
#define ONE_BUTTON_DOWN	 -1
#define ILLEGAL_COMBO  	 -2

static int hp7lc2m_left=READSIZ, hp7lc2m_datacnt=0;
static unsigned char hp7lc2m_buf[BUFRSIZ];
static unsigned char *hp7lc2m_data_ptr = hp7lc2m_buf;
static unsigned int hp7lc2m_buttonmask;
static unsigned int hp7lc2m_chording_button = 0;
static unsigned int hp7lc2m_chording_interval = 0;
static int process_button(), do_button_chording(), get_more_data(), check_combo ();
typedef struct ps2_4 ps2_buffer_t;

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
hp7lc2mconfigure (
    HPInputDeviceHeader *d)
    {
    int ret;
    ps2_buffer_t mousebuf;

    d->file_ds = open (d->path, O_RDWR | O_NDELAY);/* non-block read */
    if (d->file_ds < 0) 
	return(INIT_FAILURE);

    if (d->button_chording)
	hp7lc2m_chording_interval = d->button_chording;

    /* set the sample rate to 100 reports/second max */

    ioctl (d->file_ds, PS2_DISABLE);
    ioctl (d->file_ds, PS2_RESET);
    mousebuf.b[0] = SAMPLE_60;
    ioctl (d->file_ds, PS2_SAMPLERATE, &mousebuf);

    /* put the mouse into stream mode  */

    mousebuf.b[0] = 0;
    ioctl (d->file_ds, PS2_STREAMMODE, &mousebuf);

    /* set the mouse resolution to 8 counts/mm */

    mousebuf.b[0] = RES_4;
    ioctl (d->file_ds, PS2_RESOLUTION, &mousebuf);
    ioctl (d->file_ds, PS2_ENABLE);

    d->x_name = "PS2_DIN_MOUSE";  	/* device name for X server*/
    d->flags = DATA_IS_8_BITS;         	/* size of data for device */
    d->ax_num = 2;			/* two axes of motion      */
    if (d->button_chording)
	d->num_buttons = 5;		/* emulate 5 buttons       */
    else
	d->num_buttons = 3;		/* support 3 buttons       */
    d->num_keys =0;     		/* no keys                 */
    d->resolution = 80;			/* resolution in counts/cm */
    d->max_x = 255;        		/* maximum x value in counts */
    d->max_y = 255;			/* maximum y value in counts */

    return (INIT_SUCCESS);
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
hp7lc2mread (
    int fd, unsigned char *data, unsigned char *data_type, int *pending)
    {
    int button, data_read=0;
    unsigned int buttonmask;
    int count, x, y;

    /* If we don't already have at least PKT_SIZ bytes of data, 
     * we need to read some.  If we're getting near the end
     * of the buffer, copy the leftover data to the beginning
     * of the buffer.
     */

    if (hp7lc2m_datacnt < PKT_SIZ && (get_more_data (fd) == READ_FAILURE))
	 return(READ_FAILURE);

    /* The PS2 mouse returns PKT_SIZ bytes for each movement of the mouse or
     * button press or release.  The format of the bytes is as follows:
     *
     * Byte 0:
     * 		Bit 7	Y data overflow		(1 = overflow)
     * 		Bit 6	X data overflow		(1 = overflow)
     * 		Bit 5	Y data sign		(1 = negative)
     * 		Bit 4	X data sign		(1 = negative)
     * 		Bit 3	Not used		(always 1)
     * 		Bit 2	Center button		(1 = depressed)
     * 		Bit 1	Right button		(1 = depressed)
     * 		Bit 0	Left button		(1 = depressed)
     * Byte 1:
     *		X coordinate data byte		(2's compliment)
     * Byte 2:
     *		Y coordinate data byte		(2's compliment)
     */
    
    x = (int) hp7lc2m_data_ptr[1];
    y = -((int) hp7lc2m_data_ptr[2]);

    if (x!=0 || y != 0) {
	*data_type |= MOTION_DATA;
	data[0] = x;
	data[1] = y;
	data_read = 2;
        hp7lc2m_data_ptr[1] = 0;	/* clear motion for reprocessing case */
        hp7lc2m_data_ptr[2] = 0;	/* clear data for reprocessing case */
	}

    /* Check to see if a button has changed state */
	buttonmask = (u_char) (hp7lc2m_data_ptr[0] & BUTTON_BITS);
	if (buttonmask != hp7lc2m_buttonmask) {
	    if (hp7lc2m_chording_interval) {
		if ((button = do_button_chording(fd, &buttonmask))>=0) {
		    *data_type |= BUTTON_DATA;
		    data[data_read++] = button;
		}
	    }
	    else {
		button = process_button(buttonmask);
		*data_type |= BUTTON_DATA;
		data[data_read++] = button;
	    }
	}
    if (hp7lc2m_buttonmask == buttonmask)  /* no leftover buttons */
	{
        hp7lc2m_datacnt -= PKT_SIZ;
        hp7lc2m_data_ptr += PKT_SIZ;
	}
    *pending += data_read;
    return(READ_SUCCESS);
    }

/**************************************************************************
 *
 * Process a button from the current data packet.
 *
 */

#define hp7lc2_BUTTON1	0x01
#define hp7lc2_BUTTON2	0x04
#define hp7lc2_BUTTON3	0x02
#define NUM_BUTTONS	3

static int process_button(
    unsigned int buttonmask)
    {
    int i, button, bit, down, up;

    button = 1;
    up = ~buttonmask & hp7lc2m_buttonmask;
    for (i=0; i<NUM_BUTTONS; i++)
	{
	bit = (1 << i);
	if (bit & up)
	    {
	    switch (bit)
		{
	        case hp7lc2_BUTTON3:
		    button += 2;
	        case hp7lc2_BUTTON2:
		    button += 2;
		    break;
		}
	    hp7lc2m_buttonmask = (hp7lc2m_buttonmask & ~bit);
            return(button);
	    }
	}

    down = buttonmask & ~hp7lc2m_buttonmask;
    button = 0;
    for (i=0; i<NUM_BUTTONS; i++)
	{
	bit = (1 << i);
	if (bit & down)
	    {
	    switch (bit)
		{
	        case hp7lc2_BUTTON3:
		    button += 2;
	        case hp7lc2_BUTTON2:
		    button += 2;
		    break;
		}
	    hp7lc2m_buttonmask = (hp7lc2m_buttonmask | bit);
            return(button);
	    }
	}
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

    if (hp7lc2m_data_ptr - hp7lc2m_buf > hp7lc2m_datacnt)
	{
	bcopy (hp7lc2m_data_ptr, hp7lc2m_buf, hp7lc2m_datacnt);
	hp7lc2m_data_ptr = hp7lc2m_buf;
	hp7lc2m_left = READSIZ - hp7lc2m_datacnt;
	}
    count = read (fd, hp7lc2m_data_ptr + hp7lc2m_datacnt, hp7lc2m_left);

    if (count >0)
	{
        hp7lc2m_datacnt += count;
        hp7lc2m_left -= count;
	}

    if (hp7lc2m_datacnt < PKT_SIZ)
	return(READ_FAILURE);

    return(READ_SUCCESS);
    }

/**************************************************************************
 *
 * Handle button chording.
 *
 */

#define BUTTON1_DOWN 0
#define BUTTON1_UP   1
#define BUTTON2_DOWN 2
#define BUTTON2_UP   3
#define BUTTON3_DOWN 4
#define BUTTON3_UP   5
#define BUTTON4_DOWN 6
#define BUTTON4_UP   7
#define BUTTON5_DOWN 8
#define BUTTON5_UP   9

int ignore1, ignore2, ignorecnt;

static int  do_button_chording(
    int fd,
    unsigned int *buttonmask)
    {
    int button, ret;
    unsigned char *tptr;
    unsigned int tmask;
    struct timeval timeout;

    if (hp7lc2m_chording_button)	/* waiting for chorded button up */
	{
        button = process_button(*buttonmask); /* process one button       */
	if (button % 2)			/* odd number means  button up   */
	    if (button == ignore1)      /* up transition of chorded pair */
		{
		ignore1= -1; ignorecnt--; /* dont ignore it next time    */
		if (ignorecnt == 0)		/* done ignoring buttons */
	    	    {
	    	    button = hp7lc2m_chording_button; /* send chorded up */
	    	    hp7lc2m_chording_button = 0; /* clear saved button   */
	    	    return(button);      /* return chorded button        */
	    	    }
		else
		    return(-1);		 /* we ignored this one          */
		}
	    else if (button == ignore2)  /* other button of chorded pair */
		{
		ignore2= -1; ignorecnt--;/* dont ignore it next time     */
		if (ignorecnt == 0)	 /* done ignoring buttons        */
	    	    {
	    	    button = hp7lc2m_chording_button; /* send chorded up */
	    	    hp7lc2m_chording_button = 0; /* clear saved button   */
	    	    return(button);      /* return chorded button        */
	    	    }
		else
		    return(-1);		 /* we ignored this one          */
		}
	    else
		return(button);		/* not a button to ignore        */
	else
	    return(button);		/* button down - dont ignore     */
	}
    else if (hp7lc2m_buttonmask==0)	/* check if chording necessary   */
	{
	ret = check_combo (*buttonmask);
	if (ret == ILLEGAL_COMBO)	/* illegal combination		 */
	    return (process_button(*buttonmask)); /* process a button    */
	else if (ret != ONE_BUTTON_DOWN)/* we already chorded 		 */
	    return (ret); 		/* return chorded button 	 */

	timeout.tv_sec = 0;
	timeout.tv_usec = hp7lc2m_chording_interval * 1000; /* wait interval */
	select(0, (int *)NULL, (int *)NULL, (int *)NULL, &timeout);
	get_more_data(fd);

	for (tptr=hp7lc2m_data_ptr+PKT_SIZ; 
	     tptr<hp7lc2m_data_ptr+hp7lc2m_datacnt;
	     tptr +=PKT_SIZ)
	    {
#ifdef DEBUG
	    if (tptr > (hp7lc2m_data_ptr + (3 * PKT_SIZ)))
		break;
#endif
	    tmask = (u_char) (*tptr & BUTTON_BITS);
	    if (tmask != *buttonmask)		/* a button changed    */
		{
		if (!(tmask & *buttonmask))	/* a button went up    */
	    	    return (process_button(*buttonmask));
		if ((ret = check_combo (tmask)) > 0)
		    {
		    if (tmask == hp7lc2m_buttonmask)
			*buttonmask = hp7lc2m_buttonmask;
	     	    hp7lc2m_datacnt -= (tptr - hp7lc2m_data_ptr);
		    hp7lc2m_data_ptr = tptr;
	    	    return (ret);
		    }
		else
	    	    return (process_button(*buttonmask));
		}
	    }
	return (process_button(*buttonmask));
	}
    else			/* can't chord - button already down   */
	{
	return (process_button(*buttonmask));
	}
    }

/**************************************************************************
 *
 * Check to see if the current data packet indicates more than one button
 * is down.  If so, it is either a valid chording combingation or an
 * illegal chording combination.  If valid, remember which buttons we
 * need to ignore the first time they go up.  If invalid, we won't do
 * button chording.  If only one button is down, we need more data.
 */

static int check_combo (
    int buttonmask)
    {				
    if ((buttonmask & BUTTON1_AND_3) == BUTTON1_AND_3) /* illegal combo   */
	return (ILLEGAL_COMBO);
    else if ((buttonmask & BUTTON1_AND_2) == BUTTON1_AND_2)
	{
	process_button(buttonmask);		/* eat one button         */
	process_button(buttonmask);		/* eat the other button   */
        hp7lc2m_chording_button = BUTTON4_UP;   /* save for up transition */
	ignore1 = BUTTON1_UP;			/* ignore each button up  */
	ignore2 = BUTTON2_UP;
	ignorecnt = 2;
        return (BUTTON4_DOWN); 			/* send the chorded down  */
	}
    else if ((buttonmask & BUTTON2_AND_3) == BUTTON2_AND_3)
	{
	process_button(buttonmask);
	process_button(buttonmask);
        hp7lc2m_chording_button = BUTTON5_UP; 
	ignore2 = BUTTON2_UP;
	ignore1 = BUTTON3_UP;
	ignorecnt = 2;
        return (BUTTON5_DOWN); 
	}
    else
        return (ONE_BUTTON_DOWN);	/* only one button - need more data */
    }

/**************************************************************************
 *
 * This routine is called by the X server to write to the mouse.
 * It is called when an X request is made that causes a write to an 
 * input device.  No requests are supported here.
 *
 * See the file x_serialdrv.h for the format of the data for each request.
 *
 */

static int
hp7lc2mwrite (
    int fd, int request, char *data)
    {
    int i;
    HPPointerFeedbackControl *ctrl;
    ps2_buffer_t mousebuf;

    switch (request)
	{
	case _XChangeFeedbackControl:
	    ctrl = (HPPointerFeedbackControl *) data;

	    if (ctrl->class != PtrFeedbackClass)
		return(WRITE_FAILURE);
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
hp7lc2mclose (
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
hp7lc2m_Init(
    SerialProcs *serialproc)
    {
    serialproc->configure = hp7lc2mconfigure;/* routine to init device   */
    serialproc->read = hp7lc2mread;          /* routine to read from dev */
    serialproc->write = hp7lc2mwrite;        /* routine to write to dev  */
    serialproc->close = hp7lc2mclose;        /* routine to close device  */
    return INIT_SUCCESS;                  /* indicate success         */
    }

#ifdef DEBUG
char expected_button1[]= {
0,1,0,2,1,3,0,2,3,1,0,2,1,3,0,4,5,1,0,4,1,5,0,4,2,1,5,3, /* test case 1 */
4,5,4,2,5,3,4,2,3,5,4,2,5,3,4,2,3,5,4,0,2,5,1,3,         /* test case 2 */
2,3,2,0,1,0,1,0,1,4,5,4,5,3,0,4,1,5,                     /* test case 3 */
0,4,1,5,0,4,1,5,2,3,0,4,1,5,                             /* test case 4 */
0,2,1,3,0,2,1,3,4,5,                                     /* test case 5 */
4,2,5,3,4,2,5,3,                                         /* test case 6 */
0,4,2,1,5,3,0,4,2,5,1,4,3,0,2,5,1,4,3,0,2,1,5,3,0,4,1,5, /* test case 7 */
2,3,0,1,4,2,5,3,0,1,4,2,5,3,

0,2,3,1,4,2,3,5,0,4,5,1,0,4,2,3,5,1,0,2,1,3,4,2,5,3,0,4, /* miscellaneous */
2,1,5,3,0,2,1,4,3,0,1,5,2,3,4,2,0,1,0,1,0,1,5,3,0,1,4,2,
5,3,0,1,4,2,5,3,2,3,0,4,1,5,4,5,0,2,1,3,4,5,0,1,2,3};

char expected_button2[]= {
0,1,6,7,6,7,6,7,0,4,5,1,0,4,1,5,0,4,2,1,5,3,		 /* test case 1 */
4,5,8,9,8,9,8,9,8,9,4,0,2,5,1,3,		         /* test case 2 */
2,3,6,0,1,0,1,4,5,4,5,7,0,4,1,5,                         /* test case 3 */
0,4,1,5,0,4,1,5,2,3,0,4,1,5,                             /* test case 4 */
6,7,6,7,4,5,                                             /* test case 5 */
8,9,8,9,                                                 /* test case 6 */
0,4,2,1,5,3,0,4,2,5,1,4,3,0,2,5,1,4,3,0,2,1,5,3,0,4,1,5, /* test case 7 */
2,3,0,1,8,9,0,1,8,9,

6,7,8,9,0,4,5,1,0,4,2,3,5,1,6,7,8,9,0,4,2,1,5,3,6,4,7,0,/* miscellaneous */
1,5,2,3,8,0,1,0,1,0,1,9,0,1,8,9,0,1,8,9,2,3,0,4,1,5,4,5,6,7,4,5,0,1,2,3};

char testdata[][3] = {
			/* Case 1: left button goes down        */
			/*              remains down            */
			/*              goes up                 */
			/*              becomes chorded         */
			/*              becomes non-chordable   */
{0x08,1,1},		/* no buttons down    		        */
{0x08,1,1},		/* no buttons down    		        */
{0x09,1,1},		/* left button down	0	0       */
{0x09,1,1},		/* left button down		        */
{0x08,1,1},		/* no buttons down	1	1       */

{0x09,1,1},		/* left button down	0	        */
{0x09,1,1},		/* left button down	 	        */
{0x0d,1,1},		/* left and center    	2	6       */
{0x0d,1,1},		/* left and center    	 	        */
{0x08,1,1},		/* no buttons down	1	        */
			/*                      3       7       */
{0x09,1,1},		/* left button down	0	        */
{0x0d,1,1},		/* left and center    	2	6       */
{0x09,1,1},		/* left button down	3	        */
{0x08,1,1},		/* no buttons down	1	7       */
{0x09,1,1},		/* left button down	0	        */
{0x0d,1,1},		/* left and center    	2	6       */
{0x0c,1,1},		/* center button down	1               */
{0x08,1,1},		/* no buttons down	3	7       */
{0x09,1,1},		/* left button down	0	        */
{0x0b,1,1},		/* left and right     	4	0       */
			/*                              4       */
{0x09,1,1},		/* left button down	5	5       */
{0x08,1,1},		/* no buttons down	1	1       */
{0x09,1,1},		/* left button down	0	        */
{0x0b,1,1},		/* left and right     	4	0       */
			/*                              4       */
{0x0a,1,1},		/* right button down 	1       1       */
{0x08,1,1},		/* no buttons down	5	5       */
{0x09,1,1},		/* left button down	0	        */
{0x0f,1,1},		/* all buttons        	4	0       */
			/*                      2       4       */
			/*                              2       */
{0x0e,1,1},		/* right and center   	1	1       */
{0x08,1,1},		/* no buttons down	5	5       */
			/*                      3       3       */


			/* Case 2: right button goes down       */
			/*              remains down            */
			/*              goes up                 */
			/*              becomes chorded         */
			/*              becomes non-chordable   */
{0x08,1,1},		/* no buttons down	 	        */
{0x08,1,1},		/* no buttons down	 	        */
{0x0a,1,1},		/* right button down	4	4       */
{0x0a,1,1},		/* right button down	 	        */
{0x08,1,1},		/* no buttons down	5	5       */
{0x0a,1,1},		/* right button down	4	        */
{0x0e,1,1},		/* right and center   	2	8       */
{0x0e,1,1},		/* right and center   	 	        */
{0x08,1,1},		/* no buttons down	5	        */
			/*                      3       9       */
{0x0a,1,1},		/* right button down	4	        */
{0x0e,1,1},		/* right and center   	2	8       */
{0x0a,1,1},		/* right button down	3	        */
{0x08,1,1},		/* no buttons down	5	9       */
{0x0a,1,1},		/* right button down	4	        */
{0x0e,1,1},		/* right and center   	2	8       */
{0x0c,1,1},		/* center button down   5 	        */
{0x08,1,1},		/* no buttons down	3	9       */
{0x0a,1,1},		/* right button down	4	        */
{0x0e,1,1},		/* right and center   	2	8       */
{0x0a,1,1},		/* right button down	3	        */
{0x08,1,1},		/* no buttons down	5	9       */
{0x0a,1,1},		/* right button down	4	        */
{0x0f,1,1},		/* all buttons        	0	4       */
			/*                      2       0       */
			/*                              2       */
{0x0d,1,1},		/* left and center   	5	5       */
{0x08,1,1},		/* no buttons down	1	1       */
			/*                      3       3       */


			/* Case 3: center button goes down      */
			/*              remains down            */
			/*              goes up                 */
			/*              becomes chorded         */
			/*              becomes non-chordable   */
{0x0c,1,1},		/* center button down 	2	2       */
{0x0c,1,1},		/* center button down 		        */
{0x08,1,1},		/* no buttons down    	3	3       */
{0x0c,1,1},		/* center button down 	2	        */
{0x0d,1,1},		/* left and center    	0	6       */
{0x0d,1,1},		/* left and center    	 	        */
{0x0c,1,1},		/* center button down 	1	        */
{0x0d,1,1},		/* left and center    	0	0       */
{0x0c,1,1},		/* center button down 	1	1       */
{0x0d,1,1},		/* left and center    	0	0       */
{0x0e,1,1},		/* right and center   	1	1       */
                        /*                      4       4       */
{0x0c,1,1},		/* center button down 	5	5       */
{0x0e,1,1},		/* right and center   	4	4       */
{0x08,1,1},		/* no buttons down    	5	5       */
                        /*                      3       3       */
{0x0b,1,1},		/* left and right     	4	4       */
                        /*                      0       0       */
{0x08,1,1},		/* no buttons down    	5	5       */
                        /*                      1       1       */

			/* Case 4: left and right go down       */
{0x0b,1,1},		/* left and right     	0	0       */
                        /*                      4       4       */
{0x08,1,1},		/* no buttons down    	1	1       */
                        /*                      5       5       */
{0x0b,1,1},		/* left and right     	0	0       */
                        /*                      4       4       */
{0x0c,1,1},		/* center button down 	1	1       */
                        /*                      5       5       */
                        /*                      2       2       */
{0x0b,1,1},		/* left and right     	3	3       */
                        /*                      0       0       */
                        /*                      4       4       */
{0x08,1,1},		/* no buttons down    	1	1       */
                        /*                      5       5       */

			/* Case 5: left and center go down      */
{0x08,1,1},		/* no buttons down    	 	        */
{0x0d,1,1},		/* left and center    	0	        */
                        /*                      2       6       */
{0x08,1,1},		/* no buttons down    	1	7       */
                        /*                      3               */
{0x0d,1,1},		/* left and center    	0	6       */
                        /*                      2               */
{0x0a,1,1},		/* right button down	1	7       */
                        /*                      3       4       */
                        /*                      4               */
{0x08,1,1},		/* no buttons down    	5	5       */



			/* Case 6: right and center go down     */
{0x0e,1,1},		/* right and center   	4	        */
                        /*                      2       8       */
{0x08,1,1},		/* no buttons down    	5	9       */
                        /*                      3               */
{0x0e,1,1},		/* right and center   	4	8       */
                        /*                      2               */
{0x08,1,1},		/* no buttons down    	5	9       */
                        /*                      3               */
{0x08,1,1},		/* no buttons down    	 	        */


			/* Case 7: all buttons go down          */
{0x0f,1,1},		/* all buttons        	0	0   	*/
             		/*                    	4	4   	*/
             		/*                    	2	2   	*/
{0x0f,1,1},		/* all buttons                          */ 
{0x08,1,1},		/* no buttons down    	1	1       */
             		/*                    	5	5   	*/
             		/*                    	3	3   	*/
{0x0f,1,1},		/* all buttons        	0	0   	*/
             		/*                    	4	4   	*/
             		/*                    	2	2   	*/
{0x0d,1,1},		/* left and center    	5	5   	*/
{0x0e,1,1},		/* right and center   	1	1   	*/
                        /*                      4	4   	*/
{0x0b,1,1},		/* left and right     	3	3   	*/
                        /*                      0	0   	*/
{0x0f,1,1},		/* all buttons        	2	2   	*/
{0x0d,1,1},		/* left and center    	5	5   	*/
{0x0e,1,1},		/* right and center   	1	1   	*/
                        /*                      4	4   	*/
{0x0b,1,1},		/* left and right     	3	3   	*/
                        /*                      0	0   	*/
{0x0f,1,1},		/* all buttons        	2	2   	*/
{0x0c,1,1},		/* center button down 	1	1   	*/
                        /*                      5	5   	*/
{0x0b,1,1},		/* left and right     	3	3   	*/
                        /*                      0	0   	*/
                        /*                      4	4   	*/
{0x0c,1,1},		/* center button down 	1	1   	*/
                        /*                      5	5   	*/
                        /*                      2	2   	*/
{0x08,1,1},		/* no buttons down    	3	3   	*/
{0x09,1,1},		/* left button down   	0	    	*/
{0x0e,1,1},		/* right and center   	1	0   	*/
                        /*                      4	1   	*/
                        /*                      2	8   	*/
{0x09,1,1},		/* left button down   	5	    	*/
                        /*                      3	9   	*/
                        /*                      0	0   	*/
{0x0e,1,1},		/* right and center   	1	1   	*/
                        /*                      4	    	*/
                        /*                      2	8   	*/
{0x08,1,1},		/* no buttons down    	5	    	*/
                        /*                      3	9   	*/


			/* Case 8: miscellaneous tests          */
{0x09,1,1},		/* left button down   	0	    	*/
{0x0d,1,1},		/* left and center    	2	6   	*/
{0x09,1,1},		/* left button down   	3	    	*/
{0x08,1,1},		/* no buttons down    	1	7   	*/

{0x0a,1,1},		/* right button down  	4	    	*/
{0x0a,1,1},		/* right button down  		    	*/
{0x0e,1,1},		/* right and center   	2	8   	*/
{0x0a,1,1},		/* right button down    3	    	*/
{0x08,1,1},		/* no buttons down    	5	9   	*/

{0x09,1,1},		/* left button down   	0	0   	*/
{0x0b,1,1},		/* left and right     	4	4   	*/
{0x09,1,1},		/* left button down   	5	5   	*/
{0x08,1,1},		/* no buttons down    	1	1   	*/

{0x09,1,1},		/* left button down   	0	0   	*/
{0x0b,1,1},		/* left and right     	4	4   	*/
{0x0f,1,1},		/* all buttons        	2	2   	*/
{0x0b,1,1},		/* left and right     	3	3   	*/
{0x09,1,1},		/* left button down   	5	5   	*/
{0x08,1,1},		/* no buttons down    	1	1   	*/

{0x09,1,1},		/* left button down   	0	    	*/
{0x09,1,1},		/* left button down   		    	*/
{0x09,1,1},		/* left button down   		    	*/
{0x0d,1,1},		/* left and center    	2	6   	*/
{0x0d,1,1},		/* left and center    		    	*/
{0x08,1,1},		/* no buttons down    	1	7   	*/
                        /*                      3	    	*/

{0x0e,1,1},		/* right and center   	4	8   	*/
                        /*                      2	    	*/
{0x08,1,1},		/* no buttons down    	5	9   	*/
                        /*                      3	    	*/

{0x0f,1,1},		/* all buttons        	0	0   	*/
                        /*                      4	4   	*/
                        /*                      2	2   	*/
{0x08,1,1},		/* no buttons down    	1	1   	*/
                        /*                      5	5   	*/
                        /*                      3	3   	*/

{0x0d,1,1},		/* left and center    	0	    	*/
                        /*                      2	6   	*/
{0x0e,1,1},		/* right and center   	1	    	*/
                        /*                      4	4   	*/
{0x0b,1,1},		/* left and right     	3	7   	*/
                        /*                      0	0   	*/
{0x08,1,1},		/* no buttons down    	1	1   	*/
                        /*                      5	5   	*/
{0x0c,1,1},		/* center button down 	2	2   	*/
{0x08,1,1},		/* no buttons down    	3	3   	*/

{0x0e,1,1},		/* right and center   	4	8   	*/
                        /*                      2	    	*/
{0x0f,1,1},		/* all buttons        	0	0   	*/
{0x0e,1,1},		/* right and center   	1	1   	*/
{0x0f,1,1},		/* all buttons        	0	0   	*/
{0x0e,1,1},		/* right and center   	1	1   	*/
{0x0f,1,1},		/* all buttons        	0	0   	*/
{0x08,1,1},		/* no buttons down    	1	1   	*/
                        /*                      5	9   	*/
                        /*                      3	    	*/

{0x09,1,1},		/* left button down   	0	0   	*/
{0x0e,1,1},		/* center, right down 	1	1   	*/
                        /*                      4	8   	*/
                        /*                      2	    	*/
{0x09,1,1},		/* left button down   	5	9   	*/
                        /*                      3	0   	*/
                        /*                      0	    	*/
{0x0e,1,1},		/* center, right down 	1	1   	*/
                        /*                      4	8   	*/
                        /*                      2	    	*/
{0x08,1,1},		/* no buttons down    	5	9   	*/
                        /*                      3	    	*/

{0x0c,1,1},		/* center button down 	2	2   	*/
{0x0b,1,1},		/* left and right     	3	0   	*/
                        /*                      0	4   	*/
                        /*                      4	3   	*/
{0x08,1,1},		/* no buttons down    	1	1   	*/
                        /*                      5	5   	*/
{0x0a,1,1},		/* right button down  	4	    	*/
{0x0d,1,1},		/* left and center    	5	6   	*/
                        /*                      0	    	*/
                        /*                      2	    	*/
{0x08,1,1},		/* no buttons down    	1	7   	*/
                        /*                      3	    	*/
{0x0a,1,1},		/* right button down  	4	    	*/
{0x0a,1,1},		/* right button down  	 	    	*/
{0x0a,1,1},		/* right button down  	 	    	*/
{0x0a,1,1},		/* right button down  	 	 4  	*/
{0x0a,1,1},		/* right button down  	 	    	*/
{0x08,1,1},		/* no buttons down    	5	 5  	*/
{0x09,1,1},		/* left button down   	0	    	*/
{0x09,1,1},		/* left button down   	 	    	*/
{0x09,1,1},		/* left button down   	 	    	*/
{0x09,1,1},		/* left button down   	 	 0  	*/
{0x09,1,1},		/* left button down   	 	    	*/
{0x08,1,1},		/* no buttons down    	1	 1  	*/
{0x0c,1,1},		/* center button down 	2	    	*/
{0x0c,1,1},		/* center button down 	 	    	*/
{0x0c,1,1},		/* center button down 	 	    	*/
{0x0c,1,1},		/* center button down 	 	 2  	*/
{0x0c,1,1},		/* center button down 	 	    	*/
{0x08,1,1},		/* no buttons down    	3	 3  	*/

{0x00,1,1}};		/* terminate data     			*/

main ()
    {
    int fd;

    makedata();				/* make the test data	*/
    fd = open ("data", O_RDWR);
    printf("Test case 1: no button chording\n");
    process_test_data(fd, expected_button1);
    close(fd);
    fd = open ("data", O_RDWR);
    hp7lc2m_chording_interval = 100;
    printf("Test case 2: button chording enabled\n");
    process_test_data(fd, expected_button2);
    close(fd);
    }

process_test_data (fd, expected)
    int fd;
    char *expected;
    {
    int pending=0, ndx=0;
    unsigned char buf[128], data_type=0;

    buf[2] = 0;
    while (hp7lc2mread (fd, buf, &data_type, &pending) == READ_SUCCESS)
	{
	if (data_type & MOTION_DATA)
	    ndx = 2;
	if (data_type & BUTTON_DATA)
	    {
	    if (buf[ndx] != *expected)
	        printf ("Expected button %d received %d.\n",
		    *expected,buf[ndx]);
	    expected++;
	    }
	data_type = 0;
	buf[2]=0;
	pending=0;
	ndx = 0;
	};
    }

/* The PS2 mouse returns PKT_SIZ bytes for each movement of the mouse or
 * button press or release.  The format of the bytes is as follows:
 *
 * Byte 0:
 * 		Bit 7	Y data overflow		(1 = overflow)
 * 		Bit 6	X data overflow		(1 = overflow)
 * 		Bit 5	Y data sign		(1 = negative)
 * 		Bit 4	X data sign		(1 = negative)
 * 		Bit 3	Not used		(always 1)
 * 		Bit 2	Center button		(1 = depressed)
 * 		Bit 1	Right button		(1 = depressed)
 * 		Bit 0	Left button		(1 = depressed)
 * Byte 1:
 *		X coordinate data byte		(2's compliment)
 * Byte 2:
 *		Y coordinate data byte		(2's compliment)
 */

makedata ()
    {
    int i, fd;

    fd = creat("data", 0777);
    while (testdata[i][0] != 0)
	write (fd, testdata[i++], 3);
    close(fd);
    }
#endif /* DEBUG */
