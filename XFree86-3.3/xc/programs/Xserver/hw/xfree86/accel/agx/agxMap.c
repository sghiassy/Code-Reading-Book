/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxMap.c,v 3.3 1996/12/23 06:32:54 dawes Exp $ */
/*
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Henry Worrh not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Thomas Roell makes no representations
 * about the suitability of this software for any purpose.  It is provided
 * "as is" without express or implied warranty.
 *
 * HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, 
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, 
 * IN NO EVENT SHALL HENRY A. WORTH BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Henry A. Worth (haw30@eng.amdahl.com)
 *
 */
/* $XConsortium: agxMap.c /main/5 1996/02/21 17:18:15 kaleb $ */

#include "agx.h"
#include "regagx.h"

agxPixMap agxPixMaps[5];
agxPixMap *agxCurPixMap[2] = {NULL,NULL};

void
agxMapSetSrc( map )
   int map;
{
   agxPixMap     *pMap = &agxPixMaps[ (map) ];  

   unsigned char byteData;

   GE_WAIT_IDLE();

   if( AGX_SERIES(agxChipId) ) {
      outb(agxIdxReg,IR_M3_MODE_REG_3);
      byteData = inb(agxByteData);
      if( pMap->agx256Adjust )
         byteData |= IR_M3_256_SRC_ADJUST;
      else
         byteData &= ~IR_M3_256_SRC_ADJUST;
      outb(agxByteData,byteData);
  
      if( AGX_16_ONLY(agxChipId) ) {
         outb(agxIdxReg,IR_M8_MODE_REG_8);
         byteData = inb(agxByteData);
         if( pMap->agx128Adjust )
            byteData |= IR_M8_128_SRC_ADJUST;
         else
            byteData &= ~IR_M8_128_SRC_ADJUST;
         if( pMap->agx288Adjust )
            byteData |= IR_M8_288_SRC_ADJUST;
         else
            byteData &= ~IR_M8_288_SRC_ADJUST;
         outb(agxByteData,byteData);
       }
   }
   agxCurPixMap[ AGX_SRC_MAP ] = pMap;
}

void
agxMapSetDst( map )
   int map;
{
   agxPixMap     *pMap = &agxPixMaps[ (map) ];  
   unsigned char byteData;

   GE_WAIT_IDLE();

   if( AGX_SERIES(agxChipId) ) {
       outb(agxIdxReg,IR_M3_MODE_REG_3);
       byteData = inb(agxByteData);
       if( pMap->agx256Adjust )
          byteData |= IR_M3_256_DST_ADJUST;
       else
          byteData &= ~IR_M3_256_DST_ADJUST;
       outb(agxByteData,byteData);
  
       if( AGX_16_ONLY(agxChipId) ) {
          outb(agxIdxReg,IR_M8_MODE_REG_8);
          byteData = inb(agxByteData);
          if( pMap->agx128Adjust )
             byteData |= IR_M8_128_DST_ADJUST;
          else
             byteData &= ~IR_M8_128_DST_ADJUST;
          if( pMap->agx288Adjust )
             byteData |= IR_M8_288_DST_ADJUST;
          else
             byteData &= ~IR_M8_288_DST_ADJUST;
          outb(agxByteData,byteData);
        }
   }
   agxCurPixMap[ AGX_DST_MAP ] = pMap;
}

void
agxMapSetSrcDst( map )
   int map;
{
   agxPixMap     *pMap = &agxPixMaps[ (map) ];  
   unsigned char byteData;

   GE_WAIT_IDLE();

   if( AGX_SERIES(agxChipId) ) {
       outb(agxIdxReg,IR_M3_MODE_REG_3);
       byteData = inb(agxByteData);
       if( pMap->agx256Adjust )
          byteData |= IR_M3_256_DST_ADJUST | IR_M3_256_SRC_ADJUST;
       else
          byteData &= ~(IR_M3_256_DST_ADJUST | IR_M3_256_SRC_ADJUST);
       outb(agxByteData,byteData);
  
       if( AGX_16_ONLY(agxChipId) ) {
          outb(agxIdxReg,IR_M8_MODE_REG_8);
          byteData = inb(agxByteData);
          if( pMap->agx128Adjust )
             byteData |= IR_M8_128_DST_ADJUST | IR_M8_128_SRC_ADJUST;
          else
             byteData &= ~(IR_M8_128_DST_ADJUST | IR_M8_128_SRC_ADJUST);
          if( pMap->agx288Adjust )
             byteData |= IR_M8_288_DST_ADJUST | IR_M8_288_SRC_ADJUST;
          else
             byteData &= ~(IR_M8_288_DST_ADJUST | IR_M8_288_SRC_ADJUST);
          outb(agxByteData,byteData);
        }
   }
   agxCurPixMap[ AGX_SRC_MAP ] = pMap;
   agxCurPixMap[ AGX_DST_MAP ] = pMap;
}
