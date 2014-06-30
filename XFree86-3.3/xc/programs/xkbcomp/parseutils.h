/* $XConsortium: parseutils.h /main/1 1996/02/02 14:17:30 kaleb $ */
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

#ifndef XKBPARSE_H
#define	XKBPARSE_H 1

#ifndef DEBUG_VAR
#define	DEBUG_VAR	parseDebug
#endif

#include "xkbcomp.h"

extern	char	*scanStr;
extern	int	 scanInt;
extern	int	 lineNum;

extern	XkbFile	*rtrnValue;

#ifdef DEBUG
#define	d(str)		fprintf(stderr,"%s\n",str);
#define d1(str,a)	fprintf(stderr,str,a);
#define d2(str,a,b)	fprintf(stderr,str,a,b);
#else
#define	d(str)
#define	d1(str,a)
#define d2(str,a,b)
#endif

_XFUNCPROTOBEGIN

extern ParseCommon *AppendStmt(
#if NeedFunctionPrototypes
    ParseCommon *	/* to */,
    ParseCommon *	/* append */
#endif
);

extern ExprDef *ExprCreate(
#if NeedFunctionPrototypes
    unsigned		/* op */,
    unsigned		/* type */
#endif
);

extern ExprDef *ExprCreateUnary(
#if NeedFunctionPrototypes
    unsigned	 	/* op */,
    unsigned	 	/* type */,
    ExprDef	*	/* child */
#endif
);

extern ExprDef *ExprCreateBinary(
#if NeedFunctionPrototypes
    unsigned	 	/* op */,
    ExprDef	*	/* left */,
    ExprDef	*	/* right */
#endif
);

extern KeycodeDef *KeycodeCreate(
#if NeedFunctionPrototypes
    char *		/* name */,
    ExprDef *		/* value */
#endif
);

extern KeyAliasDef *KeyAliasCreate(
#if NeedFunctionPrototypes
    char *		/* alias */,
    char *		/* real */
#endif
);

extern VModDef *VModCreate(
#if NeedFunctionPrototypes
    Atom		/* name */,
    ExprDef *		/* value */
#endif
);

extern VarDef *VarCreate(
#if NeedFunctionPrototypes
    ExprDef *		/* name */,
    ExprDef *		/* value */
#endif
);

extern VarDef *BoolVarCreate(
#if NeedFunctionPrototypes
    Atom		/* nameToken */,
    unsigned		/* set */
#endif
);

extern InterpDef *InterpCreate(
#if NeedFunctionPrototypes
    KeySym		/* sym */,
    ExprDef *		/* match */
#endif
);

extern KeyTypeDef *KeyTypeCreate(
#if NeedFunctionPrototypes
    Atom		/* name */,
    VarDef *		/* body */
#endif
);

extern SymbolsDef *SymbolsCreate(
#if NeedFunctionPrototypes
    char *		/* keyName */,
    ExprDef *		/* symbols */
#endif
);

extern GroupCompatDef *GroupCompatCreate(
#if NeedFunctionPrototypes
    int			/* group */,
    ExprDef *		/* def */
#endif
);

extern ModMapDef *ModMapCreate(
#if NeedFunctionPrototypes
    Atom		/* modifier */,
    ExprDef *		/* keys */
#endif
);

extern IndicatorMapDef *IndicatorMapCreate(
#if NeedFunctionPrototypes
    Atom		/* name */,
    VarDef *		/* body */
#endif
);

extern IndicatorNameDef *IndicatorNameCreate(
#if NeedFunctionPrototypes
    int			/* ndx */,
    ExprDef *		/* name */,
    Bool		/* virtual */
#endif
);

extern ExprDef *ActionCreate(
#if NeedFunctionPrototypes
    Atom	 	/* name */,
    ExprDef	*	/* args */
#endif
);

extern ExprDef *CreateKeysymList(
#if NeedFunctionPrototypes
    KeySym		/* sym */
#endif
);

extern ShapeDef *ShapeDeclCreate(
#if NeedFunctionPrototypes
    Atom		/* name */,
    OutlineDef *	/* outlines */
#endif
);

extern OutlineDef *OutlineCreate(
#if NeedFunctionPrototypes
    Atom		/* field */,
    ExprDef *		/* points */
#endif
);

extern KeyDef *KeyDeclCreate(
#if NeedFunctionPrototypes
    char *	/* name */,
    ExprDef *	/* expr */
#endif
);

extern KeyDef *KeyDeclMerge(
#if NeedFunctionPrototypes
    KeyDef *	/* into */,
    KeyDef *	/* from */
#endif
);

extern RowDef *RowDeclCreate(
#if NeedFunctionPrototypes
    KeyDef *	/* keys */
#endif
);

extern SectionDef *SectionDeclCreate(
#if NeedFunctionPrototypes
    Atom	/* name */,
    RowDef *	/* rows */
#endif
);

extern OverlayKeyDef *OverlayKeyCreate(
#if NeedFunctionPrototypes
    char *		/* under */,
    char *		/* over  */
#endif
);

extern OverlayDef *OverlayDeclCreate(
#if NeedFunctionPrototypes
    Atom		/* name */,
    OverlayKeyDef *	/* rows */
#endif
);

extern DoodadDef *DoodadCreate(
#if NeedFunctionPrototypes
    unsigned	/* type */,
    Atom	/* name */,
    VarDef *	/* body */
#endif
);

extern ExprDef *AppendKeysymList(
#if NeedFunctionPrototypes
    ExprDef *	/* list */,
    KeySym	/* sym */
#endif
);

extern int LookupKeysym(
#if NeedFunctionPrototypes
    char *	/* str */,
    KeySym *	/* sym_rtrn */
#endif
);

extern	IncludeStmt *IncludeCreate(
#if NeedFunctionPrototypes
    char *	/* str */,
    unsigned	/* merge */
#endif
);

#ifdef DEBUG
extern void PrintStmtAddrs(
#if NeedFunctionPrototypes
    ParseCommon *	/* stmt */
#endif
);
#endif

extern int XKBParseFile(
#if NeedFunctionPrototypes
FILE  *			/* file */,
    XkbFile **		/* pRtrn */
#endif
);

extern XkbFile *CreateXKBFile(
#if NeedFunctionPrototypes
    int			/* type */,
    char *		/* name */,
    ParseCommon *	/* defs */,
    unsigned		/* flags */
#endif
);

extern void yyerror(
#if NeedFunctionPrototypes
    char *		/* s */
#endif
);

extern int yywrap(
#if NeedFunctionPrototypes
    void
#endif
);

extern int setScanState(
#if NeedFunctionPrototypes
    char *	/* file */,
    int 	/* line */
#endif
);

_XFUNCPROTOEND

#endif /* XKBPARSE_H */
