/* $XFree86: xc/programs/Xserver/hw/xfree86/mono/drivers/apollo/apolloBlt.c,v 3.4 1996/12/23 06:47:53 dawes Exp $ */
/*
 * MONO: Driver family for interlaced and banked monochrome video adaptors
 * Pascal Haible 8/93, 3/94, 4/94 haible@IZFM.Uni-Stuttgart.DE
 *
 * bdm2/driver/apollo/apolloBlt.c
 * Hamish Coleman 11/93 hamish@zot.apana.org.au
 */
/* $XConsortium: apolloBlt.c /main/3 1996/02/21 17:49:16 kaleb $ */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"gcstruct.h"
#include	"windowstr.h"
#include	"scrnintstr.h"
#include	"pixmapstr.h"
#include	"regionstr.h"

#include	"compiler.h"

#ifdef NOTYET
#include	"mfb.h"
#endif
#include	"mono.h"
#include	"apolloHW.h"
#include	<sys/types.h>
#ifndef MINIX
#include	<netinet/in.h>
#else
#include	<net/hton.h>
#endif


extern unsigned Apollo_IOBASE;

void
_ApolloDoBitblt(pdstBase, psrcBase, widthSrc, widthDst,
		X_Src, Y_Src, X_Dest, Y_Dest, X_Count, Y_Count,
		xdir, ydir, alu)
    unsigned char *pdstBase, *psrcBase;
    int X_Src, Y_Src, X_Dest, Y_Dest, X_Count, Y_Count;
    int xdir, ydir, alu;
{
  int Src_Addr;
  int Dest_Addr;
  int Src_Ptr;
  int Dest_Ptr;
  int Combined_Mask;
  int Y_Delta;
  int X_End;
  int X_Word_Count;
  int Start_Mask;
  int End_Mask;
  int Shift;
  int Preread;
  int YC;
  int Dummy;
  int XC;
  int Incr;
  
  ErrorF("_DoBitblt called\n");
  
  /* KLUDGE !!!! */
  if (xdir>0 && ydir>0) {
    Incr = 1;
  } else {
    Incr = -1;
  }
 
  outb(AP_PORT_CONTROL_3,AP_BIT_ROP|AP_ON);
  outw(AP_PORT_ROP,alu|alu<<4|alu<<8|alu<<12);
  /* switch (controller type)
     monochrome: */
  outb(AP_PORT_CONTROL_2,AP_DATA_PLN);
  
  Src_Addr = (int)psrcBase + Y_Src * Word_Width + X_Src/16;
  Dest_Addr = (int)pdstBase + Y_Dest * Word_Width + X_Dest/16;
  
  if (Incr > 0) {
    Y_Delta = Word_Width;
    X_End = X_Dest + X_Count - 1;
    X_Word_Count = X_End/16 - X_Dest/16 + 1;
    Start_Mask = 0xFFFF0000 << (X_Dest%16);
    End_Mask = 0x00007fff << (X_End%16);
    Combined_Mask = Start_Mask | End_Mask;
    Shift = ((X_Dest % 16) - (X_Src % 16)) % 16;
    if ((X_Dest % 16) < (X_Src % 16)) {
      Preread = 1;
    } else {
      Preread = 0;
    }
  } else {
    Y_Delta = - Word_Width;
    X_End = X_Dest - X_Count + 1;
    X_Word_Count = X_Dest/16 - X_End/16 + 1;
    Start_Mask = 0x00007fff << (X_Dest % 16);
    End_Mask = 0xffff0000 << (X_End % 16);
    Combined_Mask = Start_Mask | End_Mask;
    Shift = -((X_Src % 16) - (X_Dest % 16)) % 16;
    if ((X_Dest % 16) > (X_Src % 16)) {
      Preread = 1;
    } else {
      Preread = 0;
    }
  }
  
  outb(AP_PORT_CONTROL_0,AP_MODE_DBLT|Shift);
  
  for (YC=0;YC<Y_Count;YC++) {
    short tmp;
    Src_Ptr = Src_Addr;
    Dest_Ptr = Dest_Addr;
    
    if (Dest_Ptr < 65536) {
      outb(AP_PORT_CONTROL_3,AP_BIT_HIBIT|AP_OFF);
    } else {
      outb(AP_PORT_CONTROL_3,AP_BIT_HIBIT|AP_ON);
      Dest_Ptr -= 65536;
    }
    
    if (Preread = 1) {
      Dummy = *(short *)Src_Ptr;
      Src_Ptr += Incr;
    }
    
    if (X_Word_Count > 1) {
      tmp = htons(Start_Mask);
      outw(AP_PORT_WRITE_ENABLE,tmp);
      *(short *)Src_Ptr = htons((short)Dest_Ptr);
      Src_Ptr += Incr;
      Dest_Ptr += Incr;
      outw(AP_PORT_WRITE_ENABLE,0);
      
      for (XC=1;XC<X_Word_Count-1;XC++) {
        *(short *)Src_Ptr = htons((short)Dest_Ptr);
        Src_Ptr += Incr;
        Dest_Ptr += Incr;
      }
      
      tmp = htons(End_Mask);
      outw(AP_PORT_WRITE_ENABLE,tmp);
      *(short *)Src_Ptr = htons((short)Dest_Ptr);
    } else {
      tmp = htons(Combined_Mask);
      outw(AP_PORT_WRITE_ENABLE,tmp);
      *(short *)Src_Ptr = htons((short)Dest_Ptr);
    }
    
    Src_Addr += Y_Delta;
    Dest_Addr += Y_Delta;
  }
  
  outb(AP_PORT_CONTROL_0,AP_MODE_NORMAL);
  outb(AP_PORT_CONTROL_3,AP_BIT_ROP|AP_OFF);
}

int
ApolloDoBitblt(pSrc, pDst, alu, prgnDst, pptSrc)
    DrawablePtr	    pSrc, pDst;
    int		    alu;
    RegionPtr	    prgnDst;
    DDXPointPtr	    pptSrc;
{
  unsigned char *psrcBase, *pdstBase;	
  /* start of src and dst bitmaps */
  int widthSrc, widthDst;	/* add to get to same position in next line */
  BoxPtr pbox;
  int NoAccel = 0;
  int nbox;
  int readwrite=0;		/* DBG */
  
  
  BoxPtr pboxTmp, pboxNext, pboxBase, pboxNew1, pboxNew2;
                                  /* temporaries for shuffling rectangles */
  DDXPointPtr pptTmp, pptNew1, pptNew2;
                                  /* shuffling boxes entails shuffling the
                                     source points too */

  int xdir;			/* 1 = left right, -1 = right left/ */
  int ydir;			/* 1 = top down, -1 = bottom up */
  int careful;


  if (pSrc->type == DRAWABLE_WINDOW)
    {
      psrcBase = (unsigned char *)
	(((PixmapPtr)(pSrc->pScreen->devPrivate))->devPrivate.ptr);
      widthSrc = (int)((PixmapPtr)(pSrc->pScreen->devPrivate))->devKind;
    }
  else
    {
      psrcBase = (unsigned char *)(((PixmapPtr)pSrc)->devPrivate.ptr);
      widthSrc = (int)(((PixmapPtr)pSrc)->devKind);
    }
  
  if (pDst->type == DRAWABLE_WINDOW)
    {
      pdstBase = (unsigned char *)
	(((PixmapPtr)(pDst->pScreen->devPrivate))->devPrivate.ptr);
      widthDst = (int)
	((PixmapPtr)(pDst->pScreen->devPrivate))->devKind;
    }
  else
    {
      pdstBase = (unsigned char *)(((PixmapPtr)pDst)->devPrivate.ptr);
      widthDst = (int)(((PixmapPtr)pDst)->devKind);
    }


  /* XXX we have to err on the side of safety when both are windows,
   * because we don't know if IncludeInferiors is being used.
   */
  careful = ((pSrc == pDst) ||
	     ((pSrc->type == DRAWABLE_WINDOW) &&
	      (pDst->type == DRAWABLE_WINDOW)));
  
  pbox = REGION_RECTS(prgnDst);
  nbox = REGION_NUM_RECTS(prgnDst);
  
  if (careful && (pptSrc->y < pbox->y1)) NoAccel = 1;
  if (careful && (pptSrc->x < pbox->x1)) NoAccel = 1;
  
#ifdef NOTYET
  if (CHECKSCREEN(psrcBase))
       {
       if (CHECKSCREEN(pdstBase)) /* Screen -> Screen */
	    {
	    /* all ok */
	    }
       else			/* Screen -> Mem */
	    {
	    return(-1);
	    }
       }
  else 
       {
       if (CHECKSCREEN(pdstBase)) /* Mem -> Screen */
	    {
	    return(-1);
	    }
       else 			/* Mem -> Mem */
            {
            return(-1);
            }
       }
#endif

  ErrorF("DoBitblt used\n");
  
  pboxNew1 = NULL;
  pptNew1 = NULL;
  pboxNew2 = NULL;
  pptNew2 = NULL;
  if (careful && (pptSrc->y < pbox->y1))
    {
      /* walk source botttom to top */
      ydir = -1;
      widthSrc = -widthSrc;
      widthDst = -widthDst;
      
      if (nbox > 1)
	{
	  /* keep ordering in each band, reverse order of bands */
	  pboxNew1 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	  if(!pboxNew1)
	    return(0);
	  pptNew1 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	  if(!pptNew1)
	    {
	      DEALLOCATE_LOCAL(pboxNew1);
	      return(0);
	    }
	  pboxBase = pboxNext = pbox+nbox-1;
	  while (pboxBase >= pbox)
	    {
	      while ((pboxNext >= pbox) &&
		     (pboxBase->y1 == pboxNext->y1))
		pboxNext--;
	      pboxTmp = pboxNext+1;
	      pptTmp = pptSrc + (pboxTmp - pbox);
	      while (pboxTmp <= pboxBase)
	        {
		  *pboxNew1++ = *pboxTmp++;
		  *pptNew1++ = *pptTmp++;
	        }
	      pboxBase = pboxNext;
	    }
	  pboxNew1 -= nbox;
	  pbox = pboxNew1;
	  pptNew1 -= nbox;
	  pptSrc = pptNew1;
        }
    }
  else
    {
      /* walk source top to bottom */
      ydir = 1;
    }
  
  if (careful && (pptSrc->x < pbox->x1))
    {
      /* walk source right to left */
      xdir = -1;
      
      if (nbox > 1)
	{
	  /* reverse order of rects in each band */
	  pboxNew2 = (BoxPtr)ALLOCATE_LOCAL(sizeof(BoxRec) * nbox);
	  pptNew2 = (DDXPointPtr)ALLOCATE_LOCAL(sizeof(DDXPointRec) * nbox);
	  if(!pboxNew2 || !pptNew2)
	    {
	      if (pptNew2) DEALLOCATE_LOCAL(pptNew2);
	      if (pboxNew2) DEALLOCATE_LOCAL(pboxNew2);
	      if (pboxNew1)
		{
		  DEALLOCATE_LOCAL(pptNew1);
		  DEALLOCATE_LOCAL(pboxNew1);
		}
	      return(0);
	    }
	  pboxBase = pboxNext = pbox;
	  while (pboxBase < pbox+nbox)
	    {
	      while ((pboxNext < pbox+nbox) &&
		     (pboxNext->y1 == pboxBase->y1))
		pboxNext++;
	      pboxTmp = pboxNext;
	      pptTmp = pptSrc + (pboxTmp - pbox);
	      while (pboxTmp != pboxBase)
	        {
		  *pboxNew2++ = *--pboxTmp;
		  *pptNew2++ = *--pptTmp;
	        }
	      pboxBase = pboxNext;
	    }
	  pboxNew2 -= nbox;
	  pbox = pboxNew2;
	  pptNew2 -= nbox;
	  pptSrc = pptNew2;
	}
    }
  else
    {
      /* walk source left to right */
      xdir = 1;
    }
  
  while(nbox--) {
    _ApolloDoBitblt(pdstBase, psrcBase,widthSrc,widthDst,
	   pptSrc->x, pptSrc->y, pbox->x1, pbox->y1,
	   pbox->x2 - pbox->x1, pbox->y2 - pbox->y1,
	   xdir, ydir, alu);
    pbox++;
    pptSrc++;
  }
  
  /* free up stuff */
  if (pboxNew2)
    {
      DEALLOCATE_LOCAL(pptNew2);
      DEALLOCATE_LOCAL(pboxNew2);
    }
  if (pboxNew1)
    {
      DEALLOCATE_LOCAL(pptNew1);
      DEALLOCATE_LOCAL(pboxNew1);
    }
  return(0);
}
