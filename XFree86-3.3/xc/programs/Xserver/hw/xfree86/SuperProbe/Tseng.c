/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Tseng.c,v 3.7.2.1 1997/05/06 13:24:52 dawes Exp $ */ 
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

/* $XConsortium: Tseng.c /main/7 1996/10/19 17:48:18 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, 0x000, 0x000, 0x000,
                       0x3BF, 0x3CB, 0x3CD, 
		       ATR_IDX, ATR_REG_R, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_Tseng __STDCARGS((int));
static Long ET6Kbase = 0;

Chip_Descriptor Tseng_Descriptor = {
	"Tseng",
	Probe_Tseng,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_Tseng,
};

Bool Probe_Tseng(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte old, old1, ver;
	int i=0;

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	Ports[2] = inp(MISC_OUT_R) & 0x01 ? 0x3D8 : 0x3B8;

        /* check PCI config first */
	if (!NoPCI)
	{
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_TSENG)
		{
			switch (pcrp->_device)
			{
			case PCI_CHIP_ET6000:
				*Chipset = CHIP_ET6K;
				ET6Kbase = pcrp->_base1 & ~0xFF;
				break;
                        case PCI_CHIP_ET4000_W32P_A:
				*Chipset = CHIP_ET4KW32P_A;
				break;
                        case PCI_CHIP_ET4000_W32P_B:
				*Chipset = CHIP_ET4KW32P_B;
				break;
                        case PCI_CHIP_ET4000_W32P_C:
				*Chipset = CHIP_ET4KW32P_C;
				break;
                        case PCI_CHIP_ET4000_W32P_D:
				*Chipset = CHIP_ET4KW32P_D;
				break;
			default: 
				Chip_data = pcrp->_device;
				*Chipset = CHIP_TSENG_UNK;
				break;
			}
			PCIProbed = TRUE;
			return(TRUE);
		}
		i++;
	    }
	}

	EnableIOPorts(NUMPORTS, Ports);

	old = inp(0x3BF);
	old1 = inp(Ports[2]);
	outp(0x3BF, 0x03);
	outp(Ports[2], 0xA0);
	if (tstrg(0x3CD, 0x3F)) 
	{
		result = TRUE;
		if (testinx2(CRTC_IDX, 0x33, 0x0F))
		{
			if (tstrg(0x3CB, 0x33))
			{
				ver = rdinx(0x217A, 0xEC);
				switch (ver >> 4)
				{
				case 0x00:
					*Chipset = CHIP_ET4000W32;
					break;
				case 0x01:
					*Chipset = CHIP_ET4000W32I;
					break;
				case 0x02:
					*Chipset = CHIP_ET4KW32P_A;
					break;
				case 0x03:
					*Chipset = CHIP_ET4KW32I_B;
					break;
				case 0x05:
					*Chipset = CHIP_ET4KW32P_B;
					break;
				case 0x06:
					*Chipset = CHIP_ET4KW32P_D;
					break;
				case 0x07:
					*Chipset = CHIP_ET4KW32P_C;
					break;
				case 0x0B:
					*Chipset = CHIP_ET4KW32I_C;
					break;
				default:
					Chip_data = ver >> 4;
					*Chipset = CHIP_TSENG_UNK;
					break;
				}
			}
			else
			{
				*Chipset = CHIP_ET4000;
			}
		}
		else
		{
			*Chipset = CHIP_ET3000;
		}
	}
	outp(Ports[2], old1);
	outp(0x3BF, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_Tseng(Chipset)
int Chipset;
{
	Byte Save[2];
	int Mem = 0;

        if (PCIProbed)
        {
        Ports[3] = ET6Kbase + 0x45;
        Ports[4] = ET6Kbase + 0x47;
        }
	EnableIOPorts(NUMPORTS, Ports);

	/* 
	 * Unlock 
	 */
	Save[0] = inp(0x3BF);
	Save[1] = inp(vgaIOBase + 8);
	outp(0x3BF, 0x03);
	outp(vgaIOBase + 8, 0xA0);

	/*
	 * Check
	 */
	switch (Chipset)
	{
	case CHIP_ET3000:
		Mem = 512;
		break;
	case CHIP_ET4000:
		switch (rdinx(CRTC_IDX, 0x37) & 0x0B)
		{
		case 0x03:
		case 0x05:
			Mem = 256;
			break;
		case 0x0A:
			Mem = 512;
			break;
		case 0x0B:
			Mem = 1024;
			break;
		}
		break;
	case CHIP_ET4000W32:
	case CHIP_ET4000W32I:
	case CHIP_ET4KW32P_A:
	case CHIP_ET4KW32I_B:
	case CHIP_ET4KW32I_C:
	case CHIP_ET4KW32P_B:
	case CHIP_ET4KW32P_C:
	case CHIP_ET4KW32P_D:
		switch (rdinx(CRTC_IDX, 0x37) & 0x09)
		{
		case 0x00:
			Mem = 2048;
			break;
		case 0x01:
			Mem = 4096;
			break;
		case 0x08:
			Mem = 512;
			break;
		case 0x09:
			Mem = 1024;
			if ((Chipset != CHIP_ET4000W32) &&
			   (rdinx(CRTC_IDX, 0x32) & 0x80))
			    Mem = 2048;
			break;
		}
		break;
	case CHIP_ET6K:
		switch (rdinx(CRTC_IDX, 0x34) & 0x80)
		{
		case 0x00:  /* MDRAM */
			Mem = ((inp(ET6Kbase+0x47) & 0x07) + 1) * 8*32; /* number of 8 32kb banks  */
			if (inp(ET6Kbase+0x45) & 0x04)
			{
			Mem <<= 1;
			}
			break;
		case 0x80:  /* DRAM */
			Mem = 1024 << (inp(ET6Kbase+0x45) & 0x03);
			break;
		}
	}

	/* 
	 * Lock 
	 */
	outp(vgaIOBase + 8, Save[1]);
	outp(0x3BF, Save[0]);
	DisableIOPorts(NUMPORTS, Ports);

	return(Mem);
}
