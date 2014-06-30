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
//TabbedPages.cpp
#include "ApplicationKit.h"
#include "TabbedPages.h"
#include "DefaultTabModel.h"
#include "DefaultTabPage.h"
#include "Panel.h"

using namespace VCF;

TabbedPages::TabbedPages():
	CustomControl( true )
{
	init();
}

void TabbedPages::init()
{	
	this->setBorder( NULL );
	//add a listener to the tab model
	this->setContainer( this );	
	TabModelEventHandler<TabbedPages>* tabPageAddedHandler = 
		new TabModelEventHandler<TabbedPages>( this, TabbedPages::onTabPageAdded, "TabbedPages::tabTabPageAddedHandler" );

	TabModelEventHandler<TabbedPages>* tabPageRemovedHandler = 
		new TabModelEventHandler<TabbedPages>( this, TabbedPages::onTabPageRemoved, "TabbedPages::tabPageRemovedHandler" );

	TabModelEventHandler<TabbedPages>* tabPageSelectedHandler = 
		new TabModelEventHandler<TabbedPages>( this, TabbedPages::onTabPageSelected, "TabbedPages::tabPageSelectedHandler" );

	

	DefaultTabModel* defaultTabModel = new DefaultTabModel();
	this->m_model = defaultTabModel;

	m_model->addTabPageAddedHandler( tabPageAddedHandler );
	m_model->addTabPageRemovedHandler( tabPageRemovedHandler );
	m_model->addTabPageSelectedHandler( tabPageSelectedHandler );	

	m_borderWidth = 8;	
	m_tabHeight = 12;
}

TabbedPages::~TabbedPages()
{
	if ( NULL != m_model ){
		delete m_model;
	}
}


void TabbedPages::paint( GraphicsContext* context )
{
	CustomControl::paint( context );
	Rect tabsRect( 0.0, 0.0, 0.0, 0.0 );
	Rect* bounds = this->getBounds();
	FillState fill;
	fill.m_Color = *( this->getColor() );
	context->rectangle( 0, 0, bounds->getWidth(), bounds->getHeight() );
	context->setCurrentFillState( &fill );
	context->fillPath();
	
	if ( NULL != m_model ){
		Enumerator<TabPage*>* pages = m_model->getPages();
		if ( NULL != pages ){
			int currentLeft = m_tabAreaBounds.m_left;
			double width = 0.0;
			while ( true == pages->hasMoreElements() ){
				TabPage* page = pages->nextElement();
				if ( NULL != page ){
					width = context->getTextWidth( page->getPageName() ) + 5.0 + m_tabHeight;
					tabsRect.setRect( currentLeft, 3, 
						              currentLeft+ width, 3 + m_tabHeight  );					
					page->paint( context, &tabsRect );
				}
				currentLeft += width;
				width = 0.0;
				if ( true == page->isSelected() ) {
					Control* component = page->getPageComponent();
					
					if ( NULL != component ){
						Rect tmp( *(component->getBounds()) );
						tmp.inflate( 3,3 );
						m_activePageBorder.paint( &tmp, context );					
					}
				}
			}
			
		}
	}
	this->paintChildren( context );
}

TabModel* TabbedPages::getModel()
{
	return m_model;
}

void TabbedPages::setTabModel( TabModel* model )
{
	m_model = model;
}

void TabbedPages::onTabPageAdded( TabModelEvent* event )
{

}

void TabbedPages::onTabPageRemoved( TabModelEvent* event )
{

}

void TabbedPages::onTabPageSelected( TabModelEvent* event )
{
	TabPage* page = event->getTabPage();
	if ( true == page->isSelected() ){
		Enumerator<Control*>* children = this->getChildren();
		while ( true == children->hasMoreElements() ){
			Control* comp = children->nextElement();
			if ( comp != page->getPageComponent() ){
				comp->setVisible( false );
			}
		}
		page->getPageComponent()->setVisible( true );

		Container* container = dynamic_cast<Container*>( page->getPageComponent() );
		if ( NULL != container ) {
			container->resizeChildren();
		}		
	}
	this->repaint();
}

TabPage* TabbedPages::addNewPage( const String& caption )
{
	DefaultTabPage* page = new DefaultTabPage();
	page->setModel( this->getModel() );
	page->setPageName( caption );	
	Panel* sheet = new Panel();
	sheet->setShowBorder( false );
	page->setPageComponent( sheet );	
	sheet->setBorder( NULL );	
	this->add( sheet, ALIGN_CLIENT );
	sheet->setVisible( true );
	
	m_tabHeight = __max( m_tabHeight, page->getPreferredHeight() );

	TabModel* model = this->getModel();
	model->addTabPage( page );
	
	model->setSelectedPage( page );

	return page;
}

void TabbedPages::resizeChildren()
{
	Rect* bounds = this->getBounds();
	m_tabAreaBounds.setRect( 0, 0, bounds->getWidth(), bounds->getHeight() );
	m_tabAreaBounds.inflate( -m_borderWidth, -m_borderWidth );
	m_tabAreaBounds.m_top = m_tabAreaBounds.m_top + m_tabHeight;
	
	this->resizeChildrenUsingBounds( &m_tabAreaBounds );
}

void TabbedPages::mouseDown( MouseEvent* event )
{	
	CustomControl::mouseDown( event );	
	if ( true == this->m_tabAreaBounds.containsPt( event->getPoint() ) ){
		//find tab
		TabModel* model = this->getModel();
		if ( NULL != model ){
			Enumerator<TabPage*>* pages = model->getPages();
			if ( NULL != pages ){
				while ( true == pages->hasMoreElements() ){
					TabPage* page = pages->nextElement();
					if ( NULL != page ){
						if ( true == page->containsPoint( event->getPoint() ) ){
							this->m_model->setSelectedPage( page );
							this->resizeChildren();
							break;
						}
					}
				}
			}
		}
	}
}

void TabbedPages::mouseMove( MouseEvent* event )
{
	CustomControl::mouseMove( event );	
}

void TabbedPages::mouseUp( MouseEvent* event )
{
	CustomControl::mouseUp( event );	
}

ulong32 TabbedPages::getBorderWidth()
{
	return this->m_borderWidth;
}

void TabbedPages::setBorderWidth( const ulong32& borderWidth )
{
	m_borderWidth = borderWidth;
}
