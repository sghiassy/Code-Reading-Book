/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/Ti3025clk.c,v 3.13 1996/12/23 06:44:21 dawes Exp $ */

/*
 * Copyright 1994 The XFree86 Project, Inc
 *
 * programming the on-chip clock on the Ti3025, derived from the
 * S3 gendac code by Jon Tombs
 * Dirk Hohndel <hohndel@aib.com>
 * Robin Cutshaw <robin@XFree86.org>
 */
/* $XConsortium: Ti3025clk.c /main/9 1996/05/07 17:14:16 kaleb $ */

#include "Xfuncproto.h"
#include "Ti302X.h" 
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"
#include <math.h>



#if NeedFunctionPrototypes
void Ti3025CalcNMP(long freq, int *calc_n, int *calc_m, int *calc_p)
#else
void Ti3025CalcNMP(freq, calc_n, calc_m, calc_p)
long freq;
int *calc_n, *calc_m, *calc_p;
#endif
{
   double ffreq;
   int n, p, m;
   int best_n=32, best_m=32;
   double  diff, mindiff;
   
#define FREQ_MIN   27500  /* 110/8 MHz is the specified limit */
#undef  FREQ_MIN
#define FREQ_MIN   12000
#define FREQ_MAX  220000

   if (freq < FREQ_MIN)
      ffreq = FREQ_MIN / 1000.0;
   else if (freq > FREQ_MAX)
      ffreq = FREQ_MAX / 1000.0;
   else
      ffreq = freq / 1000.0;
   


   /* work out suitable timings */

   /* pick the right p value */

   for(p=0; p<4 && ffreq < 110.0; p++)
      ffreq *= 2;
#if FREQ_MIN < 110000/8
   if (p==4) {
      ffreq /= 2;
      p--;
   }
#endif
   
   /* now 110.0 <= ffreq <= 220.0 */   
   
   ffreq /= TI_REF_FREQ;
   
   /* now 7.6825 <= ffreq <= 15.3650 */
   /* the remaining formula is  ffreq = (m+2)*8 / (n+2) */
   
   
   mindiff = ffreq;
   
   for (n = 1; n <= (int)(TI_REF_FREQ/0.5 - 2); n++) {
      m = (int)(ffreq * (n+2) / 8.0 + 0.5) - 2;
      if (m < 1)
	 m = 1;
      else if (m > 127) 
	 m = 127;
      
      diff = ((m+2) * 8) / (n+2.0) - ffreq;
      if (diff<0)
	 diff = -diff;
      
      if (diff < mindiff) {
	 mindiff = diff;
	 best_n = n;
	 best_m = m;
      }
   }

   *calc_n = best_n;
   *calc_m = best_m;
   *calc_p = p;
}


#if NeedFunctionPrototypes
void Ti3025SetClock(long freq, int clk, void (*ProgramClockFunc)())
#else
void
Ti3025SetClock(freq, clk, ProgramClockFunc)
long freq;
int clk;
void (*ProgramClockFunc)();
#endif
{
   int n, m, p;

   Ti3025CalcNMP(freq, &n, &m, &p);

#ifdef DEBUG
   ErrorF("clk %d (%f), setting to %f, n %02x, m %02x, p %d\n", clk, freq/1e3,
	  8.0/(1 << p)*((m+2.0)/(n+2)) * TI_REF_FREQ,
	  n, m, p);
#endif

   ProgramClockFunc(clk, n, m, p);
}
