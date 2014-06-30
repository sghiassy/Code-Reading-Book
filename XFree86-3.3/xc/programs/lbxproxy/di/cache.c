/* $XConsortium: cache.c /main/11 1996/11/17 22:27:57 rws $ */
/*
Copyright (c) 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

 * Copyright 1990, 1991 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation 
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices,
 * or Digital not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Network Computing Devices, or Digital
 * make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * NETWORK COMPUTING DEVICES, AND DIGITAL DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES, OR DIGITAL BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */
#include	"misc.h"
#include	"util.h"
#include	"cache.h"

#define INITBUCKETS 64
#define INITHASHSIZE 6
#define MAXHASHSIZE 11


#define	ENTRYOFFSET		22
#define CACHE_ENTRY_MASK	0x3FFFFF
#define	CACHE_ENTRY_BITS(id)	((id) & 0x1fc00000)
#define	CACHE_ID(id)		((int)(CACHE_ENTRY_BITS(id) >> ENTRYOFFSET))

typedef struct _cache_entry {
    pointer     data;
    unsigned long timestamp;
    CacheID     id;
    unsigned long size;
    CacheFree   free_func;
    struct _cache_entry *next;
}           CacheEntryRec, *CacheEntryPtr;

typedef struct _cache {
    Cache       id;
    CacheEntryPtr *entries;
    int         elements;
    int         buckets;
    int         hashsize;
    CacheID     nextid;
    unsigned long maxsize;
    unsigned long cursize;
}           CacheRec;

#define	MAX_NUM_CACHES	2
static CachePtr caches[MAX_NUM_CACHES + 1];
static int  num_caches = 0;
static unsigned long seed = 0;

/*-
 * Notes on cache implementation
 *
 * This is basically the X11 resource code, with some modifications
 * to handle aging.
 *
 * Its currently optimized for lookup & store.  Flushing old stuff
 * is a lot slower than it should probably be, but there's tradeoffs
 * in tuning.
 */

Cache
CacheInit(maxsize)
    unsigned long maxsize;
{
    Cache       id;
    CachePtr    cache;

    cache = (CachePtr) xalloc(sizeof(CacheRec));
    if (!cache)
	return (Cache) 0;
    cache->entries = (CacheEntryPtr *)
	xalloc(INITBUCKETS * sizeof(CacheEntryPtr));
    if (!cache->entries) {
	xfree(cache);
	return (Cache) 0;
    }
    bzero((char *) cache->entries, (INITBUCKETS * sizeof(CacheEntryPtr)));
    id = (Cache) ++num_caches;
    caches[id] = cache;
    cache->elements = 0;
    cache->buckets = INITBUCKETS;
    cache->hashsize = INITHASHSIZE;
    cache->maxsize = maxsize;
    cache->cursize = 0;
    cache->nextid = id << ENTRYOFFSET;
    cache->id = id;
    return id;
}

void
CacheFreeCache(cid)
    Cache       cid;
{
    CachePtr cache = caches[cid];
    int i;
    CacheEntryPtr cp;

    for (i = 0; i < cache->buckets; i++) {
	while (cp = cache->entries[i]) {
	    cache->entries[i] = cp->next;
	    (*cp->free_func) (cp->id, cp->data, CacheWasReset);
	    xfree(cp);
	}
    }
    caches[cid] = NULL;
    xfree(cache->entries);
    xfree(cache);
}

static int
hash(cid, id)
    Cache       cid;
    CacheID     id;
{
    CachePtr    cache = caches[cid];

    switch (cache->hashsize) {
#if INITHASHSIZE < 6
    case 2:
	return ((int) (0x03 & (id ^ (id >> 2) ^ (id >> 8))));
    case 3:
	return ((int) (0x07 & (id ^ (id >> 3) ^ (id >> 9))));
    case 4:
	return ((int) (0x0F & (id ^ (id >> 4) ^ (id >> 10))));
    case 5:
	return ((int) (0x01F & (id ^ (id >> 5) ^ (id >> 11))));
#endif
    case 6:
	return ((int) (0x03F & (id ^ (id >> 6) ^ (id >> 12))));
    case 7:
	return ((int) (0x07F & (id ^ (id >> 7) ^ (id >> 13))));
    case 8:
	return ((int) (0x0FF & (id ^ (id >> 8) ^ (id >> 16))));
    case 9:
	return ((int) (0x1FF & (id ^ (id >> 9))));
    case 10:
	return ((int) (0x3FF & (id ^ (id >> 10))));
    case 11:
	return ((int) (0x7FF & (id ^ (id >> 11))));
    }
    return -1;
}

static void
rebuild_cache(cache)
    CachePtr    cache;
{
    int j;
    CacheEntryPtr cp, next, **tails, *entries, **tptr, *cptr;

    j = cache->buckets << 1;
    tails = (CacheEntryPtr **) ALLOCATE_LOCAL(j * sizeof(CacheEntryPtr *));
    if (!tails)
	return;
    entries = (CacheEntryPtr *) xalloc(j * sizeof(CacheEntryPtr));
    if (!entries) {
	DEALLOCATE_LOCAL(tails);
	return;
    }
    for (cptr = entries, tptr = tails; --j >= 0; cptr++, tptr++) {
	*cptr = NULL;
	*tptr = cptr;
    }
    cache->hashsize++;
    for (j = cache->buckets, cptr = cache->entries; --j >= 0; cptr++) {
	for (cp = *cptr; cp; cp = next) {
	    next = cp->next;
	    cp->next = NULL;
	    tptr = &tails[hash(cache->id, cp->id)];
	    **tptr = cp;
	    *tptr = &cp->next;
	}
    }
    DEALLOCATE_LOCAL(tails);
    cache->buckets <<= 1;
    xfree(cache->entries);
    cache->entries = entries;
}

void
CacheFreeAll()
{
    int         i;

    for (i = 1; i <= num_caches; i++) {
	if (caches[i])
	    CacheFreeCache(i);
    }
    num_caches = 0;
}

static void
flush_cache(cache, needed)
    CachePtr    cache;
    unsigned long needed;
{
    CacheEntryPtr cp, *prev, oldest, *oldprev;
    int i;

    while ((cache->cursize + needed) > cache->maxsize) {
	oldest = NULL;
	/* find oldest */
	for (i = 0; i < cache->buckets; i++) {
	    for (prev = &cache->entries[i]; cp = *prev; prev = &cp->next) {
		if (!oldest || (cp->timestamp < oldest->timestamp)) {
		    oldest = cp;
		    oldprev = prev;
		}
	    }
	}
	if (!oldest)
	    break;
	/* clobber it */
	cache->elements--;
	cache->cursize -= oldest->size;
	*oldprev = oldest->next;
	(*oldest->free_func) (oldest->id, oldest->data, CacheEntryOld);
	xfree(oldest);
    }
}

Bool
CacheTrimNeeded(cid)
    Cache       cid;
{
    CachePtr    cache = caches[cid];

    return cache->cursize > cache->maxsize;
}

void
CacheTrim(cid)
    Cache       cid;
{
    flush_cache(caches[cid], 0);
}

Bool
CacheStoreMemory(cid, id, data, size, free_func, can_flush)
    Cache       cid;
    CacheID     id;
    pointer     data;
    unsigned long size;
    CacheFree   free_func;
    Bool	can_flush;
{
    CacheEntryPtr cp, *head;
    CachePtr    cache = caches[cid];

    if (can_flush && (size > cache->maxsize)) /* beyond cache limits */
	return FALSE;
    cp = (CacheEntryPtr) xalloc(sizeof(CacheEntryRec));
    if (!cp)
	return FALSE;
    if (can_flush && ((cache->cursize + size) > cache->maxsize))
	flush_cache(cache, size);
    if ((cache->elements >= (cache->buckets << 2)) &&
	(cache->hashsize < MAXHASHSIZE))
	rebuild_cache(cache);
    head = &cache->entries[hash(cid, id)];
    cp->next = *head;
    cp->id = id;
    cp->timestamp = ++seed;
    cp->free_func = free_func;
    cp->size = size;
    cp->data = data;
    cache->cursize += size;
    cache->elements++;
    *head = cp;
    return TRUE;
}

pointer
CacheFetchMemory(cid, id, update)
    Cache       cid;
    CacheID     id;
    Bool        update;
{
    CachePtr    cache = caches[cid];
    CacheEntryPtr cp,
               *head;

    head = &cache->entries[hash(cid, id)];
    for (cp = *head; cp; cp = cp->next) {
	if (cp->id == id) {
	    if (update)
		cp->timestamp = ++seed;
	    return cp->data;
	}
    }
    return NULL;
}

void
CacheFreeMemory(cacheid, cid, notify)
    Cache       cacheid;
    CacheID     cid;
    Bool        notify;
{
    CachePtr    cache = caches[cacheid];
    CacheEntryPtr cp, *prev;

    for (prev = &cache->entries[hash(cacheid, cid)];
	 cp = *prev;
	 prev = &cp->next) {
	if (cp->id == cid) {
	    *prev = cp->next;
	    cache->elements--;
	    cache->cursize -= cp->size;
	    if (notify)
		(*cp->free_func)(cid, cp->data, CacheEntryFreed);
	    xfree(cp);
	    break;
	}
    }
}
