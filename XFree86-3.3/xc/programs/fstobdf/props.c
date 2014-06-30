/* $XConsortium: props.c,v 1.4 94/04/17 20:24:28 gildea Exp $ */
/*
 
Copyright (c) 1990, 1991  X Consortium

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

static char *
AddQuotes(string, length)
    unsigned char *string;
    int         length;
{
    static unsigned char new[256] = "\"";
    unsigned char *cp;
    unsigned char *end;

    end = string + length;
    for (cp = &new[1]; string < end; cp++, string++) {
	*cp = *string;
	if (*cp == '"')
	    *++cp = '"';
    }
    *cp++ = '"';
    *cp = '\0';
    return (char *)(new);
}

Bool
EmitProperties(outFile, fontHeader, propInfo, propOffsets, propData)
    FILE       *outFile;
    FSXFontInfoHeader *fontHeader;
    FSPropInfo *propInfo;
    FSPropOffset *propOffsets;
    unsigned char *propData;
{
    int         nProperties;
    FSPropOffset *property;
    Bool        needDefaultChar;
    Bool        needFontAscent;
    Bool        needFontDescent;

    needDefaultChar = True;
    needFontAscent = True;
    needFontDescent = True;

    nProperties = propInfo->num_offsets;
    for (property = &propOffsets[0]; nProperties--; property++) {
	unsigned char *name;
	int         length;

	name = propData + property->name.position;
	length = property->name.length;

	if ((length == 12) && (!strncmp(name, "DEFAULT_CHAR", 12)))
	    needDefaultChar = False;
	else if ((length == 11) && (!strncmp(name, "FONT_ASCENT", 11)))
	    needFontAscent = False;
	else if ((length == 12) && (!strncmp(name, "FONT_DESCENT", 12)))
	    needFontDescent = False;
    }

    nProperties = propInfo->num_offsets;
    fprintf(outFile, "STARTPROPERTIES %d\n", nProperties +
	    (needDefaultChar ? 1 : 0) + (needFontAscent ? 1 : 0) +
	    (needFontDescent ? 1 : 0));

    for (property = &propOffsets[0]; nProperties--; property++) {
	unsigned long value;

	/* Don't emit properties that are computed by bdftosnf */

	fwrite(propData + property->name.position, 1, property->name.length,
	       outFile);
	fputc(' ', outFile);

	value = property->value.position;
	switch (property->type) {
	case PropTypeString:
	    fprintf(outFile, "%s\n", AddQuotes(propData + value,
					       property->value.length));
	    break;
	case PropTypeUnsigned:
	    fprintf(outFile, "%lu\n", value);
	    break;
	case PropTypeSigned:
	    fprintf(outFile, "%ld\n", value);
	    break;
	default:
	    fprintf(stderr, "unknown property type\n");
	    return (False);
	}
    }
    if (needDefaultChar) {
	fprintf(outFile, "DEFAULT_CHAR %lu\n",
		(long) (fontHeader->default_char.high << 8)
		     | (fontHeader->default_char.low));
    }
    if (needFontAscent)
	fprintf(outFile, "FONT_ASCENT %d\n", fontHeader->font_ascent);
    if (needFontDescent)
	fprintf(outFile, "FONT_DESCENT %d\n", fontHeader->font_descent);
    fprintf(outFile, "ENDPROPERTIES\n");
    return (True);
}
