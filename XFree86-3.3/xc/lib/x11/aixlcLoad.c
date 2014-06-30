/* $XConsortium: aixlcLoad.c /main/12 1996/09/28 16:36:57 rws $ */
/*
 *
 * Copyright IBM Corporation 1993
 *
 * All Rights Reserved
 *
 * License to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS, AND 
 * NONINFRINGEMENT OF THIRD PARTY RIGHTS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
*/
/************************************************************************/
/*	XaixlcLoad.c							*/
/************************************************************************/
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include "Xlibint.h"
#include "Xaixlcint.h"

/************************************************************************/
/*	Private defines							*/
/************************************************************************/
enum {
    LDX_INVALID_TYPE,
    LDX_DYNAMIC,
    LDX_STATIC_SIM,
    LDX_STATIC_MIM
};

enum {
    LDX_INVALID_VERSION,
    LDX_R5,
    LDX_R6
};

typedef XLCd (*XLCdLoader)();
typedef _XlcCoreObj (*XdlEntry)();

#define	SECURE_PATH	"/usr/lib/nls/loc/X11"
#ifndef	LDXDBFILE
#define	LDXDBFILE	"ldx.dir"
#endif
#define	LDXFILE_SUFFIX	".ldx"

#ifndef	_POSIX_PATH_MAX
#define	_POSIX_PATH_MAX	1024
#endif

#define	iscomment(ch)	((ch) == '\0' || (ch) == '#')

/* standard AIX interfaces */
extern void *__lc_load();
extern int __issetuid();

extern int _XlcParsePath(char*, char**, int);

/************************************************************************/
/*	Private functions						*/
/************************************************************************/

static int
parse_line(line, argv, argsize)
    char *line;
    char **argv;
    int argsize;
{
    int argc = 0;
    char *p = line;

    while(argc < argsize){
	while(isspace(*p)){
	    ++p;
	}
	if(*p == '\0'){
	    break;
	}
	argv[argc++] = p;
	while(! isspace(*p)){
	    ++p;
	}
	if(*p == '\0'){
	    break;
	}
	*p++ = '\0';
    }

    return argc;
}

static int
read_ldxdb(lc_name, path, type, option)
    char *lc_name;
    char *path;
    int *type;
    char *option;
{
    char filename[_POSIX_PATH_MAX];
    FILE *fp;
    char buf[BUFSIZE];
    int found = 0;

    sprintf(filename, "%s/%s", path, LDXDBFILE);
    fp = fopen(filename, "r");
    if(fp == (FILE *)NULL){
	return 0;
    }

    while(fgets(buf, BUFSIZE, fp) != NULL){
	char *p = buf;
	int n;
	char *args[3];
	while(isspace(*p)){
	    ++p;
	}
	if(iscomment(*p)){
	    continue;
	}
	n = parse_line(p, args, 3);
	if(n == 0){
	    continue;
	}
	if(! strcmp(args[0], lc_name)){
	    char *str;
	    *type = LDX_STATIC_SIM;
	    if(n > 1){
		str = args[1];
		if(! _XlcCompareISOLatin1(str, "LDX_DYNAMIC")){
		    *type = LDX_DYNAMIC;
		}else if(! _XlcCompareISOLatin1(str, "LDX_STATIC_MIM")){
		    *type = LDX_STATIC_MIM;
		}
	    }
	    *option = '\0';
	    if(n > 2){
		str = args[2];
		if(! _XlcCompareISOLatin1(str, "NONE")){
		    ; /* ignore option */
		}else{
		    strcpy(option, str);
		}
	    }
	    found = 1;
	    break;
	}
    }
    fclose(fp);
    return found;
}

static int
resolve_ldxinfo(lc_name, type, option)
    char *lc_name;
    int *type;
    char *option;
{
    char buf[BUFSIZE], *dir;
    int i, n;
    char *args[256];

    dir = getenv("LDXDBPATH");
    if(dir != NULL){
	strncpy(buf, dir, BUFSIZE);
	buf[BUFSIZE-1] = '\0';
    }else{
	/* Resolve default path for database */
	_XlcResolveI18NPath(buf, BUFSIZE);
    }
    n = _XlcParsePath(buf, args, 256);
    for(i = 0; i < n; ++i){
	if(read_ldxdb(lc_name, args[i], type, option)){
	    if(*type == LDX_DYNAMIC && *option != '/'){
		/* the option should be absolute path */
		char tmp[_POSIX_PATH_MAX];
		sprintf(tmp, "%s/%s", args[i], option);
		strcpy(option, tmp);
	    }
	    return 1;
	}
    }
    return 0;
}

static int
get_ldxversion(ldx)
    _XlcCoreObj ldx;
{
    if(ldx == (_XlcCoreObj)NULL){
	return	LDX_INVALID_VERSION;
    }

#ifdef AIXV4
    if(ldx->lc_object_header.__type_id == _LC_LDX_R6 &&
       ldx->lc_object_header.__magic   == _LC_MAGIC &&
       ldx->lc_object_header.__version == _LC_VERSION_R6){
	return	LDX_R6;
    }

    if(ldx->lc_object_header.__type_id == _LC_LDX &&
       ldx->lc_object_header.__magic   == _LC_MAGIC &&
       ldx->lc_object_header.__version == _LC_VERSION){
	return	LDX_R5;
    }

#else
    if(ldx->lc_object_header.type_id == _LC_LDX_R6 &&
       ldx->lc_object_header.magic   == _LC_MAGIC &&
       ldx->lc_object_header.version == _LC_VERSION_R6){
	return	LDX_R6;
    }

    if(ldx->lc_object_header.type_id == _LC_LDX &&
       ldx->lc_object_header.magic   == _LC_MAGIC &&
       ldx->lc_object_header.version == _LC_VERSION){
	return	LDX_R5;
    }
#endif

    return LDX_INVALID_VERSION;
}

static int
complete_path(lc_name, path)
    char *lc_name;
    char *path;
{
    if(path == NULL){
	return 0;
    }
    if(*path == '\0' ||
#ifdef	never
       getuid() != geteuid() || getgid() != getegid()
#else
       __issetuid()
#endif
       ){
	/* use secure path */
	sprintf(path, "%s/%s%s", SECURE_PATH, lc_name, LDXFILE_SUFFIX);
    }else{
	int i, len;
	len = strlen(path);
	i = len - strlen(LDXFILE_SUFFIX);
	if(i < 0 || strcmp(path + i, LDXFILE_SUFFIX)){
	    if(path[len - 1] == '/'){
		strcpy(path + len, lc_name);
		len += strlen(lc_name);
	    }
	    strcpy(path + len, LDXFILE_SUFFIX);
	}
    }
    return (*path != '\0') ? 1 : 0;
}

/************************************************************************/

typedef struct _ldxList{
    char *path;
    XdlEntry entrypoint;
    _XlcCoreObj obj;
    int ref_count;
    struct _ldxList *next;
} ldxList;

static ldxList	*_ldxlist = (ldxList *)NULL;

static void
unload_ldx(obj)
    _XlcCoreObj obj;
{
    ldxList *pre, *ptr;

    for(pre = (ldxList *)NULL, ptr = _ldxlist; ptr != (ldxList *)NULL;
	pre = ptr, ptr = ptr->next){
	if(ptr->obj == obj){
	    if((-- ptr->ref_count) < 1){
		/* free and rechain cache list */
		Xfree(ptr->path);
		unload((void *)ptr->entrypoint);
		if(pre != (ldxList *)NULL){
		    pre->next = ptr->next;
		}else{
		    _ldxlist = ptr->next;
		}
		Xfree((char *)ptr);
	    }
	    return;
	}
    }
}

static void *
instantiate(path, p)
    char *path;
    void *(*p)();
{
    /* return entry point simply */
    return (void *)p;
}

static _XlcCoreObj
load_ldx(path)
    char *path;
{
    ldxList *ptr = (ldxList *)NULL;
    XdlEntry entry;
    _XlcCoreObj ldx = (_XlcCoreObj)NULL;

    for(ptr = _ldxlist; ptr != (ldxList *)NULL; ptr = ptr->next){
	if(! strcmp(path, ptr->path)){
	    ++ (ptr->ref_count);
	    return ptr->obj;
	}
    }

    /* dynamic load */
    /* the given path should be absolute path */
#ifdef	never
    entry = (XdlEntry)load(path, 1, NULL);
#else
    entry = (XdlEntry)__lc_load(path, instantiate);
#endif
    if(entry == (XdlEntry)NULL){
	goto err;
    }
    ldx = (*entry)();
    if(ldx == (_XlcCoreObj)NULL){
	goto err;
    }

    /* create cache list */
    ptr = (ldxList *)Xmalloc(sizeof(ldxList));
    if(ptr == (ldxList *)NULL){
	goto err;
    }
    ptr->path = Xmalloc(strlen(path) + 1);
    if(ptr->path == NULL){
	goto err;
    }
    strcpy(ptr->path, path);
    ptr->entrypoint = entry;
    ptr->obj = ldx;
    ptr->ref_count = 1;
    ptr->next = _ldxlist;
    _ldxlist = ptr;

    return ldx;

 err:;
    if(entry != (XdlEntry)NULL){
	unload((void *)entry);
    }
    if(ptr != (ldxList *)NULL){
	Xfree((char *)ptr);
    }
    return (_XlcCoreObj)NULL;
}

/************************************************************************/

typedef struct _lcdList {
    XLCd lcd;
    _XlcCoreObj ldx;
    XCloseLCProc close;
    struct _lcdList *next;
} lcdList;

static lcdList *_lcdlist = (lcdList *)NULL;

static void
close_xlcd(lcd)
    XLCd lcd;
{
    lcdList *pre, *ptr;

    for(pre = (lcdList *)NULL, ptr = _lcdlist; ptr != (lcdList *)NULL;
	pre = ptr, ptr = ptr->next){
	if(lcd == ptr->lcd){
	    /* free and rechain cache list */
	    (*ptr->close)(lcd);
	    unload_ldx(ptr->ldx);
	    if(pre != (lcdList *)NULL){
		pre->next = ptr->next;
	    }else{
		_lcdlist = ptr->next;
	    }
	    Xfree((char *)ptr);
	    return;
	}
    }
}

/************************************************************************/

#ifdef	STATIC_LOAD
extern XLCd _XaixlcSIMLoader();
extern XLCd _XaixlcMIMLoader();
#endif

static XLCd
instantiate_xlcd(lc_name)
    char *lc_name;
{
    int type;
    char path[_POSIX_PATH_MAX];
    _XlcCoreObj ldx = (_XlcCoreObj)NULL;
    XLCdLoader _XlcInstantiate = (XLCdLoader)NULL;
    XLCd lcd = (XLCd)NULL;

    if(! resolve_ldxinfo(lc_name, &type, path)){
	return (XLCd)NULL;
    }

    switch(type){
#ifdef	STATIC_LOAD
    case LDX_STATIC_SIM:
	_XlcInstantiate = _XaixlcSIMLoader;
	break;
    case LDX_STATIC_MIM:
	_XlcInstantiate = _XaixlcMIMLoader;
	break;
#endif
    case LDX_DYNAMIC:
	if(! complete_path(lc_name, path)){
	    break;
	}
	ldx = load_ldx(path);
	if(ldx == (_XlcCoreObj)NULL){
	    break;
	}
	switch(get_ldxversion(ldx)){
	case LDX_R5:
	    /* X11.5 LDX is not supported. */
	    break;
	case LDX_R6:
	    _XlcInstantiate = (XLCdLoader)ldx->default_loader;
	    break;
	default:
	    break;
	}
	break;
    }

    if(_XlcInstantiate != (XLCdLoader)NULL){
	lcd = (*_XlcInstantiate)(lc_name);
    }

    switch(type){
    case LDX_STATIC_SIM:
	break;
    case LDX_STATIC_MIM:
	break;
    case LDX_DYNAMIC:
	if(ldx != (_XlcCoreObj)NULL){
	    if(ldx->sticky){
		break;
	    }
	    if(lcd != (XLCd)NULL){
		lcdList *ptr = (lcdList *)Xmalloc(sizeof(lcdList));
		if(ptr != (lcdList *)NULL){
		    ptr->lcd = lcd;
		    ptr->ldx = ldx;
		    ptr->close = lcd->methods->close;
		    lcd->methods->close = (XCloseLCProc)close_xlcd;
		    ptr->next = _lcdlist;
		    _lcdlist = ptr;
		}else{
		    (*lcd->methods->close)(lcd);
		    lcd = (XLCd)NULL;
		}
	    }
	    if(lcd == (XLCd)NULL){
		/* fail to instantiate lcd */
		unload_ldx(ldx);
	    }
	}
	break;
    }

    return lcd;
}

/************************************************************************/
/*	_XaixOsDynamicLoad()						*/
/*----------------------------------------------------------------------*/
/*	This loader creates XLCd with using AIX dynamic loading 	*/
/*	feature.							*/
/************************************************************************/
XLCd
_XaixOsDynamicLoad(name)
    char *name;
{
    return instantiate_xlcd(name);
}
