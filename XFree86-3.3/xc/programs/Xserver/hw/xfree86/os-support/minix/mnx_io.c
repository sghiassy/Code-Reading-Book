/* $XFree86: xc/programs/Xserver/hw/xfree86/os-support/minix/mnx_io.c,v 3.9 1996/12/23 06:50:19 dawes Exp $ */
/*
 * Copyright 1993 by Vrije Universiteit, The Netherlands
 * Copyright 1993 by David Dawes <dawes@physics.su.oz.au>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of the Vrije Universiteit and David 
 * Dawes not be used in advertising or publicity pertaining to 
 * distribution of the software without specific, written prior permission.
 * The Vrije Universiteit and David Dawes make no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * THE VRIJE UNIVERSITEIT AND DAVID DAWES DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE VRIJE UNIVERSITEIT OR 
 * DAVID DAWES BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR 
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: mnx_io.c /main/10 1996/10/19 18:07:03 kaleb $ */

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "inputstr.h"
#include "scrnintstr.h"

#include "compiler.h"

#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_Config.h"

#include <sys/nbio.h>

void xf86SoundKbdBell(loudness, pitch, duration)
int loudness;
int pitch;
int duration;
{
	kio_bell_t kio_bell;
	int r;

	kio_bell.kb_pitch = pitch;
	kio_bell.kb_volume = 1000000 / 100 * loudness;
	kio_bell.kb_duration.tv_sec = duration / 1000;
	kio_bell.kb_duration.tv_usec = (duration % 1000) * 1000;
	r = ioctl(xf86Info.kbdFd, KIOCBELL, &kio_bell);
	if (r != 0)
	{
		ErrorF("(warning) unable to ring keyboard bell: %s\n",
		       strerror(errno));
	}
}

void xf86SetKbdLeds(leds)
int leds;
{
	kio_leds_t kio_leds;
	int r;

	kio_leds.kl_bits= leds;
	r = ioctl(xf86Info.kbdFd, KIOCSLEDS, &kio_leds);
	if (r != 0)
	{
		ErrorF("(warning) unable to set keyboard leds: %s\n",
		       strerror(errno));
	}
}

int xf86GetKbdLeds()
{
	return(0);
}

#if NeedFunctionPrototypes
void xf86SetKbdRepeat(char rad)
#else
void xf86SetKbdRepeat(rad)
char rad;
#endif
{
	return;
}

void xf86KbdInit()
{
	static int kbd_fd = -1;
	int flags, r;
	uid_t real_uid;

	real_uid= getuid();

	/* Open the keyboard device if not already done so */
	if (kbd_fd < 0)
	{
		setuid(0);
		kbd_fd = open("/dev/kbd", O_RDONLY);
		if (kbd_fd == -1)
			FatalError("Unable to open keyboard\n");
		else
			xf86Info.kbdFd = kbd_fd;
		setuid(real_uid);

		/* Mark the keyboard as asynchronous */
		flags= fcntl(xf86Info.kbdFd, F_GETFD);
		if (flags == -1)
			FatalError("Unable to get keyboard flags\n");
		r = fcntl(xf86Info.kbdFd, F_SETFD, flags | FD_ASYNCHIO);
		if (r == -1)
			FatalError("Unable to set keyboard flags\n");

		nbio_register(kbd_fd);
	}
}

int xf86KbdOn()
{
	char waste[16];
	int r;

	/* Get rid of old data */
	for (;;)
	{
		r = nbio_read(xf86Info.kbdFd, waste, sizeof(waste));
		if (r > 0)
			continue;
		if (r == -1 && errno == EAGAIN)
			break;
		FatalError("unable to read from keyboard (%s)\n",
			   strerror(errno));
	}
	return(xf86Info.kbdFd);
}

int xf86KbdOff()
{
	/* Should RemoveEnabledDevice() be done for Minix?? */
	/* If it shouldn't be done, we should return -1 here */
	return(xf86Info.kbdFd);
}

void xf86KbdEvents()
{
	unsigned char rBuf[64];
	int nBytes, i;

	while ((nBytes = nbio_read(xf86Info.kbdFd, (char *)rBuf,
		sizeof(rBuf))) > 0)
	{
		for (i = 0; i < nBytes; i++)
			xf86PostKbdEvent(rBuf[i]);
	}
	if (nBytes == 0)
		ErrorF("xf86KbdEvents: nbio_read returns EOF");
	else if (errno != EAGAIN)
	{
		ErrorF("xf86KbdEvents: nbio_read error: %s",
			strerror(errno));
	}
}

void xf86MouseInit(mouse)
MouseDevPtr mouse;
{
	static int mseFd= -1;
	int r, flags;
	uid_t real_uid;

	if (mseFd < 0)
	{
		real_uid= getuid();

		setuid(0);
		if ((mseFd = mouse->mseFd =
			open(mouse->mseDevice, O_RDWR)) < 0)
		{
			if (xf86AllowMouseOpenFail) {
				ErrorF("Cannot open mouse (%s) - Continuing...\n",
					strerror(errno));
				return;
			}
			FatalError("Cannot open mouse (%s)\n",
				strerror(errno));
		}
		setuid(real_uid);

		xf86SetupMouse(mouse);

		/* Mark the mouse as asynchronous */
		flags = fcntl(mouse->mseFd, F_GETFD);
		if (flags == -1)
		{
			if (xf86AllowMouseOpenFail) {
				ErrorF("Unable to get mouse flags (%s) - Continuing...\n",
					strerror(errno));
				return;
			}
			FatalError("Unable to get mouse flags (%s)\n",
				strerror(errno));
		}
		r = fcntl(mouse->mseFd, F_SETFD, flags | FD_ASYNCHIO);
		if (r == -1)
		{
			if (xf86AllowMouseOpenFail) {
				ErrorF("Unable to set mouse flags (%s) - Continuing...\n",
					strerror(errno));
				return;
			}
			FatalError("Unable to set mouse flags (%s)\n",
				strerror(errno));
		}
		nbio_register(mouse->mseFd);
	}
}

int xf86MouseOn(mouse)
MouseDevPtr mouse;
{
	char waste[16];
	int r;

	/* Get rid of old data */
	for (;;)
	{
		r = nbio_read(mouse->mseFd, waste, sizeof(waste));
		if (r > 0)
			continue;
		if ((r == -1 && errno == EAGAIN) || xf86AllowMouseOpenFail)
		{
			break;
		}
		if (xf86AllowMouseOpenFail) {
			ErrorF("Unable to read from mouse (%s) - Continuing...\n",
				strerror(errno));
			return -2;
		}
		FatalError("unable to read from mouse (%s)\n",
			   strerror(errno));
	}
	return(mouse->mseFd);
}

void xf86MouseEvents(device)
DeviceIntPtr	device;
{
	unsigned char rBuf[64];
	int nBytes;

	while ((nBytes = nbio_read(mouse->mseFd, (char *)rBuf,
		sizeof(rBuf))) > 0)
	{
		xf86MouseProtocol(device, rBuf, nBytes);
	}
	if (nBytes == 0)
		ErrorF("xf86MouseEvents: nbio_read returns EOF");
	else if (errno != EAGAIN)
	{
		ErrorF("xf86MouseEvents: nbio_read error: %s",
			strerror(errno));
	}
}

int xf86MouseOff(mouse, doclose)
MouseDevPtr mouse;
Bool doclose;
{
	return(mouse->mseFd);
}
