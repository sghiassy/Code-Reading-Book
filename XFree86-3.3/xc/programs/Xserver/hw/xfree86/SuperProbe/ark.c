/* $XConsortium: ARK.c /main/3 1996/10/25 21:18:52 kaleb $ */
/*
 * (c) Copyright 1996 Harm Hanemaayer <H.Hanemaayer@inter.nl.net>
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
 * HARM HANEMAAYER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of Harm Hanemaayer shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Harm Hanemaayer.
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/ARK.c,v 3.2 1996/12/27 06:53:24 dawes Exp $ */

#include "Probe.h"

static Word Ports[] = {SEQ_IDX, SEQ_REG, 0x3B4, 0x3B5, 0x3D4, 0x3D5};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_ARK __STDCARGS((int));

Chip_Descriptor ARK_Descriptor = {
	"ARK Logic",
	Probe_ARK,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_ARK,
};

Bool Probe_ARK(Chipset)
int *Chipset;
{
        Bool result = FALSE;
        Byte CR50, savedSR1D;
        int id, rev;
        int i;

	if (!NoPCI)
	{
	    i = 0;
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_ARK)
		{
			switch (pcrp->_device)
			{
			case PCI_CHIP_1000PV:
				*Chipset = CHIP_ARK1000PV;
				break;
			case PCI_CHIP_2000PV:
				*Chipset = CHIP_ARK2000PV;
				break;
			case PCI_CHIP_2000MT:
				*Chipset = CHIP_ARK2000MT;
				break;
			case PCI_CHIP_2000MI:
				*Chipset = CHIP_ARK2000MI;
				break;
			default:
				Chip_data = pcrp->_device;
				*Chipset = CHIP_ARK_UNK;
				break;
			}
			PCIProbed = TRUE;
			return(TRUE);
		}
		i++;
	    }
	}

   	/* Unlock by setting bit 0 of SR1D. */
   	savedSR1D = rdinx(SEQ_IDX, 0x1D);
      	modinx(SEQ_IDX, 0x1D, 0x01, 0x01);

	/*
	 * Check for read/writability of all the "Software Scratch"
	 * registers.
	 */
	if (!testinx(SEQ_IDX, 0x1E)
	|| !testinx(SEQ_IDX, 0x1F)
	|| !testinx(SEQ_IDX, 0x1A)
	|| !testinx(SEQ_IDX, 0x1B)) {
		wrinx(SEQ_IDX, 0x1D, savedSR1D);
		return FALSE;
	}

	CR50 = rdinx(CRTC_IDX, 0x50);
	id = CR50 >> 3;
	rev = CR50 & 7;

	switch (id) {
	/* The ARK1000VL is missing. I don't know the ID. */
	case 0x12 :
		*Chipset = CHIP_ARK1000PV;
		return TRUE;
	case 0x13 :
		*Chipset = CHIP_ARK2000PV;
		return TRUE;
	case 0x14 :
		*Chipset = CHIP_ARK2000MT;
		return TRUE;
	case 0x15 :
		*Chipset = CHIP_ARK2000MI;
		return TRUE;
	default :
		/*
		 * The general ARK check is too weak, so don't assume
		 * it's an ARK chip.
		 */
		break;
	}
   	wrinx(SEQ_IDX, 0x1D, savedSR1D);

        return(FALSE);
}

static int MemProbe_ARK(Chipset)
int Chipset;
{
	int Mem = 0;
	Byte SR10, savedSR1D;

        EnableIOPorts(NUMPORTS, Ports);

   	/* Unlock by setting bit 0 of SR1D. */
   	savedSR1D = rdinx(SEQ_IDX, 0x1D);
      	modinx(SEQ_IDX, 0x1D, 0x01, 0x01);

	SR10 = rdinx(SEQ_IDX, 0x10);
	if (Chipset == CHIP_ARK1000PV)
		if ((SR10 & 0x40) == 0)
			Mem = 1024;
		else
			Mem = 2048;
	if (Chipset == CHIP_ARK2000PV || Chipset == CHIP_ARK2000MT
	|| Chipset == CHIP_ARK2000MI)
		if ((SR10 & 0xC0) == 0)
			Mem = 1024;
		else
		if ((SR10 & 0xC0) == 0x40)
			Mem = 2048;
		else
			Mem = 4096;

   	wrinx(SEQ_IDX, 0x1D, savedSR1D);

        DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
