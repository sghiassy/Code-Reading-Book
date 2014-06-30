/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/s3_virge/s3rop.c,v 3.1.2.1 1997/05/24 08:36:02 dawes Exp $ */

#include "X.h"

#include "gcstruct.h"
#include "windowstr.h"
#include "pixmapstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "mistruct.h"
#include "miline.h"

#include "cfb.h"
#include "cfb16.h"
#include "cfb24.h"
#include "cfb32.h"
#include "cfbmskbits.h"
#include "misc.h"
#include "xf86.h"
#include "s3v.h"

int s3alu[16] =
{
   ROP_0,		/* GXclear */
   ROP_DSa,		/* GXand */
   ROP_SDna,		/* GXandReverse */
   ROP_S,		/* GXcopy */
   ROP_DSna,		/* GXandInverted */
   ROP_D,		/* GXnoop */
   ROP_DSx,		/* GXxor */
   ROP_DSo,		/* GXor */
   ROP_DSon,		/* GXnor */
   ROP_DSxn,		/* GXequiv */
   ROP_Dn,		/* GXinvert*/
   ROP_SDno,		/* GXorReverse */
   ROP_Sn,		/* GXcopyInverted */
   ROP_DSno,		/* GXorInverted */
   ROP_DSan,		/* GXnand */
   ROP_1		/* GXset */
};

/* S -> P */
int s3alu_sp[16] =
{
   ROP_0,
   ROP_DPa,
   ROP_PDna,
   ROP_P,
   ROP_DPna,
   ROP_D,
   ROP_DPx,
   ROP_DPo,
   ROP_DPon,
   ROP_DPxn,
   ROP_Dn,
   ROP_PDno,
   ROP_Pn,
   ROP_DPno,
   ROP_DPan,
   ROP_1
};

/* ROP  ->  (ROP & P) | (D & ~P) */
int s3alu_pat[16] =
{
   ROP_0PaDPnao,
   ROP_DSaPaDPnao,
   ROP_SDnaPaDPnao,
   ROP_SPaDPnao,
   ROP_DSnaPaDPnao,
   ROP_DPaDPnao,
   ROP_DSxPaDPnao,
   ROP_DSoPaDPnao,
   ROP_DSonPaDPnao,
   ROP_DSxnPaDPnao,
   ROP_DnPaDPnao,
   ROP_SDnoPaDPnao,
   ROP_SnPaDPnao,
   ROP_DSnoPaDPnao,
   ROP_DSanPaDPnao,
   ROP_1PaDPnao
};

int s3ConvertPlanemask(GCPtr pGC, int *new_color)
{
   int color = 0, rop = -1;

   if ((pGC->planemask & s3BppPMask) == s3BppPMask) {
      rop = s3alu_sp[pGC->alu];
      color = pGC->fgPixel;
   } else {
      switch (pGC->alu) {
      case GXclear:  /* ROP_0 */
         rop = ROP_DPa;
/*
         color = ~pGC->planemask;
*/
         color = ~(pGC->planemask & 0x00ffFFff);
         break;
      case GXand:  /* ROP_DPa */
         if ((pGC->fgPixel & pGC->planemask) != pGC->planemask) {
            rop = ROP_DPa;
/*
            color = pGC->fgPixel | ~pGC->planemask;
*/
            color = pGC->fgPixel | ~(pGC->planemask & 0x00ffFFff);
         }
         break;
      case GXandReverse:  /* ROP_PDna */
         if ((pGC->fgPixel & pGC->planemask) == pGC->planemask) {
            rop = ROP_DPx;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         } else if (!(pGC->fgPixel & pGC->planemask)) {
            rop = ROP_DPa;
/*
            color = ~pGC->planemask;
*/
            color = ~(pGC->planemask & 0x00ffFFff);
         }
         break;
      case GXcopy:  /* ROP_P */
         if ((pGC->fgPixel & pGC->planemask) == pGC->planemask) {
            rop = ROP_DPo;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         } else if (!(pGC->fgPixel & pGC->planemask)) {
            rop = ROP_DPa;
/*
            color = ~pGC->planemask;
*/
            color = ~(pGC->planemask & 0x00ffFFff);
         }
         break;
      case GXandInverted:  /* ROP_DPna */
         if ((~pGC->fgPixel & pGC->planemask) != pGC->planemask) {
            rop = ROP_DPa;
/*
            color = ~pGC->fgPixel | ~pGC->planemask;
*/
            color = ~pGC->fgPixel | ~(pGC->planemask & 0x00ffFFff);
         }
         break;
      case GXnoop:  /* ROP_D */
         break;
      case GXxor:  /* ROP_DPx */
/*
         color = pGC->fgPixel & pGC->planemask;
*/
         color = pGC->fgPixel & (pGC->planemask & 0x00ffFFff);
         if (color)
            rop = ROP_DPx;
         break;
      case GXor:  /* ROP_DPo */
/*
         color = pGC->fgPixel & pGC->planemask;
*/
         color = pGC->fgPixel & (pGC->planemask & 0x00ffFFff);
         if (color)
            rop = ROP_DPo;
         break;
      case GXnor:  /* ROP_DPon */
         if ((pGC->fgPixel & pGC->planemask) == pGC->planemask) {
            rop = ROP_DPa;
/*
            color = ~pGC->planemask;
*/
            color = ~(pGC->planemask & 0x00ffFFff);
         } else if (!(pGC->fgPixel & pGC->planemask)) {
            rop = ROP_DPx;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         }
         break;
      case GXequiv:  /* ROP_DPxn */
/*
         color = ~pGC->fgPixel & pGC->planemask;
*/
         color = ~pGC->fgPixel & (pGC->planemask & 0x00ffFFff);
         if (color)
            rop = ROP_DPx;
         break;
      case GXinvert:  /* ROP_Dn */
         rop = ROP_DPx;
/*
         color = pGC->planemask;
*/
         color = pGC->planemask & 0x00ffFFff;
         break;
      case GXorReverse:  /* ROP_PDno */
         if ((pGC->fgPixel & pGC->planemask) == pGC->planemask) {
            rop = ROP_DPo;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         } else if (!(pGC->fgPixel & pGC->planemask)) {
            rop = ROP_DPx;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         }
         break;
      case GXcopyInverted:  /* ROP_Pn */
         if ((pGC->fgPixel & pGC->planemask) == pGC->planemask) {
            rop = ROP_DPa;
/*
            color = ~pGC->planemask;
*/
            color = ~(pGC->planemask & 0x00ffFFff);
         } else if (!(pGC->fgPixel & pGC->planemask)) {
            rop = ROP_DPo;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         }
      case GXorInverted:  /* ROP_DPno */
/*
         color = ~pGC->fgPixel & pGC->planemask;
*/
         color = ~pGC->fgPixel & (pGC->planemask & 0x00ffFFff);
         if (color)
            rop = ROP_DPo;
         break;
      case GXnand:  /* ROP_DPan */
         if ((pGC->fgPixel & pGC->planemask) == pGC->planemask) {
            rop = ROP_DPx;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         } else if (!(pGC->fgPixel & pGC->planemask)) {
            rop = ROP_DPo;
/*
            color = pGC->planemask;
*/
            color = pGC->planemask & 0x00ffFFff;
         }
         break;
      case GXset:  /* ROP_1 */
         rop = ROP_DPo;
/*
         color = pGC->planemask;
*/
         color = pGC->planemask & 0x00ffFFff;
         break;
      }
   }

   *new_color = color;
   return rop;
}
