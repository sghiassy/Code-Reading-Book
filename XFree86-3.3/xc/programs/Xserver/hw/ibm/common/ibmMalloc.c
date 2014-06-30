/*
 * $XConsortium: ibmMalloc.c,v 1.2 91/07/16 13:08:10 jap Exp $
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
 *  malloc.c   memory allocator
 */

/*

	author: WJHansen, CMU/ITC
	(c) Copyright IBM Corporation, 1985, 1986

	Adapted from Algorithms 6.6(b) and 6.7 in
		E. M. Reingold,  W. J. Hansen,
		Data Structures in Pascal, 
		Little, Brown, 1986

	Some details are also taken from the malloc
	distributed with 4.1 BSD.

	The algorithm features a disordered free list which is
	scanned with next-fit.  Allocation scans only the 
	free blocks.  Freeing a block coalesces 
	it with its free neighbors, if any, in time O(1).

	Every block has a one word header (4 bytes) with the
	Size of the block (in bytes, counting header).
	The low bits of size are Active and PreActive bits,
	which indicate whether the block and its predecessor,
	respectively, are free.  A free list block has Next and Prev
	pointers and the last word of a free block is a Front pointer 
	which points to its beginning.   The smallest allocatable
	block is three pointers long.

	The arena may be composed of disjoint segments.  Each
	segment ends with a free block having Active set true.
	These blocks deal with the end condition for the last 
	block of the arena segment.  They also serve to guarantee
	that the free list will never be empty.

April 22, 1986, WJHansen
	reduce botch messages to short strings
	don't use stdio to avoid recursive malloc
		don't test stdout->_base before dumping stats
		always dump headers in addarena if CheckLevel>=5
	implement the PendingFree/FlushFree kludge to allow "free(x);return(x->foo)"
	save pointers to previous blocks in CheckSegment and malloc for debugging
	install InProgress to prevent recursive mallocs (as during a signal handler)
	move arena control to malloc.h; create malloc.h
	check in the free routine to see if the candidate block is within the present arena
	implement NewMallocArena and GetMallocArena
	save caller's return address and a sequence number in each malloc'ed block
	revise tail end of realloc to reduce amount of code
	revise front end or realloc to eliminate the FRxxx stuff 
		it now assumes that if previously freed, the freed block is in A.PendingFree
	eliminate the extra copy of code within malloc and free
	consolidate all testing into TestActive and TestFree

Sept 9, 1985, WJHansen
	Add AbortFullMessage(nbytes)
	add resetmstats()
	add checklevel == 4

*/

#define IDENTIFY

#include <stdio.h>
#include "ibmMalloc.h"
#include "ibmTrace.h"

#ifdef notdef
static char msgbuf[200];
#define ferr4(fmt, a, b, c, d) {sprintf(msgbuf, fmt, a, b, c, d); \
				write(2, msgbuf, strlen(msgbuf));}
#define ferr3(fmt, a, b, c) {sprintf(msgbuf, fmt, a, b, c); \
				write(2, msgbuf, strlen(msgbuf));}
#define ferr2(fmt, a, b) {sprintf(msgbuf, fmt, a, b); \
				write(2, msgbuf, strlen(msgbuf));}
#define ferr1(fmt, a) {sprintf(msgbuf, fmt, a); \
				write(2, msgbuf, strlen(msgbuf));}
#define ferr0(fmt) {sprintf(msgbuf, fmt); \
				write(2, msgbuf, strlen(msgbuf));}
#else
#define ferr4(fmt, a, b, c, d)	{ErrorF(fmt,a,b,c,d);}
#define ferr3(fmt, a, b, c)	{ErrorF(fmt,a,b,c);}
#define ferr2(fmt, a, b)	{ErrorF(fmt,a,b);}
#define ferr1(fmt, a)		{ErrorF(fmt,a);}
#define ferr0(fmt)		{ErrorF(fmt);}
#endif /* notdef */


static CheckLevel;
/* at CheckLevel = 0, no checking is done
	at 1, local reasonability tests are made
	at 2, local tests are made and each free block is
		checked as it is scanned searching
		for a big enough one
	at 3, the full free list is checked on every
		malloc or free
	at 4, a message is printed to stdout 
		for every malloc, free, & realloc
	at 5, mstats will print a complete list 
		of all blocks in memory
*/

#define ASSERT(where,pr) if(!(pr))botch(where);else

static int DefaultM0Handler();   /* (below) */

static int (*M0Handler)() = DefaultM0Handler;
/* the procedure SetM0Handler may be called to set this value
	When malloc runs out of memory, it calls the M0Handler,
	passing it the amount of the malloc request.  (It may also be 
	called from within realloc.)
	If the M0Handler returns, its value is used as the new 
	amount to be allocated, but if zero, malloc just returns zero.
*/

/* A.InProgress is set on entry and cleared before exit.  It prevents a signal handler from 
initiating a malloc or free while one is in progress.  The switch is set and tested with
	if(A.InProgress++) {<error>}
This is foolproof only where ++ to a variable is implemented as a test-and-set instruction;
for instance, on an ibm032 there is a small chance of failure to detect recursive malloc 
if the signal handler is invoked in the middle of the instructions that perform the ++
*/

#ifdef MSTATS
    int Nscans, Nrequests, Nsbrk, Ncombines, Nfrees;
#endif

/*	avoid break bug */
#ifdef pdp11
#define GRANULE 64
#else
#define GRANULE 0
#endif

static struct arenastate A = {0, 0, 0, 0, 0, 0, 0, 0};

char	*sbrk();
static struct freehdr *addarena ();

/* More kludgery:  Some programs assume that freed blocks can still be
accessed for a while.  This fails because the contents of some words of freed
blocks are overlaid with the chain pointers for the free list.  To simulate
the expected behavior, this package does not actually perform free until the
next operation to the package.  A pointer to the block to be freed is saved
in PendingFree.  The actual free routine is called FlushFree.  The routines
malloc, free, and realloc all begin with  
	if (A.PendingFree) FlushFree(A.PendingFree);
*/




/*
	malloc

	Pascal version from Data Structures in Pascal
type
    block = record
        Size: integer;           {size of this record}
        Active: boolean;        {true when this record is in use}
        PreActive: boolean;     {true when preceding record is in use}
        case blockType of
            freeBlock:(Next, Prev: ptr;    {link free list}
                    {" . . . "}
                    Front: ptr);  {last word points to start of record}
            activeBlock: ({"contents, as required"} )
    end;
    ptr = ^block;
var 
    free: ptr;

function malloc(n: integer): ptr;
var
    p: ptr;
begin
    p := free;
    {scan free list looking for a large enough record}
    while (p<>free^.Prev) and (p^.Size<n) do
        {Assert:  No record on list from (free) to (p)
                has size (n) or larger.}
        p := p^.Next;
    if p^.Size < n then
        sbrk(rounded up number of bytes);
    if p^.Size < n then
        malloc := null;
    else begin
        free := p^.Next;    {set (free) for Next allocation}
        if p^.Size - n < epsilon then begin
            {allocate entire record; remove it from free list}
            p^.Next^.Prev := p^.Prev;
            p^.Prev^.Next := p^.Next
        end
        else begin
            {split (p); allocate the right and free the left}
            p^.Size := p^.Size - n;
            {"(Front) pointer at end of (p)^"} := p;
            p := {"(p)+(p)^.(Size)"};
            p^.Size := n;
            p^.PreActive := false
        end;
        {(p)^.(Size)>=(n) and a record of length (p)^.(Size) 
        is allocated, beginning at (p)}
        p^.Active := true;
        {"((p)+(p)^.(Size))"}^.PreActive := true;
        malloc := p    {return a pointer to the new record}
    end
end;

*/

char *
Xalloc(nbytes)
unsigned nbytes;
{
	register siz;	/* # bytes to allocate */
	register struct freehdr *p, *t;
        char *tptr;

	
	tptr = (malloc (nbytes));
        return (tptr);

	if (A.InProgress++) {
		botch("Program error.  Tried 'malloc' while in malloc package.");
		A.InProgress--;
		return 0;
	}
	if (A.PendingFree) FlushFree(A.PendingFree);

#ifdef MSTATS
Nrequests++;
#endif
	if (A.allocp==0) 	{  /*first time*/
		/* start with 20K (rounded up in addarena) */
		A.allocp = addarena(1<<14);  
		if (A.allocp==0) 
			DefaultM0Handler(1<<14); 
	}

	if (CheckLevel > 0) {
		if (CheckLevel>=3) CheckAllocs("in Malloc");
		else ASSERT("m0", TestFree(A.allocp));
	}

	siz = (nbytes+sizeof(struct hdr)+(WORD-1))/WORD*WORD;
	if (siz<EPSILON) siz = EPSILON;
	t = A.allocp->Prev;

	/* scan free list looking for a large enough record */
	/* as an optimization we skip the clearbits() around p->Size,
		assuming that ACTIVE and PREACTIVE are in the
		low bits so they do not affect the size comparison */
	if (CheckLevel!=2)  for (p=A.allocp; 
#ifdef MSTATS
Nscans++,
#endif
			p!=t && p->Size<siz; 
			p = p->Next) {

	}
	else    for (p=A.allocp;      /* CheckLevel == 2 */
#ifdef MSTATS
Nscans++,
#endif
			p!=t && p->Size<siz; 
			p = p->Next) {
		if (clearbits(p->Size)) {
			ASSERT ("m1", ! testbit(p->Size,ACTIVE));
			ASSERT ("m2", p == PREVFRONT(NEXTBLOCK(p)));
		}
		else 
			ASSERT ("m3", testbit(p->Size, ACTIVE));
		ASSERT("m4", p->Next->Prev==p);
	}
	if (p->Size<siz) {
		p = addarena(siz);
		if (p==NULL) {
			if (A.RecurringM0)
				DefaultM0Handler(nbytes);
			else {
				char *v;
				int (*h)() = M0Handler;
				int newsize;
				A.InProgress--;
				newsize = (*M0Handler)(nbytes);
				/* if newsize still too big and no new
					M0Handler is set, a failure
					to allocate newsize is an abort
				*/
				if (h==M0Handler)
					A.RecurringM0 ++;
				v = (newsize) ? malloc(newsize) : NULL;
				A.RecurringM0 = 0;
#ifdef IDENTIFY
				if (v) {
					struct hdr *t = ((struct hdr *)v)-1;
					t->caller = *(((char **)&nbytes) - RETADDROFF);
					t->seqno = A.SeqNo++;
				}
#endif
				return v;
			}
		}
	}

	if (CheckLevel > 0)  
		ASSERT("m10", TestFree(p));

	A.allocp = p->Next;
	if (clearbits(p->Size)-siz < EPSILON) {
		/*allocate entire record; remove it from free list*/
		p->Next->Prev = p->Prev;
		p->Prev->Next = p->Next;
		p->Size |= ACTIVE;
	}
	else {
		/* split (p); allocate the right and free the left*/
		p->Size -= siz;   /* doesn't change bits */
		t = NEXTBLOCK(p);
		PREVFRONT(t) = p;
		p = t;
		p->Size = siz | ACTIVE;
	}
	/* Some programs have code that assumes that newly 
		malloc'ed memory is zero (Heavy SIGH): */
	p->Next = 0;
	p->Prev = 0;
	t = NEXTBLOCK(p);
	PREVFRONT(t) = 0;

	t->Size |= PREACTIVE;

	A.InProgress --;

#ifdef IDENTIFY
	p->caller = *(((char **)&nbytes) - RETADDROFF);
	p->seqno = A.SeqNo++;
#endif
	if (CheckLevel >=4) {
		int *ap = (int *)&nbytes;
		ferr3("malloc @ 0x%lx for %d bytes returns 0x%lx\n",
			*(ap-RETADDROFF), *ap, 
			((char *)p) + sizeof(struct hdr));
	}

	return ((char *)p) + sizeof(struct hdr);
}

/* addarena */
/* create a new or extended arena.  Two adjacent arenas will coallesce. */
/* In a new arena segment, The first three words are a freehdr with
	Size indicating all of block except last four words;  its Active
	bit is false and its PreActive bit is true (so no coalesce off front
	will be tried);  Next and Prev both point to a dummy free element
	in last four words.  The dummy in the last four words of the segment
	has Active true (so preceding block will not coalesce with it) and
	PreActive set depending on the preceding block (initially false);
	the Size field is zero; Next and Prev both point to the free
	element at the beginning of the segment.  The Front field
	in the last word of the segment points not to the dummy
	free element at the end, but to the beginning of the segment
	(so CheckAllocs can find segment.)

	The argument min gives the space needed.
	Return value is NULL or a pointer to a big enough block.
*/
static
struct freehdr *
addarena (min) {
	register struct freehdr *new, *trlr;
	struct freehdr *t;
	int segbytes = ((min+2*EPSILON+(SEGGRAIN-1)) 
			/ SEGGRAIN) * SEGGRAIN;
	int x;
#ifdef pdp11
	new = (struct freehdr *)sbrk(0);
	if((INT)new+segbytes+GRANULE < (INT)new) {
		return(NULL);
	}
#endif
#ifdef MSTATS
Nsbrk++;
#endif
	new = (struct freehdr *)sbrk(segbytes);
	if((INT)new == -1) 
		return(NULL);
	if ((x=(INT)new % WORD)) {
		new = (struct freehdr *)((INT)new+WORD-x);
		segbytes -= WORD; 
	}
	trlr = (struct freehdr *)((INT)new+segbytes-EPSILON);
	new->Size = setbits(segbytes - EPSILON, PREACTIVE);
	new->Next = trlr;
	trlr->Size = setbits(0, ACTIVE);
	trlr->Prev = new;
	PREVFRONT(trlr) = new;
	/* trlr is the dummy block at the end of the arena
		make its Front field point to arenastart */
	PREVFRONT(((char *)trlr)+EPSILON) = new;
	
	if (A.arenaend) {
		/* attach new arena into old free list */
		new->Prev = A.arenaend;
		trlr->Next = A.arenaend->Next;
		A.arenaend->Next->Prev = trlr;
		A.arenaend->Next = new;
	}
	else {
		A.arenastart = new;
		trlr->Next = new;
		new->Prev = trlr;
	}
	if (new == (struct freehdr *)(((char *)A.arenaend)+EPSILON)) {
		/* coallesce adjacent arenas */
		PREVFRONT(((char *)trlr)+EPSILON) =
			PREVFRONT(((char *)A.arenaend)+EPSILON);
		t = A.arenaend;
		t->Size = setbits(EPSILON, 
				testbit(t->Size, PREACTIVE)
				| ACTIVE);
		t->Next->Prev = t->Prev;
		t->Prev->Next = t->Next;
		if (A.allocp==t)
			A.allocp = t->Next;
		A.arenaend = trlr;
		FlushFree ((char *)t+sizeof (struct hdr));
	}
	else
		A.arenaend = trlr;
	if (CheckLevel>=5) 
		printarena("addarena");
	return (PREVFRONT(A.arenaend));
}


/*
	free

	Pascal version from Data Structures in Pascal

procedure free(p: ptr);
var
    t: ptr;
begin
    t := {"(p)+(p)^.(Size)"};   {(t)^ is the next record in (word) after (p)^}
    if not t^.Active then begin   {(t)^ is free}
        {merge (p)^ and (t)^, removing (t)^ from free list}
        t^.Next^.Prev := t^.Prev;
        t^.Prev^.Next := t^.Next;
        if t = free then
            free := t^.Prev;
        p^.Size := p^.Size + t^.Size;
    end
    else t^.PreActive := false;
    if not p^.PreActive then begin
        {merge (p)^ with physically preceding record, which is already
        on the free list}
        t := {"((p)-1)"}^.Front;
        t^.Size := t^.Size + p^.Size;
        {"(Front) pointer at end of (t)^"} := t;
    end
    else begin
        {put (p) on free list}
        p^.Active := false;
        {"(Front) pointer at end of (p)^"} := p;
        p^.Prev := free^.Prev;
        p^.Next := free;
        free^.Prev := p;
        p^.Prev^.Next := p
    end
end
*/


Xfree(ap)
struct hdr *ap;
{
	return (free (ap));
	if (!ap) return;
	if (CheckLevel >= 4) {
		int *app = (int *)&ap;
		ferr3("free @ 0x%lx for block of %d bytes at 0x%lx enqueued\n",
			*(app-RETADDROFF), 
			clearbits(((struct freehdr *)(((char *)ap) - sizeof(struct hdr)))->Size), 
			*app);
	}
	if (A.InProgress++) {
		botch ("Program error.  Tried 'free' while in malloc package.");
	}
	if (A.PendingFree) FlushFree(A.PendingFree);
	A.PendingFree = ap;
	A.InProgress -- ;
}

static
FlushFree(ap)
struct hdr *ap;
{
	register struct freehdr *p = (struct freehdr *)(((char *)ap) - sizeof(struct hdr));
	register struct freehdr *t = NEXTBLOCK(p);
	register int siz = clearbits(p->Size);  /* retain p->Size */
	int preact = testbit(p->Size, PREACTIVE); /* retain p->PREACTIVE */

	A.PendingFree = 0;

	if (p < A.arenastart || t > A.arenaend) {
		if ( ! A.arenahasbeenreset) 
			botch("Program error.  Free non-malloc'ed block.");
		return;
	}

#ifdef MSTATS
Nfrees++;
#endif

	if (CheckLevel > 0) {
		register struct freehdr *f = PREVFRONT(p);
		ASSERT("f0", TestActive(p));
		if (CheckLevel >= 3) CheckAllocs("in free");
		else {
			ASSERT("f1", TestFree(A.allocp));
			if (!testbit(t->Size, ACTIVE)) 
				ASSERT("f2", TestFree(t));
			else ASSERT("f3", TestActive(t));
			if (!preact) 
				ASSERT("f4", TestFree(f));
			else ASSERT("f5", TestActive(f));
		}
	}

	if (!testbit(t->Size, ACTIVE)) {  
		/* coalesce *p and *t */
#ifdef MSTATS
Ncombines++;
#endif
		t->Next->Prev = t->Prev;
		t->Prev->Next = t->Next;
		if (t==A.allocp) 
			A.allocp = t->Next;
		siz += clearbits(t->Size);
	}
	else 
		t->Size = clearbit(t->Size, PREACTIVE);

	if (!preact) {
		/* merge *p with preceding free block */
#ifdef MSTATS
Ncombines++;
#endif
		t = PREVFRONT(p);
		preact = testbit(t->Size, PREACTIVE);
		siz += clearbits(t->Size);
		p->Size = 0;  /* mark not active */
		p = t;
	}
	else {
		/* put *p on the free list */
		p->Prev = A.allocp->Prev;
		p->Next = A.allocp;
		p->Prev->Next = A.allocp->Prev = p;
	}

	p->Size = setbits(siz, preact);
	PREVFRONT((char *)p + siz) = p;
}

/*	realloc(p, nbytes) reallocates a block obtained from malloc()
 *	and (possibly) freed since last call of malloc()
 *	to have new size nbytes, and old content
 *	returns new location, or 0 on failure
 */

char *
Xrealloc(ap, nbytes)
struct hdr *ap;			/* block to realloc */
unsigned nbytes;			/* desired size */
{
	register struct freehdr *h;
	struct freehdr *f;
	unsigned siz;		/* total size needed */
	unsigned nw;		/* desired number of words */
	register unsigned onw;	/* existing number of words */
	char *msg;		/* reason for failure */

        char *tptr;

	
        free(ap);
	tptr = (realloc (ap, nbytes));
        return (tptr);

/*	return (realloc (ap, nbytes)); */
	if (!ap) {
	    ap= (struct hdr *)Xalloc(nbytes);
	}

	h= (struct freehdr *)(((char *)ap) - sizeof(struct hdr));
	f= NEXTBLOCK(h);

	if (A.InProgress++) {
		botch ("Program error.  Tried 'realloc' while in malloc package.");
		A.InProgress--;
		return 0;
	}
	if (CheckLevel>=4) {
		int *app = (int *)&ap;
		ferr3 ("realloc @ 0x%lx changes size %d at 0x%lx . . .\n",
			*(app-RETADDROFF), clearbits(h->Size), ap);
	}
	if (A.PendingFree) {
		if (A.PendingFree == ap) 
			A.PendingFree = NULL;
		else FlushFree(A.PendingFree);	
	}

	if (! TestActive(h)) {
		msg = "rx1";
	nope:	if (CheckLevel>=4)
			ferr1(". . . fails at %s\n", msg);
		A.InProgress--;
		return NULL;
	}
	if (CheckLevel > 0) {
		if (CheckLevel>=3) CheckAllocs("in realloc");
		if (!testbit(f->Size, ACTIVE))
			ASSERT("r0", TestFree(f));
		ASSERT("r1", TestFree(A.allocp));
	}
	siz = (nbytes+sizeof(struct hdr)+(WORD-1))/WORD*WORD;
	if (siz<EPSILON) siz = EPSILON;
	if (!testbit(f->Size, ACTIVE) 
			&& clearbits(h->Size)+clearbits(f->Size) >= siz) {
		/* combine active block h with following free block f */
		f->Next->Prev = f->Prev;
		f->Prev->Next = f->Next;
		if (f==A.allocp) 
			A.allocp = f->Next;
		h->Size += clearbits(f->Size);   /* don't change bits in h->Size */
		f = NEXTBLOCK(h);
		f->Size = setbits(f->Size, PREACTIVE);
	}

	/* the remainder does not affect the free list or arena, 
		so InProgress protection is no longer needed 
		(though the assigns to PendingFree may cause
		a free operation to be skipped )      */
	A.InProgress --;

	nw = (siz - sizeof(struct hdr))/WORD;
	onw = (clearbits(h->Size)-sizeof(struct hdr))/WORD;
	if (nw<=onw) {
		/* is big enough;  can we release part? */
		if (onw-nw>EPSILON/WORD) {
			h->Size = setbits(siz, 
				ACTIVE | testbit(h->Size, PREACTIVE));
			f = NEXTBLOCK(h);
			f->Size = setbits((onw-nw)*WORD,
					ACTIVE | PREACTIVE);
			A.PendingFree = ((struct hdr *)f) + 1;
		}
	}
	else {
		/* malloc a new one and copy */
		register INT *s, *t;
		s = (INT *)ap;
		ap = (struct hdr *)malloc(nbytes);   /* ap pts to data, not hdr */
		if (ap==NULL)
			{msg = "rx5"; goto nope;}
		A.PendingFree = (struct hdr *)s;
		t = (INT *)ap;
		while(onw-->0)
			*t++ = *s++;
	}
	if (CheckLevel >= 4) 
		ferr2 (". . . to size %d at 0x%lx\n", nbytes, ap);
#ifdef IDENTIFY
	(ap-1)->caller = *(((char **)&ap) - RETADDROFF);
	(ap-1)->seqno = A.SeqNo++;
#endif
	return((char *)ap);
}

/*
char	*
malloc(nbytes)
int nbytes;
{
   return(Xalloc(nbytes));
} */

/*
free(ptr)
char *ptr;
{
   return(Xfree((struct hdr *)ptr));
} */

/*
char *
realloc(ptr,nbytes)
char  *ptr;
int    nbytes;
{
   return(Xrealloc((struct hdr *)ptr,nbytes));
}*/

    static int
TestFree (f)
    register struct freehdr *f;
{
	if (testbit(f->Size,ACTIVE)) {
		/* had better be a segment trailer */
		register struct freehdr *t = ((struct segtrlr *)f)->Front;
		return (f->Next->Prev == f
			&& f->Prev->Next == f
			&& testbit(t->Size, PREACTIVE)
			&& clearbits(f->Size) == 0
			&& t < f
			&& f <= A.arenaend
			&& t >= A.arenastart);
	}
	else {
		/* test as a regular free block */
		register struct freehdr *t = NEXTBLOCK(f);
		return (f->Next->Prev == f
			&& f->Prev->Next == f
			&& testbit(f->Size, PREACTIVE)
			&& f == PREVFRONT(t)
			&& ! testbit(t->Size, PREACTIVE)
			&& f >= A.arenastart
			&& t <= A.arenaend
			&& f->Size >= EPSILON);
	}
}
    static int
TestActive (f) 
    register struct freehdr *f;
{
	register struct freehdr *t = NEXTBLOCK(f);
	return (testbit(f->Size, ACTIVE)
		&& f >= A.arenastart
		&& t <= A.arenaend
		&& testbit(t->Size, PREACTIVE)
		&& f->Size > EPSILON);
}


    static
printarena (m)
    char *m;
{
	int *x;
	int i;
	ferr4("%s:  arenastart %lx  arenaend %lx  allocp %lx\narena starts with", 
			m, A.arenastart, A.arenaend, A.allocp);
	for (x = (int *)A.arenastart, i=0; i<EPSILON/WORD; i++)
		ferr1("   %lx", *(x+i));
	ferr0 ("\nand ends with");
	for (x = (int *)A.arenaend, i = -1; i<EPSILON/WORD; i++)
		ferr1("   %lx", *(x+i));
	ferr0("\n");
}

#ifdef MSTATS
int Nactive, Nfree, Nsegs;
long int TotActive, TotFree;
static DumpBlocks = 0;
#endif

CheckAllocs(m) char *m;
{
	int nfree=0;
	register struct freehdr *t;
	if (A.allocp==0)  {	/*first time, use code from inside malloc */ 
		/* start with 20K (rounded up in addarena) */
		A.allocp = addarena(1<<14);  
		if (A.allocp==0) 
			DefaultM0Handler(1<<14); 
	}

#ifdef MSTATS
	Nactive = Nfree = Nsegs = 0;
	TotActive = TotFree = 0;
	if (DumpBlocks) {
		struct hdr *x;
		register struct freehdr *a, *olda;
		int i;
		printarena(m);
#ifndef IDENTIFY
		ferr0 ("\n    Front;       loc:      Size      Next      Prev\n");
#else
		ferr0 ("\n    Front;       loc:      Size      Caller   Seqno      Next      Prev\n");
#endif
		for (a = A.arenastart, olda = 0; 
				a>olda && a < A.arenaend; 
				a = (struct freehdr *)((char *)(olda=a) + clearbits(a->Size)) ) {
			ferr3("   %10lx; %10lx: %10lx ", 
				(((struct freetrlr *)a)-1)->Front, a,a->Size);
#ifdef IDENTIFY
			ferr2("%10lx %10lx", a->caller, a->seqno);
#endif
			ferr2("%10lx %10lx\n", a->Next, a->Prev);
		}
		if (a<=olda) ferr1 ("Illegal pointer %lx\n", a);
	}
#endif
	t = A.allocp;
	do {
		nfree++;
		ASSERT("c1", t->Next->Prev==t);
		if (testbit(t->Size, ACTIVE)) {
			/* this is a segment trlr */
			struct freetrlr *f 
				= (struct freetrlr *)(t+1);
			ASSERT("c2", clearbits(t->Size)==0);
			/* Segment must be a multiple
				of SEGGRAIN bytes: */
			ASSERT("c3", ( (int)(f+1)-(int)(f->Front) )	/* \ */				%SEGGRAIN==0);
			nfree -= (CheckSegment(m, f->Front, t));
		}
		else 
			ASSERT("c4", t==PREVFRONT(NEXTBLOCK(t)));
		t = t->Next;
	} while (t!=A.allocp);
	ASSERT("c5", nfree==0);
#ifdef MSTATS
return(TotFree+1);   /* an application may want to know this */
#else
	return(1);
#endif	 
}
static CheckSegment(m, f, t) 
char *m;  
register struct freehdr *f, *t; 
{
	register int nfree = 1;  /* count the trailer now */
	register int wasactive = PREACTIVE;  /* value in first block */
	register struct freehdr *pref=0;  /* for debugging */
#ifdef MSTATS
Nsegs++;
#endif
	for (; f<t; pref = f, 
			f = (struct freehdr *)((char *)f 
				+ clearbits(f->Size))) {
		ASSERT("s1", clearbits(f->Size)>0);
		ASSERT("s2", wasactive==testbit(f->Size, PREACTIVE));
		if (!testbit(f->Size, ACTIVE)) 
			nfree++, wasactive=0;
		else 
			wasactive = PREACTIVE;
#ifdef MSTATS
		if (!testbit(f->Size, ACTIVE)) {
			Nfree++;
			TotFree += clearbits(f->Size);
		}
		else {
			Nactive++;
			TotActive += clearbits(f->Size);
		}
#endif
	}
	ASSERT("s3", f==t);
	return nfree;
}

static
botch(s)
char *s;
{
/* don't want to use fprintf: it calls malloc! */
	char *msg;
	msg = "Malloc arena corruption discovered  at - "; 
	if (stdout->_base) fflush(stdout);
	if (stderr->_base) fflush(stderr);
	write(2, msg, strlen(msg));
	if (s) write(2, s, strlen(s));
	write(2, "\n", 1);
	abort();
}


#ifdef MSTATS
mstats (m) char *m; {
	double TotSpace;
	long int TotHdrs;
	int ActHdrs;

	ferr1("\nmstats - %s\n", m);

	if (CheckLevel>=5) DumpBlocks=1;
	CheckAllocs("");  /* so Nsegs!=0 */
	DumpBlocks = 0;
	TotSpace = TotActive + TotFree + Nsegs*EPSILON;
	ActHdrs = Nactive*sizeof(struct hdr);
	TotHdrs = (TotActive ? TotFree * ActHdrs / TotActive : EPSILON);
	TotFree -= TotHdrs;
	TotActive -= ActHdrs;
	TotHdrs += ActHdrs + Nsegs*EPSILON;


	ferr4 ("%-10s%10s%10s%10s\n", "", "Active", "Free", "Segments");
	ferr4 ("%-10s%10d%10d%10d\n", "requests", Nrequests, Nfrees, Nsbrk);
	ferr4 ("%-10s%10d%10d%10d\n", "current", Nactive, Nfree, Nsegs);
	ferr4 ("%-10s%10d%10d%10d\n",
		"avg size", (Nactive ? TotActive/Nactive : 0), 
		(Nfree ? TotFree/Nfree : 0), 
		(int)TotSpace/Nsegs);
	ferr3 ("%-10s%10.1f%10.3f\n",
		"avg ops", 
		(Nrequests ? ((float)Nscans)/Nrequests : 0.0),
		(Nfrees ? ((float)Ncombines)/Nfrees : 0.0)); 
	ferr3 ("%-10s%9.1f%%%9.1f%%\n",
		"% space", 
		100*TotActive/TotSpace,
		100*TotFree/TotSpace);
	ferr4 ("%s %d   %s%5.1f%%\n\n",
		"total space", (int)TotSpace, 
		"headers", 100*TotHdrs/TotSpace);
}
resetmstats() {
    Nscans = Nrequests = Nsbrk = Ncombines = Nfrees = 0;
}
#else
mstats() {}
resetmstats() {}
#endif



#include <sys/types.h>
#include <sys/time.h>

AbortFullMessage (nbytes) 
register unsigned int nbytes; 
{
	register siz, segbytes;
	char buf[200];
	char *maxBrk = (char *) ulimit( 3, 0 ) ;
	char *oldBrk ;

	siz = (nbytes+sizeof(struct hdr)+(WORD-1))/WORD*WORD;
	if (siz<EPSILON) siz = EPSILON;
	segbytes = ((siz+2*EPSILON+(SEGGRAIN-1)) 
			/ SEGGRAIN) * SEGGRAIN;

	if ((int) (oldBrk = (char *) sbrk(0))+segbytes > maxBrk )
	    sprintf(buf, "Malloc abort.  Attempt to allocate %d bytes caused data segment to exceed its maximum of %d bytes.\n",
			nbytes, maxBrk - oldBrk );
	if (stdout->_base) fflush(stdout);
	if (stderr->_base) fflush(stderr);
	write(2, buf, strlen(buf));
	return ;
}

int (*
SetM0Handler (p))() int (*p)(); {
    int (*t)() = M0Handler;
    M0Handler = (p==0) ? DefaultM0Handler : p;
    return t;
}

static int DefaultM0Handler (size) {
	AbortFullMessage(size);
	return (0);
}


SetMallocCheckLevel (level) int level; {
	int old = CheckLevel;
	CheckLevel = level;
	return old;
}

NewMallocArena() {
	A.arenastart = A.arenaend = A.allocp = 0;
	A.PendingFree = 0;
	A.InProgress = A.RecurringM0 = 0;
	A.arenahasbeenreset++;
	/* at present, it does not reset seqno */
}

    struct arenastate *
GetMallocArena () 
{
	return (&A);
}
