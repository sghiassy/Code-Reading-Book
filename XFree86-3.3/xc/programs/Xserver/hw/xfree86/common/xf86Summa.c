/*
 * Copyright 1996 by Steven Lang <tiger@ecis.com>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Steven Lang not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Steven Lang makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STEVEN LANG DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL STEVEN LANG BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTIONS, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/common/xf86Summa.c,v 3.5 1996/12/18 03:12:27 dawes Exp $ */

#include "Xos.h"
#include <signal.h>

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "inputstr.h"
#include "scrnintstr.h"
#include "XI.h"
#include "XIproto.h"

#if defined(sun) && !defined(i386)
#define POSIX_TTY
#include <errno.h>
#include <termio.h>
#include <fcntl.h>
#include <ctype.h>

#include "extio.h"
#else
#include "compiler.h"

#include "xf86.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"
#include "xf86Xinput.h"
#include "atKeynames.h"
#include "xf86Version.h"
#endif

#if !defined(sun) || defined(i386)
#include "osdep.h"
#include "exevents.h"

#include "extnsionst.h"
#include "extinit.h"
#endif

/*
** Debugging macros
*/
#ifdef DBG
#undef DBG
#endif
#ifdef DEBUG
#undef DEBUG
#endif

static int      debug_level = 0;
#define DEBUG	1
#if DEBUG
#define 	DBG(lvl, f) 	{if ((lvl) <= debug_level) f;}
#else
#define 	DBG(lvl, f)
#endif

/*
** Device records
*/
#define ABSOLUTE_FLAG		1
#define STYLUS_FLAG		2

typedef struct 
{
    char	*sumDevice;	/* device file name */
    int		sumInc;		/* increment between transmits */
    int		sumButTrans;	/* button translation flags */
    int		sumOldX;	/* previous X position */
    int		sumOldY;	/* previous Y position */
    int		sumOldProximity; /* previous proximity */
    int		sumOldButtons;	/* previous buttons state */
    int		sumMaxX;	/* max X value */
    int		sumMaxY;	/* max Y value */
    int		sumXLeft;	/* screen left */
    int		sumXRight;	/* screen right */
    int		sumYtop;	/* screen top */
    int		sumYbot;	/* screen bottom */
    int		sumRes;		/* resolution in lines per inch */
    int		flags;		/* various flags */
    int		sumIndex;	/* number of bytes read */
    unsigned char sumData[5];	/* data read on the device */
} SummaDeviceRec, *SummaDevicePtr;

/*
** Configuration data
*/
#define SUMMA_SECTION_NAME "SummaSketch"
#define PORT		1
#define DEVICENAME	2
#define THE_MODE	3
#define CURSOR		4
#define INCREMENT	5
#define BORDER		6
#define DEBUG_LEVEL     7
#define HISTORY_SIZE	8
#define ALWAYS_CORE	9

#if !defined(sun) || defined(i386)
static SymTabRec SumTab[] = {
	{ENDSUBSECTION,		"endsubsection"},
	{PORT,			"port"},
	{DEVICENAME,		"devicename"},
	{THE_MODE,		"mode"},
	{CURSOR,		"cursor"},
	{INCREMENT,		"increment"},
	{BORDER,		"border"},
	{DEBUG_LEVEL,		"debuglevel"},
	{HISTORY_SIZE,		"historysize"},
	{ ALWAYS_CORE,		"alwayscore" },
	{-1,			""}
};

#define RELATIVE	1
#define ABSOLUTE	2

static SymTabRec SumModeTabRec[] = {
	{RELATIVE,	"relative"},
	{ABSOLUTE,	"absolute"},
	{-1,		""}
};

#define PUCK		1
#define STYLUS		2

static SymTabRec SumPointTabRec[] = {
	{PUCK,		"puck"},
	{STYLUS,	"stylus"},
	{-1,		""}
};
  
#endif

/*
** Contants and macro
*/
#define BUFFER_SIZE	256	/* size of reception buffer */
#define XI_NAME 	"SUMMA"	/* X device name for the stylus */

#define SYSCALL(call) while(((call) == -1) && (errno == EINTR))

#define SS_TABID0	"0"	/* Tablet ID 0 */
#define SS_FIRMID	"z?"	/* Request firmware ID string */
#define SS_CONFIG	"a"	/* Send configuration (max coords) */

#define SS_ABSOLUTE	'F'	/* Absolute mode */
#define SS_RELATIVE	'E'	/* Relative mode */

#define SS_UPPER_ORIGIN	"b"	/* Origin upper left */
#define SS_500LPI	"h"	/* 500 lines per inch */

#define SS_PROMPT_MODE	"B"	/* Prompt mode */
#define SS_STREAM_MODE	"@"	/* Stream mode */
#define SS_INCREMENT	'I'	/* Set increment */
#define SS_BINARY_FMT	"zb"	/* Binary reporting */

#define SS_PROMPT	"P"	/* Prompt for current position */

static const char * ss_initstr = SS_TABID0 SS_UPPER_ORIGIN SS_BINARY_FMT SS_STREAM_MODE;

#define PHASING_BIT	0x80
#define PROXIMITY_BIT	0x40
#define TABID_BIT	0x20
#define XSIGN_BIT	0x10
#define YSIGN_BIT	0x08
#define BUTTON_BITS	0x07
#define COORD_BITS	0x7f

/*
** External declarations
*/
#if defined(sun) && !defined(i386)
#define ENQUEUE	suneqEnqueue
#else
#define ENQUEUE	xf86eqEnqueue

extern void xf86eqEnqueue(
#if NeedFunctionPrototypes
    xEventPtr /*e*/
#endif
);
#endif

extern void miPointerDeltaCursor(
#if NeedFunctionPrototypes
    int /*dx*/,
    int /*dy*/,
    unsigned long /*time*/
#endif
);

#if !defined(sun) || defined(i386)
/*
** xf86SumConfig
** Reads the SummaSketch section from the XF86Config file
*/
static Bool
xf86SumConfig(LocalDevicePtr *array, int inx, int max, LexPtr val)
{
    LocalDevicePtr	dev = array[inx];
    SummaDevicePtr	priv = (SummaDevicePtr)(dev->private);
    int			token;
    int			mtoken;

    DBG(1, ErrorF("xf86SumConfig\n"));

    while ((token = xf86GetToken(SumTab)) != ENDSUBSECTION) {
	switch(token) {
	case DEVICENAME:
	    if (xf86GetToken(NULL) != STRING)
		xf86ConfigError("Option string expected");
	    else {
		dev->name = strdup(val->str);
		if (xf86Verbose)
		    ErrorF("%s SummaSketch X device name is %s\n", XCONFIG_GIVEN,
			   dev->name);
	    }
	    break;

	case PORT:
	    if (xf86GetToken(NULL) != STRING)
		xf86ConfigError("Option string expected");
	    else {
		priv->sumDevice = strdup(val->str);
		if (xf86Verbose)
		    ErrorF("%s SummaSketch port is %s\n", XCONFIG_GIVEN,
			   priv->sumDevice);
	    }
	    break;

	case THE_MODE:
	    mtoken = xf86GetToken(SumModeTabRec);
	    if ((mtoken == EOF) || (mtoken == STRING) || (mtoken == NUMBER)) 
		xf86ConfigError("Mode type token expected");
	    else {
		switch (mtoken) {
		case ABSOLUTE:
		    priv->flags |= ABSOLUTE_FLAG;
		    break;
		case RELATIVE:
		    priv->flags &= ~ABSOLUTE_FLAG;
		    break;
		default:
		    xf86ConfigError("Illegal Mode type");
		    break;
		}
	    }
	    break;

	case CURSOR:
	    mtoken = xf86GetToken(SumPointTabRec);
	    if ((mtoken == EOF) || (mtoken == STRING) || (mtoken == NUMBER)) 
		xf86ConfigError("Cursor token expected");
	    else {
		switch (mtoken) {
		case STYLUS:
		    priv->flags |= STYLUS_FLAG;
		    break;
		case PUCK:
		    priv->flags &= ~STYLUS_FLAG;
		    break;
		default:
		    xf86ConfigError("Illegal cursor type");
		    break;
		}
	    }
	    break;

	case INCREMENT:
	    if (xf86GetToken(NULL) != NUMBER)
		xf86ConfigError("Option number expected");
	    priv->sumInc = val->num;
	    if (xf86Verbose)
		ErrorF("%s SummaSketch increment value is %d\n", XCONFIG_GIVEN,
		       priv->sumInc);
	    break;

	case DEBUG_LEVEL:
	    if (xf86GetToken(NULL) != NUMBER)
		xf86ConfigError("Option number expected");
	    debug_level = val->num;
	    if (xf86Verbose) {
#if DEBUG
		ErrorF("%s SummaSketch debug level sets to %d\n", XCONFIG_GIVEN,
		       debug_level);
#else
		ErrorF("%s SummaSketch debug level not sets to %d because"
		       " debugging is not compiled\n", XCONFIG_GIVEN,
		       debug_level);
#endif
	    }
	    break;

	case HISTORY_SIZE:
	    if (xf86GetToken(NULL) != NUMBER)
		xf86ConfigError("Option number expected");
	    dev->history_size = val->num;
	    if (xf86Verbose)
		ErrorF("%s SummaSketch Motion history size is %d\n", XCONFIG_GIVEN,
		       dev->history_size);      
	    break;

	case ALWAYS_CORE:
	    xf86AlwaysCore(dev, TRUE);
	    if (xf86Verbose)
		ErrorF("%s SummaSketch device always stays core pointer\n",
		       XCONFIG_GIVEN);
	    break;

	case EOF:
	    FatalError("Unexpected EOF (missing EndSubSection)");
	    break;

	default:
	    xf86ConfigError("SummaSketch subsection keyword expected");
	    break;
	}
    }

    DBG(1, ErrorF("xf86SumConfig name=%s\n", priv->sumDevice));

    return Success;
}
#endif

/*
** xf86SumReadInput
** Reads from the SummaSketch and posts any new events to the server.
*/
static void
xf86SumReadInput(LocalDevicePtr local)
{
    SummaDevicePtr	priv = (SummaDevicePtr) local->private;
    int			len, loop;
    int			is_core_pointer, is_absolute;
    int			x, y, buttons, prox;
    DeviceIntPtr	device;
    unsigned char	buffer[BUFFER_SIZE];
  
    DBG(7, ErrorF("xf86SumReadInput BEGIN device=%s fd=%d\n",
       priv->sumDevice, local->fd));

    SYSCALL(len = read(local->fd, buffer, sizeof(buffer)));

    if (len <= 0) {
	Error("error reading SummaSketch device");
	return;
    }

    for(loop=0; loop<len; loop++) {

/* Format of 5 bytes data packet for SummaSketch Tablets
       Byte 1
       bit 7  Phasing bit always 1
       bit 6  Proximity bit
       bit 5  Tablet ID
       bit 4  X sign (Always 1 for absolute)
       bit 3  Y sign (Always 1 for absolute)
       bit 2-0 Button status  

       Byte 2
       bit 7  Always 0
       bits 6-0 = X6 - X0

       Byte 3 (Absolute mode only)
       bit 7  Always 0
       bits 6-0 = X13 - X7

       Byte 4
       bit 7  Always 0
       bits 6-0 = Y6 - Y0

       Byte 5 (Absolute mode only)
       bit 7  Always 0
       bits 6-0 = Y13 - Y7
*/
  
	if ((priv->sumIndex == 0) && !(buffer[loop] & PHASING_BIT)) { /* magic bit is not OK */
	    DBG(6, ErrorF("xf86SumReadInput bad magic number 0x%x\n", buffer[loop]));;
	    continue;
	}

	priv->sumData[priv->sumIndex++] = buffer[loop];

	if (priv->sumIndex == (priv->flags & ABSOLUTE_FLAG? 5: 3)) {
/* the packet is OK */
/* reset char count for next read */
	    priv->sumIndex = 0;

	    if (priv->flags & ABSOLUTE_FLAG) {
		x = (int)priv->sumData[1] + ((int)priv->sumData[2] << 7);
		y = (int)priv->sumData[3] + ((int)priv->sumData[4] << 7);
	    } else {
		x = priv->sumData[0] & XSIGN_BIT? priv->sumData[1]: -priv->sumData[1];
		y = priv->sumData[0] & YSIGN_BIT? priv->sumData[2]: -priv->sumData[2];
	    }
	    prox = (priv->sumData[0] & PROXIMITY_BIT)? 0: 1;

	    buttons = (priv->sumData[0] & BUTTON_BITS);

	    device = local->dev;

	    DBG(6, ErrorF("prox=%s\tx=%d\ty=%d\tbuttons=%d\n",
		   prox ? "true" : "false", x, y, buttons));

	    is_absolute = (priv->flags & ABSOLUTE_FLAG);
	    is_core_pointer = xf86IsCorePointer(device);

	    if (is_core_pointer) {
		x = x * screenInfo.screens[0]->width / priv->sumMaxX;
		y = y * screenInfo.screens[0]->height / priv->sumMaxY;
		DBG(6, ErrorF("Adjusted coords x=%d y=%d\n", x, y));
	    }

/* coordonates are ready we can send events */
	    if (prox) {
		if (!(priv->sumOldProximity))
		    if (!is_core_pointer)
			xf86PostProximityEvent(device, 1, 0, 2, x, y);

		if ((is_absolute && ((priv->sumOldX != x) || (priv->sumOldY != y)))
		       || (!is_absolute && (x || y))) {
		    if (is_absolute || priv->sumOldProximity) {
			xf86PostMotionEvent(device, is_absolute, 0, 2, x, y);
		    }
		}
		if (priv->sumOldButtons != buttons) {
		int	delta;
		int	button;

		    delta = buttons - priv->sumOldButtons;
		    button = (delta > 0)? delta: ((delta == 0)?
			   priv->sumOldButtons : -delta);

		    if (priv->sumOldButtons != buttons) {
			DBG(6, ErrorF("xf86SumReadInput button=%d delta=%d\n", button,
			       delta));

			xf86PostButtonEvent(device, is_absolute, button,
			       (delta > 0), 0, 2, x, y);
		    }
		}
		priv->sumOldButtons = buttons;
		priv->sumOldX = x;
		priv->sumOldY = y;
		priv->sumOldProximity = prox;
	    } else { /* !PROXIMITY */
/* Any changes in buttons are ignored when !proximity */
		if (!is_core_pointer)
		    if (priv->sumOldProximity)
			xf86PostProximityEvent(device, 0, 0, 2, x, y);
		priv->sumOldProximity = 0;
	    }
	}
    }
    DBG(7, ErrorF("xf86SumReadInput END   device=0x%x priv=0x%x\n",
	   local->dev, priv));
}

/*
** xf86SumControlProc
** It really does do something.  Honest!
*/
static void
xf86SumControlProc(DeviceIntPtr	device, PtrCtrl *ctrl)
{
    DBG(2, ErrorF("xf86SumControlProc\n"));
}

/*
** fdflush
** Flushes all pending data from a fd..  (Isn't there a system call to do this
** more effeciently?)
*/
void
fdflush(int	fd)
{
  fd_set readfds;
  struct timeval timeout;
  int err;
  char buffer[100];

  FD_ZERO(&readfds);
  FD_SET(fd, &readfds);
  timeout.tv_sec = 0;
  timeout.tv_usec = 50;
  while (1) {
    SYSCALL(err = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout));
    if (err <= 0)
      return;
    read(fd, buffer, 100);
  }
}

/*
** write_and_read
** Write data, and get the response.
*/
static char *
write_and_read(int fd, char *data, char *buffer, int len, int cr_term)
{
    int err, numread = 0;
    fd_set readfds;
    struct timeval timeout;

    SYSCALL(err = write(fd, data, strlen(data)));
    if (err == -1) {
	Error("SummaSketch write");
	return NULL;
    }

    FD_ZERO(&readfds);
    FD_SET(fd, &readfds);
    while (numread < len) {
	timeout.tv_sec = 0;
	timeout.tv_usec = 200000;

	SYSCALL(err = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout));
	if (err == -1) {
	    Error("SummaSketch select");
	    return NULL;
	}
	if (!err) {
	    ErrorF("Timeout while reading SummaSketch tablet. No tablet connected ???\n");
	    return NULL;
	}

	SYSCALL(err = read(fd, buffer + numread++, 1));
	if (err == -1) {
	    Error("SummaSketch read");
	    return NULL;
	}
	if (!err) {
	    --numread;
	    break;
	}
	if (cr_term && buffer[numread - 1] == '\r') {
	    break;
	    buffer[numread - 1] = 0;
	}
    }
    buffer[numread] = 0;
    return buffer;
}

/*
** xf86SumOpen
** Open and initialize the tablet, as well as probe for any needed data.
*/
static Bool
xf86SumOpen(LocalDevicePtr local)
{
    struct termios	termios_tty;
    struct timeval	timeout;
    char		buffer[256];
    int			err;
    SummaDevicePtr	priv = (SummaDevicePtr)local->private;

    DBG(1, ErrorF("opening %s\n", priv->sumDevice));

    SYSCALL(local->fd = open(priv->sumDevice, O_RDWR|O_NDELAY));
    if (local->fd == -1) {
	Error(priv->sumDevice);
	return !Success;
    }
    DBG(2, ErrorF("%s opened as fd %d\n", priv->sumDevice, local->fd));

#ifdef POSIX_TTY
    err = tcgetattr(local->fd, &termios_tty);
    if (err == -1) {
	Error("SummaSketch tcgetattr");
	return !Success;
    }
    termios_tty.c_iflag = IXOFF;
    termios_tty.c_cflag = B9600|CS8|CREAD|CLOCAL|HUPCL|PARENB|PARODD;
    termios_tty.c_lflag = 0;

/* I wonder what these all do, anyway */
    termios_tty.c_cc[VINTR] = 0;
    termios_tty.c_cc[VQUIT] = 0;
    termios_tty.c_cc[VERASE] = 0;
#ifdef VWERASE
    termios_tty.c_cc[VWERASE] = 0;
#endif
#ifdef VREPRINT
    termios_tty.c_cc[VREPRINT] = 0;
#endif
    termios_tty.c_cc[VKILL] = 0;
    termios_tty.c_cc[VEOF] = 0;
    termios_tty.c_cc[VEOL] = 0;
#ifdef VEOL2
    termios_tty.c_cc[VEOL2] = 0;
#endif
    termios_tty.c_cc[VSUSP] = 0;
#ifdef VDISCARD
    termios_tty.c_cc[VDISCARD] = 0;
#endif
#ifdef VLNEXT
    termios_tty.c_cc[VLNEXT] = 0; 
#endif

    termios_tty.c_cc[VMIN] = 1 ;
    termios_tty.c_cc[VTIME] = 10 ;

    err = tcsetattr(local->fd, TCSANOW, &termios_tty);
    if (err == -1) {
	Error("SummaSketch tcsetattr TCSANOW");
	return !Success;
    }
#else
    Code for someone else to write to handle OSs without POSIX tty functions
#endif

    DBG(1, ErrorF("initializing SummaSketch tablet\n"));

/* Send reset (NULL) to the tablet */
    SYSCALL(err = write(local->fd, "", 1));
    if (err == -1) {
	Error("SummaSketch write");
	return !Success;
    }

/* wait 200 mSecs, just in case */
    timeout.tv_sec = 0;
    timeout.tv_usec = 200000;
    SYSCALL(err = select(0, NULL, NULL, NULL, &timeout));
    if (err == -1) {
	Error("SummaSketch select");
	return !Success;
    }

/* Put it in prompt mode so it doens't say anything before we're ready */
    SYSCALL(err = write(local->fd, SS_PROMPT_MODE, strlen(SS_PROMPT_MODE)));
    if (err == -1) {
	Error("SummaSketch write");
	return !Success;
    }
/* Clear any pending input */
    fdflush(local->fd);
  
    DBG(2, ErrorF("reading firmware ID\n"));
    if (!write_and_read(local->fd, SS_FIRMID, buffer, 255, 1))
      return !Success;

    DBG(2, ErrorF("%s\n", buffer));

    if (xf86Verbose)
	ErrorF("%s SummaSketch firmware ID : %s\n", XCONFIG_PROBED, buffer);

    DBG(2, ErrorF("reading max coordinates\n"));
    if (!write_and_read(local->fd, SS_500LPI SS_CONFIG, buffer, 5, 0))
	return !Success;
    priv->sumMaxX = (int)buffer[1] + ((int)buffer[2] << 7);
    priv->sumMaxY = (int)buffer[3] + ((int)buffer[4] << 7);

    if (xf86Verbose)
	ErrorF("%s SummaSketch tablet size is %d.%1dinx%d.%1din, %dx%d "
	       "lines of resolution\n", XCONFIG_PROBED, 
	       priv->sumMaxX / 500, (priv->sumMaxX / 50) % 10,
	       priv->sumMaxY / 500, (priv->sumMaxY / 50) % 10,
	       priv->sumMaxX, priv->sumMaxY);

    if (priv->sumInc > 95)
	priv->sumInc = 95;
    if (priv->sumInc < 1) {
/* Make a guess as to the best increment value given video mode */
	if (priv->sumMaxX / screenInfo.screens[0]->width <
	       priv->sumMaxY / screenInfo.screens[0]->height)
	    priv->sumInc = priv->sumMaxX / screenInfo.screens[0]->width;
	else
	    priv->sumInc = priv->sumMaxY / screenInfo.screens[0]->height;
	if (priv->sumInc < 1)
	    priv->sumInc = 1;
	if (xf86Verbose)
	    ErrorF("%s Using increment value of %d\n", XCONFIG_PROBED,
		   priv->sumInc);
    }

/* Sets up the tablet mode to increment, stream, and such */
    sprintf(buffer, "%s%c%c%c", ss_initstr, SS_INCREMENT, 32 + priv->sumInc,
	   (priv->flags & ABSOLUTE_FLAG)? SS_ABSOLUTE: SS_RELATIVE);

    SYSCALL(err = write(local->fd, buffer, strlen(buffer)));
    if (err == -1) {
	Error("SummaSketch write");
	return !Success;
    }

    if (err <= 0) {
	SYSCALL(close(local->fd));
	return !Success;
    }

    return Success;
}

/*
** xf86SumOpenDevice
** Opens and initializes the device driver stuff or sumpthin.
*/
static int
xf86SumOpenDevice(DeviceIntPtr pSum)
{
    LocalDevicePtr	local = (LocalDevicePtr)pSum->public.devicePrivate;
    SummaDevicePtr	priv = (SummaDevicePtr)PRIVATE(pSum);

    if (xf86SumOpen(local) != Success) {
	if (local->fd >= 0) {
	    SYSCALL(close(local->fd));
	}
	local->fd = -1;
    }

/* Set the real values */
    InitValuatorAxisStruct(pSum,
			   0,
			   0, /* min val */
			   priv->sumMaxX, /* max val */
			   500000, /* resolution */
			   0, /* min_res */
			   500000); /* max_res */
    InitValuatorAxisStruct(pSum,
			   1,
			   0, /* min val */
			   priv->sumMaxY, /* max val */
			   500000, /* resolution */
			   0, /* min_res */
			   500000); /* max_res */
    return (local->fd != -1);
}

/*
** xf86SumProc
** Handle requests to do stuff to the driver.
*/
static int
xf86SumProc(DeviceIntPtr pSum, int what)
{
    CARD8		map[25];
    int			nbaxes;
    int			nbbuttons;
    int			loop;
    LocalDevicePtr	local = (LocalDevicePtr)pSum->public.devicePrivate;
    SummaDevicePtr	priv = (SummaDevicePtr)PRIVATE(pSum);

    DBG(2, ErrorF("BEGIN xf86SumProc dev=0x%x priv=0x%x what=%d\n", pSum, priv, what));

    switch (what) {
	case DEVICE_INIT:
	    DBG(1, ErrorF("xf86SumProc pSum=0x%x what=INIT\n", pSum));

	    nbaxes = 2;			/* X, Y */
	    nbbuttons = (priv->flags & STYLUS_FLAG)? 2: 4;

	    for(loop=1; loop<=nbbuttons; loop++) map[loop] = loop;

	    if (InitButtonClassDeviceStruct(pSum,
					    nbbuttons,
					    map) == FALSE) {
		ErrorF("unable to allocate Button class device\n");
		return !Success;
	    }

	    if (InitFocusClassDeviceStruct(pSum) == FALSE) {
		ErrorF("unable to init Focus class device\n");
		return !Success;
	    }

	    if (InitPtrFeedbackClassDeviceStruct(pSum,
		   xf86SumControlProc) == FALSE) {
		ErrorF("unable to init ptr feedback\n");
		return !Success;
	    }

	    if (InitProximityClassDeviceStruct(pSum) == FALSE) {
		ErrorF("unable to init proximity class device\n"); 
		return !Success;
	    }

	    if (InitValuatorClassDeviceStruct(pSum,
		   nbaxes,
		   xf86GetMotionEvents,
		   local->history_size,
		   (priv->flags & ABSOLUTE_FLAG)? Absolute: Relative)
		   == FALSE) {
		ErrorF("unable to allocate Valuator class device\n"); 
		return !Success;
	    }
/* allocate the motion history buffer if needed */
	    xf86MotionHistoryAllocate(local);

	    AssignTypeAndName(pSum, local->atom, local->name);
/* open the device to gather informations */
	    xf86SumOpenDevice(pSum);
	    break;

	case DEVICE_ON:
	    DBG(1, ErrorF("xf86SumProc pSum=0x%x what=ON\n", pSum));

	    if ((local->fd < 0) && (!xf86SumOpenDevice(pSum))) {
		return !Success;
	    }
	    SYSCALL(write(local->fd, SS_PROMPT, strlen(SS_PROMPT)));
	    AddEnabledDevice(local->fd);
	    pSum->public.on = TRUE;
	    break;

	case DEVICE_OFF:
	    DBG(1, ErrorF("xf86SumProc  pSum=0x%x what=%s\n", pSum,
		   (what == DEVICE_CLOSE) ? "CLOSE" : "OFF"));
	    if (local->fd >= 0)
		RemoveEnabledDevice(local->fd);
	    pSum->public.on = FALSE;
	    break;

	case DEVICE_CLOSE:
	    DBG(1, ErrorF("xf86SumProc  pSum=0x%x what=%s\n", pSum,
		   (what == DEVICE_CLOSE) ? "CLOSE" : "OFF"));
	    SYSCALL(close(local->fd));
	    local->fd = -1;
	    break;

	default:
	    ErrorF("unsupported mode=%d\n", what);
	    return !Success;
	    break;
    }
    DBG(2, ErrorF("END   xf86SumProc Success what=%d dev=0x%x priv=0x%x\n",
	   what, pSum, priv));
    return Success;
}

/*
** xf86SumClose
** It...  Uh...  Closes the physical device?
*/
static void
xf86SumClose(LocalDevicePtr local)
{
    if (local->fd >= 0) {
	SYSCALL(close(local->fd));
    }
    local->fd = -1;
}

/*
** xf86SumChangeControl
** When I figure out what it does, it will do it.
*/
static int
xf86SumChangeControl(LocalDevicePtr local, xDeviceCtl *control)
{
    xDeviceResolutionCtl	*res;

    res = (xDeviceResolutionCtl *)control;
	
    if ((control->control != DEVICE_RESOLUTION) ||
	   (res->num_valuators < 1))
	return (BadMatch);

    return(Success);
}

/*
** xf86SumSwitchMode
** Switches the mode.  For now just absolute or relative, hopefully
** more on the way.
*/
static int
xf86SumSwitchMode(ClientPtr client, DeviceIntPtr dev, int mode)
{
    LocalDevicePtr	local = (LocalDevicePtr)dev->public.devicePrivate;
    SummaDevicePtr	priv = (SummaDevicePtr)(local->private);
    char		newmode;

    DBG(3, ErrorF("xf86SumSwitchMode dev=0x%x mode=%d\n", dev, mode));

    switch(mode) {
	case Absolute:
	    priv->flags |= ABSOLUTE_FLAG;
	    newmode = SS_ABSOLUTE;
	    break;

	case Relative:
	    priv->flags &= ~ABSOLUTE_FLAG;
	    newmode = SS_RELATIVE;
	    break;

	default:
	    DBG(1, ErrorF("xf86SumSwitchMode dev=0x%x invalid mode=%d\n",
		   dev, mode));
	    return BadMatch;
    }
    SYSCALL(write(local->fd, &newmode, 1));
    return Success;
}

/*
** xf86SumAllocate
** Allocates the device structures for the SummaSketch.
*/
static LocalDevicePtr
xf86SumAllocate()
{
    LocalDevicePtr	local = (LocalDevicePtr)xalloc(sizeof(LocalDeviceRec));
    SummaDevicePtr	priv = (SummaDevicePtr)xalloc(sizeof(SummaDeviceRec));
#if defined (sun) && !defined(i386)
    char		*dev_name = getenv("SUMMASKETCH_DEV");
#endif

    local->name = XI_NAME;
    local->type_name = "SummaSketch Tablet";
    local->flags = 0; /*XI86_NO_OPEN_ON_INIT;*/
#if !defined(sun) || defined(i386)
    local->device_config = xf86SumConfig;
#endif
    local->device_control = xf86SumProc;
    local->read_input = xf86SumReadInput;
    local->control_proc = xf86SumChangeControl;
    local->close_proc = xf86SumClose;
    local->switch_mode = xf86SumSwitchMode;
    local->fd = -1;
    local->atom = 0;
    local->dev = NULL;
    local->private = priv;
    local->private_flags = 0;
    local->history_size  = 0;

#if defined(sun) && !defined(i386)
    if (def_name) {
	priv->sumDevice = (char *)xalloc(strlen(dev_name) + 1);
	strcpy(priv->sumDevice, device_name);
	ErrorF("xf86SumOpen port changed to '%s'\n", priv->sumDevice);
    } else {
	priv->sumDevice = "";
    }
#else
    priv->sumDevice = "";         /* device file name */
#endif
    priv->sumInc = -1;            /* re-transmit position on increment */
    priv->sumOldX = -1;           /* previous X position */
    priv->sumOldY = -1;           /* previous Y position */
    priv->sumOldProximity = 0;    /* previous proximity */
    priv->sumOldButtons = 0;      /* previous buttons state */
    priv->sumMaxX = -1;           /* max X value */
    priv->sumMaxY = -1;           /* max Y value */
    priv->flags = 0;              /* various flags */
    priv->sumIndex = 0;           /* number of bytes read */

    return local;
}


/*
** SummaSketch device association
** Device section name and allocation function.
*/
DeviceAssocRec summasketch_assoc =
{
  SUMMA_SECTION_NAME,           /* config_section_name */
  xf86SumAllocate               /* device_allocate */
};

#ifdef DYNAMIC_MODULE
/*
** init_module
** Entry point for dynamic module.
*/
int
#ifndef DLSYM_BUG
init_module(unsigned long server_version)
#else
init_xf86Summa(unsigned long server_version)
#endif
{
    xf86AddDeviceAssoc(&summasketch_assoc);

    if (server_version != XF86_VERSION_CURRENT) {
	ErrorF("Warning: SummaKetch module compiled for version%s\n",
	       XF86_VERSION);
	return 0;
    } else {
	return 1;
    }
}
#endif

/* end of xf86Summa.c */
