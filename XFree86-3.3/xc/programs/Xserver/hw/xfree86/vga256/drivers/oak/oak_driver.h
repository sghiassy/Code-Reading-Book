/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/oak/oak_driver.h,v 3.2 1996/12/23 06:57:56 dawes Exp $ */





/* $XConsortium: oak_driver.h /main/2 1996/02/21 18:06:12 kaleb $ */

#include <X11/Xfuncproto.h>

_XFUNCPROTOBEGIN

extern void OAKFillRectSolidCopy(	/* GXcopy fill. */
#if NeedFunctionPrototypes
	DrawablePtr,
	GCPtr,
	int,
	BoxPtr
#endif
);
extern void OAKFillBoxSolid(
#if NeedFunctionPrototypes
	DrawablePtr,
	int,
	BoxPtr,
	unsigned long,
	unsigned long,
	int
#endif
);
extern void OAKFillSolidSpansGeneral(
#if NeedFunctionPrototypes
	DrawablePtr,
	GCPtr,
	int,
	DDXPointPtr,
	int *,
	int
#endif
);
extern void OAKCopyPlane1to8(
#if NeedFunctionPrototypes
	DrawablePtr,
	DrawablePtr,
	int,
	RegionPtr,
	DDXPointPtr,
	unsigned long,
	int
#endif
);
extern void oti087ColorExpandFill(
#if NeedFunctionPrototypes
	void *,
	long,
	long
#endif
);
extern void oti087ColorMaskedFill(
#if NeedFunctionPrototypes
	void *,
	long
#endif
);

_XFUNCPROTOEND

int leftmask[7];
int rightmask[7];

/* The macros for color expansion register setting */

#define SET_COLOR_EXPANSION(value) \
  outb (0x3DE, 0x30); outb (0x3DF, value)
#define SET_FG_COLOR(color) \
  outb (0x3DE, 0x31); outb (0x3DF, color)
#define SET_BG_COLOR(color) \
  outb (0x3DE, 0x32); outb (0x3DF, color)
#define SET_PATTERN(pattern) \
  outb (0x3DE, 0x33); outb (0x3DF, pattern)         
#define SET_MASK(mask)  \
  outb(0x3DE, 0x34);  outb (0x3DF, mask)
