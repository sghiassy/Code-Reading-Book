/* $XConsortium: lcDynamic.c /main/2 1996/12/05 10:40:24 swick $ */
/*
Copyright (c) 1996  X Consortium

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and sell copies of the Software, and to
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
 * Copyright 1995 by FUJITSU LIMITED
 * This is source code modified by FUJITSU LIMITED under the Joint
 * Development Agreement for the CDE/Motif PST.
 *
 * Modifier: Takanori Tateno   FUJITSU LIMITED
 *
 */
#ifdef USE_DYNAMIC_LOADER
#include <stdio.h>
#include <string.h>
#include "Xlibint.h"

#ifndef XLOCALEDIR
#define XLOCALEDIR "/usr/lib/X11/locale"
#endif

#define LCLIBNAME    "xi18n.so"

extern void *dlopen();
extern void *dlsym();
extern int dlclose();
extern char *dlerror();

#define LAZY       1
#define NOW        2
#define GLOBAL     0x100

XLCd
_XlcDynamicLoader(name)
    char *name;
{
    char libpath[1024];
    XLCdMethods _XlcGenericMethods;
    XLCd lcd;
    void *nlshandler;

    sprintf(libpath,"%s/%s/%s",
		XLOCALEDIR,name,LCLIBNAME);
    nlshandler = dlopen(libpath,LAZY);
    _XlcGenericMethods = (XLCdMethods)dlsym(nlshandler,
				"genericMethods");
    lcd = _XlcCreateLC(name,_XlcGenericMethods);
    

    return lcd;
}
#endif /* USE_DYNAMIC_LOADER */
