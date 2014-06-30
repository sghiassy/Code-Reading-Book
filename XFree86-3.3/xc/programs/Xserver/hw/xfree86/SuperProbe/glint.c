/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/GLINT.c,v 3.1.2.1 1997/05/06 13:24:00 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by Dirk Hohndel <hohndel@xfree86.org>
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
 * Except as contained in this notice, the name of Dirk Hohndel shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Dirk Hohndel.
 *
 */

#include "Probe.h"

#define PCI_EN 0x80000000

#ifdef PC98
static Word Ports[] = {0xCF8, 0xCF9, 0xCFC, 0x000 };
#else
static Word Ports[] = {0xCF8, 0xCFA, 0xCFC, 0x000 };
#endif

#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_GLINT __STDCARGS((int));
static int GLINTMem = 0;

Chip_Descriptor GLINT_Descriptor = {
	"GLINT",
	Probe_GLINT,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	FALSE,
	MemProbe_GLINT,
};

Bool Probe_GLINT(Chipset)
int *Chipset;
{
	int	  chipset_passed;
	int	  i = -1;
	Bool	  result = FALSE;
	struct pci_config_reg * pcip;
	unsigned long glint300sx = 0;
	unsigned long glint500tx = 0;
	unsigned long glintdelta = 0;
	unsigned long glintcopro = 0;
	unsigned long basecopro;
	unsigned long base3copro;
	unsigned long basedelta;
	unsigned long *delta_pci_basep;
	unsigned long cmd;
	unsigned long lbsize;

	/*
	 * to be able to detect multiple GLINT chips we need to set
	 * *Chipset to 0 first. We'll set it back to the value passed to
	 * us if nothing was found
	 */
	chipset_passed = *Chipset;
	*Chipset = 0;
	/*
	 * we only check for the GLINT in the PCI config data that we have
	 */
	while (pci_devp[++i] != NULL)
	{
	    pcip = pci_devp[i];
	    if (pcip->_vendor == PCI_VENDOR_3DLABS)
	    {
	    	switch (pcip->_device)
		{
		case PCI_CHIP_3DLABS_300SX:
			*Chipset |= CHIP_300SX;
			glint300sx = PCI_EN |
				(pcip->_pcibuses[pcip->_pcibusidx] << 16) |
				(pcip->_cardnum << 11) | (pcip->_funcnum << 8);
			result = TRUE;
			break;
		case PCI_CHIP_3DLABS_500TX:
			*Chipset |= CHIP_500TX;
			glint500tx = PCI_EN |
				(pcip->_pcibuses[pcip->_pcibusidx] << 16) |
				(pcip->_cardnum << 11) | (pcip->_funcnum << 8);
			result = TRUE;
			break;
		case PCI_CHIP_3DLABS_DELTA:
			*Chipset |= CHIP_DELTA;
			glintdelta = PCI_EN |
				(pcip->_pcibuses[pcip->_pcibusidx] << 16) |
				(pcip->_cardnum << 11) | (pcip->_funcnum << 8);
			delta_pci_basep = &(pcip->_base0);
			result = TRUE;
			break;
		}
	    }
	}
	if (!result)
	{
	    *Chipset = chipset_passed;
	    goto Probe_GLINT_exit;
	}
	/*
	 * due to a few bugs in the GLINT Delta we might have to relocate
	 * the base address of config region of the Delta, if bit 17 of
	 * the base addresses of config region of the Delta and the 500TX
	 * or 300SX are different
	 * We only handle config type 1 at this point
	 */
	if( glintdelta && (glint500tx || glint300sx) )
	{
	    if( glint500tx && glint300sx )
	    {
	    	/*
		 * can't handle if both of them are present
		 */
		goto Probe_GLINT_exit;
	    }
	    glintcopro = glint500tx | glint300sx;
	    
	    outpl(PCI_MODE1_ADDRESS_REG, glintcopro | 0x10); /* base0 */
	    basecopro  = inpl(PCI_MODE1_DATA_REG);
	    outpl(PCI_MODE1_ADDRESS_REG, glintdelta | 0x10);
	    basedelta  = inpl(PCI_MODE1_DATA_REG);
	    outpl(PCI_MODE1_ADDRESS_REG, glintcopro | 0x1c); /* base3 */
	    base3copro  = inpl(PCI_MODE1_DATA_REG);
	    /*
	     * while we're at it, we can figure out the RAM size as well
	     */
	    /*
	     * this number seems to be wrong, it shows 16MB on my board
	     * even though there are definitely just 8MB
	     * ignore it for now
	     */
#if 0
	    outpl(PCI_MODE1_DATA_REG,0xffffffff);
	    lbsize  = inpl(PCI_MODE1_DATA_REG);
	    outpl(PCI_MODE1_DATA_REG,base3copro);
	    lbsize &= 0xfffffff0;
	    lbsize ^= 0xffffffff;
	    lbsize++;
#endif
	    if( (basedelta & 0x20000) ^ (basecopro & 0x20000) )
	    {
	    	/*
		 * if the base addresses are different at bit 17,
		 * we have to remap the base0 for the delta;
		 * as wrong as this looks, we can use the base3 of the
		 * 300SX/500TX for this. The delta is working as a bridge
		 * here and gives its own addresses preference. And we
		 * don't need to access base3, as this one is the bytw
		 * swapped local buffer which we don't need.
		 * Using base3 we know that the space is
		 * a) large enough
		 * b) free (well, almost)
		 */
		if( (basecopro & 0x20000) ^ (base3copro & 0x20000) )
		{
		    /*
		     * oops, still different; we know that base3 is at least
		     * 1 MB, so we just take 128k offset into it
		     */
		    base3copro += 0x20000;
		}
		outpl(PCI_MODE1_ADDRESS_REG, glintdelta | 0x10);
		outpl(PCI_MODE1_DATA_REG,base3copro);
		/*
		 * now update our internal structure accordingly
		 */
		*delta_pci_basep = base3copro;
	    }
	    /*
	     * now make sure that memory space access is enabled on
	     * both chips
	     */
	    outpl(PCI_MODE1_ADDRESS_REG, glintcopro | 0x04);/* command */
	    cmd = inpl(PCI_MODE1_DATA_REG);
	    outpl(PCI_MODE1_ADDRESS_REG, glintcopro | 0x04);
	    outpl(PCI_MODE1_DATA_REG,cmd | 2);
	    outpl(PCI_MODE1_ADDRESS_REG, glintdelta | 0x04);
	    cmd = inpl(PCI_MODE1_DATA_REG);
	    outpl(PCI_MODE1_ADDRESS_REG, glintdelta | 0x04);
	    outpl(PCI_MODE1_DATA_REG,cmd | 2);
	}

Probe_GLINT_exit:

	return(result);
}

static int MemProbe_GLINT(Chipset)
int Chipset;
{
	int		  mem;
	unsigned int	  FBMemoryCtl,LBMemoryCtl;
	Byte		* config;
	Bool		  found = FALSE;
	int		  i = -1;
	int		  j;

	while ((pci_devp[++i] != NULL) && !found)
	{
	    if (pci_devp[i]->_vendor == PCI_VENDOR_3DLABS)
	    {
	    	/*
		 * ignore the Delta chip and use one of the real 
		 * copros for the memory check
		 */
	    	switch (pci_devp[i]->_device)
		{
		case PCI_CHIP_3DLABS_300SX:
		case PCI_CHIP_3DLABS_500TX:
		case PCI_CHIP_3DLABS_DELTA:
			config = MapMem(pci_devp[i]->_base0,0x2000L);
			if (config == 0)
				break;
#ifdef DEBUGPCI
	printf("\npci bus 0x%x cardnum 0x%02x function 0x%02x, vendor 0x%04x device 0x%04x\n",
		pci_devp[i]->_pcibuses[pci_devp[i]->_pcibusidx],
		pci_devp[i]->_cardnum, pci_devp[i]->_funcnum, 
		pci_devp[i]->_vendor, pci_devp[i]->_device);
	printf("mapped base0 from 0x%08x to 0x%08x\n\n",
		pci_devp[i]->_base0, config);
#endif
			LBMemoryCtl = *(int*)&config[0x1000];
			FBMemoryCtl = *(int*)&config[0x1800];
			mem = 1024 * (1 << ((LBMemoryCtl & 0x07000000) >> 24));
			mem <<= 16;
			mem |= 1024 * (1 << ((FBMemoryCtl & 0xe0000000)>> 29));
			UnMapMem(config,0x2000L);
			found = TRUE;
			break;
		}
	    }
	}
	return(mem);
}
