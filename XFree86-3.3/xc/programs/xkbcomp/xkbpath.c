/* $XConsortium: xkbpath.c /main/4 1996/01/14 16:48:53 kaleb $ */
/* $XFree86: xc/programs/xkbcomp/xkbpath.c,v 3.1 1996/08/26 10:52:31 dawes Exp $ */
/************************************************************
 Copyright (c) 1994 by Silicon Graphics Computer Systems, Inc.

 Permission to use, copy, modify, and distribute this
 software and its documentation for any purpose and without
 fee is hereby granted, provided that the above copyright
 notice appear in all copies and that both that copyright
 notice and this permission notice appear in supporting
 documentation, and that the name of Silicon Graphics not be 
 used in advertising or publicity pertaining to distribution 
 of the software without specific prior written permission.
 Silicon Graphics makes no representation about the suitability 
 of this software for any purpose. It is provided "as is"
 without any express or implied warranty.
 
 SILICON GRAPHICS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS 
 SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY 
 AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL SILICON
 GRAPHICS BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL 
 DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, 
 DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE 
 OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION  WITH
 THE USE OR PERFORMANCE OF THIS SOFTWARE.

 ********************************************************/

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#define	DEBUG_VAR_NOT_LOCAL
#define	DEBUG_VAR debugFlags
#include "utils.h"
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif
#include <X11/extensions/XKM.h>
#include "xkbpath.h"

#ifndef DFLT_XKB_CONFIG_ROOT
#define DFLT_XKB_CONFIG_ROOT	"/usr/lib/X11/xkb"
#endif

#ifndef PATH_MAX
#define	PATH_MAX 1024
#endif

#define	PATH_CHUNK	8

static	int	 longestPath;
static	int	 szPath;
static	int	 nPathEntries;
static	char **	 includePath;

Bool
#if NeedFunctionPrototypes
XkbParseIncludeMap(char **str_inout,char **file_rtrn,char **map_rtrn,
							char *nextop_rtrn)
#else
XkbParseIncludeMap(str_inout,file_rtrn,map_rtrn,nextop_rtrn)
    char **	str_inout;
    char **	file_rtrn;
    char **	map_rtrn;
    char *	nextop_rtrn;
#endif
{
char *tmp,*str,*next;

    str= *str_inout;
    if ((*str=='+')||(*str=='|')) {
	*file_rtrn= *map_rtrn= NULL;
	*nextop_rtrn= *str;
	next= str+1;
    }
    else if (*str=='%') {
	*file_rtrn= *map_rtrn= NULL;
	*nextop_rtrn= str[1];
	next= str+2;
    }
    else {
	next= strpbrk(str,"|+");
	if (next) {
	    *nextop_rtrn= *next;
	    *next++= '\0';
	}
	else {
	    *nextop_rtrn= '\0';
	    next= NULL;
	}
	tmp= strchr(str,'(');
	if (tmp==NULL) {
	    *file_rtrn= uStringDup(str);
	    *map_rtrn= NULL;
	}
	else if (str[0]=='(') {
	    return False;
	}
	else {
	    *tmp++= '\0';
	    *file_rtrn= uStringDup(str);
	    str= tmp;
	    tmp= strchr(str,')');
	    if ((tmp==NULL)||(tmp[1]!='\0')) {
		uFree(*file_rtrn);
		return False;
	    }
	    *tmp++= '\0';
	    *map_rtrn= uStringDup(str);
	}
    }
    if (*nextop_rtrn=='\0')
	*str_inout= NULL;
    else if ((*nextop_rtrn=='|')||(*nextop_rtrn=='+'))
	*str_inout= next;
    else return False;
    return True;
}

Bool
#if NeedFunctionPrototypes
XkbInitIncludePath(void)
#else
XkbInitIncludePath()
#endif
{
    szPath= PATH_CHUNK;
    includePath=  (char **)calloc(szPath,sizeof(char *));
    if (includePath==NULL)
	return False;
    XkbAddDirectoryToPath(".");
    XkbAddDirectoryToPath(DFLT_XKB_CONFIG_ROOT);
    return True;
}

void
#if NeedFunctionPrototypes
XkbClearIncludePath(void)
#else
XkbClearIncludePath()
#endif
{
register int i;

    if (szPath>0) {
	for (i=0;i<nPathEntries;i++) {
	    if (includePath[i]!=NULL) {
		uFree(includePath[i]);
		includePath[i]= NULL;
	    }
	}
	nPathEntries= 0;
	longestPath= 0;
    }
    return;
}

Bool
#if NeedFunctionPrototypes
XkbAddDirectoryToPath(char *dir)
#else
XkbAddDirectoryToPath(dir)
    char	*dir;
#endif
{
int	len;
    if ((dir==NULL)||(dir[0]=='\0')) {
	XkbClearIncludePath();
	return True;
    }
#ifdef __EMX__
    dir = (char*)__XOS2RedirRoot(dir);
#endif
    len= strlen(dir);
    if (len+2>=PATH_MAX) { /* allow for '/' and at least one character */
	ERROR2("Path entry (%s) too long (maxiumum length is %d)\n",
							dir,PATH_MAX-3);
	return False;
    }
    if (len>longestPath)
	longestPath= len;
    if (nPathEntries>=szPath) {
	szPath+= PATH_CHUNK;
	includePath= (char **)realloc(includePath,szPath*sizeof(char *));
	if (includePath==NULL) {
	    WSGO("Allocation failed (includePath)\n");
	    return False;
	}
    }
    includePath[nPathEntries]= (char *)calloc(strlen(dir)+1,sizeof(char));
    if (includePath[nPathEntries]==NULL) {
	WSGO1("Allocation failed (includePath[%d])\n",nPathEntries);
	return False;
    }
    strcpy(includePath[nPathEntries++],dir);
    return True;
}

/***====================================================================***/

char *
#if NeedFunctionPrototypes
XkbDirectoryForInclude(unsigned type)
#else
XkbDirectoryForInclude(type)
    unsigned	type;
#endif
{
static char buf[32];

    switch (type) {
	case XkmSemanticsFile:
	    strcpy(buf,"semantics");
	    break;
	case XkmLayoutFile:
	    strcpy(buf,"layout");
	    break;
	case XkmKeymapFile:
	    strcpy(buf,"keymap");
	    break;
	case XkmKeyNamesIndex:
	    strcpy(buf,"keycodes");
	    break;
	case XkmTypesIndex:
	    strcpy(buf,"types");
	    break;
	case XkmSymbolsIndex:
	    strcpy(buf,"symbols");
	    break;
	case XkmCompatMapIndex:
	    strcpy(buf,"compat");
	    break;
	case XkmGeometryFile:
	case XkmGeometryIndex:
	    strcpy(buf,"geometry");
	    break;
	default:
	    strcpy(buf,"");
	    break;
    }
    return buf;
}

/***====================================================================***/

typedef struct _FileCacheEntry {
	char *				name;
	unsigned			type;
	char *				path;
	void *				data;
	struct _FileCacheEntry *	next;
} FileCacheEntry;
static FileCacheEntry	*fileCache;

void *
#if NeedFunctionPrototypes
XkbAddFileToCache(char *name,unsigned type,char *path,void *data)
#else
XkbAddFileToCache(name,type,path,data)
    char *	name;
    unsigned	type;
    char *	path;
    void *	data;
#endif
{
FileCacheEntry	*entry;

    for (entry=fileCache;entry!=NULL;entry=entry->next) {
	if ((type==entry->type)&&(uStringEqual(name,entry->name))) {
	    void *old= entry->data;
	    WSGO2("Replacing file cache entry (%s/%d)\n",name,type);
	    entry->path= path;
	    entry->data= data;
	    return old;
	}
    }
    entry= uTypedAlloc(FileCacheEntry);
    if (entry!=NULL) {
	entry->name= name;
	entry->type= type;
	entry->path= path;
	entry->data= data;
	entry->next= fileCache;
	fileCache= entry;
    }
    return NULL;
}

void *
#if NeedFunctionPrototypes
XkbFindFileInCache(char *name,unsigned type,char **pathRtrn)
#else
XkbFindFileInCache(name,type,pathRtrn)
    char *	name;
    unsigned	type;
    char **	pathRtrn;
#endif
{
FileCacheEntry	*entry;

    for (entry=fileCache;entry!=NULL;entry=entry->next) {
	if ((type==entry->type)&&(uStringEqual(name,entry->name))) {
	   *pathRtrn= entry->path;
	   return entry->data;
	}
    }
    return NULL;
}

/***====================================================================***/

FILE *
#if NeedFunctionPrototypes
XkbFindFileInPath(char *name,unsigned type,char **pathRtrn)
#else
XkbFindFileInPath(name,type,pathRtrn)
    char *	name;
    unsigned	type;
    char **	pathRtrn;
#endif
{
register int i;
FILE	*file = 0;
int	 nameLen,typeLen,pathLen;
char	 buf[PATH_MAX],*typeDir;

    typeDir= XkbDirectoryForInclude(type);
    nameLen= strlen(name);
    typeLen= strlen(typeDir);
    for (i=0;i<nPathEntries;i++) {
	pathLen= strlen(includePath[i]);
	if ((nameLen+pathLen+1)>=PATH_MAX) {
	    ERROR2("File name (%s/%s) too long\n",includePath[i],name);
	    ACTION("Ignored\n");
	    continue;
	}
	sprintf(buf,"%s/%s",includePath[i],name);
	file= fopen(buf,"r");
	if (file!=NULL)
	     break;
	else if (typeLen<1)
	     continue;

	if ((nameLen+typeLen+pathLen+2)>=PATH_MAX) {
	    ERROR3("File name (%s/%s/%s) too long\n",includePath[i],typeDir,
	    							    name);
	    ACTION("Ignored\n");
	    continue;
	}
	sprintf(buf,"%s/%s/%s",includePath[i],typeDir,name);
	file= fopen(buf,"r");
	if (file!=NULL)
	    break;
    }

    if ((file!=NULL)&&(pathRtrn!=NULL)) {
	*pathRtrn= (char *)calloc(strlen(buf)+1,sizeof(char));
	if (*pathRtrn!=NULL)
	    strcpy(*pathRtrn,buf);
    }
    return file;
}

