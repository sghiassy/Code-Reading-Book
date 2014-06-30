/* $XFree86: xc/programs/Xserver/hw/xfree86/vga256/drivers/cirrus/cir_cursor.c,v 3.20.2.4 1997/05/31 13:34:43 dawes Exp $ */
/*
 *
 * Copyright 1993-94 by Simon P. Cooper, New Brunswick, New Jersey, USA.
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Simon P. Cooper not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  Simon P. Cooper makes no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * SIMON P. COOPER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL SIMON P. COOPER BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Simon P. Cooper, <scooper@vizlab.rutgers.edu>
 */
/* $XConsortium: cir_cursor.c /main/14 1996/10/25 14:12:25 kaleb $ */

/*
#define CIRRUS_DEBUG_CURSOR
*/

#define USE_MBITBLT    1

#include "X.h"
#include "Xproto.h"
#include "misc.h"
#include "input.h"
#include "cursorstr.h"
#include "regionstr.h"
#include "scrnintstr.h"
#include "servermd.h"
#include "windowstr.h"
#include "xf86.h"
#include "mipointer.h"
#include "xf86Priv.h"
#include "xf86_Option.h"
#include "xf86_OSlib.h"
#include "vga.h"
#include "cir_driver.h"
#include "cirBlitter.h"

static Bool cirrusRealizeCursor();
static Bool cirrusUnrealizeCursor();
static void cirrusSetCursor();
static void cirrusMoveCursor();
static void cirrusRecolorCursor();

static miPointerSpriteFuncRec cirrusPointerSpriteFuncs =
{
   cirrusRealizeCursor,
   cirrusUnrealizeCursor,
   cirrusSetCursor,
   cirrusMoveCursor,
};

extern miPointerScreenFuncRec xf86PointerScreenFuncs;
extern xf86InfoRec xf86Info;

static int cirrusCursGeneration = -1;

cirrusCurRec cirrusCur;

Bool
cirrusCursorInit(pm, pScr)
     char *pm;
     ScreenPtr pScr;
{
  if (cirrusCursGeneration != serverGeneration)
    {
      if (!(miPointerInitialize(pScr, &cirrusPointerSpriteFuncs,
				&xf86PointerScreenFuncs, FALSE)))
	return FALSE;

      cirrusCur.hotX = 0;
      cirrusCur.hotY = 0;
      pScr->RecolorCursor = cirrusRecolorCursor;
      cirrusCursGeneration = serverGeneration;
    }

  return TRUE;
}

void
cirrusShowCursor()
{
  /* turn the cursor on */
  if (HAVE546X()) {
    volatile unsigned short *pS = (unsigned short *)(cirrusMMIOBase + 0xE6);
    volatile unsigned short *pCursorAddr = 
      (unsigned short *)(cirrusMMIOBase + 0xE8);
    unsigned long curAddr;

    cirrusFindLastTile(NULL, NULL, &curAddr, NULL);

    curAddr >>= 8;           /* Keep only bits 22:10 */
    *pCursorAddr = curAddr & 0x7FFC;

    *pS |= 0x0001;
  } else {
    outw (0x3C4, ((cirrusCur.cur_size<<2)+1)<<8 | 0x12);
  }
}

void
cirrusHideCursor()
{
  /* turn the cursor off */
  if (HAVE546X()) {
    /* To hide the cursor, we kick it off into the corner, and then set the
       cursor image to be a transparent bitmap.  That way, if X continues
       to move the cursor while it is hidden, there is no way that the user
       can move the cursor back on screen!

       We don't just clear the cursor enable bit because doesn't work in some
       cases (like when switching back to text mode).
       */
    volatile unsigned short *pY = (unsigned short *)(cirrusMMIOBase + 0xE2);
    volatile unsigned short *pX = (unsigned short *)(cirrusMMIOBase + 0xE0);
    volatile unsigned short *pCursorAddr = 
      (unsigned short *)(cirrusMMIOBase + 0xE8);
    unsigned long curAddr;

    cirrusFindLastTile(NULL, NULL, NULL, &curAddr);

    curAddr >>= 8;           /* Keep only bits 22:10 */
    *pCursorAddr = curAddr & 0x7FFC;

    *pX = 0xFFFF;
    *pY = 0xFFFF;

  } else {
    outw (0x3C4, (0x0)<<8 | 0x12);
  }
}

static Bool
cirrusRealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;

{
   register int i, j, b;
   unsigned short *pServMsk;
   unsigned short *pServSrc;
   int   index = pScr->myNum;
   pointer *pPriv = &pCurs->bits->devPriv[index];
   int	 wsrc, wdst, h, off;
   unsigned short *ram, *curp;
   CursorBitsPtr bits = pCurs->bits;

   if (pCurs->bits->refcnt > 1)
      return TRUE;

   ram = (unsigned short *)xalloc(1024);

   /* Clear out enough memory for the entire cursor.  There are 
      width*height*2 bits (plane 0, plan 1), and thus width*height*2/8
      or width*height>>2 bytes */
   memset (ram, 0, (cirrusCur.width>>2)*cirrusCur.height);

   curp = (unsigned short *)ram;
   if (HAVE546X()) {
     off = 4;
   } else {
     off  = 64;             /* Offset (in shorts) from plane 0 to plane 1 */
   }

   *pPriv = (pointer) ram;

   if (!ram)
      return FALSE;

   pServSrc = (unsigned short *)bits->source;
   pServMsk = (unsigned short *)bits->mask;

   h = bits->height;
   if (h > cirrusCur.height)          /* Truncate cursor if it's too large */
      h = cirrusCur.height;

   wsrc = ((bits->width + 0x1f) >> 4) & ~0x1;	/* 16-bit words per line */
   wdst = (cirrusCur.width >> 4);               /* #wds in actual cursor */

#ifdef CIRRUS_DEBUG_CURSOR
   fprintf(stderr, "pServSrc\n");
   for (i = 0; i < h ; i++) {
     for (j = 0; j < wsrc; j++) {
       for (b = 0; b < 16; b++)
	 fprintf(stderr, "%c", pServSrc[i*wsrc + j]&(1<<b)?'1':'0');
     }
     fprintf(stderr, "\n");
   }
   fprintf(stderr, "pServMsk\n");
   for (i = 0; i < h ; i++) {
     for (j = 0; j < wsrc; j++) {
       for (b = 0; b < 16; b++)
	 fprintf(stderr, "%c", pServMsk[i*wsrc + j]&(1<<b)?'1':'0');
     }
     fprintf(stderr, "\n");
   }
#endif


   if (HAVE546X()) {
     /* The 546x's hardware cursor is a little different that the 
	traditional CL 32x32 cursor.  For one thing, the Laguna cursor
	is 64x64.  The other major difference is that the cursor bitmask
	planes are interleaved.  That is, the first 8 bytes of cursor
	memory are Plane 0, scanline 0, the next 8 bytes are Plane _1_, 
	scanline 0, etc.  This is opposed to the 2x/3x/4x 32x32 cursors, 
	which are noninterleaved (Plane 0, scanline 0, Plane 0, scanline 1,
	..., Plane 0, scanline 31, Plane 1, scanline 0, ...).
	*/

     for (i = 0; i < cirrusCur.height; i++) {  /* Copy all of the scanlines */
       for (j = 0; j < wdst; j++) {           /* Copy all of the columns */
	 unsigned short m, s;

	 if (i < h && j < wsrc) {     /* This is a valid word in the cursor */
	   m = *pServMsk++;
	   s = *pServSrc++;

	   /*
	      There are two bitmaps for the X cursor:  the Source and
	      the Mask.  The following table decodes these bits:

	      Src  Mask  |  Cursor color  |  Plane 0  Plane 1
	      -----------+----------------+------------------
	      0    0    |   Transparent  |     0        0
	      0    1    |     Color 0    |     0        1
	      1    0    |   Transparent  |     0        0
	      1    1    |     Color 1    |     1        1
		    
	      Thus, the data for Plane 0 bits is Src AND Mask, and 
	      the data for Plane 1 bits is Mask.
	      */
		  
	   *curp = s & m;        /* Plane 0 */
	   *(curp+off) = m;      /* Plane 1 */
	 } else {
	   /* Pad to fill out cursor image */
	   *curp       = 0x0000;
	   *(curp+off) = 0x0000;
	 }

	 /* Proceed to next word */
	 curp++;
       }  /* for j loop */
       
       /* Jump ahead to next Plane 0 scanline */
       curp+=off;
     } /* for i loop */

   } else {
     for (i = 0; i < h; i++)                /* Copy all of the scanlines */
       {
	 for (j = 0; j < wdst; j++)           /* Copy all of the columns */
	   {
	     unsigned short m, s;

	     if (i < h && j < wsrc)
	       {
		 m = *pServMsk++;
		 s = *pServSrc++;

		 ((char *)&m)[0] = byte_reversed[((unsigned char *)&m)[0]];
		 ((char *)&m)[1] = byte_reversed[((unsigned char *)&m)[1]];

		 ((char *)&s)[0] = byte_reversed[((unsigned char *)&s)[0]];
		 ((char *)&s)[1] = byte_reversed[((unsigned char *)&s)[1]];
		  
		 *curp	   = s & m;        /* Plane 0 */
		 *(curp+off) = m;            /* Plane 1 */
	       }
	     else
	       {
		 /* Pad to fill out cursor image */
		 *curp       = 0x0;
		 *(curp+off) = 0x0;
	       }
	     curp++;
	   }
       }
   }

   return TRUE;
}

static Bool
cirrusUnrealizeCursor(pScr, pCurs)
     ScreenPtr pScr;
     CursorPtr pCurs;
{
   pointer priv;

   if (pCurs->bits->refcnt <= 1 &&
       (priv = pCurs->bits->devPriv[pScr->myNum]))
     {
       xfree(priv);
       pCurs->bits->devPriv[pScr->myNum] = 0x0;
     }
   return TRUE;
}


/*
   cirrusFindLastTile() find the first tile in the last line of the frame
   buffer.  This function works only with the Laguna family of Cirrus chips,
   which use Rambus memory.
   */
cirrusFindLastTile(int *x, int *y, long unsigned *curAddr, 
		   long unsigned *hiddenAddr)
{
  int videoRam = vga256InfoRec.videoRam; /* in K */
  int tileHeight = 
    cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width?8:16;
  int tileWidth = 
    cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width?256:128;
  int tilesPerLine = 
    cirrusTilesPerLineTab[cirrusTilesPerLineIndex].tilesPerLine;
  int filledOutTileLines, leftoverMem;
  int yTile, xTile;
  int tileNumber;
    
  filledOutTileLines = videoRam / (tilesPerLine * 2); /* tiles are 2K */
  leftoverMem = videoRam - filledOutTileLines*tilesPerLine*2;
    
  if (leftoverMem > 0) {
    yTile = filledOutTileLines;
  } else {
    /* There is no incomplete row of tiles.  Then just use the last
       tile in the last line */
    yTile = filledOutTileLines - 1;
  }
  xTile = 0;   /* Always use the first tile in the determined tile row */

  if (x)
    *x = xTile * tileWidth;
  if (y)
    *y = yTile * tileHeight;

  if (curAddr) {
    if (cirrusChip == CLGD5465) {
      /* The Where's The Cursor formula changed for the 5465.  It's really 
 	 kinda wierd now. */
      unsigned long page, bank;
      unsigned int nX, nY;
      unsigned int nIL;  /* Interleaving */
      
      nX = xTile * tileWidth;
      nY = yTile * tileHeight;
      nIL = cirrusMemoryInterleave==0x00 ? 1 : 
      (cirrusMemoryInterleave==0x40 ? 2 : 4);
      
      page = (nY / (tileHeight * nIL)) * tilesPerLine + nX / tileWidth;
      bank = (nX/tileWidth + nY/tileHeight) % nIL + page/(512*nIL);
      page = page & 0x1FF;
      *curAddr = bank*1024*1024L + page*2048 + (nY%tileHeight)*tileWidth;
      
    } else {
#if 1
      switch (cirrusMemoryInterleave) {
      case 0x00: /* no interleaving */
	*curAddr = (yTile * tilesPerLine) * 2048;
	break;
      
      case 0x40: /* 2-way interleaving */
	tileNumber = (tilesPerLine*2) * (yTile/2) + yTile % 2;
	*curAddr = tileNumber * 2048;
	break;

      case 0x80: /* 4-way interleaving */
	tileNumber = (tilesPerLine*4) * (yTile/4) + yTile % 4;
	*curAddr = tileNumber * 2048;
	break;
      }
#else
       unsigned nIL;  /* Interleaving */
       nIL = cirrusMemoryInterleave==0x00? 1 : 
       (cirrusMemoryInterleave==0x40 ? 2 : 4);
 
       tileNumber = (tilesPerLine*nIL) * (yTile/nIL) + yTile % nIL;
       *curAddr = tileNumber * 2048;
 #endif
    }
  }

  if (hiddenAddr) {
    xTile++;

    if (cirrusChip == CLGD5465) {
      /* The Where's The Cursor formula changed for the 5465.  It's really 
 	 kinda wierd now. */
      unsigned long page, bank;
      unsigned nX, nY;
      unsigned nIL;  /* Interleaving */
 
      nX = xTile * tileWidth;
      nY = yTile * tileHeight;
      nIL = cirrusMemoryInterleave==0x00? 1 : 
      (cirrusMemoryInterleave==0x40 ? 2 : 4);
 
      page = (nY / (tileHeight * nIL)) * tilesPerLine + nX / tileWidth;
      bank = (nX/tileWidth + nY/tileHeight) % nIL + page/(512*nIL);
      page = page & 0x1FF;
      *hiddenAddr = bank*1024*1024L + page*2048 + (nY%tileHeight)*tileWidth;
    } else {
#if 1
      switch (cirrusMemoryInterleave) {
      case 0x00: /* no interleaving */
	*hiddenAddr = (yTile * tilesPerLine + xTile) * 2048;
	break;
	
      case 0x40: /* 2-way interleaving */
	tileNumber = (tilesPerLine*2) * (yTile/2) + (xTile*2) +
	  (yTile+xTile) % 2;
	*hiddenAddr = tileNumber * 2048;
	break;
	
      case 0x80: /* 4-way interleaving */
	tileNumber = (tilesPerLine*4) * (yTile/4) + (xTile*4) + 
	  (yTile+xTile) % 4;
	*hiddenAddr = tileNumber * 2048;
	break;
      }
#else
      unsigned nIL;  /* Interleaving */
      nIL = cirrusMemoryInterleave==0x00? 1 : 
      (cirrusMemoryInterleave==0x40 ? 2 : 4);
  
      tileNumber = (tilesPerLine*nIL) * (yTile/nIL) + xTile*nIL +
 	(yTile+xTile) % nIL;
      *hiddenAddr = tileNumber * 2048;
#endif
    }
  }
  
}





cirrusLoadCursorToCard (pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
  int   index = pScr->myNum;
  volatile int   l, w, winw, count, rshift, lshift;
  unsigned long *pDstM, *pSrcM;
  unsigned long dAddr;
  int b;

  if (!xf86VTSema)
      return;

  /* check for blitter operation: must not meddle with ram when blitter is
     running...
   */

  if (cirrusBLTisBusy)
    WAITUNTILFINISHED ();
      
  /* Calculate the number of words to transfer to the board */
  count = ((cirrusCur.cur_size == 0) ? 256 : 1024) >> 2;

  if (1) {
    /* Onboard address to start writing the cursor backwards */
    dAddr = cirrusCur.cur_addr;
    
    CIRRUSSETSINGLE(dAddr);

    pDstM = (unsigned long *)(CIRRUSSINGLEBASE() + dAddr);
    pDstM += count - 1;
  }

  pSrcM = (unsigned long *)pCurs->bits->devPriv[index];
  pSrcM += count - 1;

#ifdef CIRRUS_DEBUG_CURSOR
  fprintf(stderr, "LoadCursorToCard(x = %d  y = %d)\n", x, y);
#endif

  if (x > 0) x = 0;
  if (y > 0) y = 0;


  if (HAVE546X()) {
    int xStorage, yStorage;
    int cursorWidth, cursorHeight;

    volatile unsigned long *pOP1_opRDRAM;
#if USE_MBITBLT
    volatile unsigned long *pMBLTEXT_EX, *pOP0_opMRDRAM;
#else
    volatile unsigned long *pBLTEXT_EX, *pOP0_opRDRAM;
#endif
    volatile unsigned long *pHOSTDATA;
    volatile unsigned short *pDRAWDEF, *pBLTDEF;
    volatile unsigned char *pQFREE;

    /* Point all the MMIO registers to their appropriate addresses */
    pOP1_opRDRAM = (unsigned long *)(cirrusMMIOBase + 0x540);
    pDRAWDEF = (unsigned short *)(cirrusMMIOBase + 0x584);
    pBLTDEF = (unsigned short *)(cirrusMMIOBase + 0x586);
#if USE_MBITBLT
    pOP0_opMRDRAM = (unsigned long *)(cirrusMMIOBase + 0x524);
    pMBLTEXT_EX = (unsigned long *)(cirrusMMIOBase + 0x720);
#else
    pOP0_opRDRAM = (unsigned long *)(cirrusMMIOBase + 0x520);
    pBLTEXT_EX = (unsigned long *)(cirrusMMIOBase + 0x700);
#endif
    pQFREE = (unsigned char *)(cirrusMMIOBase + 0x404);
    pHOSTDATA = (unsigned long *)(cirrusMMIOBase + 0x800);

    /* Wait until there's ample room in the chip's queue */
    while (*pQFREE < 10)
      ;

    *pBLTDEF = 0x1120;         /* Host-to-screen blit */
    *pDRAWDEF = 0x00CC;        /* Source copy */

    /* Find where we'll be saving the cursor's image. */
    cirrusFindLastTile(&xStorage, &yStorage, NULL, NULL);

    /* The cursor is designed to fit exactly into one tile. */
    cursorWidth = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width?256:128;
    cursorHeight = cirrusTilesPerLineTab[cirrusTilesPerLineIndex].width?4:8;

#if !USE_MBITBLT
    if (vgaBitsPerPixel == 16)
      cursorWidth >>= 1;
    if (vgaBitsPerPixel == 32)
      cursorWidth >>= 2;
#endif

    /* First, copy our transparent cursor image to the next 1/2 tile boundry */
    /* Destination */
#if USE_MBITBLT
    *pOP0_opMRDRAM = (yStorage << 16) | (xStorage+cursorWidth);
#else
    *pOP0_opRDRAM = (yStorage << 16) | (xStorage+cursorWidth);
#endif

    /* Set the source pitch.  0 means that, worst case, the source is 
       alligned only on a byte boundry */
    *pOP1_opRDRAM = 0;

    
#if USE_MBITBLT
    *pMBLTEXT_EX = (cursorHeight << 16) | cursorWidth;
#else
    *pBLTEXT_EX = (cursorHeight << 16) | cursorWidth;
#endif

    for (l = 0; l < cirrusCur.height; l++)
      for (w = 0; w < cirrusCur.width >> 4; w++) 
	*pHOSTDATA = 0x00000000;

    /* Now, copy the real cursor image */
    
    /* Set the destination */
#if USE_MBITBLT
    *pOP0_opMRDRAM = (yStorage << 16) | (xStorage);
#else
    *pOP0_opRDRAM = (yStorage << 16) | (xStorage);
#endif

    /* Set the source pitch.  0 means that, worst case, the source is 
       alligned only on a byte boundry */
    *pOP1_opRDRAM = 0;


    /* Always copy an entire cursor image to the card. */
#if USE_MBITBLT
    *pMBLTEXT_EX = (cursorHeight << 16) | (cursorWidth);
#else
    *pBLTEXT_EX = (cursorHeight << 16) | (cursorWidth);
#endif

    if (x == 0 && y == 0) {
      pSrcM -= count - 1;
      for (l = 0 ; l < 256; l++)
	*pHOSTDATA = *pSrcM++;
    } else {  /* !(x == 0 && y == 0) */

      unsigned long *pDstS = pDstM - 32;
      unsigned long *pSrcS = pSrcM - 32;
      int t = -y;
      int fillerDwords = 0;
      
      winw = cirrusCur.width >> 5;
      
      if (x == 0) {  
	/* We've run up against the top border, but we're not on the left
	   hand border.  */

	pSrcM -= count - 1;
	
	for (l = 0; l < cirrusCur.height; l++) {
	  for (w = 0; w < cirrusCur.width >> 4; w++)
	    if (l < t) {
	      pSrcM++;           /* Skip over this part of the cursor */
	      fillerDwords++;
	    }
	    else
	      *pHOSTDATA = *pSrcM++;
	}

	while(fillerDwords--)
	  *pHOSTDATA = 0x00000000;

      } else {  /* !(x == 0) */
	/* We've bumped into the left hand border, and maybe even the
	   top border as well.  */

	/* both planes */

#if 1
	/* Max cursor width is 64 */
	unsigned long curScanline[64 >> 4];  
#else
	/* This isn't legal ANSI C */
	unsigned long curScanline[cirrusCur.width >> 4];  
#endif
	int fillerDwords = 0;
	int shift;
	int b;

	pSrcM -= count - 1;
	
	for (l = 0; l < cirrusCur.height; l++) {        /* All scanlines */

	  /* If we're on the top border, skip these scanlines */
	  if (y < 0 && l < -y) {
	    for (w = 0; w < cirrusCur.width >> 4; w++) {
	      pSrcM++;
	      fillerDwords++;
	    }

	    continue;
	  }

	  /* First, copy the cursor image into a temp. scanline */
	  for (w = 0; w < cirrusCur.width >> 4; w++)
	    curScanline[w] = *pSrcM++;
	    
	  /* Shift the scanline right -x bits */
	  for (shift = 0; shift < -x; shift++) {
	    for (w = (cirrusCur.width >> 5) -1; w >= 0; w--) {
	      curScanline[w] >>= 1;
	      curScanline[w+(cirrusCur.width>>5)] >>= 1;
	    }
	  }

	  /* Copy this scanline to the cursor image */
	  for (w = 0; w < cirrusCur.width >> 4; w++) {
	    *pHOSTDATA = curScanline[w];
	  }
	}

	/* Copy in any extra filler dwords */
	while(fillerDwords--)
	  *pHOSTDATA = 0x00000000;
      }
    }
  } else {     /* !HAVE546X() */
    if (x == 0 && y == 0) {
      for (l=count;l;l--)
	*pDstM-- = *pSrcM--;
    } else {     /* !(x == 0 && y == 0) */
      unsigned long *pDstS = pDstM - 32;
      unsigned long *pSrcS = pSrcM - 32;
      int t = -y;
      
      winw = cirrusCur.width >> 5;
      
      if (x == 0)
	{
	  for (l=0; l < cirrusCur.height; l++)
	    {
	      for (w=0; w < winw; w++)
		{
		  if (l<t)
		    {
		      *pDstM-- = 0;
		      *pDstS-- = 0;
		    }
		  else
		    {
		      *pDstM-- = *pSrcM--;
		      *pDstS-- = *pSrcS--;
		    }
		}		      
	    }
	}
      else
	{
	  unsigned char *bpDstS = (unsigned char *)pDstS + 3;
	  unsigned char *bpDstM = (unsigned char *)pDstM + 3;
	  unsigned char *bpSrcS = (unsigned char *)pSrcS + 3;
	  unsigned char *bpSrcM = (unsigned char *)pSrcM + 3;
	  int bskip = (-x) >> 3;
	  
	  lshift = (-x) & 0x7;
	  rshift = 8 - lshift;
	  winw <<= 2;
	  
	  for (l=0; l < cirrusCur.height; l++)
	    {
	      unsigned char leftoverM = 0;
	      unsigned char leftoverS = 0;
	      unsigned char temp;
       
	      if (l<t)
		{
		  for (w=0; w < winw; w++)
		    {
		      *bpDstM-- = 0;
		      *bpDstS-- = 0;
		    }
		}
	      else
		{
		  for (w=0; w < winw; w++)
		    {
		      if (w < bskip)
			{
			  *bpDstM-- = 0;
			  *bpDstS-- = 0;
			}
		      else
			{
			  temp = *bpSrcM--;
			  *bpDstM-- = (temp << lshift) | leftoverM;
			  leftoverM = temp >> rshift;
			  
			  temp = *bpSrcS--;
			  *bpDstS-- = (temp << lshift) | leftoverS;
			  leftoverS = temp >> rshift;
			}
		    }
		  bpSrcM -= bskip;
		  bpSrcS -= bskip;
		}
	      
	    }
	}
    }


  }

}

static void
cirrusLoadCursor(pScr, pCurs, x, y)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int x, y;
{
   int   index = pScr->myNum;
   int   i, count;
   unsigned long *pDst, *pSrc;
   unsigned long dAddr;

   if (!xf86VTSema)
      return;

   if (!pCurs)
      return;

   /* Remember the cursor currently loaded into this cursor slot */
   cirrusCur.pCurs = pCurs;

   cirrusHideCursor ();

   /* Select the cursor index */
   if (HAVE546X()) {
     /* The only cursor that we'll support at first is one located
	at the very last 1k in video memory.  Since we're always running
	in tiled mode, we have to be a little careful about how we generate
	the linear offset for the cursor.  We'll need to know what
	kind of interleaving we using, and, of course, how much video 
	memory we have */

     if (cirrusMMIOBase) {
       unsigned long curAddr;
       unsigned short *pCursorAddr = (unsigned short *)(cirrusMMIOBase + 0xE8);

       cirrusFindLastTile(NULL, NULL, &curAddr, NULL);

       curAddr >>= 8;  /* Keep only bits 22:10 */
       *pCursorAddr = curAddr & 0x7FFC;
     }
       

   } else {
     outw (0x3C4, (cirrusCur.cur_select << 8) | 0x13);
   }

   cirrusLoadCursorToCard (pScr, pCurs, x, y);

   cirrusRecolorCursor (pScr, pCurs, 1);

   /* position cursor */
   cirrusMoveCursor (pScr, x, y);

   /* Turn it on */
   cirrusShowCursor ();
}

static void
cirrusSetCursor(pScr, pCurs, x, y, generateEvent)
     ScreenPtr pScr;
     CursorPtr pCurs;
     int   x, y;
     Bool  generateEvent;

{
  if (!pCurs)
    return;

  cirrusCur.hotX = pCurs->bits->xhot;
  cirrusCur.hotY = pCurs->bits->yhot;

  cirrusLoadCursor(pScr, pCurs, x, y);
}

void
cirrusRestoreCursor(pScr)
     ScreenPtr pScr;
{
   int index = pScr->myNum;
   int x, y;

   miPointerPosition(&x, &y);

   cirrusLoadCursor(pScr, cirrusCur.pCurs, x, y);
}

static void
cirrusMoveCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
  void cirrusCursorAdjust(void);

  if (!xf86VTSema)
      return;

  x -= vga256InfoRec.frameX0 + cirrusCur.hotX;
  y -= vga256InfoRec.frameY0 + cirrusCur.hotY;

  /* The Laguna family of chips uses memory that's tiled.  An artifact
     of this tiling is that the virtual desktop must be tile-alligned.  If
     it isn't, the image in the first tile is corrupted.  BUT, X still thinks
     that the virtual desktop pans pixel by pixel, and sends the mouse
     coordinates to us (the moveMouse function) accordingly.  Thus, we must
     compensate for the tile-alligned desktop here. */
  if (HAVE546X()) {
    int screenWidth = vga256InfoRec.frameX1 - vga256InfoRec.frameX0;

    if (x + cirrusLgCursorXOffset < 0) {
      cirrusCursorAdjust();  /* Bump one tile left */
    }
    else if (x + cirrusLgCursorXOffset > screenWidth)
      cirrusCursorAdjust();  /* Bump one tile right */
    x += cirrusLgCursorXOffset;
  }

  if (x < 0 || y < 0)
    {
      cirrusLoadCursorToCard (pScr, cirrusCur.pCurs, x, y);
      cirrusCur.skewed = 1;
    }
  else if (cirrusCur.skewed)
    {
      cirrusLoadCursorToCard (pScr, cirrusCur.pCurs, 0, 0);
      cirrusCur.skewed = 0;
    }
  



  if (x < 0) x = 0;
  if (y < 0) y = 0;

  if (XF86SCRNINFO(pScr)->modes->Flags & V_DBLSCAN)
      y *= 2;

  if (cirrusChip == CLGD5434 && XF86SCRNINFO(pScr)->modes->CrtcHAdjusted) {
      /* 5434 palette-clock doubling mode; cursor is squashed but */
      /* get at least the position right. */
      x /= 2;
      if (XF86SCRNINFO(pScr)->modes->CrtcVAdjusted)
          y /= 2;
  }

  if (HAVE546X()) {
    unsigned short *pX = (unsigned short *)(cirrusMMIOBase + 0xE0);
    unsigned short *pY = (unsigned short *)(cirrusMMIOBase + 0xE2);

    /* Account for desktop being restricted to tile allignment */
    *pX = x;
    *pY = y;
  } else {
    /* Your eyes do not deceive you - the low order bits form part of the
     * the INDEX
     */

    outw (0x3C4, (x << 5) | 0x10);
    outw (0x3C4, (y << 5) | 0x11);
  }
}

static void
cirrusRecolorCursor(pScr, pCurs, displayed)
     ScreenPtr pScr;
     CursorPtr pCurs;
     Bool displayed;
{
   unsigned short red, green, blue;
   int shift;
   int i;
   VisualPtr pVisual;
   unsigned char sr12;

   if (!xf86VTSema)
       return;

   if (!displayed)
       return;

   /* Find the PseudoColour or TrueColor visual for the colour mapping
    * function
    */

   for (i = 0, pVisual = pScr->visuals; i < pScr->numVisuals; i++, pVisual++)
     {
       if ((pVisual->class == PseudoColor) || (pVisual->class == TrueColor))
	 break;
     }

   if (i == pScr->numVisuals)
     {
       ErrorF ("CIRRUS: Failed to find a visual for mapping hardware cursor colours\n");
       return;
     }

   /*
    * The cursor uses a palette entry with 6-bits color resolution, even
    * in truecolor mode.
    */
   shift = 16 - 6;

   if (HAVE546X()) {
     unsigned char *pPalStateReg = (unsigned char *)(cirrusMMIOBase + 0xB0);

     *pPalStateReg |= 0x08;    /* Enable access to cursor colors */

   } else {
     outb (0x3c4, 0x12);       /* SR12 allows access to DAC extended colors */
     sr12 = inb (0x3c5);
                               /* Disable the cursor and allow access to
                                  the hidden DAC registers */
     outb (0x3c5, (sr12 & 0xfe) | 0x02);
   }

   red   = pCurs->backRed;
   green = pCurs->backGreen;
   blue  = pCurs->backBlue;

   pScr->ResolveColor (&red, &green, &blue, pVisual);

   outb (0x3c8, 0x00);          /* DAC color 256 */
   outb (0x3c9, (red>>shift));
   outb (0x3c9, (green>>shift));
   outb (0x3c9, (blue>>shift));

   red   = pCurs->foreRed;
   green = pCurs->foreGreen;
   blue  = pCurs->foreBlue;

   pScr->ResolveColor (&red, &green, &blue, pVisual);

   outb (0x3c8, 0x0f);          /* DAC color 257 */
   outb (0x3c9, (red>>shift));
   outb (0x3c9, (green>>shift));
   outb (0x3c9, (blue>>shift));

   if (HAVE546X()) {
     unsigned char *pPalStateReg = (unsigned char *)(cirrusMMIOBase + 0xB0);

     *pPalStateReg &= ~0x08;    /* Disable access to cursor colors */

   } else {
                               /* Restore the state of SR12 */
     outw (0x3c4, (sr12 <<8) | 0x12);
   }
}

void
cirrusWarpCursor(pScr, x, y)
     ScreenPtr pScr;
     int   x, y;
{
  miPointerWarpCursor(pScr, x, y);
  xf86Info.currentScreen = pScr;
}

void
cirrusQueryBestSize(class, pwidth, pheight, pScreen)
     int class;
     unsigned short *pwidth;
     unsigned short *pheight;
     ScreenPtr pScreen;
{
  if (*pwidth > 0)
    {
      switch (class)
	{
	case CursorShape:
	  if (*pwidth > cirrusCur.width)
	    *pwidth  = cirrusCur.width;
	  if (*pheight > cirrusCur.height)
	    *pheight = cirrusCur.height;
	  break;

	default:
	  (void) mfbQueryBestSize(class, pwidth, pheight, pScreen);
	  break;
	}
    }
}
