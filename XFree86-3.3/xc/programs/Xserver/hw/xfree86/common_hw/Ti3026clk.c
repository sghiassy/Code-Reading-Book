/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/Ti3026clk.c,v 3.11 1996/12/23 06:44:22 dawes Exp $ */
/*
 * Copyright 1995 The XFree86 Project, Inc
 *
 * programming the on-chip clock on the Ti3026, derived from the
 * S3 gendac code by Jon Tombs
 * Harald Koenig <koenig@tat.physik.uni-tuebingen.de>
 */
/* $XConsortium: Ti3026clk.c /main/10 1996/10/19 18:00:22 kaleb $ */

#include "Xfuncproto.h"
#include "Ti302X.h" 
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"
#include <math.h>

void (* dacOutTi3026IndReg)(unsigned char,unsigned char,unsigned char) = NULL;
unsigned char (* dacInTi3026IndReg)(unsigned char) = NULL;

#if NeedFunctionPrototypes
static void
s3ProgramTi3026Clock(int clk, unsigned char n, unsigned char m, 
		     unsigned char p, unsigned char ln, unsigned char lm, 
		     unsigned char lp, unsigned char lq, char which)
#else
static void
s3ProgramTi3026Clock(clk, n, m, p, ln, lm, lp, lq, which)
int clk;
unsigned char n;
unsigned char m;
unsigned char p;
unsigned char ln;
unsigned char lm;
unsigned char lp;
unsigned char lq;
char which;
#endif
{
   int tmp;
   /*
    * Reset the clock data index
    */
   (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x00);

   if (clk != TI_MCLK_PLL_DATA) {
      if (which == TI_BOTH_CLOCKS) {
         /*
          * If we are using the Ti PLL clock output the clock frequency
          */
         (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, (n & 0x3f) | 0x80);
         (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, (m & 0x3f) );
         tmp = (*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA) & 0x40;
         (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, (p & 3) | tmp | 0xb0);
   
         /* wait until PLL is locked */
         for (tmp=0; tmp<10000; tmp++) 
   	 if ((*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA) & 0x40) 
   	    break;
      }
   
      /*
       * And now set up the loop clock for RCLK
       */
      (*dacOutTi3026IndReg)(TI_LOOP_CLOCK_PLL_DATA, 0x00, ln);
      (*dacOutTi3026IndReg)(TI_LOOP_CLOCK_PLL_DATA, 0x00, lm);
      (*dacOutTi3026IndReg)(TI_LOOP_CLOCK_PLL_DATA, 0x00, lp);
      (*dacOutTi3026IndReg)(TI_MCLK_LCLK_CONTROL, 0xc8, (lq & 0x0f) | 0x10);

      if (which == TI_BOTH_CLOCKS) {
         /* select pixel clock PLL as dot clock soure */
         (*dacOutTi3026IndReg)(TI_INPUT_CLOCK_SELECT, 0x00, TI_ICLK_PLL);
      }
   }
   else {
      int pn, pm, pp, csr;

      /* select pixel clock PLL as dot clock soure */
      csr = (*dacInTi3026IndReg)(TI_INPUT_CLOCK_SELECT);
      (*dacOutTi3026IndReg)(TI_INPUT_CLOCK_SELECT, 0x00, TI_ICLK_PLL);

      /* save the old dot clock PLL settings */
      (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x00);
      pn = (*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA);
      (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x01);
      pm = (*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA);
      (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x02);
      pp = (*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA);

      /* programm dot clock PLL to new MCLK frequency */
      (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x00);
      (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, (n & 0x3f) | 0x80);
      (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, (m & 0x3f) );
      tmp = (*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA) & 0x40;
      (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, (p & 3) | tmp | 0xb0);

      /* wait until PLL is locked */
      for (tmp=0; tmp<10000; tmp++)
	 if ((*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA) & 0x40) 
	    break;

      /* switch to output dot clock on the MCLK terminal */
      (*dacOutTi3026IndReg)(0x39, 0xe7, 0x00);
      (*dacOutTi3026IndReg)(0x39, 0xe7, 0x08);
      
      /* Set MCLK */
      (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x00);
      (*dacOutTi3026IndReg)(TI_MCLK_PLL_DATA, 0x00, (n & 0x3f) | 0x80);
      (*dacOutTi3026IndReg)(TI_MCLK_PLL_DATA, 0x00, (m & 0x3f));
      (*dacOutTi3026IndReg)(TI_MCLK_PLL_DATA, 0x00, (p & 3) | 0xb0);

      /* wait until PLL is locked */
      for (tmp=0; tmp<10000; tmp++) 
	 if ((*dacInTi3026IndReg)(TI_MCLK_PLL_DATA) & 0x40) 
	    break;

      /* switch to output MCLK on the MCLK terminal */
      (*dacOutTi3026IndReg)(0x39, 0xe7, 0x10);
      (*dacOutTi3026IndReg)(0x39, 0xe7, 0x18);

      /* restore dot clock PLL */
      (*dacOutTi3026IndReg)(TI_PLL_CONTROL, 0x00, 0x00);
      (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, pn);
      (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, pm);
      (*dacOutTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA, 0x00, pp);

      /* wait until PLL is locked */
      for (tmp=0; tmp<10000; tmp++) 
	 if ((*dacInTi3026IndReg)(TI_PIXEL_CLOCK_PLL_DATA) & 0x40) 
	    break;
      (*dacOutTi3026IndReg)(TI_INPUT_CLOCK_SELECT, 0x00, csr);
   }
}

#if NeedFunctionPrototypes
static void Ti30XXSetClock(long freq, int clk, int bpp, char which, int buswidth)
#else
static void
Ti30XXSetClock(freq, clk, bpp, which, buswidth)
long freq;
int clk;
int bpp;
char which;
int buswidth;
#endif
{
   double ffreq;
   int n, p, m;
   int ln, lp, lm, lq, lk, z;
   int best_n=32, best_m=32;
   double  diff, mindiff;
   
#define FREQ_MIN   13767  /* ~110000 / 8 */
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
   
   /* now 110.0 <= ffreq <= 220.0 */   
   
   ffreq /= TI_REF_FREQ;
   
   /* now 7.6825 <= ffreq <= 15.3650 */
   /* the remaining formula is  ffreq = (65-m)*8 / (65-n) */
   
   
   mindiff = ffreq;
   
   for (n=62; n >= 40; n--) {
      m = 65 - (int)(ffreq * (65-n) / 8.0 + 0.5);
      if (m < 1)
	 m = 1;
      else if (m > 62) 
	 m = 62;
      
      diff = ((65-m) * 8) / (65.0-n) - ffreq;
      if (diff<0)
	 diff = -diff;
      
      if (diff < mindiff) {
	 mindiff = diff;
	 best_n = n;
	 best_m = m;
      }
   }

#ifdef DEBUG
   ErrorF("clk %d, asked %d, setting to %f, n %02x %d, m %02x %d, p %d\n", clk,
   	  freq, 8.0/(1 << p)*((65.0-best_m)/(65-best_n)) * TI_REF_FREQ,
	  best_n, best_n, best_m, best_m, p);
#endif

   if (bpp == 3) {
#if 0  /* TI_MUX1_3026T_888_P5 */
      lk = buswidth / 8 / bpp;
      lm = 63;
      if (buswidth == 64)  /* Ti3026 */
	 ln = 60;
      else                 /* Ti3030 */
	 ln = 55;
#else  /* TI_MUX1_3026T_888_P8 */
      lk = buswidth / 8 / bpp;
      lm = 62;
      if (buswidth == 64)  /* Ti3026 */
	 ln = 57;
      else                 /* Ti3030 */
	 ln = 49;
#endif
   }
   else {
      lk = buswidth / 8 / bpp;
      ln = 65 - 4 * lk;
      lm = 61;
   }
   z = 110000.0 / (65-lm) / freq * (100 *  (65-ln)) / lk  ;
   if (z > 1600) {
      lp = 3;
      lq = (z-1600) / 1600 + 1; /* smallest q greater (z-16)/16 */
   }
   else { /* largest p less then log2(z) */
      for (lp=0; z > (200 << lp); lp++) ;
      lq = 0;
   }

#ifdef DEBUG
   ErrorF("bpp %d  lk %2d  ln %2d  lm %2d  lz %4d  lp %2d  lq %2d\n",
	  bpp,lk,ln,lm,z,lp,lq);
#endif

   if (bpp == 3) {
      if (buswidth == 64) { /* Ti3026 */
	 ln = (ln & 0x3f) | 0x80;
	 lm = (lm & 0x3f) | 0x80;
	 lp = (lp & 0x03) | 0xf8;
      }
      else {                /* Ti3030 */
	 ln = (ln & 0x3f) | 0xc0;
	 lm = (lm & 0x3f) | 0x80;
	 lp = (lp & 0x03) | 0xf8;
      }
   }
   else {
      ln = (ln & 0x3f) | 0x80;
      lm = (lm & 0x3f)       ;
      lp = (lp & 0x03) | 0xf0;
   }
   s3ProgramTi3026Clock(clk, best_n, best_m, p, ln, lm, lp, lq, which);
}


#if NeedFunctionPrototypes
void Ti3026SetClock(long freq, int clk, int bpp, char which)
#else
void
Ti3026SetClock(freq, clk, bpp, which)
long freq;
int clk;
int bpp;
char which;
#endif
{
   Ti30XXSetClock(freq, clk, bpp, which, 64);
}

#if NeedFunctionPrototypes
void Ti3030SetClock(long freq, int clk, int bpp, char which)
#else
void
Ti3030SetClock(freq, clk, bpp, which)
long freq;
int clk;
int bpp;
char which;
#endif
{
   Ti30XXSetClock(freq, clk, bpp, which, 128);
}

