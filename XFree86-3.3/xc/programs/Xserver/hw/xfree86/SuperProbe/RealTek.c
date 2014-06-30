/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/RealTek.c,v 3.4 1996/12/23 06:31:35 dawes Exp $ */
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

/* $XConsortium: RealTek.c /main/5 1996/02/21 17:12:18 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, 0x3D6, 0x3D7};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_RealTek __STDCARGS((int));

Chip_Descriptor RealTek_Descriptor = {
	"RealTek",
	Probe_RealTek,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_RealTek,
};

#ifdef __STDC__
Bool Probe_RealTek(int *Chipset)
#else
Bool Probe_RealTek(Chipset)
int *Chipset;
#endif
{
	Bool result = FALSE;
	Byte ver;

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	if (testinx2(CRTC_IDX, 0x1F, 0x3F) &&
	    tstrg(0x3D6, 0x0F) &&
	    tstrg(0x3D7, 0x0F))
	{
		result = TRUE;
		ver = rdinx(CRTC_IDX, 0x1A) >> 6;
		switch (ver)
		{
		case 0x00:
			*Chipset = CHIP_RT_3103;
			break;
		case 0x01:
			*Chipset = CHIP_RT_3105;
			break;
		case 0x02:
			*Chipset = CHIP_RT_3106;
			break;
		default:
			Chip_data = ver;
			*Chipset = CHIP_RT_UNK;
			break;
		}
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_RealTek(Chipset)
int Chipset;
{
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	switch (Chipset)
	{
	case CHIP_RT_3103:
		switch (rdinx(CRTC_IDX, 0x1E) & 0x03)
		{
		case 0x00:
			Mem = 256;
			break;
		case 0x01:
			Mem = 512;
			break;
		case 0x02:
			Mem = 768;
			break;
		case 0x03:
			Mem = 1024;
			break;
		}
		break;
	case CHIP_RT_3105:
	case CHIP_RT_3106:
		switch (rdinx(CRTC_IDX, 0x1E) & 0x03)
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
		case 0x03:
			Mem = 2048;
			break;
		}
		break;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
