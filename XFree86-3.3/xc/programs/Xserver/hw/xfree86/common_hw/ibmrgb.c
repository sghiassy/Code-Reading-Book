/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/IBMRGB.c,v 3.6 1997/01/18 06:55:39 dawes Exp $ */
/*
 * Copyright 1995 The XFree86 Project, Inc
 *
 * programming the on-chip clock on the IBM RGB52x
 * Harald Koenig <koenig@tat.physik.uni-tuebingen.de>
 */
/* $XConsortium: IBMRGB.c /main/5 1996/05/07 17:13:25 kaleb $ */

#include "Xfuncproto.h"
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"


#define S3_SERVER
#include "IBMRGB.h" 

extern int vgaCRIndex;
extern int vgaCRReg;


/*
 * s3OutIBMRGBIndReg() and s3InIBMRGBIndReg() are used to access the indirect
 * RGB52x registers only.
 */

#if NeedFunctionPrototypes
void s3OutIBMRGBIndReg(unsigned char reg, unsigned char mask, unsigned char data)
#else
void s3OutIBMRGBIndReg(reg, mask, data)
unsigned char reg;
unsigned char mask;
unsigned char data;
#endif
{
   unsigned char tmp, tmp2 = 0x00;

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);

   outb(IBMRGB_INDEX_LOW, reg);

   if (mask != 0x00)
      tmp2 = inb(IBMRGB_INDEX_DATA) & mask;
   outb(IBMRGB_INDEX_DATA, tmp2 | data);
   
   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);
}

#if NeedFunctionPrototypes
unsigned char s3InIBMRGBIndReg(unsigned char reg)
#else
unsigned char s3InIBMRGBIndReg(reg)
unsigned char reg;
#endif
{
volatile   unsigned char tmp, ret;

   outb(vgaCRIndex, 0x55);
   tmp = inb(vgaCRReg) & 0xFC;
   outb(vgaCRReg, tmp | 0x01);

   outb(IBMRGB_INDEX_LOW, reg);
   ret = inb(IBMRGB_INDEX_DATA);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, tmp);

   return(ret);
}

#if NeedFunctionPrototypes
static void
s3ProgramIBMRGBClock(int clk, unsigned char m, unsigned char n, 
		     unsigned char df)
#else
static void
s3ProgramIBMRGBClock(clk, m, n, df)
int clk;
unsigned char m;
unsigned char n;
unsigned char df;
#endif
{
   int tmp;
   tmp = 1;
   s3OutIBMRGBIndReg(IBMRGB_misc_clock, ~1, 1);

   s3OutIBMRGBIndReg(IBMRGB_m0+2*clk, 0, (df<<6) | (m&0x3f));
   s3OutIBMRGBIndReg(IBMRGB_n0+2*clk, 0, n);

   s3OutIBMRGBIndReg(IBMRGB_pll_ctrl2, 0xf0, clk);
   s3OutIBMRGBIndReg(IBMRGB_pll_ctrl1, 0xf8, 3);
}

#if NeedFunctionPrototypes
void IBMRGBSetClock(long freq, int clk, long dacspeed, long fref)
#else
void
IBMRGBSetClock(freq, clk, dacspeed, fref)
long freq;
int clk;
long dacspeed;
long fref;
#endif
{
   volatile   double ffreq,fdacspeed,ffref;
   volatile   int df, n, m, max_n, min_df;
   volatile   int best_m=69, best_n=17, best_df=0;
   volatile   double  diff, mindiff;
   
#define FREQ_MIN   16250  /* 1000 * (0+65) / 4 */
#define FREQ_MAX  dacspeed

   if (freq < FREQ_MIN)
      ffreq = FREQ_MIN / 1000.0;
   else if (freq > FREQ_MAX)
      ffreq = FREQ_MAX / 1000.0;
   else
      ffreq = freq / 1000.0;
   
   fdacspeed = dacspeed / 1e3;
   ffref = fref / 1e3;

   ffreq /= ffref;
   ffreq *= 16;
   mindiff = ffreq;

   if (freq <= dacspeed/4) 
      min_df = 0;
   else if (freq <= dacspeed/2) 
      min_df = 1;
   else 
      min_df = 2;

   for(df=0; df<4; df++) {
      ffreq /= 2;
      mindiff /= 2;
      if (df < min_df) continue;

      /* the remaining formula is  ffreq = (m+65) / n */

      if (df < 3) max_n = fref/1000/2;
      else        max_n = fref/1000;
      if (max_n > 31)  max_n = 31;
      
      for (n=2; n <= max_n; n++) {
	 m = (int)(ffreq * n + 0.5) - 65;
	 if (m < 0)
	    m = 0;
	 else if (m > 63) 
	    m = 63;
	 
	 diff = (m+65.0) / n - ffreq;
	 if (diff<0)
	    diff = -diff;
	 
	 if (diff < mindiff) {
	    mindiff = diff;
	    best_n = n;
	    best_m = m;
	    best_df = df;
	 }
      }
   }

#ifdef DEBUG
   ErrorF("clk %d, setting to %f, m 0x%02x %d, n 0x%02x %d, df %d\n", clk,
	  ((best_m+65.0)/best_n) / (8>>best_df) * ffref,
	  best_m, best_m, best_n, best_n, best_df);
#endif

   s3ProgramIBMRGBClock(clk, best_m, best_n, best_df);
}

int s3IBMRGB_Probe()
{
   unsigned char CR43, CR55, dac[3], lut[6];
   unsigned char ilow, ihigh, id, rev, id2, rev2;
   int i,j;
   int ret=0;

   outb(vgaCRIndex, 0x43);
   CR43 = inb(vgaCRReg);
   outb(vgaCRReg, CR43 & ~0x02);

   outb(vgaCRIndex, 0x55);
   CR55 = inb(vgaCRReg) ;
   outb(vgaCRReg, CR55 & ~0x03);

   /* save DAC and first LUT entries */
   for (i=0; i<3; i++) 
      dac[i] = inb(IBMRGB_PIXEL_MASK+i);
   for (i=j=0; i<2; i++) {
      outb(IBMRGB_READ_ADDR, i);
      lut[j++] = inb(IBMRGB_RAMDAC_DATA);
      lut[j++] = inb(IBMRGB_RAMDAC_DATA);
      lut[j++] = inb(IBMRGB_RAMDAC_DATA);
   }

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, (CR55 & ~0x03) | 0x01);  /* set RS2 */

   /* read ID and revision */
   ilow  = inb(IBMRGB_INDEX_LOW);
   ihigh = inb(IBMRGB_INDEX_HIGH);
   outb(IBMRGB_INDEX_HIGH, 0);  /* index high */
   outb(IBMRGB_INDEX_LOW, IBMRGB_rev);
   rev = inb(IBMRGB_INDEX_DATA);
   outb(IBMRGB_INDEX_LOW, IBMRGB_id);
   id  = inb(IBMRGB_INDEX_DATA);

   /* known IDs:  
      1 = RGB525
      2 = RGB524, RGB528 
      */
      
   if (id >= 1 && id <= 2) { 
      /* check if ID and revision are read only */
      outb(IBMRGB_INDEX_LOW, IBMRGB_rev);
      outb(IBMRGB_INDEX_DATA, ~rev);
      outb(IBMRGB_INDEX_LOW, IBMRGB_id);
      outb(IBMRGB_INDEX_DATA, ~id);
      outb(IBMRGB_INDEX_LOW, IBMRGB_rev);
      rev2 = inb(IBMRGB_INDEX_DATA);
      outb(IBMRGB_INDEX_LOW, IBMRGB_id);
      id2  = inb(IBMRGB_INDEX_DATA);
      
      if (id == id2 && rev == rev2) {  /* IBM RGB52x found */
	 ret = (id<<8) | rev;

	 /* check for 128bit VRAM -> RGB528 */
	 outb(IBMRGB_INDEX_LOW, IBMRGB_misc1);
	 if ((inb(IBMRGB_INDEX_DATA) & 0x03) == 0x03)  /* 128bit DAC found */
	    ret |= 1<<16;
      }
      else {
	 outb(IBMRGB_INDEX_LOW, IBMRGB_rev);
	 outb(IBMRGB_INDEX_DATA, rev);
	 outb(IBMRGB_INDEX_LOW, IBMRGB_id);
	 outb(IBMRGB_INDEX_DATA, id);
      }
   }   
   outb(IBMRGB_INDEX_LOW,  ilow);
   outb(IBMRGB_INDEX_HIGH, ihigh);

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, CR55 & ~0x03);  /* reset RS2 */

   /* restore DAC and first LUT entries */
   for (i=j=0; i<2; i++) {
      outb(IBMRGB_WRITE_ADDR, i);
      outb(IBMRGB_RAMDAC_DATA, lut[j++]);
      outb(IBMRGB_RAMDAC_DATA ,lut[j++]);
      outb(IBMRGB_RAMDAC_DATA ,lut[j++]);
   }
   for (i=0; i<3; i++) 
      outb(IBMRGB_PIXEL_MASK+i, dac[i]);
   
   outb(vgaCRIndex, 0x43);
   outb(vgaCRReg, CR43);
   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, CR55);   

   return ret;
}


void s3IBMRGB_Init()
{
   unsigned char CR55, tmp;

   outb(vgaCRIndex, 0x43);
   tmp = inb(vgaCRReg);
   outb(vgaCRReg, tmp & ~0x02);

   outb(vgaCRIndex, 0x55);
   CR55 = inb(vgaCRReg) ;
   outb(vgaCRReg, (CR55 & ~0x03) | 0x01);  /* set RS2 */

   tmp = inb(IBMRGB_INDEX_CONTROL);
   outb(IBMRGB_INDEX_CONTROL, tmp & ~1);  /* turn off auto-increment */
   outb(IBMRGB_INDEX_HIGH, 0);        /* index high byte */

   outb(vgaCRIndex, 0x55);
   outb(vgaCRReg, CR55 & ~0x03);  /* reset RS2 */
}
