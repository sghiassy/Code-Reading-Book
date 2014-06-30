
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


#include "FoundationKit.h"
#include "CPPParser.h"

using namespace VCF;
using namespace SrcParser;



/***** Implementation for class CtxVisitor *****/

void CtxVisitor::VisitAll( Context& atContext,
						  bool sortContent
						)
{
	mSiblingSkipped = 0;
	mChildSkipped   = 0;
	mMemberRemoved  = 0;
	mContextMask    = SP_CTX_ANY;

	if ( sortContent && !atContext.IsSorted() )

		atContext.SortMembers();

	mpCurCxt       = &atContext;
	MMemberListT& members = atContext.GetMembers();

	for( size_t i = 0; i != members.size(); ++i )
	{
		if ( mSiblingSkipped )
		
			return;

		mCurrentMember = i;

		if ( members[i]->GetContextType() & mContextMask )

				members[i]->AcceptVisitor( *this );


		if ( mMemberRemoved )
		{
			mMemberRemoved = 0;
			--i;
			continue;
		}

		if ( !mChildSkipped )
		{
			// visit members of the context recursivelly
			VisitAll( *members[i], sortContent );

			mChildSkipped = 0;
		}
	}
}

void CtxVisitor::RemoveCurrentContext()
{
	MMemberListT& members = mpCurCxt->GetMembers();

	mpCurCxt->RemoveChild( members[mCurrentMember] );	

	mMemberRemoved = 1;
}

void CtxVisitor::SkipSiblings()
{
	mSiblingSkipped = 1;
}

void CtxVisitor::SkipChildren()
{
	mChildSkipped = 1;
}

void CtxVisitor::SetFilter( int contextMask )
{
	mContextMask = contextMask;
}

/***** Implementation for class CommentCtx *****/

bool CommentCtx::IsMultiline()  const
{
	return mIsMultiline;
}

bool CommentCtx::StartsParagraph() const
{
	return mStartsPar;
}

String& CommentCtx::GetText()
{
	return mText;
}

String CommentCtx::GetText() const
{
	return mText;
}

/***** Implementation for class Context *****/

Context::Context()
	: mpParent( 0 ),
	  mpFirstOccurence( 0 ),
	  mAlreadySorted( 0 ),
	  mSrcLineNo(-1),

	  mpUserData(0)
{}

Context::~Context()
{
	for( size_t i = 0; i != mMembers.size(); ++i )
	
		delete mMembers[i];

	for( i = 0; i != mComments.size(); ++i )
	
		delete mComments[i];
}

bool Context::IsSorted()
{
	return mAlreadySorted;
}

void Context::GetContextList( MMemberListT& lst, int contextMask )
{
	for( size_t i = 0; i != mMembers.size(); ++i )
	{
		Context& member = *mMembers[i];

		if ( member.GetContextType() & contextMask )

			lst.push_back( &member );

		// collect required contexts recursively
		member.GetContextList( lst, contextMask );
	}
}

bool Context::HasComments()
{
	return ( mComments.size() != 0 );
}

void Context::RemoveChild( Context* pChild )
{
	for( size_t i = 0; i != mMembers.size(); ++i )

		if ( mMembers[i] == pChild )
		{
			mMembers.erase( &mMembers[i] );

			delete pChild;
			return;
		}

	// the given child should exist on the parent's list 
	ASSERT( 0 );
}
	
bool Context::IsFirstOccurence()
{
	return ( mpFirstOccurence != 0 );
}

Context* Context::GetFirstOccurence()
{
	// this object should not itself be 
	// the first occurence of the context
	ASSERT( mpFirstOccurence != 0 );

	return mpFirstOccurence;
}

void Context::AddMember( Context* pMember )
{
	mMembers.push_back( pMember );

	pMember->mpParent = this;
}

void Context::AddComment( CommentCtx* pComment )
{
	mComments.push_back( pComment );
}

MMemberListT& Context::GetMembers()
{
	return mMembers;
}

Context* Context::FindContext( const String& identifier,
							       int   contextType,
								   bool  searchSubMembers
								 )
{
	for( size_t i = 0; i != mMembers.size(); ++i )
	{
		Context& member = *mMembers[i];

		if ( member.GetName() == identifier && 
			 ( contextType & member.GetContextType() )
		   )

		   return &member;

		if ( searchSubMembers )
		{
			Context* result = 
				member.FindContext( identifier, contextType, 1 );

			if ( result ) return result;
		}
	}

	return 0;
}

void Context::RemoveThisContext()
{
	if ( mpParent )
		mpParent->RemoveChild( this );
}

Context* Context::GetOutterContext()
{
	return mpParent;
}

bool Context::HasOutterContext()
{
	return ( mpParent != 0 );
}

bool Context::IsInFile()
{
	return ( GetOutterContext()->GetContextType() == SP_CTX_FILE );
}

bool Context::IsInNameSpace()
{
	return ( GetOutterContext()->GetContextType() == SP_CTX_NAMESPACE );
}

bool Context::IsInClass()
{
	return ( GetOutterContext()->GetContextType() == SP_CTX_CLASS );
}

bool Context::IsInOperation()
{
	return ( GetOutterContext()->GetContextType() == SP_CTX_OPERATION );
}

ClassCtx& Context::GetClass()
{
	ASSERT( GetContextType() == SP_CTX_CLASS );
	return *((ClassCtx*)mpParent );
}

FileCtx& Context::GetFile()
{
	ASSERT( GetContextType() == SP_CTX_FILE );
	return *((FileCtx*)mpParent );
}

NameSpaceCtx& Context::GetNameSpace()
{
	ASSERT( GetContextType() == SP_CTX_NAMESPACE );
	return *((NameSpaceCtx*)mpParent );
}

OperationCtx& Context::GetOperation()
{
	ASSERT( GetContextType() == SP_CTX_OPERATION );
	return *((OperationCtx*)mpParent );
}

/***** Implementation for class ClassCtx *****/

ClassCtx::~ClassCtx()
{
	std::vector<VisibilityCtx*>::iterator it = m_visibilityDecls.begin();
	while ( it != m_visibilityDecls.end() ) {
		delete *it;
		it++;
	}
	m_visibilityDecls.clear();
}

void ClassCtx::SortMembers()
{
	// TBD::
}

/***** Implementation for class OperationCtx *****/

String OperationCtx::GetFullName(MarkupTagsT tags)
{
	// FIXME::
	// FOR NOW:: "assumed" that output is HTML!

	String txt = tags[TAG_BOLD].start + mRetType;
	txt += " ";
	txt += mName;
	txt += "( ";
	txt += tags[TAG_BOLD].end;
	
	for( size_t i = 0; i != mMembers.size(); ++i )
	{
		// DBG::
		ASSERT( mMembers[i]->GetContextType() == SP_CTX_PARAMETER );

		ParameterCtx& param = *((ParameterCtx*)mMembers[i]);

		if ( i != 0 )
			txt += ", ";
		
		txt += tags[TAG_BOLD].start;
		
		txt += param.mType;

		txt += tags[TAG_BOLD].end;
		txt += tags[TAG_ITALIC].start;

		txt += " ";
		txt += param.mName;

		if ( param.mInitVal != "" )
		{
			txt += " = ";
			txt += tags[TAG_BOLD].start;

			txt += param.mInitVal;

			txt += tags[TAG_BOLD].end;
		}

		txt += tags[TAG_ITALIC].end;;
	}

	txt += tags[TAG_BOLD].start;
	txt += " )";
	txt += tags[TAG_BOLD].end;

	// TBD:: constantness of method

	return txt;
}

/***********************************************************/
/****************** Parser implementation ******************/
/***********************************************************/

// statics used by inline'ed C helper-functions
static char* _gSrcStart = 0;
static char* _gSrcEnd   = 0;
static char* _gLastSuppresedComment = 0;

static long _gSrcLineCount = 0;

// FOR NOW:: comments queue is static
#define MAX_CQ_ENTRIES 128
static char* _gCommentsQueue[MAX_CQ_ENTRIES];
static int    _gCQSize = 0;

/***** keyword std::map related structures *****/

struct less_c_str 
{
    inline bool operator()( char* x, char* y) const 
	{ 	return ( strcmp( x,y ) < 0 );
	}
};

typedef std::map< char*, char*, less_c_str > KeywordMapT;

static KeywordMapT __gMultiLangMap;
static int         __gMapReady = 0;

static char* __gKeyWords[] =
{
	"public",
	"protected",
	"private",

	"class",
	"struct",
	"union",
	"enum",
	"interface",

	"package",
	"import",

	"typedef",
	"template",
	"friend",
	"const",
	"volatile",
	"mutable",
	"virtual",
	"inline",
	"static",
	"register",

	"final",
	"abstract",
	"native",

	"__stdcall",
	"extern",

	//added for RTTI compiler support
	"property",
	"read",
	"write",
	"event",
	"fires",

	0
};

static void check_keyword_map()
{
	if ( !__gMapReady )
	{
		__gMapReady = 1;

		// "make sure" the address of the first member of non-polimorphic class
		// coinsides with the address of the instance

		char** keyword = __gKeyWords;

		while ( (*keyword) != 0 )
		{
			__gMultiLangMap.insert( 
				KeywordMapT::value_type( *keyword, *keyword ) 
			);

			++keyword;
		}
	}
}

/***** helper functions *****/

void ParseHelpers::setContextPosition( Context& ctx, char* cur )
{
	ctx.m_codePos.m_lineNumber = getCurrentLineCount( cur );
	ctx.m_codePos.m_startPos = cur - _gSrcStart;	
	
	//StringUtils::traceWithArgs( "ctx.mName=\"%s\", ctx.m_codePos.m_lineNumber=%d, ctx.m_codePos.m_startPos=%d\n", 
	//								ctx.mName.c_str(), ctx.m_codePos.m_lineNumber, ctx.m_codePos.m_startPos );

}

void ParseHelpers::setContextEndPosition( Context& ctx, char* cur )
{	
	ctx.m_codePos.m_endPos = cur - _gSrcStart;
}

long ParseHelpers::getCurrentLineCount( char* cur )
{
	long result = 1;
	
	const char* begin = _gSrcStart;
	while ( begin < cur ) {
		switch( *begin ) {
			case 10 : {
				if ( 13 == *(begin-1) ) {
					result ++;
				}
			}
			break;
		}
		begin ++;
	}

	return result;
}

void ParseHelpers::incrementLineCount( char* cur )
{
	//String s;
	//s.append( cur+1, __min( 20, _gSrcEnd - cur) ); 	

	if ( 10 == *cur ) {
		char* prev = (cur -1 );
		if ( *prev == 13 ) {
			_gSrcLineCount ++;
			//StringUtils::traceWithArgs( "_gSrcLineCount = %d current chunk of text: \"%s\"...\n", _gSrcLineCount, s.c_str() );
		}
	}
	else if ( 13 == *cur ) {
		_gSrcLineCount ++;
		//StringUtils::traceWithArgs( "_gSrcLineCount = %d current chunk of text: \"%s\"...\n", _gSrcLineCount, s.c_str() );
	}
}

void ParseHelpers::skip_to_prev_line( char*& cur )
{
	while( cur >= _gSrcStart  &&
		   *cur != 10 &&
		   *cur != 13 
		   ) --cur;

	// NOTE:: '\n' is 13,10 on WIN32
	//        '\n' is 10 on UNIX

	--cur;
	if ( *cur == 10 ) {
		//ParseHelpers::incrementLineCount( cur );
		return;
	}

	if ( *cur == 13 ) --cur;

	while( cur >= _gSrcStart  &&
		   *cur != 10 &&
		   *cur != 13 
		   ) --cur;

	++cur; // move to the first character in the line
}



inline void ParseHelpers::clear_commets_queue()
{
	_gCQSize = 0;
}



// skips subsequent white space and comments
// (return false if the end of source code reached)

inline bool ParseHelpers::get_next_token( char*& cur )
{
	for( ; cur < _gSrcEnd; ++cur )
	{
		switch( *(cur) ) 
		{
			case ' ' : continue;
			case '\t': continue;
			case 13  : {				
				continue;
			}

			case 10  : {
				ParseHelpers::incrementLineCount( cur );
				continue;
			}
			case '/' : ParseHelpers::skip_comments( cur );
					   --cur;
				       continue;
			
			default : break;
		};

		break;
	}

	if ( cur >= _gSrcEnd )

		return 0;
	else
		return 1;
}

inline void ParseHelpers::skip_quoted_string( char*& cur )
{
	++cur; // skip first quote '"'
	
	// check if quote wasn't prefixed
	if ( *(cur-2) == '\\' )
		return;
	
	do
	{
		while ( *cur != '"' )
			++cur;
		
		++cur; // skip the last quote
		
		// check if it wasn't prefixed
		
		if ( *(cur-2) != '\\' )
			break;
		
	} while (1);
}

inline void ParseHelpers::skip_comments( char*& cur )
{
	++cur; // skip '/' token
	
	if ( *cur != '/' && *cur != '*' ) return;
	
	// first, store position of the comment into the queue
	// (which further will be attached to the next context
	//  found)
	
	if ( cur-1 != _gLastSuppresedComment )
	{
		if ( _gCQSize == MAX_CQ_ENTRIES )
		{
			size_t i = MAX_CQ_ENTRIES-1;
			
			while( i != 0 )
			{
				_gCommentsQueue[i-1] = _gCommentsQueue[i];
				--i;
			}
			
			--_gCQSize ;
		}
		
		_gCommentsQueue[_gCQSize++] = cur-1;
	}
	
	if ( *cur == '/' )
	{
		ParseHelpers::skip_to_eol( cur );
		ParseHelpers::skip_eol( cur );
		return;
	}
	
	size_t level = 1;
	
	// check for multiline comment (handle nested multiline comments!)
	
	int line_len = 0;
	
	++cur;
	++cur;
	do
	{
		// TBD:: check eof cond.
		
		// detect and remove vertical columns of '*''s
		
		while ( *cur != '/' && cur < _gSrcEnd )
		{
			switch (*cur)
			{
			case '*' : 
				{
					if ( *(cur+1) != '/' )
					{
						if ( line_len == 1 )
							
							*cur = ' ';
					}
					
					break;
				}
				
			case 13 : line_len = 0; break;

			case 10 : {
				line_len = 0; 
				//incrementLineCount( cur );
			}
			break;
				
			default : ++line_len;
			}
			
			++cur;
		}
		
		if ( cur >= _gSrcEnd  ) return;
		
		++cur;
		
		if ( *(cur-2) == '*' )
		{
			--level;
			if ( level == 0 )
				break;
		}
		else
			if ( *cur == '*' )
			{
				++cur;
				++cur;
				
				++level;
			}
			
	} while(1);
}

inline void ParseHelpers::skip_eol( char*& cur ) 
{
	if ( *cur == 13 )
		
		cur += 2;
	else
		cur += 1;

	incrementLineCount( cur );
}

inline void ParseHelpers::skip_to_eol( char*& cur ) 
{
	while( *(cur) != 10 && *cur != 13 && cur < _gSrcEnd) ++cur;
	incrementLineCount( cur );
}


inline bool ParseHelpers::skip_to_next_comment_in_the_line( char*& cur ) 
{
	do
	{
		while( cur < _gSrcEnd  &&
			*cur != 10 &&
			*cur != 13 &&
			*cur != '/'
			) ++cur;
		
		if ( cur == _gSrcEnd ) return 0;
		
		if ( *cur == '/' )
		{
			if ( (*(cur+1) == '*') ||
				(*(cur+1) == '/') ) return 1;
			else
			{
				++cur;
				continue;
			}
		}
		
		if ( (10 == *cur) ) {
			//ParseHelpers::incrementLineCount( cur );
		}
		return 0;
		
	} while(1);
}

inline void ParseHelpers::skip_preprocessor_dir( char*& cur )
{
	do
	{
		ParseHelpers::skip_to_eol(cur);
		
		if ( *(cur-1) != '\\' ) 
			break;
		
		if ( cur < _gSrcEnd )
			ParseHelpers::skip_eol( cur );
		
	} while(1);
}

void ParseHelpers::skip_token( char*& cur )
{
	if ( *cur == '"' )
	{
		ParseHelpers::skip_quoted_string( cur );
		return;
	}

	if ( *cur == ',' || 
		 *cur == ';' ||
		 *cur == '<' ||
		 *cur == '>' ||
		 *cur == '=' ||
		 *cur == ')' ||
		 *cur == '(' 
	   )
	{
		++cur;
		return;
	}

	++cur; // leading character is always skipped

	for( ;; ++cur )
	{
		switch ( *cur )
		{
			case ' ' : break;
			case '\t': break;
			case 13  : {
				
			}
			break;
			case 10  : {
				ParseHelpers::incrementLineCount( cur );
			}
			break;
			case ',' : break;
			case ';' : break;
			case '<' : break;
			case '>' : break;

			// FIXME:: quick-hack to treat scope resolution
			//         tokens are a part of the String
			case ':' : if ( *(cur+1) == ':' ) 
					   {
						   ++cur;
						   continue;
					   }

					   break;
			case '=' : break;
			case '(' : break;
			case ')' : break;
			case '{' : break;
			case '}' : break;

			default : continue;
		};
		break;
	}
}

inline size_t ParseHelpers::get_token_len( char* tok )
{
	char* start = tok;

	ParseHelpers::skip_token( tok );

	return size_t( tok - start );
}

// returns true, if given tokens are equel

inline bool ParseHelpers::cmp_tokens( char* tok1, char* tok2 )
{
	// NOTE:: the case one token includes 
	//        other in it's entirely is not handled

	size_t len = ParseHelpers::get_token_len( tok1 );

	// assuming that tokens are non-zero length

	do
	{
		if ( *(tok1++) != *(tok2++) )
			return 0;

		--len;

	} while ( --len );

	return 1;
}

inline bool ParseHelpers::cmp_tokens_fast( char* tok1, char* tok2, size_t len )
{
	do
	{
		if ( *(tok1++) != *(tok2++) )
			return 0;

	} while ( --len );

	return 1;
}

inline void ParseHelpers::skip_tempalate_statement( char*& cur )
{
	size_t level = 0;

	// go one level deeper
	while( *cur != '<')
			++cur;

	// FIXME:: template should be checked statement for 
	//         comments inside of it

	do
	{
		if ( *cur == '<' )
			++level;
		else
			--level;

		++cur; // skip '<' or '>' token

		if ( level == 0 )
			return;

		while( *cur != '<' && *cur != '>' )
			++cur;

	} while (1);
}

inline void ParseHelpers::skip_statement( char*& cur )
{
	for( ; cur < _gSrcEnd; ++cur )

		switch (*cur)
		{
			case  ';' : ++cur; // skip statement-terminator token
				        return;

			case  '"' : ParseHelpers::skip_quoted_string(cur);
						--cur;
						continue;
			case  '/' : ParseHelpers::skip_comments( cur );
						--cur;
						continue;
			default : continue;
		}
}

// "reversed" versions of ParseHelpers::skip_token() and ParseHelpers::get_next_token()

inline void ParseHelpers::skip_token_back( char*& cur )
{
	// FIXME:: now, when moving backwards, neither strings nor
	//         comment blocks are checked

	--cur; // skip to the trailing character

	if ( *cur == ',' ||
		 *cur == ')' ||
		 *cur == '(' 
	   )
	   return;


	for( ;; --cur )
	{
		switch ( *cur )
		{
			case ' ' : break;
			case '\t': break;
			case 13  : break;
			case 10  : break;
			case ',' : break;
			case '(' : break;

			default : continue;
		};

		break;
	}

	++cur; // get to the leading character of the token
}

inline void ParseHelpers::skip_next_token_back( char*& cur )
{
	--cur; // skip leading character of the current token

	if ( *cur == ',' ||
		 *cur == ')' ||
		 *cur == '(' 
	   )
	{
	   ++cur;
	   return;
	}

	for( ;; --cur )
	{
		switch ( *cur )
		{
			case ' ' : continue;
			case '\t': continue;
			case 13  : continue;
			case 10  : continue;
			case ',' : continue;
			case '(' : continue;

			default : break;
		};

		break;
	}

	++cur; // position after the trailing charcter of the prev token
}

String ParseHelpers::get_token_str( char* cur )
{
	return String( cur, ParseHelpers::get_token_len( cur ) );
}

// skips token or whole expression which may have
// nested  expressions between '(' ')' brackets.
//
// Upon return, the cursor points to the terminating bracket ')',
//
// Return value is the size of the block

size_t ParseHelpers::skip_block( char*& cur )
{
	size_t level = 0; // nesting level

	char* start = cur;

	// NOTE:: assumed that block not necessarely starts 
	//        with bracket rightaway 

	if ( *cur == '(' )
	{
		++level;
	}

	do
	{
		ParseHelpers::skip_token( cur );

		char* savedPos = cur;

		ParseHelpers::get_next_token( cur );

		if ( *cur == '(' )
		{
			++level;
		}
		else
		if ( *cur == ')' )
		{
			if ( level == 0 )
			{
				cur = savedPos;
				return size_t(cur-start);
			}

			--level;

			if ( level == 0 )
			{ 
				++cur;

				// QUICK-HACK::to easily handle function prototypes ,
				// it works, besause theoretically there should
				// be no cast-expressions in non-implementation 
				// scope (e.g. "time( (long*)(ptr+1) )" should not
				// appear in the declarations, thus it is most likelly 
				// for the ")(" fragment to be within a function 
				// protottype in the declarations scope

				if ( *cur == '(' )
				{
					++level;
					continue;
				}

				else return size_t(cur-start);
			}
		}
		else
		{
			if ( level == 0 )
			{
				cur = savedPos;
				return size_t(cur-start);
			}
		}

	} while(1);
}

// returns 0, if end of source reached
inline bool ParseHelpers::skip_imp_block( char*& cur )
{
	while( *cur != '{' ) 
	{
		ParseHelpers::skip_token( cur );
		if ( !ParseHelpers::get_next_token( cur ) ) return 0;
	}

	while( *cur != '}' ) 
	{
		ParseHelpers::skip_token( cur );
		if ( !ParseHelpers::get_next_token( cur ) ) return 0;
	}

	++cur;

	return 1;
}

bool ParseHelpers::is_class_token( char*& cur )
{
	// FIXME:: the below mess should be cleaned in it's entirely

	if ( *cur == 'i' )
		if ( *(cur+1) == 'n' )

			return ParseHelpers::cmp_tokens_fast( cur, "interface", 9 );

	if ( *cur == 'c' )
		if ( *(cur+1) == 'l' )

			return ParseHelpers::cmp_tokens_fast( cur, "class", 5 );

	if ( *cur == 's' )
		if ( *(cur+1) == 't' )

			return ParseHelpers::cmp_tokens_fast( cur, "struct", 6 );

	if ( *cur == 'u' )
		if ( *(cur+1) == 'n' )

			return ParseHelpers::cmp_tokens_fast( cur, "union", 5 );

	return 0;
}

inline bool ParseHelpers::is_forward_decl( char* cur )
{
	do
	{
		switch( *cur )
		{
			case ':' : return 0;
			case '{' : return 0;
			case '(' : return 0;

			case ';' : return 1; 

			default : break;
		};

		++cur;

	} while (cur < _gSrcEnd); // prevent running out of bounds

	return 0;
}

inline bool ParseHelpers::is_function( char* cur, bool& isAMacro )
{
	isAMacro = 0;

	// NOTE:: comments and quoted strings are not checked here

	// first,check for "single-line hanginging macros" like:
	// ___UNICODE
	//

	char* savedPos = cur;
	char* eol = cur;
	ParseHelpers::skip_to_eol( eol );

	ParseHelpers::skip_token( cur );
	ParseHelpers::get_next_token( cur );

	if ( cur > eol )
	{
		isAMacro = 1;
		return 1;
	}

	// it's not a macro, go to the begining of arg. list
	
	do
	{
		// if bracket found, it's a function or a begining
		// of some macro 
		if ( *cur == '(' )
			return 1;

		// end of statement found without any brackets in it
		// - it cannot be a function 

		if ( *cur == ';' ) 
			return 0;

		++cur;

	} while( cur < _gSrcEnd);

	isAMacro = 1;
	return 0;
}

// upon return the cursor is positioned after the
// terminating curly brace 

inline void ParseHelpers::skip_scope_block( char*& cur )
{
	size_t level = 0;

	for( ;; ++cur )

		switch( *cur )
		{
			case '/' : ParseHelpers::skip_comments( cur );
					   --cur;
					   continue;
			case '"' : ParseHelpers::skip_quoted_string( cur );
					   --cur;
					   continue;

			case '{' : ++level;
					   continue;

			case '}'  :--level;
					   if ( level == 0 ) 
					   {
						   ++cur; // skip final closing curly brace
						   return;
					   }

			default : continue;
		};
}

// moves tokens like '*' '**', '***', '&' from the name
// to the type

void ParseHelpers::arrange_indirection_tokens_between( String& type,
											    String& identifier )
{
	// TBD:: FIXME:: return value of operators !

	while ( identifier[0] == '*' ||
		    identifier[0] == '&' 
		  ) 
	{
		type += identifier[0];
		identifier.erase(0,1);

		if ( !identifier.length() ) return;
	}
}


// the only function where multi-lang keyword std::map is accessed

bool ParseHelpers::is_keyword( char* cur )
{
	size_t len = ParseHelpers::get_token_len( cur );

	// put a terminating zero after the given token 
	char tmp = *(cur + len);
	*(cur+len) = '\0';

	KeywordMapT::iterator i; 

	i = __gMultiLangMap.find( cur );
	
	// restore original character suppresed by terminating zero
	*(cur + len) = tmp;
	
	return ( i != __gMultiLangMap.end() );
}

inline void ParseHelpers::get_string_between( char* start, char* end, 
									   String* pStr )
{
	char saved = *end;

	*end  = '\0';
	*pStr = start;
	*end  = saved;
}

char* ParseHelpers::set_comment_text( String& text, char* start )
{
	char* end = start;

	// to avoid poluting the queue with this comment
	_gLastSuppresedComment = start;

	ParseHelpers::skip_comments( end );

	if ( *(end-1) == '/' ) 
		end -= 2;

	start += 2;

	// skip multiple leading '/''s or '*''s 
	while( *start == '/' && start < end ) ++start;
	while( *start == '*' && start < end ) ++start;

	ParseHelpers::get_string_between( start, end, &text );

	return end;
}

/***** Implementation for class SourceParser *****/

SourceParser::SourceParser( bool collectCommnets, bool collectMacros )
	: mpStart(0),
	  mpEnd(0),
	  mpCurCtx( 0 ),
	  mCommentsOn( collectCommnets ),
	  mMacrosOn  ( collectMacros ),
	  m_currentSrcPos(-1),
	  m_currentLineCount(0)
{
	check_keyword_map();
}

FileCtx* SourceParser::ParseAll( char* start, char* end )
{
	m_currentSrcPos = -1;
	m_currentLineCount = 0;

	_gSrcLineCount = 0;


	// set up state variables
	mCurVis       = SP_VIS_PRIVATE;

	FileCtx* pTopCtx = new FileCtx();
	mpCurCtx        = pTopCtx;

	mIsVirtaul    = 0;
	mIsTemplate   = 0;
	mNestingLevel = 0;

	cur = start;

	mpStart = start;
	mpEnd   = end;

	_gSrcEnd   = mpEnd; // let all the C-functions "smell" the end of file
	_gSrcStart = start;

	ParseHelpers::clear_commets_queue();

	// main parsing loop

	do
	{
		if ( !ParseHelpers::get_next_token( cur ) ) 
			// end of source reached
			return pTopCtx;

		switch (*cur)
		{
			case '#' :
				{
					AddMacroNode( cur );		
					continue;
				}
			
			case ':' :
				{
					ParseHelpers::skip_token( cur );
					continue;
				}

			case ';' :
				{
					ParseHelpers::skip_token( cur );
					continue;
				}

			case ')' :
				{
					ParseHelpers::skip_token( cur );
					continue;
				}

			case '=' : 
				{
					ParseHelpers::skip_token( cur );
					continue;
				}

			default: break;
		}

		if ( ParseHelpers::is_keyword( cur ) )
		{
			// parses, token, if token identifies
			// the container context (e.g. class/namespace)
			// the corresponding context object is created
			// and set as current context

			ParseKeyword( cur );
			continue;
		}

		if ( *cur >= '0' && *cur <= '9' ) 
		{
			ParseHelpers::skip_token( cur );
			continue;
		}

		if ( *cur == '}' )
		{
			if ( mCurCtxType != SP_CTX_CLASS )
			{
				// FOR NOW:: disable the below assertion 

				// DBG:: unexpected closing-bracket found
				//ASSERT(0);

				ParseHelpers::skip_token( cur ); // just skip it
				continue;
			}

			--mNestingLevel;

			// terminate operation/class/namespace context 
			// TBD:: check if it's really this type of context
			

			ASSERT( mpCurCtx );
			
			ParseHelpers::setContextEndPosition( *mpCurCtx, cur );
			
			mpCurCtx = mpCurCtx->GetOutterContext();
			ASSERT( mpCurCtx );

			if ( mNestingLevel == 0 )
			{

				mCurCtxType = SP_CTX_FILE;

				// not-nested class delclaration finished,
				// rest template flag in any case
				mIsTemplate = 0;
			}

			ParseHelpers::skip_token( cur );
			continue;
		}

		bool isAMacro = 0;

		if ( ParseHelpers::is_function( cur, isAMacro ) )
		{
			if ( isAMacro )
			{
				ParseHelpers::skip_token( cur );
				continue;
			}

			char* savedPos = cur;

			if ( !ParseNameAndRetVal( cur ) )
			{
				cur = savedPos;
				SkipFunction( cur );
				continue;
			}

			if ( !ParseArguments( cur ) )
			{
				// failure while parsing arguments,
				// remove enclosing operation context

				Context* pFailed = mpCurCtx;
				mpCurCtx = mpCurCtx->GetOutterContext();
				mpCurCtx->RemoveChild( pFailed );

				ParseHelpers::skip_to_eol( cur );
				//cur = savedPos;
			}
			else
			{
				// otherwise, successfully close operation context:

				ParseHelpers::clear_commets_queue();
				Context* pOpCtx = mpCurCtx;
				mpCurCtx = mpCurCtx->GetOutterContext();

				// DBG::
				ASSERT( mpCurCtx );

				SkipFunctionBody( cur );
				
				ParseHelpers::setContextEndPosition( *pOpCtx, cur );
			}
		}
		else // otherwise it's declaration of a variable;
		{
			// now, the cursor point to the end of statement (';' token)

			if ( mCurCtxType != SP_CTX_CLASS )
			{
				// non-class members are ignored

				ParseHelpers::skip_token( cur ); // skip the end of statement
				continue;
			}

			ParseMemberVar( cur );
		}

	} while( 1 );
}

void SourceParser::AttachComments( Context& ctx, char* cur )
{
	if ( !mCommentsOn ) return;

	MCommentListT& lst = ctx.GetCommentList();

	char* prevComEnd = 0;

	// attach comments which were found before the given context

	for( int i = 0; i != _gCQSize; ++i )
	{
		CommentCtx* pComment = new CommentCtx();
		lst.push_back( pComment );

		// find the end of comment
		char* start = _gCommentsQueue[i];

		pComment->mIsMultiline = ( *(start+1) == '*' );

		// first comment in the queue and multiline 
		// comments are always treated as a begining 
		// of the new paragraph in the comment text

		if ( i == 0 )
			pComment->mStartsPar = 1;
		else
		if ( pComment->mIsMultiline )
			pComment->mStartsPar = 1;
		else
		{
			// find out wheather there is a new-line
			// between to adjecent comments

			char* prevLine = start;
			ParseHelpers::skip_to_prev_line(prevLine);

			if ( prevLine > prevComEnd )
			
				pComment->mStartsPar = 1;
			else
				pComment->mStartsPar = 0;
		}

		prevComEnd = ParseHelpers::set_comment_text( pComment->mText, start );
	}


	// attach comments which are at the end of the line
	// of the given context (if any)

	if ( ParseHelpers::skip_to_next_comment_in_the_line( cur ) )
	{
		CommentCtx* pComment = new CommentCtx();
		lst.push_back( pComment );
	
		ParseHelpers::set_comment_text( pComment->mText, cur );

		pComment->mStartsPar = 1;
		pComment->mIsMultiline = ( *(cur+1) == '*' );

		// mark this comment, so that it would not
		// get in the comments list of the next context
		_gLastSuppresedComment = cur;
	}

	ParseHelpers::clear_commets_queue();
}



void SourceParser::AddMacroNode( char*& cur )
{
	char* start = cur;
	ParseHelpers::skip_preprocessor_dir( cur );

	if ( !mMacrosOn ) return;

	PreprocessorLineCtx* pPL = new PreprocessorLineCtx();

	AttachComments( *pPL, cur );

	ParseHelpers::get_string_between( start, cur, &pPL->mLine );

	++start; // skip '#'
	ParseHelpers::get_next_token( start );
	
	// if we found a definition or redefinition,
	// determine the type exactly and assign
	// a name to the context

	if ( *start == 'd' )

	if ( ParseHelpers::cmp_tokens_fast( start, "define", 6 ) )
	{
		char* tok = start+6;

		ParseHelpers::get_next_token( tok );

		pPL->mName = ParseHelpers::get_token_str( tok );

		ParseHelpers::skip_token( tok );
		ParseHelpers::get_next_token( tok);


		if ( tok > cur )
			pPL->mDefType = SP_PREP_DEF_DEFINE_SYMBOL;
		else
			pPL->mDefType = SP_PREP_DEF_REDEFINE_SYMBOL;
	}
	else
		pPL->mDefType = SP_PREP_DEF_OTHER;

	ParseHelpers::setContextPosition( *pPL, cur );
	mpCurCtx->AddMember( pPL );

	ParseHelpers::clear_commets_queue();
}

void SourceParser::ParseKeyword( char*& cur )
{
	// analyze token, which identifies the begining of a new context

	if ( CheckVisibilty( cur ) )
	{
		//found visibility
		if ( SP_CTX_CLASS == mCurCtxType ) {
			AddVisibilityNode( cur );
		}
		ParseHelpers::skip_token( cur );
		return;
	}

	if ( ParseHelpers::is_class_token( cur ) )
	{
		if ( ParseHelpers::is_forward_decl( cur ) )
		{
			// forward declarations are ignored;
			ParseHelpers::skip_token( cur );
			return;
		}

		if ( mNestingLevel == 0 )
		{
			// change context form global class context
			mCurCtxType = SP_CTX_CLASS;
		}

		++mNestingLevel;

		// add information about new class (name, inheritance, etc)
		AddClassNode( cur );

		// the default visiblity for class members is 'private'
		mCurVis = SP_VIS_PRIVATE;

		return;
	}

	size_t len = ParseHelpers::get_token_len( cur );

	if ( ParseHelpers::cmp_tokens_fast( cur, "typedef", len  ) )
	{
		ParseHelpers::skip_token(cur);
		ParseHelpers::get_next_token(cur);

		if ( ParseHelpers::cmp_tokens_fast( cur, "struct", len ) ||
			 ParseHelpers::cmp_tokens_fast( cur, "union",  len ) ||
			 ParseHelpers::cmp_tokens_fast( cur, "class",  len )
		   )		
		{
			if ( mNestingLevel == 0 )
			{
				// change context form global class context
				mCurCtxType = SP_CTX_CLASS;
			}

			++mNestingLevel;

			// add information about new class (name, inheritance, etc)
			AddClassNode( cur );

			// the default visiblity for class members is 'private'
			mCurVis = SP_VIS_PRIVATE;

			return;

			// FOR NOW:: typedef struct, etc are also ignored 
			//ParseHelpers::skip_scope_block( cur );
		}

		if ( ParseHelpers::cmp_tokens_fast( cur, "enum", len  ) )
		{
			AddEnumNode( cur );
			return;
		}

		AddTypeDefNode( cur );

		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "enum", len ) )
	{
		AddEnumNode( cur );
		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "extern", len ) )
	{
		// extern's are ignored (both extern "C" and extern vars)
		while ( *cur != '{' &&
			    *cur != ';' )
		{
			ParseHelpers::skip_token( cur );
			ParseHelpers::get_next_token( cur );
		}
		return;
				    
	}
	if ( ParseHelpers::cmp_tokens_fast( cur, "enum", len ) )
	{
		// enumeration blocks are ignored

		ParseHelpers::skip_scope_block( cur );

		ParseHelpers::get_next_token( cur );
		ParseHelpers::skip_token( cur ); // skip ';' token;
		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "package", len  ) )
	{
		// packages are ignored
		ParseHelpers::skip_statement( cur );
		return;
	};

	if ( ParseHelpers::cmp_tokens_fast( cur, "import", len  ) )
	{
		// import statements are ignored
		ParseHelpers::skip_statement( cur );
		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "virtual", len  ) )
	{
		// probably the virtual method is in front of us;
		mIsVirtaul = 1;
		ParseHelpers::skip_token( cur );
		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "template", len  ) )
	{
		mIsTemplate = 1;
		ParseHelpers::skip_tempalate_statement( cur );
		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "friend", len  ) )
	{
		ParseHelpers::skip_statement( cur );
		return;
	}

/////////////////////////////////////////////////////////////////////////////////
//WARNING - JC 8/19/2001
//this is completely NON C++ stuff here - it is only here
//to allow the VCF RTTI compiler to handle special extensions
/////////////////////////////////////////////////////////////////////////////////

	
	if ( ParseHelpers::cmp_tokens_fast( cur, "property", len  ) )
	{
		//need to pull of the following
		//the property type
		//the property name
		//the getter method name
		//the setter method name
		String propertyType;	
		String propertyName;
		String getMethodName;
		String setMethodName;
		
		//while ( *cur != '{' &&
		//	    *cur != ';' )
		//{
		//
		bool haveAValidProperty = false;
		char* P = cur;
		ParseHelpers::skip_token( cur );
		if ( ParseHelpers::get_next_token( cur ) ) {			
			propertyType = ParseHelpers::get_token_str( cur );
			
			ParseHelpers::skip_token( cur );
			if ( ParseHelpers::get_next_token( cur ) ) {				
				propertyName = ParseHelpers::get_token_str( cur );
				
				ParseHelpers::skip_token( cur );
				
				if ( ParseHelpers::get_next_token( cur ) ) {
					int propLen = ParseHelpers::get_token_len(cur);
					if ( ParseHelpers::cmp_tokens_fast( cur, "read", propLen  ) ) {
						ParseHelpers::skip_token( cur );
						if ( ParseHelpers::get_next_token( cur ) ) {
							getMethodName = ParseHelpers::get_token_str( cur );						
							ParseHelpers::skip_token( cur );
							haveAValidProperty = true;
							if ( ParseHelpers::get_next_token( cur ) ) {
								propLen = ParseHelpers::get_token_len(cur);								
								if ( ParseHelpers::cmp_tokens_fast( cur, "write", propLen  ) ) {
									ParseHelpers::skip_token( cur );
									if ( ParseHelpers::get_next_token( cur ) ) {										
										setMethodName  = ParseHelpers::get_token_str( cur );
									}
								}
							}
						}
					}
				}
			}
			
		}
		//ParseHelpers::get_next_token( cur );
		//}

		if ( true == haveAValidProperty ) {
			if ( mpCurCtx->GetContextType() == SP_CTX_CLASS ) {
				PropertyCtx* newProperty = new PropertyCtx ();
				newProperty->m_propertyType = propertyType;
				newProperty->m_propertyName = propertyName;
				newProperty->m_getMethodName = getMethodName;
				newProperty->m_setMethodName = setMethodName;
				
				ParseHelpers::setContextPosition( *newProperty, P );

				mpCurCtx->AddMember( newProperty );
			}
		}

		ParseHelpers::skip_statement( cur );

		
		return;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "event", len  ) )
	{
		//need the following:
		//event name
		//event object class name
		String eventName;
		String eventClassName;
		bool haveAValidEvent = false;
		char* p = cur;
		ParseHelpers::skip_token( cur );
		if ( ParseHelpers::get_next_token( cur ) ) {
			eventName = ParseHelpers::get_token_str( cur );
			ParseHelpers::skip_token( cur );

			if ( ParseHelpers::get_next_token( cur ) ) {
				int eventLen = ParseHelpers::get_token_len(cur);
				if ( ParseHelpers::cmp_tokens_fast( cur, "fires", eventLen  ) ) {
					ParseHelpers::skip_token( cur );
					if ( ParseHelpers::get_next_token( cur ) ) {
						eventClassName = ParseHelpers::get_token_str( cur );
						haveAValidEvent = true;
					}
				}
			}
		}

		if ( true == haveAValidEvent ) {
			if ( mpCurCtx->GetContextType() == SP_CTX_CLASS ) {
				EventCtx* newEvent = new EventCtx();
				newEvent->m_eventClassName = eventClassName;
				newEvent->m_eventName = eventName;
				ParseHelpers::setContextPosition( *newEvent, p );
				mpCurCtx->AddMember( newEvent );
			}
		}

		ParseHelpers::skip_statement( cur );
		return;
	}

/////////////////////////////////////////////////////////////////////////////////
//END WARNING
/////////////////////////////////////////////////////////////////////////////////

	// ingnore "unsigificant" tokens (i.e. which do not
	// affect the current parsing context)

	ParseHelpers::skip_token( cur );
}

bool SourceParser::ParseNameAndRetVal( char*& cur )
{
	// FOR NOW:: all functions in the global
	//           scope are ignored

	char* start = cur;


	while( *cur != '(' )
	{
		ParseHelpers::skip_token( cur );
		ParseHelpers::get_next_token( cur );
	}

	char* savedPos = cur + 1;

	// skip gap between function name and start of paramters list
	while ( *(cur-1) == ' ' )
		--cur;

	OperationCtx* pOp = new OperationCtx();	

	mpCurCtx->AddMember( pOp );
	pOp->mVisibility = mCurVis;

	// add comments about operation
	AttachComments( *pOp, cur );

	// go backwards to method name
	ParseHelpers::skip_token_back( cur );

	pOp->mName = ParseHelpers::get_token_str( cur );

	ParseHelpers::setContextPosition( *pOp, cur );

	// go backwards to method return type
	ParseHelpers::skip_next_token_back( cur );

	if ( cur >= start )

		pOp->mRetType = String( start, size_t( cur-start ) );

	ParseHelpers::arrange_indirection_tokens_between( pOp->mRetType, pOp->mName );

	cur = savedPos;

	ParseHelpers::setContextEndPosition( *pOp, cur );

	// now, enter operation context
	mpCurCtx = pOp;

	return 1;
}

bool SourceParser::ParseArguments( char*& cur )
{
	// now cursor position is right after the first opening bracket
	// of the function declaration

	char* blocks    [16]; // used exclusivelly for iterative "lean out"
						  // of macros and misc. not-obviouse grammar
						  // (dirty,, but we cannot do it very nice,
						  //  we're not preprocessor-free C/C++ code)
	int   blockSizes[16];

	do
	{
		size_t blocksSkipped = 0;

		ParseHelpers::get_next_token( cur );

		bool first_blk = 1;

		while( *cur != ')' && *cur != ',' )
		{
			blocks[blocksSkipped] = cur;

			if ( first_blk )
			{
				char* prev = cur;
				ParseHelpers::skip_token( cur );

				blockSizes[blocksSkipped] = size_t(cur-prev);

				first_blk = 0;
			}
			else
				blockSizes[blocksSkipped] = ParseHelpers::skip_block( cur );

			ParseHelpers::get_next_token( cur );
			++blocksSkipped;
		}


		if ( blocksSkipped == 1 ) 
		{
			// check if the empty arg. list stressed with "void" inside
			if ( ParseHelpers::cmp_tokens_fast( blocks[0] , "void", 4 ) )
				return 1;

			// FIXME:: TBD:: K&R-style function declarations!

			// if only one block enclosed, than it's probably
			// some macro, there should be at least two blocks,
			// one for argument type and another for it's identifier
			return 0;
		}

		if ( blocksSkipped == 0 )
		{
			++cur;
			break; // function without paramters
		}

		// we should be in the operation context now
		OperationCtx* pOp = (OperationCtx*)mpCurCtx;

		ParameterCtx* pPar = new ParameterCtx();
		
		ParseHelpers::setContextPosition( *pPar, cur );

		pOp->AddMember( pPar );

		size_t nameBlock = blocksSkipped - 1;
		size_t typeBlock = nameBlock - 1;

		// check if default values present
		if ( *blocks[typeBlock] == '=' )
		{
			// expressions like "int = 5" are ignored,
			// since name for paramters is required
			if ( blocksSkipped == 3 )
			{
				if ( *cur == ')' )
				{
					++cur;
					break;
				}
			else
				continue;
			}

			pPar->mInitVal = String( blocks[nameBlock], blockSizes[nameBlock] );

			nameBlock = nameBlock - 2; // skip '=' token and default value block
			typeBlock = nameBlock - 1;
		}

		// attach comments about the parameter
		AttachComments( *pPar, blocks[nameBlock] );

		// retrieve argument name
		pPar->mName = String( blocks[nameBlock], blockSizes[nameBlock] );

		// retreive argument type

		size_t len = blockSizes[ typeBlock ];
		len = size_t ( (blocks[ typeBlock ] + len) - blocks[ 0 ] );

		pPar->mType = String( blocks[0], len );
		
		ParseHelpers::arrange_indirection_tokens_between( pPar->mType, pOp->mName );

		if ( *cur == ')' )
		{
			++cur;
			break;
		}

		++cur; // skip comma
		ParseHelpers::get_next_token(cur);

	} while(1);

	// check if it was really a function not a macro,
	// if so, than it should be terminated with semicolon ';'
	// or opening implemenetaton bracket '{'

	char* tok = cur;

	do
	{
		if ( *tok == '{' || *tok == ';' ) return 1;

		// check for unexpected tokens
		if ( *tok == '=' || *tok == '0' ) 
		{
			ParseHelpers::skip_token(tok);
			if ( !ParseHelpers::get_next_token(tok) ) return 0;
			continue;
		}

		if ( *tok == '}' ) return 0;

		// if initialization list found
		if ( *tok == ':' ) return 1;

		if ( ParseHelpers::cmp_tokens_fast( tok, "const", 5 ) )
		{
			ParseHelpers::skip_token(tok);
			if ( !ParseHelpers::get_next_token(tok) ) return 0;
			continue;
		}

		if ( CheckVisibilty( tok ) ) return 0;

		// if next context found
		if ( ParseHelpers::is_keyword( tok ) ) return 0;

		ParseHelpers::skip_token(tok);
		if ( !ParseHelpers::get_next_token(tok) ) return 0;

	} while(1);
	
	return 1;
}

void SourceParser::ParseMemberVar( char*& cur )
{
	MMemberListT& members = mpCurCtx->GetMembers();

	bool firstMember = 1;

	size_t first = 0;

	String type;

	// jump to the end of statement
	// and start collecting same-type varibles
	// back-to-front towards the type identifier

	ParseHelpers::skip_statement( cur );
	char* savedPos = cur;

    --cur; // rewind back to ';'

	do
	{
		AttributeCtx* pAttr = new AttributeCtx();		

		mpCurCtx->AddMember( pAttr );

		ParseHelpers::setContextEndPosition( *pAttr, cur );

		pAttr->mVisibility = mCurVis;

		pAttr->mIsConstant = 0;

		if ( firstMember )
		{
			firstMember = 0;
			first = members.size() - 1;;
		}

		ParseHelpers::skip_token_back( cur );

		// attach comments about the attribute
		AttachComments( *pAttr, cur );

		pAttr->mName = ParseHelpers::get_token_str( cur );

		// guessing that this going to be variable type
		ParseHelpers::skip_next_token_back( cur );
		ParseHelpers::skip_token_back( cur );

		pAttr->mType = ParseHelpers::get_token_str( cur );

		ParseHelpers::setContextPosition( *pAttr, cur );

		// if comma, than variable list continues
		// otherwise the variable type reached - stop

		if ( *cur == '=' )
		{
			// yes, we've mistaken, it was not a identifier,
			// but it's default value
			pAttr->mInitVal = 
				pAttr->mName;
			
			// skip default value and '=' symbol
			ParseHelpers::skip_next_token_back( cur );
			ParseHelpers::skip_token_back( cur );

			pAttr->mName = ParseHelpers::get_token_str( cur );

			ParseHelpers::skip_next_token_back( cur );
			ParseHelpers::skip_token_back( cur );
		}

		if ( *cur != ',' )
		{
			type = ParseHelpers::get_token_str( cur );
			break;
		}

	} while(1);

	// set up types for all collected (same-type) attributes;
	while ( first != members.size() - 1 )
	{
		AttributeCtx* pAttr = (AttributeCtx*)members[first];

		pAttr->mType       = type;
		pAttr->mVisibility = mCurVis;

		ParseHelpers::arrange_indirection_tokens_between( pAttr->mType, pAttr->mName );

		++first;
	}

	cur = savedPos;

	ParseHelpers::clear_commets_queue();
}

void SourceParser::SkipFunction( char*& cur )
{
	while ( *cur != '(' ) 
		++cur;

	ParseHelpers::skip_next_token_back( cur ); // go back and skip function identifier
	ParseHelpers::skip_token_back( cur );      // go back and skip return type

	ParseHelpers::skip_block( cur );           // now, go ahead and skip whole declaration

	SkipFunctionBody( cur );

}

void SourceParser::SkipFunctionBody( char*& cur )
{
	// FIXME:: check for comments and quoted stirngs here

	while( *cur != '{' && *cur != ';' )
		++cur;

	if ( *cur == ';' )
	{
		++cur; 
		return; // no body was found, only decl.
	}
	else
		ParseHelpers::skip_scope_block( cur ); // skip the whole imp.
}

bool SourceParser::CheckVisibilty( char*& cur )
{
	size_t len = ParseHelpers::get_token_len( cur );

	if ( ParseHelpers::cmp_tokens_fast( cur, "public:", len ) )
	{
		mCurVis = SP_VIS_PUBLIC;
		return true;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "protected:", len ) )
	{
		mCurVis = SP_VIS_PROTECTED;
		return true;
	}

	if ( ParseHelpers::cmp_tokens_fast( cur, "private:", len ) )
	{
		mCurVis = SP_VIS_PRIVATE;
		return true;
	}

	return false;
}

void SourceParser::AddVisibilityNode( char*& cur )
{
	ClassCtx* pClass = (ClassCtx*)mpCurCtx;

	VisibilityCtx* newVisibility = new VisibilityCtx();
	
	switch ( mCurVis ) {
		case SP_VIS_PUBLIC: {
			newVisibility->m_visibility = "public:";
		}
		break;

		case  SP_VIS_PROTECTED : {
			newVisibility->m_visibility = "protected:";
		}
		break;

		case  SP_VIS_PRIVATE : {
			newVisibility->m_visibility = "private:";
		}
		break;
	}
	newVisibility->m_pos.m_lineNumber = ParseHelpers::getCurrentLineCount( cur );
	newVisibility->m_pos.m_startPos = cur - _gSrcStart;
	newVisibility->m_pos.m_endPos = newVisibility->m_pos.m_startPos + newVisibility->m_visibility.size();

	pClass->m_visibilityDecls.push_back( newVisibility );
}

void SourceParser::AddClassNode( char*& cur )
{
	ParseHelpers::skip_token( cur ); // skip 'class' keyword
	if ( !ParseHelpers::get_next_token( cur ) ) return;

	// in C++
	if ( *cur == ':' )
	{
		ParseHelpers::skip_token( cur );
		ParseHelpers::get_next_token( cur );
	}

	ClassCtx* pClass = new ClassCtx();

	ParseHelpers::setContextPosition( *pClass, cur );	

	mpCurCtx->AddMember( pClass );

	// by default all class members are private
	mCurVis       = SP_VIS_PRIVATE;

	// attach comments about the class
	AttachComments( *pClass, cur );

	char* nameTok = cur;
	pClass->mName = ParseHelpers::get_token_str( cur );
	String& nm = pClass->mName;

	bool isDerived = 0;

	// DANGER-MACROS::

	do
	{
		ParseHelpers::skip_token( cur );
		if ( !ParseHelpers::get_next_token( cur ) ) return;

		if ( *cur == ':' )
		{
			isDerived = 1;

			char* tok = cur;

			ParseHelpers::skip_next_token_back( tok );
			ParseHelpers::skip_token_back( tok );

			// class name should precend ':' colon, thus
			// the one which was captured before was
			// proablty something else (like __dllexpoert MyClass: )

			if ( nameTok != tok )
			{
				pClass->mName = ParseHelpers::get_token_str( tok );
			}

		}

		if ( *cur == '{' )
			break;

		if ( *cur == ',' ) 
			continue;

		size_t len = ParseHelpers::get_token_len( cur );

		// skip neglectable C++ modifieres
		if ( ParseHelpers::cmp_tokens_fast( cur, "public", len ) )
			continue;

		if ( ParseHelpers::cmp_tokens_fast( cur, "protected", len ) )
			continue;

		if ( ParseHelpers::cmp_tokens_fast( cur, "private", len ) )
			continue;

		if ( ParseHelpers::cmp_tokens_fast( cur, "virtual", len ) )
			continue;

		// skip neglectable JAVA modifieres

		if ( ParseHelpers::cmp_tokens_fast( cur, "extends", len ) )
		{
			isDerived = 1;
			continue;
		}

		if ( ParseHelpers::cmp_tokens_fast( cur, "implements", len ) )
		{
			isDerived = 1;
			continue;
		}

		// all we need to know is superclass or interface

		char* tok = cur;
		ParseHelpers::skip_token(tok);
		ParseHelpers::get_next_token(tok);
		
		if ( *tok != ':' && *cur != ':' )
		
			pClass->mSuperClassNames.push_back( String( cur, len ) );

	} while(1);

	if ( !isDerived )
	{

		while ( pClass->mSuperClassNames.size() )

			pClass->mSuperClassNames.erase( &pClass->mSuperClassNames[0] );

		char* tok = cur;

		ParseHelpers::skip_next_token_back( tok );
		ParseHelpers::skip_token_back( tok );

		pClass->mName = ParseHelpers::get_token_str( tok );
	}


	++cur; // skip opening curly brace

	// now, enter the class context
	mpCurCtx = pClass;

	ParseHelpers::clear_commets_queue();
}

void SourceParser::AddEnumNode( char*& cur )
{
	// now the cursor is at "enum" keyword
	char* start = cur;

	EnumerationCtx* pEnum = new EnumerationCtx();	

	mpCurCtx->AddMember( pEnum );
	
	ParseHelpers::setContextPosition( *pEnum, cur );


	AttachComments( *pEnum, cur );

	ParseHelpers::skip_token( cur );
	if ( !ParseHelpers::get_next_token( cur ) ) return;

	// check if enumeration has got it's identifier
	if ( *cur != '{' )
	{
		pEnum->mName = ParseHelpers::get_token_str( cur );
	}

	if ( !ParseHelpers::skip_imp_block( cur ) ) return;

	ParseHelpers::get_string_between( start, cur, &pEnum->mEnumContent );

	if ( ParseHelpers::get_next_token(cur) )
	{
		// check if the identifier if after the {...} block
		if ( *cur != ';' )
			
			pEnum->mName = ParseHelpers::get_token_str( cur );
	}

	ParseHelpers::clear_commets_queue();
}

void SourceParser::AddTypeDefNode( char*& cur )
{
	// now the cursor at the token next to "typedef" keyword

	if ( !ParseHelpers::get_next_token(cur) ) return;

	char* start = cur;

	TypeDefCtx* pTDef = new TypeDefCtx();
	mpCurCtx->AddMember( pTDef );

	ParseHelpers::setContextPosition( *pTDef, cur );

	AttachComments( *pTDef, cur );

	ParseHelpers::skip_statement( cur );

	char* tok = cur-1;
	ParseHelpers::skip_next_token_back( tok );

	char* nameEnd = tok;

	ParseHelpers::skip_token_back( tok );

	char* nameStart = tok;

	ParseHelpers::skip_next_token_back( tok );

	char* typeEnd = tok;

	// check if it's function prototype
	if ( *nameStart == ')' )
	{
		typeEnd = nameStart+1;

		// skip argument list
		while ( *nameStart != '(' ) --nameStart;

		// skip to function type definition
		while ( *nameStart != ')' ) --nameStart;
		
		ParseHelpers::skip_next_token_back( nameStart );

		nameEnd = nameStart;

		ParseHelpers::skip_token_back( nameStart );

		if ( *nameStart == '*' ) ++nameStart;
	}

	ParseHelpers::get_string_between( start, typeEnd, &pTDef->mOriginalType );

	ParseHelpers::get_string_between( nameStart, nameEnd, &pTDef->mName );

	ParseHelpers::clear_commets_queue();
}