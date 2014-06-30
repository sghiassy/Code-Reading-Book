/* $XConsortium: Initer.c,v 1.9 94/04/17 20:16:09 rws Exp $ */

/* 
 
Copyright (c) 1988, 1989  X Consortium

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

/* Created By:  Chris D. Peterson
 *              MIT X Consortium
 * Date:        May 8, 1989
 */

#include <X11/Intrinsic.h>
#include <X11/Xmu/Initer.h>

struct InitializerList {
  XmuInitializerProc function;	/* function to call */
  XPointer data;		/* Data to pass the function. */
  XtAppContext * app_con_list;	/* a null terminated list of app_contexts. */
};
  
static struct InitializerList * init_list = NULL;
static Cardinal init_list_length = 0;

static Boolean AddToAppconList();

void
XmuAddInitializer(func, data) 
XmuInitializerProc func;
XPointer data;
{
  init_list_length++;
  init_list = (struct InitializerList *) XtRealloc( (char *) init_list, 
					    (sizeof(struct InitializerList) * 
					     init_list_length) );

  init_list[init_list_length - 1].function = func;
  init_list[init_list_length - 1].data = data;
  init_list[init_list_length - 1].app_con_list = NULL;
}

void
XmuCallInitializers(app_con)
XtAppContext app_con;
{
  int i;

  for (i = 0 ; i < init_list_length ; i++) {
    if (AddToAppconList(&(init_list[i].app_con_list), app_con))
      (init_list[i].function) (app_con, init_list[i].data);
  }
}

/*	Function Name: AddToAppconList
 *	Description: Adds an action to the application context list and
 *                   returns TRUE, if this app_con is already on the list then
 *                   it is NOT added and FALSE is returned.
 *	Arguments: app_list - a NULL terminated list of application contexts.
 *                 app_con - an application context to test.
 *	Returns: TRUE if not found, FALSE if found.
 */

static Boolean
AddToAppconList(app_list, app_con)
XtAppContext **app_list, app_con;
{
  int i;
  XtAppContext *local_list;

  i = 0;
  local_list = *app_list;
  if (*app_list != NULL) {
    for ( ; *local_list != NULL ; i++, local_list++) {
      if (*local_list == app_con)
	return(FALSE);
    }
  }

  *app_list = (XtAppContext *)  XtRealloc((char *)(*app_list),
					  sizeof(XtAppContext *) * (i + 2) );
  (*app_list)[i++] = app_con;
  (*app_list)[i] = NULL;
  return(TRUE);
}
  
