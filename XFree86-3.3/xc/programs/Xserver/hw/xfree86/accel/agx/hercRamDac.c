/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/hercRamDac.c,v 3.9 1996/12/23 06:33:04 dawes Exp $ */
/*
 * Copyright 1993 by David Wexelblat <dwex@goblin.org>
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
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
 * DAVID WEXELBLAT AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH 
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF 
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE AUTHORS 
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Rewritten for the Hercules Graphite by Henry A. Worth <haw30@eng.amdahl.com> 
 *
 */
/* $XConsortium: hercRamDac.c /main/8 1996/10/19 17:50:22 kaleb $ */

#define NEED_EVENTS
#include <X.h>
#include "Xproto.h"
#include <misc.h>
#include <input.h>
#include <cursorstr.h>
#include <regionstr.h>
#include <scrnintstr.h>
#include <servermd.h>
#include <windowstr.h>
#include "xf86.h"
#include "inputstr.h"
#include "xf86Priv.h"
#include "xf86_OSlib.h"
#include "xf86RamDac.h"
#include "Bt485.h"
#include "Bt481.h"
#include "hercRamDac.h"
#include "agx.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"

unsigned int  hercBrdIO = 0;
Bool hercSmallDAC = FALSE;
Bool hercBigDAC = FALSE;
unsigned int  hercMaxClockSmallDac = 85000;
unsigned int  hercMaxSingleBigDac   = 0;
unsigned int  hercMinDoubledBigDac   = 0;
unsigned int  hercMaxDoubledBigDac   = 0;


static unsigned short rs0_rs1_Map[] = { 0x2, 0x3, 0x0, 0x1 };

#ifndef __GNUC__
# define __inline__ /**/
#endif

#ifdef __STDC__
void hercOutRamDacReg(unsigned short reg, unsigned char mask, unsigned char data)
#else
void hercOutRamDacReg(reg, mask, data)
   unsigned short reg;
   unsigned char mask;
   unsigned char data;
#endif
{
   unsigned char lowBits;
   unsigned char rs2;
   unsigned int  ramDacBase; 

   if (reg < 8) {
      ramDacBase = VGA_PAL_MASK;
      lowBits = rs0_rs1_Map[ reg & 0x3 ];
   }
   else {
      ramDacBase = hercBrdIO;
      lowBits = reg & 0x3;
   }

   rs2 = reg & 0x4;

   if (rs2)
      outb(agxIdxReg, IR_SET_RS2);
   else
      outb(agxIdxReg, IR_CLEAR_RS2);

   if (mask != 0x00) {
      unsigned char tmp;
      tmp = inb(ramDacBase+lowBits) & mask;
      outb(ramDacBase+lowBits, tmp | data);
   }
   else {
      outb(ramDacBase+lowBits, data);
   }

   outb(agxIdxReg, IR_CLEAR_RS2);
}

#ifdef __STDC__
void hercOutRamDacData(unsigned short reg, unsigned char data)
#else
void hercOutRamDacData(reg, data)
unsigned short reg;
unsigned char data;
#endif
{
   unsigned char lowBits;
   unsigned char rs2;
   unsigned int  ramDacBase;

   if (reg < 8) {
      ramDacBase = VGA_PAL_MASK;
      lowBits = rs0_rs1_Map[ reg & 0x3 ];
   }
   else {
      ramDacBase = hercBrdIO;
      lowBits = reg & 0x3;
   }

   rs2 = reg & 0x4;

   if (rs2)
      outb(agxIdxReg, IR_SET_RS2);
   else
      outb(agxIdxReg, IR_CLEAR_RS2);

   outb(ramDacBase+lowBits,data);

   outb(agxIdxReg, IR_CLEAR_RS2);

}

#ifdef __STDC__
unsigned char hercInRamDacReg(unsigned short reg)
#else
unsigned char hercInRamDacReg(reg)
   unsigned short reg;
#endif
{
   unsigned char data;
   unsigned char lowBits;
   unsigned char rs2;
   unsigned int  ramDacBase;

   if (reg < 8) {
      ramDacBase = VGA_PAL_MASK;
      lowBits = rs0_rs1_Map[ reg & 0x3 ];
   }
   else {
      ramDacBase = hercBrdIO;
      lowBits = reg & 0x3;
   }

   rs2 = reg & 0x4;

   if (rs2)
      outb(agxIdxReg, IR_SET_RS2);
   else
      outb(agxIdxReg, IR_CLEAR_RS2);

   data = inb(ramDacBase+lowBits);

   outb(agxIdxReg, IR_CLEAR_RS2);

   return data;
}


/*
 * Hercules Specific RamDac Probe Routine
 *
 */

void
hercProbeRamDac(
#ifdef __STDC__
        void
#endif
)
{
   unsigned char vendVers, hercDACStat;
   int  hercDACSw;
   int  hercBigDacType;
   int  hercSmallDacType;
   int  i;
   unsigned int agxExtDacPorts[ 8 ];
   

   outb(agxIdxReg, IR_VESA_VENDOR_ID);
   hercDACStat = inb(agxByteData);
   hercBrdIO = 0x1e0 - (((hercDACStat&0x07)-1)<<4);
   for( i=0; i<8; i++ )
      agxExtDacPorts[i] = hercBrdIO + i;
   xf86AddIOPorts(agxInfoRec.scrnIndex, 8, agxExtDacPorts);
   xf86EnableIOPorts(agxInfoRec.scrnIndex);
   hercDACSw = hercBrdIO + 4;
   if (xf86Verbose) {
      ErrorF("%s %s: Hercules DACSTAT register = 0x%02x, BoardIO = 0x%02x \n",
             XCONFIG_PROBED, agxInfoRec.name, hercDACStat, hercBrdIO );
      ErrorF("%s %s: Hercules DACSW register = 0x%02x\n",
             XCONFIG_PROBED, agxInfoRec.name, inb(hercDACSw) );
   }

   hercSmallDAC =  hercDACStat & 0x08;
   if (hercSmallDAC) {
      hercSwitchToLittleDac();
      xf86OutBt481IndReg( BT482_CURS_X_LOW, 0x00, 0xC5 );
      xf86OutBt481IndReg( BT482_CURS_Y_LOW, 0x00, 0x5C );
      if ( xf86InBt481IndReg( BT482_CURS_X_LOW ) == 0xC5 
           && xf86InBt481IndReg( BT482_CURS_X_LOW ) == 0x5C ) {
         hercSmallDacType = BT482_DAC;
         if (xf86Verbose) 
            ErrorF( "%s %s: 44-pin RAMDAC detected, Bt482 assumed\n",
                    XCONFIG_PROBED, agxInfoRec.name );
       }
       else
       {
         hercSmallDacType = BT481_DAC;
         if (xf86Verbose) 
            ErrorF( "%s %s: 44-pin RAMDAC detected, Bt481 assumed\n",
                    XCONFIG_PROBED, agxInfoRec.name );
       }
       switch( xf86RamDacBPP ) {
         case 8:
            xf86MaxClock =  86500;
            break;
         case 15:
         case 16:
            xf86MaxClock =  50000;
            break;
         case 24:
         case 32:
            xf86MaxClock =  25000;
            break;
         default:
            xf86MaxClock =  0;
       }
   }
   else {
      if (xf86Verbose) 
         ErrorF( "%s %s: No 44-pin RAMDAC detected\n",
                 XCONFIG_PROBED, agxInfoRec.name );
   }

   switch (hercDACStat & 0x30) {
      case 0x20 :
         hercBigDAC = TRUE;
         hercBigDacType = BT485_DAC;
         if (xf86Verbose) 
            ErrorF( "%s %s: 84-pin Bt485 RAMDAC detected\n",
                 XCONFIG_PROBED, agxInfoRec.name );
         break;
      case 0x30 : 
         hercBigDAC = TRUE;
         hercSwitchToBigDac( FALSE );
         vendVers = hercInRamDacReg( BT485_STATUS_REG ) & 0xF0;
         if (vendVers == 0xD0) {
            hercBigDacType = ATT20C505_DAC;
            if (xf86Verbose) 
               ErrorF( "%s %s: 84-pin ATT20C505 RAMDAC detected(0x%02x)\n",
                       XCONFIG_PROBED, agxInfoRec.name, vendVers );
         }
         else {
            hercBigDacType = ATT20C504_DAC;
            if (xf86Verbose) 
               ErrorF( "%s %s: 84-pin ATT20C504 RAMDAC detected(0x%02x)\n",

                       XCONFIG_PROBED, agxInfoRec.name, vendVers );
         }
         hercSwitchToLittleDac();
         break;
      case 0x00 :
      case 0x10 :
      default:
         hercBigDAC = FALSE;
         if (xf86Verbose) 
            ErrorF( "%s %s: No 84-pin RAMDAC detected\n",
                    XCONFIG_PROBED, agxInfoRec.name );
   }
   if ( hercSmallDAC && 
        ( !hercBigDAC || xf86RamDacType == HERC_SMALL_DAC ) ) {
      /* use 44-pin dac */
      if (xf86Verbose) 
         ErrorF( "%s %s: 44-pin RAMDAC will be used\n",
                 XCONFIG_PROBED, agxInfoRec.name );
      xf86RamDacType = hercSmallDacType; 
      hercBigDAC = FALSE;
      hercSwitchToLittleDac();
   }
   else if ( hercBigDAC ) {
      /* use 84-pin dac */
      xf86RamDacType = hercBigDacType;

      switch ( xf86RamDacType ) {
   
         case BT485_DAC:
            switch( xf86RamDacBPP ) {
               case 8:
                  xf86MaxClock           = 135000;
                  hercMaxClockSmallDac   =  86500; 
                  hercMaxSingleBigDac    =  90000;
                  hercMinDoubledBigDac   =  67500;
                  hercMaxDoubledBigDac   = 135000;
                  break;
               case 15:
               case 16:
                  xf86MaxClock           =  86500;
                  hercMaxClockSmallDac   =  50000; 
                  hercMaxSingleBigDac    =  86500;
                  hercMinDoubledBigDac   =  00000;
                  hercMaxDoubledBigDac   =  86500;
                  break;
               case 24:
               case 32:
                  xf86MaxClock           =  50350;
                  hercMaxClockSmallDac   =  25000; 
                  hercMaxSingleBigDac    =  50350;
                  hercMinDoubledBigDac   =  00000;
                  hercMaxDoubledBigDac   =  50350;
                  break;
            }
            xf86MaxCurs = 64;
            break;
   
         case ATT20C505_DAC:
            switch( xf86RamDacBPP ) {
               case 8:
                  xf86MaxClock           = 135000;
                  hercMaxClockSmallDac   =  86500; 
                  hercMaxSingleBigDac    = 110000;
                  hercMinDoubledBigDac   =  90000;
                  hercMaxDoubledBigDac   = 135000;
                  break;
               case 15:
               case 16:
                  xf86MaxClock           = 135000;
                  hercMaxClockSmallDac   =  50000;
                  hercMaxSingleBigDac    = 110000;
                  hercMinDoubledBigDac   =  90000;
                  hercMaxDoubledBigDac   = 135000;
                  break;
               case 24:
               case 32:
                  xf86MaxClock           =  86500;
                  hercMaxClockSmallDac   =  25000;
                  hercMaxSingleBigDac    = 110000;
                  hercMinDoubledBigDac   =  00000;
                  hercMaxDoubledBigDac   =  00000;
                  break;
            }
            xf86MaxCurs = 32;
            break;

         case ATT20C504_DAC:
            switch( xf86RamDacBPP ) {
               case 8:
                  xf86MaxClock           = 110000;
                  hercMaxClockSmallDac   =  86500;
                  hercMaxSingleBigDac    = 110000;
                  hercMinDoubledBigDac   =  00000;
                  hercMaxDoubledBigDac   =  00000;
                  break;
               case 15:
               case 16:
                  xf86MaxClock           = 110000;
                  hercMaxClockSmallDac   =  50000;
                  hercMaxSingleBigDac    = 110000;
                  hercMinDoubledBigDac   =  00000;
                  hercMaxDoubledBigDac   =  00000;
                  break;
               case 24:
               case 32:
                  xf86MaxClock           =  75000;
                  hercMaxClockSmallDac   =  25000;
                  hercMaxSingleBigDac    =  75000;
                  hercMinDoubledBigDac   =  00000;
                  hercMaxDoubledBigDac   =  00000;
                  break;
            }
            xf86MaxCurs = 32;
            break;

      }
      if (xf86Verbose) 
         if( xf86RamDacBPP == 8 )
            ErrorF( "%s %s: 44-pin and 84-pin RAMDAC will be used\n",
                    XCONFIG_PROBED, agxInfoRec.name );
         else
            ErrorF( "%s %s: 84-pin RAMDAC will be used\n",
                    XCONFIG_PROBED, agxInfoRec.name );
      hercSwitchToLittleDac();
   }
   else {
      if (xf86Verbose) 
         ErrorF( "%s %s: Assumming Normal DAC\n",
                 XCONFIG_PROBED, agxInfoRec.name );
      xf86RamDacType = NORMAL_DAC; 
      hercBigDAC = FALSE;
   }

   if( inb(hercDACSw) & 0x01 )
      if (xf86Verbose) 
         ErrorF( "%s %s: 84-pin RAMDAC currently in use (0x%02x)\n",
                 XCONFIG_PROBED, agxInfoRec.name, inb(hercDACSw) );
   else
      if (xf86Verbose) 
         ErrorF( "%s %s: 44-pin RAMDAC currently in use (0x%02x)\n",
                 XCONFIG_PROBED, agxInfoRec.name, inb(hercDACSw) );
}

/*
 * hercSwitchToBigDac()
 *
 */
void
hercSwitchToBigDac( doubled )
   Bool doubled;
{
   int hercDacSw = hercBrdIO + 4;
   unsigned char tmp;

   unsigned char hercDac;
   Bool  wasLittle;

   outb( agxIdxReg, IR_CLEAR_RS2 );

   hercDac = inb(hercDacSw);
   wasLittle = !(inb(hercDacSw) & 0x01);

   if( wasLittle ) {
      GlennsIODelay();

      GlennsIODelay();
      xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0xFF, 0x01 ); 

      GlennsIODelay();
      outb( agxIdxReg, 0 ); 
      GlennsIODelay();
      tmp = inb( VGA_PAL_MASK );
      GlennsIODelay();
      outb( VGA_PAL_MASK, 0x00 );

      GlennsIODelay();
      outb( hercDacSw, hercDac | 0x01 );  /* switch to big dac */

      GlennsIODelay();
   }

   /* nx1 Multiplexed */
   switch( xf86RamDacBPP ) {

      case 8:
         xf86OutRamDacData( BT485_COMMAND_REG_1, BT485_CR1_8BPP );
         if( doubled ) {  /* 8BPP 4x1 Multiplexing */
            /* pixel port selected, PCLK0, and SCLK enabled */
            xf86OutRamDacData( BT485_COMMAND_REG_2, BT485_PIXEL_INPUT_GATE );
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 
                                BT485_CLOCK_DOUBLER );
         }
         else {  /* non-mulitplexed */
            /* pixel port selected, PCLK0, and SCLK enabled */
            xf86OutRamDacData( BT485_COMMAND_REG_2, BT485_PIXEL_INPUT_GATE );
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 0x00 );
         }
         /* 8-bit dac */ 
         if( xf86Dac8Bit )
            xf86OutRamDacData( BT485_COMMAND_REG_0, 0x02 );

         break;

      case 15:
         /* pixel port selected, PCLK0, and SCLK enabled */
         xf86OutRamDacData( BT485_COMMAND_REG_2, BT485_PIXEL_INPUT_GATE );
         xf86OutRamDacData( BT485_COMMAND_REG_1, 
                            BT485_CR1_16BPP
                            | BT485_CR1_2_TO_1_16BPP 
                            | BT485_CR1_BYPASS_CLUT
                            | BT485_CR1_555_16BPP );
         if( doubled ) 
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 
                                BT485_CLOCK_DOUBLER );
         else 
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 0x00 );
         break;

      case 16:
         /* pixel port selected, PCLK0, and SCLK enabled */
         xf86OutRamDacData( BT485_COMMAND_REG_2, BT485_PIXEL_INPUT_GATE );
         xf86OutRamDacData( BT485_COMMAND_REG_1, 
                            BT485_CR1_16BPP
                            | BT485_CR1_2_TO_1_16BPP 
                            | BT485_CR1_BYPASS_CLUT
                            | BT485_CR1_565_16BPP );
         if( doubled ) 
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 
                                BT485_CLOCK_DOUBLER );
         else 
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 0x00 );
         break;

      case 24:
      case 32:
         /* pixel port selected, PCLK0, and SCLK enabled */
         xf86OutRamDacData( BT485_COMMAND_REG_2, BT485_PIXEL_INPUT_GATE );
         xf86OutRamDacData( BT485_COMMAND_REG_1,
                            BT485_CR1_24BPP
                            | BT485_CR1_BYPASS_CLUT );
         if( doubled ) 
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 
                                BT485_CLOCK_DOUBLER );
         else 
            xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 0x00 );
         break;
   }

   if( wasLittle ) {
      if( inb(hercDacSw) & 0x01 ) {
         if (xf86Verbose) 
            ErrorF( "%s %s: 84-pin RAMDAC currently in use (0x%02x)\n",
                    XCONFIG_PROBED, agxInfoRec.name, inb(hercDacSw) );
         xf86RamDacHWSave = xf86Bt485HWSave;
         xf86RamDacHWRestore = xf86Bt485HWRestore;
         xf86RamDacInit = xf86Bt485Init;
      }
      else {
         if (xf86Verbose) 
            ErrorF( "%s %s: Error, 84-pin RAMDAC switchover failed (0x%02x)\n",
                    XCONFIG_PROBED, agxInfoRec.name, inb(hercDacSw) );
         xf86RamDacHWSave = xf86Bt481HWSave;
         xf86RamDacHWRestore = xf86Bt481HWRestore;
         xf86RamDacInit = xf86Bt481Init;
      }
   }
}
   
/*
 * hercPowerDownBigDac()
 *
 */
void
hercPowerDownBigDac(
#ifdef __STDC__
      void
#endif
)
{
   unsigned int hercDacSw = hercBrdIO + 4;
   unsigned char tmp;

   outb( agxIdxReg, IR_CLEAR_RS2 );

   if ( !(inb(hercDacSw) & 0x01) ) {     /* big dac */
      xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0xFF, 0x01 ); /* power-down */
      tmp = inb(hercDacSw);
      outb( agxIdxReg, IR_CLEAR_RS2 );
      inb( VGA_PAL_MASK );
      outb( VGA_PAL_MASK, 0x00 );
      outb( hercDacSw, tmp | 0x01 );  /* switch to big DAC */
      usleep(10000);
   }

   /* 2x mult disabled */
   xf86OutBt485IndReg( BT485_COMMAND_REG_3, 0x00, 0x00 );
   /* vga port selected, PCLK0, and SCLK disabled */
   xf86OutRamDacData( BT485_COMMAND_REG_2, 0x00 );

   xf86OutRamDacData( BT485_COMMAND_REG_0, 0x01 );  /* powerdown */
   outb( agxIdxReg, IR_CLEAR_RS2 );
   tmp = inb(hercDacSw);
   outb( hercDacSw, tmp & 0xFE );  /* switch to little DAC */
   GlennsIODelay();
   outb( VGA_PAL_MASK, 0x00 );
   xf86OutRamDacData( BT481_COMMAND_REG_A, 0x00 );
   xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x7E, 0x00 ); /* powerup */

}
 
/*
 * hercSwitchToLittleDac()
 *
 */
void
hercSwitchToLittleDac(
#ifdef __STDC__
      void
#endif
)
{
   int hercDacSw = hercBrdIO + 4;
   Bool wasBig = inb(hercDacSw) & 0x01;

   if( wasBig ) {
      hercPowerDownBigDac();

      if( inb(hercDacSw) & 0x01 ) {
         if (xf86Verbose) 
            ErrorF( "%s %s: Error 44-pin RAMDAC switchover failed (0x%02x)\n",
                    XCONFIG_PROBED, agxInfoRec.name, inb(hercDacSw) );
         xf86RamDacHWSave = xf86Bt485HWSave;
         xf86RamDacHWRestore = xf86Bt485HWRestore;
         xf86RamDacInit = xf86Bt485Init;
      }
      else {
         if (xf86Verbose) 
            ErrorF( "%s %s: 44-pin RAMDAC currently in use (0x%02x)\n",
                    XCONFIG_PROBED, agxInfoRec.name, inb(hercDacSw) );
         xf86RamDacHWSave = xf86Bt481HWSave;
         xf86RamDacHWRestore = xf86Bt481HWRestore;
         xf86RamDacInit = xf86Bt481Init;
      }
   }

   GlennsIODelay();
   xf86OutRamDacData( BT481_COMMAND_REG_A, 0x00 );   /* 8-bit mode */

   GlennsIODelay();
   xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x7E, 0x00 ); /* powerup */
   GlennsIODelay();
   
   if (xf86Dac8Bit)
      xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x7C, 0x2 );

   if (xf86DacSyncOnGreen)
      xf86OutBt481IndReg( BT481_COMMAND_REG_B, 0x73, 0x4 );

}


/*
 *  hercAddDoubledCLocks() -
 *     add doubled clocks
 */

void
hercAddDoubledClocks(InfoRec)
   ScrnInfoRec *InfoRec;
{
   int num = InfoRec->clocks;
   int i;

   /* second group are the usable doubled clocks */
   for( i=0; i < num; i++ ) {
         InfoRec->clock[i + num] = InfoRec->clock[i] << 1;
   }

   InfoRec->clocks = num << 1;
}

/*
 *  hercValidateCLocks() -
 *     zero out clocks that are out of spec. 
 */

void
hercValidateClocks(InfoRec)
   ScrnInfoRec *InfoRec;
{
  
   if( InfoRec->clocks > 16 ) { 
      int num = InfoRec->clocks >> 1;
      int i;
      for( i=0; i < num; i++ ) {
         int sgl = InfoRec->clock[i];
         int dbl = InfoRec->clock[i + num];

         if( InfoRec->depth == 8 && sgl > hercMaxClockSmallDac
             || InfoRec->depth > 8 && sgl > hercMaxSingleBigDac )
            InfoRec->clock[i] = 0;

         if( dbl < hercMinDoubledBigDac || dbl > hercMaxDoubledBigDac)
            InfoRec->clock[i + num] = 0;
      }
   }
   else {
      int num = InfoRec->clocks;
      int i;
      for( i=0; i < num; i++ ) {
         int sgl = InfoRec->clock[i];

         if( InfoRec->depth == 8 && sgl > hercMaxClockSmallDac
             || InfoRec->depth > 8 && sgl > hercMaxSingleBigDac )
            InfoRec->clock[i] = 0;

      }
   }
}

