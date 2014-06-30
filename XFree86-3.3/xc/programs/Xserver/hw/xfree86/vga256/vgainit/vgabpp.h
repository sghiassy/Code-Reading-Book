/* $XConsortium: vgabpp.h /main/4 1996/10/19 18:14:51 kaleb $ */






/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/vgainit/vgabpp.h,v 3.3 1996/12/23 07:04:34 dawes Exp $ */

extern Bool vga16bppScreenInit(
#if NeedFunctionPrototypes
    ScreenPtr /* pScreen */,
    pointer /* pbits */,
    int /* xsize */,
    int /* ysize */,
    int /* dpix */,
    int /* dpiy */,
    int /* width */
#endif
);

extern Bool vga24bppScreenInit(
#if NeedFunctionPrototypes
    ScreenPtr /* pScreen */,
    pointer /* pbits */,
    int /* xsize */,
    int /* ysize */,
    int /* dpix */,
    int /* dpiy */,
    int /* width */
#endif
);

extern Bool vga32bppScreenInit(
#if NeedFunctionPrototypes
    ScreenPtr /* pScreen */,
    pointer /* pbits */,
    int /* xsize */,
    int /* ysize */,
    int /* dpix */,
    int /* dpiy */,
    int /* width */
#endif
);
