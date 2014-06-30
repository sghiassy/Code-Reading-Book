/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/Cirrus.c,v 3.13 1996/12/23 06:31:07 dawes Exp $ */
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

/* $XConsortium: Cirrus.c /main/12 1996/10/25 14:09:56 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, SEQ_IDX, SEQ_REG, GRC_IDX, GRC_REG};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

Chip_Descriptor Cirrus_Descriptor = {
	"Cirrus",
	Probe_Cirrus,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	NULL,
};

static int MemProbe_Cirrus54 __STDCARGS((int));

Chip_Descriptor Cirrus54_Descriptor = {
	"Cirrus54",
	Probe_Cirrus54,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_Cirrus54,
};

static int MemProbe_Cirrus64 __STDCARGS((int));

Chip_Descriptor Cirrus64_Descriptor = {
	"Cirrus64",
	Probe_Cirrus64,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	TRUE,
	MemProbe_Cirrus64,
};

#define CLASS_OLD	0
#define CLASS_54XX	1
#define CLASS_64XX	2

#ifdef __STDC__
static Bool Probe_Cirrus_Class(int *, int);
#else
static Bool Probe_Cirrus_Class();
#endif

Bool Probe_Cirrus(Chipset)
int *Chipset;
{
	return(Probe_Cirrus_Class(Chipset, CLASS_OLD));
}

Bool Probe_Cirrus54(Chipset)
int *Chipset;
{
	return(Probe_Cirrus_Class(Chipset, CLASS_54XX));
}

Bool Probe_Cirrus64(Chipset)
int *Chipset;
{
	return(Probe_Cirrus_Class(Chipset, CLASS_64XX));
}

static Bool Probe_Cirrus_Class(Chipset, Class)
int *Chipset;
int Class;
{
	Bool result = FALSE;
	Byte old, old1, Ver;
        int i;

	if (!NoPCI && Class == CLASS_54XX)
	{
	    i = 0;
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_CIRRUS)
		{
			switch (pcrp->_device)
			{
			case PCI_CHIP_GD5430:
				/* Note: CL-GD5440 has the same ID. */
				*Chipset = CHIP_CL5430;
				break;
			case PCI_CHIP_GD5434_4:
			case PCI_CHIP_GD5434_8:
				*Chipset = CHIP_CL5434;
				break;
			case PCI_CHIP_GD5436:
				*Chipset = CHIP_CL5436;
				break;
			case PCI_CHIP_GD5446:
				*Chipset = CHIP_CL5446;
				break;
			case PCI_CHIP_GD5462:
				*Chipset = CHIP_CL5462;
				break;
			case PCI_CHIP_GD5464:
				*Chipset = CHIP_CL5464;
				break;
			case PCI_CHIP_GD7541:
				*Chipset = CHIP_CL7541;
				break;
			case PCI_CHIP_GD7542:
				*Chipset = CHIP_CL7542;
				break;
			case PCI_CHIP_GD7543:
				*Chipset = CHIP_CL7543;
				break;
			case PCI_CHIP_GD7548:
				*Chipset = CHIP_CL7548;
				break;
			default:
			        /*
			         * The PCI probing only recognizes VGA
			         * devices, so we can be sure it is
			         * a graphics chip.
			         */
				Chip_data = pcrp->_device;
				*Chipset = CHIP_CL_UNKNOWN;
				break;
			}
			PCIProbed = TRUE;
			return(TRUE);
		}
		i++;
	    }
	}

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	if (Class == CLASS_OLD)
	{
		old = rdinx(CRTC_IDX, 0x0C);
		old1 = rdinx(SEQ_IDX, 0x06);
		wrinx(CRTC_IDX,0x0C,0);
		Ver = rdinx(CRTC_IDX, 0x1F);
		wrinx(SEQ_IDX, 0x06, (Ver >> 4) | (Ver << 4));
		if (inp(SEQ_REG) == 0)
		{
			outp(SEQ_REG, Ver);
			if (inp(SEQ_REG) == 1)
			{
				result = TRUE;
				switch (Ver)
				{
				case 0xEC:
					*Chipset = CHIP_CL510;
					break;
				case 0xCA:
					*Chipset = CHIP_CL610;
					break;
				case 0xEA:
					*Chipset = CHIP_CLV7;
					break;
				default:
					Chip_data = Ver;
					*Chipset = CHIP_CL_UNKNOWN;
					break;
				}
			}
		}
		wrinx(SEQ_IDX, 0x06, old1);
		wrinx(CRTC_IDX, 0x0C, old);
	}
	else if (Class == CLASS_54XX)
	{
		/*
		 * Cirrus 542x, 543x, 62x5 chips
	 	 */
		old = rdinx(SEQ_IDX, 0x06);
		wrinx(SEQ_IDX, 0x06, 0x00);
		if (rdinx(SEQ_IDX, 0x06) == 0x0F)
		{
			wrinx(SEQ_IDX, 0x06, 0x12);
			if ((rdinx(SEQ_IDX, 0x06) == 0x12) &&
		    	    (testinx2(SEQ_IDX, 0x1E, 0x3F)))
			{
				result = TRUE;
				Ver = rdinx(CRTC_IDX, 0x27);
				if (testinx(GRC_IDX, 0x09))
				{
					/* 542x */
					switch ((Ver & 0xFC) >> 2)
					{
					case 0x06:
						*Chipset = CHIP_CLAVGA2;
						break;
					case 0x22:
						switch (Ver & 0x03)
						{
						case 0x00:
							*Chipset=CHIP_CL5402;
							break;
						case 0x01:
							*Chipset=CHIP_CL5402R1;
							break;
						case 0x02:
							*Chipset=CHIP_CL5420;
							break;
						case 0x03:
							*Chipset=CHIP_CL5420R1;
							break;
						}
						break;
					case 0x23:
						*Chipset = CHIP_CL5422;
						break;
					case 0x25:
						*Chipset = CHIP_CL5424;
						break;
					case 0x24:
						*Chipset = CHIP_CL5426;
						break;
					case 0x26:
						*Chipset = CHIP_CL5428;
						break;
					case 0x27:
						*Chipset = CHIP_CL5429;
						break;
					case 0x28:
						*Chipset = CHIP_CL5430;
						break;
					case 0x2A:
						*Chipset = CHIP_CL5434;
						break;
					case 0x2B:
						*Chipset = CHIP_CL5436;
						break;
					case 0x2E:
						*Chipset = CHIP_CL5446;
						break;
					case 0x0A: /* guess */
						*Chipset = CHIP_CL7541;
						break;
					case 0x0B:
						*Chipset = CHIP_CL7542;
						break;
					case 0x0C:
						*Chipset = CHIP_CL7543;
						break;
					default:
						Chip_data = Ver;
						*Chipset = CHIP_CL_UNKNOWN;
						break;
					}
					if (*Chipset == CHIP_CL5430 ||
					    *Chipset == CHIP_CL5434 ||
					    *Chipset == CHIP_CL5436 ||
					    *Chipset == CHIP_CL5446)
					{
						/*
						 * Make sure that there is
						 * a sane value in the
						 * Display Compression
						 * Control register, which
						 * may be corrupted by other
						 * probes.
						 */
						unsigned char tmp;
						tmp = rdinx(GRC_IDX, 0x0F);
						wrinx(GRC_IDX, 0x0F,
						      tmp & 0xc0);
					}
				}
				else if (testinx(SEQ_IDX, 0x19))
				{
					/* 62x5 */
					switch ((Ver & 0xC0) >> 6)
					{
					case 0x00:
						*Chipset = CHIP_CL6205;
						break;
					case 0x01:
						*Chipset = CHIP_CL6235;
						break;
					case 0x02:
						*Chipset = CHIP_CL6215;
						break;
					case 0x03:
						*Chipset = CHIP_CL6225;
						break;
					}
				}
				else
				{
					*Chipset = CHIP_CLAVGA2;
				}
			}
		}
		wrinx(SEQ_IDX, 0x06, old);
	}
	else
	{
		/*
		 * The Cirrus 64xx chips.
	 	 */
		old = rdinx(GRC_IDX, 0x0A);
		wrinx(GRC_IDX, 0x0A, 0xCE);
		if (rdinx(GRC_IDX, 0x0A) == 0x00)
		{
			wrinx(GRC_IDX, 0x0A, 0xEC);
			if (rdinx(GRC_IDX, 0x0A) == 0x01)
			{
				result = TRUE;
				Ver = rdinx(GRC_IDX, 0xAA);
				switch ((Ver & 0xF0) >> 4)
				{
				case 0x04:
					*Chipset = CHIP_CL6440;
					break;
				case 0x05:
					*Chipset = CHIP_CL6412;
					break;
				case 0x06:
					*Chipset = CHIP_CL5410;
					break;
				case 0x07:
					if (testinx2(0x3CE, 0x87, 0x90))
					{
						*Chipset = CHIP_CL6420B;
					}
					else
					{
						*Chipset = CHIP_CL6420A;
					}
					break;
				case 0x08:
					*Chipset = CHIP_CL6410;	
					break;
				default:
					Chip_data = Ver;
					*Chipset = CHIP_CL_UNKNOWN;
					break;
				}
			}
		}
		wrinx(GRC_IDX, 0x0A, old);
	}

	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_Cirrus54(Chipset)
int Chipset;
{
	Byte old, SRF;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = rdinx(SEQ_IDX, 0x06);
	wrinx(SEQ_IDX, 0x06, 0x12);

	/*
	 * Check
	 */
	switch (Chipset)
	{
	case CHIP_CL6205:
	case CHIP_CL6215:
	case CHIP_CL6225:
	case CHIP_CL6235:
		Mem = 512;
		break;
	case CHIP_CL5430:
	case CHIP_CL5434:
	case CHIP_CL5436:
	case CHIP_CL5446:
	case CHIP_CL7543:
	case CHIP_CL7548:
		Mem = 512;
		SRF = rdinx(SEQ_IDX, 0x0F);
		if (SRF & 0x10)
			Mem *= 2;
		if ((SRF & 0x18) == 0x18)
			Mem *= 2;
		if (Chipset != CHIP_CL5430 && (SRF & 0x80))
			Mem *= 2;
		break;
	case CHIP_CL5462:
	case CHIP_CL5464:
		/* Read BIOS scratch register. */
		Mem = ((rdinx(SEQ_IDX, 0x14) & 0x07) + 1) * 1024;
		break;
	default:
		/* 542x, use BIOS Scratch Register value */
		switch ((rdinx(SEQ_IDX, 0x0A) & 0x18) >> 3)
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

	/*
	 * Lock
	 */
	wrinx(SEQ_IDX, 0x06, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}

/* ARGSUSED */
static int MemProbe_Cirrus64(Chipset)
int Chipset;
{
	Byte old;
	int Mem = 0;

	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Unlock
	 */
	old = rdinx(GRC_IDX, 0x0A);
	wrinx(GRC_IDX, 0x0A, 0xEC);

	/*
	 * Check
	 */
	switch (rdinx(GRC_IDX, 0xBB) >> 6)
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

	/*
	 * Lock
	 */
	wrinx(GRC_IDX, 0x0A, old);

	DisableIOPorts(NUMPORTS, Ports);
	return(Mem);
}
