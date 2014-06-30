/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/UMC.c,v 3.4 1996/12/23 06:31:41 dawes Exp $ */
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

/* $XConsortium: UMC.c /main/5 1996/02/21 17:12:56 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x3BF, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_UMC __STDCARGS((int));

Chip_Descriptor UMC_Descriptor = {
	"UMC",
	Probe_UMC,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_UMC,
};

Bool Probe_UMC(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old;

	EnableIOPorts(NUMPORTS, Ports);

	old = inp(0x3BF);
	outp(0x3BF, 0x03);
	if (!testinx(SEQ_IDX, 0x06))
	{
		outp(0x3BF, 0xAC);
		if (testinx(SEQ_IDX, 0x06))
		{
			result = TRUE;
			*Chipset = CHIP_UMC_408;
		}
	}
	outp(0x3BF, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

/* ARGSUSED */
static int MemProbe_UMC(Chipset)
int Chipset;
{
	Byte old;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = inp(0x3BF);
	outp(0x3BF, 0xAC);

	/*
	 * Check
	 */
	switch(rdinx(SEQ_IDX, 0x07) >> 6)
	{
	case 0x00:
		Mem = 256;
		break;
	case 0x01:
		Mem = 512;
		break;
	case 0x02:
	case 0x03:
		Mem = 1024;
		break;
	}

	/*
	 * Lock
	 */
	outp(0x3BF, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
