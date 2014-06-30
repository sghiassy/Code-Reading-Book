/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/xf861502x.c,v 3.9 1996/12/26 01:38:42 dawes Exp $ */
/*
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry A. Worth not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Henry A. Worth makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL THE HENRY A. WORTH BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER 
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
/* $XConsortium: xf861502x.c /main/6 1996/02/21 17:18:54 kaleb $ */

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
#include "xf86_Config.h"
#include "xf86RamDac.h"
#include "xf861502x.h"

Bool SC15021 = FALSE;
Bool SC1502X_EPRF_SET = FALSE;
Bool SC1502X_IPF_ACCESS = FALSE;


static unsigned short rs0_rs1_Map[] = { 0x2, 0x3, 0x0, 0x1 };

#ifdef __STDC__
void 
xf86OutSc1502xCmd( unsigned char mask, 
                   unsigned char data )
#else
void 
xf86OutSc1502xCmd(mask, data)
   unsigned char mask;
   unsigned char data;
#endif
{
   unsigned int  ramDacBase = 0x3c6;
   unsigned char tmp; 

   if( SC1502X_EPRF_SET ) {
      if (mask != 0x00) {
         unsigned char tmp;
         tmp = (inb(ramDacBase) & mask) | data;
         outb(ramDacBase, tmp);
         if( !(tmp & SC1502X_CMD_EPRF) )
            SC1502X_EPRF_SET = FALSE;
      }
      else {
         outb(ramDacBase, data);
         if( !(data & SC1502X_CMD_EPRF) )
            SC1502X_EPRF_SET = FALSE;
      }
   }
   else {
      if( SC1502X_IPF_ACCESS ) {
         /* 4 reads to set IPF bit and maps command register into mask addr */
         tmp = inb(ramDacBase+1);   /* make sure IPF is clear when we start */
         tmp = inb(ramDacBase);
         tmp = inb(ramDacBase);
         tmp = inb(ramDacBase);
         tmp = inb(ramDacBase);
         if (mask != 0x00) {
            unsigned char tmp;
            tmp = (inb(ramDacBase) & mask) | data;
            outb(ramDacBase, tmp);
            if( tmp & SC1502X_CMD_EPRF )
               SC1502X_EPRF_SET = TRUE;
         }
         else {
            outb(ramDacBase, data);
            if( data & SC1502X_CMD_EPRF )
               SC1502X_EPRF_SET = TRUE;
         }
         tmp = inb(ramDacBase+1);   /* reset IPF */
      }
      else {
         if (mask != 0x00) {
            unsigned char tmp;
            tmp = ( xf86InRamDacReg(SC1502X_COMMAND) & mask) | data;
            xf86OutRamDacData( SC1502X_COMMAND, tmp );
            if( tmp & SC1502X_CMD_EPRF )
               SC1502X_EPRF_SET = TRUE;
         }
         else {
            xf86OutRamDacData( SC1502X_COMMAND, data );
            if( data & SC1502X_CMD_EPRF )
               SC1502X_EPRF_SET = TRUE;
         }
      }
   }
}
      

#ifdef __STDC__
unsigned char
xf86InSc1502xCmd(void)
#else
unsigned char
xf86InSc1502xCmd()
#endif
{
   unsigned char tmp;
   unsigned char data;
   unsigned int  ramDacBase = 0x3c6;

   if( SC1502X_EPRF_SET ) {
      data = inb(ramDacBase);
   }
   else {
      if( SC1502X_IPF_ACCESS ) {
         /* 4 reads to set IPF bit and maps command register into mask addr */
         tmp = inb(ramDacBase+1);   /* make sure IPF is clear when we start */
         tmp = inb(ramDacBase);
         tmp = inb(ramDacBase);
         tmp = inb(ramDacBase);
         tmp = inb(ramDacBase);
         data = inb(ramDacBase);
         tmp = inb(ramDacBase+1);   /* reset IPF */
      }
      else {
         data = xf86InRamDacReg( SC1502X_COMMAND );
      }
   }
   return data;
}
   
   
/*
 * The extended, indexed,  registers
 * must be accessed indirectly. 
 */
#ifdef __STDC__
void 
xf86OutSc1502xIndReg( unsigned short reg, 
                      unsigned char mask, 
                      unsigned char data )
#else
void 
xf86OutSc1502xIndReg(reg, mask, data)
   unsigned short reg;
   unsigned char mask;
   unsigned char data;
#endif
{
   unsigned char tmp;
   Bool          setEPRF = !SC1502X_EPRF_SET;
   
   if( setEPRF ) 
      xf86OutSc1502xCmd( 0xFF, SC1502X_CMD_EPRF );
   xf86OutRamDacData(SC1502X_EXT_IDX_WO, reg);
   xf86OutRamDacReg(SC1502X_EXT_DATA, mask, data);
   if( setEPRF )
      xf86OutSc1502xCmd( ~SC1502X_CMD_EPRF, 0x00 );

}

#ifdef __STDC__
unsigned char 
xf86InSc1502xIndReg( unsigned short reg ) 
#else
unsigned char
xf86InSc1502xIndReg(reg)
   unsigned short reg;
#endif
{
   unsigned char ret;
   unsigned char tmp;
   Bool          setEPRF = !SC1502X_EPRF_SET;

   if( setEPRF ) 
      xf86OutSc1502xCmd( 0xFF, SC1502X_CMD_EPRF );
   xf86OutRamDacData(SC1502X_EXT_IDX_WO, reg);
   ret = xf86InRamDacReg(SC1502X_EXT_DATA);
   if( setEPRF )
      xf86OutSc1502xCmd( ~SC1502X_CMD_EPRF, 0x00 );

   return(ret);
}

#ifdef __STDC__
void
xf86Sc1502xHWSave( union xf86RamDacSave * save )
#else
void
xf86Sc1502xHWSave( save )
   union xf86RamDacSave *save;
#endif
{
   save->Sc1502x.Cmd = xf86InSc1502xCmd();
   save->Sc1502x.AuxCntl = xf86InSc1502xIndReg( SC1502X_AUX_CNTL );
   if( SC15021 ) {
      save->Sc1502x.SecCntl = xf86InSc1502xIndReg( SC15021_SEC_CNTL );
   }
}

#ifdef __STDC__
void
xf86Sc1502xHWRestore( union xf86RamDacSave * save )
#else
void
xf86Sc1502xHWRestore( save )
   union xf86RamDacSave *save;
#endif
{
   if( SC15021 ) {
      xf86OutSc1502xIndReg( SC15021_SEC_CNTL, 0x00, save->Sc1502x.SecCntl );
   }
   xf86OutSc1502xIndReg( SC1502X_AUX_CNTL, 0x00, save->Sc1502x.AuxCntl );
   xf86OutSc1502xCmd( 0x00, save->Sc1502x.Cmd ); 
}

#ifdef __STDC__
void
xf86Sc1502xInit( void )
#else
void
xf86Sc1502xInit()
#endif
{
   switch( xf86RamDacBPP ) {
      case 8:
         xf86OutSc1502xCmd( 0x00, SC1502X_CMD_8BPP_PSUEDO ); 
         xf86OutSc1502xIndReg( SC1502X_PIXEL_REPACK, 0x00, 
                               SC1502X_RP_8X1_TO_8X1);
         if( xf86RamDacType == SC15021_DAC ) {
            xf86OutSc1502xIndReg( SC15021_SEC_CNTL, 0x00, 
                                  SC15021_SC_8BPP_PSUEDO );
         }
         break;

      case 15:
         xf86OutSc1502xCmd( 0x00, SC1502X_CMD_15BPP );
         xf86OutSc1502xIndReg( SC1502X_PIXEL_REPACK, 0x00, 
                               SC1502X_RP_8X2_TO_16X1 );
         if( xf86RamDacType == SC15021_DAC ) {
            xf86OutSc1502xIndReg( SC15021_SEC_CNTL, 0x00, 
                                  SC15021_SC_15BPP_DIRECT );
         }
         break;

      case 16:
         xf86OutSc1502xCmd( 0x00, SC1502X_CMD_16BPP );
         xf86OutSc1502xIndReg( SC1502X_PIXEL_REPACK, 0x00, 
                               SC1502X_RP_8X2_TO_16X1 );
         if( xf86RamDacType == SC15021_DAC ) {
            xf86OutSc1502xIndReg( SC15021_SEC_CNTL, 0x00, 
                                  SC15021_SC_16BPP_DIRECT );
         }
         break;

      case 24:
      case 32:
         if( xf86RamDacType == SC15021_DAC ) {
            xf86OutSc1502xCmd( 0x00, SC1502X_CMD_24BPP_RGB );
            xf86OutSc1502xIndReg( SC1502X_PIXEL_REPACK, 0x00, 
                                  SC1502X_RP_8X3_TO_24X1 );
            xf86OutSc1502xIndReg( SC15021_SEC_CNTL, 0x00, 
                                  SC15021_SC_24BPP_RGB );
         }
         else {
            xf86OutSc1502xCmd( 0x00, SC15025_CMD_24BPP_BGR_EDGE_TRGR );
            xf86OutSc1502xIndReg( SC1502X_PIXEL_REPACK, 0x00, 
                                  SC1502X_RP_8X4_TO_24X1 );

         }
         break;

   }

   if (xf86Dac8Bit)
      xf86OutSc1502xIndReg( SC1502X_AUX_CNTL, ~0, SC1502X_AUX_8_BIT_DAC );
   else
      xf86OutSc1502xIndReg( SC1502X_AUX_CNTL, ~SC1502X_AUX_8_BIT_DAC, 0 );

   xf86OutSc1502xIndReg( SC1502X_PIXEL_MASK_LOW, 0x00, 0xFF );
   xf86OutSc1502xIndReg( SC1502X_PIXEL_MASK_MID, 0x00, 0xFF );
   xf86OutSc1502xIndReg( SC1502X_PIXEL_MASK_HI,  0x00, 0xFF );
}


#ifdef __STDC__
void
xf86Sc1502xPrintId( void )
#else
void
xf86Sc1502xPrintId()
#endif
{
   ErrorF( "%s : Sierra 1502x RAMDAC Command Register: 0x%02x, \
Aux. Control Register: 0x%02x\n",
           XCONFIG_PROBED, 
           xf86InSc1502xCmd(),
           xf86InSc1502xIndReg( SC1502X_AUX_CNTL ) ); 
   ErrorF( "%s : Sierra 1502x RAMDAC Pixel Repack Register: 0x%02x\n",
           XCONFIG_PROBED, 
           xf86InSc1502xIndReg( SC1502X_PIXEL_REPACK ) ); 
   if( SC15021 )
      ErrorF( "%s : Sierra 15021 RAMDAC Secondary Control Register: 0x%02x\n",
           XCONFIG_PROBED, 
           xf86InSc1502xIndReg( SC15021_SEC_CNTL ) ); 
   ErrorF( "%s : Sierra 1502x RAMDAC ID fields: \
0x%02x, 0x%02x, 0x%02x, 0x%02x\n",
           XCONFIG_PROBED, 
           xf86InSc1502xIndReg( SC1502X_ID1 ),
           xf86InSc1502xIndReg( SC1502X_ID2 ),
           xf86InSc1502xIndReg( SC1502X_ID3 ),
           xf86InSc1502xIndReg( SC1502X_ID4 )  );
   ErrorF( "%s : Sierra 1502x RAMDAC Secondary Pixel Masks: \
0x%02x, 0x%02x, 0x%02x\n",
           XCONFIG_PROBED, 
           xf86InSc1502xIndReg( SC1502X_PIXEL_MASK_LOW ),
           xf86InSc1502xIndReg( SC1502X_PIXEL_MASK_MID ),
           xf86InSc1502xIndReg( SC1502X_PIXEL_MASK_HI  )  );
}
