/*
 * (c) Copyright 1997 Harald Koenig <koenig@XFree86.Org>
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
 * JOSEPH MOSS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Joseph Moss shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Joseph Moss.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/SigmaDesigns.c,v 3.1.2.1 1997/05/06 13:24:50 dawes Exp $ */

#include "Probe.h"

static Word Ports[] = {0x000, };
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_SigmaDesigns __STDCARGS((int));

Chip_Descriptor SigmaDesigns_Descriptor = {
	"SigmaDesigns",
	Probe_SigmaDesigns,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	NULL,
};

Bool Probe_SigmaDesigns(Chipset)
int *Chipset;
{
        Bool result = FALSE;
	Byte chip, old, old1, val;
	int i = 0;

	if (!NoPCI)
	{
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_SIGMADESIGNS)
		{
			switch (pcrp->_device)
			{
			case PCI_CHIP_SD_REALMAGIG64GX:
				*Chipset = CHIP_SD_RM64GX;
				break;
			default:
				Chip_data = chip;
				*Chipset = CHIP_SD_RM_UNK;
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


