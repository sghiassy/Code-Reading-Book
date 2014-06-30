/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Weitek.c,v 3.5 1996/12/23 06:31:46 dawes Exp $ */
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

/* $XConsortium: Weitek.c /main/6 1996/10/25 06:19:09 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x3CD, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

Chip_Descriptor Weitek_Descriptor = {
	"Weitek",
	Probe_Weitek,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	NULL,
};

static Byte WeitekEnable(Flag)
Byte Flag;
{
	Byte new, old, x;

	old = rdinx(SEQ_IDX, 0x11);
	for (x=0; x < 10; x++)
	{
		;
	}
	outp(SEQ_REG, old);
	for (x=0; x < 10; x++)
	{
		;
	}
	outp(SEQ_REG, old);
	for (x=0; x < 10; x++)
	{
		;
	}
	new = inp(SEQ_REG);
	for (x=0; x < 10; x++)
	{
		;
	}
	outp(SEQ_REG, (new & 0x9F) | Flag);

	return(old);
}

Bool Probe_Weitek(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old, ver;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Disable extensions
	 */
	old = WeitekEnable(0x60);
	if (!testinx(SEQ_IDX, 0x12))
	{
		/*
		 * Enable extensions
		 */
		WeitekEnable(0);
		if (testinx(SEQ_IDX, 0x12))
		{
			if (tstrg(0x3CD, 0xFF))
			{
				result = TRUE;
				ver = rdinx(SEQ_IDX, 0x07) >> 5;
				switch (ver)
				{
				case 0x00:	/* guess */
					*Chipset = CHIP_WEIT_5086;
					break;
				case 0x01:
					*Chipset = CHIP_WEIT_5186;
					break;
				case 0x02:
					*Chipset = CHIP_WEIT_5286;
					break;
				default:
					Chip_data = ver;
					*Chipset = CHIP_WEIT_UNK;
					break;
				}
			}
		}
	}

	wrinx(SEQ_IDX, 0x11, old);
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}
