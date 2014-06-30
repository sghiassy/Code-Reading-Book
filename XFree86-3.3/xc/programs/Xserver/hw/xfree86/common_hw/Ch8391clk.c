/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/Ch8391clk.c,v 3.6 1996/12/23 06:44:02 dawes Exp $ */
/*
 * Copyright 1995 The XFree86 Project, Inc
 *
 * programming the clock chip Chrontel 8391, derived from the
 * chrontel8391 program from Richard Burdick <burdir@fionn.cuug.ab.ca>
 * Harald Koenig  <koenig@tat.physik.uni-tuebingen.de>
 */
/* $XConsortium: Ch8391clk.c /main/5 1996/05/07 17:13:03 kaleb $ */

/*
 *
 * This is a clock setting program for the chrontel 8391 RAMDAC with
 * programmable clock.  It may work with the chrontel 8398 RAMDAC
 * with programmable clock.  (I don't have one to test, but if you want
 * you can try it)
 *
 * Copyright (C) 1994 Richard Burdick
 *
 * Richard Burdick <burdir@fionn.cuug.ab.ca>
 * 16 December 1994
 *
 * modified to fit into XFree86 source tree by
 * Harald Koenig  <koenig@tat.physik.uni-tuebingen.de>
 * 14 January 1995
 *
 */



/* current status:

version 0.1

   This is basically very experimental.  Currently I am using it and have had
   no problems.  I was inspired to write this because I got tired of looking
   at XFree86 at 640x480.  Now I am looking at it at 1024x768.  If you want to
   try it, go ahead.  As expressed above, I make no claims of warranty,
   so if your machine blows up, you've been warned.

   I am releasing this right now because I was so damn tired of 640x480
   and wanted higher resolutions as soon as possible and figured others
   would as well

   I am still making modifications so I ask that if you use this,
   drop me a line so I can keep people up to date.

   If you modify this please inform me (especially if you had to modify it to
   get it to compile on your system) so I can put changes back into my version.

   If you know anything about programming the S3 please feel free to
   contact me if you have any suggestions about how I could do this better

   It works under Linux, and probably anything else XFree86 works on (I hope!)

   it has to be made suid root, as it has to call ioperm to use the video
   ports
*/

#include "Xfuncproto.h"
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"

extern int vgaIOBase;


/* when RS2 = 0 */

#define PALETTE_RAM_WRITE_ADDRESS            0x03C8
#define PALETTE_RAM_DATA_REGISTER            0x03C9
#define PIXEL_READ_MASK_REGISTER             0x03C6
#define PALETTE_RAM_READ_ADDRESS             0x03C7

/* when RS2 = 1 */
#define CLOCK_RAM_WRITE_ADDRESS              0x03C8
#define CLOCK_RAM_DATA_REGISTER              0x03C9
#define CONTROL_REGISTER                     0x03C6
#define CLOCK_RAM_READ_ADDRESS               0x03C7

#define FREQ_MIN   8500
#define FREQ_MAX 135000
#define CHRONTEL_REF_FREQ 14.31818


/* 
   the chrontel8391 and 8398 have 16 video clocks and 8 memory clocks.
   we should use clock 2 because on 8398 clock 0 is hardwired to 25.175 and
   clock 1 is hardwired to 28.322 (on 8391 nothing is hardwired).
   */

#if NeedFunctionPrototypes
static void 
s3ProgramChrontel8391Clock(unsigned char m, unsigned char n, unsigned char k, unsigned int clk)
#else
static void s3ProgramChrontel8391Clock(m, n, k, clk)
unsigned char m;
unsigned char n;
unsigned char k;
unsigned int clk;
#endif
{
   unsigned char tmp, oldCR55, oldPLLwrite, oldCSR, CSR = 0x00;
   int vgaCRAddr, vgaCRData;

   vgaCRAddr = vgaIOBase + 4;
   vgaCRData = vgaIOBase + 5;

   /* RS2 controlled in CR55 bit 0 */
   outb(vgaCRAddr, 0x55);
   oldCR55 = inb(vgaCRData);   /* save value in register 55 */
   /* set bit 0 of CR55 to 1 to set RS2 to 1 to make sure we are
      talking to correct registers */
   outb(vgaCRData, (oldCR55 & 0xFC) | 0x01);

   /* save PLL write address, then load it to write to clock "clk" */
   oldPLLwrite = inb(CLOCK_RAM_WRITE_ADDRESS);

   outb(CLOCK_RAM_WRITE_ADDRESS, clk);

   /* to write the PLL ram, we load the address into the write address
      register, then load the lsb into the pll ram data register, then
      load the msb into the pll ram data register.  the chip automatically
      makes the first write to the address the lsb, and the second the
      write to the msb.

      the pll ram is set up like this

      Address       LSB      MSB
      00H           VPLL    VPLL
      01H           VPLL    VPLL
      ...
      0FH           VPLL    VPLL
      10H           MPLL    MPLL
      11H           MPLL    MPLL
      ...
      17H           MPLL    MPLL

      VPLL refers to the fact that these values are video pll (phase locked
      loop) entries (commonly referred to as dot-clocks)

      MPLL refers to the fact that these values are memory pll entries
      (memory clocks).  I don't touch these yet, as I have no idea
      what values they should have, or even what they are for
      (I assume DRAM refresh speed, but I'm not sure)

   */

   outb(CLOCK_RAM_DATA_REGISTER, n);
   outb(CLOCK_RAM_DATA_REGISTER, (k << 6) | m );

   /* there, we've just changed clock "clk", now we have to select it */

   /* first, read the clock select on the chip */
   /* I'll use the alternate access method of accessing the chips clock
      select register, which is to read the clock write address register
      four times in a row without reading any other clock register.
      the fifth access (read or write) is the clock select register
      (Start by reading some other register to reset sequence)
   */


   /* first read the clock select */
   (void)inb(CONTROL_REGISTER);   /* reset sequence just to make sure */
   (void)inb(CLOCK_RAM_WRITE_ADDRESS);
   (void)inb(CLOCK_RAM_WRITE_ADDRESS);
   (void)inb(CLOCK_RAM_WRITE_ADDRESS);
   (void)inb(CLOCK_RAM_WRITE_ADDRESS);
   oldCSR = inb(CLOCK_RAM_WRITE_ADDRESS);

   /* the format of the clock select is: bit 7 is frequency hold
      if this is 0, the rest of the bits are ignored
      if this is 1, then:
         bits 6-4 select the Memory clock, (MPLL)
	 bits 3-0 select the video clock (VPLL) (dot-clock)
	 the VPLL is logically 'or'ed with the external FS[3:0] bits
	 (I assume these are controlled through Register 0x42)
	 so the strategy is:
	    get the clock select register, do not touch the memory clock
	    select bits as we don't know how to use the MPLL, but
	    make sure the VPLL select bits are all 0, so the clock select
	    is entirely based on the S3's register 0x42 (we select the clock
	    through 0x42 later)
*/

   if ((oldCSR & 0x0F) != 0x00)/* if bits 3-0 already 1, don't bother modifying */
     {
       CSR = oldCSR | 0x80;
       (void)inb(CONTROL_REGISTER);   /* reset sequence just to make sure */
       (void)inb(CLOCK_RAM_WRITE_ADDRESS);  /* do 4 useless reads */
       (void)inb(CLOCK_RAM_WRITE_ADDRESS);
       (void)inb(CLOCK_RAM_WRITE_ADDRESS);
       (void)inb(CLOCK_RAM_WRITE_ADDRESS);
       outb(CLOCK_RAM_WRITE_ADDRESS, CSR);/* fifth access is clock select */
     }

   /* that's done, now select clock through register 0x42 on S3 */

   tmp = inb(0x3CC);
   outb(0x3C2, tmp | 0x0C);
   outb(vgaCRAddr, 0x042);
   outb(vgaCRData, clk);


   /* we're all done (I think), put things back the way they were */

   /* write the clock ram write address back */
   outb(CLOCK_RAM_WRITE_ADDRESS, oldPLLwrite);

   /* put the value of 0x55 back */
   /* if we don't do this, and the server doesn't set CR55 and assumes it
      is okay, when it tries to write to the DAC it will the clock registers
   */

   outb(vgaCRAddr, 0x55);
   outb(vgaCRData, oldCR55);
}


/*
   the formula for the clock is

   freq desired  =  reference_freq * n  /  m * k

   reference_freq comes from the oscillator on the card (assume 14.31818 Mhz)
   n = N + 8
   m = M + 2
   k = 2^K

   we use N, M, K to program the chip

   the chrontel literature says: some values of N are restricted, including:
   0-7, 10-15, 19-23, 28-31, 37-39, 46-47, 55.  Why this is, I don't know.
   the 'including' remark makes me think there might be others, but they
   don't say which ones.  Notice that the ranges keep getting smaller
   ie: 0-7, 8 values are restricted.  10-15, 6 values are restricted.
   19-23, 5 values ; 28-31, 4 values; 37-39, 3 values; 46-47, 2; 55, 1
   if I have interpreted this pattern correctly, there aren't any other
   values, but then I could be wrong

   the chrontel literature also says use of M values of 10 or less for
   best circuit performance

*/



#if NeedFunctionPrototypes
void Chrontel8391SetClock(long freq, int clk)
#else
void
Chrontel8391SetClock(freq, clk)
long freq;
int clk;
#endif
{
   double ffreq;
   int n, nmin, nmax, k, m, m0;
   int best_n, best_m;
   double  diff, mindiff;

   if (freq < FREQ_MIN)
      ffreq = FREQ_MIN / 1000.0;
   else if (freq > FREQ_MAX)
      ffreq = FREQ_MAX / 1000.0;
   else
      ffreq = freq / 1000.0;

   /* work out suitable timings */

   /* pick the right p value */

   for(k=0; k<4 && ffreq <= 67.5; k++)
      ffreq *= 2;

   /* now 67.5 <= ffreq <= 135.0 */

   ffreq /= CHRONTEL_REF_FREQ;

   /* now 4.7142 <= ffreq <= 9.4285 */
   /* the remaining formula is  ffreq = (n+8) / (m+2) */

   nmin = (int)(ffreq * (1+2)) -8 -1;
   if (nmin<8) nmin = 8;  /* because (n <= 7) isn't allowed */
   nmax = (int)(ffreq * (32+2)) -8 +1;
   if (nmax > 255) nmax = 255;
   
   mindiff = ffreq;
   for (n = nmin; n <= nmax; n++) {
      if ( /* (n <= 7) || */
	  (n >= 10 && n <= 15) ||
	  (n >= 19 && n <= 23) || (n >= 28 && n <= 31) ||
	  (n >= 37 && n <= 39) || (n == 46) || (n == 47) ||
	  (n == 51))
	 continue;    /* the above numbers are not allowed, skip */
      m0 = (int)((n+8) / ffreq) - 2;
      for (m=m0-1; m<=m0+1; m++) {
	 if (m<1 || m>31) continue;
	
	 diff = (n+8.0) / (m+2) - ffreq;
	 if (diff<0)
	    diff = -diff;
	
	 if (diff < mindiff) {
	    mindiff = diff;
	    best_n = n;
	    best_m = m;
	 }
      }
   }
#ifdef DEBUG
   diff = freq/1000.0 - (CHRONTEL_REF_FREQ * (best_n+8.0) / ((best_m + 2) * (1 << k)));
   if (diff<0) diff = -diff;
   ErrorF("clk %d, setting to %12f, m %3d, n %3d, k %d, err %12f\n", clk,
	  CHRONTEL_REF_FREQ * (best_n+8.0) / ((best_m + 2) * (1 << k)),
	  best_m, best_n, k, diff);
#endif

   s3ProgramChrontel8391Clock(best_m, best_n, k, clk);
   return;
}
