/*
 * fsinfo -- report info about a font server
 */

/* $XConsortium: fsinfo.c,v 1.6 95/04/06 19:50:32 mor Exp $ */
/* $XFree86: xc/programs/fsinfo/fsinfo.c,v 3.1 1995/07/08 10:31:42 dawes Exp $ */
/*

Portions Copyright (c) 1987  X Consortium

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
 * Copyright 1990 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation 
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices
 * or Digital not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Network Computing Devices and Digital
 * make no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * NETWORK COMPUTING DEVICES AND DIGITAL DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES OR DIGITAL BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 */

#include	<stdio.h>
#include	<X11/Xos.h>
#include	"FSlib.h"

static void print_server_info();
static void print_catalogue_info();
static void print_extension_info();
static void print_alternate_info();

char       *progname;

static void
usage()
{
    fprintf(stderr, "usage:  %s [-server server_name]\n", progname);
    exit(-1);
}

main(argc, argv)
    int         argc;
    char      **argv;
{
    FSServer   *svr;
    char       *servername = NULL;
    int         i;

    progname = argv[0];

    for (i = 1; i < argc; i++) {
	if (strncmp(argv[i], "-s", 2) == 0) {
	    if (++i > argc)
		usage();
	    servername = argv[i];
	} else {
	    usage();
	}
    }

    svr = FSOpenServer(servername);

    if (!svr) {
	if (FSServerName(servername) == NULL) {
	    fprintf(stderr, "%s: no font server defined\n", progname);
	    exit(1);
	}
	fprintf(stderr, "%s:  unable to open server \"%s\"\n",
		progname, FSServerName(servername));
	exit(1);
    }
    print_server_info(svr);
    FSCloseServer(svr);
    exit(0);
}

static void
print_server_info(svr)
    FSServer   *svr;
{
    printf("name of server:	%s\n", FSServerString(svr));
    printf("version number:	%d\n", FSProtocolVersion(svr));
    printf("vendor string:	%s\n", FSServerVendor(svr));
    printf("vendor release number:	%d\n", FSVendorRelease(svr));
    printf("maximum request size:	%ld longwords (%ld bytes)\n",
	   FSMaxRequestSize(svr), FSMaxRequestSize(svr) * sizeof(long));
    print_catalogue_info(svr);
    print_alternate_info(svr);
    print_extension_info(svr);
}

static void
print_catalogue_info(svr)
    FSServer   *svr;
{
    int         n = 0;
    char      **cats = FSListCatalogues(svr, "*", 1000, &n);

    printf("number of catalogues:	%d\n", n);
    if (cats) {
	int         i;

	for (i = 0; i < n; i++) {
	    printf("	%s\n", cats[i]);
	}
    }
}

static void
print_extension_info(svr)
    FSServer   *svr;
{
    int         n = 0;
    char      **extlist = FSListExtensions(svr, &n);

    printf("number of extensions:	%d\n", n);
    if (extlist) {
	int         i;

	for (i = 0; i < n; i++) {
	    printf("	%s\n", extlist[i]);
	}
    }
}

static void
print_alternate_info(svr)
    FSServer   *svr;
{
    AlternateServer *alts;
    int         i,
                num;

    num = FSNumAlternateServers(svr);
    printf("Number of alternate servers: %d\n", num);
    if (num) {
	alts = FSAlternateServers(svr);
	for (i = 0; i < num; i++) {
	    printf("    #%1d\t%s%s\n", i, alts[i].name,
		   (alts[i].subset) ? "(subset)" : "");
	}
    }
}
