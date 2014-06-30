/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/VGA.c,v 3.4 1996/12/23 06:31:43 dawes Exp $ */
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

/* $XConsortium: VGA.c /main/5 1996/02/21 17:13:06 kaleb $ */

#include "Probe.h"

/*
 * Determine if this is a VGA or an EGA.  VGA has one more attribute
 * register than EGA, so see if we can read/write it.
 */
static Word Ports[] = {0x3C0, 0x3C1};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_VGA __STDCARGS((int));

Chip_Descriptor VGA_Descriptor = {
	"VGA",
	Probe_VGA,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_VGA,
};

Bool Probe_VGA(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte origVal, newVal;

	EnableIOPorts(NUMPORTS, Ports);
	origVal = rdinx(0x3C0, 0x14 | 0x20);
	outp(0x3C0, origVal ^ 0x0F);
	newVal  = rdinx(0x3C0, 0x14 | 0x20);
	outp(0x3C0, origVal);
	DisableIOPorts(NUMPORTS, Ports);
	if (newVal == (origVal ^ 0x0F))
	{
		result = TRUE;
		*Chipset = CHIP_VGA;
	}
	return(result);
}

/* ARGSUSED */
static int MemProbe_VGA(Chipset)
int Chipset;
{
	return(256);
}
