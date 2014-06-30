/* $XFree86: xc/programs/Xserver/hw/xfree86/accel/cache/xf86bcache.h,v 3.1 1996/12/23 06:33:19 dawes Exp $ */

/*
 * Structures, typedefs and function prototypes.
 */

/* $XConsortium: xf86bcache.h /main/3 1996/02/21 17:19:16 kaleb $ */

#ifndef _XF86_BCACHE_H
#define _XF86_BCACHE_H

#undef DEBUG_FCACHE    /* Define it to see the bugs */

#ifdef DEBUG_FCACHE
#define SHOWCACHE() xf86showcache()
#define ERROR_F(x)  ErrorF x
#else
#define SHOWCACHE() /**/
#define ERROR_F(x)   /**/
#endif

/*
 *  This data structure contains a allocated memory block.
 */

typedef struct _bitMapBlock{
     unsigned short x;		       /* offset in a row */
     unsigned short y;		       /* screen y */
     unsigned short w;		       /* width of block */
     unsigned short h;		       /* how high we are */
     unsigned long lru;		       /* lru */
     unsigned int id;		       /* bit plane id */
     struct _bitMapBlock *next;	       /* any following blocks */
     struct _bitMapRow *daddy;	       /* row to which we belong */
     pointer *reference;	       /* who is referencing us */
} bitMapBlockRec;

typedef struct _bitMapBlock *bitMapBlockPtr;
typedef struct _bitMapBlock *CacheBlock;

/*
 * This structure contains a row of linked memory blocks.
 */

typedef struct _bitMapRow{
     unsigned short x;		       /* x location */
     unsigned short y;		       /* y location */
     unsigned short freew;	       /* space left */
     unsigned short h;		       /* height */
     unsigned int id;	  	       /* bit plane id */
     struct _bitMapRow *next;	       /* next row */
     struct _bitMapRow *prev;	       /* previous row */
     struct _bitMapBlock *blocks;      /* start of linked list of blocks */
     pointer *crchain;		       /* points to the list head */
} bitMapRowRec;

typedef struct _bitMapRow *bitMapRowPtr;

/*
 * This structure points at a linked list of row structures.
 */

struct _cacherec { struct _cacherec *next;	/* Link to next cache rec. */
		   short width;			/* Total width of block.   */
		   short height;		/* Total height of block.  */
		   bitMapRowPtr blocks;		/* Start of row list.	   */
};

typedef struct _cacherec *CacheRecPtr;

/*
 * This is the top structure. It points at a linked list of
 * _cacherec structures. Each _cacherec structure points at a
 * linked list of row structures. Each row structure points at
 * a linked list of memory blocks.
 */

struct CachePoolRec { struct CachePoolRec *next;/* Link to next Pool struct.*/
		      unsigned int alignment;	/* Block alignment.         */
		      CacheRecPtr crecs;	/* Start of Crec list.	    */
};

typedef struct CachePoolRec *CachePool;



void xf86InitCache(
#if NeedFunctionPrototypes
    void (*/*CacheShiftBlockFunc*/)(int, int, int, int, int, int, unsigned int)
#endif
);

CachePool xf86CreateCachePool(
#if NeedFunctionPrototypes
    unsigned int /*Alignment*/
#endif
);

void xf86AddToCachePool(
#if NeedFunctionPrototypes
    CachePool /*Pool*/,
    short /*x*/,
    short /*y*/,
    short /*Width*/,
    short /*Heigth*/,
    unsigned int /*Id*/
#endif
);

CacheBlock xf86AllocFromCachePool(
#if NeedFunctionPrototypes
    CachePool /*Pool*/,
    short /*Width*/,
    short /*Height*/
#endif
);

void xf86ReleaseToCachePool(
#if NeedFunctionPrototypes
    CachePool /*Pool*/,
    CacheBlock /*Block*/
#endif
);

#endif /* _XF86_BCACHE_H */
