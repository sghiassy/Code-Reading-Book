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
#include "Toolbar.h"
#include "ToolbarDock.h"
#include "ToolbarButton.h"


using namespace VCF;

static GraphicsContext* s_tempDesktopCtx = NULL;

Toolbar::Toolbar():
	CustomControl()
{
	init();	
}

Toolbar::~Toolbar()
{

}


void Toolbar::init()
{
	setContainer( this );
	setHeight( DEFAULT_TOOLBAR_HEIGHT );
	setWidth( 200 );
	this->setAlignment( ALIGN_CLIENT );
	m_dock = NULL;
	m_dockRow = 0;
	m_isDocked = false;
	m_isHandleVisible = true;
	m_gripperWidth = 9;
	m_buttonsClientRect.setRect( 0.0,0.0,0.0,0.0);
	m_gripperHandleRect.setRect( 0.0,0.0,0.0,0.0);
	m_toolbarIsDragging = false;
}

ToolbarDock* Toolbar::getDock()
{
	return m_dock;
}

void Toolbar::setBounds( Rect* rect, const bool& anchorDeltasNeedUpdating )
{
	CustomControl::setBounds( rect, anchorDeltasNeedUpdating );
	if ( true == m_isHandleVisible ) {
		m_gripperHandleRect.setRect( rect->m_left, rect->m_top, rect->m_left + m_gripperWidth, rect->m_bottom );
	}
	else {
		m_gripperHandleRect.setRect( 0.0,0.0,0.0,0.0);
	}
	m_buttonsClientRect.setRect( m_gripperHandleRect.m_right,  rect->m_top, rect->m_right, rect->m_bottom );
	m_buttonsClientRect.inflate( -2, -2 );
}

void Toolbar::setDock( ToolbarDock* dock )
{
	ToolbarDock* oldDock = m_dock;
	m_dock = dock;
	dockChanged( oldDock );
}

void Toolbar::dockChanged( ToolbarDock* oldDock )
{
	
}

ulong32 Toolbar::getDockRow()
{
	return m_dockRow;
}

void Toolbar::setDockRow( const ulong32& dockRow )
{
	m_dockRow = dockRow;
}

bool Toolbar::isDocked()
{
	return m_isDocked;
}

void Toolbar::paint( GraphicsContext* context )
{
	CustomControl::paint( context );
	Rect r(0, 0, getWidth(), getHeight() );
	//r.inflate( -1, -1 );
	Light3DBorder bdr;
	bdr.paint( &r, context );

	this->paintChildren( context );

	this->paintHandle( context );
}

bool Toolbar::isHandleVisible()
{
	return m_isHandleVisible;
}

void Toolbar::setHandleVisible( const bool& handleVisible )
{
	m_isHandleVisible = handleVisible;
}

void Toolbar::paintHandle( GraphicsContext* context )
{
	if ( true == this->m_isHandleVisible ) {
		Rect r = this->m_gripperHandleRect;
		r.inflate( 0, -2 );
		Rect hilite( r.m_left+2, r.m_top, r.m_left + 7, r.m_bottom );
		Light3DBorder bdr;
		bdr.paint( &hilite, context );
		hilite.setRect( r.m_left + 6, r.m_top, r.m_left + 11, r.m_bottom );
		bdr.paint( &hilite, context );
	}
}

void Toolbar::add( Control* child )
{
	ToolbarButton* btn = dynamic_cast<ToolbarButton*>( child );
	if ( NULL != btn ) {
		this->addButton( btn );	
	}
	else {
		AbstractContainer::add( child, ALIGN_LEFT );	
		child->setWidth( 80 );
	}
}

void Toolbar::addButton( ToolbarButton* button )
{
	AbstractContainer::add( button, ALIGN_LEFT );
	
	button->setWidth( button->getPreferredWidth() );	
}

void Toolbar::removeButton( ToolbarButton* button )
{
	this->remove( button );
}

void Toolbar::resizeChildren()
{		
	Rect rect = m_buttonsClientRect;

	this->resizeChildrenUsingBounds( &rect );
}

void Toolbar::mouseDown( MouseEvent* event )
{
	CustomControl::mouseDown( event );	
	if ( this->m_gripperHandleRect.containsPt( event->getPoint() ) )  {
		this->m_toolbarIsDragging = event->hasLeftButton();
		ToolbarDock* dockParent = dynamic_cast<ToolbarDock*>(getParent() );
		if ( NULL != dockParent ) {
			m_toolbarIsDragging = m_toolbarIsDragging && dockParent->allowsDragging();
		}
	}

	if ( true == m_toolbarIsDragging ) {
		keepMouseEvents();
		m_dragPt = *(event->getPoint());
		//convert to desktop's screen coords
		Desktop::getDesktop()->translateToScreenCoords( this, &m_dragPt );
		
		m_draggingOutlineRect = *( getBounds() );
		Desktop::getDesktop()->translateToScreenCoords( this, &m_draggingOutlineRect );

		Desktop::getDesktop()->beginPainting();
		s_tempDesktopCtx = Desktop::getDesktop()->getContext();
		s_tempDesktopCtx->setXORModeOn( true );
		gripperHandleMouseMove( event );
	}
}

void Toolbar::mouseMove( MouseEvent* event )
{
	CustomControl::mouseMove( event );	

	if ( (true == m_toolbarIsDragging) && (event->hasLeftButton()) ) {
		gripperHandleMouseMove( event );
	}
}

void Toolbar::mouseUp( MouseEvent* event )
{
	CustomControl::mouseUp( event );
	if ( true == this->m_toolbarIsDragging ) {
		this->releaseMouseEvents();
		gripperHandleMouseMove( event );

		/**
		*determine the parent - 
		*two choices 1 we belong to a ToolbarDock
		*0r 2 we belong to a FloatingToolbar
		*/

		Control* parent = getParent();
		Toolbar::FloatingToolbar* floaterParent = dynamic_cast<Toolbar::FloatingToolbar*>(parent );
		if ( NULL != floaterParent ) {
			Point pt = *(event->getPoint());
			Desktop::getDesktop()->translateToScreenCoords( this, &pt );
			ToolbarDock* dock = DockManager::getDockManager()->findDock( &pt );
			if ( NULL != dock ) {
				dock->addToolbar( this );
				floaterParent->close();
				delete floaterParent;
			}
		}
		else {
			ToolbarDock* dockParent = dynamic_cast<ToolbarDock*>(parent );
			if ( NULL != dockParent ) {
				Toolbar::FloatingToolbar* newFloater = new Toolbar::FloatingToolbar();
				//newFloater	->setBounds( &m_draggingOutlineRect );
				newFloater->setClientBounds( &m_draggingOutlineRect );
				newFloater->add( this, ALIGN_CLIENT );
				
				newFloater->setCaption( "FooBar" );
				newFloater->setVisible( true );
			}
			else {
				//throw exception we have an invalid parent !!!
			}
		}		

		s_tempDesktopCtx->setXORModeOn( false );
		s_tempDesktopCtx = NULL;
		Desktop::getDesktop()->endPainting();
	}

	m_toolbarIsDragging = false;
}

void Toolbar::gripperHandleMouseMove( MouseEvent* event )
{
	Point pt = *(event->getPoint());
	Desktop::getDesktop()->translateToScreenCoords( this, &pt );
	if ( NULL != s_tempDesktopCtx ) {
		Rect tmpBounds = *(getBounds());
		Desktop::getDesktop()->translateToScreenCoords( this , &tmpBounds );
		s_tempDesktopCtx->rectangle( &m_draggingOutlineRect );
		s_tempDesktopCtx->strokePath();

		double offsetX = pt.m_x - m_dragPt.m_x;
		double offsetY = pt.m_y - m_dragPt.m_y;
		
		m_draggingOutlineRect.m_left = tmpBounds.m_left + offsetX;
		m_draggingOutlineRect.m_top = tmpBounds.m_top + offsetY;
		m_draggingOutlineRect.m_right = tmpBounds.m_right + offsetX;
		m_draggingOutlineRect.m_bottom = tmpBounds.m_bottom + offsetY;

		//m_dragPt = *(event->getPoint());

		s_tempDesktopCtx->rectangle( &m_draggingOutlineRect );
		s_tempDesktopCtx->strokePath();
	}
}

Toolbar::FloatingToolbar::FloatingToolbar()
{
	this->setFrameStyle( FST_TOOLBAR_BORDER_FIXED );
}

Toolbar::FloatingToolbar::~FloatingToolbar()
{

}