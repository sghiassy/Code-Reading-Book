/* $XConsortium: bdftopcf.c,v 1.8 94/04/17 20:23:59 gildea Exp $ */
/*

Copyright (c) 1991, 1993  X Consortium

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

#include <X11/X.h>
#include <X11/Xproto.h>
#include "fontmisc.h"
#include "fontstruct.h"
#include "fntfilio.h"
#include <stdio.h>
#include <X11/Xos.h>

main (argc, argv)
    char    **argv;
{
    FontRec font;
    FontFilePtr	input, output;
    char    *input_name = 0, *output_name = 0;
    char    *program_name;
    int	    bit, byte, glyph, scan;

    FontDefaultFormat (&bit, &byte, &glyph, &scan);
    program_name = argv[0];
    argc--, argv++;
    while (argc-- > 0) {
	if (argv[0][0] == '-') {
	    switch (argv[0][1]) {
	    case 'p':
		switch (argv[0][2]) {
		case '1':
		case '2':
		case '4':
		case '8':
		    if (argv[0][3] != '\0')
			goto usage;
		    glyph = argv[0][2] - '0';
		    break;
		default:
		    goto usage;
		}
		break;

	    case 'u':
		switch (argv[0][2]) {
		case '1':
		case '2':
		case '4':
		    if (argv[0][3] != '\0')
			goto usage;
		    scan = argv[0][2] - '0';
		    break;
		default:
		    goto usage;
		}
		break;

	    case 'm':
		if (argv[0][2] != '\0')
		    goto usage;
		bit = MSBFirst;
		break;

	    case 'l':
		if (argv[0][2] != '\0')
		    goto usage;
		bit = LSBFirst;
		break;

	    case 'M':
		if (argv[0][2] != '\0')
		    goto usage;
		byte = MSBFirst;
		break;

	    case 'L':
		if (argv[0][2] != '\0')
		    goto usage;
		byte = LSBFirst;
		break;

	    case 't':	/* attempt to make terminal fonts if possible */
		if (argv[0][2] != '\0')
		    goto usage;
		break;

	    case 'i':	/* inhibit ink metric computation */
		if (argv[0][2] != '\0')
		    goto usage;
		break;
	    case 'o':
		if (argv[0][2])
		    output_name = argv[0] + 2;
		else
		{
		    if (!argv[1])
			goto usage;
		    argv++;
		    argc--;
		    output_name = argv[0];
		}
		break;
	    default:
		goto usage;
	    }
	} else {
	    if (input_name)
	    {
	usage:
		fprintf(stderr,
	"usage: %s [-p#] [-u#] [-m] [-l] [-M] [-L] [-t] [-i] [-o pcf file] [bdf file]\n",
			program_name);
		fprintf(stderr,
			"       where # for -p is 1, 2, 4, or 8\n");
		fprintf(stderr,
			"       and   # for -s is 1, 2, or 4\n");
		exit(1);
	    }
	    input_name = argv[0];
	}
	argv++;
    }
    if (input_name)
    {
    	input = FontFileOpen (input_name);
    	if (!input)
    	{
	    fprintf (stderr, "%s: can't open bdf source file %s\n",
		     program_name, input_name);
	    exit (1);
    	}
    }
    else
	input = FontFileOpenFd (0);
    if (bdfReadFont (&font, input, bit, byte, glyph, scan) != Successful)
    {
	fprintf (stderr, "%s: bdf input, %s, corrupt\n",
		 program_name, input_name);
	exit (1);
    }
    if (output_name)
    {
	output = FontFileOpenWrite (output_name);
    	if (!output)
    	{
	    fprintf (stderr, "%s: can't open pcf sink file %s\n",
		     program_name, output_name);
	    exit (1);
    	}
    } 
    else
	output = FontFileOpenWriteFd (1);
    if (pcfWriteFont (&font, output) != Successful)
    {
	fprintf (stderr, "%s: can't write pcf file %s\n",
		 program_name, output_name ? output_name : "<stdout>");
	if (output_name)
	    unlink (output_name);
	exit (1);
    }
    else
	FontFileClose (output);
    exit (0);
}
