/**
*Visual Control Framework for C++
*
*Copyright © 2000 Jim Crafton
*
*This program is free software; you can redistribute it and/or
*modify it under the terms of the GNU General Public License
*as published by the Free Software Foundation; either version 2
*of the License, or (at your option) any later version.
*
*This program is distributed in the hope that it will be useful,
*but WITHOUT ANY WARRANTY; without even the implied warranty of
*MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*GNU General Public License for more details.
*
*You should have received a copy of the GNU General Public License
*along with this program; if not, write to the Free Software
*Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*
*
*Contact Jim Crafton at ddiego@one.net for more information about the 
*Visual Control Framework
*/ 

//AbstractContainer.cpp

#include "ApplicationKit.h"

#include <algorithm>

using namespace VCF;


AbstractContainer::ContainerResizeHandler::ContainerResizeHandler( AbstractContainer* container )
{
	this->m_container = container;
} 

void AbstractContainer::ContainerResizeHandler::containerResized( ControlEvent* event )
{
	this->m_container->resizeChildren();
}

/**
*Translate points to screen coords
*Translate points back to client coords
*for each child, pass the event to it's process mouse event
*/
void AbstractContainer::ContainerMouseEventHandler::onMouseEvent( MouseEvent* event )
{
	Point tmp(0.0,0.0);
	tmp = *event->getPoint();	
	
	Enumerator<Control*>* children = m_container->getEnumerator();
	children->reset( true );
	Rect* childBounds = NULL;

	Control* capturedControl = Control::getCapturedMouseControl();
	if ( NULL != capturedControl ){
		Control* container = m_container->getContainer();
		
		Control* parent = capturedControl->getParent();
		if ( parent == container ){ //then the captured control is a child of this container so keep sedning it the messages
			childBounds = capturedControl->getBounds();
			//if ( true == childBounds->containsPt( &tmp ) ){
				capturedControl->translateToLocal( &tmp );				
				MouseEvent localEvent( capturedControl, event->getType(), event->getButtonMask(), event->getKeyMask(), &tmp );
				capturedControl->processMouseEvent( &localEvent );
				event->setConsumed(true);//localEvent.isConsumed());
			//}
		}
	}
	else {
		while ( true == children->hasMoreElements(true) ){
			Control* child = children->prevElement();
			if ( NULL != child ){
				childBounds = child->getBounds();
				if ( (true == childBounds->containsPt( &tmp )) && (child->isLightWeight()) ){
					child->translateToLocal( &tmp );				
					MouseEvent localEvent( child, event->getType(), event->getButtonMask(), event->getKeyMask(), &tmp );
					child->processMouseEvent( &localEvent );
					event->setConsumed(true);//localEvent.isConsumed());
					break;
				}
			}
		}
	}
}


AbstractContainer::AbstractContainer()
{		
	this->init();
}
	
AbstractContainer::~AbstractContainer()
{
	if ( NULL != m_container ){
		m_container->removeControlSizedHandler( m_controlHandler );

		m_container->removeMouseDoubleClickedHandler( m_mouseHandler );
		m_container->removeMouseClickedHandler( m_mouseHandler );
		m_container->removeMouseMoveHandler( m_mouseHandler );
		m_container->removeMouseUpHandler( m_mouseHandler );
		m_container->removeMouseDownHandler( m_mouseHandler );
	}

	delete m_controlHandler;
	m_controlHandler = NULL;

	delete m_mouseHandler;
	m_mouseHandler = NULL;
	
	std::vector<Control*>::iterator it = this->m_controls.begin();
	while ( it != m_controls.end() ){
		Control* control = *it;
		delete control;
		control = NULL;
		it++;
	}
	m_controls.clear();	

	delete m_constainerResizeEventHandler;
	m_constainerResizeEventHandler = NULL;

	delete m_containerMouseEventHandler;
	m_containerMouseEventHandler = NULL;
}

void AbstractContainer::setContainer( Control* container )
{
	if ( NULL != m_container ){
		m_container->removeControlSizedHandler( m_controlHandler );

		m_container->removeMouseDoubleClickedHandler( m_mouseHandler );
		m_container->removeMouseClickedHandler( m_mouseHandler );
		m_container->removeMouseMoveHandler( m_mouseHandler );
		m_container->removeMouseUpHandler( m_mouseHandler );
		m_container->removeMouseDownHandler( m_mouseHandler );
	}
	this->m_container = container;	
	m_container->addControlSizedHandler( m_controlHandler );
	
	m_container->addMouseDoubleClickedHandler( m_mouseHandler );
	m_container->addMouseClickedHandler( m_mouseHandler );
	m_container->addMouseMoveHandler( m_mouseHandler );
	m_container->addMouseUpHandler( m_mouseHandler );
	m_container->addMouseDownHandler( m_mouseHandler );
}

Control* AbstractContainer::getContainer()
{
	return m_container;
}

void AbstractContainer::init()
{
	m_constainerResizeEventHandler = new ContainerResizeHandler(this);

	m_containerMouseEventHandler = new ContainerMouseEventHandler(this);

	//this is cleaned up by us
	m_controlHandler = 
		new ControlEventHandler<ContainerResizeHandler>( m_constainerResizeEventHandler, 
															ContainerResizeHandler::containerResized);
	//this is cleaned up by us
	m_mouseHandler = 
		new MouseEventHandler<ContainerMouseEventHandler>( m_containerMouseEventHandler, 
															ContainerMouseEventHandler::onMouseEvent);

	this->m_container = NULL;
	this->initContainer( this->m_controls );	
}

void AbstractContainer::add( Control * child )
{
	if ( NULL == this->m_container ){
		throw InvalidPointerException(MAKE_ERROR_MSG(INVALID_POINTER_ERROR), __LINE__);
	};
	//verify we don't already have this control
	std::vector<Control*>::iterator found = std::find( m_controls.begin(), m_controls.end(), child );
	if ( found == m_controls.end() ) {
		//check for a previous parent, if found then remove it from the parent !
		Control* parent = child->getParent();
		if ( NULL != parent ) {
			Container* parentContainer = dynamic_cast<Container*>(parent );
			if ( NULL != parentContainer ) {
				parentContainer->remove( child );
			}
		}
		this->m_controls.push_back( child );
		child->setParent( this->m_container );
		this->resizeChildren();
	}
}

void AbstractContainer::add( Control * child, const AlignmentType& alignment )
{
	if ( NULL == this->m_container ){
		throw InvalidPointerException(MAKE_ERROR_MSG(INVALID_POINTER_ERROR), __LINE__);
	};
	std::vector<Control*>::iterator found = std::find( m_controls.begin(), m_controls.end(), child );
	if ( found == m_controls.end() ) {
		//check for a previous parent, if found then remove it from the parent !
		Control* parent = child->getParent();
		if ( NULL != parent ) {
			Container* parentContainer = dynamic_cast<Container*>(parent );
			if ( NULL != parentContainer ) {
				parentContainer->remove( child );
			}
		}
		this->m_controls.push_back( child );
		child->setParent( this->m_container );
		child->setAlignment( alignment );
		this->resizeChildren();
	}
}

void AbstractContainer::remove( Control* child )
{
	std::vector<Control*>::iterator found = std::find( m_controls.begin(), m_controls.end(), child );
	if ( found != m_controls.end() ){
		m_controls.erase( found );
		resizeChildren();
	}
}

bool AbstractContainer::insertBefore( Control* c1, Control* c2, const AlignmentType& alignment )
{
	bool result = false;
	Rect bounds1;
	Rect bounds2;
	if ( (NULL != c1) && (NULL != c2) ){
		bounds1 = *c1->getBounds();
		bounds2 = *c2->getBounds();

		switch ( alignment ){
			case ALIGN_TOP:{
				result = bounds1.m_top < bounds2.m_top;
			}
			break;
			
			case ALIGN_BOTTOM:{
				result = (bounds1.m_top + bounds1.getHeight()) > (bounds2.m_top + bounds2.getHeight());
			}	
			break;

			case ALIGN_LEFT:{
				result = bounds1.m_left < bounds2.m_left;
			}
			break;

			case ALIGN_RIGHT:{
				result = (bounds1.m_left + bounds1.getWidth()) > (bounds2.m_left + bounds2.getWidth());
			}
			break;
		}
	}
	return result;
}

void AbstractContainer::doPosition( Control* component, const AlignmentType& alignment, Rect* rect )
{
	Rect* componentBounds = component->getBounds();
	double w = componentBounds->getWidth();	
	double h = componentBounds->getHeight();

	switch ( alignment ){
		case ALIGN_LEFT:{
			rect->m_left += w; 	
		}
		break;

		case ALIGN_TOP:{
			rect->m_top += h;
		}
		break;

		case ALIGN_RIGHT:{
			rect->m_right -= w;
		}
		break;

		case ALIGN_BOTTOM:{
			rect->m_bottom -= h;
		}
		break;
	}

	switch ( alignment ){
		case ALIGN_LEFT:{
			componentBounds->m_left = rect->m_left - w;
			componentBounds->m_top = rect->m_top;
			componentBounds->m_right = componentBounds->m_left + w;	
			componentBounds->m_bottom = rect->m_bottom;

			component->setBounds( componentBounds );
		}
		break;

		case ALIGN_TOP:{
			componentBounds->m_left = rect->m_left;
			componentBounds->m_top = rect->m_top - h;
			componentBounds->m_right = rect->m_right;
			componentBounds->m_bottom = componentBounds->m_top + h;

			component->setBounds( componentBounds );
		}
		break;

		case ALIGN_RIGHT:{
			componentBounds->m_left = rect->m_right;
			componentBounds->m_top = rect->m_top;
			componentBounds->m_right = rect->m_right + w;
			componentBounds->m_bottom = rect->m_bottom;

			component->setBounds( componentBounds );
		}
		break;

		case ALIGN_BOTTOM:{
			componentBounds->m_left = rect->m_left;
			componentBounds->m_top = rect->m_bottom;// - h;
			componentBounds->m_right = rect->m_right;
			componentBounds->m_bottom = componentBounds->m_top + h;

			component->setBounds( componentBounds );
		}
		break;

		case ALIGN_CLIENT:{						
			component->setBounds( rect );			
		}
		break;
	}
}

void AbstractContainer::doAlign( const AlignmentType& alignment, Rect* rect )
{
	std::vector< Control* > alignmentList;
	alignmentList.clear();
	for ( int i=0; i<m_controls.size(); i++ ){
		Control* child = m_controls[i];

		if ( (child->getAlignment() == alignment) && (true == child->getVisible()) ){
			int j = 0;
			int k=0;
			while ( j < alignmentList.size() ){
				if ( false == insertBefore( child, alignmentList[j], alignment ) ){
					j++;
				}

				k++;
				if ( k == alignmentList.size() ){
					break;
				}
			}
			
			//if ( 0 == j){
			//	alignmentList.push_back( child );
			//}
			//else {
				alignmentList.insert( alignmentList.begin() + j, child );
			//}
		}		
	}

	for (int k=0;k<alignmentList.size();k++ ){
		doPosition( alignmentList[k], alignment, rect );
	}
}

bool AbstractContainer::alignWork()
{
	bool result = true;
	for ( int i=0; i<m_controls.size(); i++ ){		
		if ( m_controls[i]->getAlignment() != ALIGN_NONE ){
			return result;
		}
	}
	result = false;
	return result;
}

void AbstractContainer::doAnchors( Rect* bounds )
{	
	Rect* tmpBounds = m_container->getBounds();
	std::vector<Control*>::iterator it = m_controls.begin();
	while ( it != m_controls.end() ) {
		Control* child = *it;
		if ( ANCHOR_NONE != child->getAnchor() ) {
			long anchorType = child->getAnchor();

			Rect anchorBounds = *child->getBounds();
			double h = anchorBounds.getHeight();
			double w = anchorBounds.getWidth();
			
			float* anchorDeltas = child->getAnchorDeltas();				
			
			if ( (anchorType & ANCHOR_RIGHT) != 0 ) {
				anchorBounds.m_right = __max( anchorBounds.m_left, tmpBounds->m_right - anchorDeltas[Control::ANCHOR_DRIGHT] );
				anchorBounds.m_left = anchorBounds.m_right - w;
			}
			
			if ( (anchorType & ANCHOR_LEFT) != 0 ) {
				anchorBounds.m_left = anchorDeltas[Control::ANCHOR_DLEFT];
			}
			
			if ( (anchorType & ANCHOR_BOTTOM) != 0 ) {
				anchorBounds.m_bottom = __max( anchorBounds.m_top, tmpBounds->m_bottom - anchorDeltas[Control::ANCHOR_DBOTTOM] );
				anchorBounds.m_top = anchorBounds.m_bottom - h;
			}
			
			if ( (anchorType & ANCHOR_TOP) != 0 ) {
				anchorBounds.m_top = anchorDeltas[Control::ANCHOR_DTOP];
			}
			
			
			child->setBounds( &anchorBounds, false );
		}
		it ++;
	}
}

void AbstractContainer::resizeChildren()
{	
	Rect* bounds = this->m_container->getBounds();
	Rect rect( 0.0, 0.0, bounds->getWidth(), bounds->getHeight() );

	resizeChildrenUsingBounds( &rect );	
}

void AbstractContainer::resizeChildrenUsingBounds( Rect* bounds )
{
	if ( NULL == this->m_container ){
		throw InvalidPointerException(MAKE_ERROR_MSG(INVALID_POINTER_ERROR), __LINE__);
	};

	if ( true == alignWork() ){
		
		this->doAlign( ALIGN_TOP, bounds );
		this->doAlign( ALIGN_BOTTOM, bounds );
		this->doAlign( ALIGN_LEFT, bounds );
		this->doAlign( ALIGN_RIGHT, bounds );
		this->doAlign( ALIGN_CLIENT, bounds );
	}

	doAnchors( bounds );
}


void AbstractContainer::paintChildren( GraphicsContext* context ){	

	Enumerator<Control*>* children = getEnumerator();
	if ( NULL == this->m_container ){
		throw InvalidPointerException(MAKE_ERROR_MSG(INVALID_POINTER_ERROR), __LINE__);
	};
	double left = m_container->getLeft();
	double top = m_container->getTop();
	double originX = 0.0;
	double originY = 0.0;
	while ( true == children->hasMoreElements() ){
		Control* child = children->nextElement(); 
		if ( NULL != child ){
			if ( true == child->isLightWeight() ){
				Rect* bounds = child->getBounds();
				Point* origin = context->getOrigin();
				originX = bounds->m_left + origin->m_x;
				originY = bounds->m_top + origin->m_y;
				context->setOrigin( originX, originY );
				
				child->paint( context );
				
				context->setOrigin( origin->m_x, origin->m_y );
			}
		}
	}	
}

Enumerator<Control*>* AbstractContainer::getChildren()
{
	return this->getEnumerator();
}

unsigned long AbstractContainer::getChildCount()
{
	return 	m_controls.size();
}


Control* AbstractContainer::findControl( const String& controlName )
{
	Control* result = NULL;
	//this is very slow !! for the moment !
	std::vector<Control*>::iterator it = this->m_controls.begin();
	bool notFound = true;
	while ( it != m_controls.end() ){
		Control* child = *it;
		if ( controlName == child->getName() ){
			result = child;
			break;
		}
		it ++;
	}

	return result;
}
