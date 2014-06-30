/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaPCI.c,v 3.11.2.2 1997/05/14 08:39:45 dawes Exp $ */
/*
 * PCI Probe
 *
 * Copyright 1995  The XFree86 Project, Inc.
 *
 * A lot of this comes from Robin Cutshaw's scanpci
 *
 */
/* $XConsortium: vgaPCI.c /main/10 1996/10/25 10:34:22 kaleb $ */

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "xf86_Config.h"
#include "vga.h"

#define INIT_PCI_VENDOR_INFO
#include "vgaPCI.h"

vgaPCIInformation *
vgaGetPCIInfo()
{
    vgaPCIInformation *info = NULL;
    pciConfigPtr pcrp, *pcrpp;
    Bool found = FALSE;
    int i = 0;
    CARD32 membase = 0, membase2 = 0, mmiobase = 0, iobase = 0;

    pcrpp = xf86scanpci(vga256InfoRec.scrnIndex);

    if (!pcrpp)
	return NULL;

    while (pcrp = pcrpp[i]) {
	if ((pcrp->_base_class == PCI_CLASS_PREHISTORIC &&
	     pcrp->_sub_class == PCI_SUBCLASS_PREHISTORIC_VGA) ||
	    (pcrp->_base_class == PCI_CLASS_DISPLAY &&
	     pcrp->_sub_class == PCI_SUBCLASS_DISPLAY_VGA)) {
	    found = TRUE;
	    if ((info = (vgaPCIInformation *)
		 xalloc(sizeof(vgaPCIInformation))) == NULL)
		return NULL;
	    info->Vendor = pcrp->_vendor;
	    info->ChipType = pcrp->_device;
	    info->ChipRev = pcrp->_rev_id;
	    info->Bus = pcrp->_bus;
	    info->Card = pcrp->_cardnum;
	    info->Func = pcrp->_func;
	    info->AllCards = pcrpp;
	    info->ThisCard = pcrp;
	    info->MemBase = 0;
	    info->MMIOBase = 0;
	    info->IOBase = 0;

	    /* If the alignment is finer than 256k, assume mmio */
	    if (pcrp->_base0) {
		if (pcrp->_base0 & 1)
		    iobase = pcrp->_base0 & 0xFFFFFFFC;
		else
		    if (pcrp->_base0 & 0x3FFF0)
			mmiobase = pcrp->_base0 & 0xFFFFFFF0;
		    else
			membase = pcrp->_base0 & 0xFFFFFFF0;
	    }
	    if (pcrp->_base1) {
		if (pcrp->_base1 & 1) {
		    if (!iobase)
			iobase = pcrp->_base1 & 0xFFFFFFFC;
		} else
		    if (pcrp->_base1 & 0x3FFF0) {
			if (!mmiobase)
			    mmiobase = pcrp->_base1 & 0xFFFFFFF0;
		    } else {
			if (!membase)
			    membase = pcrp->_base1 & 0xFFFFFFF0;
			else if (!membase2)
			    membase2 = pcrp->_base1 & 0xFFFFFFF0;
		    }
	    }
	    if (pcrp->_base2) {
		if (pcrp->_base2 & 1) {
		    if (!iobase)
			iobase = pcrp->_base2 & 0xFFFFFFFC;
		} else
		    if (pcrp->_base2 & 0x3FFF0) {
			if (!mmiobase)
			    mmiobase = pcrp->_base2 & 0xFFFFFFF0;
		    } else {
			if (!membase)
			    membase = pcrp->_base2 & 0xFFFFFFF0;
			else if (!membase2)
			    membase2 = pcrp->_base2 & 0xFFFFFFF0;
		    }
	    }
	    if (pcrp->_base3) {
		if (pcrp->_base3 & 1) {
		    if (!iobase)
			iobase = pcrp->_base3 & 0xFFFFFFFC;
		} else
		    if (pcrp->_base3 & 0x3FFF0) {
			if (!mmiobase)
			    mmiobase = pcrp->_base3 & 0xFFFFFFF0;
		    } else {
			if (!membase)
			    membase = pcrp->_base3 & 0xFFFFFFF0;
			else if (!membase2)
			    membase2 = pcrp->_base3 & 0xFFFFFFF0;
		    }
	    }
	    if (pcrp->_base4) {
		if (pcrp->_base4 & 1) {
		    if (!iobase)
			iobase = pcrp->_base4 & 0xFFFFFFFC;
		} else
		    if (pcrp->_base4 & 0x3FFF0) {
			if (!mmiobase)
			    mmiobase = pcrp->_base4 & 0xFFFFFFF0;
		    } else {
			if (!membase)
			    membase = pcrp->_base4 & 0xFFFFFFF0;
			else if (!membase2)
			    membase2 = pcrp->_base4 & 0xFFFFFFF0;
		    }
	    }
	    if (pcrp->_base5) {
		if (pcrp->_base5 & 1) {
		    if (!iobase)
			iobase = pcrp->_base5 & 0xFFFFFFFC;
		} else
		    if (pcrp->_base5 & 0x3FFF0) {
			if (!mmiobase)
			    mmiobase = pcrp->_base5 & 0xFFFFFFF0;
		    } else {
			if (!membase)
			    membase = pcrp->_base5 & 0xFFFFFFF0;
			else if (!membase2)
			    membase2 = pcrp->_base5 & 0xFFFFFFF0;
		    }
	    }
	    break;
	}
	i++;
    }
    if (found && xf86Verbose) {
	int i = 0, j;
	char *vendorname = NULL, *chipname = NULL;

	while (xf86PCIVendorInfo[i].VendorName) {
	    if (xf86PCIVendorInfo[i].VendorID == info->Vendor) {
		j = 0;
		vendorname = xf86PCIVendorInfo[i].VendorName;
		while (xf86PCIVendorInfo[i].Device[j].DeviceName) {
		    if (xf86PCIVendorInfo[i].Device[j].DeviceID ==
			info->ChipType) {
			chipname = xf86PCIVendorInfo[i].Device[j].DeviceName;
			break;
		    }
		    j++;
		}
		break;
	    }
	    i++;
	}

	ErrorF("%s %s: PCI: ", XCONFIG_PROBED, vga256InfoRec.name);
	if (vendorname)
	    ErrorF("%s ", vendorname);
	else
	    ErrorF("Unknown vendor (0x%04x) ", info->Vendor);
	if (chipname)
	    ErrorF("%s ", chipname);
	else
	    ErrorF("Unknown chipset (0x%04x) ", info->ChipType);
	ErrorF("rev %d", info->ChipRev);

	info->MemBase = membase;
	info->MMIOBase = mmiobase;
	info->IOBase = iobase;

	if (membase)
	    ErrorF(", Memory @ 0x%08x", membase);
	if (membase2)
	    ErrorF(", 0x%08x", membase2);
	if (mmiobase)
	    ErrorF(", MMIO @ 0x%08x", mmiobase);
	if (iobase)
	    ErrorF(", I/O @ 0x%04x", iobase);
	ErrorF("\n");
    }
    return info;
}

