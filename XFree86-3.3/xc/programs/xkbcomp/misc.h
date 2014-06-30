/* $XConsortium: misc.h /main/4 1996/01/23 06:57:05 kaleb $ */
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

#ifndef MISC_H
#define MISC_H 1

typedef	struct	_CommonInfo {
    unsigned short		defined;
    unsigned char		fileID;
    unsigned char		merge;
    struct _CommonInfo *	next;
} CommonInfo;

_XFUNCPROTOBEGIN

extern Bool	UseNewField(
#if NeedFunctionPrototypes
    unsigned		/* field */,
    CommonInfo * 	/* oldDefs */,
    CommonInfo *	/* newDefs */,
    unsigned *		/* pCollide */
#endif
);

extern Bool	MergeNewField(
#if NeedFunctionPrototypes
    unsigned		/* field */,
    CommonInfo * 	/* oldDefs */,
    CommonInfo *	/* newDefs */,
    unsigned *		/* pCollide */
#endif
);

extern XPointer ClearCommonInfo(
#if NeedFunctionPrototypes
    CommonInfo *	/* cmn */
#endif
);

extern XPointer AddCommonInfo(
#if NeedFunctionPrototypes
    CommonInfo *	/* old */,
    CommonInfo *	/* new */
#endif
);

extern int	ReportNotArray(
#if NeedFunctionPrototypes
    char *	/* type */,
    char *	/* field */,
    char *	/* name */
#endif
);

extern int	ReportShouldBeArray(
#if NeedFunctionPrototypes
    char *	/* type */,
    char *	/* field */,
    char *	/* name */
#endif
);

extern int	ReportBadType(
#if NeedFunctionPrototypes
    char *	/* type */,
    char *	/* field */,
    char *	/* name */,
    char *	/* wanted */
#endif
);

extern int	ReportBadIndexType(
#if NeedFunctionPrototypes
    char *	/* type */,
    char *	/* field */,
    char *	/* name */,
    char *	/* wanted */
#endif
);

extern int	ReportBadField(
#if NeedFunctionPrototypes
    char *	/* type */,
    char *	/* field */,
    char *	/* name */
#endif
);

extern int	ReportMultipleDefs(
#if NeedFunctionPrototypes
    char *	/* type */,
    char *	/* field */,
    char *	/* which */
#endif
);

extern Bool	ProcessIncludeFile(
#if NeedFunctionPrototypes
    IncludeStmt	*	/* stmt */,
    unsigned		/* file_type */,
    XkbFile **		/* file_rtrn */,
    unsigned *		/* merge_rtrn */
#endif
);

extern Status	ComputeKbdDefaults(
#if NeedFunctionPrototypes
    XkbDescPtr		/* xkb */
#endif
);

extern Bool FindNamedKey(
#if NeedFunctionPrototypes
    XkbDescPtr		/* xkb */,
    unsigned long	/* name */,
    unsigned int *	/* kc_rtrn */,
    Bool		/* use_aliases */,
    Bool		/* create */
#endif
);

extern Bool FindKeyNameForAlias(
#if NeedFunctionPrototypes
    XkbDescPtr		/* xkb */,
    unsigned long	/* lname */,
    unsigned long *	/* real_name */
#endif
);

_XFUNCPROTOEND

#endif /* MISC_H */
