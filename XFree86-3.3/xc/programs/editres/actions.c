/*
 * $XConsortium: actions.c /main/16 1995/12/11 00:30:43 gildea $
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
#include <X11/Xutil.h>

#include <X11/Xaw/Cardinals.h>	

#include "editresP.h"

/*
 * External Functions.
 */

extern void SetMessage(), _TreeSelect(), _TreeSelectNode(), _FindWidget();
extern void _TreeActivateNode(), _TreeRelabel(), _TreeRelabelNode();
extern void PrepareToLayoutTree(), LayoutTree(), _PopdownFileDialog();

/*
 * Private data.
 */

struct ActionValues {
    String name;
    int type;
};

static struct ActionValues select_values[] = {
    { "widget", (int) SelectWidget },
    { "all", (int) SelectAll },
    { "nothing", (int) SelectNone },
    { "invert", (int) SelectInvert },
    { "children", (int) SelectChildren },
    { "descendants", (int) SelectDescendants },
    { "parent", (int) SelectParent },
    { "ancestors", (int) SelectAncestors }
};

static struct  ActionValues label_values[] = {
    { "name", (int) NameLabel },
    { "class", (int) ClassLabel },
    { "id", (int) IDLabel },
    { "window", (int) WindowLabel },
    { "toggle", (int) ToggleLabel }
};

static WNode * FindTreeNodeFromWidget();
static Boolean CheckAndFindEntry();

/*	Function Name: EnableGetVal
 *	Description: sets a global variable to notify the Notify action
 *                   for the resource list widet to do GetValues.
 *      Arguments: w - any widget in the widget tree.
 *                 event - NOT USED.
 *                 params, num_params - the parameters paseed to the action
 *                                      routine. 
 *
 */

Boolean do_get_values = False;

/* ARGSUSED */
static void
EnableGetVal(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal * num_params;
{
  do_get_values = True;
}

/*	Function Name: SelectAction
 *	Description: 
 *      Arguments: w - any widget in the widget tree.
 *                 event - NOT USED.
 *                 params, num_params - the parameters paseed to the action
 *                                      routine. 
 *
 * params[0] - One of "nothing", "parent", "children", "ancestors",
 *                    "descendants", "invert", "all"
 * num_params - must be one.
 */

/* ARGSUSED */
static void
SelectAction(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal * num_params;
{
    WNode * node;
    int type;

    if (!CheckAndFindEntry("Select", params, *num_params, 
			   select_values, XtNumber(select_values), &type))
	return;

    switch(type) {
    case SelectAll:
    case SelectNone:
    case SelectInvert:
	_TreeSelect(global_tree_info, type);
	break;
    case SelectWidget:
	_FindWidget(XtParent(w));
	break;
    default:
	node = FindTreeNodeFromWidget(w);
	if (node)
	    _TreeActivateNode(node, type);	
	else
	    _TreeSelect(global_tree_info, type);
	break;
    }
}

/*	Function Name: RelabelAction
 *	Description: 
 *      Arguments: w - any widget in the widget tree.
 *                 event - NOT USED.
 *                 params, num_params - the parameters paseed to the action
 *                                      routine. 
 *
 * params[0] - One of "name", "class", "id"
 * num_params - must be one.
 */

/* ARGSUSED */
static void
RelabelAction(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal * num_params;
{
    WNode * node;
    int type;

    if (!CheckAndFindEntry("Relabel", params, *num_params, 
			   label_values, XtNumber(label_values), &type))
	return;

    if ((node = FindTreeNodeFromWidget(w)) == NULL) 
	_TreeRelabel(global_tree_info, type);
    else {
	PrepareToLayoutTree(global_tree_info->tree_widget); 
	_TreeRelabelNode(node, type, FALSE);
	LayoutTree(global_tree_info->tree_widget); 
    }
}

/*	Function Name: PopdownFileDialogAction
 *	Description: Pops down the file dialog widget.
 *                   and calls the approipriate handler.
 *	Arguments: w - any child of the dialog widget.
 *                 event - the event that caused this action.
 *                 params, num_params - params passed to the action routine.
 * RETURNED        none.
 */

/* ARGSUSED */

static void
PopdownFileDialogAction(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal * num_params;
{
    char buf[BUFSIZ];
    Boolean val;

    if (*num_params != 1) {
	sprintf(buf, res_labels[2], 
		"PopdownFileDialog");

	SetMessage(global_screen_data.info_label, buf);
	return;
    }

    XmuCopyISOLatin1Lowered(buf, params[0]);

    if (streq(buf, "cancel"))
	val = FALSE;
    else if (streq(buf, "okay"))
	val = TRUE;
    else {
	sprintf(buf, res_labels[1],
		"PopdownFileDialog");

	SetMessage(global_screen_data.info_label, buf);
	return;
    }

    _PopdownFileDialog(w, (XtPointer)(long) val, NULL);
}

/*	Function Name: ActionQuit
 *	Description: This function prints a message to stdout.
 *	Arguments: w - ** UNUSED **
 *                 call_data - ** UNUSED **
 *                 client_data - ** UNUSED **
 *	Returns: none
 */

/* ARGSUSED */
static void
ActionQuit(w, event, params, num_params)
Widget w;
XEvent * event;
String * params;
Cardinal * num_params;
{
  if (w==global_toplevel) {
    XtDestroyApplicationContext(XtWidgetToApplicationContext(w));
    exit(0);
  }
  else  {
    if (streq(XtName(w), RESOURCE_BOX))
      global_resource_box_up = FALSE;
    XtPopdown(w);
    XtDestroyWidget(w);
  }
}

/*	Function Name: SetApplicationActions
 *	Description: Sets my application actions.
 *	Arguments: app_con - the application context.
 *	Returns: none.
 */

extern void ModifySVEntry();

static XtActionsRec actions[] = {
  {"EnableGetVal",      EnableGetVal},
  {"Select",            SelectAction},
  {"SVActiveEntry",     ModifySVEntry},
  {"Relabel",      	RelabelAction}, 
  {"PopdownFileDialog", PopdownFileDialogAction},
  {"quit",              ActionQuit}
};

void
SetApplicationActions(app_con)
XtAppContext app_con;
{
    XtAppAddActions(app_con, actions, XtNumber(actions));
    
}


/************************************************************
 *
 * Private functions	
 *
 ************************************************************/

/*	Function Name: CheckAndFindEntry
 *	Description: Checks the args to make sure they are valid,
 *                   then parses the arg list to find the correct action
 *                   to take.
 *	Arguments: action_name - name of the action (for error messages).
 *                 params, num_params - params passed to the action routine.
 *                 table, num_table - table to check the parameters against.
 * RETURNED        type - info about the action to take.
 *	Returns: TRUE if the arguments are okay.
 */

static Boolean
CheckAndFindEntry(action_name, params, num_params, table, num_table, type)
String * params, action_name;
Cardinal num_params, num_table;
struct ActionValues * table;
int * type;
{
    char buf[BUFSIZ];
    int i;

    if (num_params != 1) {
	sprintf(buf, res_labels[2], 
		action_name);
	SetMessage(global_screen_data.info_label, buf);
	return(FALSE);
    }
	
    XmuCopyISOLatin1Lowered(buf, params[0]);
    for ( i = 0 ; i < num_table; i++ ) 
	if (streq(buf, table[i].name)) {
	    *type = table[i].type;
	    return(TRUE);
	}
    
    sprintf(buf,res_labels[3], 
	    action_name);

    for (i = 0; i < num_table; ) {
	strcat(buf, table[i++].name);
	
	if (i == (num_table - 1))
	    strcat(buf, ", or ");
	else if (i < num_table)
	    strcat(buf, ", ");
    }
    
    SetMessage(global_screen_data.info_label, buf);
    return(FALSE);
}

/*	Function Name: FindTreeNodeFromWidget
 *	Description: finds the tree node associated with a widget.
 *	Arguments: w - widget to check.
 *	Returns: the node associated with this widget, or NULL.
 */

static WNode *
FindTreeNodeFromWidget(w)
Widget w;
{
    int ret_val;
    XPointer data_return;

    /*
     * Yes, I really am casting a widget to a window  ** TRUST ME ***
     */

    ret_val = XFindContext(XtDisplay(w), (Window) w, NODE_INFO, &data_return);

    if (ret_val == 0) 
	return((WNode *) data_return);
    return(NULL);
}

