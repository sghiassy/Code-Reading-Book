//SingleSelectionGripper.h
#include "ApplicationKit.h"
#include "SingleSelectionGripper.h"
#include "SelectionManager.h"
#include "SelectionGripperHandle.h"
#include "Desktop.h"
#include "VCFBuilderUI.h"
#include "TranslateControlCmd.h"

using namespace VCF;

using namespace VCFBuilder;

#define HANDLEGRIPPER_MOUSEDOWN_HANDLER			"HandleGripperMouseDownHandler"
#define HANDLEGRIPPER_MOUSEUP_HANDLER			"HandleGripperMouseUpHandler"
#define HANDLEGRIPPER_MOUSEMOVE_HANDLER			"HandleGripperMouseMoveHandler"

SingleSelectionGripper::SingleSelectionGripper()
{
	MouseEventHandler<SingleSelectionGripper>* mh = 
		new MouseEventHandler<SingleSelectionGripper>( this,SingleSelectionGripper::onHandleMouseDown,HANDLEGRIPPER_MOUSEDOWN_HANDLER );

	mh = 
		new MouseEventHandler<SingleSelectionGripper>( this,SingleSelectionGripper::onHandleMouseMove,HANDLEGRIPPER_MOUSEMOVE_HANDLER );

	mh = 
		new MouseEventHandler<SingleSelectionGripper>( this,SingleSelectionGripper::onHandleMouseUp,HANDLEGRIPPER_MOUSEUP_HANDLER );	

	m_selectedControl = NULL;
}

SingleSelectionGripper::~SingleSelectionGripper()
{
	clearSelectionGripper();
}

void SingleSelectionGripper::select()
{
	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	if ( 1 == selectionMgr->getSelectionSetCount() ) {
		Component* comp = selectionMgr->getSelectedItemAt(0);
		m_selectedControl = dynamic_cast<Control*>(comp);
		if ( NULL != m_selectedControl ) {
			Frame* frame = dynamic_cast<Frame*>(m_selectedControl);
			if ( NULL == frame ) {
				Control* parent = m_selectedControl->getHeavyweightParent();
				Container* container = dynamic_cast<Container*>(parent);
				if ( NULL != container ) {
					Rect* bounds = m_selectedControl->getBounds();
					Desktop* desktop = Desktop::getDesktop();
					Point pt;
					EventHandler* mouseDownHandler = getEventHandler( HANDLEGRIPPER_MOUSEDOWN_HANDLER );
					EventHandler* mouseUpHandler = getEventHandler( HANDLEGRIPPER_MOUSEUP_HANDLER );
					EventHandler* mouseMoveHandler = getEventHandler( HANDLEGRIPPER_MOUSEMOVE_HANDLER );
					if ( true == m_gripperHandles.empty() ) {
						for ( int i=0;i<8;i++) {
							SelectionGripperHandle* handle = new SelectionGripperHandle();
							m_gripperHandles.push_back( handle );
							handle->setTag( i );
							if ( NULL != mouseDownHandler ) {
								handle->addMouseDownHandler( mouseDownHandler );
							}
							if ( NULL != mouseUpHandler ) {
								handle->addMouseUpHandler( mouseUpHandler );
							}
							if ( NULL != mouseMoveHandler ) {
								handle->addMouseMoveHandler( mouseMoveHandler );
							}
						}
					}
					
					for ( int i=0;i<8;i++) {	
						SelectionGripperHandle* handle = m_gripperHandles[i];
						container->add( handle );
						handle->setVisible( true );
						setHandlePosition( handle, bounds );						
					}
				}
			}			
			else {
				if ( false == m_gripperHandles.empty() ) {
					for ( int i=0;i<8;i++) {	
						SelectionGripperHandle* handle = m_gripperHandles[i];
						handle->setVisible( false );
					}
				}
			}
		}
	}
}

void SingleSelectionGripper::onHandleMouseDown( MouseEvent* e )
{
	if ( NULL != m_selectedControl ) {
		if ( e->hasLeftButton() ) {
			m_dragPt = *(e->getPoint());
			m_dragRect = *(m_selectedControl->getBounds());
			Control* control = (Control*)e->getSource();
			control->keepMouseEvents();
			Desktop::getDesktop()->beginPainting();
			this->dragSelectionRect( control, &m_dragRect );
		}
	}
}

void SingleSelectionGripper::onHandleMouseMove( MouseEvent* e )
{
	if ( NULL != m_selectedControl ) {
		if ( e->hasLeftButton() ) {						
			Control* control = (Control*)e->getSource();

			this->dragSelectionRect( control, &m_dragRect );

			adjustSelectionRect( control->getTag(), e->getPoint() );

			this->dragSelectionRect( control, &m_dragRect );
		}
	}
}

void SingleSelectionGripper::onHandleMouseUp( MouseEvent* e )
{
	if ( NULL != m_selectedControl ) {
		if ( e->hasLeftButton() ) {
			Control* control = (Control*)e->getSource();

			this->dragSelectionRect( control, &m_dragRect );

			adjustSelectionRect( control->getTag(), e->getPoint() );

			this->dragSelectionRect( control, &m_dragRect );
			this->dragSelectionRect( control, &m_dragRect );
			Desktop::getDesktop()->endPainting();
			control->releaseMouseEvents();		
			
			SizeControlCmd* sizeCmd = 
				new SizeControlCmd( m_selectedControl->getName(), m_selectedControl->getBounds(), &m_dragRect ); 
			UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
			cmdStack->addCommand( sizeCmd );

			for ( int i=0;i<8;i++) {
				setHandlePosition( m_gripperHandles[i], m_selectedControl->getBounds() );
			}
		}
	}
}

void SingleSelectionGripper::dragSelectionRect( Control* control, Rect* rect )
{
	GraphicsContext* ctx = Desktop::getDesktop()->getContext();
	Rect tmp = *rect;
	Control* parent = this->m_selectedControl->getHeavyweightParent();
	Desktop::getDesktop()->translateToScreenCoords( parent, &tmp );
	ctx->setXORModeOn( true );	
	ctx->rectangle( &tmp );
	ctx->strokePath();	
	
	ctx->setXORModeOn( false );	
}

void SingleSelectionGripper::adjustSelectionRect( const long& tagID, Point* pt )
{
	switch( (HandleType)tagID ) {
		case HT_NW_HANDLE : {
			double deltax = pt->m_x - m_dragPt.m_x;
			double deltay = pt->m_y - m_dragPt.m_y;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_left = bounds->m_left + deltax;
			m_dragRect.m_top = bounds->m_top + deltay;
		}
		break;

		case HT_N_HANDLE : {					
			double deltay = pt->m_y - m_dragPt.m_y;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_top = bounds->m_top + deltay;
		}
		break;

		case HT_NE_HANDLE : {
			double deltax = pt->m_x - m_dragPt.m_x;
			double deltay = pt->m_y - m_dragPt.m_y;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_right = bounds->m_right + deltax;
			m_dragRect.m_top = bounds->m_top + deltay;
		}
		break;

		case HT_E_HANDLE : {
			double deltax = pt->m_x - m_dragPt.m_x;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_right = bounds->m_right + deltax;
		}
		break;

		case HT_SE_HANDLE : {
			double deltax = pt->m_x - m_dragPt.m_x;
			double deltay = pt->m_y - m_dragPt.m_y;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_right = bounds->m_right + deltax;
			m_dragRect.m_bottom = bounds->m_bottom + deltay;
		}
		break;

		case HT_S_HANDLE : {
			double deltay = pt->m_y - m_dragPt.m_y;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_bottom = bounds->m_bottom + deltay;
		}
		break;

		case HT_SW_HANDLE : {
			double deltax = pt->m_x - m_dragPt.m_x;
			double deltay = pt->m_y - m_dragPt.m_y;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_left = bounds->m_left + deltax;
			m_dragRect.m_bottom = bounds->m_bottom + deltay;
		}
		break;

		case HT_W_HANDLE : {
			double deltax = pt->m_x - m_dragPt.m_x;
			Rect* bounds = m_selectedControl->getBounds();
			m_dragRect.m_left = bounds->m_left + deltax;
		}
		break;
	}
}

void SingleSelectionGripper::setHandlePosition( SelectionGripperHandle* handle, Rect* bounds )
{
	Point pt;
	switch ( handle->getTag() )  {
		case 0 : {
			pt.m_x = bounds->m_left - handle->getWidth()/2.0;
			pt.m_y = bounds->m_top - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_NW_SE );
		}
		break;

		case 1 : {
			pt.m_x = (bounds->m_left + bounds->getWidth()/2.0)- handle->getWidth()/2.0;
			pt.m_y = bounds->m_top - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_VERT );
		}
		break;

		case 2 : {
			pt.m_x = (bounds->m_left + bounds->getWidth())- handle->getWidth()/2.0;
			pt.m_y = bounds->m_top - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_NE_SW );
		}
		break;

		case 3 : {
			pt.m_x = (bounds->m_left + bounds->getWidth())- handle->getWidth()/2.0;
			pt.m_y = (bounds->m_top + bounds->getHeight()/2.0) - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_HORZ );
		}
		break;

		case 4 : {
			pt.m_x = (bounds->m_left + bounds->getWidth())- handle->getWidth()/2.0;
			pt.m_y = (bounds->m_top + bounds->getHeight()) - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_NW_SE );
		}
		break;

		case 5 : {
			pt.m_x = (bounds->m_left + bounds->getWidth()/2.0)- handle->getWidth()/2.0;
			pt.m_y = (bounds->m_top + bounds->getHeight()) - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_VERT );
		}
		break;

		case 6 : {
			pt.m_x = bounds->m_left- handle->getWidth()/2.0;
			pt.m_y = (bounds->m_top + bounds->getHeight()) - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_NE_SW );
		}
		break;

		case 7 : {
			pt.m_x = bounds->m_left- handle->getWidth()/2.0;
			pt.m_y = (bounds->m_top + bounds->getHeight()/2.0) - handle->getHeight()/2.0;
			handle->setCursorID( Cursor::SCT_SIZE_HORZ );
		}
		break;
	}
	
	handle->setLeft( pt.m_x );
	handle->setTop( pt.m_y );		
}