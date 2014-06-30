/* $XConsortium: ct_pci.c /main/1 1996/09/21 13:36:44 kaleb $ */





/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/chips/ct_pci.c,v 3.4.2.1 1997/05/03 09:48:06 dawes Exp $ */

#include "X.h"
#include "input.h"
#include "screenint.h"

/*
 * These are XFree86-specific header files
 */
#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"

#include "vga.h"
#include "vgaPCI.h"
#include "xf86_Config.h"

int 
ctPCIMemBase(ctisHiQV32)
    Bool ctisHiQV32;
{

    if (vgaPCIInfo && vgaPCIInfo->Vendor == PCI_VENDOR_CHIPSTECH) {
	if (vgaPCIInfo->MemBase != 0) {
	    return vgaPCIInfo->MemBase & 0xFF800000;
	} else {
	    ErrorF("%s %s: %s: Can't find valid PCI "
		"Base Address\n", XCONFIG_PROBED,
		vga256InfoRec.name, vga256InfoRec.chipset);
	    return -1;
	}
    } else {
	ErrorF("%s %s: %s: Can't find PCI device in "
	    "configuration space\n", XCONFIG_PROBED,
	    vga256InfoRec.name, vga256InfoRec.chipset);
	return -1;
    }
}

int
ctPCIChipset()
{
  if (!vgaPCIInfo)
    return (-1);
  if (vgaPCIInfo->Vendor != PCI_VENDOR_CHIPSTECH)
    return (0);
  return(vgaPCIInfo->ChipType);
}
