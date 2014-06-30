/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/agx/agxBCach.c,v 3.16 1996/12/23 06:32:26 dawes Exp $ */
/*
 * Copyright 1993 by Jon Tombs. Oxford University
 * Copyright 1994 by Henry A. Worth, Sunnyvale, California.
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Jon Tombs or Oxford University shall
 * not be used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission. The authors  make no
 * representations about the suitability of this software for any purpose. It
 * is provided "as is" without express or implied warranty.
 * 
 * JON TOMBS AND HENRY A. WORTH DISCLAIM ALL WARRANTIES WITH REGARD 
 * TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 * AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, 
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION 
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 */
/* $XConsortium: agxBCach.c /main/9 1996/10/22 10:33:19 kaleb $ */

/*
 * Id: s3bcach.c,v 2.3 1993/07/24 13:16:56 jon Exp
 */

#include	"X.h"
#include	"Xmd.h"
#include	"Xproto.h"
#include	"cfb.h"
#include	"misc.h"
#include        "xf86.h"
#include	"windowstr.h"
#include	"gcstruct.h"
#include	"fontstruct.h"
#include	"dixfontstr.h"
#include	"agx.h"
#include	"regagx.h"
#include        "agxBCach.h"
#define XCONFIG_FLAGS_ONLY
#include        "xf86_Config.h"

extern bitMapBlockPtr blockInUse;
extern unsigned long agxFontAge;

static bitMapRowPtr headBitRow = NULL;   /* top of linked list of cache rows */
CacheFont8Ptr       agxHeadFont = NULL;  /* top of list of fonts in cache */

extern int xf86Verbose;

#ifdef DEBUG_FCACHE
static void showcache();
#endif

/*
 * TERMS:
 * A bitmap row is a rectangular section of the offscreen memory, which may
 * contain one or more font blocks.
 *
 * A font block is the bitmap images of 32 characters from a font. Characters
 * are spaced regularly with spacing equal to the width of the widest character
 * in the font.
 */

void
agxBitCache8Init(x,y)
   int x;
   int y;
{
   unsigned char i;
   static Bool reEntry = FALSE;
   unsigned int numRows = agxFontCacheSize / FONT_ROW_LENGTH;
   
   if (reEntry) {
      CacheFont8Ptr CFptr;
/*
 * The screen has been destroyed so we must go through all cached font blocks
 * and return them to the cache
 */
      for (CFptr = agxHeadFont; CFptr != NULL; CFptr=CFptr->next) {
	 if (CFptr->font) {
	    for (i = 0; i < BLOCKS_PER_FONT; i++) {
	       if (CFptr->fblock[i] != NULL)
		  agxCReturnBlock(CFptr->fblock[i]);
	    }
	 }
      }
      return;
   }

   reEntry = TRUE;
      
   agxHeadFont = NULL;
   headBitRow  = NULL;
   if ( numRows ) {
      bitMapRowPtr bptr;
      /* xcalloc returns zeroed memory */
      agxHeadFont = (CacheFont8Ptr) xcalloc(1,sizeof(CacheFont8Rec));
      headBitRow  = (bitMapRowPtr) xcalloc(1,sizeof(bitMapRowRec));
      bptr        = headBitRow;
      i           = numRows;
      while( i-- ) {	
         bptr->offset = agxFontCacheOffset + i * FONT_ROW_LENGTH;
	 bptr->freel  = ROW_NUM_LINES;	
         if( i ) {
	    bptr->next   = (bitMapRowPtr) xcalloc(1,sizeof(bitMapRowRec));
	    bptr->next->prev  = bptr;
	    bptr = bptr->next;
         }
      }
   }
   else {
      agxFontCacheSize = 0;
   }

   if (xf86Verbose) {
      if (agxFontCacheSize) {
	 ErrorF( "%s %s: Font Cache = %dk @ offset 0x%x\n",
	         XCONFIG_PROBED, agxInfoRec.name, 
                 agxFontCacheSize>>10, agxFontCacheOffset );
      } else {

         if( OFLG_ISSET(OPTION_NOACCEL, &agxInfoRec.options) )
	    ErrorF("%s %s: Font caching disabled by the \"noaccel\" option\n",
                   XCONFIG_GIVEN, agxInfoRec.name);
         else
	    ErrorF("%s %s: Font caching is disabled due to lack of VRAM\n",
                   XCONFIG_PROBED, agxInfoRec.name);
      }
   }
}

/*
 * Go through the linked list of rows looking for a row with 
 * enough free lines available for the font block to be cached.
 *
 * We also look for any space that is currently in use that would have been
 * big enough. If none of the rows have room, one of these is removed from
 * the cache (subject to its lru value), and the function recurses, knowing
 * this time it will meet success.
 * If we didn't even find a block in use big enough we return NULL and the
 * font code must throw out some other blocks to make room.
 */
bitMapBlockPtr
agxCGetBlock( fentry, block, size  )
     CacheFont8Ptr fentry;
     int   block;     
     int   size;      /* Block size in lines */
{
   bitMapRowPtr bptr = headBitRow;
   bitMapBlockPtr canidate = NULL;
   unsigned long oldest = 0;
   
   do {
       if (bptr->blocks == NULL) { /* block is empty */    
	  bptr->blocks = (bitMapBlockPtr) xcalloc(1,sizeof(bitMapBlockRec));
	  bptr->blocks->line = 0;
	  bptr->blocks->sizel = size;
	  bptr->blocks->daddy = bptr; /* so we can trace a block back to its
	                               * parent row.  
		                       */
          bptr->blocks->fentry = fentry;
          bptr->blocks->block  = block;
	  bptr->freel -= size;        /* reduce the free space of this row */
	  SHOWCACHE();
	  return bptr->blocks;
       } 
       else { /* This cache already contains a block */
	  if (bptr->freel >= size) { /* is the space left big enough? */
	     bitMapBlockPtr bbptr = bptr->blocks;

	     while (bbptr->next != NULL) /* find the last block */
		bbptr = bbptr->next; 

	     /* and add this block onto the end */
	     bbptr->next = (bitMapBlockPtr) xcalloc(1,sizeof(bitMapBlockRec));
	     bbptr->next->line  = ROW_NUM_LINES - bptr->freel;
	     bbptr->next->sizel = size;
	     bbptr->next->daddy = bptr;
             bbptr->next->fentry = fentry;
             bbptr->next->block  = block;
	     bptr->freel -= size;
	     SHOWCACHE();
	     return bbptr->next;
	  }
          else { 
             /*
              * Select the oldest font of suitable size, other than 
              * the last or very recently used slots from the same font.
              */
	     bitMapBlockPtr bbptr = bptr->blocks;
	     while (bbptr != NULL)  {
		if( bbptr->sizel >= size 
                    && ( canidate == NULL 
                         || bbptr->lru < oldest
                            /* check if we haven't caught lru clock wrap */
                         || bbptr->lru > agxFontAge && oldest < agxFontAge
                       )
                    && bbptr != blockInUse 
                    && ( bbptr->fentry != fentry   
                         || agxFontAge - bbptr->lru > 100 )
                  ) {
		   oldest = bbptr->lru;
		   canidate = bbptr; /* our prime candidate to remove
				      * If no space is found
				      */
		}
		bbptr = bbptr->next;
	     }
	  }
      }
   } while ((bptr = bptr->next) != NULL);

   /*
    * If we get here then there are no slots left.
    * We throw out one out if we found a suitable
    * candidate, else let the caller deal with it.
    */

   ERROR_F(("forced font return\n"));
   if (canidate != NULL) {
      agxCReturnBlock(canidate);
      return agxCGetBlock(fentry,block,size);
   } 
   else
      return NULL;	/* shouldn't happen unless you try very hard */
   
}

/*
 * Return a block to the parent cache:
 * Several cases can arise.
 * - We are the last block of the linked list, in which case just add our
 *   length to the free length.
 * - we are a block in the middle of the list of blocks. This is nasty.
 *   we shuffle the blocks that follow by shifting them to the left our length.
 *   This keeps the free space at the right hand end.
 */

void
agxCReturnBlock(block)
     bitMapBlockPtr block;
{
   bitMapBlockPtr first, tmpb;
   bitMapRowPtr bptr;

   bptr = block->daddy;
   first  = bptr->blocks;

   if( block == blockInUse) 
   {
      GE_WAIT_IDLE_SHORT();
   }

   ERROR_F(("free 0x%x(0x%x) 0x%x: line offset=0x%x num lines=0x%x\n", 
              block->fentry, block->block, 
              block->fentry ? 
                 block->fentry->fblock[block->block] : 0,
              block->line, block->sizel));
   SHOWCACHE();
   bptr->freel += block->sizel;	/* this much we can be sure of */

   if (block->next == NULL) {	/* we are the last of a row */

      if (block == first) {	/* we are the row */
         ERROR_F(("left zero length line\n"));
         bptr->blocks = NULL;
      }
      else {
	 /* simply loose the end of the line */
	 tmpb = first;
	 while (tmpb->next != block)
	    tmpb = tmpb->next;
	 tmpb->next = NULL;
	 ERROR_F(("returning end of line\n"));
      }
   }
   else { /* we are not the last of the row */
      unsigned int newLine;
      extern Bool geBlockMove;

      newLine = block->line;
      tmpb = block->next;
      if( xf86VTSema ) {
         geBlockMove = TRUE;      
         MAP_INIT( GE_MS_MAP_B,
                   agxVideoMapFormat & ~GE_MF_MOTO_FORMAT,
                   block->daddy->offset + agxMemBase,
                   AGX_TO_PIXEL(CACHE_LINE_WIDTH_BYTES)-1,
                   ROW_NUM_LINES-1,
                   FALSE, FALSE, FALSE );

         GE_WAIT_IDLE();
         MAP_SET_SRC_AND_DST( GE_MS_MAP_B ); 
         GE_SET_MAP( GE_MS_MAP_B );
         GE_OUT_W( GE_FRGD_MIX, MIX_SRC );
         GE_OUT_D( GE_PIXEL_BIT_MASK, agxPixMask );
         GE_OUT_W( GE_PIXEL_OP,
                   GE_OP_PAT_FRGD
                   | GE_OP_MASK_DISABLED
                   | GE_OP_INC_X
                   | GE_OP_INC_Y         );

         while (tmpb != NULL) {
            unsigned int srcCoOrd, dstCoOrd, opDim;
            srcCoOrd = tmpb->line << 16;
            dstCoOrd = newLine << 16; 
            opDim    = (tmpb->sizel-1 << 16) 
                       | AGX_TO_PIXEL(CACHE_LINE_WIDTH_BYTES)-1;

            GE_WAIT_IDLE();
            GE_OUT_D( GE_SRC_MAP_X, srcCoOrd );
            GE_OUT_D( GE_DEST_MAP_X, dstCoOrd );
            GE_OUT_D( GE_OP_DIM_WIDTH, opDim ); 
            GE_START_CMDW( GE_OPW_BITBLT
                           | GE_OPW_FRGD_SRC_MAP
                           | GE_OPW_SRC_MAP_B
                           | GE_OPW_DEST_MAP_B   );
            tmpb->line = newLine;
            newLine += tmpb->sizel;
	    tmpb = tmpb->next;
         }
         GE_WAIT_IDLE_EXIT();
      }
      else {
         while (tmpb != NULL) {
            tmpb->line = newLine;
            newLine += tmpb->sizel;
	    tmpb = tmpb->next;
         }
      }

      /* reconnect the new list of blocks */
      if ( block == first ) {
         bptr->blocks = block->next;
      }
      else {
         tmpb = first;
         while (tmpb->next != block)
            tmpb = tmpb->next;

         tmpb->next = block->next;
      }
   }

   ERROR_F(("----------To---------------\n"));
   SHOWCACHE();
   /* This allows us to remove the reference to this block even if we don't
    * know where that is. This is used for when we need to free a block in
    * order to store a new one.
    */
   if( block->fentry != NULL ) {
      if( block->block < BLOCKS_PER_FONT ) 
         block->fentry->fblock[block->block]=NULL; 
      block->fentry=NULL; 
   }
   xfree(block);

}

#ifdef DEBUG_FCACHE
/*
 * debuging print out, this give a nice picture of the current structure
 * of linked lists - believe it or not.
 */
static void
showcache()
{
   bitMapBlockPtr tmpb;
   bitMapRowPtr brptr;

   for (brptr = headBitRow; brptr->next != NULL; brptr = brptr->next) {
      ErrorF("freel 0x%08x offset 0x%08x: ", brptr->freel, brptr->offset);
      for (tmpb = brptr->blocks; tmpb != NULL; tmpb = tmpb->next) {
	 ErrorF( "l=0x%x s=0x%x", tmpb->line, tmpb->sizel ); 
         if( tmpb->next )
            ErrorF( ", " );
      }
      ErrorF( "\n" );
   }
}
#endif
