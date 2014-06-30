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

//CommonDialogPeer.h
#ifndef _COMMON_DIALOG_PEER_H__
#define _COMMON_DIALOG_PEER_H__


namespace VCF {

class Color;

class Font;

/**
*
*/
class APPKIT_API CommonDialogPeer : public Interface{ 
public:
	virtual ~CommonDialogPeer(){};

	virtual void setTitle( const String& title ) = 0;

	virtual bool execute() = 0;
};


/**
*
*/
class APPKIT_API CommonFileDialogPeer : public CommonDialogPeer{ 
public:
	virtual ~CommonFileDialogPeer(){};

	virtual void addFilter( const String & description, const String & extension ) = 0;

    virtual void setDirectory( const String & directory ) = 0;
    
	virtual void setFileName( const String & filename ) = 0;

    virtual String getFileName() = 0;

    virtual String getDirectory() = 0;

    virtual String getFileExtension() = 0;

	virtual uint32 getSelectedFileCount() = 0;

	virtual Enumerator<String>* getSelectedFiles() = 0;
	
	virtual void setAllowsMultiSelect( const bool& allowsMultiSelect ) = 0;
	
};

/**
*
*/
class APPKIT_API CommonFolderBrowseDialogPeer : public CommonDialogPeer{ 
public:
	virtual ~CommonFolderBrowseDialogPeer(){};

	virtual void setDirectory( const String & directory ) = 0;

	virtual String getDirectory() = 0;
	
};

/**
*
*/
class APPKIT_API CommonColorDialogPeer : public CommonDialogPeer {
public:
	virtual ~CommonColorDialogPeer(){};

	virtual Color* getSelectedColor() = 0;

	virtual void setSelectedColor( Color* selectedColor ) = 0;

};


/**
*
*/
class APPKIT_API CommonPrintDialogPeer : public CommonDialogPeer {
public:
	virtual ~CommonPrintDialogPeer(){};
	
};


/**
*
*/
class APPKIT_API CommonFontDialogPeer : public CommonDialogPeer {
public:
	virtual ~CommonFontDialogPeer(){};
	
	virtual Font* getSelectedFont() = 0;

	virtual void setSelectedFont( Font* selectedFont ) = 0;
};

};


#endif //_COMMON_DIALOG_PEER_H__