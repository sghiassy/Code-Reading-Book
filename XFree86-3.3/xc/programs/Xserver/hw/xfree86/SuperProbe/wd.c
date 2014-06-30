/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/WD.c,v 3.5 1996/12/23 06:31:45 dawes Exp $ */
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

/* $XConsortium: WD.c /main/6 1996/10/24 07:10:17 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, GRC_IDX, GRC_REG, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_WD __STDCARGS((int));

Chip_Descriptor WD_Descriptor = {
	"WD",
	Probe_WD,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_WD,
};

Bool Probe_WD(Chipset)
int *Chipset;
{
	Byte old, old1, old2, old3;
	Word ver;
	Bool result = FALSE;

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	old = rdinx(GRC_IDX, 0x0F);
	setinx(GRC_IDX, 0x0F, 0x17);	/* Lock registers */
	if (!testinx2(GRC_IDX, 0x09, 0x7F))
	{
		wrinx(GRC_IDX, 0x0F, 0x05);	/* Unlock them again */
		if (testinx2(GRC_IDX, 0x09, 0x7F))
		{
			result = TRUE;
			old2 = rdinx(CRTC_IDX, 0x29);
			/* Unlock WD90Cxx regs */
			modinx(CRTC_IDX, 0x29, 0x8F, 0x85);
			if (!testinx(CRTC_IDX, 0x2B))
			{
				*Chipset = CHIP_WD_PVGA1;
			}
			else
			{
				old1 = rdinx(SEQ_IDX, 0x06);
				wrinx(SEQ_IDX, 0x06, 0x48);
				if (!testinx2(SEQ_IDX, 0x07, 0xF0))
				{
					*Chipset = CHIP_WD_90C00;
				}
				else if (!testinx(SEQ_IDX, 0x10))
				{
					if (testinx2(CRTC_IDX, 0x31, 0x68))
					{
						*Chipset = CHIP_WD_90C22;
					}
					else if (testinx2(CRTC_IDX, 0x31, 0x90))
					{
						*Chipset = CHIP_WD_90C20A;
					}
					else
					{
						*Chipset = CHIP_WD_90C20;
					}
					wrinx(CRTC_IDX, 0x34, 0xA6);
					if (rdinx(CRTC_IDX, 0x32) & 0x20)
					{
						wrinx(CRTC_IDX, 0x34, 0x00);
					}
				}
				else if (testinx2(SEQ_IDX, 0x14, 0x0F))
				{
					old3 = rdinx(CRTC_IDX, 0x34);
					wrinx(CRTC_IDX, 0x34, 0x00);
					ver = ((rdinx(CRTC_IDX, 0x36) << 8) |
					       (rdinx(CRTC_IDX, 0x37)));
					switch (ver)
					{
					case 0x3234:
						*Chipset = CHIP_WD_90C24;
						break;
					case 0x3236:
						*Chipset = CHIP_WD_90C26;
						break;
					case 0x3237:	/* Guess */
						*Chipset = CHIP_WD_90C27;
						break;
					case 0x3330:
						*Chipset = CHIP_WD_90C30;
						break;
					case 0x3331:
						*Chipset = CHIP_WD_90C31;
						break;
					case 0x3333:
						*Chipset = CHIP_WD_90C33;
						break;
					default:
						Chip_data = ver & 0xFF;
						*Chipset = CHIP_WD_UNK;
						break;
					}
					wrinx(CRTC_IDX, 0x34, old3);
				}
				else if (!testinx2(SEQ_IDX, 0x10, 0x04))
				{
					*Chipset = CHIP_WD_90C10;
				}
				else
				{
					*Chipset = CHIP_WD_90C11;
				}
				wrinx(SEQ_IDX, 0x06, old1);
			}
			wrinx(CRTC_IDX, 0x29, old2);
		}
	}
	wrinx(0x3CE, 0x0F, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_WD(Chipset)
int Chipset;
{
	Byte old;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = rdinx(GRC_IDX, 0x0F);
	wrinx(GRC_IDX, 0x0F, 0x05);

	/*
	 * Check
	 */
	switch (rdinx(GRC_IDX, 0x0B) >> 6)
	{
	case 0x00:
	case 0x01:
		Mem = 256;
		break;
	case 0x02:
		Mem = 512;
		break;
	case 0x03:
		Mem = 1024;
		break;
	}

	if ((Chipset >= CHIP_WD_90C33) &&
	    (rdinx(CRTC_IDX, 0x3E) & 0x80))
	{
		Mem = 2048;
	}

	/*
	 * Lock
	 */
	wrinx(GRC_IDX, 0x0F, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
