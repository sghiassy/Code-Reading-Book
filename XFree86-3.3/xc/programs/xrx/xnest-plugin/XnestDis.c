/* $XConsortium: XnestDis.c /main/1 1996/08/14 14:40:45 lehors $ */
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

#include "RxPlugin.h"

/***********************************************************************
 * Utility functions and global variable to manage display numbers
 ***********************************************************************/

/* maximum numbers of concurrent instances */
#define MAX_PLUGINS 10

/* start from 5 to avoid possible conflict with multi-display X server */
#define XNEST_OFFSET 5

/* global allowing to know which display numbers are in use */
static int16 xnest_display_numbers[MAX_PLUGINS];

void
RxpInitXnestDisplayNumbers()
{
    memset(xnest_display_numbers, 0, sizeof(int16) * MAX_PLUGINS);
}

/* function returning first display number available */
int
RxpXnestDisplayNumber()
{
    int i;
    for (i = 0; i < MAX_PLUGINS; i++)
	if (xnest_display_numbers[i] == 0) {
	    xnest_display_numbers[i] = 1;
	    return i + XNEST_OFFSET;
	}
    /* no more available */
    return -1;
}

/* function returning first display number available */
void
RxpFreeXnestDisplayNumber(int i)
{
    xnest_display_numbers[i - XNEST_OFFSET] = 0;
}

/* function returning display name with specified display number */
char *
RxpXnestDisplay(int display_number)
{
    static char name[1024];
    char *display_name, *dpy_name, *display_num, *screen_num;

    display_name = getenv("DISPLAY");
    if (display_name == NULL)
	return NULL;

    /* if of the form x11:display skip scheme part */
    if (strncmp(display_name, "x11:", 4) == 0)
	dpy_name = display_name + 4;
    else
	dpy_name = display_name;

    /* display number is after next ":" character */
    display_num = strchr(dpy_name, ':');
    if (display_num == NULL)	/* invalid display specification */
	return NULL;

    /* copy display name up to the display number */
    strncpy(name, display_name, display_num - display_name);

    /* override display_number */
    sprintf(name + (display_num - display_name), ":%d", display_number);

    /* append screen number */
    screen_num = strchr(display_num, '.');
    if (screen_num != NULL)
	strcat(name, screen_num);

    return name;
}
