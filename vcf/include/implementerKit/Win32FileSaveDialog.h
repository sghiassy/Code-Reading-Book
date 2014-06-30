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
//Win32FileSaveDialog

#ifndef _WIN32_FILE_SAVE_DIALOG_H__
#define _WIN32_FILE_SAVE_DIALOG_H__


namespace VCF {


class APPKIT_API Win32FileSaveDialog : public CommonFileDialogPeer, public Object {
public:
	Win32FileSaveDialog( Control* owner=NULL );

	virtual ~Win32FileSaveDialog();

	void init();

	virtual void setTitle( const String& title );

	virtual bool execute();

	virtual void addFilter( const String & description, const String & extension );

    virtual void setDirectory( const String & directory );
    
	virtual void setFileName( const String & filename );

    virtual String getFileName();

    virtual String getDirectory();

    virtual String getFileExtension();

	virtual uint32 getSelectedFileCount();

	virtual Enumerator<String>* getSelectedFiles();
	
	virtual void setAllowsMultiSelect( const bool& allowsMultiSelect );

private:
	String m_title;
	std::vector<String> m_filter;
	std::vector<String> m_selectedFiles;
	String m_directory;
	String m_selectedFileExt;
	String m_fileName;
	EnumeratorContainer<std::vector<String>,String> m_container;
	bool m_allowsMultiSelect;
	Control* m_owner;

};


};


#endif //_WIN32_FILE_SAVE_DIALOG_H__