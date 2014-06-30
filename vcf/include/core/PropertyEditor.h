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

//PropertyEditor.h

#ifndef _PROPERTY_EDITOR_H__
#define _PROPERTY_EDITOR_H__


namespace VCF
{

class Property;
class Control;
class GraphicsContext;

/**
*PropertyEditor
*/
class APPKIT_API PropertyEditor{
public:
	virtual ~PropertyEditor(){};

	/**
	*returns a new instance of the custom editor. The caller
	*is resposible for deleting the editor when it is finished 
	*with it.
	*Implementer of this interface should always make a new instance 
	*each time this is called.
	*/
	virtual Control* getCustomEditor() = 0;
	
	virtual void paintValue( GraphicsContext* context, const Rect& bounds ) = 0;

	/**
	*Sets the source for the Property editor
	*/
	virtual void setSource( Object* source ) = 0;

	/**
	*Sets the property of the Property editor
	*/
	virtual void setProperty( Property* property ) = 0;

	virtual VariantData* getValue() = 0;
	
	virtual void setValue( VariantData* value ) = 0;

	/**
	*is it OK to call getCustomEditor ?
	*/
	virtual bool hasCustomEditor() = 0;
	
	/**
	*is OK to call paintValue ?
	*/
	virtual bool canPaintValue() = 0;

	virtual String getValueAsText() = 0;

	virtual void setValueAsText( const String& textValue ) = 0;

	/**
	*called by an IDE to get the C++ string representation of 
	the property to insert into C++ code.
	*/
	virtual String getCodeString() = 0;
	
};


};

#endif