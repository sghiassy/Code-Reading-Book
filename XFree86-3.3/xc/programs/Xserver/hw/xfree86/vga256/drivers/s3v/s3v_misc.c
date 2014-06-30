/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/s3v/s3v_misc.c,v 1.1.2.3 1997/05/28 13:12:53 dawes Exp $ */

/*
 *
 * Copyright 1995-1997 The XFree86 Project, Inc.
 *
 */

/* 
 * Various functions used in the virge driver. 
 * Right now, this only contains the PCI probing function.
 * 
 * Created 18/03/97 by Sebastien Marineau
 * Revision: 
 * [0.1] 18/03/97: Added PCI probe function, taken from accel/s3_virge server.
 *       Not sure if the code used to adjust the PCI base address is 
 *       still needed for the ViRGE chipsets.
 */

#include "X.h"
#include "input.h"
#include "screenint.h"

#include "compiler.h"
#include "xf86.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"
#include "xf86_PCI.h"
#include "vga.h"
#include "vgaPCI.h"

#ifdef XFreeXDGA
#include "X.h"
#include "Xproto.h"
#include "scrnintstr.h"
#include "servermd.h"
#define _XF86DGA_SERVER_
#include "extensions/xf86dgastr.h"
#endif

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

#include "regs3v.h"
#include "s3v_driver.h"

extern SymTabRec s3vChipTable[];
extern S3VPRIV s3vPriv;


/*
 * s3vGetPCIInfo -- probe for PCI information
 */

S3PCIInformation *
s3vGetPCIInfo()
{
   static S3PCIInformation info = {0, };
   pciConfigPtr pcrp, *pcrpp;
   Bool found = FALSE;
   int i = 0;

   pcrpp = xf86scanpci(vga256InfoRec.scrnIndex);

   if (!pcrpp)
      return NULL;

   while ((pcrp = pcrpp[i])) {
      if (pcrp->_vendor == PCI_S3_VENDOR_ID) {
	 found = TRUE;
	 switch (pcrp->_device) {
	 case PCI_ViRGE:
	    info.ChipType = S3_ViRGE;
	    break;
	 case PCI_ViRGE_VX:
	    info.ChipType = S3_ViRGE_VX;
	    break;
	 case PCI_ViRGE_DXGX:
	    info.ChipType = S3_ViRGE_DXGX;
	    break;
	 default:
	    info.ChipType = S3_UNKNOWN;
	    info.DevID = pcrp->_device;
	    break;
	 }
	 info.ChipRev = pcrp->_rev_id;
	 info.MemBase = pcrp->_base0 & 0xFF800000;
	 break;
      }
      i++;
   }

   /* for new mmio we have to ensure that the PCI base address is
    * 64MB aligned and that there are no address collitions within 64MB.
    * S3 868/968 only pretend to need 32MB and thus fool
    * the BIOS PCI auto configuration :-(  */

   if (info.ChipType == S3_ViRGE) {
      unsigned long base0;
      char *probed;
      char map_64m[64];
      int j;

      if (vga256InfoRec.MemBase == 0) {
	 base0  = info.MemBase;
	 probed = XCONFIG_PROBED;
      }
      else {
	 base0  = vga256InfoRec.MemBase;
	 probed = XCONFIG_GIVEN;
      }

      /* map allocated 64MB blocks */
      for (j=0; j<64; j++) map_64m[j] = 0;
      map_64m[63] = 1;  /* don't use the last 64MB area */
      for (j=0; (pcrp = pcrpp[j]); j++) {
	 if (i != j) {
	    map_64m[ (pcrp->_base0 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base0+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base1 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base1+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base2 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base2+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base3 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base3+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base4 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base4+0x3ffffff) >> 26) & 0x3f] = 1;
	    map_64m[ (pcrp->_base5 >> 26) & 0x3f] = 1;
	    map_64m[((pcrp->_base5+0x3ffffff) >> 26) & 0x3f] = 1;
	 }
      }

      /* check for 64MB alignment and free space */
      if ((base0 & 0x3ffffff) ||
	  map_64m[(base0 >> 26) & 0x3f] ||
	  map_64m[((base0+0x3ffffff) >> 26) & 0x3f]) {
	 for (j=63; j>=16 && map_64m[j]; j--);
	 info.MemBase = ((unsigned long)j) << 26;
	 ErrorF("%s %s: S3V: PCI base address not correctly aligned or address conflict\n",
		probed, vga256InfoRec.name);
	 ErrorF("\t\tbase address changed from 0x%08lx to 0x%08lx\n",
		base0, info.MemBase);
         xf86writepci(vga256InfoRec.scrnIndex, pcrpp[i]->_bus, pcrpp[i]->_cardnum,
		    pcrpp[i]->_func, PCI_MAP_REG_START, ~0L,
		    info.MemBase | PCI_MAP_MEMORY | PCI_MAP_MEMORY_TYPE_32BIT);
      }
   }
   else {
      if (vga256InfoRec.MemBase != 0) {
	  /* Should we allow the user to specify this??? */
          /* Guess this should be reenabled for VLB */
      }
      else {
	
      }
   }

   /* Free PCI information */
   xf86cleanpci();
   if (found && xf86Verbose) {
      if (info.ChipType != S3_UNKNOWN) {
	 ErrorF("%s %s: S3V: %s rev %x, Linear FB @ 0x%08lx\n", XCONFIG_PROBED,
		vga256InfoRec.name,xf86TokenToString(s3vChipTable, info.ChipType), 
		info.ChipRev, info.MemBase);
      }
   }

   if (found)
      return &info;
   else
      return NULL;
}



