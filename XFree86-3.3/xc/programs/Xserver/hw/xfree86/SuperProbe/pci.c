/*
 * PCI Probe
 *
 * Copyright 1996  The XFree86 Project, Inc.
 *
 * A lot of this comes from Robin Cutshaw's scanpci
 *
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/PCI.c,v 3.7.4.1 1997/05/06 13:24:29 dawes Exp $ */

#include "Probe.h"

struct pci_config_reg *pci_devp[MAX_PCI_DEVICES + 1] = {NULL, };

void
xf86scanpci()
{
    unsigned long tmplong1, tmplong2, config_cmd, cmd;
    unsigned char tmp1, tmp2;
    unsigned int i, j, idx = 0;
    int func;
    struct pci_config_reg pcr;
#ifdef PC98
    Word PCI_CtrlIOPorts[] = { 0xCF8, 0xCF9, 0xCFC };
#else
    Word PCI_CtrlIOPorts[] = { 0xCF8, 0xCFA, 0xCFC };
#endif
    int Num_PCI_CtrlIOPorts = 3;
    unsigned PCI_DevIOAddrPorts[16*16];
    int Num_PCI_DevIOAddrPorts = 16*16;

    for (i=0; i<16; i++) {
        for (j=0; j<16; j++)
            PCI_DevIOAddrPorts[(i*16)+j] = 0xC000 + (i*0x0100) + (j*4);
    }

    /* Enable I/O access */
    EnableIOPorts(Num_PCI_CtrlIOPorts,PCI_CtrlIOPorts);

    outp(PCI_MODE2_ENABLE_REG, 0x00);
    outp(PCI_MODE2_FORWARD_REG, 0x00);
    tmp1 = inp(PCI_MODE2_ENABLE_REG);
    tmp2 = inp(PCI_MODE2_FORWARD_REG);

    if ((tmp1 == 0x00) && (tmp2 == 0x00)) {
	pcr._configtype = 2;
#ifdef DEBUGPCI
        printf("PCI says configuration type 2\n");
#endif
    } else {
        tmplong1 = inpl(PCI_MODE1_ADDRESS_REG);
        outpl(PCI_MODE1_ADDRESS_REG, PCI_EN);
        tmplong2 = inpl(PCI_MODE1_ADDRESS_REG);
        outpl(PCI_MODE1_ADDRESS_REG, tmplong1);
        if (tmplong2 == PCI_EN) {
	    pcr._configtype = 1;
#ifdef DEBUGPCI
            printf("PCI says configuration type 1\n");
#endif
	} else {
	    pcr._configtype = 0;
#ifdef DEBUGPCI
            printf("No PCI !\n");
#endif
#if 0
            DisableIOPorts(0);
#endif
	    return;
	}
    }

    /* Try pci config 1 probe first */

#ifdef DEBUGPCI
    printf("\nPCI probing configuration type 1\n");
#endif

    pcr._pcibuses[0] = 0;
    pcr._pcinumbus = 1;
    pcr._pcibusidx = 0;

#ifndef DEBUGPCI
    if (pcr._configtype == 1)
#endif
    do {
        for (pcr._cardnum = 0x0; pcr._cardnum < 0x20; pcr._cardnum += 0x1) {
	  func = 0;
	  do { /* loop over different functions, if present */
	    config_cmd = PCI_EN | (pcr._pcibuses[pcr._pcibusidx]<<16) |
                                  (pcr._cardnum<<11) | (func<<8);

            outpl(PCI_MODE1_ADDRESS_REG, config_cmd);         /* ioreg 0 */
            pcr._device_vendor = inpl(PCI_MODE1_DATA_REG);

            if (pcr._vendor == 0xFFFF)   /* nothing there */
                break;

#ifdef DEBUGPCI
	    printf("\npci bus 0x%x cardnum 0x%02x, vendor 0x%04x device 0x%04x\n",
	        pcr._pcibuses[pcr._pcibusidx], pcr._cardnum, pcr._vendor,
                pcr._device);
#endif

            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x04);
	    pcr._status_command  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x08);
	    pcr._class_revision  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x0C);
	    pcr._bist_header_latency_cache = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x10);
	    pcr._base0  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x14);
	    pcr._base1  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x18);
	    pcr._base2  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x1C);
	    pcr._base3  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x20);
	    pcr._base4  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x24);
	    pcr._base5  = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x30);
	    pcr._baserom = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x3C);
	    pcr._max_min_ipin_iline = inpl(PCI_MODE1_DATA_REG);
            outpl(PCI_MODE1_ADDRESS_REG, config_cmd | 0x40);
	    pcr._user_config = inpl(PCI_MODE1_DATA_REG);

	    pcr._funcnum = func;

            /* check for pci-pci bridges (currently we only know Digital) */
            if ((pcr._vendor == 0x1011) && (pcr._device == 0x0001))
                if (pcr._secondary_bus_number > 0)
                    pcr._pcibuses[pcr._pcinumbus++] = pcr._secondary_bus_number;

	    if (idx >= MAX_PCI_DEVICES)
	        break;;

	    /* Ignore non-VGA devices */
	    if (!((pcr._base_class == PCI_CLASS_PREHISTORIC &&
		   pcr._sub_class == PCI_SUBCLASS_PREHISTORIC_VGA) ||
		  (pcr._base_class == PCI_CLASS_DISPLAY &&
		   pcr._sub_class == PCI_SUBCLASS_DISPLAY_VGA) ||
		  (pcr._base_class == PCI_CLASS_DISPLAY &&
		   pcr._sub_class == PCI_SUBCLASS_DISPLAY_OTHER))) {
		/*
		 * unfortunately, sometimes GLINT chips are strapped
		 * as class 0 subclass 0 by vendors...
		 */
		if( pcr._vendor != PCI_VENDOR_3DLABS )
			break;
	    }
		  
	    if ((pci_devp[idx] = (struct pci_config_reg *)malloc(sizeof(
		 struct pci_config_reg))) == (struct pci_config_reg *)NULL) {
                outpl(PCI_MODE1_ADDRESS_REG, 0x00);
#if 0
                DisableIOPorts(0);
#endif
		return;
	    }

	    memcpy(pci_devp[idx++], &pcr, sizeof(struct pci_config_reg));
	    pci_devp[idx] = NULL;
	    if((func==0) && ((pcr._header_type & PCI_MULTIFUNC_DEV) == 0)) {
	    	/* not a multi function device */
		func = 8;
	    } else {
	    	func++;
	    }
	  } while (func<8);
        }
    } while (++pcr._pcibusidx < pcr._pcinumbus);

#ifndef DEBUGPCI
    if (pcr._configtype == 1) {
        outpl(PCI_MODE1_ADDRESS_REG, 0x00);
#if 0
	xf86DisableIOPorts(0);
#endif
	return;
    }
#endif
    /* Now try pci config 2 probe (deprecated) */

    outp(PCI_MODE2_ENABLE_REG, 0xF1);
    outp(PCI_MODE2_FORWARD_REG, 0x00); /* bus 0 for now */

#ifdef DEBUGPCI
    printf("\nPCI probing configuration type 2\n");
#endif

    pcr._pcibuses[0] = 0;
    pcr._pcinumbus = 1;
    pcr._pcibusidx = 0;

    do {
        for (pcr._ioaddr = 0xC000; pcr._ioaddr < 0xD000; pcr._ioaddr += 0x0100){
	    outp(PCI_MODE2_FORWARD_REG, pcr._pcibuses[pcr._pcibusidx]); /* bus 0 for now */
            pcr._device_vendor = inpl(pcr._ioaddr);
	    outp(PCI_MODE2_FORWARD_REG, 0x00); /* bus 0 for now */

            if (pcr._vendor == 0xFFFF)   /* nothing there */
                continue;
	    /* opti chipsets that use config type 1 look like this on type 2 */
            if ((pcr._vendor == 0xFF00) && (pcr._device == 0xFFFF))
                continue;

#ifdef DEBUGPCI
	    printf("\npci bus 0x%x slot at 0x%04x, vendor 0x%04x device 0x%04x\n",
	        pcr._pcibuses[pcr._pcibusidx], pcr._ioaddr, pcr._vendor,
                pcr._device);
#endif

	    outp(PCI_MODE2_FORWARD_REG, pcr._pcibuses[pcr._pcibusidx]); /* bus 0 for now */
            pcr._status_command = inpl(pcr._ioaddr + 0x04);
            pcr._class_revision = inpl(pcr._ioaddr + 0x08);
            pcr._bist_header_latency_cache = inpl(pcr._ioaddr + 0x0C);
            pcr._base0 = inpl(pcr._ioaddr + 0x10);
            pcr._base1 = inpl(pcr._ioaddr + 0x14);
            pcr._base2 = inpl(pcr._ioaddr + 0x18);
            pcr._base3 = inpl(pcr._ioaddr + 0x1C);
            pcr._base4 = inpl(pcr._ioaddr + 0x20);
            pcr._base5 = inpl(pcr._ioaddr + 0x24);
            pcr._baserom = inpl(pcr._ioaddr + 0x30);
            pcr._max_min_ipin_iline = inpl(pcr._ioaddr + 0x3C);
            pcr._user_config = inpl(pcr._ioaddr + 0x40);
	    outp(PCI_MODE2_FORWARD_REG, 0x00); /* bus 0 for now */

            /* check for pci-pci bridges (currently we only know Digital) */
            if ((pcr._vendor == 0x1011) && (pcr._device == 0x0001))
                if (pcr._secondary_bus_number > 0)
                    pcr._pcibuses[pcr._pcinumbus++] = pcr._secondary_bus_number;

	    if (idx >= MAX_PCI_DEVICES)
	        continue;

	    /* Ignore non-VGA devices */
	    if (!((pcr._base_class == PCI_CLASS_PREHISTORIC &&
		   pcr._sub_class == PCI_SUBCLASS_PREHISTORIC_VGA) ||
		  (pcr._base_class == PCI_CLASS_DISPLAY &&
		   pcr._sub_class == PCI_SUBCLASS_DISPLAY_VGA)))
		continue;
		  
	    if ((pci_devp[idx] = (struct pci_config_reg *)malloc(sizeof(
		 struct pci_config_reg))) == (struct pci_config_reg *)NULL) {
                outp(PCI_MODE2_ENABLE_REG, 0x00);
                outp(PCI_MODE2_FORWARD_REG, 0x00);
#if 0
                xf86DisableIOPorts(0);
#endif
		return;
	    }

	    memcpy(pci_devp[idx++], &pcr, sizeof(struct pci_config_reg));
	    pci_devp[idx] = NULL;
	}
    } while (++pcr._pcibusidx < pcr._pcinumbus);

    outp(PCI_MODE2_ENABLE_REG, 0x00);
    outp(PCI_MODE2_FORWARD_REG, 0x00);

#if 0
    DisableIOPorts(0);
#endif
}
