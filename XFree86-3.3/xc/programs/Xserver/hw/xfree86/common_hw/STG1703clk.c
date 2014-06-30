/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/STG1703clk.c,v 3.7 1996/12/23 06:44:19 dawes Exp $ */
/*
 * Copyright 1995 The XFree86 Project, Inc
 *
 * programming for the STG1703 clock derived from the code in the Mach64
 * server (by Kevin Martin).
 */
/* $XConsortium: STG1703clk.c /main/6 1996/05/07 17:14:06 kaleb $ */

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
#define INDEX_LO              0x03C8
#define INDEXED_REG           0x03C9
#define PIXEL_COMMAND         0x03C6
#define INDEX_HI              0x03C7

#define MIN_N1		6

#define FREQ_MIN              8500
#define FREQ_MAX            135000
#define STG1703_REF_FREQ     14318

#if NeedFunctionPrototypes
void
STG1703magic(int onoff)
#else
void
STG1703magic(onoff)
int onoff;
#endif
{
	unsigned char daccom;

	xf86dactopel();
	daccom = xf86getdaccomm();
	if(onoff)
		xf86setdaccomm(daccom | 0x10);
	else
		xf86setdaccomm(daccom & 0xEF);
	xf86dactocomm();
	inb(PIXEL_COMMAND);
	return;
}

#if NeedFunctionPrototypes
unsigned char
STG1703getIndex(unsigned int i)
#else
unsigned char
STG1703getIndex(i)
unsigned int i;
#endif
{
	unsigned char tmp;

	STG1703magic(1);				/* set the index */
	outb(PIXEL_COMMAND,  (i & 0xff));
	outb(PIXEL_COMMAND, ((i & 0xff00) >> 8));

	tmp = inb(PIXEL_COMMAND);
	return(tmp);
}

#if NeedFunctionPrototypes
void
STG1703setIndex(unsigned int i, unsigned char val)
#else
void
STG1703setIndex(i,val)
unsigned int i;
unsigned char val;
#endif
{
	STG1703magic(1);				/* set the index */
	outb(PIXEL_COMMAND,  (i & 0xff));
	outb(PIXEL_COMMAND, ((i & 0xff00) >> 8));

	outb(PIXEL_COMMAND,val);
	return;
}

#if NeedFunctionPrototypes
static void 
s3ProgramSTG1703Clock(unsigned int program_word, int clk)
#else
static void s3ProgramSTG1703Clock(program_word, clk)
unsigned int program_word;
int clk;
#endif
{
   unsigned char tmp, oldCR55;
   int vgaCRAddr, vgaCRData;

   vgaCRAddr = vgaIOBase + 4;
   vgaCRData = vgaIOBase + 5;

   /* RS2 controlled in CR55 bit 0 */
   outb(vgaCRAddr, 0x55);
   oldCR55 = inb(vgaCRData);
   /* set bit 0 of CR55 to 1 to set RS2 to 1 to make sure we are
      talking to correct registers */
   outb(vgaCRData, (oldCR55 & 0xFC) | 0x01);

   outb(INDEX_LO, (clk << 1) + 0x20);
   outb(INDEX_HI, 0);
   outb(INDEXED_REG, (program_word & 0xff00) >> 8);
   outb(INDEXED_REG, program_word & 0xff);
   
   usleep(10000);

   /* that's done, now select clock through register 0x42 on S3 */

   tmp = inb(0x3CC);
   outb(0x3C2, tmp | 0x0C);
   outb(vgaCRAddr, 0x042);
   outb(vgaCRData, clk);

   /* put the value of 0x55 back */
   /* if we don't do this, and the server doesn't set CR55 and assumes it
      is okay, when it tries to write to the DAC it will the clock registers
   */

   outb(vgaCRAddr, 0x55);
   outb(vgaCRData, oldCR55);
}

#if NeedFunctionPrototypes
static void 
et4000ProgramSTG1703Clock(unsigned int program_word, int clk)
#else
static void et4000ProgramSTG1703Clock(program_word, clk)
unsigned int program_word;
int clk;
#endif
{

   STG1703setIndex( (clk << 1) + 0x20, (program_word & 0xff) );
   outb(INDEXED_REG, (program_word & 0xff00) >> 8);
   
   usleep(10000);
}


#if NeedFunctionPrototypes
void STG1703CalcProgramWord(long freq, unsigned int * program_word)
#else
void
STG1703CalcProgramWord(freq, program_word)
long freq;
unsugned int *program_word;
#endif
{
   unsigned int temp, tempB;
   unsigned short tempA, remainder, preRemainder, divider;

   if (freq < FREQ_MIN)
      freq = FREQ_MIN;
   else if (freq > FREQ_MAX)
      freq = FREQ_MAX;

   divider = 0;
   while (freq < (FREQ_MIN << 3)) {
      freq <<= 1;
      divider += 0x20;
   }

   temp = (unsigned int)(freq);
   temp = (unsigned int)(temp * (MIN_N1 + 2));
   temp -= (short)(STG1703_REF_FREQ << 1);

   tempA = MIN_N1;
   preRemainder = 0xffff;

   do {
      tempB = temp;
      remainder = tempB % STG1703_REF_FREQ;
      tempB = tempB / STG1703_REF_FREQ;

      if ((tempB & 0xffff) <= 127 && (remainder <= preRemainder)) {
	 preRemainder = remainder;
	 divider &= ~0x1f;
	 divider |= tempA;
	 divider = (divider & 0x00ff) + ((tempB & 0xff) << 8);
      }

      temp += freq;
      tempA++;
   } while (tempA <= (MIN_N1 << 1));

   *program_word = divider;

   /*
    * M  = (program_word & 0x7F) >> 8
    * N1 = program_word & 0x1F
    * N2 = (program_word & 0x60) >> 5
    */

   return;
}

#if NeedFunctionPrototypes
void STG1703SetClock(long freq, int clk)
#else
void
STG1703SetClock(freq, clk)
long freq;
int clk;
#endif
{
   unsigned int program_word;

   /*
    * calculate the values
    */
   STG1703CalcProgramWord(freq,&program_word);
   s3ProgramSTG1703Clock(program_word, clk);
}

#if NeedFunctionPrototypes
void ET4000stg1703SetClock(long freq, int clk)
#else
void
ET4000stg1703SetClock(freq, clk)
long freq;
int clk;
#endif
{
   unsigned int program_word;

   /*
    * calculate the values
    */
printf("set clock %d to %ld\n", clk,freq);
   STG1703CalcProgramWord(freq,&program_word);
   et4000ProgramSTG1703Clock(program_word, clk);
}

