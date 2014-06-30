/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/I2061Acal.c,v 3.1 1996/12/23 06:44:08 dawes Exp $ */
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
 */
/* $XConsortium: I2061Acal.c /main/4 1996/05/07 17:13:16 kaleb $ */

#include "Xfuncproto.h"
#include "compiler.h"
#include "ICD2061A.h"

#undef  MIN
#define MIN(a, b)               (((a) < (b)) ? (a) : (b))
#undef  MAX
#define MAX(a, b)               (((a) > (b)) ? (a) : (b))

#define MAX_NUMERATOR           130
#define MAX_DENOMINATOR         MIN(129, CRYSTAL_FREQUENCY / 400000)
#define MIN_DENOMINATOR         MAX(3, CRYSTAL_FREQUENCY / 2000000)

int clock_m;
int clock_n;
int clock_p;

/* Index register frequency ranges for ICD2061A chip */
static long     vclk_range[16] = {
                0, /* should be MIN_VCO_FREQUENCY, but that causes problems. */
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
        MAX_POST_SCALE,
                0,
        };

long
ICD2061ACalcClock(frequency, select)
register long   frequency;               /* in Hz */
int select;
{
  register long         index;
  long                  temp;
  long                  min_m, min_n, min_diff;
  long                  diff;

  min_diff = 0xFFFFFFF;
  min_n = 1;
  min_m = 1;

  /* Calculate 18 bit clock value */
  clock_p = 0;
  if (frequency < MIN_VCO_FREQUENCY)
    clock_p = 1;
  if (frequency < MIN_VCO_FREQUENCY / 2)
    clock_p = 2;
  if (frequency < MIN_VCO_FREQUENCY / 4)
    clock_p = 3;
  frequency <<= clock_p;
  for (clock_n = 4; clock_n <= MAX_NUMERATOR; clock_n++)
    {
      index = CRYSTAL_FREQUENCY / (frequency / clock_n);
      if (index > MAX_DENOMINATOR)
        index = MAX_DENOMINATOR;
      if (index < MIN_DENOMINATOR)
        index = MIN_DENOMINATOR;
      for (clock_m = index - 3; clock_m < index + 4; clock_m++)
        if (clock_m >= MIN_DENOMINATOR && clock_m <= MAX_DENOMINATOR)
          {
            diff = (CRYSTAL_FREQUENCY / clock_m) * clock_n - frequency;
            if (diff < 0)
              diff = -diff;
            if (min_m * ICD2061AGCD(clock_m, clock_n) / ICD2061AGCD(min_m, min_n) == clock_m &&
              min_n * ICD2061AGCD(clock_m, clock_n) / ICD2061AGCD(min_m, min_n) == clock_n)
            if (diff > min_diff)
              diff = min_diff;
            if (diff <= min_diff)
              {
                min_diff = diff;
                min_m = clock_m;
                min_n = clock_n;
              }
          }
    }
  clock_m = min_m;
  clock_n = min_n;

  /* Calculate the index */
  temp = (((CRYSTAL_FREQUENCY / 2) * clock_n) / clock_m) << 1;
  for (index = 0; vclk_range[index + 1] < temp && index < 15; index++)
    ;

  /* Pack the clock value for the frequency snthesizer */
  temp = (((long)clock_n - 3) << 11) + ((clock_m - 2) << 1)
                + (clock_p << 8) + (index << 18) + ((long)select << 22);

  return temp;
}


/* Number theoretic function - GCD (Greatest Common Divisor) */
long
ICD2061AGCD(a, b)
register long a, b;
{
  register long c = a % b;
  while (c)
    a = b, b = c, c = a % b;
  return b;
}



