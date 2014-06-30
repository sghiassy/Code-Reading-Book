/*
 * $XConsortium: appres.c,v 1.12 94/04/17 20:38:41 rws Exp $
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

#include <X11/Intrinsic.h>
#include <stdio.h>

#define NONAME "-AppResTest-"

char *ProgramName;

XrmQuark XrmQString;

static void usage ()
{
    fprintf (stderr,
	     "usage:  %s  [class [instance]] [-1] [toolkitoptions]\n",
	     ProgramName);
    fprintf (stderr,
	     "-1      list resources only at the specified level\n");
    fprintf (stderr,
	     "The number of class and instance elements must be equal.\n");
    exit (1);
}

/* stolen from Xlib Xrm.c */
static void PrintBindingQuarkList(bindings, quarks, stream)
    XrmBindingList      bindings;
    XrmQuarkList	quarks;
    FILE		*stream;
{
    Bool	firstNameSeen;

    for (firstNameSeen = False; *quarks; bindings++, quarks++) {
	if (*bindings == XrmBindLoosely) {
	    (void) fprintf(stream, "*");
	} else if (firstNameSeen) {
	    (void) fprintf(stream, ".");
	}
	firstNameSeen = True;
	(void) fputs(XrmQuarkToString(*quarks), stream);
    }
}

/* stolen from Xlib Xrm.c */
/* output out the entry in correct file syntax */
/*ARGSUSED*/
static Bool DumpEntry(db, bindings, quarks, type, value, data)
    XrmDatabase		*db;
    XrmBindingList      bindings;
    XrmQuarkList	quarks;
    XrmRepresentation   *type;
    XrmValuePtr		value;
    XPointer		data;
{
    FILE			*stream = (FILE *)data;
    register unsigned int	i;
    register char		*s;
    register char		c;

    if (*type != XrmQString)
	(void) putc('!', stream);
    PrintBindingQuarkList(bindings, quarks, stream);
    s = value->addr;
    i = value->size;
    if (*type == XrmQString) {
	(void) fputs(":\t", stream);
	if (i)
	    i--;
    }
    else
	fprintf(stream, "=%s:\t", XrmRepresentationToString(*type));
    if (i && (*s == ' ' || *s == '\t'))
	(void) putc('\\', stream); /* preserve leading whitespace */
    while (i--) {
	c = *s++;
	if (c == '\n') {
	    if (i)
		(void) fputs("\\n\\\n", stream);
	    else
		(void) fputs("\\n", stream);
	} else if (c == '\\')
	    (void) fputs("\\\\", stream);
	else if ((c < ' ' && c != '\t') ||
		 ((unsigned char)c >= 0x7f && (unsigned char)c < 0xa0))
	    (void) fprintf(stream, "\\%03o", (unsigned char)c);
	else
	    (void) putc(c, stream);
    }
    (void) putc('\n', stream);
    return False;
}

main (argc, argv)
    int argc;
    char **argv;
{
    Widget toplevel;
    char *iname = NONAME, *cname = NONAME;
    XtAppContext xtcontext;
    XrmName names[101];
    XrmClass classes[101];
    int i;
    int mode = XrmEnumAllLevels;

    ProgramName = argv[0];
    if (argc > 1 && argv[1][0] != '-') {
	cname = argv[1];
	if (argc > 2 && argv[2][0] != '-')
	    iname = argv[2];
    }

    XrmStringToClassList(cname, classes);
    XrmStringToNameList(iname, names);
    for (i = 0; names[i]; i++)
	;
    if (!i || classes[i] || !classes[i-1]) usage ();
    argv[0] = XrmNameToString(names[0]);

    toplevel = XtAppInitialize(&xtcontext, XrmClassToString(classes[0]),
			       NULL, 0, &argc, argv, NULL, NULL, 0);

    iname = NULL;
    cname = NULL;
    for (i = 1; i < argc; i++) {
	if (!strcmp(argv[i], "-1"))
	    mode = XrmEnumOneLevel;
	else if (argv[i][0] == '-')
	    usage();
	else if (!cname)
	    cname = argv[i];
	else if (!iname)
	    iname = argv[i];
	else
	    usage();
    }

    if (!iname) {
	XtGetApplicationNameAndClass(XtDisplay(toplevel), &iname, &cname);
	names[0] = XrmStringToName(iname);
    }

    XrmQString = XrmPermStringToQuark("String");

    XrmEnumerateDatabase(XtDatabase(XtDisplay(toplevel)),
			 names, classes, mode,
			 DumpEntry, (XPointer)stdout);

    exit (0);
}
