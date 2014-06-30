/* $XConsortium: get_tv.c,v 1.1 93/08/08 12:58:41 rws Exp $ */

/*

Copyright (c) 1986, 1987 by Hewlett-Packard Company

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name of Hewlett Packard not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.

*/

/*
 * An alternative to gettimeofday(2) that is about 50 times faster
 * if used frequently.  HP-PA specific.
 *                     Dave Holt,  GSY Performance, dah@cup.hp.com
 */

#include <time.h>
#include <assert.h>

#ifdef NOTDEFINED /* The following is not currently used.  But 	
		   * I will leave it for possible future use.
		   * --kam 4/22/92
		   */

#ifdef DEBUG
#include <stdio.h>
#else /* Not DEBUG.  Don't bother with printf's. */
#define printf(s, a)
#endif /* DEBUG */

#define MILLION 1000000
#define BUNCH 8192		/* must be < 32767 */

unsigned int cr16();		/* in cr16.s (assembly code) */
static struct timeval slow_get_tv();
void *malloc();
static set_tix2usec();

/* This initial value for last_tv forces initialization of tix2usec[]. */
static struct timeval last_tv = {0, 2000000};
static unsigned int last_tix;

/*
 * tix2usec[BUNCH] is a table built by set_tix2usec() which converts
 * from CR16 ticks to microseconds:
 *     microseconds = tix2usec[ticks >> shift]
 */
static unsigned short *tix2usec;
static int shift;

struct timeval get_tv()		/* the fast path */
{
    unsigned int tix, delta_tix;
    long delta_usec;

    tix = cr16();
    delta_tix = tix - last_tix;
    last_tix = tix;
    if (delta_tix < BUNCH) {	/* we can use the array */
	delta_usec = tix2usec[delta_tix >> shift];
	last_tv.tv_usec += delta_usec;
	if (last_tv.tv_usec < MILLION) {
	    return(last_tv);
	}
    }
    return(slow_get_tv());
}

static struct timeval slow_get_tv()	/* the slow path */
{
    struct timezone tz;
    static int first_time = 1;

    if (first_time) {
	set_tix2usec();
	first_time = 0;
    }
    last_tix = cr16();
    gettimeofday(&last_tv, &tz);
    return(last_tv);
}

/* 
 * set_tix2usec() builds it by observing gettimeofday and CR16 over a
 * reasonable period (~50ms).
 */
static set_tix2usec()
{
    struct timeval tv1, tv2;
    struct timezone tz;
    int i, tix_per_entry, delta_us;
    unsigned int tix1, tix2, cr16(), delta_tix;
    double tix_per_usec, usec_per_tix;

    /* make sure the code is in memory before we time it */
    tix1 = cr16();
    gettimeofday(&tv1, &tz);

    do {
	tix1 = cr16();
	gettimeofday(&tv1, &tz);
	do {
	    tix2 = cr16();
	    gettimeofday(&tv2, &tz);
	    delta_us = (tv2.tv_sec - tv1.tv_sec) * MILLION +
	      (tv2.tv_usec - tv1.tv_usec);
	    delta_tix = tix2 - tix1;
	} while (delta_us < 50000); /* loop for at least 50ms */
    } while (tv2.tv_sec - tv1.tv_sec > 2); /* retry if delta is too big */

    tix_per_usec = 1.0 * delta_tix / delta_us;
    usec_per_tix = 1 / tix_per_usec;

    /* Table should convert at least 10ms deltas. */
    tix_per_entry = tix_per_usec * 10000 / BUNCH;
    for (shift = 0;
	 (1 << shift) < tix_per_entry;
	 shift++) {
    }

    tix2usec = (unsigned short *) malloc(BUNCH * sizeof(unsigned short));
    assert(tix2usec != NULL);

    for (i = 0; i < BUNCH; i++) {
	tix2usec[i] = i * (1 << shift) * usec_per_tix;
    }

    printf("tix_per_usec = %lf\n", tix_per_usec);
    printf("shift = %d\n", shift);
}

#endif /* NOTDEFINED */


void
calibrate_cr16(TicksPerMilli)

unsigned long *TicksPerMilli;

{

    struct timeval start, end;
    register unsigned long cr_start, cr_end;
    unsigned long total_ticks = 0, total_millis = 0;
    int i, j;

    for(i = 0; i < 6; i++)
    {
        cr_start = cr16();
        gettimeofday(&start, NULL);

	/* waste some time */
	for(j=0; j< 200000; j++)
		;
	cr_end = cr16();
	gettimeofday(&end, NULL);

	if(start.tv_usec > end.tv_usec)
	{
	    end.tv_usec += 1000000;
	    end.tv_sec--;
	}

	/* if we have a rollover during all of this, toss this one. */

	if(cr_end < cr_start)
	{
	    i--;
	}
	else
	{
	    total_millis += (end.tv_sec - start.tv_sec) * 1000 +
			    (end.tv_usec - start.tv_usec) / 1000;
	    total_ticks += cr_end - cr_start;
	}
    }

    *TicksPerMilli = total_ticks/total_millis;

}
