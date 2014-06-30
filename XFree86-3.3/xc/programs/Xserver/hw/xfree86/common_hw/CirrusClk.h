/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/CirrusClk.h,v 3.4 1996/12/23 06:44:06 dawes Exp $ */





/* $XConsortium: CirrusClk.h /main/5 1996/10/25 10:25:29 kaleb $ */

int CirrusFindClock( 
#if NeedFunctionPrototypes
   int freq, int max_clock, int *num_out, int *den_out, int *usemclk_out
#endif
);     

int CirrusSetClock(
#if NeedFunctionPrototypes
   int freq
#endif
);
