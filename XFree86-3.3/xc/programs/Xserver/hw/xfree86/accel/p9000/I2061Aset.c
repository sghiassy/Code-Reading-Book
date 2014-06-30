/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/I2061Aset.c,v 3.5 1996/12/23 06:40:25 dawes Exp $ */
/* Id: ICD2061Aset.c,v 4.0 1994/05/28 01:24:17 nygren Exp */
/* Based on the number 9 Inc code */
/* Copyright (c) 1992, Number Nine Computer Corp.  All Rights Reserved. 
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Number Nine Computer Corp not be used 
 * in advertising or publicity pertaining to distribution of the software 
 * without specific, written prior permission.  Number Nine Computer Corp 
 * makes no representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * NUMBER NINE COMPUTER CORP DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL NUMBER NINE COMPUTER CORP BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Modifications - Copyright (c) 1994, Harry Langenbacher,All Rights Reserved. 
 * HARRY LANGENBACHER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL HARRY LANGENBACHER BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notices appear in all copies and that both that
 * copyright notices and this permission notice appear in supporting
 * documentation. Harry Langenbacher makes no representations about the
 * suitability of this software for any purpose. It is provided "as is"
 * without express or implied warranty.
 */
/* $XConsortium: I2061Aset.c /main/4 1996/03/11 10:43:44 kaleb $ */

#include        "X.h"
#include        "input.h"
#include        "xf86.h"
#include        "xf86_OSlib.h"
#include	"ICD2061A.h"
#include        "p9000reg.h"

void ICD2061ASetClock ( unsigned long control_word )
   /* value now in bits 0-23 ( not 1-24 like #9 ) */
 {
  register int index;
  int savemisc ;
  int select   ;

  select = (control_word >> 21) & 7; /* extract register address for icd2061 */
                                /* if it's not vid clk we dont need it here */
                                /* 000 reg0    video clk reg 0 */
                                /* 001 reg1    video clk reg 1 */
                                /* 010 reg2    video clk reg 2 */
                                /* 011 mreg    memory clk reg */
                                /* 100 pwrdwn  divisor for power down mode */
                                /* 110 cntl reg  control reg */
                                /* 111 not used */
                                /* 101 reserved */
  if ( ( select == 7 ) || ( select == 5 ) )
    ErrorF("\aERROR illegal icd2061 register select=%d\n" , select ) ;


  /* Shut off screen */ /* why bother ?? */
  /* If you want to turn off the screen it might make more sense to turn it off
     for a few ms AFTER shifting out the number and setting MISCOUT -
     because the icd2061 output will not change 'till ~2ms after you
     quit wiggling the sel/clk/data inputs */

  /* Set up the bits and regs for sending serial data to the icd2061 */
#define CLOCK(xx) outb((short)MISC_OUT_REG, (char)(savemisc & 0xF3)| (xx) )
#define C_DATA  8
#define C_CLK   4
#define C_BOTH  0xC
#define C_NONE  0

  savemisc = inb ( (short) MISC_IN_REG ) ;  /* read MISCOUT reg */
#ifdef DEBUG
  ErrorF("ICD2061ASetClock: Debug info: savemisc=MISC_IN_REG=0x%X\n" , (int) savemisc ) ;
#endif

  /* refer to ICD2061A data sheet, and Power 9000 EISA App. Note, pg 16 */
  /* Program the IC Designs ICD2061A frequency generator */
  /* the initial Unlock sequence consists of at least 5 low to high transitions
     of CLK with DATA high, */

  for ( index = 0 ; index < 5 ; index ++ )
   {
    CLOCK(C_DATA) ; /* data held high */
    CLOCK(C_BOTH) ; /* clock goes hi while data stays hi */
   }
  /* followed immediately by a single low to high transition of CLK with DATA
     low */

  CLOCK(C_NONE); /* let them both go low */
  CLOCK(C_CLK);  /* clock goes high */
  CLOCK(C_NONE); /* start bit = 0 */
  CLOCK(C_CLK);  /* clock the start bit */

  /* shift in the 24 bit clock control word */
  for ( index = 1 ; index < 0x1000000 ; index <<= 1 )
   { /* march a 1-bit mask across the clock control word */

    /* Clock in the next magic bit - manchester style ! */
    if (control_word & index )
     { /* clock in a one */
      CLOCK(C_CLK);  /* with clock still high, data = ! bit */
      CLOCK(C_NONE); /* lower clock, dont change data */
      CLOCK(C_DATA); /* data = bit */
      CLOCK(C_BOTH); /* clock the bit */
     }
    else
     { /* clock in a zero */
      CLOCK(C_BOTH); /* with clock still high, data = ! bit */
      CLOCK(C_DATA); /* lower clock, dont change data */
      CLOCK(C_NONE); /* data = bit */
      CLOCK(C_CLK);  /* clock the bit */
     }
   }

  /* following the entry of the last data bit, a stop bit ... is issued */
  /* by bringing DATA high and toggling CLK high-to-low and low-to-high */
  CLOCK ( C_BOTH ) ; /* clock still high, bring data high */
  CLOCK ( C_DATA ) ; /* toggle clock low */
  CLOCK ( C_BOTH ) ; /* toggle clock hi */

  /* Select the CLOCK in the frequency synthesizer */
  /* if we set anything but video clocks (regs 0-2) then restore clock */
  /* select bits, otherwise set new select video freq. reg. */

  if ( ( select >= 0 ) && ( select <= 2 ) ) /* if it's a vclk reg */
   {
    CLOCK ( select << 2 ) ; /* this will set SEL1 (bit 3) and SEL0 (bit 2) */
#ifdef DEBUG
  ErrorF("ICD2061ASetClock: Debug info: savemisc=MISC_IN_REG=0x%X\n" ,
               (int) ((savemisc & 0xF3)| (select << 2) ) ) ;
#endif
   }
  else
   {
    outb ( (short) MISC_OUT_REG , (char) savemisc ) ; /* not a vclk reg restore old select bits so that old vclk is selected */
#ifdef DEBUG
  ErrorF("ICD2061ASetClock: Debug info: MISC_OUT_REG set to=0x%X\n" ,
               (int) savemisc ) ;
#endif
   }
}








