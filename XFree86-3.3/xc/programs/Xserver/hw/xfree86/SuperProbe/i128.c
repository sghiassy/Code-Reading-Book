/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/I128.c,v 3.6 1997/01/03 07:59:20 dawes Exp $ */
/*
 * (c) Copyright 1993,1994 by Robin Cutshaw <robin@xfree86.org>
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
 * Except as contained in this notice, the name of Robin Cutshaw shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from Robin Cutshaw.
 *
 */

/* $XConsortium: I128.c /main/6 1996/10/19 17:46:43 kaleb $ */

#include "Probe.h"

#ifdef PC98
static Word Ports[] = {0xCF8, 0xCF9, 0xCFC, 0x000 };
#else
static Word Ports[] = {0xCF8, 0xCFA, 0xCFC, 0x000 };
#endif

#define NUMPORTS (sizeof(Ports)/sizeof(Word))

#define PCI_EN 0x80000000

static int MemProbe_I128 __STDCARGS((int));
static int I128Mem = 0;

Chip_Descriptor I128_Descriptor = {
	"I128",
	Probe_I128,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	FALSE,
	MemProbe_I128,
};

Bool Probe_I128(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Word ioaddr, vendor, device, iobase;
	Long id, vendordevice, tmplong1, tmplong2, pcibus, cardnum;
	Byte tmp1, tmp2, configtype = 0;

	EnableIOPorts(2, Ports);

	outp(PCI_MODE2_ENABLE_REG, 0x00);
	outp(PCI_MODE2_FORWARD_REG, 0x00);
	tmp1 = inp(PCI_MODE2_ENABLE_REG);
	tmp2 = inp(PCI_MODE2_FORWARD_REG);
	if ((tmp1 == 0x00) && (tmp2 == 0x00))
	    configtype = 2;
	else {
	    tmplong1 = inpl(PCI_MODE1_ADDRESS_REG);
	    outpl(PCI_MODE1_ADDRESS_REG, PCI_EN);
	    tmplong2 = inpl(PCI_MODE1_ADDRESS_REG);
	    if (tmplong2 == PCI_EN);
		configtype = 1;
	    outpl(PCI_MODE1_ADDRESS_REG, tmplong1);
	}

	DisableIOPorts(2, Ports);

	if (configtype == 0)
	    return(FALSE);
	else if (configtype == 1) {
	    EnableIOPorts(3, Ports);
	    for (pcibus = 0x000000; pcibus < 0x100000; pcibus += 0x10000) {
		for (cardnum = 0x00000; cardnum < 0x10000; cardnum += 0x0800) {
		    outpl(PCI_MODE1_ADDRESS_REG, PCI_EN | pcibus | cardnum);
		    tmplong1 = inpl(PCI_MODE1_DATA_REG);
		    vendor = (unsigned short )(tmplong1 & 0xFFFF);
		    device = (unsigned short )((tmplong1 >> 16) & 0xFFFF);
#ifdef DEBUG
	printf("pci bus/card 0x%08x, vendor 0x%04x device 0x%04x\n",
	    pcibus | cardnum, vendor, device);
#endif
		    if ((vendor == 0x105D) &&
		        ((device == 0x2309) || (device == 0x2339))) {
			outpl(PCI_MODE1_ADDRESS_REG, PCI_EN | pcibus | cardnum | 0x24);
			iobase = inpl(PCI_MODE1_DATA_REG) & 0xFFFFFF00;

			DisableIOPorts(3, Ports);
			Ports[3] = iobase + 0x18;
			EnableIOPorts(4, Ports);

			id = inpl(iobase + 0x18);
			DisableIOPorts(4, Ports);

	                switch (id & 0xC0) {
	                    case 0x00:
	                        I128Mem =  4096;
	                        break;
	                    case 0x40:
	                        I128Mem =  8192;
	                        break;
	                    case 0x80:
	                        I128Mem =  16384;
	                        break;
	                    case 0xC0:
	                        I128Mem =  32768;
	                        break;
	                }

	                *Chipset = CHIP_I128;
			return(TRUE);
		    }
		}
	    }
	    /* not found */
	    DisableIOPorts(3, Ports);
	    return(FALSE);
	}

	/* else configtype == 2 */

	EnableIOPorts(2, Ports);
	outp(PCI_MODE2_ENABLE_REG, 0x80);
	outp(PCI_MODE2_FORWARD_REG, 0x00);
	DisableIOPorts(2, Ports);

	for (ioaddr = 0xC000; ioaddr < 0xD000; ioaddr += 0x0100) {
	    Ports[2] = ioaddr;
	    Ports[3] = ioaddr + 0x24;
	    EnableIOPorts(NUMPORTS, Ports);

	    vendordevice = inpl(ioaddr);
            vendor = (unsigned short )(vendordevice & 0xFFFF);
            device = (unsigned short )((vendordevice >> 16) & 0xFFFF);

#ifdef DEBUG
	printf("pci slot at 0x%04x, vendor 0x%04x device 0x%04x\n",
	    ioaddr, vendor, device);
#endif

	    if ((vendor != 0x105D) ||
		((device != 0x2309) && (device != 0x2339))) {
	        DisableIOPorts(NUMPORTS, Ports);
	        continue;
	    }

            iobase = inpl(ioaddr + 0x24) & 0xFFFFFF00;

	    DisableIOPorts(NUMPORTS, Ports);
	    Ports[2] = iobase + 0x18;
	    Ports[3] = 0x000;
	    EnableIOPorts(NUMPORTS, Ports);

	    id = inpl(iobase + 0x18) & 0x7FFFFFFF;
	    switch (id & 0xC0) {
	        case 0x00:
	            I128Mem =  4096;
	            break;
	        case 0x40:
	            I128Mem =  8192;
	            break;
	        case 0x80:
	            I128Mem =  16384;
	            break;
	        case 0xC0:
	            I128Mem =  32768;
	            break;
	    }

	    *Chipset = CHIP_I128;
	    result = TRUE;
	    DisableIOPorts(NUMPORTS, Ports);
	    Ports[3] = 0x0000;
	    break;
	}

	Ports[2] = Ports[3] = 0x0000;
	EnableIOPorts(NUMPORTS, Ports);
	outp(PCI_MODE2_ENABLE_REG, 0x00);
	DisableIOPorts(NUMPORTS, Ports);

	return(result);
}

static int MemProbe_I128(Chipset)
int Chipset;
{
	return(I128Mem);
}
