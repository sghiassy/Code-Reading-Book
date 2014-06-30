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



using namespace VCF;

Win32FolderBrowseDialog::Win32FolderBrowseDialog()
{

}

Win32FolderBrowseDialog::~Win32FolderBrowseDialog()
{

}

void Win32FolderBrowseDialog::setTitle( const String& title )
{
	m_title = title;
}

bool Win32FolderBrowseDialog::execute()
{
	bool result = false;
	BROWSEINFO info = {0};
	TCHAR displayName[MAX_PATH];
	memset(displayName,0,MAX_PATH);

	int sz = m_title.size() + 1;
	TCHAR* title = new TCHAR[sz];
	memset( title, 0, sz );
	m_title.copy( title, m_title.size() );

	info.hwndOwner = ::GetActiveWindow();
	info.pidlRoot = NULL;
	info.pszDisplayName = displayName;
	info.lpszTitle = title;
	info.ulFlags = 0; 

	LPITEMIDLIST itemIDList = SHBrowseForFolder( &info );
	if ( NULL != itemIDList ){
		TCHAR path[MAX_PATH];
		memset(path,0,MAX_PATH);
		if ( TRUE == SHGetPathFromIDList( itemIDList, path ) ){
			m_directory = path;
			result = true;
		}
	}

	return result;

}

void Win32FolderBrowseDialog::setDirectory( const String & directory )
{
	m_directory = m_directory;
}

String Win32FolderBrowseDialog::getDirectory()
{
	return m_directory;
}