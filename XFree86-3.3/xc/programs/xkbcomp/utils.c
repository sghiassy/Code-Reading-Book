
  /*\
   * $XConsortium: utils.c /main/3 1996/01/14 16:48:22 kaleb $
   * $XFree86: xc/programs/xkbcomp/utils.c,v 3.3 1996/08/26 10:52:25 dawes Exp $
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

#if NeedVarargsPrototypes
#include <stdarg.h>
#endif

/***====================================================================***/

Opaque
#if NeedFunctionPrototypes
uAlloc(unsigned size)
#else
uAlloc(size)
    unsigned	size;
#endif
{
    return((Opaque)malloc(size));
}

/***====================================================================***/

Opaque
#if NeedFunctionPrototypes
uCalloc(unsigned n,unsigned size)
#else
uCalloc(n,size)
    unsigned	n;
    unsigned	size;
#endif
{
    return((Opaque)calloc(n,size));
}

/***====================================================================***/

Opaque
#if NeedFunctionPrototypes
uRealloc(Opaque old,unsigned newSize)
#else
uRealloc(old,newSize)
    Opaque	old;
    unsigned	newSize;
#endif
{
    if (old==NULL)
	 return((Opaque)malloc(newSize));
    else return((Opaque)realloc((char *)old,newSize));
}

/***====================================================================***/

Opaque
#if NeedFunctionPrototypes
uRecalloc(Opaque old,unsigned nOld,unsigned nNew,unsigned itemSize)
#else
uRecalloc(old,nOld,nNew,itemSize)
    Opaque	old;
    unsigned	nOld;
    unsigned	nNew;
    unsigned	itemSize;
#endif
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
#if NeedFunctionPrototypes
uFree(Opaque ptr)
#else
uFree(ptr)
    Opaque ptr;
#endif
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
#if NeedFunctionPrototypes
uSetEntryFile(char *name)
#else
uSetEntryFile(name)
    char *name;
#endif
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

#if NeedVarargsPrototypes
void
uEntry(int l,char *s,...)
{
int	i;
va_list args;

    for (i=0;i<uEntryLevel;i++) {
	putc(' ',entryFile);
    }
    va_start(args, s);
    vfprintf(entryFile,s,args);
    va_end(args);
    uEntryLevel+= l;
}
#else
void
uEntry(l,s,a1,a2,a3,a4,a5,a6)
int	l;
char	*s;
Opaque	a1,a2,a3,a4,a5,a6;
{
int	i;

    for (i=0;i<uEntryLevel;i++) {
	putc(' ',entryFile);
    }
    fprintf(entryFile,s,a1,a2,a3,a4,a5,a6);
    uEntryLevel+= l;
    return;
}
#endif

void
#if NeedFunctionPrototypes
uExit(int l,char *rtVal)
#else
uExit(l,rtVal)
    int		l;
    char *	rtVal;
#endif
{
int	i;

    uEntryLevel-= l;
    if (uEntryLevel<0)	uEntryLevel=	0;
    for (i=0;i<uEntryLevel;i++) {
	putc(' ',entryFile);
    }
    fprintf(entryFile,"---> %p\n",rtVal);
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
#if NeedFunctionPrototypes
uSetDebugFile(char *name)
#else
uSetDebugFile(name)
    char *name;
#endif
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

#if NeedVarargsPrototypes
void
uDebug(char *s,...)
{
int	i;
va_list	args;

    for (i=(uDebugIndentLevel*uDebugIndentSize);i>0;i--) {
	putc(' ',uDebugFile);
    }
    va_start(args, s);
    vfprintf(uDebugFile,s,args);
    va_end(args);
    fflush(uDebugFile);
}
#else
void
uDebug(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
int	i;

    for (i=(uDebugIndentLevel*uDebugIndentSize);i>0;i--) {
	putc(' ',uDebugFile);
    }
    fprintf(uDebugFile,s,a1,a2,a3,a4,a5,a6);
    fflush(uDebugFile);
    return;
}
#endif

#if NeedVarargsPrototypes
void
uDebugNOI(char *s,...)
{
va_list args;

    va_start(args, s);
    vfprintf(uDebugFile,s,args);
    va_end(args);
    fflush(uDebugFile);
}
#else
void
uDebugNOI(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    fprintf(uDebugFile,s,a1,a2,a3,a4,a5,a6);
    fflush(uDebugFile);
    return;
}
#endif

/***====================================================================***/

#ifndef Lynx
static	FILE	*errorFile=	stderr;
#else
static	FILE	*errorFile=	NULL;
#endif
static	int	 outCount=	0;
static	char	*preMsg=	NULL;
static	char	*postMsg=	NULL;
static	char	*prefix=	NULL;

Boolean
#if NeedFunctionPrototypes
uSetErrorFile(char *name)
#else
uSetErrorFile(name)
    char *name;
#endif
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

#if NeedVarargsPrototypes
void
uInformation(char *s, ...)
{
va_list args;

    va_start(args, s);
    vfprintf(errorFile,s,args);
    va_end(args);
    fflush(errorFile);
}
#else
void
uInformation(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6);
    fflush(errorFile);
    return;
}
#endif

/***====================================================================***/

#if NeedVarargsPrototypes
void
uAction(char *s, ...)
{
va_list args;

    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"                  ");
    va_start(args, s);
    vfprintf(errorFile,s,args);
    va_end(args);
    fflush(errorFile);
}
#else
void
uAction(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"                  ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6);
    fflush(errorFile);
    return;
}
#endif

/***====================================================================***/

#if NeedVarargsPrototypes
void
uWarning(char *s, ...)
{
va_list args;

    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Warning:          ");
    va_start(args, s);
    vfprintf(errorFile,s,args);
    va_end(args);
    fflush(errorFile);
    outCount++;
}
#else
void
uWarning(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Warning:          ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6);
    fflush(errorFile);
    outCount++;
    return;
}
#endif

/***====================================================================***/

#if NeedVarargsPrototypes
void
uError(char *s, ...)
{
va_list args;

    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Error:            ");
    va_start(args, s);
    vfprintf(errorFile,s,args);
    va_end(args);
    fflush(errorFile);
    outCount++;
}
#else
void
uError(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Error:            ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6);
    fflush(errorFile);
    outCount++;
    return;
}
#endif

/***====================================================================***/

#if NeedVarargsPrototypes
void
uFatalError(char *s, ...)
{
va_list args;

    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Fatal Error:      ");
    va_start(args, s);
    vfprintf(errorFile,s,args);
    va_end(args);
    fprintf(errorFile,"                  Exiting\n");
    fflush(errorFile);
    outCount++;
    exit(1);
    /* NOTREACHED */
}
#else
void
uFatalError(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Fatal Error:      ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6);
    fprintf(errorFile,"                  Exiting\n");
    fflush(errorFile);
    outCount++;
    exit(1);
    /* NOTREACHED */
}
#endif

/***====================================================================***/

#if NeedVarargsPrototypes
void
uInternalError(char *s, ...)
{
va_list args;

    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Internal error:   ");
    va_start(args, s);
    vfprintf(errorFile,s,args);
    va_end(args);
    fflush(errorFile);
    outCount++;
}
#else
void
uInternalError(s,a1,a2,a3,a4,a5,a6)
char *s;
Opaque a1,a2,a3,a4,a5,a6;
{
    if ((outCount==0)&&(preMsg!=NULL))
	fprintf(errorFile,"%s\n",preMsg);
    if (prefix!=NULL)
	fprintf(errorFile,"%s",prefix);
    fprintf(errorFile,"Internal error:   ");
    fprintf(errorFile,s,a1,a2,a3,a4,a5,a6);
    fflush(errorFile);
    outCount++;
    return;
}
#endif

void
#if NeedFunctionPrototypes
uSetPreErrorMessage(char *msg)
#else
uSetPreErrorMessage(msg)
    char *msg;
#endif
{
    outCount= 0;
    preMsg= msg;
    return;
}

void
#if NeedFunctionPrototypes
uSetPostErrorMessage(char *msg)
#else
uSetPostErrorMessage(msg)
    char *msg;
#endif
{
    postMsg= msg;
    return;
}

void
#if NeedFunctionPrototypes
uSetErrorPrefix(char *pre)
#else
uSetErrorPrefix(pre)
    char *pre;
#endif
{
    prefix= pre;
    return;
}

void
#if NeedFunctionPrototypes
uFinishUp(void)
#else
uFinishUp()
#endif
{
    if ((outCount>0)&&(postMsg!=NULL))
	fprintf(errorFile,"%s\n",postMsg);
    return;
}

/***====================================================================***/

#ifndef HAVE_STRDUP
char *
#if NeedFunctionPrototypes
uStringDup(char *str)
#else
uStringDup(str)
    char *str;
#endif
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
#if NeedFunctionPrototypes
uStrCaseCmp(char *str1,char *str2)
#else
uStrCaseCmp(str1, str2)
    char *str1, *str2;
#endif
{
    char buf1[512],buf2[512];
    char c, *s;
    register int n;

    for (n=0, s = buf1; (c = *str1++) != '\0'; n++) {
	if (isupper(c))
	    c = tolower(c);
	if (n>510)
	    break;
	*s++ = c;
    }
    *s = '\0';
    for (n=0, s = buf2; (c = *str2++) != '\0'; n++) {
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
#if NeedFunctionPrototypes
uStrCasePrefix(char *my_prefix,char *str)
#else
uStrCasePrefix(my_prefix, str)
    char *my_prefix, *str;
#endif
{
    char c1;
    char c2;
    while (((c1=*my_prefix)!='\0')&&((c2=*str)!='\0')) {
	if (isupper(c1))	c1= tolower(c1);
	if (isupper(c2))	c2= tolower(c2);
	if (c1!=c2)
	    return 0;
	my_prefix++; str++;
    }
    if (c1!='\0')
	return 0;
    return 1;
}

#endif
