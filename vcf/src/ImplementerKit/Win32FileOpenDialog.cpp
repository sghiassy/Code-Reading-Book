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
#include "ApplicationKit.h"
#include "AppKitPeerConfig.h"


//Win32FileOpenDialog.cpp


using namespace VCF;

Win32FileOpenDialog::Win32FileOpenDialog( Control* owner )
{
	this->init();
	m_owner = owner;
}


Win32FileOpenDialog::~Win32FileOpenDialog() 
{

}

void Win32FileOpenDialog::init()
{
	
	this->m_allowsMultiSelect = false;
	this->m_container.initContainer( this->m_selectedFiles );
	m_owner = NULL;
	
}

void Win32FileOpenDialog::setTitle( const String& title )
{
	this->m_title = title;
}

bool Win32FileOpenDialog::execute()
{
	
	OPENFILENAME ofn = {0};
	ofn.lStructSize = sizeof(ofn);
	HWND ownerWnd = NULL;
	if ( NULL != m_owner ){
		ControlPeer* impl = m_owner->getPeer();
		ownerWnd = (HWND)impl->getHandleID();
	}

	ofn.hwndOwner = ownerWnd;
	ofn.hInstance = NULL;
	TCHAR tmpFileName[_MAX_PATH];
	memset(tmpFileName, 0, _MAX_PATH);
	TCHAR tmpDir[_MAX_PATH];
	memset(tmpDir, 0, _MAX_PATH);
	this->m_directory.copy( tmpDir, m_directory.size() );
	
	this->m_fileName.copy( tmpFileName, m_fileName.size() );

	ofn.lpstrFile = tmpFileName;
	ofn.lpstrInitialDir = tmpDir;
	ofn.nMaxFile = _MAX_PATH;
	
	TCHAR* tmpTitle = NULL;
	if ( this->m_title.size() > 0 ){
		tmpTitle = new TCHAR[m_title.size()+1];
		memset( tmpTitle, 0, m_title.size()+1 );
		m_title.copy( tmpTitle, m_title.size() );
	}

	ofn.lpstrTitle = tmpTitle;

	ofn.Flags = OFN_EXPLORER;

	if ( true == this->m_allowsMultiSelect ){
		ofn.Flags |= OFN_ALLOWMULTISELECT;
	}

	if ( m_filter.size() == 0 ) {//then add a default filter
		m_filter.push_back( "Any File" );
		m_filter.push_back( "*.*" );
	}

	int size = _MAX_PATH * m_filter.size();
	TCHAR *tmpFilter = new TCHAR[size];
	memset( tmpFilter, 0, size );	

	std::vector<String>::iterator filter = this->m_filter.begin();	
	String tmp;
	while ( filter != m_filter.end() ){
		tmp += *filter + '\0';
		filter++;
	}
	tmp += '\0';
	tmp.copy( tmpFilter, tmp.size() );

	ofn.lpstrFilter = 	tmpFilter;

	bool result = false;
	
	m_selectedFiles.clear();

	if ( TRUE == GetOpenFileName( &ofn ) ){
		result = true;
		TCHAR* fileStart = (char*)(ofn.lpstrFile + (ofn.nFileOffset-1));
		if ( *fileStart == '\0' ){

			this->m_directory = ofn.lpstrFile; //this represents the dir path - everything after this will be the file names

			TCHAR* s = fileStart + 1;
			bool atTheEnd = false;
			if ( *(s+1) == '\0' ){//double null terminators end the string
				atTheEnd = true;
			}
			while ( !atTheEnd ){
				this->m_selectedFiles.push_back( s );
				while ( *s != '\0' ){
					s++;
				}
				s++;//increment past the null seperator
				if ( *(s+1) == '\0' ){
					atTheEnd = true;
				}
			}
			m_fileName = "";
		}
		else {
			this->m_fileName = ofn.lpstrFile;
			m_directory = StringUtils::getFileDir( m_fileName );			
		}
	}
	
	delete [] tmpTitle;

	delete [] tmpFilter;

	return result;
}

void Win32FileOpenDialog::addFilter( const String & description, const String & extension )
{
	m_filter.push_back( description );
	m_filter.push_back( extension );
}

void Win32FileOpenDialog::setDirectory( const String & directory )
{
	this->m_directory = directory;
}
    
void Win32FileOpenDialog::setFileName( const String & filename )
{
	this->m_fileName = filename;
}

String Win32FileOpenDialog::getFileName()
{
	return m_fileName;
}

String Win32FileOpenDialog::getDirectory()
{	
	return this->m_directory;
}

String Win32FileOpenDialog::getFileExtension()
{
	return "";
}

uint32 Win32FileOpenDialog::getSelectedFileCount()
{
	return m_selectedFiles.size();
}

Enumerator<String>* Win32FileOpenDialog::getSelectedFiles()
{
	return m_container.getEnumerator();
}

void Win32FileOpenDialog::setAllowsMultiSelect( const bool& allowsMultiSelect )
{
	this->m_allowsMultiSelect = allowsMultiSelect;
}