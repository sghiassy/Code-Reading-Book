/* $XConsortium: lcFile.c /main/6 1996/09/28 16:37:56 rws $ */
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
/* $XFree86: xc/lib/X11/lcFile.c,v 3.6.2.3 1997/05/31 13:34:30 dawes Exp $ */
#include <stdio.h>
#include <ctype.h>
#include "Xlibint.h"
#include "XlcPubI.h"
#include <X11/Xos.h>
#ifdef X_NOT_STDC_ENV
extern char *getenv();
#endif

/************************************************************************/

#define	iscomment(ch)	((ch) == '#' || (ch) == '\0')
#define isreadable(f)	((access((f), R_OK) != -1) ? 1 : 0)
/*
#define	isspace(ch)	((ch) == ' ' || (ch) == '\t' || (ch) == '\n')
*/

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
	while(! isspace(*p) && *p != '\0'){
	    ++p;
	}
	if(*p == '\0'){
	    break;
	}
	*p++ = '\0';
    }

    return argc;
}

#ifndef __EMX__
#define LC_PATHDELIM ':'
#else
#define LC_PATHDELIM ';'
#endif


int
_XlcParsePath(path, argv, argsize)
    char *path;
    char **argv;
    int argsize;
{
    char *p = path;
    int i, n;

    while((p = strchr(p, LC_PATHDELIM)) != NULL){
	*p = ' ';	/* place space on delimter */
    }
    n = parse_line(path, argv, argsize);
    if(n == 0){
	return 0;
    }
    for(i = 0; i < n; ++i){
	int len;
	p = argv[i];
	len = strlen(p);
	if(p[len - 1] == '/'){
	    /* eliminate slash */
	    p[len - 1] = '\0';
	}
    }
    return n;
}

#ifndef XLOCALEDIR
#define XLOCALEDIR "/usr/lib/X11/locale"
#endif

static void
xlocaledir(buf, buf_len)
    char *buf;
    int buf_len;
{
    char *dir, *p = buf;
    int len = 0;

    dir = getenv("XLOCALEDIR");
    if(dir != NULL){
	len = strlen(dir);
	strncpy(p, dir, buf_len);
	if (len < buf_len) {
	    p[len++] = ':';
	    p += len;
	}
    }
    if (len < buf_len)
#ifndef __EMX__
	strncpy(p, XLOCALEDIR, buf_len - len);
#else
	strncpy(p,__XOS2RedirRoot(XLOCALEDIR), buf_len - len);
#endif
    buf[buf_len-1] = '\0';
}

enum { LtoR, RtoL };

static char *
_XlcResolveName(lc_name, file_name, direction)
    char *lc_name;
    char *file_name;
    int direction;	/* mapping direction */
{
    FILE *fp;
    char buf[BUFSIZE], *name = NULL;

    fp = fopen(file_name, "r");
    if(fp == (FILE *)NULL){
	return NULL;
    }

    while(fgets(buf, BUFSIZE, fp) != NULL){
	char *p = buf;
	int n;
	char *args[2], *from, *to;
#ifdef __EMX__  /* Take out CR under OS/2 */
	int len;

	len=strlen(p);
	if(len>1){
	   if(*(p+len-2) == '\r' && *(p+len-1) == '\n'){
	       *(p+len-2) = '\n';
	       *(p+len-1) = '\0';
	   }
	}
#endif
	while(isspace(*p)){
	    ++p;
	}
	if(iscomment(*p)){
	    continue;
	}
	n = parse_line(p, args, 2);		/* get first 2 fields */
	if(n != 2){
	    continue;
	}
	if(direction == LtoR){
	    from = args[0], to = args[1];	/* left to right */
	}else{
	    from = args[1], to = args[0];	/* right to left */
	}
	if(! strcmp(from, lc_name)){
	    name = Xmalloc(strlen(to) + 1);
	    if(name != NULL){
		strcpy(name, to);
	    }
	    break;
	}
    }
    if(fp != (FILE *)NULL){
	fclose(fp);
    }
    return name;
}

/*
#define	isupper(ch)	('A' <= (ch) && (ch) <= 'Z')
#define	tolower(ch)	((ch) - 'A' + 'a')
*/
static char *
lowercase(dst, src)
    char *dst;
    char *src;
{
    char *s, *t;

    for(s = src, t = dst; *s; ++s, ++t){
	*t = isupper(*s) ? tolower(*s) : *s;
    }
    *t = '\0';
    return dst;
}

/************************************************************************/
char *
_XlcFileName(lcd, category)
    XLCd lcd;
    char *category;
{
    char lc_name[BUFSIZE];
    char cat[BUFSIZE], dir[BUFSIZE];
    int i, n;
    char *args[256];
    char *file_name = NULL;

    if(lcd == (XLCd)NULL){
	return NULL;
    }

    if(! _XlcResolveLocaleName(XLC_PUBLIC(lcd, siname), lc_name,
			       NULL, NULL, NULL)){
	return NULL;
    }

    lowercase(cat, category);
    xlocaledir(dir,BUFSIZE);
    n = _XlcParsePath(dir, args, 256);
    for(i = 0; i < n; ++i){
	char buf[BUFSIZE], *name;

	name = NULL;
	if ((5 + (args[i] ? strlen (args[i]) : 0) +
	    (cat ? strlen (cat) : 0)) < BUFSIZE) {
	    sprintf(buf, "%s/%s.dir", args[i], cat);
	    name = _XlcResolveName(lc_name, buf, RtoL);
	}
	if(name == NULL){
	    continue;
	}
	if(*name == '/'){
	    /* supposed to be absolute path name */
	    file_name = name;
	}else{
	    file_name = Xmalloc(2 + (args[i] ? strlen (args[i]) : 0) +
				(name ? strlen (name) : 0));
	    if (file_name != NULL)
		sprintf(file_name, "%s/%s", args[i], name);
	    Xfree(name);
	}
	if(isreadable(file_name)){
	    break;
	}
	Xfree(file_name);
	file_name = NULL;
	/* Then, try with next dir */
    }
    return file_name;
}

/************************************************************************/
#ifndef LOCALE_ALIAS
#define LOCALE_ALIAS    "locale.alias"
#endif

int
_XlcResolveLocaleName(lc_name, full_name, language, territory, codeset)
    char *lc_name;
    char *full_name;
    char *language;
    char *territory;
    char *codeset;
{
    char dir[BUFSIZE], buf[BUFSIZE], *name = NULL;
    int i, n;
    char *args[256];
    static char locale_alias[] = LOCALE_ALIAS;

    xlocaledir(dir,BUFSIZE);
    n = _XlcParsePath(dir, args, 256);
    for(i = 0; i < n; ++i){
	if ((2 + (args[i] ? strlen (args[i]) : 0) + 
	    strlen (locale_alias)) < BUFSIZE) {
	    sprintf(buf, "%s/%s", args[i], locale_alias);
	    name = _XlcResolveName(lc_name, buf, LtoR);
	}
	if(name != NULL){
	    break;
	}
    }

    if(name != NULL){
	if(strlen(name) < BUFSIZE - 1){
	    strcpy(buf, name);
	}else{
	    fprintf(stderr, "Warning: locale \"%s\" is too long, ignored\n",
		    name);
	    *buf = '\0';
	}
	Xfree(name);
    }else{
	if(strlen(lc_name) < BUFSIZE - 1){
	    strcpy(buf, lc_name);
	}else{
	    fprintf(stderr, "Warning: locale \"%s\" is too long, ignored\n",
		    lc_name);
	    *buf = '\0';
	}
    }
    if(full_name != NULL){
	strcpy(full_name, buf);
    }

    if(language || territory || codeset){
	char *ptr, *name_p;
	/*
	 * Decompose locale name
	 */
	if(language) *language = '\0';
	if(territory) *territory = '\0';
	if(codeset) *codeset = '\0';

	name_p = buf;
	ptr = strchr(name_p, '_');
	if (!ptr)
	    ptr = strchr(name_p, '.');
	if (!ptr)
	    ptr = name_p + strlen(name_p);

	/*
	 * The size of the language, territory, codset buffers is 128 in
	 * initialize() in lcPublic.c
	 */
	if (language) {
	    if (ptr - name_p < 128) {
		strncpy(language, name_p, ptr - name_p);
		language[ptr - name_p] = '\0';
	    } else {
		fprintf(stderr,
	      "Warning: language part of locale \"%s\" is too long, ignored\n",
			buf);
	    }
	}
	if (*ptr == '_') {
	    name_p = ptr + 1;
	    ptr = strchr(name_p, '.');
	    if (!ptr)
		ptr = name_p + strlen(name_p);
	    if (territory) {
		if (ptr - name_p < 128) {
		    strncpy(territory, name_p, ptr - name_p);
		    territory[ptr - name_p] = '\0';
		} else {
		    fprintf(stderr,
	     "Warning: territory part of locale \"%s\" is too long, ignored\n",
			    buf);
		}
	    }
	}
	if (*ptr == '.') {
	    name_p = ptr + 1;
	    ptr = name_p + strlen(name_p);
	    if (codeset) {
		if (ptr - name_p < 128) {
		    strcpy(codeset, name_p);
		} else {
		    fprintf(stderr,
		"Warning: codeset part of locale \"%s\" is too long, ignored\n",
			    buf);
		}
	    }
	}
    }

    return (buf[0] != '\0') ? 1 : 0;
}

/************************************************************************/
#ifndef	LOCALE_DIR
#define	LOCALE_DIR	"locale.dir"
#endif

int
_XlcResolveDBName(lc_name, file_name)
    char *lc_name;
    char *file_name;
{
    char dir[BUFSIZE], buf[BUFSIZE], *name = NULL;
    int i, n;
    char *args[256];
    static char locale_dir[] = LOCALE_DIR;

    xlocaledir(dir,BUFSIZE);
    n = _XlcParsePath(dir, args, 256);
    for(i = 0; i < n; ++i){
	if ((2 + (args[i] ? strlen (args[i]) : 0) + 
	    strlen (locale_dir)) < BUFSIZE) {
	    sprintf(buf, "%s/%s", args[i], locale_dir);
	    name = _XlcResolveName(lc_name, buf, RtoL);
	}
	if(name != NULL){
	    break;
	}
    }
    if(name == NULL){
	return 0;
    }
    strcpy(buf, name);
    Xfree(name);
    if(file_name != NULL){
	strcpy(file_name, buf);
    }
    return 1;
}

/************************************************************************/
int
_XlcResolveI18NPath(buf, buf_len)
    char *buf;
    int buf_len;
{
    if(buf != NULL){
	xlocaledir(buf, buf_len);
    }
    return 1;
}
