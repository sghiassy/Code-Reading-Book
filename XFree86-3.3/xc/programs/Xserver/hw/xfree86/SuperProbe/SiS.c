/*
 * (c) Copyright 1996 Alan Hourihane <alanh@fairlite.demon.co.uk>
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
 * ALAN HOURIHANE BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Alan Hourihane shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Alan Hourihane.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/SiS.c,v 3.2.4.1 1997/05/06 13:24:48 dawes Exp $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_SiS __STDCARGS((int));

Chip_Descriptor SiS_Descriptor = {
	"SiS",
	Probe_SiS,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_SiS,
};

Bool Probe_SiS(Chipset)
int *Chipset;
{
        Bool result = FALSE;
	Byte chip, old, old1, val;
	int i = 0;

	if (!NoPCI)
	{
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_SIS)
		{
			switch (pcrp->_device)
			{
			case PCI_CHIP_SG86C201:
				*Chipset = CHIP_SIS86C201;
				break;
			case PCI_CHIP_SG86C202:
				*Chipset = CHIP_SIS86C202;
				break;
			case PCI_CHIP_SG86C205:
				*Chipset = CHIP_SIS86C205;
				break;
			default:
				Chip_data = chip;
				*Chipset = CHIP_SIS_UNK;
				break;
			}
			PCIProbed = TRUE;
			return(TRUE);
		}
		i++;
	    }
	}

        return(FALSE);
}

static int MemProbe_SiS(Chipset)
int Chipset;
{
	int Mem = 0;

        EnableIOPorts(NUMPORTS, Ports);

	switch (Chipset)
	{
	case CHIP_SIS86C201:
	case CHIP_SIS86C202:
	case CHIP_SIS86C205:
		switch (rdinx(CRTC_IDX, 0xF) & 0x03)
		{
		case 0x00:
			Mem = 1024;
			break;
		case 0x01:
			Mem = 2048;
			break;
		case 0x02:
			Mem = 4096;
			break;
		}
		break;
	    }

        DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
    }
