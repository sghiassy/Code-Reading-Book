/* $XConsortium: config.c /main/11 1996/11/30 23:54:51 swick $ */

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

#include "pmint.h"
#include "config.h"


static Bool
getline (
char	**pbuf,
int	*plen,
FILE	*f)

{
    int c, i;

    i = 0;
    while(1)
    {
	if (i+2 > *plen)
	{
	    if (*plen)
		*plen *= 2;
	    else
		*plen = BUFSIZ;
	    if (*pbuf)
		*pbuf = (char *) realloc (*pbuf, *plen + 1);
	    else
		*pbuf = (char *) malloc (*plen + 1);
	    if (! *pbuf) {
		fprintf (stderr, "Memory allocation failure reading config file\n");
		return 0;
	    }
	}
	c = getc (f);
	if (c == EOF)
	    break;
	(*pbuf)[i++] = c;
	if (c == '\n') {
	    i--;
	    break;
	}
    }
    (*pbuf)[i] = '\0';
    return i;
}


#ifdef NEED_STRCASECMP
int
ncasecmp (str1, str2, n)
    char *str1, *str2;
    int n;
{
    char buf1[512],buf2[512];
    char c, *s;
    register int i;

    for (i=0, s = buf1; i < n && (c = *str1++); i++) {
	if (isupper(c))
	    c = tolower(c);
	if (i>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    for (i=0, s = buf2; i < n && (c = *str2++); i++) {
	if (isupper(c))
	    c = tolower(c);
	if (i>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    return (strncmp(buf1, buf2, n));
}
#endif /* NEED_STRCASECMP */


Status
GetConfig (
    char *configFile,
    char *serviceName,
    Bool *managed,
    char **startCommand,
    char **proxyAddress)

{
    FILE *fp;
    int found = 0;
    char *buf, *p;
    int buflen, n;

    *startCommand = *proxyAddress = NULL;

    fp = fopen (configFile, "r");

    if (!fp)
	return 0;

    buf = NULL;
    buflen = 0;
    n = strlen (serviceName);

    while (!found && getline (&buf, &buflen, fp))
    {
	if (buf[0] == '!')
	    continue;

	if (!(ncasecmp (buf, serviceName, n) == 0 && buf[n] == ' '))
	    continue;

	/* found the right config line */
	p = buf + n + 1;
	while (*p == ' ')
	    p++;
	if (ncasecmp (p, "managed", 7) == 0)
	{
	    *managed = 1;
	    p += 7;
	}
	else if (ncasecmp (p, "unmanaged", 9) == 0)
	{
	    *managed = 0;
	    p += 9;
	}
	else
	{
	    fprintf (stderr, "Error in config file at line \"%s\"\n", buf);
	    break;
	}

	while (*p == ' ')
	    p++;

	if (*managed)
	{
	    n = strlen (p);
	    *startCommand = (char *) malloc (n + 2);
	    if (! *startCommand) {
		fprintf (stderr,
			 "Memory allocation failed for service \"%s\"\n",
			 serviceName);
		break;
	    }
	    strcpy (*startCommand, p);
	    (*startCommand)[n] = '&';
	    (*startCommand)[n + 1] = '\0';
	}
	else
	{
	    *proxyAddress = (char *) malloc (strlen (p) + 1);
	    if (! *proxyAddress) {
		fprintf (stderr,
			 "Memory allocation failed for service \"%s\" at %s\n",
			 serviceName, p);
		break;
	    }
	    strcpy (*proxyAddress, p);
	}

	found = 1;
    }

    if (buf)
	free (buf);

    fclose (fp);
    return found;
}




