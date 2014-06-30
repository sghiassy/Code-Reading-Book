/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/ICS2595.c,v 3.9 1996/12/23 06:44:12 dawes Exp $ */


/* Norbert Distler ndistler@physik.tu-muenchen.de  95/01/11 */


/* $XConsortium: ICS2595.c /main/7 1996/05/07 17:13:38 kaleb $ */

#include "Xfuncproto.h"
#include "ICS2595.h" 
#include "compiler.h"
#define NO_OSLIB_PROTOTYPES
#include "xf86_OSlib.h"

#define WRITEDELAY(x) for(i=0; i<(x); i++) GlennsIODelay();
extern int vgaIOBase;
#if NeedFunctionPrototypes
static void wrtICS2595bit(int);
static Bool SetICS2595(unsigned int, unsigned int, unsigned int);
#else
static void wrtICS2595bit();
static Bool SetICS2595();
#endif

Bool
ICS2595SetClock(frequency)
  register long frequency;
{  
  int vgaCRIndex = vgaIOBase + 4;
  int vgaCRReg = vgaIOBase + 5; 
  unsigned int i, FeedDiv, BigD, Location=17;
  
          /* programm ICS2595 clocks */
      if (frequency>MAX_ICS2595_FREQ)
        return FALSE;        /* Frequency too high! */
      
      
          /* calculate POST-DIVIDER */
      BigD = 3;
      if (frequency < MIN_ICS2595_FREQ)
        BigD = 2;
      if (frequency < MIN_ICS2595_FREQ / 2)
        BigD = 1;
      if (frequency < MIN_ICS2595_FREQ / 4)
        BigD = 0;
      frequency <<= (~BigD)&3;         /* negation, two last bits significant */	
      
      if (frequency<MIN_ICS2595_FREQ)
        return FALSE; 	/* Frequency too low!  */ 
      
      FeedDiv = (unsigned int) ((frequency*46)/QUARZFREQ);
      if (FeedDiv >= 257)
        FeedDiv -= 257;
      if (FeedDiv >= 256)
        FeedDiv = 255 ;
      
      Location = 0x0d; 		/* what clock to reprogram */    		
      
      SetICS2595( FeedDiv, BigD, Location);
    
      outb(vgaCRIndex,  0x42);       /* set VCLK */
      outb(vgaCRReg,    Location);
      WRITEDELAY(20);
      outb(vgaCRIndex,  0x5c);
      outb(vgaCRReg,    0x20);       /* STROBE   */
      WRITEDELAY(20);
      outb(vgaCRReg,    0x00);
      usleep(80000);                
  
  return TRUE;
  
}	/* end of ICS2595SetClock */


static Bool 
#if NeedFunctionPrototypes
SetICS2595(unsigned int N, unsigned int D, unsigned int L)
#else
SetICS2595(N, D, L)
  unsigned int N, D, L;
#endif
{
  int vgaCRIndex = vgaIOBase + 4;
  int vgaCRReg = vgaIOBase + 5;
  unsigned int i;
  
  (void)xf86DisableInterrupts(); 

  outb(vgaCRIndex, 0x42); /* Start programming sequence for ICS2595-02 */
  outb(vgaCRReg,   0x00);
  outb(vgaCRIndex, 0x5c);
  outb(vgaCRReg,   0x00);
  usleep(50000);
  outb(vgaCRIndex, 0x42); 	/* 'rubble bits' */ 				
  outb(vgaCRReg,   0x00);
  outb(vgaCRIndex, 0x5c);
  outb(vgaCRReg,   0x20);
  WRITEDELAY(30);
  outb(vgaCRReg,   0x00);
  WRITEDELAY(30);
  outb(vgaCRIndex, 0x42);
  outb(vgaCRReg,   0x01);
  outb(vgaCRIndex, 0x5c);
  outb(vgaCRReg,   0x20);
  WRITEDELAY(30);
  outb(vgaCRReg,   0x00);
  WRITEDELAY(30); 
  
      /* enable programming of ICS2595 */
  
  wrtICS2595bit(0);              /* start bit must be 0         */
  wrtICS2595bit(0);              /* R/W control -> Write	*/
  
  for(i=1; i<6; i++)
    {
      wrtICS2595bit(L&1);    	 /* Location control		*/
      L>>=1;
    }
  
  
  for(i=1; i<9; i++)
    {   
      wrtICS2595bit(N&1);     
      N>>=1;
    }
  
  wrtICS2595bit(0);		/* disable EXTFREQ		*/
  
  wrtICS2595bit(D&1);     	/* set post-divider             */
  D>>=1;
  wrtICS2595bit(D&1); 
  
  wrtICS2595bit(1);		/* STOP1			*/
  wrtICS2595bit(1);		/* STOP2			*/

  outb(vgaCRIndex, 0x42);
  outb(vgaCRReg, 0x00);  	/* End State 			*/
   
  (void)xf86EnableInterrupts();
  usleep(10000); 
  return TRUE;         
}		

static void wrtICS2595bit(bool)
  int bool;
{ 
  int vgaCRIndex = vgaIOBase + 4;
  int vgaCRReg = vgaIOBase + 5;
  int i,ti=10;

  if (bool==1)
    {
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg,   0x04);
      WRITEDELAY(ti); 
      outb(vgaCRIndex, 0x5c);
      outb(vgaCRReg,   0x20);
      WRITEDELAY(ti); 
      outb(vgaCRReg,   0x00);
      WRITEDELAY(ti); 
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg,   0x0c);
      WRITEDELAY(ti); 
      outb(vgaCRIndex, 0x5c);
      outb(vgaCRReg,   0x20);
      WRITEDELAY(ti); 
      outb(vgaCRReg,   0x00);
      WRITEDELAY(ti); 
    }
  else
    {
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg,   0x00);
      WRITEDELAY(ti);
      outb(vgaCRIndex, 0x5c);
      outb(vgaCRReg,   0x20);
      WRITEDELAY(ti);
      outb(vgaCRReg,   0x00);
      WRITEDELAY(ti);
      outb(vgaCRIndex, 0x42);
      outb(vgaCRReg,   0x08);
      WRITEDELAY(ti);
      outb(vgaCRIndex, 0x5c);
      outb(vgaCRReg,   0x20);
      WRITEDELAY(ti);
      outb(vgaCRReg,   0x00);
      WRITEDELAY(ti);
    }
}         

/* End of ICS2595.C */
