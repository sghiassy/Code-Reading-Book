/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/p9000/I2061Acal.c,v 3.5 1996/12/23 06:40:24 dawes Exp $ */
/* Id: ICD2061Acal.c,v 4.0 1994/05/28 01:24:17 nygren Exp */
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
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation.  Harry Langenbacher 
 * makes no representations about the suitability of this software for any 
 * purpose.  It is provided "as is" without express or implied warranty.
 */
/* $XConsortium: I2061Acal.c /main/4 1996/03/11 10:43:36 kaleb $ */

#include        "X.h"
#include        "input.h"
#include        "xf86.h"
#include        "xf86_OSlib.h"
#include	"ICD2061A.h"

#undef  MIN
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#undef  MAX
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))

#define MAX_NUMERATOR           130
#define MAX_DENOMINATOR         MIN(129, CRYSTAL_FREQUENCY / 400000)
#define MIN_DENOMINATOR         MAX(3, CRYSTAL_FREQUENCY / 2000000)

/* Index register frequency ranges for ICD2061A chip */
/* "The Index Field is used to preset the VCO to an appropriate range." pg. 13 */
static long vclk_range[14] =
 {
     51000000,
     53200000,
     58500000,
     60700000,
     64400000,
     66800000,
     73500000,
     75600000,
     80900000,
     83200000,
     91500000,
    100000000,
    120000000,
    135000000    /* BE WARNED THIS COULD BE VERY VERY BAD (i.e. , I'm guessing)!!!!!!! */
 } ;

unsigned long ICD2061ACalcClock
             ( long frequency , int chipreg )  /* frequency in Hz */
 /* chipreg - select one of 3 registers in icd2061a to be selected
                                          to hold new frequency parameters */
 {
  int Q_Divider ;
  int P_Divider ;
  int Mux ; /* divider after vco */

  register int      index;
  unsigned long     temp;
  int               Best_Qdivider, Best_Pdivider;
  long              min_diff;
  long               diff;

  min_diff = 0xFFFFFFF;
  Best_Pdivider = 1;
  Best_Qdivider = 1;
#ifdef DEBUG
  ErrorF ( "Buiding clock control word for freq=%d, chipreg=%d\n" ,
	  frequency , chipreg ) ;
#endif
  /* Calculate 17 bit ( 7 + 3 + 7 ) frequency control field */

  /* find what post VCO divider we need */
  Mux = 0;
  if (frequency < MIN_VCO_FREQUENCY)
    Mux = 1;
  if (frequency < MIN_VCO_FREQUENCY / 2)
    Mux = 2;
  if (frequency < MIN_VCO_FREQUENCY / 4)
    Mux = 3; /* boy, this is a really low frequency (50Mhz/4?)*/

  frequency <<= Mux ; /* make frequency = the VCO frequency we have to get */
  for (P_Divider = 4; P_Divider <= MAX_NUMERATOR; P_Divider++)
   {
    index = CRYSTAL_FREQUENCY / (frequency / P_Divider);
    if (index > MAX_DENOMINATOR)
      index = MAX_DENOMINATOR;
    if (index < MIN_DENOMINATOR)
      index = MIN_DENOMINATOR;
    for (Q_Divider = index - 3; Q_Divider < index + 4; Q_Divider++)
      if (Q_Divider >= MIN_DENOMINATOR && Q_Divider <= MAX_DENOMINATOR)
       {
        diff = (CRYSTAL_FREQUENCY / Q_Divider) * P_Divider - frequency;
        if (diff < 0)
          diff = -diff;
        if ( Best_Qdivider * ICD2061AGCD(Q_Divider, P_Divider) /
                      ICD2061AGCD(Best_Qdivider, Best_Pdivider) == Q_Divider
             &&
             Best_Pdivider * ICD2061AGCD(Q_Divider, P_Divider) /
                      ICD2061AGCD(Best_Qdivider, Best_Pdivider) == P_Divider )
         {
          if (diff > min_diff)
            diff = min_diff;
         }
        if (diff <= min_diff)
         {
          min_diff = diff;
          Best_Qdivider = Q_Divider;
          Best_Pdivider = P_Divider;
         }
       }
   }
  Q_Divider = Best_Qdivider;
  P_Divider  = Best_Pdivider;

  /* Calculate the resultant VCO frequency */
  temp = CalcVCOfreq ( Q_Divider , P_Divider ) ;

  /* look up the index field value appropriate for this vco freq */
  index = 0 ;
  while ( ( vclk_range [ index ] < temp ) && ( index < 14 ) )
    index ++ ;

  if ( index == 14 ) /* vco freq too high */
   {
     ErrorF ("\a\aProgram error - vco freq too high !\n" ) ;
   }

  /* Pack the control word for the frequency snthesizer ,
     packed into bits 0-23, not 1-24 like before */

  temp = ( (long) chipreg            << 21 ) |
         ( (long) index              << 17 ) |
         ( (long) ( P_Divider  - 3 ) << 10 ) |
         ( (long) Mux                <<  7 ) |
         ( Q_Divider - 2 )         ;

#ifdef DEBUG
  ErrorF ("Clock Control Word is 0x%06X\n" , temp ) ;
#endif

  return temp;
}

/* Number theoretic function - GCD (Greatest Common Divisor) */
int ICD2061AGCD ( int number1 , int number2 )
 {
  register int remainder = number1 % number2 ;
  while ( remainder )
   {
    number1 = number2 ;
    number2 = remainder ;
    remainder = number1 % number2 ;
   }
  return number2 ;
 }

unsigned int ICD2061AGetClock ( unsigned long control_word )
    /* value now in bits 0-23 ( not 1-24 like before ) */
 {
  unsigned int Q_Divider ;
  unsigned int Feedback_Divider_P ;
  unsigned int Post_VCO_Divider_M ;
  unsigned int FinalFreq ;
  unsigned int VCOFreq ;
  unsigned int Index ;
  unsigned int select ;

  /* Unpack the clock value */
  select             =   ( control_word >> 21 ) & 7 ;
  Index              = ( ( control_word >> 17 ) & 0x0F) ;     /* P */
  Feedback_Divider_P = ( ( control_word >> 10 ) & 0x7F) + 3 ; /* P */
  Post_VCO_Divider_M = ( ( control_word >>  7 ) & 0x07) ;     /* M */
  Q_Divider          = ( ( control_word       ) & 0x7F) + 2 ; /* Q */

  /* f(VCO) = Prescale * f(ref) * P / Q */
  /* where Prescale is determined by a bit in the control reg, and will
     be 2 or 4 , default 2 */
  /* then the VCO output is divided by 2^^M */

  VCOFreq = CalcVCOfreq ( Q_Divider , Feedback_Divider_P ) ;
  FinalFreq = VCOFreq >> Post_VCO_Divider_M ;
                                  /* (2*f(ref)*P/Q)/2^^Post_VCO_Divider_M */

#ifdef DEBUG
  ErrorF ("\
	register select=%d\n\
	Index Field       =%d=0x%X\n\
	Feedback_Divider_P=%d=0x%X\n\
	Post_VCO_Divider_M=%d\n\
	Q_Divider=%d=0x%X\n\
	VCOFreq=%d Hz, FinalFreq=%d Hz\n" ,
     select , Index , Index , Feedback_Divider_P , Feedback_Divider_P ,
     Post_VCO_Divider_M , Q_Divider , Q_Divider ,
     VCOFreq , FinalFreq ) ;
#endif

  return FinalFreq ;
 }

unsigned long CalcVCOfreq
      ( unsigned int Q_Divider , unsigned int Feedback_Divider_P )
 {
  unsigned long VCO_Freq ;

  /* f(VCO) = Prescale * f(ref) * P / Q */
  /* where Prescale is determined by a bit in the control reg, and will
     be 2 or 4 , default 2 */
  /* assume a (standard?) 14.318180 Mhz Crystal */
  VCO_Freq = ( 2 * 14318180 * Feedback_Divider_P ) / Q_Divider ;
  return VCO_Freq ;
 }





