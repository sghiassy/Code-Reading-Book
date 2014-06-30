/* $XConsortium: genauth.c,v 1.23 95/07/10 21:18:07 gildea Exp $ */
/* $XFree86: xc/programs/xdm/genauth.c,v 3.4 1996/01/24 22:04:37 dawes Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 */

# include   <X11/Xauth.h>
# include   <X11/Xos.h>
# include   "dm.h"

#include <errno.h>

#ifdef X_NOT_STDC_ENV
#define Time_t long
extern Time_t time ();
extern int errno;
#else
#include <time.h>
#define Time_t time_t
#endif

static unsigned char	key[8];

#ifdef HASXDMAUTH

typedef unsigned char auth_cblock[8];	/* block size */

typedef struct auth_ks_struct { auth_cblock _; } auth_wrapper_schedule[16];

extern void _XdmcpWrapperToOddParity();

static
longtochars (l, c)
    long	    l;
    unsigned char    *c;
{
    c[0] = (l >> 24) & 0xff;
    c[1] = (l >> 16) & 0xff;
    c[2] = (l >> 8) & 0xff;
    c[3] = l & 0xff;
}


# define FILE_LIMIT	1024	/* no more than this many buffers */

static
sumFile (name, sum)
char	*name;
long	sum[2];
{
    long    buf[1024*2];
    int	    cnt;
    int	    fd;
    int	    loops;
    int	    reads;
    int	    i;
    int     ret_status = 0;

    fd = open (name, 0);
    if (fd < 0) {
	LogError("Cannot open randomFile \"%s\", errno = %d\n", name, errno);
	return 0;
    }
#ifdef FRAGILE_DEV_MEM
    if (strcmp(name, "/dev/mem") == 0) lseek (fd, (off_t) 0x100000, SEEK_SET);
#endif
    reads = FILE_LIMIT;
    sum[0] = 0;
    sum[1] = 0;
    while ((cnt = read (fd, (char *) buf, sizeof (buf))) > 0 && --reads > 0) {
	loops = cnt / (2 * sizeof (long));
	for (i = 0; i < loops; i+= 2) {
	    sum[0] += buf[i];
	    sum[1] += buf[i+1];
	    ret_status = 1;
	}
    }
    if (cnt < 0)
	LogError("Cannot read randomFile \"%s\", errno = %d\n", name, errno);
    close (fd);
    return ret_status;
}

static
InitXdmcpWrapper ()
{
    long	    sum[2];
    unsigned char   tmpkey[8];

    if (!sumFile (randomFile, sum)) {
	sum[0] = time ((Time_t *) 0);
	sum[1] = time ((Time_t *) 0);
    }
    longtochars (sum[0], tmpkey+0);
    longtochars (sum[1], tmpkey+4);
    tmpkey[0] = 0;
    _XdmcpWrapperToOddParity (tmpkey, key);
}

#endif

#ifndef HASXDMAUTH
/* A random number generator that is more unpredictable
   than that shipped with some systems.
   This code is taken from the C standard. */

static unsigned long int next = 1;

static int
xdm_rand()
{
    next = next * 1103515245 + 12345;
    return (unsigned int)(next/65536) % 32768;
}

static void
xdm_srand(seed)
    unsigned int seed;
{
    next = seed;
}
#endif /* no HASXDMAUTH */

GenerateAuthData (auth, len)
char	*auth;
int	len;
{
    long	    ldata[2];

#ifdef ITIMER_REAL
    {
	struct timeval  now;

	X_GETTIMEOFDAY (&now);
	ldata[0] = now.tv_usec;
	ldata[1] = now.tv_sec;
    }
#else
    {
#ifndef __EMX__
	long    time ();
#endif

	ldata[0] = time ((long *) 0);
	ldata[1] = getpid ();
    }
#endif
#ifdef HASXDMAUTH
    {
    	int		    bit;
    	int		    i;
	auth_wrapper_schedule    schedule;
	unsigned char	    data[8];
	static int	    xdmcpAuthInited;
    
	longtochars (ldata[0], data+0);
	longtochars (ldata[1], data+4);
	if (!xdmcpAuthInited)
	{
	    InitXdmcpWrapper ();
	    xdmcpAuthInited = 1;
	}
	_XdmcpAuthSetup (key, schedule);
    	for (i = 0; i < len; i++) {
	    auth[i] = 0;
	    for (bit = 1; bit < 256; bit <<= 1) {
	    	_XdmcpAuthDoIt (data, data, schedule, 1);
	    	if (data[0] + data[1] & 0x4)
		    auth[i] |= bit;
	    }
    	}
    }
#else
    {
    	int	    seed;
    	int	    value;
    	int	    i;
    
    	seed = (ldata[0]) + (ldata[1] << 16);
    	xdm_srand (seed);
    	for (i = 0; i < len; i++)
    	{
	    value = xdm_rand ();
	    auth[i] = (value & 0xff00) >> 8;
    	}
	value = len;
	if (value > sizeof (key))
	    value = sizeof (key);
    	memmove( (char *) key, auth, value);
    }
#endif
}
