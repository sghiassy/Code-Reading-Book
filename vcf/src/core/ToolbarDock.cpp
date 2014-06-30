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
#include "ToolbarDock.h"
#include "Toolbar.h"


#include <algorithm>

using namespace VCF;




ToolbarDock::ToolbarDock()
{
	setContainer( this );
	m_dockSide = TDS_TOP;
	m_backgroundImage = NULL;
	m_isUsingBackground = false;
	m_allowsDockDragging = true;
	m_backgroundStyle = TBKS_NONE;
	setHeight( DEFAULT_TOOLBAR_HEIGHT );
	this->setAlignment( ALIGN_TOP );
	DockManager::getDockManager()->addDock( this );

	m_originalHeightWidth = 0.0;
}

ToolbarDock::~ToolbarDock()
{
	DockManager::getDockManager()->removeDock( this );
}

void ToolbarDock::recalcToolbarLayout()
{
	int currentDockRow = -1;
	double totalDelta = 0.0;
	Enumerator<Control*>* children = this->getChildren();
	while ( children->hasMoreElements() ) {
		Control* child = children->nextElement();
		Toolbar* toolbar = (Toolbar*)child;
		if ( currentDockRow != toolbar->getDockRow() ) {
			currentDockRow = toolbar->getDockRow();
			switch ( this->m_dockSide ) {
				case TDS_TOP : case TDS_BOTTOM : {				
					totalDelta += toolbar->getPreferredHeight() + 4;
					toolbar->setWidth( 200 );
				}
				break;

				case TDS_LEFT : case TDS_RIGHT :{
					totalDelta += toolbar->getWidth() + 4;
				}
				break;
			}	
		}
	}

	switch ( this->m_dockSide ) {
		case TDS_TOP : case TDS_BOTTOM : {				
			setHeight( totalDelta );
		}
		break;

		case TDS_LEFT : case TDS_RIGHT :{
			setWidth( totalDelta );
		}
		break;
	}
}

void ToolbarDock::add( Control* child )
{
	AbstractContainer::add( child );
	recalcToolbarLayout();
}

void ToolbarDock::add( Control* child, const AlignmentType& alignment )
{
	AbstractContainer::add( child, alignment );
	recalcToolbarLayout();
}

ToolbarDockSide ToolbarDock::getDockSide()
{
	return m_dockSide;
}

void ToolbarDock::setDockSide( const ToolbarDockSide& dockSide )
{
	m_dockSide = dockSide;
}

Image* ToolbarDock::getBackground()
{
	return m_backgroundImage;
}

void ToolbarDock::setBackground( Image* background )
{
	this->m_backgroundImage = background;
}

ToolbarBackgroundStyle ToolbarDock::getBackgroundStyle()
{
	return this->m_backgroundStyle;
}

void ToolbarDock::setBackgroundStyle( const ToolbarBackgroundStyle& backgroundStyle )
{
	m_backgroundStyle = backgroundStyle;
}

bool ToolbarDock::isUsingBackground()
{
	return this->m_isUsingBackground;
}

void ToolbarDock::setUsingBackground( const bool& usingBackground )
{
	m_isUsingBackground = usingBackground;
}

bool ToolbarDock::allowsDragging()
{
	return this->m_allowsDockDragging;
}

void ToolbarDock::setAllowsDragging( const bool& allowsDrag )
{
	m_allowsDockDragging = allowsDrag;
}

void ToolbarDock::paint( GraphicsContext* context )
{
	CustomControl::paint( context );
	if ( CS_DESIGNING == this->getComponentState() ){
		Rect r = *this->getBounds();
		r.inflate( -1, -1 );
		Color* oldColor = context->getColor();
		context->setColor( Color::getColor("black") );
		context->rectangle( &r );
		context->strokePath();
		context->setColor( oldColor );
	}
	this->paintChildren( context );
}

void ToolbarDock::mouseDown( MouseEvent* event )
{

}

void ToolbarDock::mouseMove( MouseEvent* event )
{

}

void ToolbarDock::mouseUp( MouseEvent* event )
{

}

void ToolbarDock::addToolbar( Toolbar* toolbar )
{
	switch ( m_dockSide ) {
		case TDS_TOP : {
			setHeight( m_originalHeightWidth );
			add( toolbar, ALIGN_LEFT );	
		}
		break;
		
		case TDS_LEFT : {
			setWidth( m_originalHeightWidth );
			add( toolbar, ALIGN_TOP );	
		}
		break;
		
		case TDS_RIGHT : {
			setWidth( m_originalHeightWidth );
			add( toolbar, ALIGN_TOP );	
		}
		break;
		
		case TDS_BOTTOM : {
			setHeight( m_originalHeightWidth );
			add( toolbar, ALIGN_LEFT );	
		}
		break;
	}	
}

Toolbar* ToolbarDock::findToolbar( Point* pt )
{
	Toolbar* result = NULL;
	return result;
}

void ToolbarDock::remove( Control* child )
{
	AbstractContainer::remove( child );

	if ( this->getChildCount() <= 0 ) {
		switch ( m_dockSide ) {
			case TDS_TOP : {
				m_originalHeightWidth = getHeight();
				setHeight( 1 );
			}
			break;

			case TDS_LEFT : {
				m_originalHeightWidth = getWidth();
				setWidth( 1 );
			}
			break;

			case TDS_RIGHT : {
				m_originalHeightWidth = getWidth();
				setWidth( 1 );
			}
			break;

			case TDS_BOTTOM : {
				m_originalHeightWidth = getHeight();
				setHeight( 1 );
			}
			break;
		}

		Control* parent = getParent();
		Container* parentContainer = dynamic_cast<Container*>( parent );
		if ( NULL != parentContainer ) {
			parentContainer->resizeChildren();
		}
	}
}



/**
*DockManager implementation
*/

DockManager* DockManager::create()
{
	if ( NULL == DockManager::dockManagerInstance ) {
		DockManager::dockManagerInstance = new DockManager();
		DockManager::dockManagerInstance->init();
	}

	return DockManager::dockManagerInstance;
}


DockManager::DockManager()
{

}

DockManager::~DockManager()
{

}

void DockManager::removeDock( ToolbarDock* dock )
{
	std::vector<ToolbarDock*>::iterator found = std::find( m_docks.begin(), m_docks.end(), dock );
	if ( found != m_docks.end() ) {
		m_docks.erase( found );
	}
}

ToolbarDock* DockManager::findDock( Point* point )
{
	ToolbarDock* result = NULL;

	std::vector<ToolbarDock*>::iterator it = m_docks.begin();
	Point tmpPt;
	while ( it != m_docks.end() ) {		
		ToolbarDock* dock = *it;
		tmpPt = *point;
		Desktop::getDesktop()->translateFromScreenCoords( dock->getParent(), &tmpPt );
		Rect bounds = *(dock->getBounds());
		switch ( dock->getDockSide() ) {
			case TDS_TOP : case TDS_BOTTOM : {				
				if ( dock->getChildCount() == 0 ) {
					bounds.inflate( 0, 20 );
				}
			}
			break;

			case TDS_LEFT : case TDS_RIGHT :{
				if ( dock->getChildCount() == 0 ) {
					bounds.inflate( 20, 0 );
				}
			}
			break;
		}
		
		if ( true == bounds.containsPt( &tmpPt ) ) {
			result = dock;
			break;
		}

		it++;
	}
	return result;
}

void DockManager::addFloatingToolbar( Toolbar* toolbar )
{
	m_floatingToolbars.push_back( toolbar );
}

void DockManager::removeFloatingToolbar( Toolbar* toolbar )
{
	std::vector<Toolbar*>::iterator found = std::find( m_floatingToolbars.begin(), m_floatingToolbars.end(), toolbar );
	if ( found != m_floatingToolbars.end() ) {
		m_floatingToolbars.erase( found );
	}
}