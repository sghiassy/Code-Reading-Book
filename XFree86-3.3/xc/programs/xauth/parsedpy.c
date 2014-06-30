/*
 * $XConsortium: parsedpy.c,v 1.9 94/04/17 20:37:51 hersh Exp $
 * $XFree86: xc/programs/xauth/parsedpy.c,v 3.0 1996/06/10 09:17:48 dawes Exp $
 *
 * parse_displayname - utility routine for splitting up display name strings
 *
 * 
Copyright (c) 1989  X Consortium

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
 * *
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>			/* for NULL */
#include <ctype.h>			/* for isascii() and isdigit() */
#include <X11/Xos.h>			/* for strchr() and string routines */
#include <X11/Xlib.h>			/* for Family contants */
#ifdef hpux
#include <sys/utsname.h>		/* for struct utsname */
#endif
#include <X11/Xauth.h>			/* for FamilyLocal */
#include <X11/Xmu/SysUtil.h>

#ifdef UNIXCONN
#define UNIX_CONNECTION "unix"
#define UNIX_CONNECTION_LENGTH 4
#endif

#ifdef X_NOT_STDC_ENV
extern char *malloc();
#else
#include <stdlib.h>
#endif


/*
 * private utility routines
 */

/*static*/ char *copystring (src, len)
    char *src;
    int len;
{
    char *cp;

    if (!src && len != 0) return NULL;
    cp = malloc (len + 1);
    if (cp) {
	if (src) strncpy (cp, src, len);
	cp[len] = '\0';
    }
    return cp;
}


char *get_local_hostname (buf, maxlen)
    char *buf;
    int maxlen;
{
    buf[0] = '\0';
    (void) XmuGetHostname (buf, maxlen);
    return (buf[0] ? buf : NULL);
}

#ifndef UNIXCONN
static char *copyhostname ()
{
    char buf[256];

    return (get_local_hostname (buf, sizeof buf) ? 
	    copystring (buf, strlen (buf)) : NULL);
}
#endif

/*
 * parse_displayname - display a display string up into its component parts
 */
Bool parse_displayname (displayname, familyp, hostp, dpynump, scrnump, restp)
    char *displayname;
    int *familyp;			/* return */
    char **hostp;			/* return */
    int *dpynump, *scrnump;		/* return */
    char **restp;			/* return */
{
    char *ptr;				/* work variables */
    int len;				/* work variable */
    int family = -1;			/* value to be returned */
    char *host = NULL;			/* must free if set and error return */
    int dpynum = -1;			/* value to be returned */
    int scrnum = 0;			/* value to be returned */
    char *rest = NULL;			/* must free if set and error return */
    Bool dnet = False;			/* if true then using DECnet */

					/* check the name */
    if (!displayname || !displayname[0]) return False;

					/* must have at least :number */
    ptr = strchr(displayname, ':');
    if (!ptr || !ptr[1]) return False;
    if (ptr[1] == ':') {
	if (ptr[2] == '\0') return False;
	dnet = True;
    }


    /*
     * get the host string; if none is given, use the most effiecient path
     */

    len = (ptr - displayname);	/* length of host name */
    if (len == 0) {			/* choose most efficient path */
#ifdef UNIXCONN
	host = copystring (UNIX_CONNECTION, UNIX_CONNECTION_LENGTH);
	family = FamilyLocal;
#else
	if (dnet) {
	    host = copystring ("0", 1);
	    family = FamilyDECnet;
	} else {
	    host = copyhostname ();
	    family = FamilyInternet;
	}
#endif
    } else {
	host = copystring (displayname, len);
	if (dnet) {
	    family = dnet;
	} else {
#ifdef UNIXCONN
	    if (host && strcmp (host, UNIX_CONNECTION) == 0)
	      family = FamilyLocal;
	    else
#endif
	      family = FamilyInternet;
	}
    }

    if (!host) return False;


    /*
     * get the display number; we know that there is something after the
     * colon (or colons) from above.  note that host is now set and must
     * be freed if there is an error.
     */

    if (dnet) ptr++;			/* skip the extra DECnet colon */
    ptr++;				/* move to start of display num */
    {
	register char *cp;

	for (cp = ptr; *cp && isascii(*cp) && isdigit(*cp); cp++) ;
	len = (cp - ptr);
					/* check present and valid follow */
	if (len == 0 || (*cp && *cp != '.')) {
	    free (host);
	    return False;
	}
	
	dpynum = atoi (ptr);		/* it will handle num. as well */
	ptr = cp;
    }

    /*
     * now get screen number if given; ptr may point to nul at this point
     */
    if (ptr[0] == '.') {
	register char *cp;

	ptr++;
	for (cp = ptr; *cp && isascii(*cp) && isdigit(*cp); cp++) ;
	len = (cp - ptr);
	if (len == 0 || (*cp && *cp != '.')) {	/* all prop name */
	    free (host);
	    return False;
	}

	scrnum = atoi (ptr);		/* it will handle num. as well */
	ptr = cp;
    }

    /*
     * and finally, get any additional stuff that might be following the
     * the screen number; ptr must point to a period if there is anything
     */

    if (ptr[0] == '.') {
	ptr++;
	len = strlen (ptr);
	if (len > 0) {
	    rest = copystring (ptr, len);
	    if (!rest) {
		free (host);
		return False;
	    }
	}
    }

    /*
     * and we are done!
     */

    *familyp = family;
    *hostp = host;
    *dpynump = dpynum;
    *scrnump = scrnum;
    *restp = rest;
    return True;
}

	    
