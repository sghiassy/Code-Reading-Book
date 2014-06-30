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




#ifndef TEXTMODEL_H
#define TEXTMODEL_H



namespace VCF{

#define TEXTMODEL_CLASSID	"8B2CDC31-3CAD-11d4-B553-00C04F0196DA"

/** @interface */
/**
*TextModel is a model for working with text data.
*
*@version 1.0
*@author Jim Crafton
*/
class APPKIT_API TextModel : public Model {
public:

	BEGIN_ABSTRACT_CLASSINFO(TextModel, "VCF::TextModel", "VCF::Model", TEXTMODEL_CLASSID)	
	EVENT("VCF::TextModelEventHandler", "VCF::TextEvent", "TextModelChanged" )
	END_CLASSINFO(TextModel)

	TextModel(){
		
	};

	virtual ~TextModel(){};

	/**
	*adds a new TextModelEvent handler
	*/
    virtual void addTextModelChangedHandler( EventHandler * handler ) = 0;

	/**
	*removes a TextModelEvent handler
	*/
    virtual void removeTextModelChangedHandler( EventHandler * handler ) = 0;

	/**
	*sets the contents of the text model, completely changes what was previously 
	*in the model
	*/
    virtual void setText( const String& text ) = 0;

	/**
	*inserts text into the model at the given index
	*@param long the starting point to begin inserting the text. This number represents
	*a zero based index.
	*@param String the text to insert
	*/
    virtual void insertText( const unsigned long& index, const String& text ) = 0;

	/**
	*deletes text from the model, starting at index, and continuing for count characters, 
	*or until the text data is empty.
	*@param long the starting point. The index is zero based.
	*@param long the number of characters to delete
	*/
    virtual void deleteText( const unsigned long& index, const unsigned long& count ) = 0;

	/**
	*adds text to end of the current text data
	*/
    virtual void appendText( const String& text ) = 0;

	/**
	*returns all of the TextModel's text in a string.
	*/
	virtual String getText() = 0;

	/**
	*returns the size of the TextModel
	*/
	virtual unsigned long getSize() = 0;

};

};
#endif //TEXTMODEL_HTMODEL_H