#ifndef _XKBRULES_H_
#define	_XKBRULES_H_ 1

/* $XConsortium: XKBrules.h /main/1 1996/08/31 12:45:06 kaleb $ */
/************************************************************
 Copyright (c) 1996 by Silicon Graphics Computer Systems, Inc.

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

/***====================================================================***/

typedef struct _XkbRF_VarDefs {
	char *			model;
	char *			layout;
	char *			variant;
	char *			options;
	unsigned short		sz_extra;
	unsigned short		num_extra;
	char *			extra_names;
	char **			extra_values;
} XkbRF_VarDefsRec,*XkbRF_VarDefsPtr;

typedef struct _XkbRF_VarDesc {
	char *			name;
	char *			desc;
} XkbRF_VarDescRec, *XkbRF_VarDescPtr;

typedef struct _XkbRF_DescribeVars {
	int			sz_desc;
	int			num_desc;
	XkbRF_VarDescPtr	desc;
} XkbRF_DescribeVarsRec,*XkbRF_DescribeVarsPtr;

typedef struct _XkbRF_Rule {
	char *			model;
	char *			layout;
	char *			variant;
	char *			option;
	/* yields */
	char *			keycodes;
	char *			symbols;
	char *			types;
	char *			compat;
	char *			geometry;
	char *			keymap;
	unsigned		flags;
} XkbRF_RuleRec,*XkbRF_RulePtr;

#define	XkbRF_PendingMatch	(1L<<1)
#define	XkbRF_Delayed		(1L<<2)
#define	XkbRF_Append		(1L<<3)
#define	XkbRF_Invalid		(1L<<4)

typedef struct _XkbRF_Rules {
	XkbRF_DescribeVarsRec	models;
	XkbRF_DescribeVarsRec	layouts;
	XkbRF_DescribeVarsRec	variants;
	XkbRF_DescribeVarsRec	options;
	unsigned short		sz_extra;
	unsigned short		num_extra;
	char **			extra_names;
	XkbRF_DescribeVarsPtr	extra;

	unsigned short		sz_rules;
	unsigned short		num_rules;
	XkbRF_RulePtr		rules;
} XkbRF_RulesRec, *XkbRF_RulesPtr;

/***====================================================================***/

_XFUNCPROTOBEGIN

extern Bool	XkbRF_ApplyRule(
#if NeedFunctionPrototypes
    XkbRF_RulePtr		/* rule */,
    XkbComponentNamesPtr	/* names */
#endif
);

extern Bool	XkbRF_CheckApplyRule(
#if NeedFunctionPrototypes
    XkbRF_RulePtr		/* rule */,
    XkbRF_VarDefsPtr		/* defs */,
    XkbComponentNamesPtr	/* names */
#endif
);

extern void	XkbRF_ClearPartialMatches(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr		/* rules */
#endif
);

extern Bool	XkbRF_ApplyPartialMatches(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr		/* rules */,
    XkbComponentNamesPtr	/* names */
#endif
);

extern void	XkbRF_CheckApplyDelayedRules(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr		/* rules */,
    XkbRF_VarDefsPtr		/* defs */,
    XkbComponentNamesPtr	/* names */
#endif
);

extern Bool	XkbRF_CheckApplyRules(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr		/* rules */,
    XkbRF_VarDefsPtr		/* defs */,
    XkbComponentNamesPtr	/* names */
#endif
);

extern char *	XkbRF_SubstituteVars(
#if NeedFunctionPrototypes
    char *		/* p_name */,
    XkbRF_VarDefsPtr 	/* defs */
#endif
);

extern Bool	XkbRF_GetComponents(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr		/* rules */,
    XkbRF_VarDefsPtr		/* var_defs */,
    XkbComponentNamesPtr	/* names */
#endif
);

extern XkbRF_RulePtr	XkbRF_AddRule(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr	/* rules */
#endif
);

extern Bool	XkbRF_LoadRules(
#if NeedFunctionPrototypes
    FILE *		/* file */,
    XkbRF_RulesPtr	/* rules */
#endif
);

extern Bool XkbRF_LoadRulesByName(
#if NeedFunctionPrototypes
    char *		/* base */,
    char *		/* locale */,
    XkbRF_RulesPtr	/* rules */
#endif
);

/***====================================================================***/

extern XkbRF_VarDescPtr	XkbRF_AddVarDesc(
#if NeedFunctionPrototypes
    XkbRF_DescribeVarsPtr	/* vars */
#endif
);

extern XkbRF_VarDescPtr	XkbRF_AddVarDescCopy(
#if NeedFunctionPrototypes
    XkbRF_DescribeVarsPtr	/* vars */,
    XkbRF_VarDescPtr		/* copy_from */
#endif
);

extern XkbRF_DescribeVarsPtr XkbRF_AddVarToDescribe(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr		/* rules */,
    char *			/* name */
#endif
);

extern Bool	XkbRF_LoadDescriptions(
#if NeedFunctionPrototypes
    FILE *		/* file */,
    XkbRF_RulesPtr	/* rules */
#endif
);

extern Bool XkbRF_LoadDescriptionsByName(
#if NeedFunctionPrototypes
    char *		/* base */,
    char *		/* locale */,
    XkbRF_RulesPtr	/* rules */
#endif
);

extern XkbRF_RulesPtr XkbRF_Load(
#if NeedFunctionPrototypes
    char *		/* base */,
    char *		/* locale */,
    Bool		/* wantDesc */,
    Bool		/* wantRules */
#endif
);

extern XkbRF_RulesPtr XkbRF_Create(
#if NeedFunctionPrototypes
    int			/* sz_rules */,
    int			/* sz_extra */
#endif
);

/***====================================================================***/

extern void XkbRF_Free(
#if NeedFunctionPrototypes
    XkbRF_RulesPtr	/* rules */,
    Bool		/* freeRules */
#endif
);


/***====================================================================***/

#define	_XKB_RF_NAMES_PROP_ATOM		"_XKB_RULES_NAMES"
#define	_XKB_RF_NAMES_PROP_MAXLEN	1024

#ifndef XKB_IN_SERVER

extern Bool XkbRF_GetNamesProp(
#if NeedFunctionPrototypes
   Display *		/* dpy */,
   char **		/* rules_file_rtrn */,
   XkbRF_VarDefsPtr	/* var_defs_rtrn */
#endif
);

extern Bool XkbRF_SetNamesProp(
#if NeedFunctionPrototypes
   Display *		/* dpy */,
   char *		/* rules_file */,
   XkbRF_VarDefsPtr	/* var_defs */
#endif
);

#endif

_XFUNCPROTOEND

#endif /* _XKBRULES_H_ */
