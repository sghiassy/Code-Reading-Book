/* $XConsortium: lock.c /main/6 1996/01/14 16:52:07 kaleb $ */
/* $XFree86: xc/programs/xsm/lock.c,v 3.0 1996/01/30 15:28:15 dawes Exp $ */
/******************************************************************************

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
******************************************************************************/

#include "xsm.h"
#include <sys/types.h>


static char *
GetPath ()

{
    char *path = (char *) getenv ("SM_SAVE_DIR");

    if (!path)
    {
	path = (char *) getenv ("HOME");
	if (!path)
	    path = ".";
    }

    return (path);
}


Status
LockSession (session_name, write_id)

char *session_name;
Bool write_id;

{
    char *path;
    char lock_file[PATH_MAX];
    char temp_lock_file[PATH_MAX];
    Status status;
    int fd;

    path = GetPath ();

#ifndef __EMX__
    sprintf (lock_file, "%s/.XSMlock-%s", path, session_name);
    sprintf (temp_lock_file, "%s/.XSMtlock-%s", path, session_name);
#else
    sprintf (temp_lock_file, "%s/%s.slk", path, session_name);
#endif

    if ((fd = creat (temp_lock_file, 0444)) < 0)
	return (0);

    if (write_id &&
        (write (fd, networkIds, strlen (networkIds)) != strlen (networkIds)) ||
	(write (fd, "\n", 1) != 1))
    {
	close (fd);
	return (0);
    }

    close (fd);

#ifndef __EMX__
    status = 1;

    if (link (temp_lock_file, lock_file) < 0)
	status = 0;

    if (unlink (temp_lock_file) < 0)
	status = 0;
#else
    status = 0;
#endif

    return (status);
}


void
UnlockSession (session_name)

char *session_name;

{
    char *path;
    char lock_file[PATH_MAX];

    path = GetPath ();

    sprintf (lock_file, "%s/.XSMlock-%s", path, session_name);

    unlink (lock_file);
}


char *
GetLockId (session_name)

char *session_name;

{
    char *path;
    FILE *fp;
    char lock_file[PATH_MAX];
    char buf[256];
    char *ret;

    path = GetPath ();

    sprintf (lock_file, "%s/.XSMlock-%s", path, session_name);

    if ((fp = fopen (lock_file, "r")) == NULL)
    {
	return (NULL);
    }

    buf[0] = '\0';
    fscanf (fp, "%s\n", buf);
    ret = XtNewString (buf);

    fclose (fp);

    return (ret);
}


Bool
CheckSessionLocked (session_name, get_id, id_ret)

char *session_name;
Bool get_id;
char **id_ret;

{
    if (get_id)
	*id_ret = GetLockId (session_name);

    if (!LockSession (session_name, False))
	return (1);

    UnlockSession (session_name);
    return (0);
}


void
UnableToLockSession (session_name)

char *session_name;

{
    /*
     * We should popup a dialog here giving error.
     */

#ifdef XKB
    XkbStdBell(XtDisplay(topLevel),XtWindow(topLevel),0,XkbBI_Failure);
#else
    XBell (XtDisplay (topLevel), 0);
#endif
    sleep (2);

    ChooseSession ();
}
