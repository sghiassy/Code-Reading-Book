/* $XConsortium: apm_cursor.h /main/2 1996/10/23 13:24:05 kaleb $ */




/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/apm/apm_cursor.h,v 3.1 1996/12/27 07:04:47 dawes Exp $ */

/* Variables defined in apm_cursor.c. */

extern int apmCursorHotX;
extern int apmCursorHotY;
extern int apmCursorWidth;
extern int apmCursorHeight;

/* Functions defined in apm_cursor.c. */

extern void ApmCursorInit();
extern void ApmRestoreCursor();
extern void ApmWarpCursor();
extern void ApmQueryBestSize();
