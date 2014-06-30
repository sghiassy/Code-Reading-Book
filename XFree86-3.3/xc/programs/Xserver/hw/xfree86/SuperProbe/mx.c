/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/MX.c,v 3.4 1996/12/23 06:31:15 dawes Exp $ */
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

/* $XConsortium: MX.c /main/5 1996/02/21 17:10:36 kaleb $ */

#include "Probe.h"

static Word Ports[] = {SEQ_REG, SEQ_IDX};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_MX __STDCARGS((int));

Chip_Descriptor MX_Descriptor = {
	"MX",
	Probe_MX,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_MX,
};

Bool Probe_MX(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old;

	EnableIOPorts(NUMPORTS, Ports);

	old = rdinx(SEQ_IDX, 0xA7);
	wrinx(SEQ_IDX, 0xA7, 0x00);
	if (!testinx(SEQ_IDX, 0xC5))
	{
		wrinx(SEQ_IDX, 0xA7, 0x87);
		if (testinx(SEQ_IDX, 0xC5))
		{
			result = TRUE;
			if ((rdinx(SEQ_IDX, 0x26) & 0x01) == 0)
			{
				*Chipset = CHIP_MX68010;
			}
			else
			{
				*Chipset = CHIP_MX68000;
			}
		}
	}
	wrinx(SEQ_IDX, 0xA7, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

/* ARGSUSED */
static int MemProbe_MX(Chipset)
int Chipset;
{
	Byte old;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = rdinx(SEQ_IDX, 0xA7);
	wrinx(SEQ_IDX, 0xA7, 0x87);

	/*
	 * Check
	 */
	switch ((rdinx(SEQ_IDX, 0xC2) >> 2) & 0x03)
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
	}

	/*
	 * Lock
	 */
	wrinx(SEQ_IDX, 0xA7, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
