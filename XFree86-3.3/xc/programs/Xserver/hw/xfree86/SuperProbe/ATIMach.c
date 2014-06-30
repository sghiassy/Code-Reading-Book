/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/ATIMach.c,v 3.9.2.2 1997/05/22 14:00:33 dawes Exp $ */
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

/* $XConsortium: ATIMach.c /main/10 1996/10/25 07:00:15 kaleb $ */

#include "Probe.h"

static Word Ports[] = {ROM_ADDR_1, DESTX_DIASTP, READ_SRC_X,
		       CONFIG_STATUS_1, MISC_OPTIONS, GP_STAT};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_ATIMach __STDCARGS((int));
extern void Probe_ATI_ChipID __STDCARGS((int, int *));

Chip_Descriptor ATIMach_Descriptor = {
	"ATI_Mach",
	Probe_ATIMach,
	Ports,
	NUMPORTS,
	TRUE,
	FALSE,
	FALSE,
	MemProbe_ATIMach,
};

#define WaitIdleEmpty() { int i; \
			  for (i=0; i < 100000; i++) \
				if (!(inpw(GP_STAT) & (GPBUSY | 1))) \
					break; \
			}

/* ATI Mach64 I/O port addresses */
Word ATIMach64MEM_INFO,
     ATIMach64SCRATCH_REG1,
     ATIMach64DAC_CNTL,
     ATIMach64CONFIG_CHIP_ID;

#ifdef __STDC__
static void Probe_ATIMach64(int *Chipset,
			    unsigned short int IOBase,
			    Bool SparseIO)
#else
static void Probe_ATIMach64(Chipset, IOBase, SparseIO)
int *Chipset;
unsigned short int IOBase;
Bool SparseIO;
#endif
{
	Long tmp;
	Word IOPort;
	int chip;

	if ((*Chipset != -1) || (IOBase == 0))
		return;

	/* Determine if a Mach64 answers the call */
	IOPort = (SparseIO ? 0x4000 : 0x0080) + IOBase;
	EnableIOPorts(1, &IOPort);
	tmp = inpl(IOPort);
	outpl(IOPort, 0x55555555);		/* Test odd bits */
	if (inpl(IOPort) == 0x55555555)
	{
		outpl(IOPort, 0xAAAAAAAA);	/* Test even bits */
		if (inpl(IOPort) == 0xAAAAAAAA)
		{
			/*
			 * Fix I/O ports.  I know, I know:  hard-wired
			 * constants are *EVIL*, but this'll do for now.
			 */
			if (SparseIO)
			{
				ATIMach64SCRATCH_REG1 = 0x4400;
				ATIMach64MEM_INFO = 0x5000;
				ATIMach64DAC_CNTL = 0x6000;
				ATIMach64CONFIG_CHIP_ID = 0x6C00;
			}
			else
			{
				ATIMach64SCRATCH_REG1 = 0x0084;
				ATIMach64MEM_INFO = 0x00B0;
				ATIMach64DAC_CNTL = 0x00C4;
				ATIMach64CONFIG_CHIP_ID = 0x00E0;
			}
			ATIMach64SCRATCH_REG1 += IOBase;
			ATIMach64MEM_INFO += IOBase;
			ATIMach64DAC_CNTL += IOBase;
			ATIMach64CONFIG_CHIP_ID += IOBase;

			/*
			 * Something's responding to our hail.  Make sure it's
			 * a Mach64.  This assumes ATI won't be producing any
			 * more adapters that don't register themselves in PCI
			 * configuration space.
			 */
			Probe_ATI_ChipID(CHIP_MACH64, &chip);
			if ((chip == CHIP_ATI_UNK) && SparseIO)
				Chip_data = (Long)~0;
			else
				*Chipset = CHIP_MACH64;
		}
	}
	outpl(IOPort, tmp);
	DisableIOPorts(1, &IOPort);
}

#ifdef __STDC__
Bool Probe_ATIMach(int *Chipset)
#else
Bool Probe_ATIMach(Chipset)
int *Chipset;
#endif
{
	Long tmp;
	static int chip = -1;
	static Bool Already_Called = FALSE;
	struct pci_config_reg *PCIDevice;
	int Index;

	if (Already_Called)
	{
		if (chip != -1)
			*Chipset = chip;
		return (chip != -1);
	}
	Already_Called = TRUE;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Check for 8514/A registers.  Don't read BIOS, or an attached 8514
	 * Ultra won't be detected (the slave SVGA's BIOS is in the normal SVGA
	 * place).
	 */
	tmp = inpw(ROM_ADDR_1);
	outpw(ROM_ADDR_1, 0x5555);
	WaitIdleEmpty();
	if (inpw(ROM_ADDR_1) == 0x5555)
	{
		outpw(ROM_ADDR_1, 0x2A2A);
		WaitIdleEmpty();
		if (inpw(ROM_ADDR_1) == 0x2A2A)
			chip = CHIP_8514;
	}
	outpw(ROM_ADDR_1, tmp);
	if (chip != -1)
	{
		/*
		 * An 8514 accelerator is really present;  now figure
		 * out which one.
		 */
		outpw(DESTX_DIASTP, 0xAAAA);
		WaitIdleEmpty();
		if (inpw(READ_SRC_X) != 0x02AA)
			chip = CHIP_MACH8;
		else
			chip = CHIP_MACH32;
		outpw(DESTX_DIASTP, 0x5555);
		WaitIdleEmpty();
		if (inpw(READ_SRC_X) != 0x0555)
		{
			if (chip != CHIP_MACH8)
				/*
				 * Something bizarre is happening.
				 */
				chip = -1;
		}
		else
		{
			if (chip != CHIP_MACH32)
				/*
				 * Something bizarre is happening.
				 */
				chip = -1;
		}
	}

	DisableIOPorts(NUMPORTS, Ports);

	if (chip == -1)
	{
		/* 
		 * Check for a Mach64.  Start with sparse I/O base addresses,
		 * then move on to PCI information.
		 */
		Probe_ATIMach64(&chip, 0x02EC, TRUE);
		Probe_ATIMach64(&chip, 0x01CC, TRUE);
		Probe_ATIMach64(&chip, 0x01C8, TRUE);

		Index = 0;
		while ((chip == -1) && (PCIDevice = pci_devp[Index++]))
		{
			if (PCIDevice->_vendor != PCI_VENDOR_ATI)
				continue;
			if (PCIDevice->_device == PCI_CHIP_MACH32)
				continue;
			Probe_ATIMach64(&chip, PCIDevice->_base1 & 0xFF00,
				FALSE);
		}
	}

	if (chip != -1)
	{
		*Chipset = chip;
	}

	return(chip != -1);
}

#ifdef __STDC__
static int MemProbe_ATIMach(int Chipset)
#else
static int MemProbe_ATIMach(Chipset)
int Chipset;
#endif
{
	static int Mem = 0;
	static Bool Already_Called = FALSE;

	if (Already_Called)
		return (Mem);
	Already_Called = TRUE;

	EnableIOPorts(NUMPORTS, Ports);
	if (Chipset == CHIP_MACH8)
	{
		if (inpw(CONFIG_STATUS_1) & 0x0020)
		{
			Mem = 1024;
		}
		else
		{
			Mem = 512;
		}
	}
	else if (Chipset == CHIP_MACH32)
	{
		switch ((inpw(MISC_OPTIONS) & 0x000C) >> 2)
		{
		case 0x00:
			Mem = 512;
			break;
		case 0x01:
			Mem = 1024;
			break;
		case 0x02:
			Mem = 2048;
			break;
		case 0x03:
			Mem = 4096;
			break;
		}
	}
	else if (Chipset == CHIP_MACH64)
	{
		extern Bool Mach64xTB;
		int tmp;

		EnableIOPorts(1, &ATIMach64MEM_INFO);
		tmp = inpl(ATIMach64MEM_INFO);
		if (!Mach64xTB) {
			switch (tmp & 0x00000007)
			{
			case 0x00:
				Mem = 512;
				break;
			case 0x01:
				Mem = 1024;
				break;
			case 0x02:
				Mem = 2048;
				break;
			case 0x03:
				Mem = 4096;
				break;
			case 0x04:
				Mem = 6144;
				break;
			case 0x05:
				Mem = 8192;
				break;
			case 0x06:
				Mem = 12288;
				break;
			case 0x07:
				Mem = 16384;
				break;
			}
		} else {
			if ((tmp &= 0x0000000F) < 8)
				Mem = (tmp + 1) * 512;
			else if (tmp < 12)
				Mem = (tmp - 3) * 1024;
			else
				Mem = (tmp - 7) * 2048;
		}
		DisableIOPorts(1, &ATIMach64MEM_INFO);
	}
	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
