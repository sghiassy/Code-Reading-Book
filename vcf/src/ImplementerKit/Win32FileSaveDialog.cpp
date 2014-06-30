/**
*Visual Component Framework for C++
*
*Copyright © 2000 Jim Crafton
*
*This program is free software; you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation; either version 2
*of the License, or (at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*
*Contact Jim Crafton at ddiego@one.net for more information about the 
*Visual Component Framework
*/
//Win32FileSaveDialog.cpp
#include "ApplicationKit.h"
#include "Win32FileSaveDialog.h"
#include "Win32Peer.h"
#include "Commdlg.h"

//Win32FileSaveDialog.cpp


using namespace VCF;

Win32FileSaveDialog::Win32FileSaveDialog( Control* owner )
{
	this->init();
	m_owner = owner;
}


Win32FileSaveDialog::~Win32FileSaveDialog() 
{

}

void Win32FileSaveDialog::init()
{
	
	this->m_allowsMultiSelect = false;
	this->m_container.initContainer( this->m_selectedFiles );
	m_owner = NULL;
	
}

void Win32FileSaveDialog::setTitle( const String& title )
{
	this->m_title = title;
}

bool Win32FileSaveDialog::execute()
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

	if ( TRUE == GetSaveFileName( &ofn ) ){
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
	
	

	delete [] tmpFilter;

	delete [] tmpTitle;

	return result;
}

void Win32FileSaveDialog::addFilter( const String & description, const String & extension )
{
	m_filter.push_back( description );
	m_filter.push_back( extension );
}

void Win32FileSaveDialog::setDirectory( const String & directory )
{
	this->m_directory = directory;
}
    
void Win32FileSaveDialog::setFileName( const String & filename )
{
	this->m_fileName = filename;
}

String Win32FileSaveDialog::getFileName()
{
	return m_fileName;
}

String Win32FileSaveDialog::getDirectory()
{	
	return this->m_directory;
}

String Win32FileSaveDialog::getFileExtension()
{
	return "";
}

uint32 Win32FileSaveDialog::getSelectedFileCount()
{
	return m_selectedFiles.size();
}

Enumerator<String>* Win32FileSaveDialog::getSelectedFiles()
{
	return m_container.getEnumerator();
}

void Win32FileSaveDialog::setAllowsMultiSelect( const bool& allowsMultiSelect )
{
	this->m_allowsMultiSelect = allowsMultiSelect;
}