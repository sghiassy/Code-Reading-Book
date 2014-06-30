/* $XConsortium: x_serialdrv.h,v 1.3 95/01/24 23:37:51 gildea Exp $ */
/************************************************************
Copyright (c) 1992 by Hewlett-Packard Company, Palo Alto, California.

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

/***************************************************************************
 *
 * Constants and structs for dynamically loaded serial input device drivers.
 *
 */

#ifndef _X_SERIALDRV_H_
#define _X_SERIALDRV_H_
#include <dl.h>

#define X_KEYMAP_NAME		"/etc/kbdlang"
#define MIN_KEYCODE		8
#define MAX_NM			64
#define DATA_IS_8_BITS		0x01
#define DATA_IS_16_BITS		0x02
#define DATA_IS_32_BITS		0x04
#define REPORTS_PROXIMITY	0x08
#define	ABSOLUTE_DATA		0x040
#define NON_CONTIGUOUS_DATA	0x080
#define FALSE			0
#define TRUE 			1
#define KEY_DATA 		0x01
#define BUTTON_DATA 		0x02
#define PROXIMITY_DATA 		0x04
#define MOTION_DATA 		0x08
#define INIT_SUCCESS		0
#define INIT_FAILURE		1
#define READ_SUCCESS		0
#define READ_FAILURE		1
#define WRITE_SUCCESS		0
#define WRITE_FAILURE		1
#define CLOSE_SUCCESS		0
#define IN_PROXIMITY		0
#define OUT_OF_PROXIMITY	1

#define SCROLLLOCK_LED		(1 << 0)
#define NUMLOCK_LED		(1 << 1) 
#define CAPSLOCK_LED		(1 << 2)

#define _XSetDeviceMode		0
#define _XSetDeviceValuators	1
#define _XChangeDeviceControl	2
#define _XChangeFeedbackControl	3
#define _XChangeKeyboardControl	4
#define _XChangePointerControl	5
#define _XBell			6

typedef struct {
	int	class;
	int	bell_percent;
} HPBell;

typedef struct {
	int	class;
	int	led_mask;
	int	led_values;
} HPLedFeedbackControl;

typedef struct {
	int	class;
	int	click;
	int	bell_percent;
	int	bell_pitch;
	int	bell_duration;
	int 	autoRepeat;
	unsigned char	autoRepeats[32];
	int 	leds;
} HPKeyboardFeedbackControl;

typedef struct {
	int	class;
	int	num;
	int	den;
	int	threshold;
} HPPointerFeedbackControl;

typedef struct {
	int	class;
	int	resolution;
	int	min_value;
	int	max_value;
	int	integer_displayed;
} HPIntegerFeedbackControl;

typedef struct {
	int	max_symbols;
	int	num_symbols_supported;
	int  	*symbols_supported;
} HPStrF;

typedef struct {
	int	class;
	int	max_symbols;
	int	num_symbols_supported;
	int	num_symbols_displayed;
	int  	*symbols_supported;
	int  	*symbols_displayed;
} HPStringFeedbackControl;

typedef struct {
	int	class;
	int	percent;
	int	pitch;
	int	duration;
} HPBellFeedbackControl;

typedef struct {
	int	*valuators;
	int	first_valuator;
	int	num_valuators;
} HPValuatorControl;

typedef struct {
	int	*resolutions;
	int	first_valuator;
	int	num_valuators;
} HPResolutionControl;

typedef int (*pfrb)();
typedef int (*ConfigureProc)();
typedef int (*InitProc)();
typedef int (*ReadProc)();
typedef int (*WriteProc)();
typedef int (*CloseProc)();

typedef struct _SerialProcs
    {
    ConfigureProc	configure;	/* filled in by driver		*/
    ReadProc		read;		/* filled in by driver		*/
    WriteProc		write;		/* filled in by driver		*/
    CloseProc		close;		/* filled in by driver		*/
    } SerialProcs; 

typedef struct _HPInputDeviceHeader
    {
    char	path[MAX_NM];	/* device path - filled in by X server  */
    char	*x_name;     	/* device name                          */
    char	*keymap_name;  	/* keymap name, if device has keys      */
    char	*keymap_file;  	/* keymap file, if device has keys      */
    int		resolution;	/* resolution in counts/cm         	*/
    int         max_x;        	/* maximum x value in counts    	*/
    int         max_y;          /* maximum y value in counts    	*/
    int         file_ds;       	/* file descriptor              	*/
    int         num_fdbk;       /* length of list that follows  	*/
    u_char	*feedbacks;	/* kbd, ptr, bell, and integer feedbacks*/
    int         num_ledf;       /* length of list that follows  	*/
    u_char	*ledf; 		/* led feedbacks                        */
    int         num_strf;       /* length of list that follows  	*/
    HPStrF	*strf;          /* string feedbacks                     */
    u_char	flags; 		/* device characteristics		*/
    u_char	ax_num;		/* number of axes			*/
    u_char	num_buttons;   	/* number of buttons         		*/
    u_char 	num_keys; 	/* number of keys            		*/
    u_char 	min_kcode;	/* minimum keycode           		*/
    u_char 	max_kcode;	/* maximum keycode           		*/
    u_char	reset;		/* keycode to cause X server reset      */
    u_char	reset_mods;	/* mask of modifiers for server reset   */
    u_char	button_chording;/* interval (ms) if chording enabled    */
    u_char	reserved[8];    /* reserved for future use              */
    }HPInputDeviceHeader;
#endif /* _X_SERIALDRV_H_ */
