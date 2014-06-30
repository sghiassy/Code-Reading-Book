/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Herc.c,v 3.4 1996/12/23 06:31:11 dawes Exp $ */
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

/* $XConsortium: Herc.c /main/5 1996/02/21 17:10:16 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x3BA};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

#define DSP_VSYNC_MASK 0x80
#define DSP_ID_MASK 0x70

Bool Probe_Herc(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	int i, cnt = 0;
	Byte dsp, dsp_old;

	EnableIOPorts(NUMPORTS, Ports);
	dsp_old = inp(0x3BA) & DSP_VSYNC_MASK;
	for (i = 0; i < 0x10000; i++)
	{
		dsp = inp(0x3BA) & DSP_VSYNC_MASK;
		if (dsp != dsp_old)
			cnt++;
		dsp_old = dsp;
	}

	/* If there are active sync changes, we found a Hercules board. */
	if (cnt)
	{
		dsp = inp(0x3BA) & DSP_ID_MASK;
		switch(dsp)
		{
		case 0x10:
			*Chipset = CHIP_HERC_PLUS;
			break;
		case 0x50:
			*Chipset = CHIP_HERC_COL;
			break;
		default:
			*Chipset = CHIP_HERC_STD;
		}
		result = TRUE;
	}
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}
