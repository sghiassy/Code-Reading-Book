/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Compaq.c,v 3.5 1996/12/23 06:31:08 dawes Exp $ */
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

/* $XConsortium: Compaq.c /main/5 1996/02/21 17:09:58 kaleb $ */

#include "Probe.h"

static Word Ports[] = {GRC_IDX, GRC_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_Compaq __STDCARGS((int));

Chip_Descriptor Compaq_Descriptor = {
	"Compaq",
	Probe_Compaq,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	FALSE,
	MemProbe_Compaq,
};

Bool Probe_Compaq(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old, ver;

	EnableIOPorts(NUMPORTS, Ports);

	old = rdinx(GRC_IDX, 0x0F);
	wrinx(GRC_IDX, 0x0F, 0x00);
	if (!testinx(GRC_IDX, 0x45))
	{
		wrinx(GRC_IDX, 0x0F, 0x05);
		if (testinx(GRC_IDX, 0x45))
		{
			result = TRUE;
			ver = rdinx(GRC_IDX, 0x0C) >> 3;
			switch (ver)
			{
			case 0x03:
				*Chipset = CHIP_CPQ_IVGS;
				break;
			case 0x05:
				*Chipset = CHIP_CPQ_AVGA;
				break;
			case 0x06:
				*Chipset = CHIP_CPQ_Q1024;
				break;
			case 0x0E:
				if (rdinx(GRC_IDX, 0x56) & 0x04)
				{
					*Chipset = CHIP_CPQ_Q1280;
				}
				else
				{
					*Chipset = CHIP_CPQ_Q1024;
				}
				break;
			case 0x10:
				*Chipset = CHIP_CPQ_AVGA_P;
				break;
			default:
				Chip_data = ver >> 3;
				*Chipset = CHIP_CPQ_UNK;
				break;
			}
		}
	}
	wrinx(GRC_IDX, 0x0F, old);
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_Compaq(Chipset)
int Chipset;
{
	Byte old, temp;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock.
	 */
	old = rdinx(GRC_IDX, 0x0F);
	wrinx(GRC_IDX, 0x0F, 0x05);

	/*
	 * Check
	 */
	switch (Chipset)
	{
	case CHIP_CPQ_IVGS:
		Mem = 256;
		break;
	case CHIP_CPQ_AVGA:
	case CHIP_CPQ_AVGA_P:
		Mem = 512;
		break;
	case CHIP_CPQ_Q1024:
	case CHIP_CPQ_Q1280:
		temp = rdinx(GRC_IDX, 0x0F);
		wrinx(GRC_IDX, 0x0F, 0x05);
		switch (rdinx(GRC_IDX, 0x54))
		{
		case 0x00:
			Mem = 1024;
			break;
		case 0x02:
			Mem = 512;
			break;
		case 0x04:
			Mem = 1024;
			break;
		case 0x08:
			Mem = 2048;
			break;
		}
		wrinx(GRC_IDX, 0x0F, temp);
		break;
	}

	/*
	 * Lock 
	 */
	wrinx(GRC_IDX, 0x0F, 0x05);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
