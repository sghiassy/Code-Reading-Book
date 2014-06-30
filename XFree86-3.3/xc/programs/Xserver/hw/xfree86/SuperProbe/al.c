/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/AL.c,v 3.4 1996/12/23 06:30:56 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by David Wexelblat <dwex@xfree86.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of David Wexelblat shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from David Wexelblat.
 *
 */

/* $XConsortium: AL.c /main/5 1996/02/21 17:09:04 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_AL __STDCARGS((int));

Chip_Descriptor AL_Descriptor = {
	"AL",
	Probe_AL,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	FALSE,
	MemProbe_AL,
};

Bool Probe_AL(Chipset)
int *Chipset;
{
	Byte old;
	Bool result = FALSE;

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	old = rdinx(CRTC_IDX, 0x1A);
	clrinx(CRTC_IDX, 0x1A, 0x10);
	if (!testinx2(CRTC_IDX, 0x19, 0xCF))
	{
		setinx(CRTC_IDX, 0x1A, 0x10);
		if ((testinx2(CRTC_IDX, 0x19, 0xCF) && 
		    (testinx2(CRTC_IDX, 0x1A, 0x3F))))
		{
			result = TRUE;
			switch (rdinx(CRTC_IDX, 0x1A) >> 6)
			{
			case 0x03:
				*Chipset = CHIP_AL2101;
				break;
			case 0x02:
				*Chipset = CHIP_AL2228;
				break;
			default:
				Chip_data = rdinx(CRTC_IDX, 0x1A);
				*Chipset = CHIP_AL_UNKNOWN;
				break;
			}
		}
	}
	wrinx(CRTC_IDX, 0x1A, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

/* ARGSUSED */
static int MemProbe_AL(Chipset)
int Chipset;
{
	Byte old;
	int Mem;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = rdinx(CRTC_IDX, 0x1A);
	setinx(CRTC_IDX, 0x1A, 0x10);

	/*
	 * Check
	 */
	switch (rdinx(CRTC_IDX, 0x1E) & 0x03)
	{
	case 0x00:
		Mem = 256;
		break;
	case 0x01:
		Mem = 512;
		break;
	case 0x02:
		Mem = 1024;
		break;
	case 0x03:
		Mem = 2048;
		break;
	}

	/*
	 * Lock
	 */
	wrinx(CRTC_IDX, 0x1A, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
