/*
 * $XConsortium: skyCur.c,v 1.4 91/12/11 21:23:51 eswu Exp $
 *
 * Copyright IBM Corporation 1987,1988,1989,1990,1991
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/

/*
 * skyCur.c - support for software cursor using miDCInitialize
 *              skyQueryBestSize copied from ibm/pgc/pgcGlobal.c
 *	Also, support for hardware cursor of the skyway adapter.
 */

#include <sys/types.h>
#include "misc.h"
#include "X.h"
#define NEED_EVENTS
#include "Xproto.h"
#include "scrnintstr.h"
#include "pixmap.h"
#include "input.h"
#include "cursorstr.h"
#include "mipointer.h"
#include "misprite.h"

#include "ibmScreen.h"
#include "skyHdwr.h"
#include "ibmTrace.h"

extern int lastEventTime ;
static long skyEventTime();
static Bool skyCursorOffScreen();
static void skyCrossScreen();
extern void miPointerQueueEvent();

void skyRecolorCursor();

miPointerScreenFuncRec skyPointerScreenFuncs = {
    skyCursorOffScreen,
    skyCrossScreen,
    miPointerWarpCursor,
};

static Bool
skyCursorOffScreen (pScreen, x, y)
    ScreenPtr   *pScreen;
    int         *x, *y;
{
    TRACE (("skyCursorOffScreen\n"));
    return FALSE ;
}

static long
skyEventTime (pScreen)
    ScreenPtr   pScreen;
{
    TRACE (("skyEventTime\n"));
    TRACE (("skyEventTime (time=0x%x)\n", lastEventTime));
    return lastEventTime;
}


static void
skyCrossScreen (pScreen, entering)
    ScreenPtr   pScreen;
    Bool        entering;
{
    u_char  select;

    TRACE(("skyCrossScreen( pScreen=0x%x, entering=%d )\n", pScreen, entering));
    select = 1;
    if (entering)
	select = 0;
  /*
    if (sunFbs[pScreen->myNum].EnterLeave)
	(*sunFbs[pScreen->myNum].EnterLeave) (pScreen, select);
   */
}

void
skyQueryBestSize( class, pwidth, pheight )
int class;
short *pwidth;
short *pheight;
{
if ( class == CursorShape )
  *pwidth = *pheight = 32 ;
else /* either TileShape or StippleShape */
  /* round width to nearest multiple of 8, don't care about height */
  *pwidth = ( *pwidth + 0x7 ) & ~ 0x7;
return;
}

/* following procedures used for skyway hardware cursor */
Bool
skyRealizeCursor( pScr, pCurs)
    ScreenPtr	pScr;
    CursorPtr	pCurs;	/* a SERVER-DEPENDENT cursor */
{
    TRACE(("skyRealizeCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));

    /* could allocate devPriv, and copy source & mask into a form 
       matching that wanted by skyway hdwr.  Now just use the source &
       mask as found in pCurs. Modify when moved to adapter in
       DisplayCursor(). */
    pCurs->devPriv[ pScr->myNum ]= (pointer)NULL;

    /* if no current cursor, make first realized cursor current */
    if (!ibmCurrentCursor(pScr->myNum)) {
	ibmCurrentCursor(pScr->myNum)= pCurs;
        ibmCursorHotX( pScr->myNum ) = pCurs->bits->xhot  ;
        ibmCursorHotY( pScr->myNum ) = pCurs->bits->yhot  ;
    }
    return TRUE;
}


Bool
skyUnrealizeCursor( pScr, pCurs)
    ScreenPtr 	pScr;
    CursorPtr 	pCurs;
{

    TRACE(("skyUnrealizeCursor( pScr= 0x%x, pCurs= 0x%x )\n",pScr,pCurs));
    return TRUE;
}


	/*
	 *  Display (and track) the cursor described by "pCurs"
	 *  Copies the cursor image into the hardware active cursor
	 *  area.
	 *
	 *  After copying the cursor image, adjust ibmCursorHotX and 
	 *  ibmCursorHotY so that the cursor is displayed with it's
	 *  hot spot at the coordinates of mouse motion events.
	 */

Bool
skyDisplayCursor( pScr, pCurs )
    ScreenPtr	pScr ;
    CursorPtr	pCurs ;
{
    unsigned char  *src, *mask;
    int   width,height,i,j;
    int index = pScr->myNum ;
    unsigned char src_byte,mask_byte;

    TRACE( ( "skyDisplayCursor( pScr =0x%x, pCurs =0x%x )\n",
	      pScr, pCurs)) ;

    if ( !(ibmScreenState( index )==SCREEN_ACTIVE ) )
       return 0;

    /* disable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_disable;

    /* load cursor starting at 0,0 */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursHi_Plane0;

    /* ASSUME cursor source & mask padded to 32 bit boundary.  May have to
       mask off pad bits, if not zero already */
    width = ((pCurs->bits->width + 31)/32)*4; /* width in bytes of pixmap */
    height = pCurs->bits->height;
    src = pCurs->bits->source;
    mask = pCurs->bits->mask;

    /* move source & mask to skyway.  skyway cursor is 64x64 */
    for ( i=0; i<64; i++ )
    {
      for ( j=0; j<8; j++ )
      {
	if ( i<height && j<width )
	{
              src_byte = *src++;
              mask_byte = *mask++ & ~src_byte;

	      SKY_SINDEX_REG(index) = CursImg_Plane0 | mask_byte;
	      SKY_SINDEX_REG(index) = CursImg_Plane1 | src_byte;
	 }
	else {
	      SKY_SINDEX_REG(index) = CursImg_Plane0; /* 0 in data byte */
	      SKY_SINDEX_REG(index) = CursImg_Plane1;
	    }
      }
    }

    /* enable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_enable;

    /* try recoloring the cursor */
    skyRecolorCursor( pScr, pCurs, TRUE );

    /* set fields in ibmperScreenInfo, used elsewhere */
    ibmCurrentCursor( index ) = pCurs     ;
    ibmCursorHotX( index ) = pCurs->bits->xhot  ;
    ibmCursorHotY( index ) = pCurs->bits->yhot  ;

    return TRUE ;
}


void
skyMoveCursor( x, y )
unsigned short x,y;
{
int  index  =  ibmCurrentScreen;

    TRACE(("skyMoveCursor( x=%d, y=%d )\n",x,y));

    /* addjust x,y for skyway pass2 correction factor & hotspot offset */
    x = x + xc_correct2 - ibmCursorHotX(index);
    y = y + yc_correct2 - ibmCursorHotY(index);

    /* disable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_disable;

    /* specify low order byte of x */
    SKY_SINDEX_REG(index) = CursLo_Plane0 | 0x0001;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | (x & 0x00FF);
    SKY_SINDEX_REG(index) = CursCntl_Plane0 |  x >> 8;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | (y & 0x00FF);
    SKY_SINDEX_REG(index) = CursCntl_Plane0 |  y >> 8;

    /* enable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_enable;

    return;
}


void
skyRecolorCursor (pScr, pCurs, visible)
    register ScreenPtr pScr;
    register CursorPtr pCurs;
    register int visible;
{
  int index = pScr->myNum;

  volatile unsigned char *rptr;       /* pointer to control index reg*/
  volatile unsigned char *dptr;       /* pointer to palette datab reg*/
  volatile unsigned short *io_idata;  /* pointer to control index reg*/

  TRACE(("skyRecolorCursor(pScr = 0x%x, pCurs = 0x%x, visible = %d)\n",
  pScr, pCurs, visible));

  TRACE (("\tforeground=(%d,%d,%d) background=(%d,%d,%d)\n",
          pCurs->foreRed >> 8, pCurs->foreGreen >> 8, pCurs->foreBlue >> 8, 
          pCurs->backRed >> 8, pCurs->backGreen >> 8, pCurs->backBlue >> 8));

  /* do nothing when not visible...should maybe check that pCurs is
     ibmCurrentCursor for the skyway screen ?? */
  if ( ! visible ) return;

  /* disable the cursor in the DAC */
  SKY_SINDEX_REG(index) = CursLo_Plane0;
  SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_disable;

  /* Initialize the pointers to the index and data_b registers.        */
  io_idata = (unsigned short *) &IOREG[index]->index;
  rptr = (uchar *) &IOREG[index]->index;
  dptr = (uchar *) &IOREG[index]->data_b;

  /* Load the colormap onto the RGB hardware:			       */
  /*	 io_idata  defines the cursor colormap hardware address. The   */
  /*		   caller should start at 1 since 0 is transparency.   */
  /*	 rptr	   gives the command that colors are being written     */
  /*		   in RGB values.				       */
  /*	 dptr	   writes the color value to the hardware	       */

    *io_idata = PaletIndex | 0x0001;   	 /* Starting palette index     */
    *rptr = ColorCmd;
    *dptr = pCurs->backRed >> 8;
    *dptr = pCurs->backGreen >> 8;
    *dptr = pCurs->backBlue >> 8;
    *rptr = ColorCmd;
    *dptr = pCurs->foreRed >> 8;
    *dptr = pCurs->foreGreen >> 8;
    *dptr = pCurs->foreBlue >> 8;

    /* enable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_enable;

  return;

}


void
skyRemoveCursor( pScr, pCurs )
    ScreenPtr	pScr ;
    CursorPtr	pCurs ;
{
    int  i;
    int index = pScr->myNum ;

    TRACE( ( "skyRemoveCursor( pScr =0x%x, pCurs =0x%x )\n",
	      pScr, pCurs)) ;

    if ( !(ibmScreenState( index )==SCREEN_ACTIVE ) )
       return;

    /* disable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_disable;

    /* load cursor starting at 0,0 */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursHi_Plane0;

    /* move source & mask to skyway.  skyway cursor is 64x64 */
    for ( i=0; i<64*8; i++ )
    {
      SKY_SINDEX_REG(index) = CursImg_Plane0; /* 0 in data byte */
      SKY_SINDEX_REG(index) = CursImg_Plane1;
    }

    /* enable the cursor in the DAC */
    SKY_SINDEX_REG(index) = CursLo_Plane0;
    SKY_SINDEX_REG(index) = CursCntl_Plane0 | DAC_enable;

    return;
}
