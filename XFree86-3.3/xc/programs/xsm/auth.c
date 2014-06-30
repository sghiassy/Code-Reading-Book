/* $XConsortium: auth.c,v 1.12 94/12/30 16:09:14 mor Exp $ */
/******************************************************************************

Copyright (c) 1993  X Consortium

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
/* $XFree86: xc/programs/xsm/auth.c,v 1.1.1.1.4.2 1997/05/13 11:31:37 hohndel Exp $ */

#include "xsm.h"

#include <X11/ICE/ICEutil.h>

#ifdef HAS_MKSTEMP
#include <unistd.h>
#endif

static char *addAuthFile = NULL;
static char *remAuthFile = NULL;



/*
 * Host Based Authentication Callback.  This callback is invoked if
 * the connecting client can't offer any authentication methods that
 * we can accept.  We can accept/reject based on the hostname.
 */

Bool
HostBasedAuthProc (hostname)

char *hostname;

{
    return (0);	      /* For now, we don't support host based authentication */
}



/*
 * We use temporary files which contain commands to add/remove entries from
 * the .ICEauthority file.
 */

static void
write_iceauth (addfp, removefp, entry)

FILE		 *addfp;
FILE 		 *removefp;
IceAuthDataEntry *entry;

{
    fprintf (addfp,
	"add %s \"\" %s %s ",
	entry->protocol_name,
        entry->network_id,
        entry->auth_name);
    fprintfhex (addfp, entry->auth_data_length, entry->auth_data);
    fprintf (addfp, "\n");

    fprintf (removefp,
	"remove protoname=%s protodata=\"\" netid=%s authname=%s\n",
	entry->protocol_name,
        entry->network_id,
        entry->auth_name);
}



#ifndef HAS_MKSTEMP
static char *
unique_filename (path, prefix)
char *path;
char *prefix;
#else
static char *
unique_filename (path, prefix, pFd)
char *path;
char *prefix;
int *pFd;
#endif

{
#ifndef HAS_MKSTEMP
#ifndef X_NOT_POSIX
    return ((char *) tempnam (path, prefix));
#else
    char tempFile[PATH_MAX];
    char *tmp;

    sprintf (tempFile, "%s/%sXXXXXX", path, prefix);
    tmp = (char *) mktemp (tempFile);
    if (tmp)
    {
	char *ptr = (char *) malloc (strlen (tmp) + 1);
	strcpy (ptr, tmp);
	return (ptr);
    }
    else
	return (NULL);
#endif
#else 
    char tempFile[PATH_MAX];
    char *ptr;

    sprintf (tempFile, "%s/%sXXXXXX", path, prefix);
    ptr = (char *)malloc(strlen(tempFile) + 1);
    if (ptr != NULL) 
    {
	strcpy(ptr, tempFile);
	*pFd =  mkstemp(ptr);
    }
    return ptr;
#endif
}




/*
 * Provide authentication data to clients that wish to connect
 */

#define MAGIC_COOKIE_LEN 16

Status
SetAuthentication (count, listenObjs, authDataEntries)

int			count;
IceListenObj		*listenObjs;
IceAuthDataEntry	**authDataEntries;

{
    FILE	*addfp = NULL;
    FILE	*removefp = NULL;
    char	*path;
    int		original_umask;
    char	command[256];
    int		i;
#ifdef HAS_MKSTEMP
    int         fd;
#endif

    original_umask = umask (0077);	/* disallow non-owner access */

    path = (char *) getenv ("SM_SAVE_DIR");
    if (!path)
    {
	path = (char *) getenv ("HOME");
	if (!path)
	    path = ".";
    }
#ifndef HAS_MKSTEMP
    if ((addAuthFile = unique_filename (path, ".xsm")) == NULL)
	goto bad;

    if (!(addfp = fopen (addAuthFile, "w")))
	goto bad;

    if ((remAuthFile = unique_filename (path, ".xsm")) == NULL)
	goto bad;

    if (!(removefp = fopen (remAuthFile, "w")))
	goto bad;
#else
    if ((addAuthFile = unique_filename (path, ".xsm", &fd)) == NULL)
	goto bad;
    
    if (!(addfp = fdopen(fd, "wb"))) 
	goto bad;

    if ((remAuthFile = unique_filename (path, ".xsm", &fd)) == NULL)
	goto bad;
    
    if (!(removefp = fdopen(fd, "wb"))) 
	goto bad;
#endif

    if ((*authDataEntries = (IceAuthDataEntry *) XtMalloc (
	count * 2 * sizeof (IceAuthDataEntry))) == NULL)
	goto bad;

    for (i = 0; i < count * 2; i += 2)
    {
	(*authDataEntries)[i].network_id =
	    IceGetListenConnectionString (listenObjs[i/2]);
	(*authDataEntries)[i].protocol_name = "ICE";
	(*authDataEntries)[i].auth_name = "MIT-MAGIC-COOKIE-1";

	(*authDataEntries)[i].auth_data =
	    IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
	(*authDataEntries)[i].auth_data_length = MAGIC_COOKIE_LEN;

	(*authDataEntries)[i+1].network_id =
	    IceGetListenConnectionString (listenObjs[i/2]);
	(*authDataEntries)[i+1].protocol_name = "XSMP";
	(*authDataEntries)[i+1].auth_name = "MIT-MAGIC-COOKIE-1";

	(*authDataEntries)[i+1].auth_data = 
	    IceGenerateMagicCookie (MAGIC_COOKIE_LEN);
	(*authDataEntries)[i+1].auth_data_length = MAGIC_COOKIE_LEN;

	write_iceauth (addfp, removefp, &(*authDataEntries)[i]);
	write_iceauth (addfp, removefp, &(*authDataEntries)[i+1]);

	IceSetPaAuthData (2, &(*authDataEntries)[i]);

	IceSetHostBasedAuthProc (listenObjs[i/2], HostBasedAuthProc);
    }

    fclose (addfp);
    fclose (removefp);

    umask (original_umask);

    sprintf (command, "iceauth source %s", addAuthFile);
    execute_system_command (command);

    unlink (addAuthFile);

    return (1);

 bad:

    if (addfp)
	fclose (addfp);

    if (removefp)
	fclose (removefp);

    if (addAuthFile)
    {
	unlink (addAuthFile);
	free (addAuthFile);
    }
    if (remAuthFile)
    {
	unlink (remAuthFile);
	free (remAuthFile);
    }

    return (0);
}



/*
 * Free up authentication data.
 */

void
FreeAuthenticationData (count, authDataEntries)

int			count;
IceAuthDataEntry 	*authDataEntries;

{
    /* Each transport has entries for ICE and XSMP */

    char command[256];
    int i;

    for (i = 0; i < count * 2; i++)
    {
	free (authDataEntries[i].network_id);
	free (authDataEntries[i].auth_data);
    }

    XtFree ((char *) authDataEntries);

    sprintf (command, "iceauth source %s", remAuthFile);
    execute_system_command (command);

    unlink (remAuthFile);

    free (addAuthFile);
    free (remAuthFile);
}
