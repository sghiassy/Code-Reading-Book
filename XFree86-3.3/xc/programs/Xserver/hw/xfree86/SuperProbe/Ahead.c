/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Ahead.c,v 3.4 1996/12/23 06:31:01 dawes Exp $ */
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

/* $XConsortium: Ahead.c /main/5 1996/02/21 17:09:18 kaleb $ */

#include "Probe.h"

static Word Ports[] = {GRC_IDX, GRC_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_Ahead __STDCARGS((int));

Chip_Descriptor Ahead_Descriptor = {
	"Ahead",
	Probe_Ahead,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_Ahead,
};

Bool Probe_Ahead(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old, tmp;

	EnableIOPorts(NUMPORTS, Ports);
	old = rdinx(GRC_IDX, 0x0F);
	wrinx(GRC_IDX, 0x0F, 0);
	if (!testinx2(GRC_IDX, 0x0C, 0xFB))
	{
		wrinx(GRC_IDX, 0x0F, 0x20);
		if (testinx2(GRC_IDX, 0x0C, 0xFB))
		{
			result = TRUE;
			tmp = rdinx(GRC_IDX, 0x0F) & 0x0F;
			switch (tmp)
			{
			case 0x00:
				*Chipset = CHIP_AHEAD_A;
				break;
			case 0x01:
				*Chipset = CHIP_AHEAD_B;
				break;
			default:
				Chip_data = tmp;
				*Chipset = CHIP_AHEAD_UNK;
				break;
			}
		}
	}
	wrinx(GRC_IDX, 0x0F, old);
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

/* ARGSUSED */
static int MemProbe_Ahead(Chipset)
int Chipset;
{
	Byte old;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = rdinx(GRC_IDX, 0x0F);
	setinx(GRC_IDX, 0x0F, 0x20);

	/*
	 * Check
	 */
	switch (rdinx(GRC_IDX, 0x1F) & 0x03)
	{
	case 0x00:
		Mem = 256;
		break;
	case 0x01:
		Mem = 512;
		break;
	case 0x03:
		Mem = 1024;
		break;
	}

	/*
	 * Lock
	 */
	wrinx(GRC_IDX, 0x0F, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
