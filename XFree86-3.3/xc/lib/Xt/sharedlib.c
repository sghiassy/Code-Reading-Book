/* $XConsortium: sharedlib.c,v 1.18 95/03/30 19:00:57 converse Exp $ */
/* $XFree86: xc/lib/Xt/sharedlib.c,v 3.1 1996/08/20 12:12:05 dawes Exp $ */

/*

Copyright (c) 1989, 1994  X Consortium

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

#if (defined(SUNSHLIB) || defined(AIXSHLIB)) && !defined(SHAREDCODE)
#include "IntrinsicI.h"
#include "VarargsI.h"
#include "ShellP.h"
#include "VendorP.h"


#if defined(AIXSHLIB) || defined(__EMX__)
WidgetClass vendorShellWidgetClass = (WidgetClass) &vendorShellClassRec;

static void _XtVendorInitialize()
{
    transientShellWidgetClass->core_class.superclass =
	(WidgetClass) &vendorShellClassRec;
    topLevelShellWidgetClass->core_class.superclass =
	(WidgetClass) &vendorShellClassRec;
}

#define VENDORINIT _XtVendorInitialize();

#else

#define VENDORINIT /* as nothing */

#endif

#ifdef SUNSHLIB
/*
 * _XtInherit needs to be statically linked since it is compared against as
 * well as called.
 */
void _XtInherit()
{
    extern void __XtInherit();
    __XtInherit();
}
#endif

/*
 * The following routine will be called by every toolkit
 * application, forcing this file to be statically linked.
 *
 * Note: XtInitialize, XtAppInitialize, and XtOpenApplication
 *       call XtToolkitInitialize.
 */

void XtToolkitInitialize()
{
    extern void _XtToolkitInitialize();
    VENDORINIT
    _XtToolkitInitialize();
}

#if NeedFunctionPrototypes
Widget 
XtInitialize(
_Xconst char* name,
_Xconst char* classname,
XrmOptionDescRec *options,
Cardinal num_options,
int *argc,
String *argv
)
#else
Widget 
XtInitialize(name, classname, options, num_options, argc, argv)
String name, classname;
XrmOptionDescRec *options;
Cardinal num_options;
String *argv;
int *argc;
#endif
{
    extern Widget _XtInitialize();
    VENDORINIT
    return _XtInitialize (name, classname, options, num_options, argc, argv);
}

#if NeedFunctionPrototypes
Widget
XtAppInitialize(
XtAppContext * app_context_return,
_Xconst char* application_class,
XrmOptionDescRec *options,
Cardinal num_options,
int *argc_in_out,
String *argv_in_out,
String *fallback_resources,
ArgList args_in,
Cardinal num_args_in
)
#else
Widget
XtAppInitialize(app_context_return, application_class, options, num_options,
		argc_in_out, argv_in_out, fallback_resources, 
		args_in, num_args_in)
XtAppContext * app_context_return;
String application_class;
XrmOptionDescRec *options;
Cardinal num_options, num_args_in;
int *argc_in_out;
String *argv_in_out, * fallback_resources;     
ArgList args_in;
#endif
{
    extern Widget _XtAppInitialize();
    VENDORINIT
    return _XtAppInitialize (app_context_return, application_class, options,
			     num_options, argc_in_out, argv_in_out, 
			     fallback_resources, args_in, num_args_in);
}

#if NeedVarargsPrototypes
Widget
XtVaAppInitialize(
    XtAppContext *app_context_return,
    _Xconst char* application_class,
    XrmOptionDescList options,
    Cardinal num_options,
    int *argc_in_out,
    String *argv_in_out,
    String *fallback_resources,
    ...)
#else
Widget XtVaAppInitialize(app_context_return, application_class, options,
			 num_options, argc_in_out, argv_in_out,
			 fallback_resources, va_alist)
    XtAppContext *app_context_return;
    String application_class;
    XrmOptionDescList options;
    Cardinal num_options;
    int *argc_in_out;
    String *argv_in_out;
    String *fallback_resources;
    va_dcl
#endif
{
    va_list	var;
    extern Widget _XtVaAppInitialize();

    VENDORINIT
    Va_start(var, fallback_resources);
    return _XtVaAppInitialize(app_context_return, application_class, options,
			      num_options, argc_in_out, argv_in_out,
			      fallback_resources, var);
}

#if NeedFunctionPrototypes
Widget
XtOpenApplication(
XtAppContext * app_context_return,
_Xconst char* application_class,
XrmOptionDescRec *options,
Cardinal num_options,
int *argc_in_out,
String *argv_in_out,
String *fallback_resources,
WidgetClass widget_class,
ArgList args_in,
Cardinal num_args_in
)
#else
Widget
XtOpenApplication(app_context_return, application_class, options, num_options,
		  argc_in_out, argv_in_out, fallback_resources, 
		  widget_class, args_in, num_args_in)
XtAppContext * app_context_return;
String application_class;
XrmOptionDescRec *options;
Cardinal num_options, num_args_in;
int *argc_in_out;
String *argv_in_out, * fallback_resources;
WidgetClass widget_class;
ArgList args_in;
#endif
{
    extern Widget _XtOpenApplication();
    VENDORINIT
    return _XtOpenApplication (app_context_return, application_class, options,
			       num_options, argc_in_out, argv_in_out, 
			       fallback_resources, widget_class,
			       args_in, num_args_in);
}

#if NeedVarargsPrototypes
Widget
XtVaOpenApplication(
    XtAppContext *app_context_return,
    _Xconst char* application_class,
    XrmOptionDescList options,
    Cardinal num_options,
    int *argc_in_out,
    String *argv_in_out,
    String *fallback_resources,
    WidgetClass widget_class,
    ...)
#else
Widget XtVaOpenApplication(app_context_return, application_class, options,
			   num_options, argc_in_out, argv_in_out,
			   fallback_resources, widget_class, va_alist)
    XtAppContext *app_context_return;
    String application_class;
    XrmOptionDescList options;
    Cardinal num_options;
    int *argc_in_out;
    String *argv_in_out;
    String *fallback_resources;
    WidgetClass widget_class;
    va_dcl
#endif
{
    va_list	var;
    extern Widget _XtVaOpenApplication();

    VENDORINIT
    Va_start(var, widget_class);
    return _XtVaOpenApplication(app_context_return, application_class, options,
				num_options, argc_in_out, argv_in_out,
				fallback_resources, widget_class, var);
}

#else

#ifndef lint
static int dummy;			/* avoid warning from ranlib */
#endif

#endif /* SUNSHLIB or AIXSHLIB */

#if defined(SUNSHLIB) && !defined(SHAREDCODE)

int _XtInheritTranslations = 0;

extern CompositeClassRec compositeClassRec;
WidgetClass compositeWidgetClass = (WidgetClass) &compositeClassRec;

extern ConstraintClassRec constraintClassRec;
WidgetClass constraintWidgetClass = (WidgetClass) &constraintClassRec;

extern WidgetClassRec widgetClassRec;
WidgetClass widgetClass = &widgetClassRec;
WidgetClass coreWidgetClass = &widgetClassRec;

extern ObjectClassRec objectClassRec;
WidgetClass objectClass = (WidgetClass)&objectClassRec;

extern RectObjClassRec rectObjClassRec;
WidgetClass rectObjClass = (WidgetClass)&rectObjClassRec;

extern ShellClassRec shellClassRec;
WidgetClass shellWidgetClass = (WidgetClass) &shellClassRec;

extern OverrideShellClassRec overrideShellClassRec;
WidgetClass overrideShellWidgetClass = (WidgetClass) &overrideShellClassRec;

extern WMShellClassRec wmShellClassRec;
WidgetClass wmShellWidgetClass = (WidgetClass) &wmShellClassRec;

extern TransientShellClassRec transientShellClassRec;
WidgetClass transientShellWidgetClass = (WidgetClass) &transientShellClassRec;

extern TopLevelShellClassRec topLevelShellClassRec;
WidgetClass topLevelShellWidgetClass = (WidgetClass) &topLevelShellClassRec;

extern ApplicationShellClassRec applicationShellClassRec;
WidgetClass applicationShellWidgetClass = (WidgetClass) &applicationShellClassRec;

extern SessionShellClassRec sessionShellClassRec;
WidgetClass sessionShellWidgetClass = (WidgetClass) &sessionShellClassRec;

extern HookObjClassRec hookObjClassRec;
WidgetClass hookObjectClass = (WidgetClass) &hookObjClassRec;

#endif /* SUNSHLIB */
