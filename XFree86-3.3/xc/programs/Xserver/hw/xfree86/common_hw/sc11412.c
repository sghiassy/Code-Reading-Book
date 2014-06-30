/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/SC11412.c,v 3.7 1996/12/23 06:44:18 dawes Exp $ */


/* Norbert Distler ndistler@physik.tu-muenchen.de  */


/* $XConsortium: SC11412.c /main/7 1996/05/07 17:14:02 kaleb $ */

#include "Xfuncproto.h"
#include "SC11412.h" 
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"

extern int vgaIOBase;
#if NeedFunctionPrototypes
static void wrtSC11412bit(int);
static Bool SetSC11412(unsigned  int, unsigned int, unsigned int, unsigned int);
#else
static void wrtSC11412bit();
static Bool SetSC11412();
#endif

Bool
SC11412SetClock(frequency)
register long frequency;
{  
unsigned i;
unsigned  Multiplier, Divider, BigD;
unsigned ErrorTerm=32767;

SetSC11412(106,31,0,0);  /* set Mclock to 49 MHz */
 

  /* calculate postscalar divider */
  BigD = 0;
  if (frequency < MIN_SC11412_FREQ)
   BigD = 1;
  if (frequency < MIN_SC11412_FREQ / 2)
   BigD = 2;
  if (frequency < MIN_SC11412_FREQ / 4)
    BigD = 3;
  frequency <<= BigD;

if (frequency<MIN_SC11412_FREQ)
  return FALSE; 	/* Frequency too low! */ 
if (frequency>MAX_SC11412_FREQ)
  return FALSE;    /* Frequency too high! */

/* taken from F. Klemms clockprg. */

for (i=0; i<125; i++)
      { unsigned long t1=QUARZFREQ*1000*(i+3)/((unsigned long)frequency);
        unsigned long t2=(t1+1000/2)/1000;
        unsigned long t3=t1-t2*1000;
        t2 -= 2;
        t3=(long)t3 > 0 ? t3 : -t3;
        t3=t3*127/(i+3);
        if (t2 > 125)
          continue;
        if (t3 < ErrorTerm)
          ErrorTerm=(unsigned)t3, Multiplier=i, Divider=(unsigned)t2;
      }
        Multiplier+=3; 
        Divider+=2;
 if (Divider == 31)         /* avoid commom denominators of MCLK and PCLK  */
    SetSC11412(107,32,0,0); 

 return SetSC11412(Multiplier,Divider,BigD,1);

}	/* end of SC11412SetClock */


static Bool 
#if NeedFunctionPrototypes
SetSC11412(unsigned int N, unsigned int M, unsigned int D, unsigned int Clock)
#else
SetSC11412(N, M, D, Clock)
unsigned int N, M, D, Clock;
#endif
{
	/* N, M, D -> Freq = N/(M*2^D)*QUARZFREQ   C: 1 -> Pclock, 0 -> Mclock selected */
	int vgaCRIndex = vgaIOBase + 4;
	int vgaCRReg = vgaIOBase + 5;
        unsigned int i; 
	if (Clock>1)
	  return FALSE; /* ERROR in M/Pclock selection on SC11412 */

    	outb(0x3C2,0xef); 
	/* reset pointer with rising edge for access to serial interface */

	outb(vgaCRIndex,0x42);
	outb(vgaCRReg,0x00);
	outb(vgaCRReg,0x08);
	/* enable programming of SC11412 */

        wrtSC11412bit(Clock);          /* select, enable M/Pclock       */
        wrtSC11412bit(0);              /* enable output driver          */
        wrtSC11412bit(0);              /* enable VCO                    */
        wrtSC11412bit(0);              /* external freq. prog. mode     */
        wrtSC11412bit(D&1);
        wrtSC11412bit((D>>1)&1);       /* auxiliary divider    */
        
        for(i=1; i<8; i++)
        {   
                wrtSC11412bit(N&1);     
                N>>=1;
        }
        for(i=1; i<8; i++)
        {       
                wrtSC11412bit(M&1);
                M>>=1;
        }
	outb(vgaCRReg,0x02); /* End programming of SC11412 */
        return TRUE;         /* successful M/Pclock programmed */
}		

static void wrtSC11412bit(bool)
int bool;
{ 
   int vgaCRIndex = vgaIOBase + 4;
   int vgaCRReg = vgaIOBase + 5;
   if (bool==1)
   {
    outb(vgaCRReg,0x09);
    usleep(10);
    outb(vgaCRReg,0x0b);
    usleep(10);
   }
   else
   {
    outb(vgaCRReg,0x08);
    usleep(10);
    outb(vgaCRReg,0x0a);
    usleep(10);
   }
}         

/* End of SC11412.C */
