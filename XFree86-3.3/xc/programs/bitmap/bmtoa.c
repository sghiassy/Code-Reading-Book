/* $XConsortium: bmtoa.c,v 1.6 94/04/17 20:23:49 rws Exp $ */
/* $XFree86: xc/programs/bitmap/bmtoa.c,v 3.0.4.1 1997/05/12 12:52:40 hohndel Exp $ */
/*

Copyright (c) 1988, 1993  X Consortium

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
 * bmtoa - bitmap to ascii filter
 * Author:  Jim Fulton, MIT X Consortium
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>

#include <X11/Xmu/Drawing.h>

#ifdef X_NOT_STDC_ENV
extern char *malloc();
#else
#include <stdlib.h>
#endif
#ifndef HAS_MKSTEMP
extern char *mktemp();
#else
#include <unistd.h>
#endif

char *ProgramName;


static void usage ()
{
    fprintf (stderr, "usage:  %s [-options ...] [filename]\n\n",
	     ProgramName);
    fprintf (stderr, 
	"where options include:\n");
    fprintf (stderr,
	"    -chars cc        chars to use for 0 and 1 bits, respectively\n");
    fprintf (stderr, "\n");
    exit (1);
}

static char *copy_stdin ()
{
#ifdef WIN32
    static char tmpfilename[] = "/temp/bmtoa.XXXXXX";
#else
    static char tmpfilename[] = "/tmp/bmtoa.XXXXXX";
#endif
    char buf[BUFSIZ];
    FILE *fp;
    int nread, nwritten;

#ifndef HAS_MKSTEMP
    if (mktemp (tmpfilename) == NULL) {
	fprintf (stderr,
		 "%s:  unable to genererate temporary file name for stdin.\n",
		 ProgramName);
	exit (1);
    }
    fp = fopen (tmpfilename, "w");
#else
    int fd;

    if ((fd = mkstemp(tmpfilename)) < 0) {
	fprintf (stderr,
		 "%s:  unable to genererate temporary file name for stdin.\n",
		 ProgramName);
	exit (1);
    }
    fp = fdopen(fd, "w");
#endif
    while (1) {
	buf[0] = '\0';
	nread = fread (buf, 1, sizeof buf, stdin);
	if (nread <= 0) break;
	nwritten = fwrite (buf, 1, nread, fp);
	if (nwritten != nread) {
	    fprintf (stderr,
		     "%s:  error copying stdin to file (%d of %d chars)\n",
		     ProgramName, nwritten, nread);
	    (void) fclose (fp);
	    (void) unlink (tmpfilename);
	    exit (1);
	}
    }
    (void) fclose (fp);
    return tmpfilename;
}

main (argc, argv) 
    int argc;
    char **argv;
{
    char *filename = NULL;
    int isstdin = 0;
    char *chars = "-#";
    int i;
    unsigned int width, height;
    unsigned char *data;
    int x_hot, y_hot;
    int status;

    ProgramName = argv[0];

    for (i = 1; i < argc; i++) {
	char *arg = argv[i];

	if (arg[0] == '-') {
	    switch (arg[1]) {
	      case '\0':
		filename = NULL;
		continue;
	      case 'c':
		if (++i >= argc) usage ();
		chars = argv[i];
		continue;
	      default:
		usage ();
	    }
	} else {
	    filename = arg;
	}
    }

    if (strlen (chars) != 2) {
	fprintf (stderr,
	 "%s:  bad character list \"%s\", must have exactly 2 characters\n",
		 ProgramName, chars);
	exit (1);
    }

    if (!filename) {
	filename = copy_stdin ();
	isstdin = 1;
    }

    status = XmuReadBitmapDataFromFile (filename, &width, &height, &data,
					&x_hot, &y_hot);
    if (isstdin) (void) unlink (filename);  /* don't need it anymore */
    if (status != BitmapSuccess) {
	fprintf (stderr, "%s:  unable to read bitmap from file \"%s\"\n",
		 ProgramName, isstdin ? "(stdin)" : filename);
	exit (1);
    }

    print_scanline (width, height, data, chars);
    exit (0);
}

print_scanline (width, height, data, chars)
    unsigned int width, height;
    unsigned char *data;
    char *chars;
{
    char *scanline = (char *) malloc (width + 1);
    unsigned char *dp = data;
    int row, column;
    static unsigned char masktable[] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
    int padded = ((width & 7) != 0);

    if (!scanline) {
	fprintf (stderr, "%s:  unable to allocate %d bytes for scanline\n",
		 ProgramName, width + 1);
	exit (1);
    }

    for (row = 0; row < height; row++) {
	for (column = 0; column < width; column++) {
	    int i = (column & 7);

	    if (*dp & masktable[i]) {
		putchar (chars[1]);
	    } else {
		putchar (chars[0]);
	    }

	    if (i == 7) dp++;
	}
	putchar ('\n');
	if (padded) dp++;
    }
    return;
}

