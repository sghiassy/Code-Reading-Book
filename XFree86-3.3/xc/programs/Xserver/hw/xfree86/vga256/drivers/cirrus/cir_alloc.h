/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_alloc.h,v 3.7 1996/12/23 06:56:31 dawes Exp $ */

/*
 * Definitions for video memory allocator in cir_alloc.c.
 */

/* $XConsortium: cir_alloc.h /main/7 1996/10/19 18:10:47 kaleb $ */

#include <X11/Xfuncproto.h>

_XFUNCPROTOBEGIN

typedef struct {
  int addr;             /* Linear address */
  short x;              /* X coordinate (546x only) */
  short y;              /* Y coordinate (546x only) */
} cirOffscreenAddr;


#if NeedFunctionPrototypes

int CirrusInitializeAllocator(int base);
cirOffscreenAddr CirrusAllocate(int size);
void CirrusFree(int vidaddr);
void CirrusUploadPattern(unsigned char *pattern, int w, int h, 
			 int vidaddr, int srcpitch);
int CirrusCursorAllocate ( cirrusCurRecPtr cirrusCur );

#else

int CirrusInitializeAllocator();
cirOffscreenAddr CirrusAllocate();
int CirrusCursorAllocate ();
void CirrusFree();
void CirrusUploadPattern();

#endif

_XFUNCPROTOEND
