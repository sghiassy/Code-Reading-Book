/*
 * $XConsortium: xfd.c,v 1.33 94/04/17 20:24:15 rws Exp $
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
#include <X11/StringDefs.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>
#include <X11/Xaw/Cardinals.h>
#include <X11/Xaw/Paned.h>
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <stdio.h>
#include "grid.h"

char *ProgramName;

static XrmOptionDescRec xfd_options[] = {
{"-fn",		"*grid.font",	XrmoptionSepArg,	(caddr_t) NULL },
{"-start",	"*startChar",	XrmoptionSepArg, 	(caddr_t) NULL },
{"-box",	"*grid.boxChars", XrmoptionNoArg,	(caddr_t) "on" },
{"-bc",		"*grid.boxColor", XrmoptionSepArg, 	(caddr_t) NULL },
{"-center",	"*grid.centerChars", XrmoptionNoArg,	(caddr_t) "on" },
{"-rows",	"*grid.cellRows", XrmoptionSepArg,	(caddr_t) NULL },
{"-columns",	"*grid.cellColumns", XrmoptionSepArg,	(caddr_t) NULL },
};

static void do_quit(), do_next(), do_prev();
static void change_page (), set_button_state ();
static char *get_font_name();
static void SelectChar();

static XtActionsRec xfd_actions[] = {
  { "Quit", do_quit },
  { "Prev", do_prev },
  { "Next", do_next },
};

static Atom wm_delete_window;

Widget quitButton, prevButton, nextButton;


#define DEF_SELECT_FORMAT "character 0x%02x%02x (%u,%u) (%#o,%#o)"
#define DEF_METRICS_FORMAT "width %d; left %d, right %d; ascent %d, descent %d (font %d, %d)"
#define DEF_RANGE_FORMAT "range:  0x%02x%02x (%u,%u) thru 0x%02x%02x (%u,%u)"
#define DEF_START_FORMAT "upper left:  0x%04x (%d,%d)"
#define DEF_NOCHAR_FORMAT "no such character 0x%02x%02x (%u,%u) (%#o,%#o)"

static struct _xfd_resources {
  char *select_format;
  char *metrics_format;
  char *range_format;
  char *start_format;
  char *nochar_format;
} xfd_resources;

#define Offset(field) XtOffsetOf(struct _xfd_resources, field)

static XtResource Resources[] = {
  { "selectFormat", "SelectFormat", XtRString, sizeof(char *), 
      Offset(select_format), XtRString, DEF_SELECT_FORMAT },
  { "metricsFormat", "MetricsFormat", XtRString, sizeof(char *), 
      Offset(metrics_format), XtRString, DEF_METRICS_FORMAT },
  { "rangeFormat", "RangeFormat", XtRString, sizeof(char *), 
      Offset(range_format), XtRString, DEF_RANGE_FORMAT },
  { "startFormat", "StartFormat", XtRString, sizeof(char *), 
      Offset(start_format), XtRString, DEF_START_FORMAT },
  { "nocharFormat", "NocharFormat", XtRString, sizeof(char *), 
      Offset(nochar_format), XtRString, DEF_NOCHAR_FORMAT },
};

#undef Offset

usage()
{
    fprintf (stderr, "usage:  %s [-options ...] -fn font\n\n", ProgramName);
    fprintf (stderr, "where options include:\n");
    fprintf (stderr,
	"    -display dpy           X server to contact\n");
    fprintf (stderr, 
	"    -geometry geom         size and location of window\n");
    fprintf (stderr, 
	"    -start num             first character to show\n");
    fprintf (stderr, 
	"    -box                   show a box around each character\n");
    fprintf (stderr, 
	"    -center                center each character inside its grid\n");
    fprintf (stderr, 
	"    -rows number           number of rows in grid\n");
    fprintf (stderr, 
	"    -columns number        number of columns in grid\n");
    fprintf (stderr, "\n");
    exit (1);
}


static Widget selectLabel, metricsLabel, rangeLabel, startLabel, fontGrid;

static Boolean fontConversionFailed = False;
static void (*oldWarningHandler)();
static void CatchFontConversionWarning();

main (argc, argv) 
    int argc;
    char **argv;
{
    XtAppContext xtcontext;
    Widget toplevel, pane, toplabel, box, form;
    char buf[256];
    Arg av[10];
    Cardinal i;
    static XtCallbackRec cb[2] = { { SelectChar, NULL }, { NULL, NULL } };
    XFontStruct *fs;
    char *fontname;

    ProgramName = argv[0];

    toplevel = XtAppInitialize (&xtcontext, "Xfd",
				xfd_options, XtNumber(xfd_options),
				&argc, argv, NULL, NULL, 0);
    if (argc != 1) usage ();
    XtAppAddActions (xtcontext, xfd_actions, XtNumber (xfd_actions));
    XtOverrideTranslations
        (toplevel, XtParseTranslationTable ("<Message>WM_PROTOCOLS: Quit()"));

    XtGetApplicationResources (toplevel, (XtPointer) &xfd_resources, Resources,
			       XtNumber (Resources), NULL, ZERO);


    /* pane wrapping everything */
    pane = XtCreateManagedWidget ("pane", panedWidgetClass, toplevel,
				  NULL, ZERO);

    /* font name */
    toplabel = XtCreateManagedWidget ("fontname", labelWidgetClass, pane, 
				      NULL, ZERO);

    /* button box */
    box = XtCreateManagedWidget ("box", boxWidgetClass, pane, NULL, ZERO);
    quitButton = XtCreateManagedWidget ("quit", commandWidgetClass, box,
					NULL, ZERO);
    prevButton = XtCreateManagedWidget ("prev", commandWidgetClass, box,
					NULL, ZERO);
    nextButton = XtCreateManagedWidget ("next", commandWidgetClass, box,
					NULL, ZERO);


    /* and labels in which to put information */
    selectLabel = XtCreateManagedWidget ("select", labelWidgetClass,
					 pane, NULL, ZERO);

    metricsLabel = XtCreateManagedWidget ("metrics", labelWidgetClass,
					  pane, NULL, ZERO);

    rangeLabel = XtCreateManagedWidget ("range", labelWidgetClass, pane, 
					NULL, ZERO);

    startLabel = XtCreateManagedWidget ("start", labelWidgetClass, pane, 
					NULL, ZERO);

    /* form in which to draw */
    form = XtCreateManagedWidget ("form", formWidgetClass, pane, NULL, ZERO);
    
    i = 0;
    XtSetArg (av[i], XtNtop, XtChainTop); i++;
    XtSetArg (av[i], XtNbottom, XtChainBottom); i++;
    XtSetArg (av[i], XtNleft, XtChainLeft); i++;
    XtSetArg (av[i], XtNright, XtChainRight); i++;
    XtSetArg (av[i], XtNcallback, cb); i++;

    oldWarningHandler = XtAppSetWarningMsgHandler(xtcontext, 
						  CatchFontConversionWarning);

    fontGrid = XtCreateManagedWidget ("grid", fontgridWidgetClass, form,
				      av, i);

    XtAppSetWarningMsgHandler(xtcontext, oldWarningHandler);

    /* set the label at the top to tell us which font this is */
    i = 0;
    XtSetArg (av[i], XtNfont, &fs); i++;
    XtGetValues (fontGrid, av, i);
    if (!fs || fontConversionFailed) {
	fprintf (stderr, "%s:  no font to display\n", ProgramName);
	exit (1);
    }
    fontname = get_font_name (XtDisplay(toplevel), fs);
    if (!fontname) fontname = "unknown font!";
    i = 0;
    XtSetArg (av[i], XtNlabel, fontname); i++;
    XtSetValues (toplabel, av, i);

    i = 0;
    XtSetArg (av[i], XtNlabel, buf); i++;
    sprintf (buf, xfd_resources.range_format, 
	     fs->min_byte1, fs->min_char_or_byte2,
	     fs->min_byte1, fs->min_char_or_byte2,
	     fs->max_byte1, fs->max_char_or_byte2,
	     fs->max_byte1, fs->max_char_or_byte2);
    XtSetValues (rangeLabel, av, i);

    XtRealizeWidget (toplevel);

    wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
                                   False);
    (void) XSetWMProtocols (XtDisplay(toplevel), XtWindow(toplevel),
                            &wm_delete_window, 1);

    change_page (0);
    XtAppMainLoop (xtcontext);
}

/*ARGSUSED*/
static void SelectChar (w, closure, data)
    Widget w;
    XtPointer closure, data;
{
    FontGridCharRec *p = (FontGridCharRec *) data;
    XFontStruct *fs = p->thefont;
    unsigned n = ((((unsigned) p->thechar.byte1) << 8) |
		  ((unsigned) p->thechar.byte2));
    int direction, fontascent, fontdescent;
    XCharStruct metrics;
    char buf[256];
    Arg arg;

    XtSetArg (arg, XtNlabel, buf);

    if ((!fs->min_byte1 && !fs->max_byte1) ?
	(n < fs->min_char_or_byte2 || n > fs->max_char_or_byte2) :
	(p->thechar.byte1 < fs->min_byte1 || p->thechar.byte1 > fs->max_byte1 ||
	 p->thechar.byte2 < fs->min_char_or_byte2 ||
	 p->thechar.byte2 > fs->max_char_or_byte2)) {
	sprintf (buf, xfd_resources.nochar_format,
		 (unsigned) p->thechar.byte1, (unsigned) p->thechar.byte2,
		 (unsigned) p->thechar.byte1, (unsigned) p->thechar.byte2,
		 (unsigned) p->thechar.byte1, (unsigned) p->thechar.byte2);
	XtSetValues (selectLabel, &arg, ONE);
	buf[0] = '\0';
	XtSetValues (metricsLabel, &arg, ONE);
	return;
    }

    XTextExtents16 (fs, &p->thechar, 1, &direction, &fontascent, &fontdescent,
		    &metrics);
    sprintf (buf, xfd_resources.select_format,
	     (unsigned) p->thechar.byte1, (unsigned) p->thechar.byte2,
	     (unsigned) p->thechar.byte1, (unsigned) p->thechar.byte2,
	     (unsigned) p->thechar.byte1, (unsigned) p->thechar.byte2);
    XtSetValues (selectLabel, &arg, ONE);

    sprintf (buf, xfd_resources.metrics_format,
	     metrics.width, metrics.lbearing, metrics.rbearing,
	     metrics.ascent, metrics.descent, fontascent, fontdescent);
    XtSetValues (metricsLabel, &arg, ONE);

    return;
}


/*ARGSUSED*/
static void do_quit (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    exit (0);
}

static void change_page (page)
    int page;
{
    Dimension oldstart, newstart;
    int ncols, nrows;
    char buf[256];
    Arg arg;

    arg.name = XtNstartChar;
    GetFontGridCellDimensions (fontGrid, &oldstart, &ncols, &nrows);

    if (page) {
	Dimension start = (oldstart + 
			   ((long) ncols) * ((long) nrows) * ((long) page));

	arg.value = (XtArgVal) start;
	XtSetValues (fontGrid, &arg, ONE);
    }

    /* find out what it got set to */
    arg.value = (XtArgVal) &newstart;
    XtGetValues (fontGrid, &arg, ONE);

    /* if not paging, then initialize it, else only do it actually changed */
    if (!page || newstart != oldstart) {
	unsigned int row = (unsigned int) ((newstart >> 8) & 0xff);
	unsigned int col = (unsigned int) (newstart & 0xff);

	XtSetArg (arg, XtNlabel, buf);
	sprintf (buf, xfd_resources.start_format, newstart, row, col);
	XtSetValues (startLabel, &arg, ONE);
    }

    set_button_state ();

    return;
}


static void set_button_state ()
{
    Bool prevvalid, nextvalid;
    Arg arg;

    GetPrevNextStates (fontGrid, &prevvalid, &nextvalid);
    arg.name = XtNsensitive;
    arg.value = (XtArgVal) (prevvalid ? TRUE : FALSE);
    XtSetValues (prevButton, &arg, ONE);
    arg.value = (XtArgVal) (nextvalid ? TRUE : FALSE);
    XtSetValues (nextButton, &arg, ONE);
}


/* ARGSUSED */
static void do_prev (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    change_page (-1);
}


/* ARGSUSED */
static void do_next (w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    change_page (1);
}


static char *get_font_name (dpy, fs)
    Display *dpy;
    XFontStruct *fs;
{
    register XFontProp *fp;
    register int i;
    Atom fontatom = XInternAtom (dpy, "FONT", False);

    for (i = 0, fp = fs->properties; i < fs->n_properties; i++, fp++) {
	if (fp->name == fontatom) {
	    return (XGetAtomName (dpy, fp->card32));
	}
    }
    return NULL;
}


static void CatchFontConversionWarning(name, type, class, defaultp, params, np)
    String name, type, class, defaultp, *params;
    Cardinal *np;
{
    if (np && *np > 1 &
	strcmp(name, "conversionError") == 0 &&
	strcmp(type, "string") == 0 &&
	strcmp(class, "XtToolkitError") == 0 &&
	strcmp(params[1], "FontStruct") == 0) fontConversionFailed = True;

    (*oldWarningHandler)(name, type, class, defaultp, params, np);
}
