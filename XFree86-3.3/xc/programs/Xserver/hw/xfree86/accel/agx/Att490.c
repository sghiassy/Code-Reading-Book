/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/Att490.c,v 3.2 1996/12/23 06:32:16 dawes Exp $ */
/*
 *
 */

/* $XConsortium: Att490.c /main/3 1996/02/21 17:15:08 kaleb $ */

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
#include "Att490.h"

#ifdef __STDC__
void
xf86Att490HWSave( union xf86RamDacSave * save )
#else
void
xf86Att490HWSave( save )
   union xf86RamDacSave *save;
#endif
{
   save->Att490.ComA = xf86InRamDacReg( ATT490_COMMAND_REG_A );
}

#ifdef __STDC__
void
xf86Att490HWRestore( union xf86RamDacSave * save )
#else
void
xf86Att490HWRestore( save )
   union xf86RamDacSave *save;
#endif
{
   xf86OutRamDacData( ATT490_COMMAND_REG_A, save->Att490.ComA ); 
}

#ifdef __STDC__
void
xf86Att490Init( void )
#else
void
xf86Att490Init()
#endif
{
   GlennsIODelay();
   switch( xf86RamDacBPP ) {
   
      case 8:
         xf86OutRamDacData( ATT490_COMMAND_REG_A, 
                            ATT490_SLEEP | ATT490_8BPP_PSUEDO_COLOR );
         break;

      case 15:
         xf86OutRamDacData( ATT490_COMMAND_REG_A, 
                            ATT490_SLEEP | ATT490_15BPP_EDGE_TRIGGR );
         break;
   
      case 16:
         xf86OutRamDacData( ATT490_COMMAND_REG_A, 
                            ATT490_SLEEP | ATT490_16BPP_LEVL_TRIGGR );
         break;
   
      case 24:
      case 32:
         ErrorF( "ERROR!! - Unpacked 24bpp not supported by AT&T490\n" );

   }

   GlennsIODelay();
   if (xf86Dac8Bit)
      xf86OutRamDacReg( ATT490_COMMAND_REG_A, 
                        ~ATT490_8BPP_DAC, ATT490_8BPP_DAC );

   GlennsIODelay();
   if (xf86DacSyncOnGreen)
      xf86OutRamDacReg( ATT490_COMMAND_REG_A, 
                        ATT490_SYNC_MASK, ATT490_SYNC_ON_GREEN );

   GlennsIODelay();
   xf86OutRamDacReg( ATT490_COMMAND_REG_A, ~ATT490_SLEEP, 0x00 );
}

