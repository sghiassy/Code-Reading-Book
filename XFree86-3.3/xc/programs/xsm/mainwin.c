/* $XConsortium: mainwin.c,v 1.8 94/12/27 17:43:10 mor Exp $ */
/******************************************************************************

Copyright (c) 1993  X Consortium

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
#include "info.h"
#include "save.h"
#include "log.h"

#include <X11/Shell.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SmeBSB.h>

Widget mainWindow;
Widget   clientInfoButton;
Widget   logButton;
Widget   checkPointButton;
Widget   shutdownButton;
Widget     shutdownMenu;
Widget       shutdownSave;
Widget       shutdownDontSave;



static void
DelMainWinAction (w, event, params, num_params)

Widget w;
XEvent *event;
String *params;
Cardinal *num_params;

{
    XtCallCallbacks (shutdownSave, XtNcallback, NULL);
}



void
create_main_window ()

{
    /*
     * Main window
     */

    static XtActionsRec actions[] = {
        {"DelMainWinAction", DelMainWinAction}
    };

    mainWindow = XtVaCreateManagedWidget (
	"mainWindow", formWidgetClass, topLevel,
	NULL);

    clientInfoButton = XtVaCreateManagedWidget (
	"clientInfoButton", commandWidgetClass, mainWindow,
        XtNfromHoriz, NULL,
        XtNfromVert, NULL,
	XtNresizable, True,
	XtNjustify, XtJustifyLeft,
	NULL);

    XtAddCallback (clientInfoButton, XtNcallback, ClientInfoXtProc, 0);

    logButton = XtVaCreateManagedWidget (
	"logButton", commandWidgetClass, mainWindow,
        XtNfromHoriz, clientInfoButton,
        XtNfromVert, NULL,
	XtNresizable, True,
	XtNjustify, XtJustifyLeft,
	NULL);

    XtAddCallback (logButton, XtNcallback, DisplayLogXtProc, 0);

    checkPointButton = XtVaCreateManagedWidget (
	"checkPointButton", commandWidgetClass, mainWindow,
        XtNfromHoriz, NULL,
        XtNfromVert, clientInfoButton,
	XtNresizable, True,
	XtNjustify, XtJustifyLeft,
	NULL);

    XtAddCallback (checkPointButton, XtNcallback, CheckPointXtProc, 0);

    shutdownButton = XtVaCreateManagedWidget (
	"shutdownButton", menuButtonWidgetClass, mainWindow,
	XtNmenuName, "shutdownMenu",
        XtNfromHoriz, checkPointButton,
        XtNfromVert, clientInfoButton,
	XtNresizable, True,
	XtNjustify, XtJustifyLeft,
	NULL);

    shutdownMenu = XtVaCreatePopupShell (
	"shutdownMenu", simpleMenuWidgetClass, mainWindow,
	NULL);

    shutdownSave = XtVaCreateManagedWidget (
	"shutdownSave", smeBSBObjectClass, shutdownMenu,
	NULL);

    shutdownDontSave = XtVaCreateManagedWidget (
	"shutdownDontSave", smeBSBObjectClass, shutdownMenu,
	NULL);

    XtAddCallback (shutdownSave, XtNcallback, ShutdownSaveXtProc, 0);
    XtAddCallback (shutdownDontSave, XtNcallback, ShutdownDontSaveXtProc, 0);

    XtAppAddActions (appContext, actions, XtNumber (actions));
}
