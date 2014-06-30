/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/

// StringUtils.cpp

#include "FoundationKit.h"


#ifdef macintosh
	#include <stdio.h>
	#include <string.h>
	#include <extras.h>
#endif

#ifdef WIN32
#include "Win32Peer.h"
#include "Rpcdce.h" //make sure to link with Rpcrt4.lib
#endif

#define TO_STRING_TXT_SIZE		50

using namespace VCF;


StringUtils::StringUtils()
{

}

StringUtils::~StringUtils()
{

}

String StringUtils::getFileDir( const VCF::String& fileName )
{
	int pos = fileName.find_last_of( "\\" );
	if ( pos == -1 ){
		return "";
	}
	return fileName.substr( 0, pos );
}

String StringUtils::getFileName( const VCF::String& fileName )
{
	int pos = fileName.find_last_of( "\\" );
	if ( pos == -1 ){
		return "";
	}
	return fileName.substr( pos+1 );
}

String StringUtils::getFileExt( const VCF::String& fileName )
{
	int pos = fileName.find_last_of( "." );
	if ( pos == -1 ){
		return "";
	}
	return fileName.substr( pos+1 );
}

void StringUtils::traceWithArgs( String text,... )
{
//#ifdef _DEBUG	
	va_list argList;

	va_start( argList, text );     // Initialize variable arguments. 

	char* buf = new char[MAX_TRACE_STRING];
	memset( buf, 0, MAX_TRACE_STRING );

	_vsnprintf( buf, MAX_TRACE_STRING, text.c_str(), argList );

	va_end( argList );              // Reset variable arguments.      

	StringUtils::trace( String(buf) );

	delete [] buf;
//#endif   
}


void StringUtils::trace( const String& text )
{
//#ifdef _DEBUG
#ifdef WIN32
	VCFWin32::Win32Utils::trace( text );
#endif
//#endif
}

String StringUtils::lowerCase( const String& text )
{
	String result = "";
	
	char* copyText = new char[text.size()+1];
	memset(copyText, 0, text.size()+1 );
	text.copy( copyText, text.size() );

	_strlwr( copyText ); // not in ANSI standard library


	result = copyText;
	delete [] copyText;
	return result;
}

String StringUtils::upperCase( const VCF::String& text )
{
	String result = "";
	char* copyText = new char[text.size()+1];
	memset(copyText, 0, text.size()+1 );
	text.copy( copyText, text.size() );

	_strupr( copyText );

	result = copyText;
	delete [] copyText;
	return result;
}

VCF::String StringUtils::toString( const int& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%d", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const long& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%d", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const float& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%.5f", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const double& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%.5f", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const ulong32& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%d", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const uint32& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%d", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const char& value )
{
	char tmp[TO_STRING_TXT_SIZE];
	memset( tmp, 0, TO_STRING_TXT_SIZE );
	sprintf( tmp, "%c", value );
	return String( tmp );
}

VCF::String StringUtils::toString( const bool& value )
{
	return ( value == true ) ? "true" : "false";
}

VCF::String StringUtils::newUUID()
{
	VCF::String result = "";
#ifdef WIN32
	UUID id;
	if ( RPC_S_OK == ::UuidCreate( &id ) ){
		unsigned char *tmpid = NULL;
		
		RPC_STATUS rpcresult = UuidToString(  &id, &tmpid );
		
		if ( RPC_S_OUT_OF_MEMORY != rpcresult ) {
			result = VCF::String( (VCFChar*)tmpid );
			
			RpcStringFree( &tmpid );
		}
	}
#endif
	return result;
}

VCF::String StringUtils::format( VCF::String formatText, ... )
{
	VCF::String result = "";
	
	va_list argList;

	va_start( argList, formatText );     // Initialize variable arguments. 

	char* buf = new char[MAX_TRACE_STRING];
	memset( buf, 0, MAX_TRACE_STRING );

	_vsnprintf( buf, MAX_TRACE_STRING, formatText.c_str(), argList );

	va_end( argList );              // Reset variable arguments.      

	result = buf;

	delete [] buf;

	return result;
}

VCF::String StringUtils::getClassNameFromTypeInfo( const type_info& typeInfo  )
{	
	VCF::String result = "";
#ifdef WIN32 //don't know if we really need this here
		std::string tmp = typeInfo.name();  //put back in when we find typeid 
		//strip out the preceding "class" or "enum" or whatever
		std::string::size_type idx = tmp.find( " " );
		if ( idx != tmp.npos ) {
			tmp = tmp.substr( idx+1 );
		}
		
#ifndef KEEP_NAMESPACE_IN_CLASSNAME
		
		std::string::size_type idx = tmp.find( "::" );
		if ( idx == tmp.npos ) {
			result = tmp;  // not part of a namespace
		} else {
			result = tmp.substr( idx + 2 );  // strip namespace off from string
		}
		
#else
		
		result = tmp;
		
#endif	
		
#else //not supported in GCC compiler yet
		result = "unknown class instance.";
#endif

	return result;
}