/**
* Name:        No names yet.
*@author	Aleksandras Gluchovas
*@version 1.0
*Modified by:
*Created:     22/09/98
*Copyright:   (c) Aleskandars Gluchovas
*Licence:   	wxWindows licence
*
*@author Jim Crafton
*@version 2.0, 9/2/2001
*Modified for use in the VCF, including name changes
*/

#ifndef __CPPPARSER_H__
#define __CPPPARSER_H__



#ifndef ASSERT
#define ASSERT(x) if (!(x)) throw
#endif


#include "markup.h" // markup tags used in spOperator::GetFullName()

// context class list in "inside-out" order :

namespace VCF {

namespace SrcParser {

class Context;

class ParameterCtx;

//added for VCF C++ RTTI parsing
class PropertyCtx;
class EventCtx;

class AttributeCtx;
class OperationCtx;
class EnumerationCtx;
class TypeDefCtx;
class PreprocessorLineCtx;
class ClassCtx;
class NameSpaceCtx;
class FileCtx;

// source visibilities
enum SRC_VISIBLITY_TYPES
{
	SP_VIS_PUBLIC,
	SP_VIS_PROTECTED,
	SP_VIS_PRIVATE
};

// class types
enum SP_CLASS_TYPES
{
	SP_CLTYPE_CLASS,
	SP_CLTYPE_TEMPLATE_CLASS,
	SP_CLTYPE_STRUCTURE,
	SP_CLTYPE_UNION,
	SP_CLTYPE_INTERFACE
};

// inheritance types
enum SP_INHERITANCE_TYPES
{
	SP_INHERIT_VIRTUAL,
	SP_INHERIT_PUBLIC,
	SP_INHERIT_PRIVATE
};

// proprocessor definitions types
enum SP_PREP_DEFINITION_TYPES
{
	SP_PREP_DEF_DEFINE_SYMBOL,
	SP_PREP_DEF_REDEFINE_SYMBOL,
	SP_PREP_DEF_OTHER
};

// context types
#define	SP_CTX_FILE 	     0x001
#define	SP_CTX_NAMESPACE     0x002
#define	SP_CTX_CLASS         0x004
#define SP_CTX_TYPEDEF       0x008
#define SP_CTX_PREPROCESSOR  0x010
#define SP_CTX_ENUMERATION   0x020
#define	SP_CTX_ATTRIBUTE     0x040
#define	SP_CTX_OPERATION     0x080
#define	SP_CTX_PARAMETER     0x100

//added for VCF C++ parsing
#define	SP_CTX_PROPERTY			0x200
#define	SP_CTX_EVENT			0x400

#define SP_CTX_ANY           0x1FFF

class CommentCtx;

// String list
typedef std::vector<String>     StrListT;

// comments list
typedef std::vector<CommentCtx*> MCommentListT;

// context members
typedef std::vector<Context*> MMemberListT;
 
/**
*Position determinse the line number and 
*start pos of the chunk of characters that
*make up the specific context
*/
class Position {
public:
	Position() {
		m_lineNumber = -1;
		m_startPos = -1;
		m_endPos = -1;
	};

	long m_lineNumber;
	long m_startPos;
	long m_endPos;
};

// base class for all visitors of source code contents

class CtxVisitor 
{
protected:
	bool mSiblingSkipped;
	bool mChildSkipped;
	int  mContextMask;
	bool mMemberRemoved;

	int  mCurrentMember;
	Context* mpCurCxt;

public:
	// methods invoked by context

	// method invoked from user's controling code
	// to visit all nodes staring at the given context.
	// Content is sorted if requrired, see comments 
	// ClassCtx on sorting the class members

	void VisitAll( Context& atContext,
				   bool sortContent = 1
				 );

	// methods invoked by visitor

	// goes to the next context in the outter scope
	// NOTE:: should not be invoked more than once while
	//        visiting certain context

	void SkipSiblings();

	// prevents going down into the contexts contained by
	// the current context
	// NOTE:: the same as above

	void SkipChildren();

	// can be called only in from visiting procedure
	void RemoveCurrentContext();

	// method enables fast filtered traversal
	// of source content, e.g. collecting only classes,
	// or only global functions

	// arg. context - can contain combination of contexts concatinated
	//           with bitwise OR, e.g. SP_CTX_CLASS | SP_CTX_NAMESPACE
	//
	// method can be invoked from the user's controling as well as
	// from within the visting procedure

	void SetFilter( int contextMask );

	// methods should be implemneted by specific visitor:

	// NOTE:: Do not confuse visiting with parsing, first
	//        the source is parsed, and than can be visited
	//        multiple times by variouse visitors (there can
	//        be more the one visitor visiting content at a time)

	virtual void VisitFile( FileCtx& fl ) {}

	virtual void VisitNameSpace( NameSpaceCtx& ns ) {}   

	virtual void VisitClass( ClassCtx& cl ) {}

	virtual void VisitEnumeration( EnumerationCtx& en ) {}

	virtual void VisitTypeDef( TypeDefCtx& td ) {}

	virtual void VisitPreprocessorLine( PreprocessorLineCtx& pd ) {}

	virtual void VisitAttribute( AttributeCtx& attr ) {}

	virtual void VisitOperation( OperationCtx& op ) {}

	virtual void VisitParameter( ParameterCtx& param ) {}

	virtual void VisitProperty( PropertyCtx& property ) {}

	virtual void VisitEvent( EventCtx& event ) {}
};

class CommentCtx
{
public:
	String mText;
	bool   mIsMultiline;
	bool   mStartsPar;
public:

	bool    IsMultiline() const;
	bool    StartsParagraph() const;

	String& GetText();
	String  GetText() const;
};

class VisibilityCtx {
public:
	String m_visibility;
	Position m_pos;
};
// abstract base class for all C++/Java code contexts (constructs)
// (file, namespace, class, operation, etc)

class Context
{
protected:
	// "linked" list of comments
	MCommentListT mComments;
	
	// NULL, if this is file context
	MMemberListT   mMembers;

	Context*    mpParent;	

	int           mSrcLineNo;

	// points to context object, where the this context
	// was originally declared, meaning that this object
	// is redeclaration (or if in the case of operation
	// this context object most probably referres to the
	// implemnetation in .cpp file for example)

	// is NULL, if this object referres to the first occurence
	// of the context

	Context*    mpFirstOccurence;

	bool          mAlreadySorted;

public:
	int           mVisibility;

	// e.g. can be used by documentation generator to store
	// reference to section object
	void*         mpUserData;

public:
	// universal identifier of the context
	String        mName;    
	
	//position of the context in the code
	Position m_codePos;
public:
	Context();
	virtual ~Context();

	// see mUererData member;
	void* GetUserData() { return mpUserData; }
	void SetUserData( void* pUserData )
		{ mpUserData = pUserData; }

	void GetContextList( MMemberListT& lst, int contextMask );

	bool IsSorted();

	bool HasComments();
	inline MCommentListT& GetCommentList()
		{ return mComments; }

	// should be overriden, if the context suports sorting
	// of it's members
	virtual void SortMembers() {}

	inline String& GetName() { return mName; }
	inline int GetSourceLineNo() { return mSrcLineNo; }

	bool IsFirstOccurence();
	Context* GetFirstOccurence();

	Context* GetOutterContext();
	bool HasOutterContext();

	void AddMember ( Context* pMember );
	void AddComment( CommentCtx* pComment );
	MMemberListT& GetMembers();

	// returns NULL, if the context with the given
	// name and type is not contained by this context
	// and it's children. Children's children are not
	// searched recursivelly if searchSubMembers is FALSE

	Context* FindContext( const String& identifier,
		                    int   contextType      = SP_CTX_ANY,
							bool  searchSubMembers = 1
		                  );

	void RemoveThisContext();

	// returns FALSE, if this object was found in the class/struct,
	// otherwise it was found in the global scope of the file

	bool IsInFile();
	bool IsInNameSpace();
	bool IsInClass();
	bool IsInOperation();

	// NOTE:: method returns not the type of this object
	//		  but the file/namespace/class/operation or file in which this
	//		  attribute is contained. First, check for the type of
	//        context using the above method. 
	
	//		  Requiering container which does not exist, will result
	//        in assertion failure

	ClassCtx&     GetClass(); 
	FileCtx&      GetFile();
	NameSpaceCtx& GetNameSpace();
	OperationCtx& GetOperation();

	virtual int GetContextType() { return SP_CTX_ANY; }

	// derived classes override this to invoke VisitXXXXX method
	// which corresponds to the class of specific context,
	// - this is what the "Visitor" pattern told us ^)

	virtual void AcceptVisitor( CtxVisitor& visitor ) = 0;

	// called by visitors
	void RemoveChild( Context* pChild );
};


class ParameterCtx : public Context
{
public:
	String mType;
	String mInitVal;

public:
	virtual int GetContextType() { return SP_CTX_PARAMETER; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitParameter( *this ); }
};

class PropertyCtx : public Context
{
public:
	String m_propertyType;	
	String m_propertyName;
	String m_getMethodName;
	String m_setMethodName;

	virtual int GetContextType() { 
		return SP_CTX_PROPERTY; 
	}

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitProperty( *this ); }

};

class EventCtx : public Context {
public:
	String m_eventName;
	String m_eventClassName;

	virtual int GetContextType() { 
		return SP_CTX_EVENT; 
	}

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitEvent( *this ); }
};

typedef std::vector<ParameterCtx*> MParamListT;

class AttributeCtx : public Context
{
public:
	String mType;
	String mInitVal;
	bool   mIsConstant;
public:

	virtual int GetContextType() { return SP_CTX_ATTRIBUTE; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitAttribute( *this ); }
};

class OperationCtx : public Context
{
public:
	String      mRetType;
	MParamListT mParams;
	bool        mIsConstant;
public:

	// returns full declaration of the operations
	// (ret val., identifier, arg. list),

	// arguments are marked up with italic,
	// default values marked up with bold-italic,
	// all the rest is marked as bold

	String GetFullName(MarkupTagsT tags);

	virtual int GetContextType() { return SP_CTX_OPERATION; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitOperation( *this ); }

};

class PreprocessorLineCtx : public Context 
{

public:

	// prepocessor statement including '#' and
	// attached multiple lines with '\' character
	String mLine;               

	int    mDefType; // see SP_PREP_DEFINITION_TYPES enumeration 

public:

	virtual int GetContextType() { return SP_CTX_PREPROCESSOR; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitPreprocessorLine( *this ); }
};

class ClassCtx : public Context
{
public:
	std::vector<VisibilityCtx*> m_visibilityDecls;

	// list of superclasses/interfaces
	StrListT     mSuperClassNames;

	// see SP_CLASS_TYPES enumeration
	int          mClassSubType;    

	// see SP_INHERITANCE_TYPES enumeration
	int          mInheritanceType;

	// valid if mClassSubType is SP_CLTYPE_TEMPLATE_CLASS
	String       mTemplateTypes;

public:
	virtual ~ClassCtx();

	// sorts class members in the following order:
	// 
	// (by "privacy level" - first private, than protected, public)
	//
	//     within above set
	//
	//       (by member type - attributes first, than methods, nested classes)
	//
	//          within above set
	//
	//             (by identifier of the member)
	//
	// (IMHO SQL statement would have been much better comment here ^)

	virtual void SortMembers();

	virtual int GetContextType() { return SP_CTX_CLASS; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitClass( *this ); }
};

class EnumerationCtx  : public Context
{
public:
	String mEnumContent;

public:
	virtual int GetContextType() { return SP_CTX_ENUMERATION; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitEnumeration( *this ); }
};

class TypeDefCtx  : public Context
{
public:
	String mOriginalType;

public:
	virtual int GetContextType() { return SP_CTX_TYPEDEF; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitTypeDef( *this ); }
};

class FileCtx : public Context
{
public:
	String mFileName;

public:
	virtual int GetContextType() { return SP_CTX_FILE; }

	virtual void AcceptVisitor( CtxVisitor& visitor ) 
		{ visitor.VisitFile( *this ); }
};

// class parses given "memory-resident" Java or C++ source code
// and captures information about classes/attrubutes/methods/
// arguments/etc into structures.

class SourceParser
{
protected:
	// begining of the full-text area of the source file
	char* mpStart;

	// points to first character after the end
	// of teh full-text area
	char* mpEnd;

	// current "privacy level"
	int   mCurVis;

	// current parsing position int full-text area
	char*  cur;

	// about the current class
	bool   mIsVirtaul;
	bool   mIsTemplate;
	size_t mNestingLevel;

	// context data for which is currently being collected
	Context* mpCurCtx;

	int mCurCtxType; // type of the current context

	bool mCommentsOn;
	bool mMacrosOn;

	long m_currentSrcPos;
	long m_currentLineCount;
protected:

	void AttachComments( Context& ctx, char* cur );
	
	void ParseKeyword( char*& cur );
	bool ParseNameAndRetVal( char*& cur );
	bool ParseArguments( char*& cur );
	void ParseMemberVar( char*& cur );
	void SkipFunction( char*& cur );
	void SkipFunctionBody( char*& cur );
	bool CheckVisibilty( char*& cur );

	void AddClassNode( char*& cur );
	
	void AddVisibilityNode( char*& cur );

	void AddMacroNode( char*& cur );
	void AddEnumNode( char*& cur );
	void AddTypeDefNode( char*& cur );

	void DumpOperationInfo( OperationCtx& info, const String& tab, std::ostream& os );
	void DumpClassHeader( ClassCtx& info, std::ostream& os );
	void DumpClassBody( ClassCtx& info, std::ostream& os );

public:

	// NOTE:: discarding of macros or comments improves performance and 
	//		  decreases memory usage

	SourceParser(bool collectCommnets = 1,
		        bool collectMacros   = 1);

	// returns the root-node of the context tree
	// (user is responsible for releasing it from the heep)
	// "end" should point to the last (character + 1) of the 
	// source text

	FileCtx* ParseAll( char* start, char* end );
};

// inline'ed helpers used:
class ParseHelpers {
public:	
	
	static void setContextPosition( Context& ctx, char* cur );

	static void setContextEndPosition( Context& ctx, char* cur );

	static long getCurrentLineCount( char* cur );

	static void incrementLineCount( char* cur );

	static inline void skip_to_eol( char*& cur );
	
	static inline void skip_eol( char*& cur );
	
	static inline bool skip_to_next_comment_in_the_line( char*& cur );
	
	static void skip_to_prev_line( char*& cur );
	
	static inline void skip_comments( char*& cur );
	
	static inline void clear_commets_queue();
	
	static inline void skip_quoted_string( char*& cur );
	
	static inline bool get_next_token( char*& cur );
	
	static inline void skip_preprocessor_dir( char*& cur );
	
	static void skip_token( char*& cur );
	
	static inline size_t get_token_len( char* tok );
	
	static inline bool cmp_tokens( char* tok1, char* tok2 );
	
	static inline bool cmp_tokens_fast( char* tok1, char* tok2, size_t len );
	
	static inline void skip_tempalate_statement( char*& cur );
	
	static inline void skip_statement( char*& cur );
	
	static inline void skip_token_back( char*& cur );
	
	static inline void skip_next_token_back( char*& cur );
	
	static String get_token_str( char* cur );
	
	static size_t skip_block( char*& cur );
	
	static inline bool skip_imp_block( char*& cur );
	
	static bool is_class_token( char*& cur );
	
	inline static bool is_forward_decl( char* cur );
	
	inline static bool is_function( char* cur, bool& isAMacro );
	
	static inline void skip_scope_block( char*& cur );
	
	static void arrange_indirection_tokens_between( String& type, String& identifier );
	
	static bool is_keyword( char* cur );
	
	static inline void get_string_between( char* start, char* end, String* pStr );
	
	static char* set_comment_text( String& text, char* start );

};



}; //end of namespace SrcParser

}; //end of namespace VCF

#endif //__CPPPARSER_H__

