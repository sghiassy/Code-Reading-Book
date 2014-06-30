/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/ChipsTech.c,v 3.10 1996/12/23 06:31:06 dawes Exp $ */
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

/* $XConsortium: ChipsTech.c /main/8 1996/10/28 04:23:17 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x3D6, 0x3D7};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_CT __STDCARGS((int));

Chip_Descriptor CT_Descriptor = {
	"CT",
	Probe_CT,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_CT,
};

#ifdef __STDC__
Bool Probe_CT(int *Chipset)
#else
Bool Probe_CT(Chipset)
int *Chipset;
#endif
{
	Bool result = FALSE;
	Byte vers;

	EnableIOPorts(NUMPORTS, Ports);
	if ((testinx(0x3D6, 0x18) && (testinx2(0x3D6, 0x7E, 0x3F))))
	{
		/*
		 * It's a Chips & Tech.  Now figure out which one.
		 */
		result = TRUE;
		vers = rdinx(0x3D6, 0x00);
		switch (vers >> 4)
		{
		case 0x0:
			*Chipset = CHIP_CT451;
			break;
		case 0x1:
			*Chipset = CHIP_CT452;
			break;
		case 0x2:
			*Chipset = CHIP_CT455;
			break;
		case 0x3:
			*Chipset = CHIP_CT453;
			break;
		case 0x4:
			*Chipset = CHIP_CT450;
			break;
		case 0x5:
			*Chipset = CHIP_CT456;
			break;
		case 0x6:
			*Chipset = CHIP_CT457;
			break;
		case 0x7:
			*Chipset = CHIP_CTF65520;
			break;
		case 0x8:
		case 0xc:				/* guess */
			*Chipset = CHIP_CTF65530;
			break;
		case 0x9:
			*Chipset = CHIP_CTF65510;
			break;
		case 0xd:
			switch (vers & 0xf)
			{
			case 0x0:
				*Chipset = CHIP_CTF65540;
				break;
			case 0x8:
			case 0x9:
			case 0xa:
				*Chipset = CHIP_CTF65545;
				break;
			case 0xb:
				*Chipset = CHIP_CTF65546;
				break;
			case 0xc:
				*Chipset = CHIP_CTF65548;
				break;
			default:
				Chip_data = vers;
				*Chipset = CHIP_CT_UNKNOWN;
				break;
			}
			break;
		default:
			Chip_data = (vers >> 4);
			*Chipset = CHIP_CT_UNKNOWN;
			break;
		}
		if (*Chipset == CHIP_CT_UNKNOWN)
		{
			vers = rdinx(0x3D6, 0x02);
			if ((vers & 0xf0) == 0xe0) {
				switch (vers & 0x0f)
				{
				case 0x0:
					*Chipset = CHIP_CTF65550;
					break;
				case 0x4:
					*Chipset = CHIP_CTF65554;
					break;
				default:
					Chip_data = vers & 0x0f;
					*Chipset = CHIP_CT_UNKNOWN;
					break;
				}
			}
		}
	}
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_CT(Chipset)
int Chipset;
{
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	switch (Chipset)
	{
	case CHIP_CT450:
	case CHIP_CT456:
		if (rdinx(0x3D6, 0x04) & 0x01)
		{
			Mem = 512;
		}
		else
		{
			Mem = 256;
		}
		break;
	case CHIP_CT451:
	case CHIP_CT455:
	case CHIP_CT457:
		Mem = 256;
		break;
	case CHIP_CT452:
		switch (rdinx(0x3D6, 0x04) & 0x03)
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
		break;
	case CHIP_CT453:
		switch ((rdinx(0x3D6, 0x24) >> 1) & 0x03)
		{
		case 0x01:
			Mem = 512;
			break;
		case 0x02:
			Mem = 1024;
			break;
		case 0x03:
			Mem = 256;
			break;
		}
		break;
	case CHIP_CTF65510:
		Mem = 512;
		break;
	case CHIP_CTF65520:
	case CHIP_CTF65530:
	case CHIP_CTF65540:
	case CHIP_CTF65545:
	case CHIP_CTF65546:
	case CHIP_CTF65548:
		switch (rdinx(0x3D6, 0x0F) & 0x03)
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
		break;
	case CHIP_CTF65550:
	case CHIP_CTF65554:
		switch ((rdinx(0x3D6, 0x43) & 0x06) >> 1)
		{
		case 0x00:
			Mem = 1024;
			break;
		case 0x01:
			Mem = 2048;
			break;
		case 0x03:
		case 0x04:
			Mem = 4096;
			break;
		}
		break;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
