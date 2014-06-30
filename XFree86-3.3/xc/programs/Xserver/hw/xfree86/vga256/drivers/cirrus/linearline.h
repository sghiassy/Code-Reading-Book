/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/linearline.h,v 3.3 1996/12/23 06:57:00 dawes Exp $ */





/* $XConsortium: linearline.h /main/3 1996/02/21 18:04:29 kaleb $ */

/* linearline.c */
extern void LinearFramebufferVerticalLine(
#if NeedFunctionPrototypes
    unsigned char *,
    int,
    int,
    int
#endif
);

extern void LinearFramebufferDualVerticalLine(
#if NeedFunctionPrototypes
    unsigned char *,
    int,
    int,
    int,
    int
#endif
);

extern void LinearFramebufferSlopedLineLeft(
#if NeedFunctionPrototypes
    unsigned char *,
    int,
    int,
    int,
    int,
    int
#endif
);

extern void LinearFramebufferSlopedLineRight(
#if NeedFunctionPrototypes
    unsigned char *,
    int,
    int,
    int,
    int,
    int
#endif
);

extern void LinearFramebufferSlopedLineVerticalLeft(
#if NeedFunctionPrototypes
    unsigned char *,
    int,
    int,
    int,
    int,
    int
#endif
);

extern void LinearFramebufferSlopedLineVerticalRight(
#if NeedFunctionPrototypes
    unsigned char *,
    int,
    int,
    int,
    int,
    int
#endif
);
