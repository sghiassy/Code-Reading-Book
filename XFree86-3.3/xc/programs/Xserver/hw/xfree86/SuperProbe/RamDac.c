/* $XConsortium: RamDac.c /main/17 1996/10/25 07:00:23 kaleb $ */
/*
 * (c) Copyright 1993,1994 by David Wexelblat <dwex@xfree86.org>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a 
 * copy of this software and associated documentation files (the "Software"), 
 * to deal in the Software without restriction, including without limitation 
 * the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the 
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL 
 * DAVID WEXELBLAT BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF 
 * OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
 * SOFTWARE.
 * 
 * Except as contained in this notice, the name of David Wexelblat shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from David Wexelblat.
 *
 * Contributing Authors:
 *	Robin Cutshaw <robin@xfree86.org>
 *	Harald Koenig <koenig@tat.physik.uni-tuebingen.de>
 * 
 */

/* $XFree86: xc/programs/Xserver/hw/xfree86/SuperProbe/RamDac.c,v 3.26.2.5 1997/05/22 14:00:35 dawes Exp $ */

#include "Probe.h"

static Word Ports[] = {0x000, 0x000, 0x3C6, 0x3C7, 0x3C8, 0x3C9};
#define NUMPORTS (sizeof(Ports)/sizeof(Word))

#define RED	0
#define GREEN	1
#define BLUE	2

static void ReadPelReg __STDCARGS((Byte, Byte *));
static void WritePelReg __STDCARGS((Byte, Byte *));
static Byte SetComm __STDCARGS((Byte));
static Bool Width8Check __STDCARGS((void));
static Bool TestDACBit __STDCARGS((Byte, Byte, Byte));
static Bool S3_Bt485Check __STDCARGS((int *));
static Bool S3_TVP3020Check __STDCARGS((int *));
static Bool S3_ATT498Check __STDCARGS((int *));
static Bool S3_STG1700Check __STDCARGS((int *));
static Bool S3_GENDACCheck __STDCARGS((int *));
static void CheckMach32 __STDCARGS((int, int *));
static void CheckMach64 __STDCARGS((int, int *));
static void CheckMatrox __STDCARGS((int, int *));

#ifdef __STDC__
static void ReadPelReg(Byte Index, Byte *Pixel)
#else
static void ReadPelReg(Index, Pixel)
Byte Index;
Byte *Pixel;
#endif
{
	outp(0x3C7, Index);
	Pixel[RED] = inp(0x3C9);
	Pixel[GREEN] = inp(0x3C9);
	Pixel[BLUE] = inp(0x3C9);

	return;
}

#ifdef __STDC__
static void WritePelReg(Byte Index, Byte *Pixel)
#else
static void WritePelReg(Index, Pixel)
Byte Index;
Byte *Pixel;
#endif
{
	outp(0x3C8, Index);
	outp(0x3C9, Pixel[RED]);
	outp(0x3C9, Pixel[GREEN]);
	outp(0x3C9, Pixel[BLUE]);
	return;
}

#ifdef __STDC__
static Byte SetComm(Byte Comm)
#else
static Byte SetComm(Comm)
Byte Comm;
#endif
{
	(void) dactocomm();
	outp(0x3C6, Comm);
	(void) dactocomm();
	return(inp(0x3C6));
}

static Bool Width8Check()
{
	Byte old, x, v;
	Byte pix[3];

	old = inp(0x3C8);
	ReadPelReg(0xFF, pix);
	v = pix[RED];
	pix[RED]= 0xFF;
	WritePelReg(0xFF, pix);
	ReadPelReg(0xFF, pix);
	x = pix[RED];
	pix[RED] = v;
	WritePelReg(0xFF, pix);
	outp(0x3C8, old);
	return(x == 0xFF);
}

#ifdef __STDC__
static Bool TestDACBit(Byte Bit, Byte OldComm, Byte OldPel)
#else
static Bool TestDACBit(Bit, OldComm, OldPel)
Byte Bit;
Byte OldComm;
Byte OldPel;
#endif
{
	Byte tmp;

	dactopel();
	outp(0x3C6, OldPel & (Bit ^ 0xFF));
	(void) dactocomm();
	outp(0x3C6, OldComm | Bit);
	tmp = inp(0x3C6);
	outp(0x3C6, tmp & (Bit ^ 0xFF));
	return((tmp & Bit) != 0);
}

static Bool S3_Bt485Check(RamDac)
int *RamDac;
{
	Byte old1, old2, old3, old4;
	Byte lock1, lock2;
	Bool Found = FALSE;
	Bool DoChecks = FALSE;

	lock1 = rdinx(CRTC_IDX, 0x38);
	lock2 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x48);
	wrinx(CRTC_IDX, 0x39, 0xA5);

	old1 = inp(0x3C6);
	old2 = rdinx(CRTC_IDX, 0x55);
	outp(0x3C6, 0x0F);
	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x02);
	old3 = inp(0x3C6);
	if ((old3 & 0x80) == 0x80)
	{
		Found = TRUE;
		if ((old3 & 0xC0) == 0x80)
		{
			*RamDac = DAC_BT485;
			DoChecks = TRUE;
		}
		else if ((old3 & 0xF0) == 0xD0)
		{
			DoChecks = TRUE;
			*RamDac = DAC_ATT505;
		}
		else
		{
			*RamDac = DAC_UNKNOWN;
		}
	}
	else if ((old3 & 0xF0) == 0x40)
	{
		Found = TRUE;
		*RamDac = DAC_ATT504;
		DoChecks = TRUE;
	}
	else
	{
		/* Perhaps status reg is hidden behind CR3 */
		wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01);
		old3 = inp(0x3C6);
		if ((old3 & 0x80) == 0x80)
		{
			/* OK.  CR3 is active... */
			wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00);
			old3 = inp(0x3C8);
			outp(0x3C8, 0x00);
			wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x02);
			old4 = inp(0x3C6);
			if ((old4 & 0x80) == 0x80)
			{
				Found = TRUE;
				if ((old4 & 0xC0) == 0x80)
				{
					*RamDac = DAC_BT485;
				}
				else if ((old4 & 0xF0) == 0xD0)
				{
					*RamDac = DAC_ATT505;
				}
				else
				{
					*RamDac = DAC_UNKNOWN;
				}
			}
			else if ((old4 & 0xF0) == 0x40)
			{
				Found = TRUE;
				*RamDac = DAC_ATT504;
			}
			if ((Found) && (*RamDac != DAC_UNKNOWN))
			{
				*RamDac |= DAC_6_8_PROGRAM;
				if (Width8Check())
				{
					*RamDac |= DAC_8BIT;
				}
			}
			wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00);
			outp(0x3C8, old3);
		}
	}
	if (DoChecks)
	{
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
	}
	wrinx(CRTC_IDX, 0x55, old2);
	outp(0x3C6, old1);

	wrinx(CRTC_IDX, 0x39, lock2);
	wrinx(CRTC_IDX, 0x38, lock1);

	return(Found);
}

static Bool S3_TVP3020Check(RamDac)
int *RamDac;
{
	Byte old1, old2, old3, old4;
	Byte lock1, lock2;
	Bool Found = FALSE;

	/*
	 * TI ViewPoint TVP3020 support - Robin Cutshaw
	 *
	 * The 3020 has 8 direct registers accessed through standard
	 * VGA registers 0x3C8, 0x3C9, 0x3C6, and 0x3C7.  Bit 0 of
	 * CR55 is used to map these four register to the low four
	 * or high four direct 3020 registers.  The high register set
	 * includes index and data registers which are used to address
	 * indirect registers 0x00-0x3F and 0xFF.  Indirect register
	 * 0x3F is the chip ID register which will always return 0x20.
	 */

	lock1 = rdinx(CRTC_IDX, 0x38);
	lock2 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x48);
	wrinx(CRTC_IDX, 0x39, 0xA5);

	old1 = inp(0x3C6);
	old2 = rdinx(CRTC_IDX, 0x55);

	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01); /* high four registers */
	old3 = inp(0x3C6);             /* read current index register value */

	outp(0x3C6, 0x3F);     /* write ID register index to index register */
	old4 = inp(0x3C7);     /* read ID register from data register       */
	if (old4 == 0x20) {
		Found = TRUE;
		*RamDac = DAC_TVP3020 | DAC_6_8_PROGRAM;
		wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00); /* regular VGA */
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
	} else {
	    Byte old5, old6;

	    /* check for Ti3025 hiding behind Bt485 mode */
	    old5 = rdinx(CRTC_IDX, 0x5C);

	    /* clear 0x20 (RS4) for 3020 mode */
	    wrinx(CRTC_IDX, 0x5C, old5 & 0xDF);
	    /* already twiddled CR55 above */
	    old6 = inp(0x3C6);          /* read current index register value */
	    outp(0x3C6, 0x3F);  /* write ID register index to index register */
	    old4 = inp(0x3C7);  /* read ID register from data register       */
	    if (old4 == 0x25) {
		Found = TRUE;
	        *RamDac = DAC_TVP3025 | DAC_6_8_PROGRAM;
		wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00); /* regular VGA */
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
	        wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01);
	    }

	    outp(0x3C6, old6);              /* restore index register value */
	    wrinx(CRTC_IDX, 0x5C, old5);    /* restore 5C                   */
        }

	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x01); /* high four registers */
	outp(0x3C6, old3);                  /* restore index register value */

	wrinx(CRTC_IDX, 0x55, old2);
	outp(0x3C6, old1);

	wrinx(CRTC_IDX, 0x39, lock2);
	wrinx(CRTC_IDX, 0x38, lock1);

	return(Found);
}

static Bool S3_TVP3026Check(RamDac)
int *RamDac;
{
	Byte old1, old2, old3, old4, old5;
	Byte lock1, lock2;
	Bool Found = FALSE;

	/*
	 * TI ViewPoint TVP3026/3030 support - Harald Koenig
	 *
	 * The 3026/3030 have 16 direct registers accessed through standard
	 * VGA registers 0x3C8, 0x3C9, 0x3C6, and 0x3C7.  Bits 0,1 of
	 * CR55 are used to map these four register to sets of four 
	 * direct 3026/3030 registers each.  The 00 register set includes 
	 * the index register and the 10 register set includes the 
	 * data register which are used to address indirect registers 
	 * 0x00-0x3F and 0xFF.  Indirect register 0x3F is the 
	 * chip ID register which will always return 0x26.
	 */

	lock1 = rdinx(CRTC_IDX, 0x38);
	lock2 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x48);
	wrinx(CRTC_IDX, 0x39, 0xA5);

	old1 = inp(0x3C6);
	old2 = rdinx(CRTC_IDX, 0x55);
	old5 = rdinx(CRTC_IDX, 0x45);

	
	wrinx(CRTC_IDX, 0x45, old5 & ~0x20);
	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00);   /* 00 four registers */
	old3 = inp(0x3C8);             /* read current index register value */

	outp(0x3C8, 0x3F);     /* write ID register index to index register */
	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x02);   /* 10 four registers */
	old4 = inp(0x3C6);     /* read ID register from data register       */
	if (old4 == 0x26 || old4 == 0x30) {
	    outp(0x3C6, ~old4);  /* check if ID register is read only */
	    if (inp(0x3C6) != old4) {
	        outp(0x3C6, old4);
	    }
	    else {
		Found = TRUE;
		if (old4 == 0x26)
		   *RamDac = DAC_TVP3026 | DAC_6_8_PROGRAM;
		else /* old4 == 0x30 */
		   *RamDac = DAC_TVP3030 | DAC_6_8_PROGRAM;
		wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00); /* regular VGA */
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
	    }
	}

	wrinx(CRTC_IDX, 0x55, (old2 & 0xFC) | 0x00);   /* 00 four registers */
	outp(0x3C8, old3);                  /* restore index register value */

	wrinx(CRTC_IDX, 0x55, old2);
	wrinx(CRTC_IDX, 0x45, old5);
	outp(0x3C6, old1);

	wrinx(CRTC_IDX, 0x39, lock2);
	wrinx(CRTC_IDX, 0x38, lock1);

	return(Found);
}

static Bool S3_IBMRGBCheck(RamDac)
int *RamDac;
{
	Byte lock1, lock2;
	Bool Found = FALSE;
	unsigned char CR43, CR55, dac[3], lut[6];
	unsigned char ilow, ihigh, id, rev, id2, rev2;	
	int i,j;
	int ret=0;

	/*
	 * IBM RGB52x support - Harald Koenig
	 */

	lock1 = rdinx(CRTC_IDX, 0x38);
	lock2 = rdinx(CRTC_IDX, 0x39);
	wrinx(CRTC_IDX, 0x38, 0x48);
	wrinx(CRTC_IDX, 0x39, 0xA5);

	CR43 = rdinx(CRTC_IDX, 0x43);
	CR55 = rdinx(CRTC_IDX, 0x55);

	wrinx(CRTC_IDX, 0x43, CR43 & ~0x02);
	wrinx(CRTC_IDX, 0x55, CR55 & ~0x03);
	
	/* save DAC and first LUT entries */
	for (i=0; i<3; i++) 
	   dac[i] = inp(0x3c6+i);
	for (i=j=0; i<2; i++) {
	   outp(0x3c7, i);
	   lut[j++] = inp(0x3c9);
	   lut[j++] = inp(0x3c9);
	   lut[j++] = inp(0x3c9);
	}
	
	wrinx(CRTC_IDX, 0x55, (CR55 & ~0x03) | 0x01);  /* set RS2 */
	
	/* read ID and revision */
	ilow  = inp(0x3c8);
	ihigh = inp(0x3c9);
	outp(0x3c9, 0);  /* index high */
	outp(0x3c8, 0);
	rev = inp(0x3c6);
	outp(0x3c8, 1);
	id  = inp(0x3c6);
	
	/* known IDs:  
	   1 = RGB525
	   2 = RGB524, RGB528 
	   */
	
	if (id >= 1 && id <= 2) { 
	   /* check if ID and revision are read only */
	   outp(0x3c8, 0);
	   outp(0x3c6, ~rev);
	   outp(0x3c8, 1);
	   outp(0x3c6, ~id);
	   outp(0x3c8, 0);
	   rev2 = inp(0x3c6);
	   outp(0x3c8, 1);
	   id2  = inp(0x3c6);
      
	   if (id == id2 && rev == rev2) { /* IBM RGB52x found */
	      Found = TRUE;
	      switch(id) {
	      case 1: 
		 *RamDac = DAC_IBMRGB525 | DAC_6_8_PROGRAM;
		 break;
	      case 2: 
		 outp(0x3c8, 0x70);
		 id2  = inp(0x3c6);
		 if ((id2 & 0x03) == 3)
		    *RamDac = DAC_IBMRGB528 | DAC_6_8_PROGRAM;
		 else
		    *RamDac = DAC_IBMRGB524 | DAC_6_8_PROGRAM;
		 break;
	      }
	   }
	   else {
	      outp(0x3c8, 0);
	      outp(0x3c6, rev);
	      outp(0x3c8, 1);
	      outp(0x3c6, id);
	   }
	}   
	outp(0x3c8,  ilow);
	outp(0x3c9, ihigh);
	
	wrinx(CRTC_IDX, 0x55, CR55 & ~0x03);
	
	/* restore DAC and first LUT entries */
	for (i=j=0; i<2; i++) {
	   outp(0x3c8, i);
	   outp(0x3c9, lut[j++]);
	   outp(0x3c9 ,lut[j++]);
	   outp(0x3c9 ,lut[j++]);
	}
	for (i=0; i<3; i++) 
	   outp(0x3c6+i, dac[i]);
	
	wrinx(CRTC_IDX, 0x43, CR43);
	wrinx(CRTC_IDX, 0x55, CR55);
	

	if (Found && Width8Check()) {
	   *RamDac |= DAC_8BIT;
	}

	wrinx(CRTC_IDX, 0x39, lock2);
	wrinx(CRTC_IDX, 0x38, lock1);

	return(Found);
}

static Bool S3_ATT498Check(RamDac)
int *RamDac;
{
	Byte mir, dir, daccomm;
	int i;
	Bool Found = FALSE;

	/*
	 * ATT20C498 support - Harald Koenig
	 * ATT20C409 and
	 * ATT20C499 support - Dirk Hohndel
	 *
	 * The ATT498 has 4 direct registers accessed through standard
	 * VGA registers 0x3C8, 0x3C9, 0x3C6, and 0x3C7 and 6 indirect
	 * registers accessed through a back door by successive reads 
	 * on RMR (Pixel read mask register 0x3C6).
	 */

	dactocomm();
	inp(0x3C6);		/* reading CR0 */
	mir = inp(0x3C6);
	dir = inp(0x3C6);
	dactopel();

	if ((mir == 0x84) && (dir == 0x98)) {
	   daccomm = getdaccomm();
	   SetComm(0);
	   SetComm(0x0a);
	   if (getdaccomm() == 0)
	      *RamDac = DAC_ATT22C498;
	   else
	      *RamDac = DAC_ATT498;
	   SetComm(daccomm);
	   Found = TRUE;
	   *RamDac |= DAC_6_8_PROGRAM;
	}
	else if( (mir = 0x84) && (dir == 0x09) ) {
	   SetComm(daccomm);
	   *RamDac = DAC_ATT409 | DAC_6_8_PROGRAM;
	   Found = TRUE;
	}
	else if( (mir = 0x84) && (dir == 0x99) ) {
	   SetComm(daccomm);
	   *RamDac = DAC_ATT499 | DAC_6_8_PROGRAM;
	   Found = TRUE;
	}

	return(Found);
}

static Bool CH8398Check(RamDac)
int *RamDac;
{
/* This dac does not provide much information to distinguish itself. */
/* So this probe may not be suitable, unless you know that the ch8398 */
/* is a possibility. */
	Byte cid;
	Bool Found = FALSE;

	dactopel();
	inp(0x3c6);
	inp(0x3c6);
	inp(0x3c6);
	cid = inp(0x3c6);     /* device ID */
        if (cid == 0xc0) {
	    Found = TRUE;
	    *RamDac = DAC_CH8398;
	}
	dactopel();
	return Found;
}

static Bool S3_STG1700Check(RamDac)
int *RamDac;
{
	Byte cid, did, daccomm, readmask;
	int i;
	Bool Found = FALSE;

	readmask = inp(0x3c6);
	dactopel();
	daccomm = getdaccomm();
	SetComm(daccomm | 0x10);
	dactocomm();
	inp(0x3C6);
	outp(0x3c6, 0x00);
	outp(0x3c6, 0x00);
	cid = inp(0x3c6);     /* company ID */
	did = inp(0x3c6);     /* device ID */
	dactopel();
	outp(0x3c6,readmask);
	SetComm(daccomm);

	if ((cid == 0x44) && (did == 0x00)) {
	   Found = TRUE;
	   *RamDac = DAC_STG1700;
	   *RamDac |= DAC_6_8_PROGRAM;
	}
	if ((cid == 0x44) && (did == 0x02)) {
	   Found = TRUE;
	   *RamDac = DAC_STG1702;
	   *RamDac |= DAC_6_8_PROGRAM;
	}
	if ((cid == 0x44) && (did == 0x03)) {
	   Found = TRUE;
	   *RamDac = DAC_STG1703;
	   *RamDac |= DAC_6_8_PROGRAM;
	}

	return(Found);
}

static Bool S3_GENDACCheck(RamDac)
int *RamDac;
{
   Byte daccomm;
   int i;
   Bool Found = FALSE;
   Byte lock1, lock2;

   Byte saveCR55, savelut[6];
   long clock01, clock23;

   /* probe for S3 GENDAC or SDAC */
   /* 
    * S3 GENDAC and SDAC have two fixed read only PLL clocks
    *     CLK0 f0: 25.255MHz   M-byte 0x28  N-byte 0x61
    *     CLK0 f1: 28.311MHz   M-byte 0x3d  N-byte 0x62
    * which can be used to detect GENDAC and SDAC since there is no chip-id
    * for the GENDAC.
    * 
    * NOTE: for the GENDAC on a MIRO 10SD (805+GENDAC) reading PLL values
    * for CLK0 f0 and f1 always returns 0x7f (but is documented "read only)
    */


   lock1 = rdinx(CRTC_IDX, 0x38);
   lock2 = rdinx(CRTC_IDX, 0x39);
   wrinx(CRTC_IDX, 0x38, 0x48);
   wrinx(CRTC_IDX, 0x39, 0xA5);
	 
   saveCR55 = rdinx(CRTC_IDX, 0x55);
   wrinx(CRTC_IDX, 0x55, saveCR55 & ~1);
   
   outp(0x3c7,0);
   for(i=0; i<2*3; i++)		/* save first two LUT entries */
      savelut[i] = inp(0x3c9);
   outp(0x3c8,0);
   for(i=0; i<2*3; i++)		/* set first two LUT entries to zero */
      outp(0x3c9,0);

   wrinx(CRTC_IDX, 0x55, saveCR55 | 1);
	 
   outp(0x3c7,0);
   for(i=clock01=0; i<4; i++)
      clock01 = (clock01 << 8) | (inp(0x3c9) & 0xff);
   for(i=clock23=0; i<4; i++)
      clock23 = (clock23 << 8) | (inp(0x3c9) & 0xff);

   wrinx(CRTC_IDX, 0x55, saveCR55 & ~1);

   outp(0x3c8,0);
   for(i=0; i<2*3; i++)		/* restore first two LUT entries */
      outp(0x3c9,savelut[i]);

   wrinx(CRTC_IDX, 0x55, saveCR55);
   wrinx(CRTC_IDX, 0x39, lock2);
   wrinx(CRTC_IDX, 0x38, lock1);

   if ( clock01 == 0x28613d62 ||
       (clock01 == 0x7f7f7f7f && clock23 != 0x7f7f7f7f)) {      
      Found = TRUE;
      
      dactopel();
      inp(0x3c6);
      inp(0x3c6);
      inp(0x3c6);
      
      /* the fourth read will show the SDAC chip ID and revision */
      if (((i=inp(0x3c6)) & 0xf0) == 0x70)
	 *RamDac = DAC_S3_SDAC;
      else
	 *RamDac = DAC_S3_GENDAC;
      dactopel();
   }
      
   return(Found);
}

static Bool Tseng_GENDACCheck(RamDac)
int *RamDac;
{
   Byte daccomm;
   int i;
   Bool Found = FALSE;

   Byte saveCR31, savelut[6];
   Byte saveHercComp, saveModeContr;
   long clock01, clock23;

   /* probe for Tseng GENDAC or SDAC */
   /* 
    * S3 GENDAC and SDAC have two fixed read only PLL clocks
    *     CLK0 f0: 25.255MHz   M-byte 0x28  N-byte 0x61
    *     CLK0 f1: 28.311MHz   M-byte 0x3d  N-byte 0x62
    * which can be used to detect GENDAC and SDAC since there is no chip-id
    * for the GENDAC.
    */


   saveHercComp = inp(0x3BF);
   saveModeContr= inp(0x3D8);
   outp(0x3BF, 0x03);  /* unlock ET4000 special */
   outp(0x3D8, 0xA0);

   saveCR31 = rdinx(CRTC_IDX, 0x31);
   wrinx(CRTC_IDX, 0x31, saveCR31 & ~0x40);
   
   outp(0x3c7,0);
   for(i=0; i<2*3; i++)		/* save first two LUT entries */
      savelut[i] = inp(0x3c9);
   outp(0x3c8,0);
   for(i=0; i<2*3; i++)		/* set first two LUT entries to zero */
      outp(0x3c9,0);

   wrinx(CRTC_IDX, 0x31, saveCR31 | 0x40);
	 
   outp(0x3c7,0);
   for(i=clock01=0; i<4; i++)
      clock01 = (clock01 << 8) | (inp(0x3c9) & 0xff);
   for(i=clock23=0; i<4; i++)
      clock23 = (clock23 << 8) | (inp(0x3c9) & 0xff);

   wrinx(CRTC_IDX, 0x31, saveCR31 & ~0x40);

   outp(0x3c8,0);
   for(i=0; i<2*3; i++)		/* restore first two LUT entries */
      outp(0x3c9,savelut[i]);

   wrinx(CRTC_IDX, 0x31, saveCR31);

   outp(0x3BF, saveHercComp);
   outp(0x3D8, saveModeContr);

   if ( clock01 == 0x28613d62 ||
       (clock01 == 0x7f7f7f7f && clock23 != 0x7f7f7f7f)) {      
      Found = TRUE;
      
      dactopel();
      inp(0x3c6);
      inp(0x3c6);
      inp(0x3c6);
      
      /* the fourth read will show the SDAC chip ID and revision */
      if (((i=inp(0x3c6)) & 0xf0) == 0xb0)
	 *RamDac = DAC_ICS5341;
      else
	 *RamDac = DAC_ICS5301;  /* ID code ICS5301 = 0xf0 */
      dactopel();
   }
      
   return(Found);
}

static Bool S3_SC15025Check(RamDac)
int *RamDac;
{
	Byte c,id[4];
	int i;
	Bool Found = FALSE;

	/*
	 * Sierra 15025 support - Harald Koenig
	 *
	 * The SC15025 has 9 indexed extended registers which can be accessed
	 * by setting bit 0x10 in the command register. 
	 * In extended registers 0x9-0xC an identification code is stored
	 * should be 53 3a b1 41 
	 */

	c = getdaccomm();
	SetComm(c | 0x10); /* enable extened data registers */
	for (i=0; i<4; i++) {
	   outp(0x3C7, 0x9+i); 
	   id[i] = inp(0x3C8);
	}
	SetComm(c); /* restore command register */
	dactopel();

	if (id[0] == 'S' &&                  /* Sierra */
	    ((id[1]<<8)|id[2]) == 15025) {   /* unique for the SC 15025/26 */
	        if (id[3] != 'A') {                     /* version number */
		   fprintf(stderr,"*** ==> New Sierra SC 15025/26 version (%x) found, please report!\n",id[3]);
		}
                Found = TRUE;
		*RamDac = DAC_SIERRA24;
		*RamDac |= DAC_6_8_PROGRAM;
        }

	return(Found);
}

static Bool ARK_ZOOMDACCheck(RamDac)
int *RamDac;
{
	/*
	 * This DAC has the same ID as an AT&T ATT20C498, but
	 * with an ARK chip it is reasonable to assume it's a ZOOMDAC.
	 */
	Byte cid, did, daccomm, readmask;
	int i;
	Bool Found = FALSE;

	dactopel();
	dactocomm();
	inp(0x3C6);
	cid = inp(0x3c6);     /* company ID */
	did = inp(0x3c6);     /* device ID */
	dactopel();

	if ((cid == 0x84) && (did == 0x98)) {
	   Found = TRUE;
	   *RamDac = DAC_ZOOMDAC;
	   *RamDac |= DAC_6_8_PROGRAM;
	}

	return(Found);
}


static void CheckMach32(ChipSet, RamDac)
int ChipSet;
int *RamDac;
{
	Word Port = CONFIG_STATUS_1;
	EnableIOPorts(1, &Port);

	switch ((inpw(Port) & 0x0E00) >> 9)
	{
	case 0x00:
		*RamDac = DAC_ATI68830;
		break;
	case 0x01:
		*RamDac = DAC_SIERRA15_16;
		break;
	case 0x02:
		*RamDac = DAC_ATI68875;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		break;
	case 0x03:
		*RamDac = DAC_STANDARD;
		break;
	case 0x04:
		*RamDac = DAC_ATIMISC24;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		break;
	case 0x05:
		*RamDac = DAC_ATI68860;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
		{
			*RamDac |= DAC_8BIT;
		}
		break;
	case 0x06:
		*RamDac = DAC_STG1700;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x07:
		*RamDac = DAC_ATT498;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	}

	DisableIOPorts(1, &Port);
	return;
}

static void CheckMach64(ChipSet, RamDac)
int ChipSet;
int *RamDac;
{
	extern Word ATIMach64DAC_CNTL, ATIMach64SCRATCH_REG1;

	if (ChipSet >= CHIP_ATI264CT)
	{
		*RamDac = DAC_ATI_INTERNAL;
		*RamDac |= DAC_6_8_PROGRAM;
		if (Width8Check())
			*RamDac |= DAC_8BIT;
		return;
	}

	EnableIOPorts(1, &ATIMach64DAC_CNTL);
	EnableIOPorts(1, &ATIMach64SCRATCH_REG1);

	switch (((inpl(ATIMach64DAC_CNTL) & 0x00070000) |
		 (inpl(ATIMach64SCRATCH_REG1) & 0x0000F000)) >> 12)
	{
	case 0x10:
		*RamDac = DAC_IBMRGB525;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x20:
		*RamDac = DAC_ATI68875;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x27:
	case 0x57:
		*RamDac = DAC_TVP3025;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x30:
		*RamDac = DAC_STANDARD;
		break;
	case 0x40:
		*RamDac = DAC_ATIMISC24;
		break;
	case 0x41:
		*RamDac = DAC_ATT491;
		break;
	case 0x42:
		*RamDac = DAC_SIERRA24;
		break;
	case 0x43:
		*RamDac = DAC_MU9C1880;
		break;
	case 0x44:
		*RamDac = DAC_IMSG174;
		break;
	case 0x50:
	case 0x51:
		*RamDac = DAC_ATI68860;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x60:
		*RamDac = DAC_STG1700;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x61:
		*RamDac = DAC_ATT498;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x70:
		*RamDac = DAC_STG1702;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x71:
		*RamDac = DAC_SIERRA24;
		break;
	case 0x72:
		*RamDac = DAC_ATT498;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x73:
		*RamDac = DAC_STG1703;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x74:
		*RamDac = DAC_CH8398;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	case 0x75:
		*RamDac = DAC_ATT408;
		*RamDac |= DAC_6_8_PROGRAM;
		break;
	default:
		break;
	}
	if (Width8Check())
	{
		*RamDac |= DAC_8BIT;
	}

	DisableIOPorts(1, &ATIMach64DAC_CNTL);
	DisableIOPorts(1, &ATIMach64SCRATCH_REG1);
	return;
}

void Probe_RamDac(Chipset, RamDac)
int Chipset;
int *RamDac;
{
	Byte x, y, z, u, v, oldcomm, oldpel, notcomm, tmp;
	Bool dac8, dac8now;

	*RamDac = DAC_STANDARD;
	Ports[0] = CRTC_IDX;
	Ports[1] = CRTC_REG;
	EnableIOPorts(NUMPORTS, Ports);

	/*
	 * Handle special cases.
	 */
	if (Chipset == CHIP_AL2101)
	{
	    *RamDac = DAC_ALG1101;
	    if (Width8Check())
	    {
		*RamDac |= DAC_8BIT;
	    }
	    DisableIOPorts(NUMPORTS, Ports);
	    return;
	}
	else if (SVGA_VENDOR(Chipset) == V_ATI)
	{
	    extern Bool Crippled_Mach32, Crippled_Mach64;

	    if ((Chipset < CHIP_ATI68800_3) || Crippled_Mach32)
	    {
		if (ReadBIOS(0x44, &x, 1) != 1)
		{
		    fprintf(stderr, "%s: Failed to read ATI BIOS data\n",
			    MyName);
		    DisableIOPorts(NUMPORTS, Ports);
		    return;
		}
		if (x & 0x80)
		{
		    *RamDac = DAC_ATI68830;
		    if (Width8Check())
		    {
			*RamDac |= DAC_8BIT;
		    }
		    DisableIOPorts(NUMPORTS, Ports);
		    return;
		}
	    }
	    else if (!Crippled_Mach64)
	    {
		if (Chipset <= CHIP_ATI88800GXC)
		{
		    CheckMach32(Chipset, RamDac);
		    if (DAC_CHIP(*RamDac) != DAC_ATIMISC24)
		    {
		        DisableIOPorts(NUMPORTS, Ports);
		        return;
		    }
		}
		else
		{
		    CheckMach64(Chipset, RamDac);
		    DisableIOPorts(NUMPORTS, Ports);
		    return;
		}
	    }
	}
	else if ((SVGA_VENDOR(Chipset) == V_CIRRUS) &&
		 (Chipset >= CHIP_CL5420))
	{
	    if ((Chipset == CHIP_CL5420) || (Chipset >= CHIP_CL5410))
	    {
		*RamDac = DAC_CIRRUSA;
	    }
	    else
	    {
		*RamDac = DAC_CIRRUSB;
	    }
	    if (Width8Check())
	    {
		*RamDac |= DAC_8BIT;
	    }
	    DisableIOPorts(NUMPORTS, Ports);
	    return;
	}
	else if ((SVGA_VENDOR(Chipset) == V_S3) && (Chipset >= CHIP_S3_928D))
	{
	    if (S3_TVP3026Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_IBMRGBCheck(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_TVP3020Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_Bt485Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_ATT498Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_STG1700Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_GENDACCheck(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_SC15025Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	}
	else if (  ((SVGA_VENDOR(Chipset) == V_S3) && (Chipset >= CHIP_S3_801B))
                 ||(SVGA_VENDOR(Chipset) == V_TSENG) ) 
	{
	    if ( Chipset == CHIP_ET6K )
	    {
	      *RamDac = DAC_ET6K;
	    }
	    if (S3_GENDACCheck(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_STG1700Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (CH8398Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if ( (SVGA_VENDOR(Chipset) == V_TSENG) && ( Chipset != CHIP_ET6K ) )
	    {
	        if (Tseng_GENDACCheck(RamDac))
	        {
		   DisableIOPorts(NUMPORTS, Ports);
		   return;
	        }
	    }
	}
	else if (SVGA_VENDOR(Chipset) == V_MATROX) 
	{
		if (Chipset == CHIP_MGA2085PX)
			*RamDac = DAC_BT485;
		if (Chipset == CHIP_MGA2064W)
			*RamDac = DAC_TVP3026;
		if (Chipset == CHIP_MGA1064SG)
			*RamDac = DAC_MGA1064SG;
		DisableIOPorts(NUMPORTS, Ports);
		return;
	}
	else if ( (SVGA_VENDOR(Chipset) == V_TRIDENT) && 
		   (Chipset >= CHIP_TVGA9000I) )
	{
		if (Chipset <= CHIP_TVGA9440)
			*RamDac = DAC_TKD8001;
		else
			*RamDac = DAC_TGUIDAC;
	    	DisableIOPorts(NUMPORTS, Ports);
	    	return;
	}
	else if (SVGA_VENDOR(Chipset) == V_ARK)
	{
	    if (ARK_ZOOMDACCheck(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	    if (S3_STG1700Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	}
	else if (SVGA_VENDOR(Chipset) == V_ALLIANCE)
	{
	    if (Chipset == CHIP_ALSC6422 || Chipset == CHIP_ALSCAT24)
	    {
	        *RamDac = DAC_ALSC_642x;
	    }
	    if (S3_STG1700Check(RamDac))
	    {
		DisableIOPorts(NUMPORTS, Ports);
		return;
	    }
	}

	/*
	 * Save current state.
	 */
	(void) dactocomm();
	oldcomm = inp(0x3C6);
	dactopel();
	oldpel = inp(0x3C6);

	/*
	 * Do 8-bit-wide check.
	 */
	(void) dactocomm();
	outp(0x3C6, 0x00);
	dac8 = Width8Check();
	dactopel();

	/*
	 * Check whether this DAC has a HiColor-style command register.
	 */
	notcomm = ~oldcomm;
	outp(0x3C6, notcomm);
	(void) dactocomm();
	v = inp(0x3C6);
	if (v != notcomm)
	{
	    /*
	     * Check for early-generation HiColor-style DAC.
	     */
	    if ((SetComm(0xE0) & 0xE0) != 0xE0)
	    {
		dactopel();
		x = inp(0x3C6);
		do
		{
		    /*
		     * Wait for same value twice.
		     */
		    y = x;
		    x = inp(0x3C6);
		}
		while (x != y);
		z = x;
		u = dactocomm();
		if (u != 0x8E)
		{
		    /*
		     * If command register is 0x8E, we've got an SS24;
		     */
		    y = 8;
		    do
		    {
			x = inp(0x3C6);
			y--;
		    }
		    while ((x != 0x8E) && (y != 0));
		}
		else
		{
		    x = u;
		}
		if (x == 0x8E)
		{
		    *RamDac = DAC_SS24;
		}
		else
		{
		    /*
		     * Sierra SC11486
		     */
		    *RamDac = DAC_SIERRA15;
		}
		dactopel();
	    }
	    else
	    {
		/*
		 * New generation of advanced DACs
		 */
		if ((SetComm(0x60) & 0xE0) == 0x00)
		{
		    /*
		     * AT&T 20C490/20C493
		     */
		    if ((SetComm(0x02) & 0x02) != 0x00)
		    {
			*RamDac = DAC_ATT490;
			*RamDac |= DAC_6_8_PROGRAM;
		    }
		    else
		    {
			*RamDac = DAC_ATT493;
		    }
		}
		else
		{
		    x = SetComm(oldcomm);
		    if (inp(0x3C6) == notcomm)
		    {
			if (SetComm(0xFF) != 0xFF)
			{
			    *RamDac = DAC_ACUMOS;
			}
			else
			{
			    (void) Width8Check();
			    dactocomm();
			    tmp = inp(0x3C6);
			    dactocomm();
			    outp(0x3C6, (tmp | 0x02) & 0xFE);
			    dac8now = Width8Check();
			    if (dac8now)
			    {
				if (Width8Check())
				{
					*RamDac = DAC_ATT491;
					*RamDac |= DAC_6_8_PROGRAM;
				}
				else
				{
					*RamDac = DAC_CIRRUS24;
				}
			    }
			    else
			    {
				*RamDac = DAC_ATT492;
			    }
			}
		    }
		    else
		    {
			if (trigdac() == notcomm)
			{
				*RamDac = DAC_CIRRUS24;
			}
			else
			{
			    dactopel();
			    outp(0x3C6, 0xFF);
			    switch (trigdac())
			    {
			    case 0x44:
				*RamDac = DAC_MUSIC4870;
				break;
			    case 0x82:
				*RamDac = DAC_MUSIC4910;
				break;
			    case 0x8E:
				*RamDac = DAC_SS24;
				break;
			    default:
				if (TestDACBit(0x10,oldcomm,oldpel))
				{
					*RamDac = DAC_SIERRA24;
					*RamDac |= DAC_6_8_PROGRAM;
				}
				else if (TestDACBit(0x04,oldcomm,oldpel))
				{
					*RamDac = DAC_UNKNOWN;
				}
				else
				{
					*RamDac = DAC_SIERRA15_16;
				}
				break;
			    }
			}
		    }
		}
	    }
	    dactocomm();
	    outp(0x3C6, oldcomm);
	}
	dactopel();
	outp(0x3C6, oldpel);

	/*
	 * If no special DAC found, check for the EDSUN DAC.
	 */
	if (*RamDac == DAC_STANDARD)
	{
	    /*
	     * Write "CEGEDSUN" + mode to DAC index 0xDE (222)
	     */
	    waitforretrace();
	    outp(0x3C8, 0xDE);
	    outp(0x3C9, (Byte)'C');
	    outp(0x3C9, (Byte)'E');
	    outp(0x3C9, (Byte)'G');
	    outp(0x3C8, 0xDE);
	    outp(0x3C9, (Byte)'E');
	    outp(0x3C9, (Byte)'D');
	    outp(0x3C9, (Byte)'S');
	    outp(0x3C8, 0xDE);
	    outp(0x3C9, (Byte)'U');
	    outp(0x3C9, (Byte)'N');
	    outp(0x3C9, (Byte)'\n');
	    /*
	     * Should be in CEG mode now.
	     */
	    outp(0x3C6, 0xFF);
	    x = (inp(0x3C6) >> 4) & 0x07;
	    if (x < 0x07)
	    {
		*RamDac = DAC_EDSUN;
		waitforretrace();
		outp(0x3C8, 0xDF);
		outp(0x3C9, 0x00);
	    }
	}

	/*
	 * Remember if DAC was in 8-bit mode.
	 */
	if (dac8)
	{
		*RamDac |= DAC_8BIT;
	}

	DisableIOPorts(NUMPORTS, Ports);
	return;
}
