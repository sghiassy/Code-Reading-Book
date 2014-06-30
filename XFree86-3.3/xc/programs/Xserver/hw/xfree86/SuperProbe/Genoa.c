/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Genoa.c,v 3.4 1996/12/23 06:31:09 dawes Exp $ */
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

/* $XConsortium: Genoa.c /main/5 1996/02/21 17:10:03 kaleb $ */

#include "Probe.h"

static int MemProbe_Genoa __STDCARGS((int));

Chip_Descriptor Genoa_Descriptor = {
	"Genoa",
	Probe_Genoa,
	NULL,
	0,
	FALSE,
	TRUE,
	TRUE,
	MemProbe_Genoa,
};

Bool Probe_Genoa(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte addr, data[4];

	if (ReadBIOS(0x37, &addr, 1) != 1)
	{
		fprintf(stderr, "%s: Failed to read Genoa BIOS address.\n",
			MyName);
		return(FALSE);
	}
	if (ReadBIOS((unsigned)addr, data, 4) != 4)
	{
		fprintf(stderr, "%s: Failed to read Genoa BIOS signature.\n",
			MyName);
		return(FALSE);
	}
	if ((data[0] == 0x77) && (data[2] == 0x99) && (data[3] == 0x66))
	{
		/*
		 * Genoa also has ET3000 and (possibly) ET4000 based
		 * boards that match this signature.  We only match
		 * the ones with Genoa chips, and let other probe
		 * functions deal with other chipsets.
		 */
		switch (data[1])
		{
		case 0x00:
			*Chipset = CHIP_G_6200;
			result = TRUE;
			break;
		case 0x11:
			*Chipset = CHIP_G_6400;
			result = TRUE;
			break;
		case 0x22:
			*Chipset = CHIP_G_6100;
			result = TRUE;
			break;
		}
	}
	return(result);
}

static int MemProbe_Genoa(Chipset)
int Chipset;
{
	int Mem = 0;

	switch (Chipset)
	{
	case CHIP_G_6100:
	case CHIP_G_6200:
		Mem = 256;
		break;
	case CHIP_G_6400:
		Mem = 512;
		break;
	}

	return(Mem);
}
