/*
 * $XConsortium: xlogo.c /main/21 1996/01/14 16:51:25 kaleb $
 * $XFree86: xc/programs/xlogo/xlogo.c,v 3.1 1996/01/16 15:09:23 dawes Exp $
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
 *
 */

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include "Logo.h"
#include <X11/Xaw/Cardinals.h>
#ifdef XKB
#include <X11/extensions/XKBbells.h>
#endif

extern void exit();
static void quit();

static XrmOptionDescRec options[] = {
{ "-shape", "*shapeWindow", XrmoptionNoArg, (XPointer) "on" },
};

static XtActionsRec actions[] = {
    {"quit",	quit}
};

static Atom wm_delete_window;

String fallback_resources[] = {
    "*iconPixmap:    xlogo32",
    "*iconMask:      xlogo32",
    NULL,
};

static void die(w, client_data, call_data)
    Widget	w;
    XtPointer	client_data, call_data;
{
    XCloseDisplay(XtDisplay(w));
    exit(0);
}

static void save(w, client_data, call_data)
    Widget w;
    XtPointer client_data, call_data;
{
    return;
}

/*
 * Report the syntax for calling xlogo.
 */

static void Syntax(toplevel, call)
    Widget toplevel;
    char *call;
{
    Arg arg;
    SmcConn connection;
    String reasons[6];
    int i, num_reasons = 6;

    reasons[0] = "Usage: ";
    reasons[1] = call;
    reasons[2] = " [-fg <color>] [-bg <color>] [-rv] [-bw <pixels>] [-bd <color>]\n";
    reasons[3] = "             [-d [<host>]:[<vs>]]\n";
    reasons[4] = "             [-g [<width>][x<height>][<+-><xoff>[<+-><yoff>]]]\n";
    reasons[5] = "             [-shape]\n\n";

    XtSetArg(arg, XtNconnection, &connection);
    XtGetValues(toplevel, &arg, (Cardinal)1);
    if (connection) 
	SmcCloseConnection(connection, num_reasons, reasons);
    else {
	for (i=0; i < num_reasons; i++)
	    printf(reasons[i]);
    }
    exit(1);
}

void 
main(argc, argv)
int argc;
char **argv;
{
    Widget toplevel;
    XtAppContext app_con;

    toplevel = XtOpenApplication(&app_con, "XLogo",
				 options, XtNumber(options), 
				 &argc, argv, fallback_resources,
				 sessionShellWidgetClass, NULL, ZERO);
    if (argc != 1)
	Syntax(toplevel, argv[0]);

    XtAddCallback(toplevel, XtNsaveCallback, save, NULL);
    XtAddCallback(toplevel, XtNdieCallback, die, NULL);
    XtAppAddActions
	(XtWidgetToApplicationContext(toplevel), actions, XtNumber(actions));
    XtOverrideTranslations
	(toplevel, XtParseTranslationTable ("<Message>WM_PROTOCOLS: quit()"));
    XtCreateManagedWidget("xlogo", logoWidgetClass, toplevel, NULL, ZERO);
    XtRealizeWidget(toplevel);
    wm_delete_window = XInternAtom(XtDisplay(toplevel), "WM_DELETE_WINDOW",
				   False);
    (void) XSetWMProtocols (XtDisplay(toplevel), XtWindow(toplevel),
                            &wm_delete_window, 1);
    XtAppMainLoop(app_con);
}

/*ARGSUSED*/
static void quit(w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
{
    Arg arg;

    if (event->type == ClientMessage && 
	event->xclient.data.l[0] != wm_delete_window) {
#ifdef XKB
	XkbStdBell(XtDisplay(w), XtWindow(w), 0, XkbBI_BadValue);
#else
	XBell(XtDisplay(w), 0);
#endif
    } else {
	/* resign from the session */
	XtSetArg(arg, XtNjoinSession, False);
	XtSetValues(w, &arg, ONE);
	die(w, NULL, NULL);
    }
}

#ifdef MINIX
SmcCloseStatus SmcCloseConnection(SmcConn smcConn, int count, char **reasonMsgs)
{
	return SmcClosedNow;
}
#endif
