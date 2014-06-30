/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/S3.c,v 3.15.2.1 1997/05/06 13:24:45 dawes Exp $ */
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

/* $XConsortium: S3.c /main/12 1996/10/25 11:33:27 kaleb $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

static int MemProbe_S3 __STDCARGS((int));

Chip_Descriptor S3_Descriptor = {
	"S3",
	Probe_S3,
	Ports,
	NUMPORTS,
	FALSE,
	FALSE,
	FALSE,
	MemProbe_S3,
};

Bool Probe_S3(Chipset)
int *Chipset;
{
	Bool result = FALSE;
	Byte chip, cr38, cr39, cr42, tmp, rev;
	int i = 0;
	struct pci_config_reg *pcrp2;
	if (!NoPCI)
	{
	    while ((pcrp = pci_devp[i]) != (struct pci_config_reg *)NULL) {
		if (pcrp->_vendor == PCI_VENDOR_S3)
		{ 
		        Ports[0] = CRTC_IDX;
			Ports[1] = CRTC_REG;
			EnableIOPorts(NUMPORTS, Ports);
			cr38 = rdinx(CRTC_IDX, 0x38);
			cr39 = rdinx(CRTC_IDX, 0x39);
			wrinx(CRTC_IDX, 0x38, 0x48);
			wrinx(CRTC_IDX, 0x39, 0xa5);

			switch (pcrp->_device)
			{
			case PCI_CHIP_TRIO:
#if 1  /* can only be distinguished using port probing... */
			        PCIProbed = FALSE;
#else
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_Trio32; 
				*Chipset = CHIP_S3_Trio64V; 
				*Chipset = CHIP_S3_Trio64; 
#endif
				break;
			case PCI_CHIP_868:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_868;
				break;
			case PCI_CHIP_928:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_928P;
				break;
			case PCI_CHIP_864_0:
			case PCI_CHIP_864_1:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_864;
				break;
			case PCI_CHIP_964_0:
			case PCI_CHIP_964_1:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_964;
				break;
			case PCI_CHIP_968:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_968;

				/* probe for 3Dlabs chip in same PCI slot for ELSA Gloria */
				cr42 = rdinx(CRTC_IDX, 0x42);
				wrinx(CRTC_IDX, 0x42, cr42 | 0x08);
				xf86scanpci();
				if ((pcrp2 = pci_devp[i]) != (struct pci_config_reg *)NULL) {
				   if (pcrp2->_vendor == PCI_VENDOR_3DLABS) {
				      switch (pcrp2->_device) {
				      case PCI_CHIP_3DLABS_300SX:
					 *Chipset = CHIP_S3_968_3DLABS_300SX;
					 break;
				      default:
					 *Chipset = CHIP_S3_968_3DLABS_UNK;
					 break;
				      }
				   }
				}				      
				xf86scanpci();

				break;
			case PCI_CHIP_ViRGE:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_ViRGE;
				/*
				 * the ViRGE is used as VGA part for
				 * the GLINT
				 */
				S3_Descriptor.check_coproc = TRUE;
				break;
			case PCI_CHIP_ViRGE_DXGX:
			        PCIProbed = TRUE;
				if (rdinx(CRTC_IDX, 0x6f) & 1)
				   *Chipset = CHIP_S3_ViRGE_GX;
				else
				   *Chipset = CHIP_S3_ViRGE_DX;
				break;
			case PCI_CHIP_ViRGE_VX:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_ViRGE_VX;
				break;
			case PCI_CHIP_AURORA64VP:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_Aurora64VP;
				break;
			case PCI_CHIP_TRIO64UVP:
			        PCIProbed = TRUE;
				*Chipset = CHIP_S3_Trio64UVP;
				break;
			case PCI_CHIP_TRIO64V2_DXGX:
			        PCIProbed = TRUE;
				if (rdinx(CRTC_IDX, 0x6f) & 1)
				   *Chipset = CHIP_S3_Trio64V2_GX;
				else
				   *Chipset = CHIP_S3_Trio64V2_DX;
				break;
#if 0  /* use port probing then... */
			default:
			        PCIProbed = TRUE;
				Chip_data = chip;
				*Chipset = CHIP_S3_UNKNOWN;
				break;
#endif
			}

			wrinx(CRTC_IDX, 0x39, cr39);
			wrinx(CRTC_IDX, 0x38, cr38);
			DisableIOPorts(NUMPORTS, Ports);
			if (PCIProbed)
			   return(TRUE);
		}
		i++;
	    }
	}

	/* Add CRTC to enabled ports */
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);
	cr38 = rdinx(CRTC_IDX, 0x38);
	cr39 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x00);
	if (!testinx2(CRTC_IDX, 0x35, 0x0F))
	{
		wrinx(CRTC_IDX, 0x38, 0x48);
		if (testinx2(CRTC_IDX, 0x35, 0x0F))
		{
			result = TRUE;
			wrinx(CRTC_IDX, 0x39, 0xa5);
			rev = rdinx(CRTC_IDX, 0x30);
			switch (rev & 0xF0)
			{
			case 0x80:
				switch (rev & 0x0F)
				{
				case 0x01:
					*Chipset = CHIP_S3_911;
					break;
				case 0x02:
					*Chipset = CHIP_S3_924;
					break;
				default:
					Chip_data = rev;
					*Chipset = CHIP_S3_UNKNOWN;
					break;
				}
				break;
			case 0xA0:
				tmp = rdinx(CRTC_IDX, 0x36);
				switch (tmp & 0x03)
				{
				case 0x00:
				case 0x01:
					/* EISA or VLB - 805 */
					switch (rev & 0x0F)
					{
					case 0x00:
						*Chipset = CHIP_S3_805B;
						break;
					case 0x01:
						Chip_data = rev;
						*Chipset = CHIP_S3_UNKNOWN;
						break;
					case 0x02:
					case 0x03:
					case 0x04:
						*Chipset = CHIP_S3_805C;
						break;
					case 0x05:
						*Chipset = CHIP_S3_805D;
						break;
					case 0x08:
						*Chipset = CHIP_S3_805I;
						break;
					default:
						/* Call >0x05 D step for now */
						*Chipset = CHIP_S3_805D;
						break;
					}
					break;
				case 0x03:
					/* ISA - 801 */
					switch (rev & 0x0F)
					{
					case 0x00:
						*Chipset = CHIP_S3_801B;
						break;
					case 0x01:
						Chip_data = rev;
						*Chipset = CHIP_S3_UNKNOWN;
						break;
					case 0x02:
					case 0x03:
					case 0x04:
						*Chipset = CHIP_S3_801C;
						break;
					case 0x05:
						*Chipset = CHIP_S3_801D;
						break;
					case 0x08:
						*Chipset = CHIP_S3_801I;
						break;
					default:
						/* Call >0x05 D step for now */
						*Chipset = CHIP_S3_801D;
						break;
					}
					break;
				default:
					Chip_data = rev;
					*Chipset = CHIP_S3_UNKNOWN;
					break;
				}
				break;
			case 0x90:
				switch (rev & 0x0F)
				{
				case 0x00:
				case 0x01:
					/*
					 * Contradictory documentation -
					 * one says 0, the other says 1.
					 */
					*Chipset = CHIP_S3_928D;
					break;
				case 0x02:
				case 0x03:
					Chip_data = rev;
					*Chipset = CHIP_S3_UNKNOWN;
					break;
				case 0x04:
				case 0x05:
					*Chipset = CHIP_S3_928E;
					break;
				default:
					/* Call >0x05 E step for now */
					*Chipset = CHIP_S3_928E;
				}
				break;
			case 0xB0:
				/*
				 * Don't know anything more about this
				 * just yet.
				 */
				*Chipset = CHIP_S3_928P;
				break;
			case 0xC0:
				*Chipset = CHIP_S3_864;
				break;
			case 0xD0:
				*Chipset = CHIP_S3_964;
				break;
			case 0xE0: {
			   int chip_id, chip_rev;
			   chip_id  = rdinx(CRTC_IDX, 0x2d) << 8;
			   chip_id |= rdinx(CRTC_IDX, 0x2e);
			   chip_rev = rdinx(CRTC_IDX, 0x2f);
			   if      (chip_id == 0x8880)
			      *Chipset = CHIP_S3_866;
			   else if (chip_id == 0x8890) 
			      *Chipset = CHIP_S3_868;
			   else if (chip_id == 0x8810) 
			      *Chipset = CHIP_S3_Trio32;
			   else if (chip_id == PCI_CHIP_TRIO)
			      if ((chip_rev&0x40)  ==  0x40)
				 *Chipset = CHIP_S3_Trio64V;
			      else
				 *Chipset = CHIP_S3_Trio64;
			   else if (chip_id == PCI_CHIP_968) 
			      *Chipset = CHIP_S3_968;
			   else if (chip_id == PCI_CHIP_ViRGE) 
			      *Chipset = CHIP_S3_ViRGE;
			   else if (chip_id == PCI_CHIP_ViRGE_VX) 
			      *Chipset = CHIP_S3_ViRGE_VX;
			   else if (chip_id == PCI_CHIP_PLATO_PX) 
			      *Chipset = CHIP_S3_PLATO_PX;
			   else if (chip_id == PCI_CHIP_TRIO64V2_DXGX)
			      if (rdinx(CRTC_IDX, 0x6f) & 1)
				 *Chipset = CHIP_S3_Trio64V2_GX;
			      else
				 *Chipset = CHIP_S3_Trio64V2_DX;
			   else if (chip_id == 0x8a01)
			      if (rdinx(CRTC_IDX, 0x6f) & 1)
				 *Chipset = CHIP_S3_ViRGE_GX;
			      else
				 *Chipset = CHIP_S3_ViRGE_DX;
			   else {
			      Chip_data = rev;
			      Chip_data = (Chip_data << 16) | chip_id;
			      Chip_data = (Chip_data <<  8) | chip_rev;
			      *Chipset = CHIP_S3_UNKNOWN;
			   }
			   break;				 
			}
			default:
				Chip_data = rev;
				*Chipset = CHIP_S3_UNKNOWN;
				break;
			}
		}
	}
	wrinx(CRTC_IDX, 0x39, cr39);
	wrinx(CRTC_IDX, 0x38, cr38);
	DisableIOPorts(NUMPORTS, Ports);
	return(result);
}

static int MemProbe_S3(Chipset)
int Chipset;
{
	Byte config, old, config2;
	int Mem = 0, MemOffScreen = 0;

	EnableIOPorts(NUMPORTS, Ports);

	old = rdinx(CRTC_IDX, 0x38);
	wrinx(CRTC_IDX, 0x38, 0x00);
	if (!testinx2(CRTC_IDX, 0x35, 0x0F))
	{
		wrinx(CRTC_IDX, 0x38, 0x48);
		if (testinx2(CRTC_IDX, 0x35, 0x0F))
		{
			config = rdinx(CRTC_IDX, 0x36);
			config2 = rdinx(CRTC_IDX, 0x37);
			if ((Chipset == CHIP_S3_911) || 
			    (Chipset == CHIP_S3_924))
			{
			   if ((config & 0x20) != 0)
			      Mem = 512;
			   else
			      Mem = 1024;
			}
			else if (Chipset == CHIP_S3_ViRGE)
			{
			   switch((config & 0xE0) >> 5)
			   {
			   case 0:
			      Mem = 4 * 1024;
			      break;
			   case 4:
			      Mem = 2 * 1024;
			      break;
			   case 6:
			      Mem = 1 * 1024;
			      break;
			   }
			}
			else if (Chipset == CHIP_S3_ViRGE_VX)
			{
			   switch((config2 & 0x60) >> 5)
			   {
			   case 1:
			      MemOffScreen = 4 * 1024;
			      break;
			   case 2:
			      MemOffScreen = 2 * 1024;
			      break;
			   }
			   switch((config & 0x60) >> 5)
			   {
			   case 0:
			      Mem = 2 * 1024;
			      break;
			   case 1:
			      Mem = 4 * 1024;
			      break;
			   case 2:
			      Mem = 6 * 1024;
			      break;
			   case 3:
			      Mem = 8 * 1024;
			      break;
			   }
			   Mem -= MemOffScreen;
			}
			else 
			{
				switch((config & 0xE0) >> 5)
				{
				case 0:
					Mem = 4 * 1024;
					break;
				case 2:
					Mem = 3 * 1024;
					break;
				case 3:
					Mem = 8 * 1024;
					break;
				case 4:
					Mem = 2 * 1024;
					break;
				case 5:
					Mem = 6 * 1024;
					break;
				case 6:
					Mem = 1 * 1024;
					break;
				}
			}
		}
	}
	wrinx(CRTC_IDX, 0x38, old);

	DisableIOPorts(NUMPORTS, Ports);
	return((MemOffScreen << 16) | Mem);
}
