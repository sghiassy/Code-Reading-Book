/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/xf86RamDac.c,v 3.9 1996/12/23 06:33:09 dawes Exp $ */
/*
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry Worth not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Henry Worth makes no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * HENRY A. WORTH DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL HENRY A. WORTH BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author: Henry A. Worth  (haw30@eng.amdahl.com)
 * 
 */
/* $XConsortium: xf86RamDac.c /main/7 1996/02/21 17:19:01 kaleb $ */

#include "X.h"
#include "Xmd.h"
#include "input.h"
#include "scrnintstr.h"
#include "pixmapstr.h"
#include "mipointer.h"
#include "cursorstr.h"

#include "compiler.h"

#include "xf86.h"
#include "xf86Priv.h"
#include "xf86Procs.h"
#include "xf86_OSlib.h"
#include "xf86_HWlib.h"

#define XCONFIG_FLAGS_ONLY
#include "xf86_Config.h"
#include "xf86RamDac.h"
#include "Bt485.h"
#include "Bt481.h"
#include "xf861502x.h"
#include "Att490.h"

/* RamDac related globals */
Bool xf86DacSyncOnGreen = FALSE;
Bool xf86Dac8Bit = FALSE;
int  xf86RamDacType = -1;
int  xf86RamDacBPP  = 8;
int  xf86MaxCurs = 0;
int  xf86FrameX0 = 0;
int  xf86FrameY0 = 0;
int  xf86MaxClock;

unsigned char xf86SwapBits[256] = {
0x00,0x80,0x40,0xC0,0x20,0xA0,0x60,0xE0,0x10,0x90,0x50,0xD0,0x30,0xB0,0x70,0xF0,
0x08,0x88,0x48,0xC8,0x28,0xA8,0x68,0xE8,0x18,0x98,0x58,0xD8,0x38,0xB8,0x78,0xF8,
0x04,0x84,0x44,0xC4,0x24,0xA4,0x64,0xE4,0x14,0x94,0x54,0xD4,0x34,0xB4,0x74,0xF4,
0x0C,0x8C,0x4C,0xCC,0x2C,0xAC,0x6C,0xEC,0x1C,0x9C,0x5C,0xDC,0x3C,0xBC,0x7C,0xFC,
0x02,0x82,0x42,0xC2,0x22,0xA2,0x62,0xE2,0x12,0x92,0x52,0xD2,0x32,0xB2,0x72,0xF2,
0x0A,0x8A,0x4A,0xCA,0x2A,0xAA,0x6A,0xEA,0x1A,0x9A,0x5A,0xDA,0x3A,0xBA,0x7A,0xFA,
0x06,0x86,0x46,0xC6,0x26,0xA6,0x66,0xE6,0x16,0x96,0x56,0xD6,0x36,0xB6,0x76,0xF6,
0x0E,0x8E,0x4E,0xCE,0x2E,0xAE,0x6E,0xEE,0x1E,0x9E,0x5E,0xDE,0x3E,0xBE,0x7E,0xFE,
0x01,0x81,0x41,0xC1,0x21,0xA1,0x61,0xE1,0x11,0x91,0x51,0xD1,0x31,0xB1,0x71,0xF1,
0x09,0x89,0x49,0xC9,0x29,0xA9,0x69,0xE9,0x19,0x99,0x59,0xD9,0x39,0xB9,0x79,0xF9,
0x05,0x85,0x45,0xC5,0x25,0xA5,0x65,0xE5,0x15,0x95,0x55,0xD5,0x35,0xB5,0x75,0xF5,
0x0D,0x8D,0x4D,0xCD,0x2D,0xAD,0x6D,0xED,0x1D,0x9D,0x5D,0xDD,0x3D,0xBD,0x7D,0xFD,
0x03,0x83,0x43,0xC3,0x23,0xA3,0x63,0xE3,0x13,0x93,0x53,0xD3,0x33,0xB3,0x73,0xF3,
0x0B,0x8B,0x4B,0xCB,0x2B,0xAB,0x6B,0xEB,0x1B,0x9B,0x5B,0xDB,0x3B,0xBB,0x7B,0xFB,
0x07,0x87,0x47,0xC7,0x27,0xA7,0x67,0xE7,0x17,0x97,0x57,0xD7,0x37,0xB7,0x77,0xF7,
0x0F,0x8F,0x4F,0xCF,0x2F,0xAF,0x6F,0xEF,0x1F,0x9F,0x5F,0xDF,0x3F,0xBF,0x7F,0xFF
};

/* pointers to chipset/implementation specific funtions */

void (*xf86OutRamDacReg)(
#ifdef __STDC__
        unsigned short,
        unsigned char,
        unsigned char
#endif
);

void (*xf86OutRamDacData)(
#ifdef __STDC__
        unsigned short,
        unsigned char
#endif
);

unsigned char (*xf86InRamDacReg)(
#ifdef __STDC__
        unsigned short
#endif
);

void (*xf86RamDacHWSave)(
#ifdef __STDC__ 
        union xf86RamDacSave *
#endif
) = xf86RamDacHWNoop;

void (*xf86RamDacHWRestore)(
#ifdef _STDC__
        union xf86RamDacSave *
#endif
) = xf86RamDacHWNoop;

void (*xf86RamDacInit)(
#ifdef __STDC__
        void
#endif
) = xf86RamDacNoopInit;


/*
 * xf86ProbeRamDac()
 *
 *   generic probe for the RamDac type.
 *
 */
void
xf86ProbeRamDac(
#ifdef __STDC__
        void
#endif
) 
{
   unsigned char tmp, tmp1, tmp2;
   
   if (xf86RamDacType  == UNKNOWN_DAC) {
      /*
       * Bt485/AT&T20C505 first
       *
       * Probe for the bloody thing.  Set 0x3C6 to a bogus value, then
       * try to get the Bt485 status register.  If it's there, then we will
       * get something else back from this port.
       */
      unsigned char tmp2;
      tmp = xf86InRamDacReg(BT485_COMMAND_REG_0);
      xf86OutRamDacReg(BT485_COMMAND_REG_0,0x00,0x0F);
      if (((tmp2 = xf86InRamDacReg(BT485_STATUS_REG)) & 0x80) == 0x80) {
	 /*
	  * Found either a BrookTree Bt485 or AT&T 20C505.
	  */
	 if ((tmp2 & 0xF0) == 0xD0) {
	    xf86RamDacType = ATT20C505_DAC;
            if (xf86Verbose)
	       ErrorF("%s: Detected an AT&T 20C505 RAMDAC\n", XCONFIG_PROBED);
	 } else {
            /* could also be a BT484 or AT&T 20C504 */
	    xf86RamDacType = BT485_DAC;
            if (xf86Verbose)
	      ErrorF("%s: Detected a BrookTree Bt485 RAMDAC\n", XCONFIG_PROBED);
	 }
         xf86MaxCurs = 64;
         xf86RamDacHWSave = xf86Bt485HWSave;
         xf86RamDacHWRestore = xf86Bt485HWRestore;
         xf86RamDacInit = xf86Bt485Init;
      }
      xf86OutRamDacReg(BT485_COMMAND_REG_0,0x00,tmp);

#if 0 /* not fully converted yet */
      /* If it wasn't a Bt485 or AT&T 20C505, probe for the Ti3020 */
      if (xf86RamDacType == UNKNOWN_DAC) {
	 tmp1 = xf86InRamDacReg( TI_INDEX_REG );
	 if (xf86InTiIndReg( TI_ID )  == TI_VIEWPOINT_ID) {
	    /*
	     * Found TI ViewPoint DAC
	     */
            if (xf86Verbose)
	       ErrorF("%s : Detected a TI ViewPoint 3020 RAMDAC\n",
                      XCONFIG_PROBED);
	    xf86RamDacType = TI3020_DAC;
	 }
	 xf86OutRamDacData( TI_INDEX_REG, tmp1 );

         xf86MaxCurs = 64;
         xf86RamDacHWSave = xf86TiHWSave;
         xf86RamDacHWRestore = xf86TiHWRestore;
         xf86RamDacInit = xf86TiInit;
      }
#endif

      /* If it wasn't a Ti3020 either, it must be a "normal" ramdac */
      if (xf86RamDacType == UNKNOWN_DAC) {
	 xf86RamDacType = NORMAL_DAC;
         if (xf86Verbose)
	    ErrorF("%s : Assuming normal RAMDAC\n", XCONFIG_PROBED);

         xf86MaxCurs = 0;
         xf86RamDacHWSave = xf86RamDacHWNoop;
         xf86RamDacHWRestore = xf86RamDacHWNoop;
         xf86RamDacInit = xf86RamDacNoopInit;
      }
   }
}

/*
 * xf86SetUpRamDac()
 *
 *   setup pointers and globals for the RamDac type.
 *
 */
void
xf86SetUpRamDac(
#ifdef __STDC__
        void
#endif
) 
{
   switch ( xf86RamDacType ) { 
 
      case BT482_DAC:
      case BT481_DAC:
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
         xf86MaxCurs = 0;
         xf86RamDacHWSave = xf86Bt481HWSave;
         xf86RamDacHWRestore = xf86Bt481HWRestore;
         xf86RamDacInit = xf86Bt481Init;
         break;

      case SC15025_DAC:
      case SC15021_DAC:
         xf86MaxCurs = 0;
         xf86RamDacHWSave = xf86Sc1502xHWSave;
         xf86RamDacHWRestore = xf86Sc1502xHWRestore;
         xf86RamDacInit = xf86Sc1502xInit;
         SC15021 = FALSE;
         SC1502X_EPRF_SET = FALSE;
         SC1502X_IPF_ACCESS = FALSE;

         if( xf86InSc1502xIndReg( SC1502X_ID1 ) == SC1502X_ID1_SIERRA
             && xf86InSc1502xIndReg( SC1502X_ID4 ) == SC1502X_ID4_1502X ) {
              if( xf86Verbose )
                ErrorF("%s : Sierra 1502x RAMDAC -- Using RS2 access method.\n",
                    XCONFIG_PROBED );
         }
         else {
            SC1502X_IPF_ACCESS = TRUE;
            if( xf86InSc1502xIndReg( SC1502X_ID1 ) == SC1502X_ID1_SIERRA
                && xf86InSc1502xIndReg( SC1502X_ID4 ) == SC1502X_ID4_1502X ) {
               if( xf86Verbose )
                  ErrorF( "%s : Sierra 1502x RAMDAC -- \
Using IPF access method.\n",
                    XCONFIG_PROBED );
            }
            else {
               if( xf86Verbose )
                  ErrorF( "%s : Unable to access Sierra 1502x extended \
registers, assuming normal RAMDAC.\n",
                    XCONFIG_PROBED );
          
               xf86RamDacType = NORMAL_DAC; 
               if( xf86RamDacBPP != 8 )
                  xf86MaxClock = 0;
               else
                  xf86MaxClock = 85000;
               xf86MaxCurs = 0;
               xf86RamDacHWSave = xf86RamDacHWNoop;
               xf86RamDacHWRestore = xf86RamDacHWNoop;
               xf86RamDacInit = xf86RamDacNoopInit;
               break;
            }
         }

         if ( xf86InSc1502xIndReg( SC1502X_ID3 ) == SC1502X_ID3_15021 ) {
            xf86RamDacType = SC15021_DAC; 
            switch( xf86RamDacBPP ) {
               case 8:
                  xf86MaxClock = 135000;
                  break;
               case 15:
               case 16:
                  xf86MaxClock =  80000;
                  break;
               case 24:
               case 32:
                  xf86MaxClock =  67500;
                  break;
               default:
                  xf86MaxClock = 0;
            }
         }
         else {
            xf86RamDacType = SC15025_DAC; 
            switch( xf86RamDacBPP ) {
               case 8:
                  xf86MaxClock = 110000;
                  break;
               case 15:
               case 16:
                  xf86MaxClock =  65000;
                  break;
               case 24:
               case 32:
                  xf86MaxClock =  55000;
                  break;
               default:
                  xf86MaxClock =  0;
            }
         }

         if (xf86Verbose)
            xf86Sc1502xPrintId();
         
         break;

      case ATT490_DAC:
         switch( xf86RamDacBPP ) {
            case 8:
               xf86MaxClock = 110000;
               break;
            case 15:
               xf86MaxClock =  67500;
               break;
            case 16:
               xf86MaxClock =  55000;
               break;
            default:
               xf86MaxClock =  0;
         }
         xf86MaxCurs = 0;
         xf86RamDacHWSave = xf86Att490HWSave;
         xf86RamDacHWRestore = xf86Att490HWRestore;
         xf86RamDacInit = xf86Att490Init;
         break;

      case XGA_DAC:
         break;

      case UNKNOWN_DAC:
      case NORMAL_DAC:
      default:
         if( xf86RamDacBPP != 8 )
            xf86MaxClock = 0;
         else
            xf86MaxClock =  85000;
         xf86MaxCurs = 0;
         xf86RamDacHWSave = xf86RamDacHWNoop;
         xf86RamDacHWRestore = xf86RamDacHWNoop;
         xf86RamDacInit = xf86RamDacNoopInit;
   }
}

void 
#ifdef __STDC__ 
xf86RamDacHWNoop( union xf86RamDacSave * save )
#else
xf86RamDacHWNoop( save )
   union xf86RamDacSave *save;
#endif
{
}

void 
xf86RamDacNoopInit(
#ifdef __STDC__
        void
#endif
)
{
}

