/* $XConsortium: LoginP.h,v 1.7 94/04/17 20:03:54 jim Exp $ */
/* $XFree86: xc/programs/xdm/greeter/LoginP.h,v 3.1 1995/10/21 12:52:34 dawes Exp $ */
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

#ifndef _LoginP_h
#define _LoginP_h

#include "Login.h"
#include <X11/CoreP.h>

#define GET_NAME	0
#define GET_PASSWD	1
#define DONE		2

/* New fields for the login widget instance record */
typedef struct {
	Pixel		textpixel;	/* foreground pixel */
	Pixel		promptpixel;	/* prompt pixel */
	Pixel		greetpixel;	/* greeting pixel */
	Pixel		failpixel;	/* failure pixel */
	GC		textGC;		/* pointer to GraphicsContext */
	GC		bgGC;		/* pointer to GraphicsContext */
	GC		xorGC;		/* pointer to GraphicsContext */
	GC		promptGC;
	GC		greetGC;
	GC		failGC;
	char		*greeting;	/* greeting */
	char		*unsecure_greet;/* message displayed when insecure */
	char		*namePrompt;	/* name prompt */
	char		*passwdPrompt;	/* password prompt */
	char		*fail;		/* failure message */
	XFontStruct	*font;		/* font for text */
	XFontStruct	*promptFont;	/* font for prompts */
	XFontStruct	*greetFont;	/* font for greeting */
	XFontStruct	*failFont;	/* font for failure message */
	int		state;		/* state */
	int		cursor;		/* current cursor position */
	int		failUp;		/* failure message displayed */
	LoginData	data;		/* name/passwd */
	char		*sessionArg;	/* argument passed to session */
	void		(*notify_done)();/* proc to call when done */
	int		failTimeout;	/* seconds til drop fail msg */
	XtIntervalId	interval_id;	/* drop fail message note */
	Boolean		secure_session;	/* session is secured */
	Boolean		allow_access;	/* disable access control on login */
	Boolean		allow_null_passwd; /* allow null password on login */
   } LoginPart;

/* Full instance record declaration */
typedef struct _LoginRec {
   CorePart core;
   LoginPart login;
   } LoginRec;

/* New fields for the Login widget class record */
typedef struct {int dummy;} LoginClassPart;

/* Full class record declaration. */
typedef struct _LoginClassRec {
   CoreClassPart core_class;
   LoginClassPart login_class;
   } LoginClassRec;

/* Class pointer. */
extern LoginClassRec loginClassRec;

#endif /* _LoginP_h */
