/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Yamaha.c,v 3.4 1996/12/23 06:31:47 dawes Exp $ */
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

/* $XConsortium: Yamaha.c /main/5 1996/02/21 17:13:23 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x3D4, 0x3D5};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

Chip_Descriptor Yamaha_Descriptor = {
	"Yamaha",
	Probe_Yamaha,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	NULL,
};

Bool Probe_Yamaha(Chipset)
int *Chipset;
{
	Bool result = FALSE;

	EnableIOPorts(NUMPORTS, Ports);
	if (testinx2(0x3D4, 0x7C, 0x7C))
	{
		result = TRUE;
		*Chipset = CHIP_YAMAHA6388;
	}
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}
