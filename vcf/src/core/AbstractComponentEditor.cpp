//AbstractComponentEditor.h
/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/
#include "ApplicationKit.h"
#include "ComponentEditor.h"
#include "AbstractComponentEditor.h"
#include "TextOutputStream.h"
#include "VFFOutputStream.h"

using namespace VCF;


AbstractComponentEditor::AbstractComponentEditor()
{
	m_commandCount = 0;
	m_defaultCommandIndex = 0;
	m_component = NULL;
}

AbstractComponentEditor::~AbstractComponentEditor()
{
	
}

void AbstractComponentEditor::intialize()
{
	if ( NULL != m_component ) {
		m_component->setComponentState( CS_DESIGNING );
	}
}


Command* AbstractComponentEditor::getCommand( const ulong32& index )
{
	return NULL;
}

void AbstractComponentEditor::setComponent( Component* component )
{
	m_component = component;
}

String AbstractComponentEditor::getComponentVFFFragment()
{
	String result;

	if ( NULL != m_component ) {
		TextOutputStream tos;
		VFFOutputStream vos(&tos);		
		
		vos.writeComponent( m_component );
		result = tos.getTextBuffer();
	}
	else {
		//throw exception here
	}

	return result;
}


/**
*AbstractControlEditor
*/

AbstractControlEditor::AbstractControlEditor()
{

}

AbstractControlEditor::~AbstractControlEditor()
{

}

bool AbstractControlEditor::isParentValid( Control* parent )
{
	return true;
}

Control* AbstractControlEditor::getControl()
{
	return dynamic_cast<Control*>( getComponent() );
}

void AbstractControlEditor::setControl( Control* control )
{
	setComponent( control );
}

void AbstractControlEditor::mouseDown( MouseEvent* event )
{

}

void AbstractControlEditor::mouseMove( MouseEvent* event )
{

}

void AbstractControlEditor::mouseUp( MouseEvent* event )
{

}