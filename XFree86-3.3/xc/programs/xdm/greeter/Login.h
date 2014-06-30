/* $XConsortium: Login.h,v 1.9 94/04/17 20:03:53 keith Exp $ */
/* $XFree86: xc/programs/xdm/greeter/Login.h,v 3.1 1995/10/21 12:52:32 dawes Exp $ */
/*

Copyright (c) 1988  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization
from the X Consortium.

*/

/*
 * xdm - display manager daemon
 * Author:  Keith Packard, MIT X Consortium
 */


#ifndef _XtLogin_h
#define _XtLogin_h

/***********************************************************************
 *
 * Login Widget
 *
 ***********************************************************************/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		pixel		White
 border		     BorderColor	pixel		Black
 borderWidth	     BorderWidth	int		1
 foreground	     Foreground		Pixel		Black
 height		     Height		int		120
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 width		     Width		int		120
 x		     Position		int		0
 y		     Position		int		0

*/

# define XtNgreeting		"greeting"
# define XtNunsecureGreeting	"unsecureGreeting"
# define XtNnamePrompt		"namePrompt"
# define XtNpasswdPrompt	"passwdPrompt"
# define XtNfail		"fail"
# define XtNnotifyDone		"notifyDone"
# define XtNpromptColor		"promptColor"
# define XtNgreetColor		"greetColor"
# define XtNfailColor		"failColor"
# define XtNpromptFont		"promptFont"
# define XtNgreetFont		"greetFont"
# define XtNfailFont		"failFont"
# define XtNfailTimeout		"failTimeout"
# define XtNsessionArgument	"sessionArgument"
# define XtNsecureSession	"secureSession"
# define XtNallowAccess		"allowAccess"
# define XtNallowNullPasswd	"allowNullPasswd"

# define XtCGreeting		"Greeting"
# define XtCNamePrompt		"NamePrompt"
# define XtCPasswdPrompt	"PasswdPrompt"
# define XtCFail		"Fail"
# define XtCFailTimeout		"FailTimeout"
# define XtCSessionArgument	"SessionArgument"
# define XtCSecureSession	"SecureSession"
# define XtCAllowAccess		"AllowAccess"
# define XtCAllowNullPasswd	"AllowNullPasswd"

/* notifyDone interface definition */

#define NAME_LEN	32

typedef struct _LoginData { 
	char	name[NAME_LEN], passwd[NAME_LEN];
} LoginData;

# define NOTIFY_OK	0
# define NOTIFY_ABORT	1
# define NOTIFY_RESTART	2
# define NOTIFY_ABORT_DISPLAY	3

typedef struct _LoginRec *LoginWidget;  /* completely defined in LoginPrivate.h */
typedef struct _LoginClassRec *LoginWidgetClass;    /* completely defined in LoginPrivate.h */

extern WidgetClass loginWidgetClass;

#endif /* _XtLogin_h */
/* DON'T ADD STUFF AFTER THIS #endif */
