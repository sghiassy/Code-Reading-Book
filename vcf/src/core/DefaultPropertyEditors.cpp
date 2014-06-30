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

//DefaultPropertyEditors.cpp
#include "ApplicationKit.h"
#include "Panel.h"
#include "DefaultPropertyEditors.h"
#include "DefaultListItem.h"
#include "TextControl.h"
#include "ListBoxControl.h"
#include "Label.h"

using namespace VCF;


AbstractPropertyEditor::AbstractPropertyEditor()
{
	m_source = NULL;
	m_property = NULL;
}

AbstractPropertyEditor::~AbstractPropertyEditor()
{

}

Control* AbstractPropertyEditor::getCustomEditor()
{
	return NULL;
}
	
void AbstractPropertyEditor::paintValue( GraphicsContext* context, const Rect& bounds )
{

}

void AbstractPropertyEditor::setSource( Object* source )
{
	m_source = source;
}

void AbstractPropertyEditor::setProperty( Property* property )
{
	m_property = property;
}

VariantData* AbstractPropertyEditor::getValue()
{
	/*VariantData* result = NULL;

	if ( NULL != m_property ){
		if ( NULL == m_source ){
			m_source = m_property->getSource();
		}
		if ( (NULL != this->m_property) && (NULL != m_source) ){
			result = m_property->get( m_source );
		}
	}
	*/
	return &m_data;
}

void AbstractPropertyEditor::setValue( VariantData* value )
{
	/*
	if ( NULL != m_property ){
		if ( NULL == m_source ){
			m_source = m_property->getSource();
		}
		if ( (NULL != this->m_property) && (NULL != m_source) ){
			m_property->set( m_source, value );
		}
	}
	*/
	m_data = *value;
}

bool AbstractPropertyEditor::hasCustomEditor()
{
	return false;
}
	
bool AbstractPropertyEditor::canPaintValue()
{
	return false;
}

String AbstractPropertyEditor::getValueAsText()
{
	/*
	String result = "null";
	if ( NULL != this->m_property ){
		VariantData* val = this->getValue();
		if ( NULL != val ){
			result = val->toString();
		}		
	}
	*/
	return m_data.toString();
}

void AbstractPropertyEditor::setValueAsText( const String& textValue )
{
	/*
	if ( NULL != m_property ){
		if ( NULL == m_source ){
			m_source = m_property->getSource();
		}
		if ( (NULL != this->m_property) && (NULL != m_source) ){
			m_property->set( m_source, textValue );
		}
	}
	*/
	m_data.setFromString( textValue );
}

String AbstractPropertyEditor::getCodeString()
{
	return "";
}


BoolPropertyEditor::BoolPropertyEditor()
{
	m_comboBoxHandler = 
		new ComboBoxItemEventHandler<BoolPropertyEditor>( this, BoolPropertyEditor::onSelectionChange, "ComboBoxAdapter" );		
	
}

BoolPropertyEditor::~BoolPropertyEditor()
{
	
}

Control* BoolPropertyEditor::getCustomEditor()
{
	m_combo = new ComboBoxControl();
	m_combo->addSelectionChangedHandler( m_comboBoxHandler );
	
	ListModel* lm = m_combo->getListModel();
	if ( NULL != lm ){
		lm->addItem( new DefaultListItem( lm, "true" ) );
		lm->addItem( new DefaultListItem( lm, "false" ) );		
	}

	bool val = *(this->getValue());
	ulong32 selectedIndex = (val) ? 0 : 1;
	m_combo->setSelectedItemIndex( selectedIndex );

	return 	m_combo;
}

void BoolPropertyEditor::onSelectionChange( ItemEvent* event )
{
	ListItem* item = (ListItem*)event->getSource();
	if ( NULL != item ){
		ulong32 selectedIndex = item->getIndex();
		this->m_data = (selectedIndex == 0) ? true : false;
	}
	
}

EnumPropertyEditor::EnumPropertyEditor()
{
	m_comboBoxHandler = 
		new ComboBoxItemEventHandler<EnumPropertyEditor>( this, EnumPropertyEditor::onSelectionChange, "ComboBoxAdapter" );		
	
}

EnumPropertyEditor::~EnumPropertyEditor()
{

}

Control* EnumPropertyEditor::getCustomEditor()
{
	m_combo = new ComboBoxControl();
	m_combo->addSelectionChangedHandler( m_comboBoxHandler );
	
	ListModel* lm = m_combo->getListModel();
	
	Enum* val = *(this->getValue());

	int current = val->get();

	int end = val->end();
	int start = val->begin();
	String s1 = val->toString();
	int next = val->next();
	String s2 = val->toString();
	if ( NULL != lm ){
		lm->addItem( new DefaultListItem( lm, s1 ) );
		lm->addItem( new DefaultListItem( lm, s2 ) );
		while ( next != end ) {			
			next = val->next();
			lm->addItem( new DefaultListItem( lm, val->toString() ) );
		}
	}	
	
	m_combo->setSelectedItemIndex( current );

	return 	m_combo;
}

void EnumPropertyEditor::onSelectionChange( ItemEvent* event )
{
	ListItem* item = (ListItem*)event->getSource();
	if ( NULL != item ){
		ulong32 selectedIndex = item->getIndex();
		Enum* val = m_data;
		val->set( selectedIndex );

		this->m_data = val;
	}
}



class ColorPanel : public Panel {
public:
	ColorPanel(){
		
	}

	virtual ~ColorPanel(){

	}

};


ColorPropertyEditor::ColorPropertyEditor()
{

}

ColorPropertyEditor::~ColorPropertyEditor()
{

}

void ColorPropertyEditor::paintValue( GraphicsContext* context, const Rect& bounds )
{
	Color* c = (Color*)(Object*)(*(this->getValue()));
	Color* oldColor = context->getColor();
	
	Rect innerBds = bounds;
	
	innerBds.inflate( -2, -2 );
	context->setColor( Color::getColor("black") );
	context->rectangle( &innerBds );
	context->strokePath();

	innerBds.inflate( -3, -3 );
	context->setColor( c );
	context->rectangle( &innerBds );
	context->fillPath();
	context->setColor( oldColor );
}

Control* ColorPropertyEditor::getCustomEditor()
{	
	return new ModalPropertyEditorControl<ColorPropertyEditor>( 
			ColorPropertyEditor::showColorEditor, this->getValue(), this );
}

void ColorPropertyEditor::showColorEditor( VariantData* data )
{
	CommonColor clrDlg;
	Color* c = (Color*)(Object*)(*(this->getValue()));
	clrDlg.setSelectedColor( c );
	if ( true == clrDlg.execute() ){
		VariantData data;
		c->copy( clrDlg.getSelectedColor() );
		data = c;
		this->setValue( &data );
	}
}

FontPropertyEditor::FontPropertyEditor()
{

}

FontPropertyEditor::~FontPropertyEditor()
{

}

void FontPropertyEditor::paintValue( GraphicsContext* context, const Rect& bounds )
{
	Font* f = (Font*)(Object*)(*(this->getValue()));
	Font* oldFont = context->getCurrentFont();
	Color* oldColor = context->getColor();
	Rect innerBds = bounds;
	
	innerBds.inflate( -2, -2 );
	context->setColor( Color::getColor("black") );
	context->rectangle( &innerBds );
	context->strokePath();
	
	context->setColor( oldColor );

	innerBds.inflate( -3, 0 );
	//context->setCurrentFont( f );	
	context->textAt( innerBds.m_left + 2, innerBds.m_top, "ABC" );
	
	context->setCurrentFont( oldFont );	
}

Control* FontPropertyEditor::getCustomEditor()
{	
	return new ModalPropertyEditorControl<FontPropertyEditor>( 
			FontPropertyEditor::showFontEditor, this->getValue(), this );
}

void FontPropertyEditor::showFontEditor( VariantData* data )
{
	CommonFont fontDlg(NULL);
	Font* f = (Font*)(Object*)(*(this->getValue()));
	fontDlg.setSelectedFont( f );
	if ( true == fontDlg.execute() ){
		VariantData data;
		f->copy( fontDlg.getSelectedFont() );
		data = f;
		this->setValue( &data );
	}
}

DefaultMenuItemPropertyEditor::DefaultMenuItemPropertyEditor()
{

}

DefaultMenuItemPropertyEditor::~DefaultMenuItemPropertyEditor()
{

}

Control* DefaultMenuItemPropertyEditor::getCustomEditor()
{
	return new ModalPropertyEditorControl<DefaultMenuItemPropertyEditor>( 
			DefaultMenuItemPropertyEditor::showDefaultMenuItemEditor, this->getValue(), this );
}

void DefaultMenuItemPropertyEditor::showDefaultMenuItemEditor( VariantData* data )
{
	Dialog::showMessage( "DefaultMenuItemPropertyEditor::showDefaultMenuItemEditor( VariantData* data )" );
}

DefaultListModelPropertyEditor::DefaultListModelPropertyEditor()
{

}

DefaultListModelPropertyEditor::~DefaultListModelPropertyEditor()
{

}

Control* DefaultListModelPropertyEditor::getCustomEditor()
{
	return new ModalPropertyEditorControl<DefaultListModelPropertyEditor>( 
			DefaultListModelPropertyEditor::showDefaultListModelEditor, this->getValue(), this );
}


void DefaultListModelPropertyEditor::showDefaultListModelEditor( VariantData* data )
{
	Dialog* d = new Dialog();
	
	d->setBounds( &Rect(400,400,750,650) );
		
	Panel* contents = new Panel();

	Panel* bottom = new Panel();
	bottom->setBounds( &Rect(0,0,10,30) );
	d->add( bottom, ALIGN_BOTTOM );
	d->add( contents, ALIGN_CLIENT );
	contents->setBorder( NULL );
	bottom->setBorder( NULL );

	CommandButton* okBtn = new CommandButton();
	okBtn->setBounds( &Rect(160,4,240,26) );
	okBtn->setCaption( "OK" );
	okBtn->setCommandType( BC_OK );

	CommandButton* cancelBtn = new CommandButton();
	cancelBtn->setBounds( &Rect(252,4,332,26) );
	cancelBtn->setCaption( "Cancel" );
	cancelBtn->setCommandType( BC_CANCEL );
	bottom->add( okBtn );
	bottom->add( cancelBtn );
	
	d->setCaption( "Edit List Model" );
	
	Rect r = *contents->getBounds();
	r.inflate( -5,-5 );
	
	ListBoxControl* lbc = new ListBoxControl();
	lbc->setBounds( &Rect( 5, 5, 120, r.m_bottom ) );
	contents->add( lbc );
	
	ListModel* lm = lbc->getListModel();	

	Label* l = new Label();
	TextControl* tc = new TextControl();
	Rect dlgRect = *d->getBounds();
	tc->setBounds( &Rect( dlgRect.getWidth() - 120, 10, dlgRect.getWidth() - 5, 35 ) );
	l->setBounds( &Rect( tc->getLeft() - 80, 10, tc->getLeft(), 35 ) );
	l->setCaption( "Item Caption" );
	contents->add( l );
	contents->add( tc );

	CommandButton* addBtn = new CommandButton();
	Rect* tcRect = tc->getBounds();
	addBtn->setBounds( &Rect( tc->getLeft(), tcRect->getBottom() + 10, tc->getLeft() + 80, tcRect->getBottom() + 35 ) );
	addBtn->setCaption( "Add Item" );

	tcRect = addBtn->getBounds();
	CommandButton* removeBtn = new CommandButton();
	removeBtn->setBounds( &Rect( tc->getLeft(), tcRect->getBottom() + 10, tc->getLeft() + 80, tcRect->getBottom() + 35 ) );
	removeBtn->setCaption( "Remove Item" );		

	contents->add( addBtn );
	contents->add( removeBtn );

	d->showModal();

	delete d;
	d = NULL;
}

