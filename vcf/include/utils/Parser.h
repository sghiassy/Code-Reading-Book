/*
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world.
*/
//Parser.h

#ifndef _PARSER_H__
#define _PARSER_H__


namespace VCF {

class InputStream;

#define PARSE_BUF_SIZE		4096

#define TO_EOF				0
#define TO_SYMBOL			1
#define TO_STRING			2
#define TO_INTEGER			3
#define TO_FLOAT			4


/**
*A parsing utility class, at this moment (8/25/2001) meant 
*for internal use only
*/
class FRAMEWORK_API Parser : public Object {
public:

	Parser( InputStream* is );

	virtual ~Parser();

	void resetStream();

	void checkToken( const VCFChar& T );
    
	void checkTokenSymbol( const String& s );
    
	void error( const String& Ident );
    
	void errorStr( const String& Message);	
    
	VCFChar nextToken();
    
	long sourcePos();
    
	String tokenComponentIdent();
    
	double tokenFloat();
    
	long tokenInt();
    
	String tokenString();
    
	bool tokenSymbolIs(const String& s);

    long getSourceLine() {
		return m_sourceLine;
	}

    VCFChar getToken(){
		return m_token;
	}

	String binHexToString();

protected:
	InputStream* m_stream;
    long m_origin;
    VCFChar* m_buffer;
	VCFChar* m_bufPtr;
	VCFChar* m_bufEnd;
	VCFChar* m_sourcePtr;	
    VCFChar* m_sourceEnd;
    VCFChar* m_tokenPtr;
    VCFChar* m_stringPtr;
    long m_sourceLine;
    VCFChar m_saveChar;
    VCFChar m_token;
    
    void skipBlanks();
};


};

#endif //_PARSER_H__