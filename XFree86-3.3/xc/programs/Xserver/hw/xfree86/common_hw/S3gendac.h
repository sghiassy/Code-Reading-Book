/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/S3gendac.h,v 3.10 1997/01/23 11:02:08 dawes Exp $ */ 


/* Jon Tombs <jon@esix2.us.es>  */


/* $XConsortium: S3gendac.h /main/9 1996/10/25 11:36:57 kaleb $ */

#define GENDAC_INDEX	     0x3C8
#define GENDAC_DATA	     0x3C9
#define BASE_FREQ         14.31818   /* MHz */

int S3gendacSetClock( 
#if NeedFunctionPrototypes
   long freq, int clock
#endif
);     

int ET4000gendacSetClock( 
#if NeedFunctionPrototypes
   long freq, int clock
#endif
);     

int ET4000gendacSetpixmuxClock( 
#if NeedFunctionPrototypes
   long freq, int clock
#endif
);     

int ET6000SetClock( 
#if NeedFunctionPrototypes
   long freq, int clock
#endif
);     

int ICS5342SetClock( 
#if NeedFunctionPrototypes
   long freq, int clock
#endif
);     

int S3TrioSetClock( 
#if NeedFunctionPrototypes
   long freq, int clock
#endif
);     

int commonCalcClock(
#if NeedFunctionPrototypes
   long freq,
   int min_m, int min_n1, int max_n1, int min_n2, int max_n2,
   long freq_min, long freq_max,
   unsigned char *mdiv, unsigned char *ndiv
#endif
);

