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

// AbstractTextModel.h

#ifndef _ABSTRACTTEXTMODEL_H__
#define _ABSTRACTTEXTMODEL_H__



namespace VCF
{

#define ABSTRACTTEXTMODEL_CLASSID	"8B2CDC33-3CAD-11d4-B553-00C04F0196DA"

/**
*
*/
class APPKIT_API AbstractTextModel : public TextModel  
{
public:
	BEGIN_ABSTRACT_CLASSINFO(AbstractTextModel, "VCF::AbstractTextModel", "VCF::TextModel", ABSTRACTTEXTMODEL_CLASSID)	
	END_CLASSINFO(AbstractTextModel)

	AbstractTextModel();

	virtual ~AbstractTextModel();

	EVENT_HANDLER_LIST(ModelEmptied)
	EVENT_HANDLER_LIST(ModelValidate)
	

    virtual void addModelValidationHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelValidate) 
	}
	
	virtual void removeModelValidationHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelValidate) 
	}

	virtual void addModelHandler( EventHandler* handler ) {
		ADD_EVENT_IMPL(ModelEmptied) 
	}

	virtual void removeModelHandler( EventHandler* handler ) {
		REMOVE_EVENT_IMPL(ModelEmptied) 
	}

	FIRE_EVENT(ModelValidate,ValidationEvent)

	FIRE_EVENT(ModelEmptied,ModelEvent)


	EVENT_HANDLER_LIST(TextModelChanged)

	virtual void addTextModelChangedHandler( EventHandler * handler ){
		ADD_EVENT_IMPL(TextModelChanged) 
	}

	virtual void removeTextModelChangedHandler( EventHandler * handler ) {
		REMOVE_EVENT_IMPL(TextModelChanged) 
	}

	FIRE_EVENT(TextModelChanged,TextEvent)

	/**
     * validate the model. 
     * The implementation for this can vary widely, or even be nonexistant for model's that do not require validation.
     * The basic idea is to call all the listeners in the list , passing in a local variable to the
     * onModelValidate() methods of the listener's. The variable is initialized to true, and if it is
     * still true at the end of the listener iterations, then it is safe to apply the changes to the
     * model, other wise the changes are removed. 
     */
    virtual void validate();

    /**
     * clears out the model's data 
     */
    virtual void empty();

	/**
	*sets the contents of the text model, completely changes what was previously 
	*in the model
	*/
    virtual void setText( const String& text );

	/**
	*inserts text into the model at the given index
	*@param long the starting point to begin inserting the text. This number represents
	*a zero based index.
	*@param String the text to insert
	*/
    virtual void insertText( const unsigned long& index, const String& text );

	/**
	*deletes text from the model, starting at index, and continuing for count characters, 
	*or until the text data is empty.
	*@param long the starting point. The index is zero based.
	*@param long the number of characters to delete
	*/
    virtual void deleteText( const unsigned long& index, const unsigned long& count );

	/**
	*adds text to end of the current text data
	*/
    virtual void appendText( const String& text );

	/**
	*returns all of the TextModel's text in a string.
	*/
	virtual String getText();

	/**
	*returns the size of the TextModel
	*/
	virtual unsigned long getSize();

protected:
	String m_text;
};

};


#endif 
