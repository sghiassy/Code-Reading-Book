/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Matrox.c,v 3.2.2.3 1997/05/22 14:00:33 dawes Exp $ */
/*
 * (c) Copyright 1997 Alan Hourihane <alanh@fairlite.demon.co.uk>
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

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, SEQ_IDX, SEQ_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_Matrox __STDCARGS((int));

Chip_Descriptor Matrox_Descriptor = {
	"Matrox",
	Probe_Matrox,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
#if 0
	MemProbe_Matrox,
#else
	NULL,
#endif
};

Bool Probe_Matrox(Chipset)
int *Chipset;
{
        Bool result = FALSE;
	Byte chip, old, old1, val;
	int i = 0;

	if (!NoPCI)
	{
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_MATROX)
		{
			switch (pcrp->_device)
			{
			case PCI_CHIP_MGA1064SG:
				*Chipset = CHIP_MGA1064SG;
				break;
			case PCI_CHIP_MGA2064W:
				*Chipset = CHIP_MGA2064W;
				break;
			case PCI_CHIP_MGA2085PX:
				*Chipset = CHIP_MGA2085PX;
				break;
			default:
				Chip_data = chip;
				*Chipset = CHIP_MATROX_UNK;
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

static int MemProbe_Matrox(Chipset)
int Chipset;
{
	int Mem = 0;

        EnableIOPorts(NUMPORTS, Ports);

	Mem = 2048;

        DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
