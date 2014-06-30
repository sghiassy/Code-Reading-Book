/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Oak.c,v 3.4 1996/12/23 06:31:29 dawes Exp $ */
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
/* $XConsortium: Oak.c /main/5 1996/02/21 17:11:32 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x3DE, 0x3DF};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_Oak __STDCARGS((int));

Chip_Descriptor Oak_Descriptor = {
	"Oak",
	Probe_Oak,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_Oak,
};

Bool Probe_Oak(Chipset)
int *Chipset;
{
	Byte temp;
	Bool result = FALSE;

	EnableIOPorts(NUMPORTS, Ports);

	if (testinx2(0x3DE, 0x0D, 0x38))
	{
		result = TRUE;
		if (testinx2(0x3DE, 0x23, 0x1F))
		{
			if ((rdinx(0x3DE, 0x00) & 0x02) == 0x00)
			{
				*Chipset = CHIP_OAK087;
			}
			else
			{
				*Chipset = CHIP_OAK083;
			}
		}
		else 
		{
			temp = inp(0x3DE) >> 5;
			switch (temp)
			{
			case 0x00:
				*Chipset = CHIP_OAK037C;
				break;
			case 0x02:
				*Chipset = CHIP_OAK067;
				break;
			case 0x05:
				*Chipset = CHIP_OAK077;
				break;
			case 0x07:
				*Chipset = CHIP_OAK057;
				break;
			default:
				Chip_data = temp;
				*Chipset = CHIP_OAK_UNK;
				break;
			}
		}
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_Oak(Chipset)
int Chipset;
{
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	switch (Chipset)
	{
	case CHIP_OAK083:
	case CHIP_OAK087:
		switch (rdinx(0x3DE, 0x02) & 0x06)
		{
		case 0x00:
			Mem = 256;
			break;
		case 0x02:
			Mem = 512;
			break;
		case 0x04:
			Mem = 1024;
			break;
		case 0x06:
			Mem = 2048;
			break;
		}
		break;
	default:
		switch (rdinx(0x3DE, 0x0D) >> 6)
		{
		case 0x00:
			Mem = 256;
			break;
		case 0x02:
			Mem = 512;
			break;
		case 0x01:
		case 0x03:
			Mem = 1024;
			break;
		}
		break;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
