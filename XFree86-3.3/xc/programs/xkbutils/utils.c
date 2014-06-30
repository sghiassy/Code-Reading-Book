
  /*\
   * $XConsortium: utils.c /main/2 1996/12/04 10:24:08 lehors $
   * $XFree86: xc/programs/xkbutils/utils.c,v 3.1 1996/12/23 07:14:03 dawes Exp $
   *
   *		              COPYRIGHT 1990
   *		        DIGITAL EQUIPMENT CORPORATION
   *		           MAYNARD, MASSACHUSETTS
   *			    ALL RIGHTS RESERVED.
   *
   * THE INFORMATION IN THIS SOFTWARE IS SUBJECT TO CHANGE WITHOUT NOTICE AND
   * SHOULD NOT BE CONSTRUED AS A COMMITMENT BY DIGITAL EQUIPMENT CORPORATION.
   * DIGITAL MAKES NO REPRESENTATIONS ABOUT THE SUITABILITY OF THIS SOFTWARE 
   * FOR ANY PURPOSE.  IT IS SUPPLIED "AS IS" WITHOUT EXPRESS OR IMPLIED 
   * WARRANTY.
   *
   * IF THE SOFTWARE IS MODIFIED IN A MANNER CREATING DERIVATIVE COPYRIGHT
   * RIGHTS, APPROPRIATE LEGENDS MAY BE PLACED ON THE DERIVATIVE WORK IN
   * ADDITION TO THAT SET FORTH ABOVE.
   *
   * Permission to use, copy, modify, and distribute this software and its
   * documentation for any purpose and without fee is hereby granted, provided
   * that the above copyright notice appear in all copies and that both that
   * copyright notice and this permission notice appear in supporting
   * documentation, and that the name of Digital Equipment Corporation not be
   * used in advertising or publicity pertaining to distribution of the 
   * software without specific, written prior permission.
  \*/

#include 	"utils.h"
#include	<ctype.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#else
char *malloc();
#endif

/***====================================================================***/

Opaque
uAlloc(size)
    unsigned	size;
{
    return((Opaque)malloc(size));
}

/***====================================================================***/

Opaque
uCalloc(n,size)
    unsigned	n;
    unsigned	size;
{
    return((Opaque)calloc(n,size));
}

/***====================================================================***/

Opaque
uRealloc(old,newSize)
    Opaque	old;
    unsigned	newSize;
{
    if (old==NULL)
	 return((Opaque)malloc(newSize));
    else return((Opaque)realloc((char *)old,newSize));
}

/***====================================================================***/

Opaque
uRecalloc(old,nOld,nNew,itemSize)
    Opaque	old;
    unsigned	nOld;
    unsigned	nNew;
    unsigned	itemSize;
{
char *rtrn;

    if (old==NULL)
	 rtrn= (char *)calloc(nNew,itemSize);
    else {
	rtrn= (char *)realloc((char *)old,nNew*itemSize);
   	if ((rtrn)&&(nNew>nOld)) {
	    bzero(&rtrn[nOld*itemSize],(nNew-nOld)*itemSize);
	}
    }
    return (Opaque)rtrn;
}

/***====================================================================***/

void
uFree(ptr)
    Opaque ptr;
{
    if (ptr!=(Opaque)NULL)
	free((char *)ptr);
    return;
}

/***====================================================================***/
/***                  FUNCTION ENTRY TRACKING                           ***/
/***====================================================================***/

#ifndef Lynx
static	FILE	*entryFile=	stderr;
#else
static	FILE	*entryFile=	NULL;
#endif
	int	 uEntryLevel;

Boolean
uSetEntryFile(name)
    char *name;
{
    if ((entryFile!=NULL)&&(entryFile!=stderr)) {
	fprintf(entryFile,"switching to %s\n",name?name:"stderr");
	fclose(entryFile);
    }
    if (name!=NullString)	entryFile=	fopen(name,"w");
    else			entryFile=	stderr;
    if (entryFile==NULL) {
	entryFile=	stderr;
	return(False);
    }
    return(True);
}

void
uEntry(l,s,a1,a2,a3,a4,a5,a6,a7,a8)
int	l;
char	*s;
Opaque	a1,a2,a3,a4,a5,a6,a7,a8;
{
int	i;

    for (i=0;i<uEntryLevel;i++) {
	putc(' ',entryFile);
    }
    fprintf(entryFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    uEntryLevel+= l;
    return;
}

void
uExit(l,rtVal)
    int		l;
    char *	rtVal;
{
int	i;

    uEntryLevel-= l;
    if (uEntryLevel<0)	uEntryLevel=	0;
    for (i=0;i<uEntryLevel;i++) {
	putc(' ',entryFile);
    }
    fprintf(entryFile,"---> 0x%p\n",rtVal);
    return;
}

/***====================================================================***/
/***			PRINT FUNCTIONS					***/
/***====================================================================***/

#ifndef Lynx
	FILE	*uDebugFile=		stderr;
#else
	FILE	*uDebugFile=		NULL;
#endif
	int	 uDebugIndentLevel=	0;
	int	 uDebugIndentSize=	4;

Boolean
uSetDebugFile(name)
    char *name;
{
    if ((uDebugFile!=NULL)&&(uDebugFile!=stderr)) {
	fprintf(uDebugFile,"switching to %s\n",name?name:"stderr");
	fclose(uDebugFile);
    }
    if (name!=NullString)	uDebugFile=	fopen(name,"w");
    else			uDebugFile=	stderr;
    if (uDebugFile==NULL) {
	uDebugFile=	stderr;
	return(False);
    }
    return(True);
}

void
uDebug(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
int	i;

    for (i=(uDebugIndentLevel*uDebugIndentSize);i>0;i--) {
	putc(' ',uDebugFile);
    }
    fprintf(uDebugFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(uDebugFile);
    return;
}

void
uDebugNOI(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
int	i;

    fprintf(uDebugFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(uDebugFile);
    return;
}

/***====================================================================***/

#ifndef Lynx
static	FILE	*errorFile=	stderr;
#else
static	FILE	*errorFile=	NULL;
#endif

Boolean
uSetErrorFile(name)
    char *name;
{
    if ((errorFile!=NULL)&&(errorFile!=stderr)) {
	fprintf(errorFile,"switching to %s\n",name?name:"stderr");
	fclose(errorFile);
    }
    if (name!=NullString)	errorFile=	fopen(name,"w");
    else			errorFile=	stderr;
    if (errorFile==NULL) {
	errorFile=	stderr;
	return(False);
    }
    return(True);
}

void
uInformation(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(errorFile);
    return;
}

/***====================================================================***/

void
uAction(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
    fprintf(errorFile,"                  ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(errorFile);
    return;
}

/***====================================================================***/

void
uWarning(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
    fprintf(errorFile,"Warning:          ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(errorFile);
    return;
}

/***====================================================================***/

void
uError(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
    fprintf(errorFile,"Error:            ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(errorFile);
    return;
}

/***====================================================================***/

void
uFatalError(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
    fprintf(errorFile,"Fatal Error:      ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fprintf(errorFile,"                  Exiting\n");
    fflush(errorFile);
    exit(1);
    /* NOTREACHED */
}

/***====================================================================***/

void
uInternalError(s,a1,a2,a3,a4,a5,a6,a7,a8)
char *s;
Opaque a1,a2,a3,a4,a5,a6,a7,a8;
{
    fprintf(errorFile,"Internal error:   ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6,a7,a8);
    fflush(errorFile);
    return;
}

/***====================================================================***/

#ifndef HAVE_STRDUP
char *
uStringDup(str)
    char *str;
{
char *rtrn;

    if (str==NULL)
	return NULL;
    rtrn= (char *)uAlloc(strlen(str)+1);
    strcpy(rtrn,str);
    return rtrn;
}
#endif

#ifndef HAVE_STRCASECMP
int
uStrCaseCmp(str1, str2)
    char *str1, *str2;
{
    char buf1[512],buf2[512];
    char c, *s;
    register int n;

    for (n=0, s = buf1; c = *str1++; n++) {
	if (isupper(c))
	    c = tolower(c);
	if (n>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    for (n=0, s = buf2; c = *str2++; n++) {
	if (isupper(c))
	    c = tolower(c);
	if (n>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    return (strcmp(buf1, buf2));
}

int
uStrCasePrefix(prefix, str)
    char *prefix, *str;
{
    char c1, *s1;
    char c2, *s2;
    while (((c1=*prefix)!='\0')&&((c2=*str)!='\0')) {
	if (isupper(c1))	c1= tolower(c1);
	if (isupper(c2))	c2= tolower(c2);
	if (c1!=c2)
	    return 0;
	prefix++; str++;
    }
    if (c1!='\0')
	return 0;
    return 1;
}

#endif
