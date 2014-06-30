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

// AbstractTextModel.cpp

#include "ApplicationKit.h"


using namespace VCF;


AbstractTextModel::AbstractTextModel()
{
	INIT_EVENT_HANDLER_LIST(ModelEmptied)
	INIT_EVENT_HANDLER_LIST(ModelValidate)

	INIT_EVENT_HANDLER_LIST(TextModelChanged)
}

AbstractTextModel::~AbstractTextModel()
{

}

void AbstractTextModel::validate()
{
	ValidationEvent event( this );

	fireOnModelValidate( &event );
}

void AbstractTextModel::empty()
{
	this->m_text = "";
	TextEvent event( this );
	
	fireOnTextModelChanged( &event );
}

void AbstractTextModel::setText( const String& text )
{
	TextEvent event( this, text );
	
	fireOnTextModelChanged( &event );
}

void AbstractTextModel::insertText( const unsigned long& index, const String& text )
{
	TextEvent event( this, text );

	fireOnTextModelChanged( &event );
}

void AbstractTextModel::deleteText( const unsigned long& index, const unsigned long& count )
{
	String changeText = this->m_text.substr( index, count );
	TextEvent event( this, changeText );
	
	fireOnTextModelChanged( &event );
}

void AbstractTextModel::appendText( const String& text )
{
	TextEvent event( this, text );

	fireOnTextModelChanged( &event );
}

String AbstractTextModel::getText()
{
	return this->m_text;
}

unsigned long AbstractTextModel::getSize()
{
	return this->m_text.size();
}
