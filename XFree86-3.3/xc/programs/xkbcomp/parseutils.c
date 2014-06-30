/* $XConsortium: parseutils.c /main/6 1996/02/02 14:17:26 kaleb $ */
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
#define DEBUG_VAR_NOT_LOCAL
#define	DEBUG_VAR parseDebug
#include "parseutils.h"
#include "xkbpath.h"
#include <X11/keysym.h>
#include <X11/extensions/XKBgeom.h>

XkbFile	*rtrnValue;

ParseCommon *
#if NeedFunctionPrototypes
AppendStmt(ParseCommon *to,ParseCommon *append)
#else
AppendStmt(to,append)
    ParseCommon *	to;
    ParseCommon *	append;
#endif
{
ParseCommon	*start= to;

    if (append==NULL)
	return to;
    while ((to!=NULL) && (to->next!=NULL)) {
	to= to->next;
    }
    if (to) {
	to->next= append;
	return start;
    }
    return append;
}

ExprDef *
#if NeedFunctionPrototypes
ExprCreate(unsigned op,unsigned type)
#else
ExprCreate(op,type)
    unsigned	op;
    unsigned	type;
#endif
{
ExprDef *expr;
    expr= uTypedAlloc(ExprDef);
    if (expr) {
	expr->common.stmtType= StmtExpr;
	expr->common.next= NULL;
	expr->op= op;
	expr->type= type;
    }
    else {
	FATAL("Couldn't allocate expression in parser\n");
	/* NOTREACHED */
    }
    return expr;
}

ExprDef *
#if NeedFunctionPrototypes
ExprCreateUnary(unsigned op,unsigned type,ExprDef *child)
#else
ExprCreateUnary(op,type,child)
    unsigned	 op;
    unsigned	 type;
    ExprDef	*child;
#endif
{
ExprDef *expr;
    expr= uTypedAlloc(ExprDef);
    if (expr) {
	expr->common.stmtType= StmtExpr;
	expr->common.next= NULL;
	expr->op= op;
	expr->type= type;
	expr->value.child= child;
    }
    else {
	FATAL("Couldn't allocate expression in parser\n");
	/* NOTREACHED */
    }
    return expr;
}

ExprDef *
#if NeedFunctionPrototypes
ExprCreateBinary(unsigned op,ExprDef *left,ExprDef *right)
#else
ExprCreateBinary(op,left,right)
    unsigned	 op;
    ExprDef	*left;
    ExprDef	*right;
#endif
{
ExprDef *expr;
    expr= uTypedAlloc(ExprDef);
    if (expr) {
	expr->common.stmtType= StmtExpr;
	expr->common.next= NULL;
	expr->op= op;
	if ((op==OpAssign)||(left->type==TypeUnknown))
	     expr->type= right->type;
	else if ((left->type==right->type)||(right->type==TypeUnknown))
	     expr->type= left->type;
	else expr->type= TypeUnknown;
	expr->value.binary.left= left;
	expr->value.binary.right= right;
    }
    else {
	FATAL("Couldn't allocate expression in parser\n");
	/* NOTREACHED */
    }
    return expr;
}

KeycodeDef *
#if NeedFunctionPrototypes
KeycodeCreate(char *name,ExprDef *value)
#else
KeycodeCreate(name,value)
    char *	name;
    ExprDef *	value;
#endif
{
KeycodeDef *def;

    def= uTypedAlloc(KeycodeDef);
    if (def) {
	def->common.stmtType= StmtKeycodeDef;
	def->common.next= NULL;
	strncpy(def->name,name,XkbKeyNameLength);
	def->name[XkbKeyNameLength]= '\0';
	def->value= value;
    }
    else {
	FATAL("Couldn't allocate key name definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

KeyAliasDef *
#if NeedFunctionPrototypes
KeyAliasCreate(char *alias,char *real)
#else
KeyAliasCreate(alias,real)
    char *	alias;
    char *	real;
#endif
{
KeyAliasDef *def;

    def= uTypedAlloc(KeyAliasDef);
    if (def) {
	def->common.stmtType= StmtKeyAliasDef;
	def->common.next= NULL;
	strncpy(def->alias,alias,XkbKeyNameLength);
	def->alias[XkbKeyNameLength]= '\0';
	strncpy(def->real,real,XkbKeyNameLength);
	def->real[XkbKeyNameLength]= '\0';
    }
    else {
	FATAL("Couldn't allocate key alias definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

VModDef *
#if NeedFunctionPrototypes
VModCreate(Atom name,ExprDef *value)
#else
VModCreate(name,value)
    Atom	name;
    ExprDef *	value;
#endif
{
VModDef *def;
    def= uTypedAlloc(VModDef);
    if (def) {
	def->common.stmtType= StmtVModDef;
	def->common.next= NULL;
	def->name= name;
	def->value= value;
    }
    else {
	FATAL("Couldn't allocate variable definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

VarDef *
#if NeedFunctionPrototypes
VarCreate(ExprDef *name,ExprDef *value)
#else
VarCreate(name,value)
    ExprDef *	name;
    ExprDef *	value;
#endif
{
VarDef *def;
    def= uTypedAlloc(VarDef);
    if (def) {
	def->common.stmtType= StmtVarDef;
	def->common.next= NULL;
	def->name= name;
	def->value= value;
    }
    else {
	FATAL("Couldn't allocate variable definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

VarDef *
#if NeedFunctionPrototypes
BoolVarCreate(Atom nameToken,unsigned set)
#else
BoolVarCreate(nameToken,set)
    Atom	nameToken;
    unsigned	set;
#endif
{
ExprDef	*name,*value;

    name= ExprCreate(ExprIdent,TypeUnknown);
    name->value.str= nameToken;
    value= ExprCreate(ExprValue,TypeBoolean);
    value->value.uval= set;
    return VarCreate(name,value);
}

InterpDef *
#if NeedFunctionPrototypes
InterpCreate(KeySym sym,ExprDef *match)
#else
InterpCreate(sym,match)
    KeySym	sym;
    ExprDef *	match;
#endif
{
InterpDef *def;

    def= uTypedAlloc(InterpDef);
    if (def) {
	def->common.stmtType= StmtInterpDef;
	def->common.next= NULL;
	def->sym= sym;
	def->match= match;
    }
    else {
	FATAL("Couldn't allocate interp definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

KeyTypeDef *
#if NeedFunctionPrototypes
KeyTypeCreate(Atom name,VarDef *body)
#else
KeyTypeCreate(name,body)
    Atom	name;
    VarDef *	body;
#endif
{
KeyTypeDef *def;

    def= uTypedAlloc(KeyTypeDef);
    if (def) {
	def->common.stmtType= StmtKeyTypeDef;
	def->common.next= NULL;
	def->merge= MergeDefault;
	def->name= name;
	def->body= body;
    }
    else {
	FATAL("Couldn't allocate key type definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

SymbolsDef *
#if NeedFunctionPrototypes
SymbolsCreate(char *keyName,ExprDef *symbols)
#else
SymbolsCreate(keyName,symbols)
    char *	keyName;
    ExprDef *	symbols;
#endif
{
SymbolsDef *def;

    def= uTypedAlloc(SymbolsDef);
    if (def) {
	def->common.stmtType= StmtSymbolsDef;
	def->common.next= NULL;
	def->merge= MergeDefault;
	bzero(def->keyName,5);
	strncpy(def->keyName,keyName,4);
	def->symbols= symbols;
    }
    else {
	FATAL("Couldn't allocate symbols definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

GroupCompatDef *
#if NeedFunctionPrototypes
GroupCompatCreate(int group,ExprDef *val)
#else
GroupCompatCreate(group,val)
    int		group;
    ExprDef *	val;
#endif
{
GroupCompatDef *def;

    def= uTypedAlloc(GroupCompatDef);
    if (def) {
	def->common.stmtType= StmtGroupCompatDef;
	def->common.next= NULL;
	def->merge= MergeDefault;
	def->group= group;
	def->def= val;
    }
    else {
	FATAL("Couldn't allocate group compat definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

ModMapDef *
#if NeedFunctionPrototypes
ModMapCreate(Atom modifier,ExprDef *keys)
#else
ModMapCreate(modifier,keys)
    Atom	modifier;
    ExprDef *	keys;
#endif
{
ModMapDef *def;

    def= uTypedAlloc(ModMapDef);
    if (def) {
	def->common.stmtType= StmtModMapDef;
	def->common.next= NULL;
	def->merge= MergeDefault;
	def->modifier= modifier;
	def->keys= keys;
    }
    else {
	FATAL("Couldn't allocate mod mask definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

IndicatorMapDef *
#if NeedFunctionPrototypes
IndicatorMapCreate(Atom name,VarDef *body)
#else
IndicatorMapCreate(name,body)
    Atom	name;
    VarDef *	body;
#endif
{
IndicatorMapDef *def;

    def= uTypedAlloc(IndicatorMapDef);
    if (def) {
	def->common.stmtType= StmtIndicatorMapDef;
	def->common.next= NULL;
	def->merge= MergeDefault;
	def->name= name;
	def->body= body;
    }
    else {
	FATAL("Couldn't allocate indicator map definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

IndicatorNameDef *
#if NeedFunctionPrototypes
IndicatorNameCreate(int	ndx,ExprDef *name,Bool virtual)
#else
IndicatorNameCreate(ndx,name,virtual)
    int		ndx;
    ExprDef *	name;
    Bool	virtual;
#endif
{
IndicatorNameDef *def;

    def= uTypedAlloc(IndicatorNameDef);
    if (def) {
	def->common.stmtType= StmtIndicatorNameDef;
	def->common.next= NULL;
	def->merge= MergeDefault;
	def->ndx= ndx;
	def->name= name;
	def->virtual= virtual;
    }
    else {
	FATAL("Couldn't allocate indicator index definition in parser\n");
	/* NOTREACHED */
    }
    return def;
}

ExprDef *
#if NeedFunctionPrototypes
ActionCreate(Atom name,ExprDef *args)
#else
ActionCreate(name,args)
    Atom	 name;
    ExprDef	*args;
#endif
{
ExprDef *act;

    act= uTypedAlloc(ExprDef);
    if (act) {
	act->common.stmtType= StmtExpr;
	act->common.next= NULL;
	act->op= ExprActionDecl;
	act->value.action.name= name;
        act->value.action.args= args;
	return act;
    }
    FATAL("Couldn't allocate ActionDef in parser\n");
    return NULL;
}

ExprDef *
#if NeedFunctionPrototypes
CreateKeysymList(KeySym sym)
#else
CreateKeysymList(sym)
    KeySym	sym;
#endif
{
ExprDef	 *def;

    def= ExprCreate(ExprKeysymList,TypeSymbols);
    if (def) {
	def->value.list.nSyms= 1;
	def->value.list.szSyms= 2;
	def->value.list.syms= uTypedCalloc(2,KeySym);
	if (def->value.list.syms!=NULL) {
	    def->value.list.syms[0]= sym;
	    return def;
	}
    }
    FATAL("Couldn't allocate expression for keysym list in parser\n");
    return NULL;
}

ShapeDef *
#if NeedFunctionPrototypes
ShapeDeclCreate(Atom name,OutlineDef *outlines)
#else
ShapeDeclCreate(name,outlines)
    Atom		name;
    OutlineDef *	outlines;
#endif
{
ShapeDef *	shape;
OutlineDef *	ol;

    shape= uTypedAlloc(ShapeDef);
    if (shape!=NULL) {
	bzero(shape,sizeof(ShapeDef));
	shape->common.stmtType=	StmtShapeDef;
	shape->common.next=	NULL;
	shape->merge=		MergeDefault;
	shape->name=		name;
	shape->nOutlines=	0;
	shape->outlines=	outlines;
	for (ol=outlines;ol!=NULL;ol= (OutlineDef *)ol->common.next) {
	    if (ol->nPoints>0)
		shape->nOutlines++;
	}
    }
    return shape;
}

OutlineDef *
#if NeedFunctionPrototypes
OutlineCreate(Atom field,ExprDef *points)
#else
OutlineCreate(field,points)
    Atom		field;
    ExprDef *		points;
#endif
{
OutlineDef *	outline;
ExprDef *	pt;

    outline= uTypedAlloc(OutlineDef);
    if (outline!=NULL) {
	bzero(outline,sizeof(OutlineDef));
	outline->common.stmtType= 	StmtOutlineDef;
	outline->common.next=		NULL;
	outline->field=		field;
	outline->nPoints=	0;
	if (points->op==ExprCoord) {
	    for (pt=points;pt!=NULL;pt= (ExprDef *)pt->common.next) {
		outline->nPoints++;
	    }
	}
	outline->points= points;
    }
    return outline;
}

KeyDef *
#if NeedFunctionPrototypes
KeyDeclCreate(char *name,ExprDef *expr)
#else
KeyDeclCreate(name,expr)
    char *	name;
    ExprDef *	expr;
#endif
{
KeyDef *	key;

    key= uTypedAlloc(KeyDef);
    if (key!=NULL) {
	bzero(key,sizeof(KeyDef));
	key->common.stmtType= StmtKeyDef;
	key->common.next= NULL;
	if (name)	key->name= name;
	else		key->expr= expr;
    }
    return key;
}

KeyDef *
#if NeedFunctionPrototypes
KeyDeclMerge(KeyDef *into,KeyDef *from)
#else
KeyDeclMerge(into,from)
    KeyDef *	into;
    KeyDef *	from;
#endif
{
    into->expr= (ExprDef *)AppendStmt(&into->expr->common,&from->expr->common);
    from->expr= NULL;
    uFree(from);
    return into;
}

RowDef *
#if NeedFunctionPrototypes
RowDeclCreate(KeyDef *	keys)
#else
RowDeclCreate(keys)
    KeyDef *	keys;
#endif
{
RowDef *	row;
KeyDef *	key;

    row= uTypedAlloc(RowDef);
    if (row!=NULL) {
	bzero(row,sizeof(RowDef));
	row->common.stmtType= StmtRowDef;
	row->common.next= NULL;
	row->nKeys= 0;
	row->keys= keys;
	for (key=keys;key!=NULL;key=(KeyDef *)key->common.next) {
	    if (key->common.stmtType==StmtKeyDef)
		row->nKeys++;
	}
    }
    return row;
}

SectionDef *
#if NeedFunctionPrototypes
SectionDeclCreate(Atom name,RowDef *rows)
#else
SectionDeclCreate(name,rows)
    Atom	name;
    RowDef *	rows;
#endif
{
SectionDef *	section;
RowDef *	row;

    section= uTypedAlloc(SectionDef);
    if (section!=NULL) {
	bzero(section,sizeof(SectionDef));
	section->common.stmtType= StmtSectionDef;
	section->common.next= NULL;
	section->name= name;
	section->nRows= 0;
	section->rows= rows;
	for (row=rows;row!=NULL;row=(RowDef *)row->common.next) {
	    if (row->common.stmtType==StmtRowDef)
		section->nRows++;
	}
    }
    return section;
}

OverlayKeyDef *
#if NeedFunctionPrototypes
OverlayKeyCreate(char *	under,char *over)
#else
OverlayKeyCreate(under,over)
    char *	under;
    char *	over;
#endif
{
OverlayKeyDef *	key;

    key= uTypedAlloc(OverlayKeyDef);
    if (key!=NULL) {
	bzero(key,sizeof(OverlayKeyDef));
	key->common.stmtType= StmtOverlayKeyDef;
	strncpy(key->over,over,XkbKeyNameLength);
	strncpy(key->under,under,XkbKeyNameLength);
	if (over)	uFree(over);
	if (under)	uFree(under);
    }
    return key;
}

OverlayDef *
#if NeedFunctionPrototypes
OverlayDeclCreate(Atom name,OverlayKeyDef *keys)
#else
OverlayDeclCreate(name,keys)
    Atom		name;
    OverlayKeyDef *	keys;
#endif
{
OverlayDef *	ol;
OverlayKeyDef *	key;

    ol= uTypedAlloc(OverlayDef);
    if (ol!=NULL) {
	bzero(ol,sizeof(OverlayDef));
	ol->common.stmtType= 	StmtOverlayDef;
	ol->name=		name;
	ol->keys=		keys;
	for (key=keys;key!=NULL;key=(OverlayKeyDef *)key->common.next) {
	    ol->nKeys++;
	}
    }
    return ol;
}

DoodadDef *
#if NeedFunctionPrototypes
DoodadCreate(unsigned type,Atom name,VarDef *body)
#else
DoodadCreate(type,name,body)
    unsigned	type;
    Atom	name;
    VarDef *	body;
#endif
{
DoodadDef *	doodad;

    doodad= uTypedAlloc(DoodadDef);
    if (doodad!=NULL) {
	bzero(doodad,sizeof(DoodadDef));
	doodad->common.stmtType= StmtDoodadDef;
	doodad->common.next= NULL;
	doodad->type= type;
	doodad->name= name;
	doodad->body= body;
    }
    return doodad;
}

ExprDef *
#if NeedFunctionPrototypes
AppendKeysymList(ExprDef *list,KeySym sym)
#else
AppendKeysymList(list,sym)
    ExprDef *	list;
    KeySym	sym;
#endif
{
    if (list->value.list.nSyms>=list->value.list.szSyms) {
	list->value.list.szSyms*=2;
	list->value.list.syms= uTypedRecalloc(list->value.list.syms,
						list->value.list.nSyms,
						list->value.list.szSyms,
						KeySym);
	if (list->value.list.syms==NULL) {
	    FATAL("Couldn't resize list of symbols for append\n");
	    return NULL;
	}
    }
    list->value.list.syms[list->value.list.nSyms++]= sym;
    return list;
}

int
#if NeedFunctionPrototypes
LookupKeysym(char *str,KeySym *sym_rtrn)
#else
LookupKeysym(str,sym_rtrn)
    char	*str;
    KeySym	*sym_rtrn;
#endif
{
KeySym sym;

    if ((!str)||(uStrCaseCmp(str,"any")==0)||(uStrCaseCmp(str,"nosymbol")==0)) {
	*sym_rtrn= NoSymbol;
	return 1;
    }
    else if ((uStrCaseCmp(str,"none")==0)||(uStrCaseCmp(str,"voidsymbol")==0)) {
	*sym_rtrn= XK_VoidSymbol;
	return 1;
    }
    sym= XStringToKeysym(str);
    if (sym!=NoSymbol) {
	*sym_rtrn= sym;
	return 1;
    }
    return 0;
}

IncludeStmt *
#if NeedFunctionPrototypes
IncludeCreate(char *str,unsigned merge)
#else
IncludeCreate(str,merge)
    char *	str;
    unsigned	merge;
#endif
{
IncludeStmt *	incl,*first;
char *		file,*map,*stmt,*tmp;
char 		nextop;
Bool		haveSelf;

    haveSelf= False;
    incl= first= NULL;
    file= map= NULL;
    tmp= str;
    stmt= uStringDup(str);
    while ((tmp)&&(*tmp)) {
	if (XkbParseIncludeMap(&tmp,&file,&map,&nextop)) {
	    if ((file==NULL)&&(map==NULL)) {
		if (haveSelf)
		    goto BAIL;
		haveSelf= True;
	    }
	    if (first==NULL)
		first= incl= uTypedAlloc(IncludeStmt);
	    else {
		incl->next= uTypedAlloc(IncludeStmt);
		incl= incl->next;
	    }
	    if (incl) {
		incl->common.stmtType= StmtInclude;
		incl->common.next= NULL;
		incl->merge= merge;
		incl->stmt= NULL;
		incl->file= file;
		incl->map= map;
		incl->path= NULL;
		incl->next= NULL;
	    }
	    else {
		WSGO("Allocation failure in IncludeCreate\n");
		ACTION("Using only part of the include\n");
		break;
	    }
	    if (nextop=='|')	merge= MergeAugment;
	    else		merge= MergeOverride;
	}
	else {
	    goto BAIL;
	}
    }
    if (first)		first->stmt= stmt;
    else if (stmt)	uFree(stmt);
    return first;
BAIL:
    ERROR1("Illegal include statement \"%s\"\n",stmt);
    ACTION("Ignored\n");
    while (first) {
	incl= first->next;
	if (first->file) uFree(first->file);
	if (first->map) uFree(first->map);
	if (first->path) uFree(first->path);
	first->file= first->map= first->path= NULL;
	uFree(first);
	first= incl;
    }
    if (stmt)
	uFree(stmt);
    return NULL;
}

#ifdef DEBUG
void
#if NeedFunctionPrototypes
PrintStmtAddrs(ParseCommon *stmt)
#else
PrintStmtAddrs(stmt)
    ParseCommon *	stmt;
#endif
{
    fprintf(stderr,"0x%x",stmt);
    if (stmt) {
	do {
	    fprintf(stderr,"->0x%x",stmt->next);
	    stmt= stmt->next;
	} while (stmt);
    }
    fprintf(stderr,"\n");
}
#endif

static void
#if NeedFunctionPrototypes
CheckDefaultMap(XkbFile	*maps)
#else
CheckDefaultMap(maps)
XkbFile	*maps;
#endif
{
XkbFile * dflt,*tmp;

    dflt= NULL;
    for (tmp=maps,dflt=NULL;tmp!=NULL;tmp=(XkbFile *)tmp->common.next) {
	if (tmp->flags&XkbLC_Default) {
	    if (dflt==NULL) 
		dflt= tmp;
	    else {
		if (warningLevel>2) {
		    WARN1("Multiple default components in %s\n",
					(scanFile?scanFile:"(unknown)"));
		    ACTION2("Using %s, ignoring %s\n",
					(dflt->name?dflt->name:"(first)"),
					(tmp->name?tmp->name:"(subsequent)"));
		}
		tmp->flags&= (~XkbLC_Default);
	    }
	}
    }
    return;
}

_XFUNCPROTOBEGIN
extern int yyparse(
#if NeedFunctionPrototypes
	void
#endif
);
_XFUNCPROTOEND

extern FILE *	yyin;

int
#if NeedFunctionPrototypes
XKBParseFile(FILE *file,XkbFile	**pRtrn)
#else
XKBParseFile(file,pRtrn)
FILE 		 *file;
XkbFile		**pRtrn;
#endif
{
    if (file) {
	yyin= file;
	rtrnValue= NULL;
	if (yyparse()==0) {
	    *pRtrn= rtrnValue;
	    CheckDefaultMap(rtrnValue);
	    rtrnValue= NULL;
	    return 1;
	}
	*pRtrn= NULL;
	return 0;
    }
    *pRtrn= NULL;
    return 1;
}

XkbFile *
#if NeedFunctionPrototypes
CreateXKBFile(int type,char *name,ParseCommon *defs,unsigned flags)
#else
CreateXKBFile(type,name,defs,flags)
int		type;
char *		name;
ParseCommon *	defs;
unsigned	flags;
#endif
{
XkbFile *	file;
static int	fileID;

    file= uTypedAlloc(XkbFile);
    if (file) {
	XkbEnsureSafeMapName(name);
	bzero(file,sizeof(XkbFile));
	file->type= type;
	file->topName= uStringDup(name);
	file->name= name;
	file->defs= defs;
	file->id= fileID++;
	file->compiled= False;
	file->flags= flags;
    }
    return file;
}
