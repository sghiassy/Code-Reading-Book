/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/S3gendac.c,v 3.23.2.2 1997/05/11 02:56:20 dawes Exp $ */ 
/*
 * Progaming of the S3 gendac programable clocks, from the S3 Gendac
 * programing documentation by S3 Inc. 
 * Jon Tombs <jon@esix2.us.es>
 * Also used for GenDAC and GenDAC-like chips on non-S3 chipsets.
 */
/* $XConsortium: S3gendac.c /main/16 1996/10/25 14:11:48 kaleb $ */
 
#include <math.h>
#include "Xfuncproto.h"
#include "S3gendac.h" 
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#define CLK_MCLK         10


#define PLL_S3GENDAC      1
#define PLL_S3TRIO        2
#define PLL_ET4000GENDAC  3
#define PLL_ARK2000GENDAC 4
#define PLL_ET6000        5


extern int vgaIOBase;

static void setS3gendacpll(
#if NeedFunctionPrototypes
int reg, unsigned char data1, unsigned char data2
#endif
);

static void settriopll(
#if NeedFunctionPrototypes
int reg, unsigned char data1, unsigned char data2
#endif
);

static void setET4000gendacpll(
#if NeedFunctionPrototypes
int reg, unsigned char data1, unsigned char data2
#endif
);

static void setET6000pll(
#if NeedFunctionPrototypes
int reg, unsigned char data1, unsigned char data2
#endif
);

static void setARK2000gendacpll(
#if NeedFunctionPrototypes
int reg, unsigned char data1, unsigned char data2
#endif
);


static int commonSetClock( 
#if NeedFunctionPrototypes
   long freq, int clock,
   int min_m, int min_n1, int max_n1, int min_n2, int max_n2, int pll_type,
   long freq_min, long freq_max
#endif
);     

int
S3gendacSetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 0, 3, PLL_S3GENDAC, 100000, 250000);
}

int
ET4000gendacSetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 0, 3, PLL_ET4000GENDAC, 100000, 270000);
}

int
ET6000SetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 0, 3, PLL_ET6000, 100000, 270000);
}

int
ET4000gendacSetpixmuxClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 2, 3, PLL_ET4000GENDAC, 100000, 270000);
}

int
ARK2000gendacSetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 0, 3, PLL_ARK2000GENDAC, 100000, 270000);
}

int
ICS5342SetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 1, 3, PLL_S3GENDAC, 100000, 270000);
}

int
S3TrioSetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 0, 3, PLL_S3TRIO, 135000, 270000);
}

int
S3Trio64V2SetClock(freq, clk)
long freq;
int clk;
{
   return commonSetClock(freq, clk, 1, 1, 31, 0, 4, PLL_S3TRIO, 170000, 340000);
}

int
S3ViRGE_VXSetClock(freq, clk)
long freq;
int clk;
{
   if (clk != CLK_MCLK)
      return commonSetClock(freq, clk, 0, 0, 31, 0, 4, PLL_S3TRIO, 220000, 440000);
   else
      return commonSetClock(freq, clk, 0, 0, 31, 0, 3, PLL_S3TRIO, 220000, 440000);
}

int
S3AuroraSetClock(freq, clk)
long freq;
int clk;
{
   if (clk == CLK_MCLK)
      return commonSetClock(freq, clk, 1, 1, 31, 0, 3, PLL_S3TRIO, 135000, 270000);
   else
      return commonSetClock(freq, clk, 1, 1, 63, 0, 3, PLL_S3TRIO, 135000, 270000);
}

int
#if NeedFunctionPrototypes
commonCalcClock(long freq, int min_m, int min_n1, int max_n1, int min_n2, int max_n2, 
		long freq_min, long freq_max,
		unsigned char * mdiv, unsigned char * ndiv)
#else
commonCalcClock(freq, min_m, min_n1, max_n1, min_n2, max_n2, 
		freq_min, freq_max, mdiv, ndiv)
long freq;
int min_m, min_n1, max_n1, min_n2, max_n2;
long freq_min, freq_max;
unsigned char *mdiv, *ndiv;
#endif
{
   double ffreq, ffreq_min, ffreq_max;
   double div, diff, best_diff;
   unsigned int m;
   unsigned char n1, n2;
   unsigned char best_n1=16+2, best_n2=2, best_m=125+2;

   ffreq     = freq     / 1000.0 / BASE_FREQ;
   ffreq_min = freq_min / 1000.0 / BASE_FREQ;
   ffreq_max = freq_max / 1000.0 / BASE_FREQ;

   if (ffreq < ffreq_min / (1<<max_n2)) {
      ErrorF("invalid frequency %1.3f MHz  [freq >= %1.3f MHz]\n", 
	     ffreq*BASE_FREQ, ffreq_min*BASE_FREQ / (1<<max_n2));
      ffreq = ffreq_min / (1<<max_n2);
   }
   if (ffreq > ffreq_max / (1<<min_n2)) {
      ErrorF("invalid frequency %1.3f MHz  [freq <= %1.3f MHz]\n", 
	     ffreq*BASE_FREQ, ffreq_max*BASE_FREQ / (1<<min_n2));
      ffreq = ffreq_max / (1<<min_n2);
   }

   /* work out suitable timings */

   best_diff = ffreq;
   
   for (n2=min_n2; n2<=max_n2; n2++) {
      for (n1 = min_n1+2; n1 <= max_n1+2; n1++) {
	 m = (int)(ffreq * n1 * (1<<n2) + 0.5) ;
	 if (m < min_m+2 || m > 127+2) 
	    continue;
	 div = (double)(m) / (double)(n1);	 
	 if ((div >= ffreq_min) &&
	     (div <= ffreq_max)) {
	    diff = ffreq - div / (1<<n2);
	    if (diff < 0.0) 
	       diff = -diff;
	    if (diff < best_diff) {
	       best_diff = diff;
	       best_m    = m;
	       best_n1   = n1;
	       best_n2   = n2;
	    }
	 }
      }
   }
   
#if EXTENDED_DEBUG
   ErrorF("Clock parameters for %1.6f MHz: m=%d, n1=%d, n2=%d\n",
	  ((double)(best_m) / (double)(best_n1) / (1 << best_n2)) * BASE_FREQ,
	  best_m-2, best_n1-2, best_n2);
#endif
  
   if (max_n1 == 63)
      *ndiv = (best_n1 - 2) | (best_n2 << 6);
   else
      *ndiv = (best_n1 - 2) | (best_n2 << 5);
   *mdiv = best_m - 2;
}
  
static int
#if NeedFunctionPrototypes
commonSetClock(long freq, int clk, int min_m, int min_n1, int max_n1, int min_n2, 
	       int max_n2, int pll_type, long freq_min, long freq_max)
#else
commonSetClock(freq, clk, min_m, min_n1, max_n1, min_n2, max_n2, pll_type, 
	       freq_min, freq_max)
long freq;
int clk;
int min_m, min_n1, max_n1, min_n2, max_n2, pll_type;
long freq_min, freq_max;
#endif
{
   unsigned char m, n;

   commonCalcClock(freq, min_m, min_n1, max_n1, min_n2, max_n2, freq_min, 
		   freq_max, &m, &n);

   switch(pll_type)
   {
     case PLL_S3GENDAC:
         setS3gendacpll(clk, m, n);
         break;
     case PLL_S3TRIO:
         settriopll(clk, m, n);
         break;
     case PLL_ET4000GENDAC:
         setET4000gendacpll(clk, m, n);
         break;
     case PLL_ARK2000GENDAC:
         setARK2000gendacpll(clk, m, n);
         break;
     case PLL_ET6000:
         setET6000pll(clk, m, n);
         break;
     default: 
         ErrorF("Internal error: unknown pll_type in S3gendac.c");
         return -1;
    }
   return 0;
}	   


static void
#if NeedFunctionPrototypes
setS3gendacpll(int reg, unsigned char data1, unsigned char data2)
#else
setS3gendacpll(reg, data1, data2)
int reg;
unsigned char data1;
unsigned char data2;
#endif
{
   unsigned char tmp, tmp1;
   int vgaCRIndex = vgaIOBase + 4;
   int vgaCRReg = vgaIOBase + 5;
		
   /* set RS2 via CR55, yuck */
   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);  
   tmp1 = inb(GENDAC_INDEX);

   outb(GENDAC_INDEX, reg);
   outb(GENDAC_DATA, data1);
   outb(GENDAC_DATA, data2);

   /* Now clean up our mess */
   outb(GENDAC_INDEX, tmp1);  
   outb(vgaCRReg, tmp);
}


static void
#if NeedFunctionPrototypes
setET4000gendacpll(int reg, unsigned char data1, unsigned char data2)
#else
setET4000gendacpll(reg, data1, data2)
int reg;
unsigned char data1;
unsigned char data2;
#endif
{
   unsigned char tmp, tmp1;
   int vgaCRIndex = vgaIOBase + 4;
   int vgaCRReg = vgaIOBase + 5;
		
   /* set RS2 via CR31 */
   outb(vgaCRIndex, 0x31);
   tmp = inb(vgaCRReg) & 0xBF;
   outb(vgaCRReg, tmp | 0x40);  
   tmp1 = inb(GENDAC_INDEX);

   outb(GENDAC_INDEX, reg);
   outb(GENDAC_DATA, data1);
   outb(GENDAC_DATA, data2);

   /* Now clean up our mess */
   outb(GENDAC_INDEX, tmp1);  
   outb(vgaCRReg, tmp);
}


static void
#if NeedFunctionPrototypes
setET6000pll(int reg, unsigned char data1, unsigned char data2)
#else
setET6000pll(reg, data1, data2)
int reg;
unsigned char data1;
unsigned char data2;
#endif
{
   /* Tseng Labs engineers were wise enough to use the same
    * PLL scheme used in other standard GenDACs -- good!
    * Clocks 0..7 are pixel clocks, clock 10 is the memory clock
    */

   unsigned long PCIIOBase  = 0xDEADBEEF;
   unsigned char tmp;
   int vgaCRIndex = vgaIOBase + 4;
   int vgaCRReg = vgaIOBase + 5;

   /* get PCI IO base address first */
   outb(vgaCRIndex, 0x21);
   PCIIOBase = (inb(vgaCRReg)<<8);
   outb(vgaCRIndex, 0x22);
   PCIIOBase += (inb(vgaCRReg)<<16);
   outb(vgaCRIndex, 0x23);
   PCIIOBase += (inb(vgaCRReg)<<24);

#if EXTENDED_DEBUG
   ErrorF("ET6000: PCIIOBase: 0x%lX ; M/N parameters: 0x%X 0x%X ; clk index: %d\n",
           PCIIOBase, data1, data2, reg);
#endif

   /* set the PLL parameters for specified clock */
   tmp = inb(PCIIOBase+0x67); /* remember old CLKDAC index register pointer */
   outb(PCIIOBase+0x67, reg);
   outb(PCIIOBase+0x69, data1);
   outb(PCIIOBase+0x69, data2);

   /* restore old index register */
   outb(PCIIOBase+0x67, tmp);
}


static void
#if NeedFunctionPrototypes
setARK2000gendacpll(int reg, unsigned char data1, unsigned char data2)
#else
setARK2000gendacpll(reg, data1, data2)
int reg;
unsigned char data1;
unsigned char data2;
#endif
{
   unsigned char tmp, tmp1;
   int vgaCRIndex = vgaIOBase + 4;
   int vgaCRReg = vgaIOBase + 5;
		
   /* set RS2 via CR1C, bit 0x80 */
   outb(vgaCRIndex, 0x1C);
   tmp = inb(vgaCRReg) & ~0x80;
   outb(vgaCRReg, tmp | 0x80);  
   tmp1 = inb(GENDAC_INDEX);

   outb(GENDAC_INDEX, reg);
   outb(GENDAC_DATA, data1);
   outb(GENDAC_DATA, data2);

   /* Now clean up our mess */
   outb(GENDAC_INDEX, tmp1);  
   outb(vgaCRReg, tmp);
}


static void
#if NeedFunctionPrototypes
settriopll(int clk, unsigned char m, unsigned char n)
#else
settriopll(clk, m, n)
     int clk;
     unsigned char m;
     unsigned char n;
#endif
{
   unsigned char tmp;
   int index2;

   /*
    * simlulate S3 GENDAC clock numbers:
    * 0,1 for fixed 25 and 28 MHz clocks
    * 2-8 free programmable
    * 10  MCLK
    */

   if (clk < 2) {
      tmp = inb(0x3cc);
      outb(0x3c2, (tmp & 0xf3) | (clk << 2));
   }
   else {
      tmp = inb(0x3cc);
      outb(0x3c2, tmp | 0x0c);

      outb(0x3c4, 0x08);
      outb(0x3c5, 0x06);  /* unlock extended CR9-CR18 */

      if (clk != CLK_MCLK) {  /* DCLK */
	 outb(0x3c4, 0x12);
	 outb(0x3c5, n);
	 outb(0x3c4, 0x13);
	 outb(0x3c5, m);

	 outb(0x3c4, 0x15);
	 tmp = inb(0x3c5) & ~0x21;
	 outb(0x3c5, tmp | 0x02);
	 outb(0x3c5, tmp | 0x22);
	 outb(0x3c5, tmp | 0x02);
      }
      else {		/* MCLK */
	 index2 = 0x10;
	 outb(0x3c4, 0x10);
	 outb(0x3c5, n);
	 outb(0x3c4, 0x11);
	 outb(0x3c5, m);
	 outb(0x3c4, 0x1a);
	 outb(0x3c5, n);

	 outb(0x3c4, 0x15);
	 tmp = inb(0x3c5) & ~0x21;
	 outb(0x3c5, tmp | 0x01);
	 outb(0x3c5, tmp | 0x21);
	 outb(0x3c5, tmp | 0x01);
	 outb(0x3c5, tmp);
      }

      outb(0x3c4, 0x08);
      outb(0x3c5, 0x00);  /* lock extended CR9-CR18 */
      
   }
}


int
#if NeedFunctionPrototypes
gendacMNToClock(unsigned char m,unsigned char n)
#else
gendacMNToClock(m,n)
     unsigned char m;
     unsigned char n;
#endif
{
	int rtn;

	rtn = (1431818 * (m + 2) / ((n & 0x1f)+2) /
		(1 << ((n & 0x60) >> 5))) / 100;

	return rtn;
}
