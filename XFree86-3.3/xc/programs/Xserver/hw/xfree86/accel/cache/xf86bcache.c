/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/cache/xf86bcache.c,v 3.6 1996/12/23 06:33:18 dawes Exp $ */
/*
 * Based on the S3 block allocator code in XFree86-2.0 by Jon Tombs.
 * The original copyright is reproduced below.
 *
 * Copyright 1993 by Jon Tombs. Oxford University
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
 * JON TOMBS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 * 
 */
/* $XConsortium: xf86bcache.c /main/8 1996/10/22 10:27:47 kaleb $ */

/*
 * Adapted to XFree86 in X11R6 by Hans Nasten. ( nasten@everyware.se ).
 *
 * The cache memory is managed in a tree structure 4 levels deep.
 * At the top level is a CachePool structure, containing a linked list
 * of CacheRec structures.
 * There can be an arbitrary number of Cache Pools, each used for a
 * different purpose. ( font cache, stipple cache or pixmap cache ).
 * A CachePool structure is allocated on each call to the
 * xf86CreateCachePool function and a pointer to the CachePool struct is
 * returned to the caller. All other block allocator functions requires
 * this pointer as a argument in order to identify the Cache Pool to be
 * affected by the call.
 *
 * The list of Cacherec structures each points at a linked list of
 * BitMapRow structures describing a row of memory.
 * A CacheRec structure is allocated for each invocation of the
 * xf86AddToCachePool function and is then linked to the Cache Pool
 * identified by the Pool argument.
 * A CacheRec struct can be used for a single or an arbitrary number of
 * bitplanes. The caller supplies a 32-bit id number that is stored in
 * the various data structures and is passed on when doing callbacks to
 * hw driver code. ( the only callback from the block allocator is for
 * compacting a memory row. Callbacks from font cache code etc. must also
 * pass on this id number when appropriate ).
 * The block allocator does not use the id number internally, it's expected
 * use is to enable hw driver code to identify which bitplanes that are to
 * be affected by hw driver code.
 * 
 * Each BitMapRow structure contains a linked list of BitMapBlock structures
 * describing a row of cache memory.
 *
 * Each BitMapBlock structure describes a allocated piece of cache memory.
 * The BitMapBlock struct contains a reference field used when allowing the
 * cache allocator to reuse already used cache blocks. This is used by the
 * font cache code to let the cache allocator take care of keeping the most
 * recently used fonts in the cache. The higher layer ( font cache etc. ) has
 * to keep the lru field updated if this reallocation scheme is to work.
 * If the reference field is left untouched, ( it's initialized to NULL ),
 * the allocator does not attempt to reuse already allocated cache blocks.
 */

#include	"X.h"
#include	"Xmd.h"
#include	"misc.h"
#include        "xf86.h"
#include        "xf86bcache.h"
#define XCONFIG_FLAGS_ONLY
#include        "xf86_Config.h"


#ifdef DEBUG_FCACHE
static void xf86showcache();
#endif

static void (*xf86CacheMoveBlockFunc)();
static struct CachePoolRec *xf86PoolList = NULL;

/*
 * Init the static pointers.
 */

void xf86InitCache( CacheMoveBlockFunc )
void (*CacheMoveBlockFunc)();

{

    xf86CacheMoveBlockFunc = CacheMoveBlockFunc;

}


/*
 * Create a data structure to keep info about a cache memory pool.
 * A pointer to this structure is the used to identify the pool.
 */

CachePool xf86CreateCachePool( Alignment )
unsigned int Alignment;

{
  CachePool CPool;

    if(( CPool = (CachePool)xcalloc(1, sizeof (struct CachePoolRec) )) == NULL )
      return( NULL );

    CPool->alignment = Alignment - 1;
    CPool->next = xf86PoolList;
    xf86PoolList = CPool;
    return( CPool );

}


/*
 * Add a cache memory area to a pool.
 * Each area added to the pool is pointed at by a CacheRec structure
 * that is linked to the CachePool structure.
 */

#ifdef __STDC__
void xf86AddToCachePool( CachePool Pool, short x, short y, 
			 short Width, short Height, unsigned int Id )
#else
void xf86AddToCachePool( Pool, x, y, Width, Height, Id )
CachePool Pool;
short x, y, Width, Height;
unsigned int Id;
#endif

{
  bitMapRowPtr bptr;
  CacheRecPtr CrPtr;

    /*
     * Create a linked list of all rows. Initially there
     * is only one row.
     */
    bptr = (bitMapRowPtr) xcalloc(1,sizeof(bitMapRowRec));
    bptr->x = x;
    bptr->y = y;
    bptr->freew = Width;
    bptr->h = Height;
    bptr->id = Id;

    /*
     * Link the new row to the pool via the CacheRec list.
     */
    CrPtr = (CacheRecPtr) xcalloc(1, sizeof( struct _cacherec ) );
    CrPtr->width = Width;
    CrPtr->height = Height;
    CrPtr->blocks = bptr;
    CrPtr->next = Pool->crecs;
    Pool->crecs = CrPtr;
    bptr->crchain = (pointer *) CrPtr;
}


/*
 * Return a block to the parent row:
 * Several cases can arise.
 * - We are the last block of the linked list, in which case just add our
 *   length to the free length.
 * - We are the only block in a row. The row is now empty, so if another
 *   empty row exists below or above we merge.
 * - we are a block in the middle of the list of blocks. This is nasty.
 *   we shuffle the blocks that follow by shifting them to the left our length.
 *   This keeps the free space at the right hand end.
 */

#ifdef __STDC__
void xf86ReleaseToCachePool( CachePool Pool, CacheBlock Block )
#else
void xf86ReleaseToCachePool( Pool, Block )
CachePool Pool;
CacheBlock Block;
#endif
{
  bitMapBlockPtr line, tmpb;
  bitMapRowPtr bptr, tmpr;

  bptr = Block->daddy;
  line = bptr->blocks;

  ERROR_F(("free block: (%dx%d):\n", bptr->h, Block->w));
  SHOWCACHE();
  bptr->freew += Block->w;	/* this much we can be sure of */

  if (Block->next == NULL) {	/* we are the last block in a row */

    if (Block == line) {	/* we are the row */
      if ((bptr->prev != NULL) && (bptr->id == bptr->prev->id)
	     && (bptr->prev->blocks == NULL)) {
	/* we are not first in plane so cut ourselves out upward */
	ERROR_F(("returning row to above"));
	bptr->prev->h += bptr->h;
	bptr->prev->next = bptr->next;
	if (bptr->next) { /* don't go off the end of the last row */
	  bptr->next->prev = bptr->prev;
	  tmpr = bptr->next;
	  if( tmpr->blocks == NULL
	      && tmpr->id == tmpr->prev->id ) {
			/* If the row below is empty, merge downwards. */
	    ERROR_F((" and to below"));
	    tmpr->prev->h += tmpr->h;
	    tmpr->prev->next = tmpr->next;
	    if( tmpr->next )
	      tmpr->next->prev = tmpr->prev;
	    xfree(tmpr);
	  }
	}
	ERROR_F(("\n"));
	xfree(bptr);
      } else if ((bptr->next != NULL)
		&& (bptr->id == bptr->next->id)
		&& (bptr->next->blocks == NULL)) {
	/* we are not last in plane and the row below is empty, so cut
	 * ourselves out merging with the row below.
	 */
	bptr->next->h += bptr->h;
	bptr->next->prev = bptr->prev;
	bptr->next->y = bptr->y;
	if (bptr->prev) {		/* we are not the head row */
	  bptr->prev->next = bptr->next;
	} else {	  /* promote the row below to new head row */
	  ((CacheRecPtr)(bptr->crchain))->blocks = bptr->next; 
	}
	xfree(bptr);
       	ERROR_F(("returning row to below\n"));
      } else {  /* just zero our length */
	ERROR_F(("left empty row %d high\n",bptr->h));
	bptr->blocks = NULL;
      }
    } else {
      /* simply loose the end of the line */
      tmpb = line;
	 
      while (tmpb->next != Block)
	tmpb = tmpb->next;

      tmpb->next = NULL;
      ERROR_F(("returning end of line\n"));
    }
  } else { /* we are not the last block in the row */
    int len;
    len = 0;
    tmpb = Block->next;

    /* find the length of blocks behind and adjust there x co-ordinate
     * by our width
     */
    while (tmpb != NULL) {
      len += tmpb->w;	 
      tmpb->x -= Block->w;
      tmpb = tmpb->next;
    }
    if (xf86VTSema) {  /* now shift the following block using a plane copy */
      (xf86CacheMoveBlockFunc)( Block->x + Block->w, Block->y, Block->x,
				 Block->y, bptr->h, len, Block->id );
    }

    /* reconnect the new list of blocks */
    if (Block == line)
      bptr->blocks = Block->next;
    else {
      tmpb = line;
      while (tmpb->next != Block)
	tmpb = tmpb->next;

      tmpb->next = Block->next;
    }
  }
  ERROR_F(("----------To---------------\n"));
  SHOWCACHE();
  /* This allows us to remove the reference to this block even if we don't
   * know where that is. This is used for when we need to free a block in
   * order to store a new one.
   */
  if( Block->reference != NULL )
    *(Block->reference)=NULL; 

  xfree(Block);

}

/*
 * Go through the linked list of rows looking for a row with height big
 * enough for the requested block.
 * If the height is OK then we check that the free length is also big enough
 * and if so add the block to a linked list in blocks in that row.
 *
 * We also look for any space that is currently in use that would have been
 * big enough. If none of the rows have room, one of these is removed from
 * the cache (subject to its lru value), and the function recurses, knowing
 * this time it will meet success.
 * If we didn't even find a block in use big enough we return NULL and the
 * caller code must throw out some other blocks to make room.
 */

#ifdef __STDC__
CacheBlock xf86AllocFromCachePool( CachePool Pool, short Width, short Height )
#else
CacheBlock xf86AllocFromCachePool( Pool, Width, Height )
CachePool Pool;
short Width, Height;
#endif

{
  CacheRecPtr CrPtr = Pool->crecs;
  bitMapRowPtr bptr;
  bitMapBlockPtr candidate = NULL;
  int oldest=0x7fffffff;   

  Width = ( Width + Pool->alignment ) & ~Pool->alignment;
  while( CrPtr != NULL ) {
    if( Width <= CrPtr->width && Height <= CrPtr->height ) {
      bptr = CrPtr->blocks;
        while( bptr != NULL ) {
        if (bptr->h >= Height ) {
          if (bptr->blocks == NULL) {    /* First use of this row. */
	    bptr->blocks = (bitMapBlockPtr) xcalloc(1,sizeof(bitMapBlockRec));
	    bptr->blocks->x = bptr->x;
	    bptr->blocks->y = bptr->y;
	    bptr->blocks->h = Height;
	    bptr->blocks->w = Width;
	    bptr->blocks->daddy = bptr; /* so we can trace a block back to its
					 * parent row.  
					 */
	    if (bptr->h > Height) { /* split the row. We create a new row with
	                             * the unused height of the first.
				     */
	      bitMapRowPtr nbptr=(bitMapRowPtr) xcalloc(1,sizeof(bitMapRowRec));

	      nbptr->h = bptr->h - Height;
	      nbptr->freew = bptr->freew;
	      nbptr->x = bptr->x;
	      nbptr->y = bptr->y + Height;
	      nbptr->id = bptr->id;
	      nbptr->crchain = bptr->crchain;
	      if( ( nbptr->next = bptr->next ) != NULL )
	        nbptr->next->prev = nbptr;

	      bptr->next = nbptr;
	      nbptr->prev = bptr;
	      bptr->h = Height;
	    }
	    bptr->freew -= Width;   /* reduce the free space of this row */
	    bptr->blocks->id = bptr->id;
	    SHOWCACHE();
	    return bptr->blocks;
          } else {		        /* This row already contains a block */
	    if (bptr->freew >= Width) {   /* is the space left big enough? */
	      bitMapBlockPtr bbptr = bptr->blocks;

	      while (bbptr->next != NULL) /* find the last block */
	        bbptr = bbptr->next; 

	      /* and add this block onto the end */
	      bbptr->next = (bitMapBlockPtr) xcalloc(1,sizeof(bitMapBlockRec));
	      bbptr->next->x = bbptr->x + bbptr->w;
	      bbptr->next->y = bptr->y;
	      bbptr->next->h = Height;
	      bbptr->next->w = Width;
	      bbptr->next->daddy = bptr;
	      bbptr->next->id = bptr->id;
	      bptr->freew -= Width;
	      SHOWCACHE();
	      return bbptr->next;
	    } else { /* see if any slot is big enough anyway */
	      bitMapBlockPtr bbptr = bptr->blocks;
	      while (bbptr/*->next*/ != NULL)  {
	        if (bbptr->w >= Width && bbptr->lru < oldest
		   && bbptr->lru > 1 && bbptr->reference != NULL) {
	          oldest = bbptr->lru;
	          candidate = bbptr; /* Our prime candidate to remove
				      * if no space is found.
				      * ( don't remove blocks that has
				      *   no valid reference pointer ).
				      */
		  ERROR_F(("Want h=%d w=%d Candidate %d h=%d w=%d lru=%d\n",
			   Height,Width,bbptr,bptr->h,bbptr->w,bbptr->lru));
	        }
	        bbptr = bbptr->next;
	      }
	    }
          }
        }
        bptr = bptr->next;	/* Next row. */
      }
    }
    CrPtr = CrPtr->next;	/* Next CacheRec struct. */
  }
  /* If we get here then there are no slots left
   * We throw out the least used block if we found one big enough.
   * else we return null and let the calling code do something about it.
   */
  if (candidate != NULL) {
    ERROR_F(("forced block return\n"));
    xf86ReleaseToCachePool( Pool, candidate );
    return xf86AllocFromCachePool( Pool, Width, Height);
  } else {
    ERROR_F(("no block available, returning NULL\n"));
    return NULL;	/* shouldn't happen unless you try very hard */
  }

}

#ifdef DEBUG_FCACHE
/*
 * debugging print out, this give a nice picture of the current structure
 * of linked lists - believe it or not.
 */
static void xf86showcache()
{
    bitMapBlockPtr tmpb;
    bitMapRowPtr brptr;
    CacheRecPtr CrPtr;
    struct CachePoolRec *PPtr;

    for( PPtr = xf86PoolList; PPtr != NULL; PPtr = PPtr->next ) {
      ErrorF("----- Cache Pool %d Alignment %d -----\n",
	     PPtr, PPtr->alignment + 1);
      for( CrPtr = PPtr->crecs; CrPtr != NULL; CrPtr = CrPtr->next ) {
	for (brptr = CrPtr->blocks; brptr != NULL; brptr = brptr->next) {
          ErrorF("id %d freew %d h %d y %d: ", brptr->id,
	         brptr->freew, brptr->h, brptr->y);
          for (tmpb = brptr->blocks; tmpb != NULL; tmpb = tmpb->next)
	    ErrorF(" block x=%d w=%d y=%d h=%d",
		   tmpb->x, tmpb->w, tmpb->y, tmpb->h);

	  ErrorF(":\n");
        }
      }
    }

}
#endif
