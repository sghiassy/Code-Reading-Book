/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vga/vgaAsm.h,v 3.4 1996/12/23 06:59:27 dawes Exp $ */





/* $XConsortium: vgaAsm.h /main/6 1996/02/21 18:10:00 kaleb $ */

/* Definitions for VGA bank assembler routines */

#ifdef CSRG_BASED
#define VGABASE CONST(0xFF000000)
#else
#define VGABASE CONST(0xF0000000)
#endif

