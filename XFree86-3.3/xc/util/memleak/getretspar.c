/*
 * $XConsortium: getretspar.c,v 1.2 94/04/17 20:59:45 keith Exp $
 *
Copyright (c) 1992  X Consortium

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
 *
 * Author:  Keith Packard, MIT X Consortium
 */

/* Trace up the stack and build a call history -- SPARC specific code */

/* hack -- flush out the register windows by recursing */

static void
flushWindows (depth)
{
    if (depth == 0)
	return;
    flushWindows (depth-1);
}

getStackTrace (results, max)
    unsigned long   *results;
    int		    max;
{
    unsigned long   *sp, *getStackPointer (), *getFramePointer();
    unsigned long   *ra, mainCall;
    extern int	    main ();

    flushWindows (32);
    sp = getFramePointer ();
    while (max) 
    {
	/* sparc stack traces are easy -- chain up the saved FP/SP values */
	ra = (unsigned long *) sp[15];
	sp = (unsigned long *) sp[14];
	/* stop when we get the call to main */
	mainCall = ((((unsigned long) main) - ((unsigned long) ra)) >> 2) | 0x40000000;
	if (ra[0] == mainCall)
	{
	    *results++ = 0;
	    break;
	}
	*results++ = (unsigned long) ra;
	max--;
    }
}
