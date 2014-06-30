/* $XConsortium: XUrls.c /main/7 1996/12/02 17:04:45 lehors $ */
/*

Copyright (C) 1996 X Consortium

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Soft-
ware"), to deal in the Software without restriction, including without
limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to
whom the Software is furnished to do so, subject to the following condi-
tions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT
SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABIL-
ITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization from
the X Consortium.

*/

#include "RxI.h"
#include <sys/utsname.h>
#include <netdb.h>
#include <limits.h>		/* for MAXHOSTNAMELEN */

/* and in case we didn't get it from the headers above */
#ifndef MAXHOSTNAMELEN
# define MAXHOSTNAMELEN 256
#endif

/* return the Url of the user's X display
 * this must be of the form:
 *   x11: [protocol/] [hostname] : [:] displaynumber [.screennumber] \
 *	[ ; auth= auth_data ]
 */
char *
GetXUrl(char *display_name, char *auth)
{
    char *dpy_name, *proto;
    char *url, *ptr;
    char *name;
    int len, proto_len, dpy_len, name_len, auth_len;

    len = 5;			/* this is for "x11:" */

    /* if of the form "x11:display" change it to "display" */
    if (strncmp(display_name, "x11:", 4) == 0)
	dpy_name = display_name + 4;
    else
	dpy_name = display_name;

    /* if protocol is specified store it and remove it from display name  */
    ptr = strchr(dpy_name, '/');
    if (ptr != NULL) {
	proto = dpy_name;
	proto_len = ptr - dpy_name;
	dpy_name = ptr + 1;
	/* if local forget it */
	if (strncmp(proto, "local", proto_len) == 0)
	    proto_len = 0;
    } else
	proto_len = 0;

    /* if of the form "unix:0.0" change it to ":0.0" */
    if (strncmp(dpy_name, "unix", 4) == 0)
	dpy_name += 4;

    /* if of the form ":0.0" get the real hostname */
    if (dpy_name[0] == ':') {
	struct utsname host;

	uname(&host);
	name = host.nodename;
    } else {			/* otherwise get canonical hostname */
	char hostname[MAXHOSTNAMELEN], *ptr;
	struct hostent *host;

	ptr = strchr(dpy_name, ':');
	if (ptr == NULL) {	/* no display number - not valid actually... */
	    strcpy(hostname, dpy_name);
	    dpy_name = NULL;
	} else {
	    strncpy(hostname, dpy_name, ptr - dpy_name);
	    hostname[ptr - dpy_name] = '\0';
	    /* since we have the canonical name skip the one we were given */
	    dpy_name = ptr;
	}
	host = gethostbyname(hostname);
	name = host->h_name;
    }
    name_len = strlen(name);

    dpy_len = dpy_name == NULL ? 0 : strlen(dpy_name);
    auth_len = auth == NULL ? 0 : 6 + strlen(auth); /* 6 for ";auth=" */
    len += proto_len + 1 + name_len + dpy_len + auth_len;

    url = ptr = (char *)Malloc(len);
    if (url == NULL)
	return NULL;

    strcpy(ptr, "x11:");
    ptr += 4;
    if (proto_len != 0) {
	strncpy(ptr, proto, proto_len + 1);
	ptr += proto_len + 1;
    }
    if (name_len != 0) {
	strcpy(ptr, name);
	ptr += name_len;
    }
    if (dpy_len != 0) {
	strcpy(ptr, dpy_name);
	ptr += dpy_len;
    }
    if (auth_len != 0)
	sprintf(ptr, ";auth=%s", auth);
    else
	*ptr = '\0';

    return url;
}


/* return the Url of the user's XPrint server
 * this must be of the form:
 *   xprint: [printername@] [protocol/] [hostname] : [:] displaynumber \
 *	[ ; auth= auth_data ]
 */
char *
GetXPrintUrl(char *display_name, char *printer, char *auth)
{
    char *dpy_name, *proto;
    char *url, *ptr;
    char *name;
    int len, proto_len, dpy_len, name_len, auth_len, printer_len;

    len = 7;			/* this is for "xprint:" */

    /* if of the form "xprint:display" change it to "display" */
    if (strncmp(display_name, "xprint:", 7) == 0)
	dpy_name = display_name + 7;
    else
	dpy_name = display_name;

    /* if protocol is specified store it and remove it from display name  */
    ptr = strchr(dpy_name, '/');
    if (ptr != NULL) {
	proto = dpy_name;
	proto_len = ptr - dpy_name;
	dpy_name = ptr + 1;
	/* if local forget it */
	if (strncmp(proto, "local", proto_len) == 0)
	    proto_len = 0;
    } else
	proto_len = 0;

    /* if of the form "unix:0.0" change it to ":0.0" */
    if (strncmp(dpy_name, "unix", 4) == 0)
	dpy_name += 4;

    /* if of the form ":0.0" get the real hostname */
    if (dpy_name[0] == ':') {
	struct utsname host;

	uname(&host);
	name = host.nodename;
    } else {			/* otherwise get canonical hostname */
	char hostname[MAXHOSTNAMELEN], *ptr;
	struct hostent *host;

	ptr = strchr(dpy_name, ':');
	if (ptr == NULL) {	/* no display number - not valid actually... */
	    strcpy(hostname, dpy_name);
	    dpy_name = NULL;
	} else {
	    strncpy(hostname, dpy_name, ptr - dpy_name);
	    hostname[ptr - dpy_name] = '\0';
	    /* since we have the canonical name skip the one we were given */
	    dpy_name = ptr;
	}
	host = gethostbyname(hostname);
	name = host->h_name;
    }

    /* if a screen number is specified strip it - not valid for printing */
    ptr = strchr(dpy_name, '.');
    if (ptr != NULL)
	dpy_len = ptr - dpy_name;
    else
	dpy_len = strlen(dpy_name);

    name_len = strlen(name);
    printer_len = printer ? strlen(printer) : 0;
    auth_len = auth == NULL ? 0 : 6 + strlen(auth); /* 6 for ";auth=" */
    len += printer_len + 1 + proto_len + 1 + name_len + dpy_len + auth_len;

    url = ptr = (char *)Malloc(len);
    if (url == NULL)
	return NULL;

    strcpy(ptr, "xprint:");
    ptr += 7;
    if (printer_len != 0) {
	strcpy(ptr, printer);
	ptr += printer_len;
	*ptr++ = '@';
    }
    if (proto_len != 0) {
	strncpy(ptr, proto, proto_len + 1);
	ptr += proto_len + 1;
    }
    if (name_len != 0) {
	strcpy(ptr, name);
	ptr += name_len;
    }
    if (dpy_len != 0) {
	strncpy(ptr, dpy_name, dpy_len);
	ptr += dpy_len;
    }
    if (auth_len != 0)
	sprintf(ptr, ";auth=%s", auth);
    else
	*ptr = '\0';

    return url;
}

/*
 * Extract the hostname of a given url. The hostname is copied to the given
 * buffer if it is big enough and its length is returned.
 * This relies strongly on the following expected syntax:
 *        scheme : [//] hostname [: port] [/path]
 */
int
ParseHostname(char *url, char *buf, int buflen)
{
    char *ptr, *begin;

    if (url == NULL)
	return 0;

    /* skip scheme part */
    ptr = strchr(url, ':');
    if (ptr != NULL)
	ptr++;
    else
	ptr = url;
    /* skip possible "//" */
    while (*ptr && *ptr == '/')
	ptr++;
    begin = ptr;
    /* look for possible port specification */
    ptr = strchr(begin, ':');
    if (ptr == NULL) {
	/* look for possible path */
	ptr = strchr(begin, '/');
	if (ptr == NULL)
	    ptr += strlen(begin);
    }
    if (ptr - begin < buflen) {
	strncpy(buf, begin, ptr - begin);
	buf[ptr - begin] = '\0';
	return ptr - begin;
    } else
	return 0;
}
