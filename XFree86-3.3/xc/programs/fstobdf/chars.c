/* $XConsortium: chars.c,v 1.4 94/04/17 20:24:26 gildea Exp $ */
/* $XFree86: xc/programs/fstobdf/chars.c,v 3.2 1996/08/13 11:32:34 dawes Exp $ */
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
#include	<X11/Xlib.h>
#include	"FSlib.h"

extern long yResolution;	/* intended vertical resoultion for font */
extern long pointSize;		/* font height in points */

#define BIT_ORDER	BitmapFormatBitOrderMSB
#ifdef BYTE_ORDER
#undef BYTE_ORDER
#endif
#define BYTE_ORDER	BitmapFormatByteOrderMSB
#define SCANLINE_UNIT	BitmapFormatScanlineUnit8
#define SCANLINE_PAD	BitmapFormatScanlinePad8
#define EXTENTS		BitmapFormatImageRectMin

#define SCANLINE_PAD_BYTES	1

#define GLWIDTHBYTESPADDED(bits, nBytes)				    \
	((nBytes) == 1 ? (((bits)  +  7) >> 3)		/* pad to 1 byte  */\
	:(nBytes) == 2 ? ((((bits) + 15) >> 3) & ~1)	/* pad to 2 bytes */\
	:(nBytes) == 4 ? ((((bits) + 31) >> 3) & ~3)	/* pad to 4 bytes */\
	:(nBytes) == 8 ? ((((bits) + 63) >> 3) & ~7)	/* pad to 8 bytes */\
	: 0)


static void
EmitBitmap(outFile, fontHeader, charInfo, encoding, bpr, data)
    FILE       *outFile;
    FSXFontInfoHeader *fontHeader;
    FSXCharInfo *charInfo;
    unsigned int encoding;
    int         bpr;
    unsigned char *data;
{
    char       *glyphName;
    unsigned int row;

    /*-
     * format:
     * STARTCHAR name
     * ENCODING index
     * SWIDTH scalablewidth 0
     * DWIDTH pixels 0
     * BBX width height xoff yoff
     * ATTRIBUTES xxxx
     * BITMAP hhhhhhhh ...
     * ENDCHAR
     *
     * where, SWIDTH * (point / 1000) * (yres / 72) = DWIDTH or,
     *        SWIDTH = 72000 *
     * DWIDTH / (point * yres)
     */

    fprintf(outFile, "STARTCHAR ");
    glyphName = XKeysymToString((KeySym) encoding);
    if (glyphName)
	fputs(glyphName, outFile);
    else
	fprintf(outFile, (fontHeader->char_range.min_char.low > 0 ?
			  "C%06o" : "C%03o"), encoding);
    fputc('\n', outFile);
    fprintf(outFile, "ENCODING %u\n", encoding);
    fprintf(outFile, "SWIDTH %ld 0\n",
	    (((long) charInfo->width) * 72000L) /
	    (pointSize * yResolution));
    fprintf(outFile, "DWIDTH %d 0\n", charInfo->width);
    fprintf(outFile, "BBX %d %d %d %d\n",
	    charInfo->right - charInfo->left,
	    charInfo->ascent + charInfo->descent,
	    charInfo->left,
	    -charInfo->descent);
    if (charInfo->attributes)
	fprintf(outFile, "ATTRIBUTES 0x%04x\n", charInfo->attributes);

    /*
     * emit the bitmap
     */
    fprintf(outFile, "BITMAP\n");
    for (row = 0; row < (charInfo->ascent + charInfo->descent); row++) {
	unsigned    byte;
	unsigned    bit;

	static unsigned maskTab[] =
	{
	    (1 << 7), (1 << 6), (1 << 5), (1 << 4),
	    (1 << 3), (1 << 2), (1 << 1), (1 << 0),
	};

	byte = 0;
	for (bit = 0; bit < (charInfo->right - charInfo->left); bit++) {
	    byte |= maskTab[bit & 7] & data[bit >> 3];
	    if ((bit & 7) == 7) {
		fprintf(outFile, "%02x", byte);
		byte = 0;
	    }
	}
	if ((bit & 7) != 0)
	    fprintf(outFile, "%02x", byte);
	fputc('\n', outFile);
	data += bpr;
    }
    fprintf(outFile, "ENDCHAR\n");
}


Bool
EmitCharacters(outFile, fontServer, fontHeader, fontID)
    FILE       *outFile;
    FSServer   *fontServer;
    FSXFontInfoHeader *fontHeader;
    Font        fontID;
{
    FSXCharInfo *extents;
    FSXCharInfo *charInfo;
    int         encoding;
    FSOffset   *offsets;
    unsigned char *glyph;
    unsigned char *glyphs;
    unsigned int nChars;
    int         firstChar;
    int         lastChar;
    int         ch;
    FSBitmapFormat format;

    nChars = 0;

    format = BYTE_ORDER | BIT_ORDER | SCANLINE_UNIT |
	SCANLINE_PAD | EXTENTS;
    firstChar = (fontHeader->char_range.min_char.high << 8)
	        + fontHeader->char_range.min_char.low;
    lastChar = (fontHeader->char_range.max_char.high << 8)
	       + fontHeader->char_range.max_char.low;

    (void) FSQueryXExtents16(fontServer, fontID, True, (FSChar2b *) 0, 0,
			     &extents);
    (void) FSQueryXBitmaps16(fontServer, fontID, format, True, (FSChar2b *) 0,
			     0, &offsets, &glyphs);

    charInfo = extents;
    /* calculate the actual number of chars */
    for (ch = 0; ch <= (lastChar - firstChar); ch++) {
	if ((charInfo->width != 0) || (charInfo->left != charInfo->right))
	    nChars++;
	charInfo++;
    }

    fprintf(outFile, "CHARS %u\n", nChars);

    /*
     * actually emit the characters
     */
    charInfo = extents;
    encoding = firstChar;
    glyph = glyphs;
    for (ch = 0; ch <= (lastChar - firstChar); ch++) {
	int         bpr;

	bpr = GLWIDTHBYTESPADDED((charInfo->right - charInfo->left),
				 SCANLINE_PAD_BYTES);
	if ((charInfo->width != 0) || (charInfo->right != charInfo->left))
	    EmitBitmap(outFile, fontHeader, charInfo, encoding, bpr, glyph);
	glyph += (charInfo->descent + charInfo->ascent) * bpr;

	charInfo++;
	encoding++;
    }
    FSFree((char *) extents);
    FSFree((char *) glyphs);
    FSFree((char *) offsets);
    return (True);
}
