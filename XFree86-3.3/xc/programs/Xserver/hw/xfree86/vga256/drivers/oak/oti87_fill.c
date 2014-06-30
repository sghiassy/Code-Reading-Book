/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/oak/oti87_fill.c,v 3.2 1996/12/23 06:57:58 dawes Exp $ */





/* $XConsortium: oti87_fill.c /main/2 1996/02/21 18:06:19 kaleb $ */

#include "cfbrrop.h"
#include "stdio.h"
#include "vga256.h"
#include "mergerop.h"
#include "vgaBank.h"
#include "xf86.h"
#include "vga.h"        /* For vgaBase. */
#include "compiler.h"
#include "oak_driver.h"
#include "xf86_Config.h"


/* medium level routines */

#if 0 /* C low level routine */
static void oti087ColorExpandFill(void *s, size_t width, size_t height)
{
  char * pixel;
  char * pixel2;
  register void * place;
  register int i;
  register int k;
  for (k=0 ; k < height ; k++)
    {
      place = s  ;

      for (i=0 ; i < width ; i++)
	{
	  pixel = place;
	  *pixel = 0xFF;
	  pixel2=place+1;
	  *pixel2 = 0xFF;
	  place = place + 8; 
	}
      s = s+ 1024;
    }
}

static void side_filling(void *s, size_t height)
{
  char * pixel;
  char * pixel2;
  int k;

  for (k=0 ; k < height ; k++)
    {
      pixel = s;
      *pixel = 0xFF;
      pixel2= pixel+1;
      *pixel2 = 0xFF;
      s = s + 1024;
    }
}
#endif




void 
OAKFillSolidUnbanked (int x,int  y, int w, int h, int color, int destpitch)
{
  int i,j,temp;
  unsigned char * vgabase;
  int x_l,x_r,w_f,fillbase;

  x_l = (x/8)*8;
  x_r = ((x+w)/8)*8; 

  SET_FG_COLOR(color);
  SET_BG_COLOR(0xFF);
  SET_PATTERN(0xFF);

  outb (0x3DE, 0x30);
  temp = inb (0x3DF);        /* saving the contents of the expander */
  SET_COLOR_EXPANSION(0x1D);
  
  if ( x_l != x)
    {
      fillbase = (int) vgaBase + x_l + 8 + (y * destpitch);
      w_f = x_r - x_l - 8 ; 

      SET_MASK(leftmask[(7 - (x-x_l))]); 
      oti087ColorMaskedFill( (void *) (fillbase - 8),h); 

    }
  else  /* left side clipped */
    {
      fillbase = (int) vgaBase + x_l + (y * destpitch);
      w_f = x_r - x_l; 
    }

  /* big unmasked chunk */

  if (w_f > 0)
    {
      SET_MASK(0xFF);
      oti087ColorExpandFill ( (void *) fillbase, w_f/8, h);  
    }

  if ( x+w != x_r) 
    {
      SET_MASK(rightmask[(x+w-1 - x_r)]);
      oti087ColorExpandFill( (void *) ( fillbase + w_f),1, h);
    }

  SET_COLOR_EXPANSION(temp);    /* return to normal value */
}


void
OAKColorExpandSolidFill ( int x, int y, int w, int h, int color, int destpitch)
{
  int coordinate;
  int boundary_line;

  coordinate = (y * destpitch) + x;
  coordinate = coordinate / 65536;       /* here we have the bank number */

  for (;;)
    {
      boundary_line  = (coordinate +1) * 64;
      if ((y+h-1) < boundary_line)   /* no need for splitting */
        {
          outb (0x3DE, 0x24);  
          outb (0x3DF, coordinate);
          OAKFillSolidUnbanked ( x, (y - (coordinate*64)), w, h, color, 
				destpitch);
          outb (0x3DE, 0x24);  
          outb (0x3DF, 0);
          return;
        }
      else
        {
            outb (0x3DE, 0x24);  
            outb (0x3DF, coordinate);
            OAKFillSolidUnbanked ( x, 
                                  (y - (coordinate * 64)), 
                                  w, 
                                (boundary_line-y), 
                                  color,
				  destpitch);
            h = h - (boundary_line - y);
            coordinate = coordinate + 1;
            y = boundary_line;
        }
     } 
}


/* High level routines */


void
OAKFillRectSolidCopy (pDrawable, pGC, nBox, pBox)
    DrawablePtr	    pDrawable;
    GCPtr	    pGC;
    int		    nBox;
    BoxPtr	    pBox;
{
  unsigned long rrop_xor,rrop_and;
  RROP_FETCH_GC(pGC);

  OAKFillBoxSolid (pDrawable, nBox, pBox, rrop_xor, 0, pGC->alu);
}


void
OAKFillBoxSolid (pDrawable, nBox, pBox, pixel1, pixel2, alu)
    DrawablePtr	    pDrawable;
    int		    nBox;
    BoxPtr	    pBox;
    unsigned long   pixel1;
    unsigned long   pixel2;
    int	            alu;
{
  unsigned char   *pdstBase;
  Bool            flag;
  int		  widthDst;
  int             h;
  int		  w;

  if (pDrawable->type == DRAWABLE_WINDOW)
    {
      pdstBase = (unsigned char *)
	(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devPrivate.ptr);
      widthDst = (int)
	(((PixmapPtr)(pDrawable->pScreen->devPrivate))->devKind);
    }
  else
    {
      pdstBase = (unsigned char *)(((PixmapPtr)pDrawable)->devPrivate.ptr);
      widthDst = (int)(((PixmapPtr)pDrawable)->devKind);
    }

  flag = CHECKSCREEN(pdstBase);	/* On screen? */

  if (flag)
    {

      for (; nBox; nBox--, pBox++)
	{
	  unsigned int dstAddr;
	      
	  h = pBox->y2 - pBox->y1;
	  w = pBox->x2 - pBox->x1;
	  
          dstAddr = pBox->y1 * widthDst + pBox->x1;

          if (alu == GXcopy) 
	    {
      
	      if (w < 17)
		{
		  vga256FillBoxSolid(pDrawable, 1, pBox, pixel1,
				     pixel2, alu);
		}
	      else
		{
		  OAKColorExpandSolidFill(pBox->x1, pBox->y1, w, h, pixel1,
					  widthDst);
		}
	      
	      continue;
	    }

          else 
	    {	/* alu != GXcopy */
	      
	      vga256FillBoxSolid(pDrawable, 1, pBox, pixel1,
				 pixel2, alu);
	      continue;
	    }
	} /* for all boxes */
    }
  /* Shouldn't happen. */
  else vga256FillBoxSolid(pDrawable, nBox, pBox, pixel1, pixel2, alu);

}
