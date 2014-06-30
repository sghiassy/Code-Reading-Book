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

// Win32ResourceBundle.cpp
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"

using namespace VCF;


BOOL CALLBACK Win32ResourceBundle_EnumResTypeProc( HMODULE hModule, LPTSTR lpszType, LPARAM lParam );
BOOL CALLBACK Win32ResourceBundle_EnumResNameProc( HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LPARAM lParam );

static bool foundResName = false;
static String foundResType="Hello";

Win32ResourceBundle::Win32ResourceBundle()
{
	m_appPeer = NULL;
}

Win32ResourceBundle::~Win32ResourceBundle()
{
	
}

String Win32ResourceBundle::getString( const String& resourceName )
{	
	String result;
	HRSRC resHandle = ::FindResource( getResourceInstance(), resourceName.c_str(), RT_RCDATA );
	if ( NULL != resHandle ){
		HGLOBAL	data = ::LoadResource( NULL, resHandle );
		if ( NULL != data ){
			TCHAR* strData = (TCHAR*)::LockResource( data );
			result = strData;
			::FreeResource( data );
		}
	}
	else {
		//throw exception- resource not found !!!!
	}
	return result;
}

String Win32ResourceBundle::getVFF( const String& resourceName )
{
	String result;
	HRSRC resHandle = ::FindResource( getResourceInstance(), resourceName.c_str(), "VFF" );
	if ( NULL != resHandle ){
		HGLOBAL	data = ::LoadResource( NULL, resHandle );
		if ( NULL != data ){
			void* dataPtr = ::LockResource( data );
			TCHAR* strData = (TCHAR*)dataPtr;
			int size = SizeofResource( getResourceInstance(), resHandle );			
			result = strData;
			int resSize = result.size();
			void *tmp = dataPtr;
			/**
			*this is here to properly skip over '\0' characters in the stream
			*/
			while ( resSize < size ){
				tmp = (void*)((char*)dataPtr + resSize + 1);
				strData = (TCHAR*)tmp;
				result += "\n";
				result += strData;
				resSize = result.size();
			}
			::FreeResource( data );
		}
	}
	else {
		//throw exception- resource not found !!!!
	}
	return result;
}

Image* Win32ResourceBundle::getImage( const String& resourceName )
{
	HBITMAP resBMP = (HBITMAP)LoadImage( getResourceInstance(), resourceName.c_str(), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION ); 
	//LoadBitmap( getResourceInstance(), resourceName.c_str() );
	if ( NULL != resBMP ){
		return new Win32Image( resBMP );
	}
	else {
		//throw exception- resource not found !!!!
	}
	return NULL;
}

Resource* Win32ResourceBundle::getResource( const String& resourceName )
{
	foundResName = false;
	foundResType = "\0";
	
	::EnumResourceTypes( getResourceInstance(), 
		                 Win32ResourceBundle_EnumResTypeProc,
						 (LPARAM)resourceName.c_str() );
	if ( true == foundResName ){
		HRSRC resHandle = ::FindResource( getResourceInstance(), 
			                              resourceName.c_str(), 
										  foundResType.c_str() );
		if ( NULL != resHandle ){
			HGLOBAL	dataHandle = ::LoadResource( NULL, resHandle );
			if ( NULL != dataHandle ){
				void* data = ::LockResource( dataHandle );
				int size = ::SizeofResource( getResourceInstance(), resHandle );
				return new Resource( data, size, resourceName );				
			}
		}
		else {
			//throw exception- resource not found !!!!
		}
	}
	return NULL;
}

BOOL CALLBACK Win32ResourceBundle_EnumResTypeProc( HMODULE hModule, LPTSTR lpszType, LPARAM lParam )
{
	return ::EnumResourceNames( hModule,
		                        lpszType,
								Win32ResourceBundle_EnumResNameProc,
								lParam );
}

BOOL CALLBACK Win32ResourceBundle_EnumResNameProc( HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LPARAM lParam )
{
	BOOL result = TRUE;
	const String resName( (TCHAR*)lParam );
	if ( resName == lpszName ){
		foundResName = true;
		foundResType = lpszType;
		result = FALSE;
	}
	return result;
}

HINSTANCE Win32ResourceBundle::getResourceInstance()
{
	HINSTANCE result = NULL;	
	if ( NULL != this->m_appPeer ) {
		result = (HINSTANCE)m_appPeer->getHandleID();
	}
	else {
		//throw exception !!
	}
	return result;
}