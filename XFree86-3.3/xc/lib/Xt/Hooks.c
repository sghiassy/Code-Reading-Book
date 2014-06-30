/* $XConsortium: Hooks.c,v 1.8 94/04/17 20:14:10 kaleb Exp $ */

/*

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

*/

/*LINTLIBRARY*/

#include "IntrinsicI.h"

static void FreeBlockHookList( widget, closure, call_data )
    Widget widget;		/* unused (and invalid) */
    XtPointer closure;		/* ActionHook* */
    XtPointer call_data;	/* unused */
{
    BlockHook list = *(BlockHook*)closure;
    while (list != NULL) {
	BlockHook next = list->next;
	XtFree( (XtPointer)list );
	list = next;
    }
}


XtBlockHookId XtAppAddBlockHook( app, proc, closure )
    XtAppContext app;
    XtBlockHookProc proc;
    XtPointer closure;
{
    BlockHook hook = XtNew(BlockHookRec);
    LOCK_APP(app);
    hook->next = app->block_hook_list;
    hook->app = app;
    hook->proc = proc;
    hook->closure = closure;
    if (app->block_hook_list == NULL) {
	_XtAddCallback( &app->destroy_callbacks,
		        FreeBlockHookList,
		        (XtPointer)&app->block_hook_list
		      );
    }
    app->block_hook_list = hook;
    UNLOCK_APP(app);
    return (XtBlockHookId)hook;
}


void XtRemoveBlockHook( id )
    XtBlockHookId id;
{
    BlockHook *p, hook = (BlockHook)id;
    XtAppContext app = hook->app;
    LOCK_APP(app);
    for (p = &app->block_hook_list; p != NULL && *p != hook; p = &(*p)->next);
    if (p == NULL) {
#ifdef DEBUG
	XtAppWarningMsg(app, "badId", "xtRemoveBlockHook", XtCXtToolkitError,
			"XtRemoveBlockHook called with bad or old hook id",
			(String*)NULL, (Cardinal*)NULL);
#endif /*DEBUG*/	
	UNLOCK_APP(app);
	return;
    }
    *p = hook->next;
    XtFree( (XtPointer)hook );
    UNLOCK_APP(app);
}

static void DeleteShellFromHookObj(shell, closure, call_data)
    Widget shell;
    XtPointer closure, call_data;
{
    /* app_con is locked when this function is called */
    int ii, jj;
    HookObject ho = (HookObject) closure;

    for (ii = 0; ii < ho->hooks.num_shells; ii++)
	if (ho->hooks.shells[ii] == shell) {
	    /* collapse the list */
	    for (jj = ii; jj < ho->hooks.num_shells; jj++) {
		if ((jj+1) < ho->hooks.num_shells)
		    ho->hooks.shells[jj] = ho->hooks.shells[jj+1];
	    }
	    break;
	}
    ho->hooks.num_shells--;
}

#define SHELL_INCR 4

#if NeedFunctionPrototypes
void _XtAddShellToHookObj(
    Widget shell)
#else
void _XtAddShellToHookObj(shell)
    Widget shell;
#endif
{
    /* app_con is locked when this function is called */
    HookObject ho = (HookObject) XtHooksOfDisplay(XtDisplay(shell));

    if (ho->hooks.num_shells == ho->hooks.max_shells) {
	ho->hooks.max_shells += SHELL_INCR;
	ho->hooks.shells = 
	    (WidgetList)XtRealloc((char*)ho->hooks.shells, 
		ho->hooks.max_shells * sizeof (Widget));
    }
    ho->hooks.shells[ho->hooks.num_shells++] = shell;

    XtAddCallback(shell, XtNdestroyCallback, DeleteShellFromHookObj, 
		  (XtPointer)ho);
}

#if NeedFunctionPrototypes
Boolean _XtIsHookObject(
    Widget widget)
#else
Boolean _XtIsHookObject(widget)
    Widget widget;
#endif
{
    return (widget->core.widget_class == hookObjectClass);
}

Widget XtHooksOfDisplay(dpy)
    Display* dpy;
{
    extern Widget _XtCreateHookObj();
    Widget retval;
    XtPerDisplay pd;
    DPY_TO_APPCON(dpy);

    LOCK_APP(app);
    pd = _XtGetPerDisplay(dpy);
    if (pd->hook_object == NULL)
	pd->hook_object = 
	    _XtCreateHookObj((Screen*)DefaultScreenOfDisplay(dpy));
    retval = pd->hook_object;
    UNLOCK_APP(app);
    return retval;
}
