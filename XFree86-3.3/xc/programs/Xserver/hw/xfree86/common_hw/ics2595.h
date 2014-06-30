/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/ICS2595.h,v 3.3 1996/12/23 06:44:13 dawes Exp $ */


/* Norbert Distler ndistler@physik.tu-muenchen.de */


/* $XConsortium: ICS2595.h /main/4 1996/02/21 17:40:17 kaleb $ */

typedef int Bool;
#define TRUE 1
#define FALSE 0
#define QUARZFREQ	        14318
#define MIN_ICS2595_FREQ        79994 /* 85565 */
#define MAX_ICS2595_FREQ       145000     

Bool ICS2595SetClock( 
#if NeedFunctionPrototypes
   long
#endif
);     
