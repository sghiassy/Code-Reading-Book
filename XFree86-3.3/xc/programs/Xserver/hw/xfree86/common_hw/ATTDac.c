/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/ATTDac.c,v 3.5 1996/12/23 06:44:00 dawes Exp $ */
/*
 * Copyright 1994 by David Wexelblat <dwex@goblin.org>
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of David Wexelblat not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  David Wexelblat makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * DAVID WEXELBLAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL DAVID WEXELBLAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: ATTDac.c /main/9 1996/05/07 17:12:54 kaleb $ */

#include "Xfuncproto.h"
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"
#include "misc.h"
/* #include "xf86_HWlib.h" */

#define ATT409_CC	0x06
#define ATT409_CR0	0x01

void xf86dactopel()
{
	outb(0x3C8,0);
	return;
}

unsigned char 
xf86dactocomm()
{
	(void)inb(0x3C6);
	(void)inb(0x3C6);
	(void)inb(0x3C6);
	return(inb(0x3C6));
}

unsigned char
xf86getdaccomm()
{
	unsigned char ret;

	xf86dactopel();
	(void)xf86dactocomm();
	ret = inb(0x3C6);
	xf86dactopel();

#ifdef EXTENDED_DEBUG
	ErrorF("Old CR0 0x%x\n",ret);
#endif
	return(ret);
}

void
xf86setdaccomm(comm)
unsigned char comm;
{
	xf86dactopel();
	(void)xf86dactocomm();
	outb(0x3C6, comm);
	xf86dactopel();
#ifdef EXTENDED_DEBUG
	ErrorF("New CR0 0x%x\n",comm);
#endif
	return;
}

void
xf86setdaccommbit(bits)
unsigned char bits;
{
	unsigned char tmp;

	tmp = xf86getdaccomm() | bits;
	xf86setdaccomm(tmp);
	return;
}

void
xf86clrdaccommbit(bits)
unsigned char bits;
{
	unsigned char tmp;

	tmp = xf86getdaccomm() & ~bits;
	xf86setdaccomm(tmp);
	return;
}

unsigned char
xf86getdacregindexed(idx)
unsigned char idx;
{
	unsigned char	ret;
	/*
	 * first check if we are in indexed mode
	 */
	if( !xf86testdacindexed() )
	{
		ret = xf86getdaccomm();
		ErrorF("Yieck! CR0 = 0x%x which is not indexed mode\n",ret);
	}
	outb(0x3C8,idx);
	ret = inb(0x3C6);
#ifdef EXTENDED_DEBUG
	ErrorF("Index 0x%x is 0x%x\n",idx,ret);
#endif
	return(ret);
}

void
xf86setdacregindexed(idx,val)
unsigned char idx;
unsigned char val;
{
	unsigned char	tmp;
	/*
	 * first check if we are in indexed mode
	 */
	if( !xf86testdacindexed() )
	{
		tmp = xf86getdaccomm();
		ErrorF("Yieck! CR0 = 0x%x which is not indexed mode\n",tmp);
	}
#ifdef EXTENDED_DEBUG
	outb(0x3C8,idx);
	tmp = inb(0x3C6);
#endif
	outb(0x3C8,idx);
	outb(0x3C6,val);
#ifdef EXTENDED_DEBUG
	ErrorF("Index 0x%x set to 0x%x (was 0x%x)\n",idx,val,tmp);
#endif
	return;
}

int
xf86testdacindexed()
{
	/*
	 * figure out whether we are in indexed addressing mode
	 */
	outb(0x3C8,0);		/* reset state machine, if we are non-index */
	(void) inb(0x3C6);
	(void) inb(0x3C6);
	(void) inb(0x3C6);
	(void) inb(0x3C6);
	(void) inb(0x3C6);
	if(  (inb(0x3C6) != 0x84) || ((inb(0x3C6) & 0xf) != 9) )
		return TRUE;
	else
		return FALSE;
}

static
void
Att409UseClock(clk)
int clk;
{
	unsigned char	tmp;

	/*
	 * this function should only be called when the chip is in indexed
	 * addressing mode (CR0[0] = 1) !!
	 */
		
	tmp = xf86getdacregindexed(ATT409_CC);
	if( clk <= 3 )
	{
		/*
		 * this sets the PCLK (Clock A) by setting CC[5:4]
		 */
		xf86setdacregindexed(ATT409_CC, (tmp & 0xCF) | 0x80 | clk << 4);
	}
	else
	{
		/*
		 * this sets the MCLK (Clock B) by setting CC[1:0]
		 */
		xf86setdacregindexed(ATT409_CC, (tmp & 0xFC) | 0x08 | (clk & 3));
	}

#ifdef EXTENDED_DEBUG
	ErrorF("CC  set to 0x%x (was 0x%x)\n",
			xf86getdacregindexed(ATT409_CC),tmp);
#endif

	return;
}

#define BASE_FREQ 	 14.31818
#define FREQ_MIN	(120000.0 / 8.0)	/* p = 3 */
#define FREQ_MAX	(240000.0 / 1.0)	/* p = 0 */

static
void
Att409CalcMNP(freq,m,n,p)
long freq;
unsigned char *m,*n,*p;
{
	double ffreq, ffreq_min, ffreq_max;
	double div, diff, best_diff;
	unsigned int mm,nn;
	int pp; /* let's make that signed so the loop is easier to phrase */
	unsigned char best_n=5+2, best_p=2, best_m=125+2;

	ffreq     = freq     / 1000.0 / BASE_FREQ;
	ffreq_min = FREQ_MIN / 1000.0 / BASE_FREQ;
	ffreq_max = FREQ_MAX / 1000.0 / BASE_FREQ;
	best_diff = ffreq;
   
	for (pp=3; pp >= 0; pp--) 
	{
		for (nn = 0+2; nn <= 63+2; nn++) 
		{
			mm = (int)(ffreq * nn * (1<<pp) + 0.5) ;
			if (mm < 1+2 || mm > 255+2) 
				continue;
			div = (double)(mm) / (double)(nn);	 
			if ((div >= ffreq_min) &&
			    (div <= ffreq_max)) 
			{
				diff = ffreq - div / (1<<pp);
				if (diff < 0.0) 
					diff = -diff;
				if (diff < best_diff) 
				{
					best_diff = diff;
					best_m    = mm;
					best_n    = nn;
					best_p    = pp;
				}
				if (diff < ffreq * 0.005)
				{
					/*
					 * 0.5% is close enough
					 */
					break;
				}
			}
		}
	}
   
	*m = best_m - 2;
	*n = best_n - 2;
	*p = best_p;
}
  
static
void
Att409SetFreq(clk,freq)
int clk;
long freq;
{
	unsigned char m,n,p,reg_m,reg_n;
#ifdef EXTENDED_DEBUG
	unsigned char tmp,m_old,n_old;
#endif

	/*
	 * this function should only be called when the chip is in indexed
	 * addressing mode (CR0[0] = 1) !!
	 */
	/*
	 * This function does not try to program the Loop Filter Control Bits
	 * or the Ibias Control (as we have no idea what to put there...)
	 */
	
	reg_m = 0x40 + 4*clk + ((clk >= 4) ? 0x10 : 0x0);
	reg_n = 0x41 + 4*clk + ((clk >= 4) ? 0x10 : 0x0);

	Att409CalcMNP(freq,&m,&n,&p);
	n = (n & 0x3f) | ((p & 0x3) << 6);	/* combine n and p into 8 bit */

#ifdef EXTENDED_DEBUG
	ErrorF("setting Clk %d to %1.6f MHz (m %d, n %d, p %d)\n", clk,
		((double)(m+2)/(double)((n&0x3f)+2)/(1<<p))*BASE_FREQ,
		m,(n&0x3f),p);
	m_old = xf86getdacregindexed(reg_m);
	n_old = xf86getdacregindexed(reg_n);
	ErrorF("setting Index 0x%x to 0x%x (was 0x%x)\n", reg_m, m, m_old);
	ErrorF("setting Index 0x%x to 0x%x (was 0x%x)\n", reg_n, n, n_old);
#endif
	/*
	 * now set the corresponding registers
	 *
	 * 0x48/0x49 for PCLK C (clk == 2)
	 * 0x4c/0x4d for PCLK D (clk == 3)
	 * 0x6c/0x6d for MCLK D (clk == 7)
	 */
	xf86setdacregindexed(reg_m,m);
	xf86setdacregindexed(reg_n,n);

	return;
}

int
Att409SetClock(freq, clk)
long freq;
int clk;
{
	unsigned char tmp;
	/*
	 * we accept clocks 2 and 3 (these are the programmable PCLKs C and D)
	 * and clock 7 (this we interpret as programmable MCLK D)
	 */
	if( (clk != 2) && (clk != 3) && (clk != 7) )
	{
		ErrorF("-------- Illegal Clock for ATT20C409: %d\n", clk);
		return(FALSE);
	}
	/*
	 * enable indexed addressing and power down the chip for programming
	 * the clocks without corrupting the pixel color RAM
	 */
	xf86setdaccommbit(0x09);
	usleep(300); 			/* PLL settling time */
	Att409SetFreq(clk,freq);
#if NOT_NEEDED
	Att409UseClock(clk);
#endif
	usleep(300);
	/* 
	 * turn off indexed addressing and power up RAMDAC 
	 */
	tmp = xf86getdacregindexed(ATT409_CR0);	
	xf86setdacregindexed(ATT409_CR0,tmp & ~0x09);
					
	return(TRUE);
}
