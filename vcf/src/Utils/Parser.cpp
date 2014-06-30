//Parser.cpp

#include "FoundationKit.h"
#include "Parser.h"

using namespace VCF;

Parser::Parser( InputStream* is )
{
	m_stream = is;
	
	m_buffer =  NULL;

	resetStream();
}

Parser::~Parser()
{
	delete [] m_buffer;
}

void Parser::resetStream()
{
	if ( NULL != m_buffer ) {
		delete [] m_buffer;
	}

	m_buffer =  new VCFChar[ m_stream->getSize()+1 ];
	
	memset(m_buffer,0,m_stream->getSize()+1);

	m_stream->seek( 0, SEEK_FROM_START );

	m_stream->read( m_buffer, m_stream->getSize() );

	m_bufPtr = m_buffer;
	m_bufEnd = m_bufPtr + m_stream->getSize();
	m_sourcePtr = m_buffer;
	m_sourceEnd = m_buffer;
	m_tokenPtr = m_buffer;
	m_sourceLine = 1;
	m_token = '\0';
	this->nextToken();
}

void Parser::skipBlanks()
{
	while (true) {
		if ( *m_sourcePtr == 10 ) {
			m_sourceLine ++;
		}
		else if ( (*m_sourcePtr >= 33) && (*m_sourcePtr <= 255) ) {
			return;
		}
		m_sourcePtr++;
	}  
}

void Parser::checkToken( const VCFChar& T )
{
	if ( m_token != T ) {
		switch ( T ) {    
			case TO_SYMBOL: {
				error("Identifier Expected");
			}
			break;
			
			case TO_STRING: {
				error("String Expected" );
			}
			break;

			case TO_INTEGER: case TO_FLOAT: {
				error("Number Expected");
			}
			break;

			default: {
				error( StringUtils::format( "Char Expected, instead: %c", T ) );		
			}
			break;
		}  
	}
}

String Parser::binHexToString()
{
	String result;

	skipBlanks();
	VCFChar* tmpBufPtr = m_sourcePtr;
	VCFChar* tmpSourcePtr = m_sourcePtr;
	while ( *tmpSourcePtr != '}' ) {		
		if ( ((*tmpSourcePtr >= '0') && (*tmpSourcePtr <= '9')) || ((*tmpSourcePtr >= 'A') && (*tmpSourcePtr <= 'F')) 
				|| ((*tmpSourcePtr >= 'a') && (*tmpSourcePtr <= 'f')) ) {
			m_sourcePtr ++;		
		}
		tmpSourcePtr++;
	}
	result.append( tmpBufPtr, m_sourcePtr-tmpBufPtr);
	m_sourcePtr = tmpSourcePtr;
	
	return result;
}

void Parser::checkTokenSymbol( const String& s )
{
	bool tki = tokenSymbolIs( s );
	if ( false == tki ) {
		error( StringUtils::format( "Symbol Expected, instead: %s", s.c_str() ) );
	}
}

void Parser::error( const String& Ident )
{
	errorStr(Ident);
}

void Parser::errorStr( const String& Message)
{
	throw RuntimeException( MAKE_ERROR_MSG_2(StringUtils::format( "Parse Error, message: %s", Message.c_str() )) );
}

VCFChar Parser::nextToken()
{
	VCFChar result = '\0';
	long I = 0;
	VCFChar* P = NULL;
	VCFChar* S = NULL;
	
	skipBlanks();
	
	P = m_sourcePtr;
	m_tokenPtr = P;
	if ( ((*P >= 'A') && (*P <= 'Z')) || ((*P >= 'a') && (*P <= 'z')) || ( *P == '_' ) ) {
		P++;
		while ( ((*P == ':')) || ((*P >= 'A') && (*P <= 'Z')) || ((*P >= 'a') && (*P <= 'z')) || ((*P >= '0') && (*P <= '9')) || ( *P == '_' ) ) {
			P++;		
		}
		result = TO_SYMBOL;
	}
	else if ( (*P == '#') || (*P == '\'') ) {
		S = P;
		bool finished = false;
		while ( !finished ) {
			switch ( *P ) {
				case '#' : 
				{
					P++;
					I = 0;
					while ( (*P >= '0') && (*P <= '9') ) {
						I = I * 10 + (((int)(*P)) - ((int)('0')));
						P++;
					}
					*S = (VCFChar)I;
					S++;
				}
				break;

				case '\'' : 
				{
					P++;
					bool finished2 = false;
					while ( !finished2 ) {
						switch ( *P ) {
							case '\0': case 10: case 13 : {
								error( "Invalid String" );
							}
							break;

							case '\'' : {
								P++;
								if ( *P != '\'' ) {
									finished2 = true;	
								}
							}
							break;
						}
						if ( (*P != '\n') && (*P != '\r') ) {
							*S = *P;
						}
						else {
							*S = '\0';
						}
						P++;
						S++;
					}
				}
				break;

				default : {
					finished = true;
				}
				break;
			}
		}
		this->m_stringPtr = S;
		result = TO_STRING;
	}
	else if ( *P == '$' ) {
		P++;
		while ( ((*P >= '0') && (*P <= '9')) || ((*P >= 'A') && (*P <= 'F')) || ((*P >= 'a') && (*P <= 'f')) ) {
			P++;		
		}
		result = TO_INTEGER;
	}
    else if ( (*P == '-') ||  ((*P >= '0') && (*P <= '9')) ) {
		P++;
		while ( ((*P >= '0') && (*P <= '9')) ) {
			P++;
		}
		result = TO_INTEGER;
		while ( ((*P >= '0') && (*P <= '9')) || (*P == '.') || (*P == 'e') || (*P == '+') || (*P == '-') ) {
			P++;
			result = TO_FLOAT;
		}
	}
	else {
		result = *P;
		if ( result != TO_EOF ) {
			P++;	
		}
	}
	m_sourcePtr = P;
	m_token = result;
	
	return result;
}

long Parser::sourcePos()
{	
	return m_origin + (m_tokenPtr - m_buffer);
}

String Parser::tokenComponentIdent()
{

	checkToken( TO_SYMBOL );
	VCFChar* P = m_sourcePtr;
	
	while ( (*P == '.') ) {
		P++;
		bool alpha = ((*P >= 'A') && (*P <= 'Z')) || ((*P >= 'a') && (*P <= 'z')) || ( *P == '_' );
		if ( !alpha ) {
			error( "Identifier Expected" );
		}

		do {
			alpha = ((*P >= 'A') && (*P <= 'Z')) || ((*P >= 'a') && (*P <= 'z')) || ((*P >= '0') && (*P <= '9')) || ( *P == '_' );
			P++;			
		}while ( !alpha );
	}	
	
	m_sourcePtr = P;

	return tokenString();
}

double Parser::tokenFloat()
{
	String s = tokenString();
	double result = 0.0;	
	float f = 0.0;
	sscanf( s.c_str(), STR_DOUBLE_CONVERSION, &f );
	result = f;
	return result;
}

long Parser::tokenInt()
{
	String s = tokenString();
	long result = 0;
	int i = 0;
	sscanf( s.c_str(), STR_INT_CONVERSION, &i );
	result = i;
	return result;
}

String Parser::tokenString()
{
	String result;
	long L = 0;
	if ( m_token == TO_STRING) { 
		L = (m_stringPtr - m_tokenPtr) - 1;
	}
	else {
		L = m_sourcePtr - m_tokenPtr;
	}
	result.append( m_tokenPtr, L );
	
	return result;
}	

bool Parser::tokenSymbolIs(const String& s)
{
	return (m_token == TO_SYMBOL) && ( _stricmp( s.c_str(), tokenString().c_str() ) == 0 );
}