/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/CGA.c,v 3.4 1996/12/23 06:31:05 dawes Exp $ */
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

/* $XConsortium: CGA.c /main/5 1996/02/21 17:09:35 kaleb $ */

#include "Probe.h"

/*
 * Assume that the CRT Controller is at 0x3D4 (either no other adapter,
 * or an EGA/CGA is installed in Mono mode).  Look for the cursor
 * postition register.
 */
static Word Ports[] = {0x3D4, 0x3D5};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

Bool Probe_CGA(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte tmp;

	EnableIOPorts(NUMPORTS, Ports);
	tmp = rdinx(0x3D4, 0x0F);
	if (testinx(0x3D4, 0x0F))
	{
		result = TRUE;
		if (testinx(0x3D4, 0x13))
		{
			*Chipset = CHIP_MCGA;
		}
		else
		{
			*Chipset = CHIP_CGA;
		}
	}
	wrinx(0x3D4, 0x0F, tmp);
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}
