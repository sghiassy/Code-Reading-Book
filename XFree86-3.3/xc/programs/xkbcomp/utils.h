#ifndef UTILS_H
#define	UTILS_H 1

  /*\
   * $XConsortium: utils.h /main/3 1996/01/14 16:48:26 kaleb $
   * $XFree86: xc/programs/xkbcomp/utils.h,v 3.1 1996/08/26 10:52:26 dawes Exp $
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

/***====================================================================***/

#include 	<stdio.h>
#include	<X11/Xos.h>
#include	<X11/Xfuncproto.h>
#include	<X11/Xfuncs.h>

#ifndef	NULL
#define	NULL	0
#endif

#ifndef NUL
#define	NUL	'\0'
#endif

/***====================================================================***/

#ifndef OPAQUE_DEFINED
typedef	void *Opaque;
#endif
#ifndef NullOpaque
#define	NullOpaque	((Opaque)NULL)
#endif

#ifndef BOOLEAN_DEFINED
typedef	char	Boolean;
#endif

#ifndef True
#define	True	((Boolean)1)
#define	False	((Boolean)0)
#endif /* ndef True */
#define	booleanText(b)	((b)?"True":"False")

#ifndef COMPARISON_DEFINED
typedef	int		Comparison;

#define	Greater		((Comparison)1)
#define	Equal		((Comparison)0)
#define	Less		((Comparison)-1)
#define	CannotCompare	((Comparison)-37)
#define	comparisonText(c)	((c)?((c)<0?"Less":"Greater"):"Equal")
#endif

_XFUNCPROTOBEGIN

#if 0
typedef union {
	int		 i;
	unsigned	 u;
	void		*p;
	void		*(*fp)();
} Union;
#endif

/***====================================================================***/

extern	Opaque	uAlloc(
#if NeedFunctionPrototypes
    unsigned	/* size */
#endif
);
extern	Opaque	uCalloc(
#if NeedFunctionPrototypes
    unsigned	/* n */,
    unsigned	/* size */
#endif
);
extern	Opaque	uRealloc(
#if NeedFunctionPrototypes
    Opaque	/* old */,
    unsigned	/* newSize */
#endif
);
extern	Opaque	uRecalloc(
#if NeedFunctionPrototypes
    Opaque 	/* old */,
    unsigned	/* nOld */,
    unsigned	/* nNew */,
    unsigned 	/* newSize */
#endif
);
extern	void	uFree(
#if NeedFunctionPrototypes
    Opaque	/* ptr */
#endif
);

#define	uTypedAlloc(t)		((t *)uAlloc((unsigned)sizeof(t)))
#define	uTypedCalloc(n,t)	((t *)uCalloc((unsigned)n,(unsigned)sizeof(t)))
#define	uTypedRealloc(pO,n,t)	((t *)uRealloc((Opaque)pO,((unsigned)n)*sizeof(t)))
#define	uTypedRecalloc(pO,o,n,t) ((t *)uRecalloc((Opaque)pO,((unsigned)o),((unsigned)n),sizeof(t)))
#if (defined mdHasAlloca) && (mdHasAlloca)
#define	uTmpAlloc(n)	((Opaque)alloca((unsigned)n))
#define	uTmpFree(p)
#else
#define	uTmpAlloc(n)	uAlloc(n)
#define	uTmpFree(p)	uFree(p)
#endif

/***====================================================================***/

extern	Boolean	uSetErrorFile(
#if NeedFunctionPrototypes
	char *	/* name */
#endif
);

#if NeedVarargsPrototypes
#define INFO6 			uInformation
#define INFO5 			uInformation
#define INFO4 			uInformation
#define INFO3 			uInformation
#define INFO2 			uInformation
#define INFO1 			uInformation
#define INFO 			uInformation
#else
#define	INFO6(s,a,b,c,d,e,f)	uInformation((s),(Opaque)(a),(Opaque)(b),\
						(Opaque)(c),(Opaque)(d),\
						(Opaque)(e),(Opaque)(f))
#define	INFO5(s,a,b,c,d,e)	INFO6(s,a,b,c,d,e,NULL)
#define	INFO4(s,a,b,c,d)	INFO5(s,a,b,c,d,NULL)
#define	INFO3(s,a,b,c)		INFO4(s,a,b,c,NULL)
#define	INFO2(s,a,b)		INFO3(s,a,b,NULL)
#define	INFO1(s,a)		INFO2(s,a,NULL)
#define	INFO(s)			INFO1(s,NULL)
#endif

extern	void	uInformation(
#if NeedVarargsPrototypes
	char * /* s */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

#if NeedVarargsPrototypes
#define ACTION6			uAction
#define ACTION5			uAction
#define ACTION4			uAction
#define ACTION3			uAction
#define ACTION2			uAction
#define ACTION1			uAction
#define ACTION			uAction
#else
#define	ACTION6(s,a,b,c,d,e,f)	uAction((s),(Opaque)(a),(Opaque)(b),\
						(Opaque)(c),(Opaque)(d),\
						(Opaque)(e),(Opaque)(f))
#define	ACTION5(s,a,b,c,d,e)	ACTION6(s,a,b,c,d,e,NULL)
#define	ACTION4(s,a,b,c,d)	ACTION5(s,a,b,c,d,NULL)
#define	ACTION3(s,a,b,c)	ACTION4(s,a,b,c,NULL)
#define	ACTION2(s,a,b)		ACTION3(s,a,b,NULL)
#define	ACTION1(s,a)		ACTION2(s,a,NULL)
#define	ACTION(s)		ACTION1(s,NULL)
#endif

extern	void	uAction(
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

#if NeedVarargsPrototypes
#define WARN6			uWarning
#define WARN5			uWarning
#define WARN4			uWarning
#define WARN3			uWarning
#define WARN2			uWarning
#define WARN1			uWarning
#define WARN			uWarning
#else
#define	WARN6(s,a,b,c,d,e,f)	uWarning((s),(Opaque)(a),(Opaque)(b),\
						(Opaque)(c),(Opaque)(d),\
						(Opaque)(e),(Opaque)(f))
#define	WARN5(s,a,b,c,d,e)	WARN6(s,a,b,c,d,e,NULL)
#define	WARN4(s,a,b,c,d)	WARN5(s,a,b,c,d,NULL)
#define	WARN3(s,a,b,c)		WARN4(s,a,b,c,NULL)
#define	WARN2(s,a,b)		WARN3(s,a,b,NULL)
#define	WARN1(s,a)		WARN2(s,a,NULL)
#define	WARN(s)			WARN1(s,NULL)
#endif

extern	void	uWarning(
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

#if NeedVarargsPrototypes
#define ERROR6			uError
#define ERROR5			uError
#define ERROR4			uError
#define ERROR3			uError
#define ERROR2			uError
#define ERROR1			uError
#define ERROR			uError
#else
#define	ERROR6(s,a,b,c,d,e,f)	uError((s),(Opaque)(a),(Opaque)(b),\
						(Opaque)(c),(Opaque)(d),\
						(Opaque)(e),(Opaque)(f))
#define	ERROR5(s,a,b,c,d,e)	ERROR6(s,a,b,c,d,e,NULL)
#define	ERROR4(s,a,b,c,d)	ERROR5(s,a,b,c,d,NULL)
#define	ERROR3(s,a,b,c)		ERROR4(s,a,b,c,NULL)
#define	ERROR2(s,a,b)		ERROR3(s,a,b,NULL)
#define	ERROR1(s,a)		ERROR2(s,a,NULL)
#define	ERROR(s)		ERROR1(s,NULL)
#endif

extern	void	uError(
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

#if NeedVarargsPrototypes
#define FATAL6			uFatalError
#define FATAL5			uFatalError
#define FATAL4			uFatalError
#define FATAL3			uFatalError
#define FATAL2			uFatalError
#define FATAL1			uFatalError
#define FATAL			uFatalError
#else
#define	FATAL6(s,a,b,c,d,e,f)	uFatalError((s),(Opaque)(a),(Opaque)(b),\
						(Opaque)(c),(Opaque)(d),\
						(Opaque)(e),(Opaque)(f))
#define	FATAL5(s,a,b,c,d,e)	FATAL6(s,a,b,c,d,e,NULL)
#define	FATAL4(s,a,b,c,d)	FATAL5(s,a,b,c,d,NULL)
#define	FATAL3(s,a,b,c)		FATAL4(s,a,b,c,NULL)
#define	FATAL2(s,a,b)		FATAL3(s,a,b,NULL)
#define	FATAL1(s,a)		FATAL2(s,a,NULL)
#define	FATAL(s)		FATAL1(s,NULL)
#endif

extern	void	uFatalError(
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

/* WSGO stands for "Weird Stuff Going On" */
#if NeedVarargsPrototypes
#define WSGO6			uInternalError
#define WSGO5			uInternalError
#define WSGO4			uInternalError
#define WSGO3			uInternalError
#define WSGO2			uInternalError
#define WSGO1			uInternalError
#define WSGO			uInternalError
#else
#define	WSGO6(s,a,b,c,d,e,f)	uInternalError((s),(Opaque)(a),(Opaque)(b),\
						(Opaque)(c),(Opaque)(d),\
						(Opaque)(e),(Opaque)(f))
#define	WSGO5(s,a,b,c,d,e)	WSGO6(s,a,b,c,d,e,NULL)
#define	WSGO4(s,a,b,c,d)	WSGO5(s,a,b,c,d,NULL)
#define	WSGO3(s,a,b,c)		WSGO4(s,a,b,c,NULL)
#define	WSGO2(s,a,b)		WSGO3(s,a,b,NULL)
#define	WSGO1(s,a)		WSGO2(s,a,NULL)
#define	WSGO(s)			WSGO1(s,NULL)
#endif

extern	void	uInternalError(
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

extern	void	uSetPreErrorMessage(
#if NeedFunctionPrototypes
	char * /* msg */
#endif
);

extern	void	uSetPostErrorMessage(
#if NeedFunctionPrototypes
	char * /* msg */
#endif
);

extern	void	uSetErrorPrefix(
#if NeedFunctionPrototypes
	char * /* void */
#endif
);

extern	void	uFinishUp(
#if NeedFunctionPrototypes
	void
#endif
);


/***====================================================================***/

#define	NullString	((char *)NULL)

#define	uStringText(s)		((s)==NullString?"<NullString>":(s))
#define	uStringEqual(s1,s2)	(uStringCompare(s1,s2)==Equal)
#define	uStringPrefix(p,s)	(strncmp(p,s,strlen(p))==0)
#define	uStringCompare(s1,s2)	(strcmp(s1,s2))
#define	uStrCaseEqual(s1,s2)	(uStrCaseCmp(s1,s2)==0)
#ifdef HAVE_STRCASECMP
#define	uStrCaseCmp(s1,s2)	(strcasecmp(s1,s2))
#define	uStrCasePrefix(p,s)	(strncasecmp(p,s,strlen(p))==0)
#else
extern	int uStrCaseCmp(
#if NeedFunctionPrototypes
	char *	/* s1 */,
	char *	/* s2 */
#endif
);
extern	int uStrCasePrefix(
#if NeedFunctionPrototypes
	char *	/* p */,
	char *	/* str */
#endif
);
#endif
#ifdef HAVE_STRDUP
#define	uStringDup(s1)		(strdup(s1))
#else
extern	char *uStringDup(
#if NeedFunctionPrototypes
	char *	/* s1 */
#endif
);
#endif

/***====================================================================***/

#ifdef	ASSERTIONS_ON
#define	uASSERT(where,why) \
	{if (!(why)) uFatalError("assertion botched in %s ( why )\n",where);}
#else
#define	uASSERT(where,why)
#endif

/***====================================================================***/

#ifndef DEBUG_VAR
#define	DEBUG_VAR	debugFlags
#endif

#ifdef DEBUG_VAR_NOT_LOCAL
extern
#endif
unsigned	int	DEBUG_VAR;

extern	void	uDebug(
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

extern	void	uDebugNOI(	/* no indent */
#if NeedVarargsPrototypes
	char * /* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 1, 2)))
#endif
;

extern	Boolean	uSetDebugFile(
#if NeedFunctionPrototypes
    char *name
#endif
);

extern	FILE	*uDebugFile;
extern	int	uDebugIndentLevel;
extern	int	uDebugIndentSize;
#define	uDebugIndent(l)		(uDebugIndentLevel+=(l))
#define	uDebugOutdent(l)	(uDebugIndentLevel-=(l))
#ifdef DEBUG_ON
#define	uDEBUG(f,s)		{ if (DEBUG_VAR&(f)) uDebug(s);}
#define	uDEBUG1(f,s,a)		{ if (DEBUG_VAR&(f)) uDebug(s,a);}
#define	uDEBUG2(f,s,a,b)	{ if (DEBUG_VAR&(f)) uDebug(s,a,b);}
#define	uDEBUG3(f,s,a,b,c)	{ if (DEBUG_VAR&(f)) uDebug(s,a,b,c);}
#define	uDEBUG4(f,s,a,b,c,d)	{ if (DEBUG_VAR&(f)) uDebug(s,a,b,c,d);}
#define	uDEBUG5(f,s,a,b,c,d,e)	{ if (DEBUG_VAR&(f)) uDebug(s,a,b,c,d,e);}
#define	uDEBUG_NOI(f,s)		{ if (DEBUG_VAR&(f)) uDebug(s);}
#define	uDEBUG_NOI1(f,s,a)	{ if (DEBUG_VAR&(f)) uDebugNOI(s,a);}
#define	uDEBUG_NOI2(f,s,a,b)	{ if (DEBUG_VAR&(f)) uDebugNOI(s,a,b);}
#define	uDEBUG_NOI3(f,s,a,b,c)	{ if (DEBUG_VAR&(f)) uDebugNOI(s,a,b,c);}
#define	uDEBUG_NOI4(f,s,a,b,c,d) { if (DEBUG_VAR&(f)) uDebugNOI(s,a,b,c,d);}
#define	uDEBUG_NOI5(f,s,a,b,c,d,e) { if (DEBUG_VAR&(f)) uDebugNOI(s,a,b,c,d,e);}
#else
#define	uDEBUG(f,s)
#define	uDEBUG1(f,s,a)
#define	uDEBUG2(f,s,a,b)
#define	uDEBUG3(f,s,a,b,c)
#define	uDEBUG4(f,s,a,b,c,d)
#define	uDEBUG5(f,s,a,b,c,d,e)
#define	uDEBUG_NOI(f,s)
#define	uDEBUG_NOI1(f,s,a)
#define	uDEBUG_NOI2(f,s,a,b)
#define	uDEBUG_NOI3(f,s,a,b,c)
#define	uDEBUG_NOI4(f,s,a,b,c,d)
#define	uDEBUG_NOI5(f,s,a,b,c,d,e)
#endif

extern	Boolean	uSetEntryFile(
#if NeedFunctionPrototypes
    char *name
#endif
);
extern	void	uEntry(
#if NeedVarargsPrototypes
	int	/* l */,
	char *	/* s  */, ...
#endif
)
#if __GNUC__-0 > 2 || (__GNUC__-0 == 2 && __GNUC_MINOR__ >= 6)
__attribute__((format(printf, 2, 3)))
#endif
;

extern	void	uExit(
#if NeedFunctionPrototypes
    int l,char *rtVal
#endif
);
#ifdef ENTRY_TRACKING_ON
#define	ENTRY_BIT	0x10
#define	LOW_ENTRY_BIT	0x1000
#define	ENTER	(DEBUG_VAR&ENTRY_BIT)
#define	FLAG(fLag)	(DEBUG_VAR&(fLag))

extern	int	uEntryLevel;

#define	uENTRY(s)			{ if (ENTER) uEntry(1,s);}
#define	uENTRY1(s,a)			{ if (ENTER) uEntry(1,s,a);}
#define	uENTRY2(s,a,b)			{ if (ENTER) uEntry(1,s,a,b);}
#define	uENTRY3(s,a,b,c)		{ if (ENTER) uEntry(1,s,a,b,c);}
#define	uENTRY4(s,a,b,c,d)		{ if (ENTER) uEntry(1,s,a,b,c,d);}
#define	uENTRY5(s,a,b,c,d,e)		{ if (ENTER) uEntry(1,s,a,b,c,d,e);}
#define	uENTRY6(s,a,b,c,d,e,f)		{ if (ENTER) uEntry(1,s,a,b,c,d,e,f);}
#define	uENTRY7(s,a,b,c,d,e,f,g)	{ if (ENTER) uEntry(1,s,a,b,c,d,e,f,g);}
#define	uRETURN(v)			{ if (ENTER) uEntryLevel--; return(v); }
#define	uVOIDRETURN			{ if (ENTER) uEntryLevel--; return; }

#define	uFLAG_ENTRY(w,s)		{ if (FLAG(w)) uEntry(0,s);}
#define	uFLAG_ENTRY1(w,s,a)		{ if (FLAG(w)) uEntry(0,s,a);}
#define	uFLAG_ENTRY2(w,s,a,b)		{ if (FLAG(w)) uEntry(0,s,a,b);}
#define	uFLAG_ENTRY3(w,s,a,b,c)		{ if (FLAG(w)) uEntry(0,s,a,b,c);}
#define	uFLAG_ENTRY4(w,s,a,b,c,d)	{ if (FLAG(w)) uEntry(0,s,a,b,c,d);}
#define	uFLAG_ENTRY5(w,s,a,b,c,d,e)	{ if (FLAG(w)) uEntry(0,s,a,b,c,d,e);}
#define	uFLAG_ENTRY6(w,s,a,b,c,d,e,f)	{ if (FLAG(w)) uEntry(0,s,a,b,c,d,e,f);}
#define	uFLAG_ENTRY7(w,s,a,b,c,d,e,f,g)	{ if(FLAG(w))uEntry(0,s,a,b,c,d,e,f,g);}
#define	uFLAG_RETURN(v)			{ return(v);}
#define	uFLAG_VOIDRETURN		{ return; }
#else
#define	uENTRY(s)
#define	uENTRY1(s,a)
#define	uENTRY2(s,a1,a2)
#define	uENTRY3(s,a1,a2,a3)
#define	uENTRY4(s,a1,a2,a3,a4)
#define	uENTRY5(s,a1,a2,a3,a4,a5)
#define	uENTRY6(s,a1,a2,a3,a4,a5,a6)
#define	uENTRY7(s,a1,a2,a3,a4,a5,a6,a7)
#define	uRETURN(v)	{ return(v); }
#define	uVOIDRETURN	{ return; }

#define	uFLAG_ENTRY(f,s)
#define	uFLAG_ENTRY1(f,s,a)
#define	uFLAG_ENTRY2(f,s,a,b)
#define	uFLAG_ENTRY3(f,s,a,b,c)
#define	uFLAG_ENTRY4(f,s,a,b,c,d)
#define	uFLAG_ENTRY5(f,s,a,b,c,d,e)
#define	uFLAG_ENTRY6(f,s,a,b,c,d,e,g)
#define	uFLAG_ENTRY7(f,s,a,b,c,d,e,g,h)
#define	uFLAG_RETURN(v)			{ return(v);}
#define	uFLAG_VOIDRETURN		{ return; }
#endif 

_XFUNCPROTOEND

#endif /* UTILS_H */


