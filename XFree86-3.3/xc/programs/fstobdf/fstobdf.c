/* $XConsortium: fstobdf.c,v 1.5 94/04/17 20:24:27 gildea Exp $ */
/*
 
Copyright (c) 1990  X Consortium

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

 * Copyright 1990 Network Computing Devices;
 * Portions Copyright 1987 by Digital Equipment Corporation 
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Network Computing Devices, or Digital
 * not be used in advertising or publicity pertaining to distribution
 * of the software without specific, written prior permission.
 *
 * NETWORK COMPUTING DEVICES, AND DIGITAL DISCLAIM ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL NETWORK COMPUTING DEVICES,
 * OR DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

#include	<stdio.h>
#include	"FSlib.h"

extern Bool EmitHeader();
extern Bool EmitProperties();
extern Bool EmitCharacters();
extern Bool EmitTrailer();

static void
usage(progName)
    char       *progName;
{
    fprintf(stderr, "Usage: %s [-s <font server>] -fn <font name>\n",
	    progName);
    exit(0);
}

main(argc, argv)
    int         argc;
    char      **argv;
{
    FSServer   *fontServer;
    Font        fontID,
                dummy;
    FSBitmapFormat bitmapFormat;
    FSXFontInfoHeader fontHeader;
    FSPropInfo  propInfo;
    FSPropOffset *propOffsets;
    unsigned char *propData;

    FILE       *outFile;
    char       *fontName;
    char       *serverName;
    int         i;

    fontName = NULL;
    serverName = NULL;
    outFile = stdout;

    for (i = 1; i < argc; i++) {
	if (!strncmp(argv[i], "-s", 2)) {
	    if (argv[++i])
		serverName = argv[i];
	    else
		usage(argv[0]);
	} else if (!strncmp(argv[i], "-fn", 3)) {
	    if (argv[++i])
		fontName = argv[i];
	    else
		usage(argv[0]);
	}
    }

    if (fontName == NULL)
	usage(argv[0]);

    fontServer = FSOpenServer(serverName);
    if (!fontServer) {
	fprintf(stderr, "can't open font server \"%s\"\n",
		FSServerName(serverName));
	exit(0);
    }
    bitmapFormat = 0;
    fontID = FSOpenBitmapFont(fontServer, bitmapFormat, (FSBitmapFormatMask) 0,
			      fontName, &dummy);
    if (!fontID) {
	printf("can't open font \"%s\"\n", fontName);
	exit(0);
    }
    FSQueryXInfo(fontServer, fontID, &fontHeader, &propInfo, &propOffsets,
		 &propData);

    if (!EmitHeader(outFile, &fontHeader, &propInfo, propOffsets, propData))
	Fail(argv[0]);
    if (!EmitProperties(outFile, &fontHeader, &propInfo, propOffsets, propData))
	Fail(argv[0]);
    if (!EmitCharacters(outFile, fontServer, &fontHeader, fontID))
	Fail(argv[0]);
    fprintf(outFile, "ENDFONT\n");

    FSFree((char *) propOffsets);
    FSFree((char *) propData);
}

Fail(progName)
    char       *progName;
{
    fprintf(stderr, "%s: unable to dump font\n", progName);
    exit(1);
}
