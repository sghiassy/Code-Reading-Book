/* $XConsortium: log.c,v 1.4 95/01/26 19:38:24 mor Exp $ */
/******************************************************************************

Copyright (c) 1994  X Consortium

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
******************************************************************************/

#include "xsm.h"
#include "save.h"
#include "popup.h"

#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/AsciiText.h>

Widget logPopup;
Widget   logForm;
Widget	   logText;
Widget	   logOkButton;



void
DisplayLogXtProc (w, client_data, callData)

Widget	  w;
XtPointer client_data;
XtPointer callData;

{
    static int first_time = 1;

    if (client_log_visible)
    {
	/* Make sure it is visible */

	XMapRaised (XtDisplay (topLevel), XtWindow (logPopup));
    }
    else
    {
	PopupPopup (mainWindow, logPopup,
	    False, first_time, 50, 50, "DelLogWinAction()");

	client_log_visible = 1;

	if (first_time)
	    first_time = 0;
    }
}



static void
logOkXtProc (w, client_data, callData)

Widget		w;
XtPointer 	client_data;
XtPointer 	callData;

{
    XtPopdown (logPopup);
    client_log_visible = 0;
}



void
add_log_text (str)

char *str;

{
    XawTextPosition pos = XawTextGetInsertionPoint (logText);
    XawTextBlock text;

    text.firstPos = 0;
    text.length = strlen (str);
    text.ptr = str;
    text.format = XawFmt8Bit;

    XawTextReplace (logText, pos, pos, &text);
}



static void
DelLogWinAction (w, event, params, num_params)

Widget w;
XEvent *event;
String *params;
Cardinal *num_params;

{
    XtCallCallbacks (logOkButton, XtNcallback, NULL);
}



void
create_log_popup ()

{
    /*
     * Pop up for session log
     */

    static XtActionsRec actions[] = {
        {"DelLogWinAction", DelLogWinAction}
    };

    XtAppAddActions (appContext, actions, XtNumber (actions));

    logPopup = XtVaCreatePopupShell ("logPopup",
	topLevelShellWidgetClass, topLevel,
	XtNallowShellResize, True,
	NULL);
    
    logForm = XtVaCreateManagedWidget (
	"logForm", formWidgetClass, logPopup,
	NULL);

    logText = XtVaCreateManagedWidget (
	"logText", asciiTextWidgetClass, logForm,
        XtNfromHoriz, NULL,
        XtNfromVert, NULL,
	XtNeditType, XawtextAppend,
        XtNdisplayCaret, False,
	XtNscrollVertical, XawtextScrollAlways,
	XtNscrollHorizontal, XawtextScrollWhenNeeded,
	XtNresizable, True,
	XtNtop, XawChainTop,
	XtNbottom, XawChainBottom,
	NULL);

    logOkButton = XtVaCreateManagedWidget (
	"logOkButton", commandWidgetClass, logForm,
        XtNfromHoriz, NULL,
        XtNfromVert, logText,
	XtNtop, XawChainBottom,
	XtNbottom, XawChainBottom,
	XtNleft, XawChainLeft,
	XtNright, XawChainLeft,
        NULL);

    XtAddCallback (logOkButton, XtNcallback, logOkXtProc, 0);
}
