/* $TOG: VarGet.c /main/21 1997/05/15 17:32:13 kaleb $ */
/*

Copyright 1993 by Sun Microsystems, Inc. Mountain View, CA.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and without
fee is hereby granted, provided that the above copyright
notice appear in all copies and that both that copyright
notice and this permission notice appear in supporting
documentation, and that the name Sun not be used in
advertising or publicity pertaining to distribution of the
software without specific, written prior permission.
Sun makes no representations about the suitability of
this software for any purpose.  It is provided "as is"
without express or implied warranty.

SUN DISCLAIMS ALL WARRANTIES WITH REGARD TO  THIS  SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FIT-
NESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SUN BE  LI-
ABLE  FOR  ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,  DATA  OR
PROFITS,  WHETHER  IN  AN  ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
THE USE OR PERFORMANCE OF THIS SOFTWARE.

*/

/*

Copyright (c) 1985, 1986, 1987, 1988, 1989  X Consortium

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

#include "IntrinsicI.h"
#include "VarargsI.h"
#include "StringDefs.h"

static String XtNxtGetTypedArg = "xtGetTypedArg";

#if NeedVarargsPrototypes
void
XtVaGetSubresources(
    Widget widget,
    XtPointer base,
    _Xconst char* name,
    _Xconst char* class,
    XtResourceList resources,
    Cardinal num_resources,
    ...)
#else
/*VARARGS6*/
void XtVaGetSubresources(widget, base, name, class, resources, num_resources, va_alist)
    Widget widget;
    XtPointer base;
    String name;
    String class;
    XtResourceList resources;
    Cardinal num_resources;
    va_dcl
#endif
{
    va_list                 var;
    ArgList                 args;
    Cardinal                num_args;
    int			    total_count, typed_count;
    WIDGET_TO_APPCON(widget);

    LOCK_APP(app);
    Va_start(var, num_resources);
    _XtCountVaList(var, &total_count, &typed_count);
    va_end(var);
	 
    Va_start(var, num_resources);
	      
    _XtVaToArgList(widget, var, total_count, &args, &num_args);

    XtGetSubresources(widget, base, name, class, resources, num_resources, 
	args, num_args);

    if (num_args != 0) {
	XtFree((XtPointer)args);
    }    

    va_end(var);
    UNLOCK_APP(app);
}


#if NeedVarargsPrototypes
void
XtVaGetApplicationResources(Widget widget, XtPointer base, XtResourceList resources, Cardinal num_resources, ...)
#else
/*VARARGS4*/
void XtVaGetApplicationResources(widget, base, resources, num_resources, va_alist)
    Widget widget;
    XtPointer base;
    XtResourceList resources;
    Cardinal num_resources;
    va_dcl
#endif
{
    va_list                 var;
    ArgList                 args; 
    Cardinal                num_args; 
    int			    total_count, typed_count;
    WIDGET_TO_APPCON(widget);

    LOCK_APP(app);
    Va_start(var,num_resources);
    _XtCountVaList(var, &total_count, &typed_count);
    va_end(var);
	
    Va_start(var,num_resources); 

    _XtVaToArgList(widget, var, total_count, &args, &num_args);
                                
    XtGetApplicationResources(widget, base, resources, num_resources, 
	args, num_args); 

    if (num_args != 0) {
	XtFree((XtPointer)args);
    }    

    va_end(var);         
    UNLOCK_APP(app);
} 


static void
_XtGetTypedArg(widget, typed_arg, resources, num_resources)
    Widget              widget;
    XtTypedArgList	typed_arg;
    XtResourceList      resources;
    Cardinal            num_resources;
{
    String              from_type = NULL;
    Cardinal		from_size = 0;
    XrmValue            from_val, to_val;
    register int        i;
    Arg			arg;
    XtPointer		value;

    /* note we presume that the XtResourceList to be un-compiled */

    for (i = 0; i < num_resources; i++) {
        if (StringToName(typed_arg->name) == StringToName(resources[i].resource_name)) {
            from_type = resources[i].resource_type;
	    from_size = resources[i].resource_size;
            break;
        }
    }    

    if (i == num_resources) {
	XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "unknownType", XtNxtGetTypedArg, XtCXtToolkitError,
            "Unable to find type of resource for conversion",
            (String *)NULL, (Cardinal *)NULL);
 	return;
    }

    value = ALLOCATE_LOCAL(from_size);
    if (value == NULL) _XtAllocError(NULL);
    XtSetArg(arg, typed_arg->name, value);
    XtGetValues(widget, &arg, 1);

    from_val.size = from_size;
    from_val.addr = (XPointer)value;
    to_val.addr = (XPointer)typed_arg->value;
    to_val.size = typed_arg->size;

    if (!XtConvertAndStore(widget, from_type, &from_val,
			   typed_arg->type, &to_val)) {
	if (to_val.size > typed_arg->size) {
	    String params[2];
	    Cardinal num_params = 2;
	    params[0] = typed_arg->type;
	    params[1] = XtName(widget);
	    XtAppWarningMsg(XtWidgetToApplicationContext(widget),
		"insufficientSpace", XtNxtGetTypedArg, XtCXtToolkitError,
		"Insufficient space for converted type '%s' in widget '%s'",
		params, &num_params);
	}
	else {
	    String params[3];
	    Cardinal num_params = 3;
	    params[0] = from_type;
	    params[1] = typed_arg->type;
	    params[2] = XtName(widget);
	    XtAppWarningMsg(XtWidgetToApplicationContext(widget),
		"conversionFailed", XtNxtGetTypedArg, XtCXtToolkitError,
		"Type conversion (%s to %s) failed for widget '%s'",
		params, &num_params);
	}
    }
    DEALLOCATE_LOCAL(value);
}

static int
_XtGetNestedArg(widget, avlist, args, resources, num_resources)
    Widget              widget;
    XtTypedArgList	avlist;
    ArgList             args;
    XtResourceList      resources;
    Cardinal            num_resources;
{
    int         count = 0;
 
    for (; avlist->name != NULL; avlist++) {
        if (avlist->type != NULL) {
	    _XtGetTypedArg(widget, avlist, resources, num_resources);
        } else if(strcmp(avlist->name, XtVaNestedList) == 0) {
            count += _XtGetNestedArg(widget, (XtTypedArgList)avlist->value,
				     args, resources, num_resources);
        } else {
            (args+count)->name = avlist->name;
            (args+count)->value = avlist->value;
            ++count;
        }
    }   
 
    return(count);
}

#if NeedVarargsPrototypes
void
XtVaGetValues(Widget widget, ...)
#else
/*VARARGS1*/
void XtVaGetValues(widget, va_alist)
    Widget widget;
    va_dcl
#endif
{
    va_list		var;
    String      	attr;
    ArgList    		args;
    XtTypedArg		typed_arg;
    XtResourceList      resources = (XtResourceList)NULL;
    Cardinal            num_resources;
    int			count, total_count, typed_count;
    WIDGET_TO_APPCON(widget);

    LOCK_APP(app);
    Va_start(var,widget);

    _XtCountVaList(var, &total_count, &typed_count);

    if (total_count != typed_count) {
        args = (ArgList)__XtMalloc((unsigned)((total_count - typed_count) 
				* sizeof(Arg)));
    }
    else args = NULL;		/* for lint; really unused */
    va_end(var);

    Va_start(var,widget);
    for(attr = va_arg(var, String), count = 0 ; attr != NULL;
			attr = va_arg(var, String)) {
	if (strcmp(attr, XtVaTypedArg) == 0) {
	    typed_arg.name = va_arg(var, String);
	    typed_arg.type = va_arg(var, String);
	    typed_arg.value = va_arg(var, XtArgVal);
	    typed_arg.size = va_arg(var, int);

	    if (resources == NULL) {
		XtGetResourceList(XtClass(widget), &resources,&num_resources);
	    }

	    _XtGetTypedArg(widget, &typed_arg, resources, num_resources);
	} else if (strcmp(attr, XtVaNestedList) == 0) {
	    if (resources == NULL) {
		XtGetResourceList(XtClass(widget),&resources, &num_resources);
	    }

	    count += _XtGetNestedArg(widget, va_arg(var, XtTypedArgList),
				     (args+count), resources, num_resources);
	} else {
	    args[count].name = attr;
	    args[count].value = va_arg(var, XtArgVal);
	    count ++;
	}
    }
    va_end(var);

    if (resources != (XtResourceList)NULL) { 
	XtFree((XtPointer)resources); 
    }

    if (total_count != typed_count) {
	XtGetValues(widget, args, count);
	XtFree((XtPointer)args);
    }
    UNLOCK_APP(app);
}

#if NeedVarargsPrototypes
void
XtVaGetSubvalues(XtPointer base,XtResourceList  resources, Cardinal num_resources, ...)
#else
/*VARARGS3*/
void XtVaGetSubvalues(base, resources, num_resources, va_alist)
    XtPointer base;
    XtResourceList  resources;
    Cardinal num_resources;
    va_dcl
#endif
{
    va_list	var;
    ArgList    	args;
    Cardinal   	num_args;
    int		total_count, typed_count;		

    Va_start(var,num_resources);

    _XtCountVaList(var, &total_count, &typed_count);

    if (typed_count != 0) {
	XtWarning("XtVaTypedArg is an invalid argument to XtVaGetSubvalues()\n");
    }
    va_end(var);

    Va_start(var,num_resources);
    _XtVaToArgList((Widget)NULL, var, total_count, &args, &num_args);
    va_end(var);

    XtGetSubvalues(base, resources, num_resources, args, num_args);

    if (num_args != 0) {
        XtFree((XtPointer)args);
    }    
}
