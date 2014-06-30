/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/NCR.c,v 3.5 1996/12/23 06:31:19 dawes Exp $ */
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

/* $XConsortium: NCR.c /main/6 1996/10/24 07:09:54 kaleb $ */

#include "Probe.h"

static Word Ports[] = {SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

Chip_Descriptor NCR_Descriptor = {
	"NCR",
	Probe_NCR,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	NULL,
};

Bool Probe_NCR(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old, tmp;

	EnableIOPorts(NUMPORTS, Ports);

	if (testinx2(SEQ_IDX, 0x05, 0x05))
	{
		old = rdinx(SEQ_IDX, 0x05);
		wrinx(SEQ_IDX, 0x05, 0x00);
		if (!testinx2(SEQ_IDX, 0x10, 0xFF))
		{
			wrinx(SEQ_IDX, 0x05, 0x01);
			if (testinx2(SEQ_IDX, 0x10, 0xFF))
			{
				result = TRUE;
				tmp = rdinx(SEQ_IDX, 0x08) >> 4;
				if (tmp == 0)
				{
					*Chipset = CHIP_NCR77C22;
				}
				else if (tmp == 1)
				{
					*Chipset = CHIP_NCR77C21;
				}
				else if (tmp == 2)
				{
					if ((rdinx(SEQ_IDX, 0x08) & 0x0F) >= 8)
					{
						*Chipset = CHIP_NCR77C22EP;
					}
					else
					{
						*Chipset = CHIP_NCR77C22E;
					}
				}
				else if (tmp == 3)
				{
					*Chipset = CHIP_NCR77C32B;
				}
				else
				{
					Chip_data = tmp;
					*Chipset = CHIP_NCR_UNK;
				}
			}
		}
		wrinx(SEQ_IDX, 0x05, old);
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}
