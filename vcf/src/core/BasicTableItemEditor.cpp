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
#include "TableItemEditor.h"
#include "BasicTableItemEditor.h"
#include "TextControl.h"

using namespace VCF;

BasicTableItemEditor::BasicTableItemEditor( TableCellItem* editingItem )
{
	INIT_EVENT_HANDLER_LIST(CellItemChanged)
	INIT_EVENT_HANDLER_LIST(CellItemValidateChange)

	m_editingItem = editingItem;
	this->m_textModelHandler = 
		new TextModelEventHandler<BasicTableItemEditor>( this, BasicTableItemEditor::onEditorTextChanged );
	
}

BasicTableItemEditor::~BasicTableItemEditor()
{
	m_editorMap.clear();	
}	


bool BasicTableItemEditor::isCellEditable()
{
	return true;
}

void BasicTableItemEditor::setItemToEdit( TableCellItem* itemToEdit )
{
	m_editingItem = itemToEdit;
}

void BasicTableItemEditor::updateItem()
{
	if ( NULL != m_editingItem ){
		TextControl* editingControl = dynamic_cast<TextControl*>(getCurrentEditingControl(m_editingItem) );
		if ( NULL != editingControl ){
			TextModel* tm = editingControl->getTextModel();
			
			String captionOfItem = tm->getText();
			
			void* itemCaptionPtr = (void*)&captionOfItem;
			ItemEditorEvent event( this, itemCaptionPtr );
			event.setItemBeingEdited( m_editingItem );
			
			try {
				fireOnCellItemValidateChange( &event );
				
				event.setType( ITEMEDITOR_CHANGED );
				//if we got this far we are OK to send off a change notification !
				
				m_editingItem->setCaption( captionOfItem );
				fireOnCellItemChanged( &event );						
			}
			catch ( InvalidStateException& ex ){
				
			}
			catch (...){
				throw;//pass it on to someone else !!
			}
		}
	}
}

Control* BasicTableItemEditor::getEditingControl()
{
	if ( NULL != m_editingItem ){
		TextControl* tc = new TextControl();
		m_editorMap[m_editingItem] = tc;
		TextModel* tm = tc->getTextModel();
		tm->setText( m_editingItem->getCaption() );
		
		tm->addTextModelChangedHandler( m_textModelHandler );

		return tc;
	}
	
	return NULL;
}

void BasicTableItemEditor::onEditorTextChanged( TextEvent* event )
{
	String editorText = event->getTextModel()->getText();

}

Control* BasicTableItemEditor::getCurrentEditingControl( TableCellItem* currentItem )
{
	Control* result = NULL;
	std::map<TableCellItem*,Control*>::iterator found = m_editorMap.find( currentItem );
	if ( found != m_editorMap.end() ){
		result = found->second;
	}

	return result;
}