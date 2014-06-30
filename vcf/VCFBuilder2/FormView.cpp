//FormView.h
#include "ApplicationKit.h"
#include "SingleSelectionGripper.h"
#include "FormView.h"

#include "DefaultListItem.h"
#include "FormGrid.h"
#include "ComponentNameManager.h"

#include "GridRuler.h"
#include "GridRulerBtn.h"
#include "Panel.h"

#include "MainUIPanel.h"
#include "ControlListener.h"
#include "PopupMenu.h"
#include "DefaultMenuItem.h"

#include "VFFOutputStream.h"
#include "TextOutputStream.h"	
#include "VFFInputStream.h"
#include "BasicInputStream.h"	

#include "VCFBuilderUI.h"

#include "Project.h"
#include "Form.h"

#include "NewComponentCmd.h"
#include "ComponentInstance.h"

#include "TranslateControlCmd.h"
#include "RemoveComponentCmd.h"

#include "SelectionManager.h"

#include "ComponentEditor.h"
#include "ComponentEditorManager.h"


#define COMPONENT_EDITOR_CMD_HANDLER		"componentEditorCmdHandler"

using namespace VCF;

using namespace VCFBuilder;


FormView::FormView()
{
	m_toolMode = FTM_SELECT;
	m_selectionToolMode = STM_DRAW_SELECTION_RECT;
	setShowBorder( false );
	m_activeFormInst = NULL;

	WindowEventHandler<FormView>* wh = 
		new WindowEventHandler<FormView>( this, FormView::onActiveFormClose, FORMVIEW_WINDOW_HANDLER );	
	
	m_canBeginDraggingSelectionRect = false;

	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	SelectionManagerEventHandler<FormView>* smh = 
		new SelectionManagerEventHandler<FormView>( this, FormView::onSelectionManagerChanged, "SelectionManagerHandler" );
	
	selectionMgr->addSelectionChangedHandler( smh );
	

	MouseEventHandler<FormView>* mh = 
		new MouseEventHandler<FormView>( this, FormView::onControlMouseDown, SELECTED_CONTROL_MOUSEDOWNHANDLER );
	
	mh = 
		new MouseEventHandler<FormView>( this, FormView::onControlMouseMove, SELECTED_CONTROL_MOUSEMOVEHANDLER );

	mh = 
		new MouseEventHandler<FormView>( this, FormView::onControlMouseUp, SELECTED_CONTROL_MOUSEUPHANDLER );
	
	KeyboardEventHandler<FormView>* kh = 
		new KeyboardEventHandler<FormView>( this, FormView::onControlKeyPressed, SELECTED_CONTROL_KEYPRESSEDHANDLER );
	
	//
	ControlEventHandler<FormView>* ch = 
		new ControlEventHandler<FormView>( this, FormView::onControlPositioned, SELECTED_CONTROL_POSITIONEDHANDLER );
	

	m_formComponentsView = new ListViewControl();
	m_formComponentsView->setHeight( 65 );	
	m_formComponentsView->setVisible( true );
	add( m_formComponentsView, ALIGN_BOTTOM );
	mh = new MouseEventHandler<FormView>( this, FormView::onComponentsViewClick, "ComponentViewMouseHandler" );
	
	m_formComponentsView->addMouseClickedHandler( mh );

	m_verticalRuler = new GridRuler();
	m_verticalRuler->setHorizontalRuler( false );
	m_verticalRuler->setWidth( 25 );
	add( m_verticalRuler, ALIGN_LEFT );

	Panel* topHolder = new Panel();
	add( topHolder, ALIGN_TOP );
	topHolder->setShowBorder( false );
	topHolder->setHeight( 25 );
	
	m_gridRulerBtn = new GridRulerBtn();
	m_gridRulerBtn->setWidth( 25 );
	topHolder->add( m_gridRulerBtn, ALIGN_LEFT );

	m_horizontalRuler = new GridRuler();
	m_horizontalRuler->setWidth( 25 );
	topHolder->add( m_horizontalRuler, ALIGN_CLIENT );

	m_formGrid = new FormGrid();
	m_formGrid->setShowBorder( false );
	add( m_formGrid, ALIGN_CLIENT );

	m_formAsTextView = new MultilineTextControl();
	m_formAsTextView->setVisible( false );
	m_formAsTextView->getFont()->setName( "Courier New" );
	add( m_formAsTextView, ALIGN_CLIENT );

	PopupMenu* gridMenu = new PopupMenu();
	DefaultMenuItem* root = new DefaultMenuItem( "root", NULL, gridMenu );
	gridMenu->setRootMenuItem( root );
	DefaultMenuItem* showVFFItem = new DefaultMenuItem( "Show As VFF", root, gridMenu ); 
	addComponent( gridMenu );

	MenuItemEventHandler<FormView>* mih = 
		new MenuItemEventHandler<FormView>( this, FormView::onShowFormAsVFF, "gridMenuMenuItemClickedHandler" );

	showVFFItem->addMenuItemClickedHandler( mih );
	
	mih = new MenuItemEventHandler<FormView>( this, FormView::onUpdateShowFormAsVFF, "gridMenuMenuItemUpdatedHandler" );
	showVFFItem->addMenuItemUpdateHandler( mih );

	m_formGrid->setPopupMenu( gridMenu );
	m_formAsTextView->setPopupMenu( gridMenu );

	m_componentClass = NULL;
	m_activeForm = NULL;	

	//FORMVIEW_PROJECT_HANDLER	
	ProjectEventHandler<FormView>* ph = new ProjectEventHandler<FormView>( this, FormView::onProjectChanged, FORMVIEW_PROJECT_HANDLER );	


	EventHandler* cmdHandler = 
				new MenuItemEventHandler<FormView>(this,FormView::onComponentEditorCommandClicked,COMPONENT_EDITOR_CMD_HANDLER);	
}

FormView::~FormView()
{

}

void FormView::paint( GraphicsContext* ctx )
{
	Panel::paint( ctx );
}


void FormView::mouseDown( MouseEvent* e )
{
	Panel::mouseDown( e );
}

void FormView::onActiveFormClose( WindowEvent* event )
{
	
}

void FormView::onControlMouseDown( MouseEvent* e )
{
	m_startDragPt.m_x = 0.0;
	m_startDragPt.m_y = 0.0;
	m_selectionRect.setRect( 0.0, 0.0, 0.0, 0.0 );
	m_selectionToolMode = STM_NONE;
	if ( e->hasLeftButton() ) {
		switch ( m_toolMode ) {
			case FTM_SELECT : {
				SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
				m_startDragPt = *(e->getPoint());
				m_selectionRect.setRect( m_startDragPt.m_x, m_startDragPt.m_y,
										 m_startDragPt.m_x, m_startDragPt.m_y );

				Control* control = (Control*)e->getSource();

				if ( 1 == selectionMgr->getSelectionSetCount() ) {
					Control* selectedControl = dynamic_cast<Control*>(selectionMgr->getSelectedItemAt(0));
					if ( (control == selectedControl) && (control != this->m_activeForm) ) {
						m_selectionRect = *(selectedControl->getBounds());
						
						Desktop::getDesktop()->translateToScreenCoords( (Control*)e->getSource(), &m_startDragPt );
						Desktop::getDesktop()->translateToScreenCoords( selectedControl->getHeavyweightParent(), &m_selectionRect );
						
						Rect tmpBounds = *(getBounds());
						Desktop::getDesktop()->translateToScreenCoords( getParent(), &tmpBounds );
						Desktop::getDesktop()->beginPainting( &tmpBounds );
						control->keepMouseEvents();
						m_selectionToolMode = STM_TRANSLATE_CONTROL;
					}
					else  {
						selectionMgr->clearSelectionSet();					
						selectionMgr->addToSelectionSet( control );
						if ( this->m_activeForm == control ) {
							Desktop::getDesktop()->translateToScreenCoords( (Control*)e->getSource(), &m_startDragPt );
							Desktop::getDesktop()->translateToScreenCoords( (Control*)e->getSource(), &m_selectionRect );
							Rect tmpBounds = *(getBounds());
							Desktop::getDesktop()->translateToScreenCoords( getParent(), &tmpBounds );
							Desktop::getDesktop()->beginPainting( &tmpBounds );
							
							m_selectionToolMode = STM_DRAW_SELECTION_RECT;
							
							control->keepMouseEvents();
						}
					}
				}
				else if ( selectionMgr->getSelectionSetCount() > 1 ) {
					
				}
			}
			break;		

			case FTM_NEW_COMPONENT : {
				m_startDragPt = *(e->getPoint());
				m_selectionRect.setRect( m_startDragPt.m_x, m_startDragPt.m_y,
										 m_startDragPt.m_x, m_startDragPt.m_y );  
				Control* control = (Control*)e->getSource();
				Container* container = dynamic_cast<Container*>(control);
				if ( NULL != container ) {
					Desktop::getDesktop()->translateToScreenCoords( (Control*)e->getSource(), &m_startDragPt );
					Desktop::getDesktop()->translateToScreenCoords( (Control*)e->getSource(), &m_selectionRect );
					Rect tmpBounds = *(getBounds());
					Desktop::getDesktop()->translateToScreenCoords( getParent(), &tmpBounds );
					Desktop::getDesktop()->beginPainting( &tmpBounds );
					
					m_canBeginDraggingSelectionRect = true;
					
					control->keepMouseEvents();
				}
			}
			break;	
		}
	}
}

void FormView::onControlMouseMove( MouseEvent* e )
{
	if ( e->hasLeftButton() ) {
		Point tmp = *(e->getPoint());
		switch ( m_toolMode ) {
			case FTM_SELECT :  {
				if ( STM_DRAW_SELECTION_RECT == m_selectionToolMode ) {
					dragSelectionRect( (Control*)e->getSource(), e->getPoint() );
				}
				else if ( STM_TRANSLATE_CONTROL == m_selectionToolMode ){
					translateSelectionRect( (Control*)e->getSource(), e->getPoint() );
				}
			}
			break;

			case FTM_NEW_COMPONENT: { 
				if ( true == m_canBeginDraggingSelectionRect ) {
					dragSelectionRect( (Control*)e->getSource(), e->getPoint() );
				}
			}
			break;
		}
	}
}

void FormView::onComponentEditorCommandClicked( MenuItemEvent* e )
{
 	MenuItem* menuItem = (MenuItem*)e->getSource();
	Command* command = (Command*)menuItem->getData();
	Object* commandObject = dynamic_cast<Object*>( command );
	if ( NULL != commandObject ) {
		commandObject->addRef();

		UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
		cmdStack->addCommand( command );	
	}
}

void FormView::onControlMouseUp( MouseEvent* e )
{	
	if ( e->hasLeftButton() ) {
		switch ( m_toolMode ) {
			case FTM_SELECT : {
				Control* control = (Control*)e->getSource();
				if ( STM_DRAW_SELECTION_RECT == m_selectionToolMode ) {					
					dragSelectionRect( (Control*)e->getSource(), e->getPoint() );
					dragSelectionRect( NULL, NULL );
					Desktop::getDesktop()->endPainting();					
					control->releaseMouseEvents();
				}
				else if ( STM_TRANSLATE_CONTROL == m_selectionToolMode ) {
					translateSelectionRect( (Control*)e->getSource(), e->getPoint() );
					translateSelectionRect( NULL, NULL );
					Desktop::getDesktop()->endPainting();
					control->releaseMouseEvents();
					Desktop::getDesktop()->translateFromScreenCoords( control->getHeavyweightParent(), &m_selectionRect );

					
					TranslateControlCmd* translateCmd = new TranslateControlCmd( control->getName(), control->getBounds(), &m_selectionRect ); 
					UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
					cmdStack->addCommand( translateCmd );
					
				}
				if ( SelectionManager::getSelectionMgr()->getSelectionSetCount() > 1 ) {

				}
				else {
					this->m_singleSelectionGripper.select();
				}
			}
			break;

			case FTM_NEW_COMPONENT : {
				if ( true == m_canBeginDraggingSelectionRect ) {
					
					dragSelectionRect( (Control*)e->getSource(), e->getPoint() );
					dragSelectionRect( NULL, NULL );
					Desktop::getDesktop()->endPainting();
					Control* control = (Control*)e->getSource();
					control->releaseMouseEvents();
				}
				addNewComponent( m_componentClass );				
			}
			break;
		}
		m_componentClass = NULL;
	}
	else if ( e->hasRightButton() ) {
		Control* control = (Control*)e->getSource();
		String className = control->getClassName();
		ComponentEditor* componentEditor = ComponentEditorManager::findComponentEditor( className );
		if ( NULL == componentEditor ) {
			componentEditor = ComponentEditorManager::findComponentEditor( REGISTER_FOR_ALL_COMPONENTS );	
		}

		if ( NULL != componentEditor ) {
			std::vector<Command*> tmpCommandList;
			componentEditor->setComponent( control );

			ulong32 cmdCount = componentEditor->getCommandCount();
			PopupMenu* componentEditorCtxMenu = new PopupMenu();
			DefaultMenuItem* root = new DefaultMenuItem( "root", NULL, componentEditorCtxMenu );
			componentEditorCtxMenu->setRootMenuItem( root );			
			
			EventHandler* cmdHandler = this->getEventHandler( COMPONENT_EDITOR_CMD_HANDLER );
			

			for ( ulong32 i=0;i<cmdCount;i++ ) {				

				Command* cmd = componentEditor->getCommand( i );
				if ( NULL != cmd ) {
					Object* commandObj = dynamic_cast<Object*>(cmd);
					if ( NULL != commandObj ) {
						commandObj->addRef();
					}
					tmpCommandList.push_back( cmd );
					
					DefaultMenuItem* cmdMenuItem = new DefaultMenuItem( cmd->getName(), root, componentEditorCtxMenu ); 	
					cmdMenuItem->setData( (void*)cmd );
					cmdMenuItem->addMenuItemClickedHandler( cmdHandler );
				}
			}
			componentEditorCtxMenu->setControl( control );
			componentEditorCtxMenu->popup( e->getPoint() );
			//AHA Our first use of refcounting !!
			//we'll go through our tmp list of commands and release() all of 
			//them. The call back method for whatever was clicked on will
			//addRef() that command
			std::vector<Command*>::iterator it = tmpCommandList.begin();
			while ( it != tmpCommandList.end() ) {
				Object* cmd = dynamic_cast<Object*>(*it);
				if ( NULL != cmd ) {
					cmd->release();
				}
				it ++;
			}

			
			delete componentEditorCtxMenu;
			componentEditorCtxMenu = NULL;
		}
	}
	m_canBeginDraggingSelectionRect = false;	
}

void FormView::translateSelectionRect( Control* control, Point* pt )
{
	GraphicsContext* ctx = Desktop::getDesktop()->getContext();
	ctx->setXORModeOn( true );	
	ctx->rectangle( &m_selectionRect );
	ctx->strokePath();

	if ( NULL != pt ) {
		Point tmp = *pt;
		Desktop::getDesktop()->translateToScreenCoords( control, &tmp );	
		
		double deltax = tmp.m_x - m_startDragPt.m_x;
		double deltay = tmp.m_y - m_startDragPt.m_y;
		
		Rect tmpBounds = *(control->getBounds());
		Desktop::getDesktop()->translateToScreenCoords( control->getHeavyweightParent(), &tmpBounds );	
		
		m_selectionRect.m_left = tmpBounds.m_left + deltax;
		m_selectionRect.m_top = tmpBounds.m_top + deltay;
		m_selectionRect.m_right = tmpBounds.m_right + deltax;
		m_selectionRect.m_bottom = tmpBounds.m_bottom + deltay;		
		
		ctx->rectangle( &m_selectionRect );
		ctx->strokePath();
	}

	ctx->setXORModeOn( false );	
}

void FormView::dragSelectionRect( Control* control, Point* pt )
{
	GraphicsContext* ctx = Desktop::getDesktop()->getContext();
	ctx->setXORModeOn( true );	
	ctx->rectangle( &m_selectionRect );
	ctx->strokePath();
	
	if ( NULL != pt ) {
		Point tmp = *pt;
		Desktop::getDesktop()->translateToScreenCoords( control, &tmp );	
		m_selectionRect.m_right = tmp.m_x;
		m_selectionRect.m_bottom = tmp.m_y;			
		
		ctx->rectangle( &m_selectionRect );
		ctx->strokePath();
	}

	ctx->setXORModeOn( false );	
}

void FormView::addNewComponent( Class* componentClass )
{
	Project* currentProject = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
	NewComponentCmd* newComponentCmd = new NewComponentCmd( componentClass, currentProject, m_activeFormInst );
	UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
	cmdStack->addCommand( newComponentCmd );	

	this->setToolMode( FTM_SELECT );
}

void FormView::addComponentToForm( Component* componentToAdd )
{
	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	if ( NULL != m_activeForm ){
		Component* selectedComponent = NULL;
		if ( selectionMgr->getSelectionSetCount() == 1 ) { 
			selectedComponent = selectionMgr->getSelectedItemAt( 0 );
		}
		else {
			selectedComponent = m_activeForm;
		}

		if ( NULL != componentToAdd ){			
			
			Control* control = dynamic_cast<Control*>( componentToAdd );
			if ( NULL != control ){
				Rect controlBounds;
				controlBounds.m_left = m_selectionRect.m_left;
				controlBounds.m_top = m_selectionRect.m_top;
				if ( m_selectionRect.getWidth() <= 3.0 ) {
					controlBounds.m_right = controlBounds.m_left + control->getPreferredWidth();
				}	
				else {
					controlBounds.m_right = m_selectionRect.m_right;
				}
				
				if ( m_selectionRect.getHeight() <= 3.0 ) {
					controlBounds.m_bottom = controlBounds.m_top + control->getPreferredHeight();
				}	
				else {
					controlBounds.m_bottom = m_selectionRect.m_bottom;
				}				
				
				Container* cont = dynamic_cast<Container*>(selectedComponent);
				
				if ( NULL != cont ){
					Control* parent = (Control*)selectedComponent;
					Desktop::getDesktop()->translateFromScreenCoords( parent, &controlBounds );
					control->setBounds( &controlBounds );
					cont->add( control );
				}
				else if ( NULL != m_activeForm ){
					Desktop::getDesktop()->translateFromScreenCoords( m_activeForm, &controlBounds );
					control->setBounds( &controlBounds );
					m_activeForm->add( control );
				}
				EventHandler* eventHandler = getEventHandler( SELECTED_CONTROL_MOUSEDOWNHANDLER );
				control->addMouseDownHandler( eventHandler );
				eventHandler = getEventHandler( SELECTED_CONTROL_MOUSEUPHANDLER );
				control->addMouseUpHandler( eventHandler );
				eventHandler = getEventHandler( SELECTED_CONTROL_MOUSEMOVEHANDLER );
				control->addMouseMoveHandler( eventHandler );
				
				eventHandler = getEventHandler( SELECTED_CONTROL_KEYPRESSEDHANDLER );
				control->addKeyPressedHandler( eventHandler );

				eventHandler = getEventHandler( SELECTED_CONTROL_POSITIONEDHANDLER );
				control->addControlPositionedHandler( eventHandler );
				control->addControlSizedHandler( eventHandler );				
				
				control->setVisible( true );
				control->setFocus( true );
				selectionMgr->clearSelectionSet();
				selectionMgr->addToSelectionSet( control );				
			}
			else {				
				Container* cont = dynamic_cast<Container*>(selectedComponent);
				if ( NULL != cont ){
					selectedComponent->addComponent( componentToAdd );
				}
				else if ( NULL != m_activeForm ){
					m_activeForm->addComponent( componentToAdd );
				}
				selectionMgr->clearSelectionSet();
				selectionMgr->addToSelectionSet( m_activeForm );				
			}
			//component has been added, save out the frame to disk
			m_activeFormInst->saveVFF();
			
			//component has been added so clear the pressed button on the 
			//Comoponent palette
			
			MainUIPanel* mainUI = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
			mainUI->releaseSelectedPaletteToolbarBtn();
			
			this->m_singleSelectionGripper.select();
		}
	}
}

void FormView::removeComponentFromForm( Component* componentToRemove )
{
	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	if ( selectionMgr->getSelectionSetCount() > 0 ) {
		selectionMgr->removeFromSelectionSet( componentToRemove );
	}
	Control* parent = NULL;
	//just controls for now
	Control* control = dynamic_cast<Control*>( componentToRemove );
	if ( NULL != control ) {
		parent = control->getParent();
		//parent->remo
		Container* container = dynamic_cast<Container*>( parent );
		while ( container == NULL ) {
			Control* tmpParent = parent->getParent();
			container = dynamic_cast<Container*>( tmpParent );
		}
		if ( NULL != container ) {
			container->remove( control );
			control->setVisible( false );

			SelectionManager::getSelectionMgr()->scheduleControlForDeletion( control );			
		}
		selectionMgr->clearSelectionSet();
		this->m_singleSelectionGripper.clearSelectionGripper();

		this->m_activeFormInst->saveVFF();
	}

	if ( NULL != parent ) {
		selectionMgr->addToSelectionSet( parent );
	}
}

void FormView::onSelectionManagerChanged( SelectionManagerEvent* e )
{
	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	switch( e->getType() ) {
		case SELECTION_MGR_ITEM_ADDED : {
			StringUtils::trace( "FormView::onSelectionManagerChanged()\n" );
			Enumerator<Component*>* selectionSet = selectionMgr->getSelectionSet();			

			EventHandler* keyPressHandler = getEventHandler( SELECTED_CONTROL_KEYPRESSEDHANDLER );

			EventHandler* mouseDownHandler = getEventHandler( SELECTED_CONTROL_MOUSEDOWNHANDLER );
			EventHandler* mouseUpHandler = getEventHandler( SELECTED_CONTROL_MOUSEUPHANDLER );
			EventHandler* mouseMoveHandler = getEventHandler( SELECTED_CONTROL_MOUSEMOVEHANDLER );

			EventHandler* controlPositionedHandler = getEventHandler( SELECTED_CONTROL_POSITIONEDHANDLER );

			while ( true == selectionSet->hasMoreElements() ) {
				Component* selectedComponent = selectionSet->nextElement();
				StringUtils::trace( "Selected component: \"" + selectedComponent->getName() + "\"\n" );
				Control* selectedControl = dynamic_cast<Control*>(selectedComponent);
				if ( NULL != selectedControl ) {					
					
					selectedControl->addMouseDownHandler( mouseDownHandler );
					selectedControl->addMouseUpHandler( mouseUpHandler );
					selectedControl->addMouseMoveHandler( mouseMoveHandler );

					Frame* frame = dynamic_cast<Frame*>(selectedControl);
					if ( NULL != frame ) {
						m_activeForm = frame;
					}
					else {						
						selectedControl->addKeyPressedHandler( keyPressHandler );
						selectedControl->addControlPositionedHandler( controlPositionedHandler );
						selectedControl->addControlSizedHandler( controlPositionedHandler );
					}
				}
			}
			selectionSet->reset();
			updateComponentListForSelectedComponents( selectionSet );
		}
		break;

		case SELECTION_MGR_ITEM_REMOVED : {
			Enumerator<Component*>* selectionSet = selectionMgr->getSelectionSet();			
			
			EventHandler* keyPressHandler = getEventHandler( SELECTED_CONTROL_KEYPRESSEDHANDLER );

			EventHandler* mouseDownHandler = getEventHandler( SELECTED_CONTROL_MOUSEDOWNHANDLER );
			EventHandler* mouseUpHandler = getEventHandler( SELECTED_CONTROL_MOUSEUPHANDLER );
			EventHandler* mouseMoveHandler = getEventHandler( SELECTED_CONTROL_MOUSEMOVEHANDLER );

			EventHandler* controlPositionedHandler = getEventHandler( SELECTED_CONTROL_POSITIONEDHANDLER );

			while ( true == selectionSet->hasMoreElements() ) {
				Component* selectedComponent = selectionSet->nextElement();
				Control* selectedControl = dynamic_cast<Control*>(selectedComponent);
				if ( NULL != selectedControl ) {					
					selectedControl->removeMouseDownHandler( mouseDownHandler );
					selectedControl->removeMouseUpHandler( mouseUpHandler );
					selectedControl->removeMouseMoveHandler( mouseMoveHandler );

					Frame* frame = dynamic_cast<Frame*>(selectedControl);
					if ( NULL == frame ) {						
						selectedControl->removeKeyPressedHandler( keyPressHandler );
						selectedControl->removeControlPositionedHandler( controlPositionedHandler );
						selectedControl->removeControlSizedHandler( controlPositionedHandler );
					}
				}
			}
			selectionSet->reset();
			updateComponentListForSelectedComponents( selectionSet );
		}
		break;
	}
}

void FormView::updateComponentListForSelectedComponents( Enumerator<Component*>* componentSelectionSet )
{
	ListModel* lm = m_formComponentsView->getListModel();
	lm->empty();

	if ( NULL != componentSelectionSet ) {
		//only read off the first item in the set
		if ( true == componentSelectionSet->hasMoreElements() ) {
			Component* component = componentSelectionSet->nextElement();
			Control* control = dynamic_cast<Control*>( component );
			if ( NULL != control ) {
				Enumerator<Component*>* components = component->getComponents();
				while ( true == components->hasMoreElements() ) {
					Component* childComponent = components->nextElement();
					DefaultListItem* componentListItem = new DefaultListItem();
					componentListItem->setData( (void*)childComponent );
					componentListItem->setCaption( childComponent->getName() );
					lm->addItem( componentListItem );
				}
			}
		}
	}
}

void FormView::setActiveForm( Frame* activeForm, Form* activeFormInst )
{
	if ( NULL != this->m_activeForm ) {
		SelectionManager::getSelectionMgr()->clearSelectionSet();
		this->m_singleSelectionGripper.clearSelectionGripper();
		//save off the current state 
		this->m_activeFormInst->saveVFF();

		m_formGrid->remove( m_activeForm );
		m_activeForm->close();
		delete m_activeForm;
		m_activeForm = NULL;		
	}
	if ( NULL != m_activeFormInst ) {
		m_activeFormInst->setRootComponent( NULL );
	}
	m_activeForm = activeForm;
	m_activeFormInst = activeFormInst;

	if ( (NULL != m_activeForm) && (NULL != m_activeFormInst) ) {
		m_formGrid->add( m_activeForm );		
		m_activeFormInst->setRootComponent( m_activeForm );

		m_activeForm->setComponentState( CS_DESIGNING );
		Window* windowForm = dynamic_cast<Window*>( m_activeForm );
		if ( NULL != windowForm ) {				
			EventHandler* handler = getEventHandler( FORMVIEW_WINDOW_HANDLER );
			windowForm->addWindowCloseHandler( handler );
		}
		reloadControlHandlers( m_activeForm );
		m_activeForm->setVisible( true );
		SelectionManager::getSelectionMgr()->addToSelectionSet( m_activeForm );
	}
}

void FormView::onControlKeyPressed( KeyboardEvent* e )
{	
	ulong32 virtKeyCode = e->getVirtualCode();
	Control* control = (Control*)e->getSource();
	Rect newBounds = *(control->getBounds());
	double extraSpacer = 0.0;
	if ( e->hasAltKey() ) {
		extraSpacer += 5.0;
	}
	bool controlBoundsNeedModifying = false;
	switch ( virtKeyCode ) {
		case VIRT_KEY_DELETE : {	
			
			SelectionManager::getSelectionMgr()->clearSelectionSet();

			Project* currentProject = VCFBuilderUIApplication::getVCFBuilderUI()->getCurrentOpenProject();
			RemoveComponentCmd* removeComponentCmd = new RemoveComponentCmd( control->getName(), currentProject, m_activeFormInst );
			UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
			cmdStack->addCommand( removeComponentCmd );
			
		}
		break;

		case VIRT_KEY_UP_ARROW : {
			if ( e->hasControlKey() ) {
				newBounds.m_bottom -= 1 + extraSpacer;
				newBounds.m_top -= 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
			else if ( e->hasShiftKey() ) {
				newBounds.m_bottom -= 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
		}
		break;

		case VIRT_KEY_DOWN_ARROW : {
			if ( e->hasControlKey() ) {
				newBounds.m_bottom += 1 + extraSpacer;
				newBounds.m_top += 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
			else if ( e->hasShiftKey() ) {
				newBounds.m_bottom += 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
		}
		break;

		case VIRT_KEY_LEFT_ARROW : {
			if ( e->hasControlKey() ) {
				newBounds.m_right -= 1 + extraSpacer;
				newBounds.m_left -= 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
			else if ( e->hasShiftKey() ) {
				newBounds.m_right -= 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
		}
		break;

		case VIRT_KEY_RIGHT_ARROW : {
			if ( e->hasControlKey() ) {
				newBounds.m_right += 1 + extraSpacer;
				newBounds.m_left += 1 + extraSpacer;
				controlBoundsNeedModifying = true;
			}
			else if ( e->hasShiftKey() ) {
				newBounds.m_right += 1 + extraSpacer;
				controlBoundsNeedModifying = true;				
			}
		}
		break;
	}

	if ( true == controlBoundsNeedModifying ) {
		TranslateControlCmd* translateCmd = new TranslateControlCmd( control->getName(), control->getBounds(), &newBounds ); 
		UndoRedoStack* cmdStack = VCFBuilderUIApplication::getVCFBuilderUI()->getCommandStack();
		cmdStack->addCommand( translateCmd );
	}
}

void FormView::onControlPositioned( ControlEvent* e )
{
	m_singleSelectionGripper.select();
}

void FormView::onControlSized( ControlEvent* e )
{

}

void FormView::onShowFormAsVFF( MenuItemEvent* e )
{
	bool visible = m_formAsTextView->getVisible();
	m_formAsTextView->setVisible( !visible );
	if ( false == visible ) { //means we're about to show the VFF 
		
		TextOutputStream tos;
		VFFOutputStream vos(&tos);		

		vos.writeComponent( m_activeForm );
		String s = tos.getTextBuffer();
		m_formAsTextView->getTextModel()->setText( s );
	}
	else {
		BasicInputStream bis( m_formAsTextView->getTextModel()->getText() );
		VFFInputStream vis(&bis);
		

		//setActiveForm( NULL, m_activeFormInst );		
		
		Control* newActiveForm = NULL;
		vis.readComponentInstance( m_activeForm );//(Component**)&newActiveForm );
		
		//setActiveForm( (Frame*)newActiveForm, m_activeFormInst );
	}
	
	this->m_formGrid->setVisible( visible );
	resizeChildren();
}

void FormView::onUpdateShowFormAsVFF( MenuItemEvent* e )
{
	MenuItem* item = dynamic_cast<MenuItem*>( e->getSource() );
	
	item->setEnabled( m_activeForm != NULL );
	
	String menuItemName = item->getCaption();
	if ( true == m_formAsTextView->getVisible() ) {
		menuItemName = "Show Form as Control";	
	}
	else {
		menuItemName = "Show Form as VFF";
	}
	item->setCaption( menuItemName );
}

void FormView::reloadControlHandlers( Control* control )
{
	EventHandler* keyPressHandler = getEventHandler( SELECTED_CONTROL_KEYPRESSEDHANDLER );
	
	EventHandler* mouseDownHandler = getEventHandler( SELECTED_CONTROL_MOUSEDOWNHANDLER );
	EventHandler* mouseUpHandler = getEventHandler( SELECTED_CONTROL_MOUSEUPHANDLER );
	EventHandler* mouseMoveHandler = getEventHandler( SELECTED_CONTROL_MOUSEMOVEHANDLER );
	
	EventHandler* controlPositionedHandler = getEventHandler( SELECTED_CONTROL_POSITIONEDHANDLER );
	
	control->setComponentState( CS_DESIGNING );
	if ( control != this->m_activeForm ) {		
		control->addMouseDownHandler( mouseDownHandler );
		control->addMouseUpHandler( mouseUpHandler );
		control->addMouseMoveHandler( mouseMoveHandler );
		
		control->addKeyPressedHandler( keyPressHandler );
		
		control->addControlPositionedHandler( controlPositionedHandler );
		control->addControlSizedHandler( controlPositionedHandler );
	}	

	Container* container = dynamic_cast<Container*>( control );
	if ( NULL != container ) {
		Enumerator<Control*>* children = container->getChildren();
		if ( NULL != children ) {
			while ( true == children->hasMoreElements() ) {
				Control* childControl = children->nextElement();
				reloadControlHandlers( childControl );
			}
		}
	}
}

void FormView::setActiveProject( Project* activeProject )
{
	if ( NULL != activeProject ) {
		EventHandler* pl = getEventHandler( FORMVIEW_PROJECT_HANDLER );
		activeProject->addProjectChangedHandler( pl );
	}
}

void FormView::onProjectChanged( ProjectEvent* e )
{
	switch ( e->getType() ) {
		case PROJECT_ITEM_ADDED: {
			switch ( e->getItemType() ) {
				case PIT_COMPONENT : {
					ComponentInstance* compInst = (ComponentInstance*)e->getObjectThatChanged();
					Component* newComponent = compInst->getComponent();
					addComponentToForm( newComponent );
				}
				break;
			}
		}
		break;

		case PROJECT_ITEM_REMOVED: {
			switch ( e->getItemType() ) {
				case PIT_COMPONENT : {
					ComponentInstance* compInst = (ComponentInstance*)e->getObjectThatChanged();
					Component* componentToRemove = compInst->getComponent();
					this->removeComponentFromForm( componentToRemove );
				}
				break;
			}
		}
		break;
	}
}

void FormView::onComponentsViewClick( MouseEvent* event )
{
	ListItem* selectedItem = this->m_formComponentsView->getSelectedItem();
	if ( NULL != selectedItem ) {
		SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
		selectionMgr->clearSelectionSet();
		Component* component = (Component*)selectedItem->getData();
		selectionMgr->addToSelectionSet( component );
	}
}
