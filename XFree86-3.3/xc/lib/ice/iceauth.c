/* $XConsortium: iceauth.c,v 1.21 94/12/20 16:49:58 mor Exp $ */
/* $XFree86: xc/lib/ICE/iceauth.c,v 3.1 1995/01/27 04:44:52 dawes Exp $ */
/******************************************************************************


Copyright (c) 1993  X Consortium

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

Author: Ralph Mor, X Consortium
******************************************************************************/

#include <X11/ICE/ICElib.h>
#include "ICElibint.h"
#include <X11/ICE/ICEutil.h>

#if defined(X_NOT_STDC_ENV) && !defined(__EMX__)
#define Time_t long
extern Time_t time ();
#else
#include <time.h>
#define Time_t time_t
#endif

static int binaryEqual ();

static int was_called_state;

/*
 * MIT-MAGIC-COOKIE-1 is a sample authentication method implemented by
 * the SI.  It is not part of standard ICElib.
 */


char *
IceGenerateMagicCookie (len)

int len;

{
    char    *auth;
    long    ldata[2];
    int	    seed;
    int	    value;
    int	    i;
    
    if ((auth = (char *) malloc (len + 1)) == NULL)
	return (NULL);

#ifdef ITIMER_REAL
    {
	struct timeval  now;
	X_GETTIMEOFDAY (&now);
	ldata[0] = now.tv_sec;
	ldata[1] = now.tv_usec;
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
    seed = (ldata[0]) + (ldata[1] << 16);
    srand (seed);
    for (i = 0; i < len; i++)
    {
	value = rand ();
	auth[i] = value & 0xff;
    }
    auth[len] = '\0';

    return (auth);
}



IcePoAuthStatus
_IcePoMagicCookie1Proc (iceConn, authStatePtr, cleanUp, swap,
    authDataLen, authData, replyDataLenRet, replyDataRet, errorStringRet)

IceConn		iceConn;
IcePointer	*authStatePtr;
Bool 		cleanUp;
Bool		swap;
int     	authDataLen;
IcePointer	authData;
int 		*replyDataLenRet;
IcePointer	*replyDataRet;
char    	**errorStringRet;

{
    if (cleanUp)
    {
	/*
	 * We didn't allocate any state.  We're done.
	 */

	return (IcePoAuthDoneCleanup);
    }

    *errorStringRet = NULL;

    if (*authStatePtr == NULL)
    {
	/*
	 * This is the first time we're being called.  Search the
	 * authentication data for the first occurence of
	 * MIT-MAGIC-COOKIE-1 that matches iceConn->connection_string.
	 */

	unsigned short  length;
	char		*data;

	_IceGetPoAuthData ("ICE", iceConn->connection_string,
	    "MIT-MAGIC-COOKIE-1", &length, &data);

	if (!data)
	{
	    char *tempstr =
		"Could not find correct MIT-MAGIC-COOKIE-1 authentication";

	    *errorStringRet = (char *) malloc (strlen (tempstr) + 1);
	    if (*errorStringRet)
		strcpy (*errorStringRet, tempstr);

	    return (IcePoAuthFailed);
	}
	else
	{
	    *authStatePtr = (IcePointer) &was_called_state;

	    *replyDataLenRet = length;
	    *replyDataRet = data;

	    return (IcePoAuthHaveReply);
	}
    }
    else
    {
	/*
	 * We should never get here for MIT-MAGIC-COOKIE-1 since it is
	 * a single pass authentication method.
	 */

	char *tempstr = "MIT-MAGIC-COOKIE-1 authentication internal error";

	*errorStringRet = (char *) malloc (strlen (tempstr) + 1);
	if (*errorStringRet)
	    strcpy (*errorStringRet, tempstr);

	return (IcePoAuthFailed);
    }
}



IcePaAuthStatus
_IcePaMagicCookie1Proc (iceConn, authStatePtr, swap,
    authDataLen, authData, replyDataLenRet, replyDataRet, errorStringRet)

IceConn		iceConn;
IcePointer	*authStatePtr;
Bool		swap;
int     	authDataLen;
IcePointer	authData;
int 		*replyDataLenRet;
IcePointer	*replyDataRet;
char    	**errorStringRet;

{
    *errorStringRet = NULL;
    *replyDataLenRet = 0;
    *replyDataRet = NULL;

    if (*authStatePtr == NULL)
    {
	/*
	 * This is the first time we're being called.  We don't have
	 * any data to pass to the other client.
	 */

	*authStatePtr = (IcePointer) &was_called_state;

	return (IcePaAuthContinue);
    }
    else
    {
	/*
	 * Search the authentication data for the first occurence of
	 * MIT-MAGIC-COOKIE-1 that matches iceConn->connection_string.
	 */

	unsigned short  length;
	char		*data;

	_IceGetPaAuthData ("ICE", iceConn->connection_string,
	    "MIT-MAGIC-COOKIE-1", &length, &data);

	if (data)
	{
	    IcePaAuthStatus stat;

	    if (authDataLen == length &&
	        binaryEqual ((char *) authData, data, authDataLen))
	    {
		stat = IcePaAuthAccepted;
	    }
	    else
	    {
		char *tempstr = "MIT-MAGIC-COOKIE-1 authentication rejected";

		*errorStringRet = (char *) malloc (strlen (tempstr) + 1);
		if (*errorStringRet)
		    strcpy (*errorStringRet, tempstr);

		stat = IcePaAuthRejected;
	    }

	    free (data);
	    return (stat);
	}
	else
	{
	    /*
	     * We should never get here because in the ConnectionReply
	     * we should have passed all the valid methods.  So we should
	     * always find a valid entry.
	     */

	    char *tempstr =
		"MIT-MAGIC-COOKIE-1 authentication internal error";

	    *errorStringRet = (char *) malloc (strlen (tempstr) + 1);
	    if (*errorStringRet)
		strcpy (*errorStringRet, tempstr);

	    return (IcePaAuthFailed);
	}
    }
}



/*
 * local routines
 */

static int
binaryEqual (a, b, len)

register char		*a, *b;
register unsigned	len;

{
    while (len--)
	if (*a++ != *b++)
	    return 0;
    return 1;
}
