//Splitter.h
#include "ApplicationKit.h"
#include "Splitter.h"


using namespace VCF;
Splitter::Splitter():
	CustomControl(true)
{
	init();
}

void Splitter::init()
{
	this->m_attachedControl = NULL;	
	this->setCursorID( (long)Cursor::SCT_SPLIT_VERT );
	this->setAlignment( ALIGN_LEFT );
	m_dragPoint.m_x = 0.0;
	m_dragPoint.m_y = 0.0;
	m_delta = 0.0;
	m_updateAttachedControl = false;
	this->setWidth( 5 );
	this->setHeight( 5 );	
}

Splitter::~Splitter()
{

}

void Splitter::mouseDown( MouseEvent* e )
{
	CustomControl::mouseDown( e );
	keepMouseEvents();
	m_dragPoint = *(e->getPoint());
	Point tmp = m_dragPoint;
	switch ( this->getAlignment() ) {
		case ALIGN_LEFT : {
			tmp.m_x = this->getLeft() - 1.0;
		}
		break;

		case ALIGN_TOP : {
			tmp.m_y = this->getTop() - 1.0;
		}
		break;

		case ALIGN_RIGHT : {
			tmp.m_x = this->getLeft() + this->getWidth() + 1.0;
		}
		break;

		case ALIGN_BOTTOM : {
			tmp.m_y = this->getTop() + this->getHeight() + 1.0;
		}
		break;
	}
	this->m_attachedControl = NULL;
 	Container* container = dynamic_cast<Container*>(this->getParent() );
	if ( NULL != container ) {
		Enumerator<Control*>* children = container->getChildren();
		while ( children->hasMoreElements() ) {
			Control* child = children->nextElement();
			if ( true == (child->getBounds()->containsPt( &tmp )) ) {
				this->m_attachedControl = child;
				break;
			}
		}

	}
}

void Splitter::updateAttachedControl( Point* pt )
{
	switch ( this->getAlignment() ) {
		case ALIGN_LEFT : {			
			m_delta = pt->m_x - m_dragPoint.m_x;
			if ( NULL != m_attachedControl ) {
				m_delta = __max( 5.0, m_attachedControl->getWidth() + m_delta );
				m_attachedControl->setWidth( m_delta );
			}
		}
		break;

		case ALIGN_TOP : {
			m_delta = pt->m_y - m_dragPoint.m_y;
			if ( NULL != m_attachedControl ) {
				m_delta = __max( 5.0, m_attachedControl->getHeight() + m_delta );
				m_attachedControl->setHeight( m_delta );
			}
		}
		break;

		case ALIGN_RIGHT : {
			m_delta = m_dragPoint.m_x - pt->m_x;
			if ( NULL != m_attachedControl ) {
				m_delta = __max( 5.0, m_attachedControl->getWidth() + m_delta );
				m_attachedControl->setWidth( m_delta );
			}
		}
		break;

		case ALIGN_BOTTOM : {
			m_delta = m_dragPoint.m_y - pt->m_y;
			if ( NULL != m_attachedControl ) {
				m_delta = __max( 5.0, m_attachedControl->getHeight() + m_delta );
				m_attachedControl->setHeight( m_delta );
			}
		}
		break;
	
	}

	Container* container = dynamic_cast<Container*>(this->getParent() );
	if ( NULL != container ) {
		container->resizeChildren();
	}
}

void Splitter::mouseMove( MouseEvent* e )
{
	CustomControl::mouseMove( e );
	if ( e->hasLeftButton() ) {		
		updateAttachedControl( e->getPoint() );		
	}
}

void Splitter::mouseUp( MouseEvent* e )
{
	CustomControl::mouseUp( e );
	this->releaseMouseEvents();

	updateAttachedControl( e->getPoint() );	
}

void Splitter::paint( GraphicsContext* ctx )
{
	CustomControl::paint( ctx );
	Rect r( 0, 0, getWidth(), getHeight() );
	this->m_bdr.paint( &r, ctx );
}