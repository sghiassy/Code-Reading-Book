//ErrorLog.h
/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#include "FoundationKit.h"
#include "ErrorLog.h"

using namespace VCF;


ErrorLog::ErrorLog( const String& ouputFilename )
{
	m_outputFile = ouputFilename;
	if ( m_outputFile == "" ) {
		m_outputFile = "errorLog.log";
	}

	m_fileStream = new FileStream( m_outputFile );

	m_textOutStream = new TextOutputStream( m_fileStream );
	m_errStream = m_fileStream;//m_textOutStream;
	System::setErrorLog( this );
}

ErrorLog::~ErrorLog()
{
	System::setErrorLog( NULL );

	delete m_fileStream;

	m_fileStream = NULL;
	
	delete m_textOutStream;

	m_textOutStream = NULL;

	m_errStream = NULL;
}

void ErrorLog::toLog( const String& text ) 
{
	m_errStream->write( text );
}