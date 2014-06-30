/* $XConsortium: Prefs.h /main/2 1996/11/27 14:27:04 lehors $ */
/*

Copyright (C) 1996 X Consortium

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Soft-
ware"), to deal in the Software without restriction, including without
limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to
whom the Software is furnished to do so, subject to the following condi-
tions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABIL-
ITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT
SHALL THE X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABIL-
ITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall
not be used in advertising or otherwise to promote the sale, use or
other dealings in this Software without prior written authorization from
the X Consortium.

*/

#ifndef _Prefs_h
#define _Prefs_h

#include <X11/Intrinsic.h>

typedef struct {
    unsigned int mask;
    unsigned int value;
} AddressFilter;

typedef struct {
    Boolean has_fwp;
    AddressFilter *internal_webservers;
    AddressFilter *trusted_webservers;
    AddressFilter *fast_webservers;
    int internal_webservers_count;
    int trusted_webservers_count;
    int fast_webservers_count;
} Preferences;


extern void GetPreferences(Widget widget, Preferences *preferences);
extern void FreePreferences(Preferences *preferences);
extern void ComputePreferences(Preferences *preferences, char *webserver,
	     Boolean *trusted_ret, Boolean *use_fwp_ret, Boolean *use_lbx_ret);

#endif /* _Prefs_h */
