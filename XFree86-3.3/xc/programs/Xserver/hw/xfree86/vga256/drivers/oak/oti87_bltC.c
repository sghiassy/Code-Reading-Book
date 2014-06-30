/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/oak/oti87_bltC.c,v 3.2 1996/12/23 06:57:57 dawes Exp $ */





/* $XConsortium: oti87_bltC.c /main/2 1996/02/21 18:06:15 kaleb $ */

#include  "X.h"
#include  "Xmd.h"
#include  "Xproto.h"
#include  "gcstruct.h"
#include  "windowstr.h"
#include  "scrnintstr.h"
#include  "pixmapstr.h"
#include  "xf86.h"
#include  "regionstr.h"
#include  "vgaBank.h"
#include  "vga256.h"
#include  "oak_driver.h"
#include  "xf86_Config.h"


extern void cfbCopyPlane1to8();

void OAKCopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
planemask, bitplane)
        DrawablePtr pSrcDrawable;
        DrawablePtr pDstDrawable;
        int rop;
        RegionPtr prgnDst;
        DDXPointPtr pptSrc;
        unsigned long planemask;
        int bitplane;   /* Unused. */
{
  unsigned long *psrcBase, *pdstBase;
  int widthSrc, widthDst;
  int pixwidth;
  int nbox;
  BoxPtr  pbox;
  
  cfbGetLongWidthAndPointer (pSrcDrawable, widthSrc, psrcBase);
    
  cfbGetLongWidthAndPointer (pDstDrawable, widthDst, pdstBase);
      
  if (!CHECKSCREEN(pdstBase) || cfb8StippleRRop != GXcopy) 
    {
      vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
			  planemask, 1);
      return;
    }
  
  ErrorF("This is using copyplane\n");

#if 0
  vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop, prgnDst, pptSrc,
		      planemask, 1);

  return;
#endif

  nbox = REGION_NUM_RECTS(prgnDst);
  pbox = REGION_RECTS(prgnDst);
  
  /* The planemask is understood to be 1 for all cases in which */
  /* this function is called. */

  while (nbox--)
    {
      int srcx, srcy, dstx, dsty, width, height;
      int bg, fg;
      dstx = pbox->x1;
      dsty = pbox->y1;
      srcx = pptSrc->x;
      srcy = pptSrc->y;
      width = pbox->x2 - pbox->x1;
      height = pbox->y2 - pbox->y1;
      pbox++;
      pptSrc++;
      
      fg = cfb8StippleFg;
      bg = cfb8StippleBg;
      
      if (width >= 32)
#if 0
	OAKBLTWriteBitmap(dstx, dsty, width, height,
			  psrcBase, widthSrc * 4, srcx, srcy, bg, fg,
			  widthDst * 4);
#endif
       OAKColorExpandSolidFill(dstx, dsty, width, height, fg,
                                          widthDst * 4);
      
       
      else 
	{
	  /* Create singular region. */
	  RegionRec reg;
	  (*pDstDrawable->pScreen->RegionInit)(&reg, pbox - 1, 1);
                vga256CopyPlane1to8(pSrcDrawable, pDstDrawable, rop,
				    &reg, pptSrc - 1, planemask, 1);
	  (*pDstDrawable->pScreen->RegionUninit)(&reg);
	}
    }

}


