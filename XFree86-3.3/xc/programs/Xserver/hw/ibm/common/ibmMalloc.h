/*
 * $XConsortium: ibmMalloc.h,v 1.2 91/07/16 13:08:24 jap Exp $
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
    malloc.h - definitions for memory allocation

	author: WJHansen, CMU/ITC
	(c) Copyright IBM Corporation, 1986
*/

/*
 *	a different implementation may need to redefine
 *	INT WORD BLOCK ACTIVE PREACTIVE
 *	where INT is integer type to which a pointer can be cast
 *	and INT is the number of bytes given by WORD
 *	WORD needs to be at least 4 so there are two zero bits
 *	at the bottom of a Size field
 */

#define INT int
#define WORD 4
#define EPSILON  (sizeof(struct freehdr)+sizeof(struct freetrlr))
#define SEGGRAIN 4096 /* granularity for sbrk requests (in bytes) */
#define ACTIVE    0x1
#define PREACTIVE 0x2
#define testbit(p, b) ((p)&(b))
#define setbits(p, b) ((p)|(b))
#define clearbits(p) ((p)&(~ACTIVE)&(~PREACTIVE))
#define clearbit(p, b) ((p)&~(b))
#define NEXTBLOCK(p) ((struct freehdr *)((char *)p+clearbits(p->Size)))
#define PREVFRONT(p) (((struct freetrlr *)(p)-(sizeof (struct freetrlr)/WORD))->Front)

#define RETADDROFF (6)

#ifndef IDENTIFY

struct hdr { int Size };
struct freehdr {
	int Size;
	struct freehdr *Next, *Prev;
};
struct freetrlr { struct freehdr *Front };
struct segtrlr {
	int Size;
	struct freehdr *Next, *Prev;
	struct freehdr *Front;
};

#else

/* two additional words on every free block identify the caller that created the block
   and it sequence number among all block creations */
struct hdr { 
	char *caller;
	int seqno;
	int Size;
};
struct freehdr {
	char *caller;
	int seqno;
	int Size;
	struct freehdr *Next, *Prev;
};
struct freetrlr { struct freehdr *Front; };
struct segtrlr {
	char *caller;
	int seqno;
	int Size;
	struct freehdr *Next, *Prev;
	struct freehdr *Front;
};

#endif

struct arenastate {
	struct freehdr *arenastart;
	struct freehdr *arenaend;
	struct freehdr *allocp;	/*free list ptr*/
	struct hdr *PendingFree;
	int SeqNo;
	char arenahasbeenreset;
	char InProgress;
	char RecurringM0;
	
};

struct arenastate *GetMallocArena();
char *malloc(), *realloc();
