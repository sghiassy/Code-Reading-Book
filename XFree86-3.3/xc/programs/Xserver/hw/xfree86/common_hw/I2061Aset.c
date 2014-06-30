/* $XFree86: xc/programs/Xserver/hw/xfree86/common_hw/I2061Aset.c,v 3.2 1996/12/23 06:44:08 dawes Exp $ */
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
/* $XConsortium: I2061Aset.c /main/5 1996/05/07 17:13:21 kaleb $ */

#include "Xfuncproto.h"
#include "compiler.h"
#include "ICD2061A.h"


int clock_m;
int clock_n;
int clock_p;

void
ICD2061ASetClock(clock_value)
register long clock_value;              /* 7bits M, 7bits N, 2bits P */
{
  register long         index;
  register char         iotemp;
  int select;
  unsigned short crtcaddr;

  select = (clock_value >> 22) & 3;

  crtcaddr = (inb(0x3CC) & 0x01) ? 0x3D4 : 0x3B4;

  /* Unlock the S3 registers */
  outb(crtcaddr, LOCK_INDEX);
  outb(crtcaddr+1, UNLOCK_PATTERN);

  /* Shut off screen */
  outb(0x3C4, 0x01);
  iotemp = inb(0x3C5);
  outb(0x3C5, iotemp | 0x20);

  /* set clock inbut to 11 binary */
  iotemp = inb(0x3CC);
  outb(0x3C2, iotemp | 0x0C);

  outb(crtcaddr, SSW_WRITE_INDEX);
  outb(crtcaddr+1, 0);

  outb(crtcaddr, MODE_CTRL_INDEX);
  iotemp = inb(crtcaddr+1) & 0xF0;

  /* Set up the softswitch write value */
#define CLOCK(x) outb(crtcaddr+1, iotemp | (x))
#define C_DATA  2
#define C_CLK   1
#define C_BOTH  3
#define C_NONE  0

  /* Program the IC Designs ICD2061A frequency generator */
  CLOCK(C_NONE);

  /* Unlock sequence */
  CLOCK(C_DATA);
  for (index = 0; index < 6; index++)
    {
      CLOCK(C_BOTH);
      CLOCK(C_DATA);
    }
  CLOCK(C_NONE);
  CLOCK(C_CLK);
  CLOCK(C_NONE);
  CLOCK(C_CLK);
  
  /* Program the 24 bit value into REG0 */
  for (index = 0; index < 24; index++)
    {
      /* Clock in the next bit */
      clock_value >>= 1;
      if (clock_value & 1)
        {
          CLOCK(C_CLK);
          CLOCK(C_NONE);
          CLOCK(C_DATA);
          CLOCK(C_BOTH);
        }
      else
        {
          CLOCK(C_BOTH);
          CLOCK(C_DATA);
          CLOCK(C_NONE);
          CLOCK(C_CLK);
        }
    }

  CLOCK(C_BOTH);
  CLOCK(C_DATA);
  CLOCK(C_BOTH);

  /* If necessary, reprogram other ICD2061A registers to defaults */

  /* Select the CLOCK in the frequency synthesizer */
  CLOCK(C_NONE | select);

  /* Turn screen back on */
  outb(0x3C4, 0x01);
  iotemp = inb(0x3C5);
  outb(0x3C5, iotemp & 0xDF);

}


long
ICD2061AGetClock(clock_value)
register long clock_value;              /* 7bits M, 7bits N, 2bits P */
{
  long                  temp;

  /* Unpack the clock value */
  clock_m = ((clock_value >>  1) & 0x7F) + 2;
  clock_n = ((clock_value >> 11) & 0x7F) + 3;
  clock_p = ((clock_value >>  8) & 0x03);

  temp = (((CRYSTAL_FREQUENCY / 2) * clock_n) / clock_m) << 1;
  temp >>= clock_p;

  return temp;
}
