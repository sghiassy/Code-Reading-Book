/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/et4000/tseng_clock.c,v 3.3.2.2 1997/05/09 13:49:47 hohndel Exp $ */

/*
 *
 * Copyright 1993-1997 The XFree86 Project, Inc.
 *
 */
/**
 ** Clock setting methods for Tseng chips
 **
 ** This code was moved out of et4_driver.c [kmg]
 **
 ** The code in here is ONLY used used for clock probing!
 ** Setting the actual clock is done in ET4000Restore().
 **/

#include "compiler.h"

#include "xf86.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#include "tseng.h"

/*
 * ET4000ClockSelect --
 *      select one of the possible clocks ...
 */

Bool
Tseng_ET4000ClockSelect(no)
     int no;
{
  static unsigned char save1, save2, save3, save4;
  unsigned char temp;

  switch(no)
  {
    case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaIOBase + 4, 0x34); save2 = inb(vgaIOBase + 5);
      outb(0x3C4, 7); save3 = inb(0x3C5);
      if( et4000_type > TYPE_ET4000 )
      {
         outb(vgaIOBase + 4, 0x31); save4 = inb(vgaIOBase + 5);
      }
      break;
    case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outw(vgaIOBase + 4, 0x34 | (save2 << 8));
      outw(0x3C4, 7 | (save3 << 8));
      if( et4000_type > TYPE_ET4000 )
      {
         outw(vgaIOBase + 4, 0x31 | (save4 << 8));
      }
      break;
    default:
      temp = inb(0x3CC);
      outb(0x3C2, ( temp & 0xf3) | ((no << 2) & 0x0C));
      outb(vgaIOBase + 4, 0x34);	/* don't nuke the other bits in CR34 */
      temp = inb(vgaIOBase + 5);
      outw(vgaIOBase + 4, 0x34 | ((temp & 0xFD) << 8) | ((no & 0x04) << 7));

#ifndef OLD_CLOCK_SCHEME
      {
         outb(vgaIOBase + 4, 0x31);
         temp = inb(vgaIOBase + 5);
         outb(vgaIOBase + 5, (temp & 0x3f) | ((no & 0x10) << 2));
         outb(0x3C4, 7); temp = inb(0x3C5);
         outb(0x3C5, (tseng_save_divide ^ ((no & 0x8) << 3)) | (temp & 0xBF));
      }
#else
      {
         outb(0x3C4, 7); temp = inb(0x3C5);
         outb(0x3C5, (tseng_save_divide ^ ((no & 0x10) << 2)) | (temp & 0xBF));
      }
#endif
  }
  return(TRUE);
}

/*
 * LegendClockSelect --
 *      select one of the possible clocks ...
 */

Bool
Tseng_LegendClockSelect(no)
     int no;
{
  /*
   * Sigma Legend special handling
   *
   * The Legend uses an ICS 1394-046 clock generator.  This can generate 32
   * different frequencies.  The Legend can use all 32.  Here's how:
   *
   * There are two flip/flops used to latch two inputs into the ICS clock
   * generator.  The five inputs to the ICS are then
   *
   * ICS     ET-4000
   * ---     ---
   * FS0     CS0
   * FS1     CS1
   * FS2     ff0     flip/flop 0 output
   * FS3     CS2
   * FS4     ff1     flip/flop 1 output
   *
   * The flip/flops are loaded from CS0 and CS1.  The flip/flops are
   * latched by CS2, on the rising edge. After CS2 is set low, and then high,
   * it is then set to its final value.
   *
   */
  static unsigned char save1, save2;
  unsigned char temp;

  switch(no)
  {
    case CLK_REG_SAVE:
      save1 = inb(0x3CC);
      outb(vgaIOBase + 4, 0x34); save2 = inb(vgaIOBase + 5);
      break;
    case CLK_REG_RESTORE:
      outb(0x3C2, save1);
      outw(vgaIOBase + 4, 0x34 | (save2 << 8));
      break;
    default:
      temp = inb(0x3CC);
      outb(0x3C2, (temp & 0xF3) | ((no & 0x10) >> 1) | (no & 0x04));
      outw(vgaIOBase + 4, 0x0034);
      outw(vgaIOBase + 4, 0x0234);
      outw(vgaIOBase + 4, ((no & 0x08) << 6) | 0x34);
      outb(0x3C2, (temp & 0xF3) | ((no << 2) & 0x0C));
  }
  return(TRUE);
}

/*
 * ET6000ClockSelect --
 *      programmable clock chip
 */

Bool
Tseng_ET6000ClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = Tseng_ET4000ClockSelect(freq);
      break;
   default:
      {
        ET6000SetClock(freq, 2);
        result = Tseng_ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}

#ifdef W32_SUPPORT
/*
 * ICS5341ClockSelect --
 *      programmable clock chip
 */

Bool
Tseng_ICS5341ClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = Tseng_ET4000ClockSelect(freq);
      break;
   default:
      {
        /*
	 * right now this is never called
	 * the code programs the clocks directly :-(
	 */
        ET4000gendacSetClock(freq, 2); /* can't fail */
        result = Tseng_ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}

/*
 * STG1703ClockSelect --
 *      programmable clock chip
 */

Bool
Tseng_STG1703ClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = Tseng_ET4000ClockSelect(freq);
      break;
   default:
      {
        /*
	 * right now this is never called
	 * the code programs the clocks directly :-(
	 */
        ET4000stg1703SetClock(freq, 2); /* can't fail */
        result = Tseng_ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}

/*
 * ICD2061AClockSelect --
 *      programmable clock chip
 */

Bool
Tseng_ICD2061AClockSelect(freq)
     int freq;
{
   Bool result = TRUE;

   switch(freq)
   {
   case CLK_REG_SAVE:
   case CLK_REG_RESTORE:
      result = Tseng_ET4000ClockSelect(freq);
      break;
   default:
      {
        Et4000AltICD2061SetClock((long)freq*1000, 2); /* can't fail */
        result = Tseng_ET4000ClockSelect(2);
        usleep(150000);
      }
   }
   return(result);
}
#endif

