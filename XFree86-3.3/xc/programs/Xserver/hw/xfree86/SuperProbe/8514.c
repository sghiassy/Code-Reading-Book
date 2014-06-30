/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/8514.c,v 3.4 1996/12/23 06:30:55 dawes Exp $ */
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

/* $XConsortium: 8514.c /main/5 1996/02/21 17:08:59 kaleb $ */

#include "Probe.h"

/*
 * Check for basic 8514 functionality.  8514 extended functionality will
 * be checked for elsewhere.
 */

static Word Ports[] = {SUBSYS_CNTL,ERR_TERM};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_8514 __STDCARGS((int));

Chip_Descriptor IBM8514_Descriptor = {
	"8514/A",
	Probe_8514,
	Ports,
	NUMPORTS,
	TRUE,
	FALSE,
	FALSE,
	MemProbe_8514,
};

Bool Probe_8514(Chipset)
int *Chipset;
{
	Bool result = FALSE;

	EnableIOPorts(NUMPORTS, Ports);

	/* 
	 * Reset the 8514/A, and disable all interrupts. 
	 */
	outpw(SUBSYS_CNTL, GPCTRL_RESET|CHPTEST_NORMAL);
	outpw(SUBSYS_CNTL, GPCTRL_ENAB|CHPTEST_NORMAL);

	/*
	 * Check to see if an 8514/A is actually installed by writing
	 * to the ERR_TERM register and reading back.  The 0x5A5A value
	 * is entirely arbitrary.
	 */
	outpw(ERR_TERM, 0x5A5A);
	if (inpw(ERR_TERM) == 0x5A5A)
	{
		/* 
		 * Let's make certain.
		 */
		outpw(ERR_TERM, 0x5555);
		if (inpw(ERR_TERM) == 0x5555)
		{
			if (tstrg(EXT_CONF_3, 0xF0))
			{
				*Chipset = CHIP_CT480;
			}
			else
			{
				*Chipset = CHIP_8514;
			}
			result = TRUE;
		}
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

/* ARGSUSED */
static int MemProbe_8514(Chipset)
int Chipset;
{
	int Mem;

	EnableIOPorts(NUMPORTS, Ports);

	if (inpw(SUBSYS_CNTL) & 0x0080)
	{
		Mem = 1024;
	}
	else
	{
		Mem = 512;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
