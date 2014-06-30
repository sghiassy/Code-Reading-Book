/*
 * $XConsortium: editres.c,v 1.18 94/04/17 20:38:52 rws Exp $
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
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Xaw/Cardinals.h>	

#define THIS_IS_MAIN		/* Don't get extern definitions of global
				   variables. */

#include "editresP.h"

/*
 * Global variables. 
 */

/* array of toolkit dependent labels taken from the resource file */
String res_labels[NUM_RES_LABELS];

/* decremented if the target client does not speak the current version */
int global_effective_protocol_version = CURRENT_PROTOCOL_VERSION;

/* toolkit type of client whose "resources" we are currently editing */
char *global_effective_toolkit = "xt";

int global_error_code;
unsigned long global_serial_num;
int (*global_old_error_handler)();

Boolean global_resource_box_up = FALSE;
TreeInfo *global_tree_info = NULL;
CurrentClient global_client;
ScreenData global_screen_data;
Widget global_tree_parent;
Widget global_paned = NULL;		/* named after toolkit */
Widget global_toplevel;
AppResources global_resources;

/*
 * external function definitions.
 */

extern void InternAtoms(), SetMessage(), BuildWidgetTree();
extern void SetApplicationActions();

static void Syntax();

String fallback_resources[] = { 
    NULL,
};

#define Offset(field) (XtOffsetOf(AppResources, field))

static XtResource editres_resources[] = {
  {"debug", "Debug", XtRBoolean, sizeof(Boolean),
     Offset(debug), XtRImmediate, (XtPointer) FALSE},
  {"numFlashes", "NumFlashes", XtRInt, sizeof(int),
     Offset(num_flashes), XtRImmediate, (XtPointer) NUM_FLASHES},       
  {"flashTime", "FlashTime", XtRInt, sizeof(int),
     Offset(flash_time), XtRImmediate, (XtPointer) FLASH_TIME},       
  {"flashColor", XtCForeground, XtRPixel, sizeof(Pixel),
     Offset(flash_color), XtRImmediate, (XtPointer) XtDefaultForeground},
  {"saveResourceFile", "SaveResourcesFile", XtRString, sizeof(String),
     Offset(save_resources_file), XtRString, (XtPointer) ""},
};

Atom wm_delete_window;

void
main(argc, argv)
int argc;
char **argv;
{
    XtAppContext app_con;

    global_toplevel = XtAppInitialize(&app_con, "Editres", NULL, ZERO,
			       &argc, argv, fallback_resources,
			       NULL, ZERO);

    if (argc != 1)		
	Syntax(app_con, argv[0]);

    SetApplicationActions(app_con);
    XtGetApplicationResources(global_toplevel, (XtPointer) &global_resources, 
			      editres_resources, XtNumber(editres_resources),
			      NULL, (Cardinal) 0);
    global_resources.allocated_save_resources_file = FALSE;

    XtOverrideTranslations
      (global_toplevel, 
       XtParseTranslationTable ("<Message>WM_PROTOCOLS: quit()"));  

    /* build tree for Xt intrinsics */
    BuildWidgetTree(global_toplevel); 

    SetMessage(global_screen_data.info_label, 
	       res_labels[13]);

    global_screen_data.set_values_popup = NULL;

    InternAtoms(XtDisplay(global_toplevel));

    XtRealizeWidget(global_toplevel);

    wm_delete_window = 
      XInternAtom(XtDisplay(global_toplevel), "WM_DELETE_WINDOW",
				   False);
    (void) XSetWMProtocols (XtDisplay(global_toplevel), 
			    XtWindow(global_toplevel),
                            &wm_delete_window, 1);
    XtAppMainLoop(app_con);
}

/*	Function Name: Syntax
 *	Description: Prints a the calling syntax for this function to stdout.
 *	Arguments: app_con - the application context.
 *                 call - the name of the application.
 *	Returns: none - exits tho.
 */

static void 
Syntax(app_con, call)
XtAppContext app_con;
char *call;
{
    XtDestroyApplicationContext(app_con);
    fprintf(stderr, "Usage: %s\n", call);
    exit(1);
}
