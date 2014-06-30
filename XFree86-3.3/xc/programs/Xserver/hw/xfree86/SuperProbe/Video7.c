/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Video7.c,v 3.5 1996/12/23 06:31:44 dawes Exp $ */
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

/* $XConsortium: Video7.c /main/6 1996/10/24 07:10:14 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

Chip_Descriptor Video7_Descriptor = {
	"Video7",
	Probe_Video7,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	NULL,
};

Bool Probe_Video7(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old, old1;
	Word id;

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	old = rdinx(SEQ_IDX, 0x06);
	wrinx(SEQ_IDX, 0x06, 0xEA);		/* enable extensions */
	old1 = rdinx(CRTC_IDX, 0x0C);
	wrinx(CRTC_IDX, 0x0C, 0x55);
	id = rdinx(CRTC_IDX, 0x1F);
	wrinx(CRTC_IDX, 0x0C, old1);
	if (id == (0x55 ^ 0xEA))
	{
		/*
		 * It's Video7
		 */
		result = TRUE;
		id = (rdinx(SEQ_IDX, 0x8F) << 8) | rdinx(SEQ_IDX, 0x8E);
		if ((id >= 0x7000) && (id <= 0x70FF))
		{
			*Chipset = CHIP_V7_FWRITE;
		}
		else if ((id >= 0x7140) && (id <= 0x714F))
		{
			*Chipset = CHIP_V7_1024i;
		}
		else if (id == 0x7151)
		{
			*Chipset = CHIP_V7_VRAM2_B;
		}
		else if (id == 0x7152)
		{
			*Chipset = CHIP_V7_VRAM2_C;
		}
		else if (id == 0x7760)
		{
			*Chipset = CHIP_HT216BC;
		}
		else if (id == 0x7763)
		{
			*Chipset = CHIP_HT216D;
		}
		else if (id == 0x7764)
		{
			*Chipset = CHIP_HT216E;
		}
		else if (id == 0x7765)
		{
			*Chipset = CHIP_HT216F;
		}
		else if ((id >= 0x8000) && (id < 0xFFFF))
		{
			*Chipset = CHIP_V7_VEGA;
		}
		else
		{
			Chip_data = id;
			*Chipset = CHIP_V7_UNKNOWN;
		}
	}
	wrinx(SEQ_IDX, 6, old);		/* disable extensions */
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}
