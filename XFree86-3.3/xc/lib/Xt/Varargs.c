/* $TOG: Varargs.c /main/32 1997/05/15 17:32:20 kaleb $ */

/*

Copyright (c) 1985, 1986, 1987, 1988, 1989, 1994  X Consortium

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

static String XtNxtConvertVarToArgList = "xtConvertVarToArgList";

/*
 *    Given a nested list, _XtCountNestedList() returns counts of the
 *    total number of attribute-value pairs and the count of those
 *    attributes that are typed. The list is counted recursively.
 */
static  void
_XtCountNestedList(avlist, total_count, typed_count)
    XtTypedArgList  avlist;
    int             *total_count;
    int             *typed_count;
{
    for (; avlist->name != NULL; avlist++) {
        if (strcmp(avlist->name, XtVaNestedList) == 0) {
            _XtCountNestedList((XtTypedArgList)avlist->value, total_count,
                	       typed_count);
        } else {
            if (avlist->type != NULL) {
                ++(*typed_count);
            }
            ++(*total_count);
        }
    }    
}


/*
 *    Given a variable length attribute-value list, _XtCountVaList()
 *    returns counts of the total number of attribute-value pairs,
 *    and the count of the number of those attributes that are typed.
 *    The list is counted recursively.
 */
#if NeedFunctionPrototypes
void
_XtCountVaList(va_list var, int* total_count, int* typed_count)
#else
void
_XtCountVaList(var, total_count, typed_count)
    va_list     var;
    int         *total_count;
    int         *typed_count;
#endif
{
    String          attr;
    
    *total_count = 0;
    *typed_count = 0;
 
    for(attr = va_arg(var, String) ; attr != NULL;
                        attr = va_arg(var, String)) {
        if (strcmp(attr, XtVaTypedArg) == 0) {
            va_arg(var, String);
            va_arg(var, String);
            va_arg(var, XtArgVal);
            va_arg(var, int);
            ++(*total_count);
            ++(*typed_count);
        } else if (strcmp(attr, XtVaNestedList) == 0) {
            _XtCountNestedList(va_arg(var, XtTypedArgList), total_count,
                typed_count);
        } else {
            va_arg(var, XtArgVal);
            ++(*total_count);
	}
    }
}


/* 
 *   Given a variable length attribute-value list, XtVaCreateArgsList()
 *   constructs an attribute-value list of type XtTypedArgList and 
 *   returns the list.
 */
#if NeedVarargsPrototypes
XtVarArgsList
XtVaCreateArgsList(XtPointer unused, ...)
#else
/*ARGSUSED*/
/*VARARGS1*/
XtVarArgsList XtVaCreateArgsList(unused, va_alist)
    XtPointer unused;
    va_dcl
#endif
{
    va_list var;
    XtTypedArgList  avlist;
    int		    count = 0;
    String	    attr;

    /*
     * Count the number of attribute-value pairs in the list.
     * Note: The count is required only to allocate enough space to store
     * the list. Therefore nested lists are not counted recursively.
     */
    Va_start(var,unused);
    for(attr = va_arg(var, String) ; attr != NULL;
                        attr = va_arg(var, String)) {
        ++count;
        if (strcmp(attr, XtVaTypedArg) == 0) {
            va_arg(var, String);
            va_arg(var, String);
            va_arg(var, XtArgVal);
            va_arg(var, int);
        } else {
            va_arg(var, XtArgVal);
        }
    }
    va_end(var);

    Va_start(var,unused);
    avlist = _XtVaCreateTypedArgList(var, count);
    va_end(var);
    return (XtVarArgsList)avlist;
}


#if NeedFunctionPrototypes
XtTypedArgList _XtVaCreateTypedArgList(va_list var, register int count)
#else
XtTypedArgList _XtVaCreateTypedArgList(var, count)
    va_list    	    var;     
    register int    count;
#endif
{
    String	    attr;
    XtTypedArgList  avlist;

    avlist = (XtTypedArgList)
		__XtCalloc((int)count + 1, (unsigned)sizeof(XtTypedArg));

    for(attr = va_arg(var, String), count = 0; attr != NULL; 
		attr = va_arg(var, String)) {
	if (strcmp(attr, XtVaTypedArg) == 0) {
	    avlist[count].name = va_arg(var, String);
	    avlist[count].type = va_arg(var, String);
	    avlist[count].value = va_arg(var, XtArgVal);
	    avlist[count].size = va_arg(var, int);
	} else {
	    avlist[count].name = attr;
	    avlist[count].type = NULL;
	    avlist[count].value = va_arg(var, XtArgVal);
	}
	++count;
    }
    avlist[count].name = NULL;

    return avlist;
}


/*
 *    _XtTypedArgToArg() invokes a resource converter to convert the
 *    passed typed arg into a name/value pair and stores the name/value
 *    pair in the passed Arg structure.  If memory is allocated for the
 *    converted value, the address is returned in the value field of 
 *    memory_return; otherwise that field is NULL.  The function returns
 *    1 if the conversion succeeded and 0 if the conversion failed.
 */
static int
_XtTypedArgToArg(widget, typed_arg, arg_return, resources, num_resources,
		 memory_return)
    Widget              widget;
    XtTypedArgList      typed_arg;
    ArgList             arg_return;
    XtResourceList      resources;
    Cardinal            num_resources;
    ArgList		memory_return;
{     
    String              to_type = NULL;
    XrmValue            from_val, to_val;
      

    if (widget == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "nullWidget", XtNxtConvertVarToArgList, XtCXtToolkitError,
	    "XtVaTypedArg conversion needs non-NULL widget handle",
            (String *)NULL, (Cardinal *)NULL);
        return(0);
    }
       
    /* again we assume that the XtResourceList is un-compiled */

    for (; num_resources--; resources++)
	if (strcmp(typed_arg->name, resources->resource_name) == 0) {
	    to_type = resources->resource_type;
	    break;
	}

    if (to_type == NULL) {
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "unknownType", XtNxtConvertVarToArgList, XtCXtToolkitError,
            "Unable to find type of resource for conversion",
            (String *)NULL, (Cardinal *)NULL);
        return(0);
    }
       
    to_val.addr = NULL;
    from_val.size = typed_arg->size;
    if ((strcmp(typed_arg->type, XtRString) == 0) ||
            (typed_arg->size > sizeof(XtArgVal))) {
        from_val.addr = (XPointer)typed_arg->value;
    } else {
            from_val.addr = (XPointer)&typed_arg->value;
    }

    LOCK_PROCESS;
    XtConvertAndStore(widget, typed_arg->type, &from_val, to_type, &to_val);
 
    if (to_val.addr == NULL) {
	UNLOCK_PROCESS;
        XtAppWarningMsg(XtWidgetToApplicationContext(widget),
            "conversionFailed", XtNxtConvertVarToArgList, XtCXtToolkitError,
            "Type conversion failed", (String *)NULL, (Cardinal *)NULL);
        return(0);
    }

    arg_return->name = typed_arg->name;
    memory_return->value = (XtArgVal) NULL;

    if (strcmp(to_type, XtRString) == 0) {
	arg_return->value = (XtArgVal) to_val.addr;
    }
    else {
	if (to_val.size == sizeof(long))
	    arg_return->value = (XtArgVal) *(long *)to_val.addr;
	else if (to_val.size == sizeof(short))
	    arg_return->value = (XtArgVal) *(short *)to_val.addr;
	else if (to_val.size == sizeof(char))
	    arg_return->value = (XtArgVal) *(char *)to_val.addr;
	else if (to_val.size == sizeof(XtArgVal))
	    arg_return->value = *(XtArgVal *)to_val.addr;
	else if (to_val.size > sizeof(XtArgVal)) {
	    arg_return->value = (XtArgVal) __XtMalloc(to_val.size);
	    memory_return->value = (XtArgVal)
		memcpy((void *)arg_return->value, to_val.addr, to_val.size);
	}
    }
    UNLOCK_PROCESS;
       
    return(1);
}


/*
 *    _XtNestedArgtoArg() converts the passed nested list into
 *    an ArgList/count.
 */
static int
_XtNestedArgtoArg(widget, avlist, args, resources, num_resources,
		  memory_return)
    Widget              widget;
    XtTypedArgList      avlist;
    ArgList             args;
    XtResourceList      resources;
    Cardinal            num_resources;
    ArgList		memory_return;
{
    int         count = 0;
 
    for (; avlist->name != NULL; avlist++) {
        if (avlist->type != NULL) {
            /* If widget is NULL, the typed arg is ignored */
            if (widget != NULL) {
                /* this is a typed arg */
                count += _XtTypedArgToArg(widget, avlist, (args+count),
					  resources, num_resources,
					  (memory_return+count));
            }
        } else if (strcmp(avlist->name, XtVaNestedList) == 0) {
            count += _XtNestedArgtoArg(widget, (XtTypedArgList)avlist->value,
				       (args+count), resources, num_resources,
				       (memory_return+count));
        } else {
            (args+count)->name = avlist->name;
            (args+count)->value = avlist->value;
            ++count;
        }
    }

    return(count);
}

/*
 * Free memory allocated through _XtVaToArgList.  The actual args array
 * size is expected to be total_count * 2, where total_count is the number
 * of elements needed for resource representations.  The lower half of the
 * array contains pairs of resource names and values as usual.  For each
 * element [n] in the lower half of the array, the value field of the
 * corresponding element [n + total_count] in the upper half of the array
 * has been pressed into service in order to note whether the resource value
 * is a pointer to memory that was allocated in _XtTypedArgToArg.  In the
 * upper half, if the value field is not NULL, it contains the address of
 * memory which should now be freed.  That memory could have been allocated
 * only as a result of the conversion of typed arguments.  Therefore, if
 * there were no typed arguments in the original varargs, there is no need
 * to examine the upper half of the array.  In the choice of data structure
 * to make this representation, priority was given to the wish to retrofit
 * the release of memory around the existing signature of _XtVaToArgList.
 */
#if NeedFunctionPrototypes
void
_XtFreeArgList(
    ArgList	args,	     /* as returned by _XtVaToArgList */
    int		total_count, /*  argument count returned by _XtCountVaList */
    int 	typed_count) /* typed arg count returned by _XtCountVaList */
#else
void
_XtFreeArgList(args, total_count, typed_count)
    ArgList	args;
    int		total_count;
    int 	typed_count;
#endif
{
    ArgList p;

    if (args) {
	if (typed_count)
	    for (p = args + total_count; total_count--; ++p) {
		if (p->value) XtFree((char *)p->value);
	    }
	XtFree((char *)args);
    }
}


static void		GetResources();

 
/* 
 *    Given a variable argument list, _XtVaToArgList() returns the 
 *    equivalent ArgList and count. _XtVaToArgList() handles nested 
 *    lists and typed arguments.  If typed arguments are present, the
 *    ArgList should be freed with _XtFreeArgList.
 */
#if NeedFunctionPrototypes
void
_XtVaToArgList(
    Widget		widget,
    va_list     	var,
    int			max_count,
    ArgList		*args_return,
    Cardinal		*num_args_return)
#else
void
_XtVaToArgList(widget, var, max_count, args_return, num_args_return)
    Widget		widget;
    va_list     	var;
    int			max_count;
    ArgList		*args_return;
    Cardinal		*num_args_return;
#endif
{
    String		attr;
    int			count;
    ArgList		args = (ArgList)NULL;
    XtTypedArg		typed_arg;
    XtResourceList	resources = (XtResourceList)NULL;
    Cardinal		num_resources;
    Boolean		fetched_resource_list = False;

    if (max_count  == 0) {
	*num_args_return = 0;
	*args_return = (ArgList)NULL;
	return;
    }

    max_count *= 2;
    args = (ArgList)__XtMalloc((unsigned)(max_count * sizeof(Arg)));
    for (count = max_count; --count >= 0; )
	args[count].value = (XtArgVal) NULL;
    max_count /= 2;
    count = 0;

    for(attr = va_arg(var, String) ; attr != NULL;
			attr = va_arg(var, String)) {
	if (strcmp(attr, XtVaTypedArg) == 0) {
	    typed_arg.name = va_arg(var, String);
	    typed_arg.type = va_arg(var, String);
	    typed_arg.value = va_arg(var, XtArgVal);
	    typed_arg.size = va_arg(var, int);

	    /* if widget is NULL, typed args are ignored */
	    if (widget != NULL) {
		if (!fetched_resource_list) {
		    GetResources(widget, &resources, &num_resources);
		    fetched_resource_list = True;
		}
		count += _XtTypedArgToArg(widget, &typed_arg, &args[count],
					  resources, num_resources,
					  &args[max_count + count]);
	    }
	} else if (strcmp(attr, XtVaNestedList) == 0) {
	    if (widget != NULL || !fetched_resource_list) {
		GetResources(widget, &resources, &num_resources);
		fetched_resource_list = True;
	    }

	    count += _XtNestedArgtoArg(widget, va_arg(var, XtTypedArgList),
				       &args[count], resources, num_resources,
				       &args[max_count + count]);
	} else {
	    args[count].name = attr;
	    args[count].value = va_arg(var, XtArgVal);
	    count ++;
	}
    }

    XtFree((XtPointer)resources);

    *num_args_return = (Cardinal)count;
    *args_return = (ArgList)args;
}

/*	Function Name: GetResources
 *	Description: Retreives the normal and constraint resources
 *                   for this widget.
 *	Arguments: widget - the widget.
 * RETURNED        res_list - the list of resource for this widget
 * RETURNED        number - the number of resources in the above list.
 *	Returns: none
 */

static void
GetResources(widget, res_list, number)
Widget widget;
XtResourceList * res_list;
Cardinal * number;
{
    Widget parent = XtParent(widget);

    XtInitializeWidgetClass(XtClass(widget));
    XtGetResourceList(XtClass(widget), res_list, number);
    
    if (!XtIsShell(widget) && parent && XtIsConstraint(parent)) {
	XtResourceList res, constraint, cons_top;
	Cardinal num_constraint, temp;

	XtGetConstraintResourceList(XtClass(parent), &constraint, 
				    &num_constraint);

	cons_top = constraint;
	*res_list = (XtResourceList) XtRealloc((char*)*res_list, 
					       ((*number + num_constraint) * 
						sizeof(XtResource)));

	for (temp= num_constraint, res= *res_list + *number; temp != 0; temp--)
	    *res++ = *constraint++;

	*number += num_constraint;
	XtFree( (XtPointer) cons_top);
    }
}

static int _XtNestedArgtoTypedArg(args, avlist) 
    XtTypedArgList      args;
    XtTypedArgList      avlist;
{    
    int         count = 0;
     
    for (; avlist->name != NULL; avlist++) { 
        if (avlist->type != NULL) { 
            (args+count)->name = avlist->name; 
            (args+count)->type = avlist->type; 
            (args+count)->size = avlist->size;
            (args+count)->value = avlist->value;
            ++count; 
        } else if(strcmp(avlist->name, XtVaNestedList) == 0) {             
            count += _XtNestedArgtoTypedArg((args+count),  
                            (XtTypedArgList)avlist->value); 
        } else {                             
            (args+count)->name = avlist->name; 
	    (args+count)->type = NULL;
            (args+count)->value = avlist->value; 
            ++count;
        }                                     
    }         
    return(count);
}


/*
 *    Given a variable argument list, _XtVaToTypedArgList() returns 
 *    the equivalent TypedArgList. _XtVaToTypedArgList() handles nested
 *    lists.
 *    Note: _XtVaToTypedArgList() does not do type conversions.
 */
#if NeedFunctionPrototypes
void
_XtVaToTypedArgList(
    va_list             var,
    int			max_count,
    XtTypedArgList   	*args_return,
    Cardinal            *num_args_return)
#else
void
_XtVaToTypedArgList(var, max_count, args_return, num_args_return)
    va_list             var;
    int			max_count;
    XtTypedArgList   	*args_return;
    Cardinal            *num_args_return;
#endif
{
    XtTypedArgList	args = NULL;
    String              attr;
    int			count;

    args = (XtTypedArgList)
	__XtMalloc((unsigned)(max_count * sizeof(XtTypedArg))); 

    for(attr = va_arg(var, String), count = 0 ; attr != NULL;
		    attr = va_arg(var, String)) {
        if (strcmp(attr, XtVaTypedArg) == 0) {
	    args[count].name = va_arg(var, String);
	    args[count].type = va_arg(var, String);
	    args[count].value = va_arg(var, XtArgVal);
	    args[count].size = va_arg(var, int);
	    ++count;
	} else if (strcmp(attr, XtVaNestedList) == 0) {
   	    count += _XtNestedArgtoTypedArg(&args[count], 
			va_arg(var, XtTypedArgList));
	} else {
	    args[count].name = attr;
	    args[count].type = NULL;
	    args[count].value = va_arg(var, XtArgVal);
	    ++count;
	}
    }

    *args_return = args;
    *num_args_return = count;
}
