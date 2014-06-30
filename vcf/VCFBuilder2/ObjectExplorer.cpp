//ObjectExplorer.h
#include "ApplicationKit.h"
#include "ObjectExplorer.h"
#include "Panel.h"
#include "PropertyTableModel.h"
#include "SelectionManager.h"
#include "PropertyCellItem.h"
#include "EventTableModel.h"
#include "EventCellItem.h"
#include "VCFBuilderUI.h"
#include "MainUIPanel.h"
#include "Form.h"
#include "TableItemEditor.h"
#include "Project.h"
#include "CodeGenerator.h"

using namespace VCF;

using namespace VCFBuilder;


ObjectExplorer::ObjectExplorer()
{
	TabPage* propPage = addNewPage( "Properties" );
	m_propertyTable = new TableControl( new PropertyTableModel() );
	Panel* propPanel = (Panel*)propPage->getPageComponent();
	propPanel->add( m_propertyTable, ALIGN_CLIENT );

	//m_propertyScroller.setScrollableControl( m_propertyTable );
	//m_propertyTable->setScrollable( &m_propertyScroller );
	

	TabPage* eventPage = addNewPage( "Events" );

	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	SelectionManagerEventHandler<ObjectExplorer>* smh = 
		new SelectionManagerEventHandler<ObjectExplorer>( this, ObjectExplorer::onSelectionManagerChanged,"SelectionManagerHandler" );
	
	selectionMgr->addSelectionChangedHandler( smh );
	


	m_eventTable = new TableControl( new EventTableModel() );
	Panel* eventPanel = (Panel*)eventPage->getPageComponent();
	eventPanel->add( m_eventTable, ALIGN_CLIENT );

	MouseEventHandler<ObjectExplorer>* mh = 
		new MouseEventHandler<ObjectExplorer>( this, ObjectExplorer::onEventItemDblClick, "EventTableMouseHandler" );		
	m_eventTable->addMouseDoubleClickedHandler( mh );

	m_itemEventHandler = 
		new ItemEditorEventHandler<ObjectExplorer>( this, ObjectExplorer::onEventItemChanged, "ItemEventHandler" );		

	m_prevEventItemEditor = NULL;
}

ObjectExplorer::~ObjectExplorer()
{

}

void ObjectExplorer::onSelectionManagerChanged( SelectionManagerEvent* e )
{
	SelectionManager* selectionMgr = SelectionManager::getSelectionMgr();
	switch( e->getType() ) {
		case SELECTION_MGR_ITEM_ADDED : {
			Enumerator<Component*>* selectionSet = selectionMgr->getSelectionSet();						

			while ( true == selectionSet->hasMoreElements() ) {
				Component* selectedComponent = selectionSet->nextElement();		
				updateComponentProperties( selectedComponent );
				updateComponentEvents( selectedComponent );
				break;//only do first element for now
			}			
		}
		break;

		case SELECTION_MGR_ITEM_REMOVED : {
			Enumerator<Component*>* selectionSet = selectionMgr->getSelectionSet();			
			
			updateComponentProperties( NULL );
			updateComponentEvents( NULL );

			while ( true == selectionSet->hasMoreElements() ) {
				Component* selectedComponent = selectionSet->nextElement();
				
				break;//only do first element for now
			}			
		}
		break;

		case SELECTION_MGR_ITEMS_CLEARED : {

			updateComponentProperties( NULL );
			updateComponentEvents( NULL );
		}
		break;		
	}
}

void ObjectExplorer::updateComponentProperties( Component* component )
{

	TableModel* tm = m_propertyTable->getTableModel();
	//m_propertyScroller.setVirtualViewWidth( m_propertyTable->getWidth() );
	double virtHeight = 0.0;
	double col1Width = -1.0;
	double col2Width = -1.0;
	if ( tm->getColumnCount() > 1 ) {
		col1Width = tm->getItem( 0, 0 )->getBounds()->getWidth();
		col2Width = tm->getItem( 0, 1 )->getBounds()->getWidth();
	}
	tm->empty();
	if ( NULL != component ) {
		tm->addColumn();		
		tm->addColumn();
		
		TableCellItem* item = tm->getItem( 0, 0 );		
		item->setCaption( "Name" );
		
		item = tm->getItem( 0, 1 );
		item->setCaption( "Value" );

		Class* clazz = component->getClass();
		if ( NULL != clazz ){
			Enumerator<Property*>* props = clazz->getProperties();
			if ( NULL != props ){
				int row = 1;
				while ( props->hasMoreElements() ){
					Property* p = props->nextElement();
					tm->addRow();
					virtHeight += m_propertyTable->getDefaultRowHeight();
					item = tm->getItem( row, 0 );
					item->setCaption( p->getName() );
					
					item = tm->getItem( row, 1 );						
					PropertyCellItem* pci = (PropertyCellItem*)item;
					pci->setProperty( p );
					row++;
				}
			}
		}
	}
	m_propertyTable->setFixedColumn( true, 0 );
	m_propertyTable->setFixedRow( true, 0 );
	if ( col1Width > 0.0 ) {
		m_propertyTable->setColumnWidth( 0, col1Width );
	}
	else {
		m_propertyTable->setColumnWidth( 0, 200 );
	}
	if ( col2Width > 0.0 ) {
		m_propertyTable->setColumnWidth( 1, col2Width );
	}
	else {
		m_propertyTable->setColumnWidth( 1, 200 );
	}
	
	m_propertyTable->repaint();
}

void ObjectExplorer::updateComponentEvents( Component* component )
{

	TableModel* tm = m_eventTable->getTableModel();
	double col1Width = -1.0;
	double col2Width = -1.0;
	if ( tm->getColumnCount() > 1 ) {
		col1Width = tm->getItem( 0, 0 )->getBounds()->getWidth();
		col2Width = tm->getItem( 0, 1 )->getBounds()->getWidth();
	}
	tm->empty();
	if ( NULL != component ) {
		tm->addColumn();		
		tm->addColumn();
		
		TableCellItem* item = tm->getItem( 0, 0 );		
		item->setCaption( "Event" );
		
		item = tm->getItem( 0, 1 );
		item->setCaption( "Handler" );		

		Class* clazz = component->getClass();
		if ( NULL != clazz ){
			Enumerator<EventProperty*>* events = clazz->getEvents();			
			if ( NULL != events ){					
				int row = 1;
				while ( true == events->hasMoreElements() ){
					EventProperty* event = events->nextElement();							
					if ( NULL != event ){
						tm->addRow();
						
						item = tm->getItem( row, 0 );
						EventCellItem* eci = dynamic_cast<EventCellItem*>(item);
						if ( NULL != eci ){
							eci->setEvent( event );
						}
						//eps->getListenerClassName() + "::" + 
						item->setCaption( event->getEventMethodName() );
						
						item = tm->getItem( row, 1 );
						
						item->setData( (void*)component );
						eci = dynamic_cast<EventCellItem*>(item);
						
						String evKey = component->getName();
						
						MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
						if ( NULL != mainUIPanel ) {
							Form* activeForm = mainUIPanel->getActiveForm();
							String eventMethodName = activeForm->findComponentEventHandler( component, event );
							if ( false == eventMethodName.empty() ) {
								item->setCaption( eventMethodName );
							}
						}

						if ( NULL != eci ){
							eci->setEvent( event );
						}
						row++;
					}
				}
			}
		}
	}
	m_eventTable->setFixedColumn( true, 0 );
	m_eventTable->setFixedRow( true, 0 );
	if ( col1Width > 0.0 ) {
		m_eventTable->setColumnWidth( 0, col1Width );
	}
	else {
		m_eventTable->setColumnWidth( 0, 200 );
	}
	if ( col2Width > 0.0 ) {
		m_eventTable->setColumnWidth( 1, col2Width );
	}
	else {
		m_eventTable->setColumnWidth( 1, 200 );
	}
	m_eventTable->repaint();
}

void ObjectExplorer::onEventItemDblClick( MouseEvent* e )
{
	TableCellItem* selectedItem = this->m_eventTable->getSelectedItem();
	if ( NULL != m_prevEventItemEditor ){
		//m_prevItemEditor->removeItemEditorListener( m_itemEventAdapter );
	}
	if ( NULL != selectedItem ){
		Component* component = (Component*)selectedItem->getData();
		EventCellItem* eci = dynamic_cast<EventCellItem*>(selectedItem);
		TableItemEditor* tie = selectedItem->getItemEditor();		
		tie->addCellItemChangedHandler( this->m_itemEventHandler );
		m_prevEventItemEditor = tie;
		if ( NULL != eci ){
			String itemCaption = selectedItem->getCaption();
			if ( itemCaption == "" ){
				MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
				if ( NULL != mainUIPanel ) {
					Form* activeForm = mainUIPanel->getActiveForm();					
					Project* owningProject = activeForm->getOwningProject();
					CodeGenerator* codeGen = owningProject->getCodeGenerator();
					codeGen->setOwningForm( activeForm );
					if ( NULL == codeGen ) {
						throw RuntimeException( "Project has no code generator associated with it - Can't output code" );
					}

					//this chunk of code need to be encapsulated in a command !
					EventProperty* event = eci->getEvent();
					String methName = codeGen->generateEventMethodName( component, event );
					selectedItem->setCaption( methName );					
				}
			}

			m_selectedItemsOriginalEventHandlerCallback = itemCaption;
		}
	}
	else {
		m_prevEventItemEditor = NULL;
	}
}

void ObjectExplorer::onEventItemChanged( ItemEditorEvent* e )
{		
	String eventHandlerCallback = e->getItemBeingEdited()->getCaption();
	
	EventCellItem* eci = dynamic_cast<EventCellItem*>(e->getItemBeingEdited());

	MainUIPanel* mainUIPanel = VCFBuilderUIApplication::getVCFBuilderUI()->getMainUIPanel();
	if ( NULL != mainUIPanel ) {
		Form* activeForm = mainUIPanel->getActiveForm();					
		
		Project* owningProject = activeForm->getOwningProject();
		
		CodeGenerator* codeGen = owningProject->getCodeGenerator();
		
		if ( NULL == codeGen ) {
			throw RuntimeException( "Project has no code generator associated with it - Can't output code" );
		}

		codeGen->setOwningForm( activeForm );
		
		Component* component = (Component*)eci->getData();
		
		EventProperty* event = eci->getEvent();

		if ( true == eventHandlerCallback.empty() ) {
			activeForm->removeComponentEventHandler( component, event, m_selectedItemsOriginalEventHandlerCallback );

			try {				
				codeGen->removeEvent( m_selectedItemsOriginalEventHandlerCallback, component, event );
			}
			catch ( BasicException& e ) {
				StringUtils::trace( "unable to remove the event from " + activeForm->getName() + "\n") ;
				throw e;		
			}
		}
		else {
			//does this event already exist ? this is a bit wimpy 
			//we'll have to expand this in the future
			String existingCallback = activeForm->findComponentEventHandler( component, event );

			if ( true == existingCallback.empty() ) {
				activeForm->addComponentEventHandler( component, event, eventHandlerCallback );					
				
				try {
					
					codeGen->addEvent( eventHandlerCallback, component, event );
				}
				catch ( BasicException& e ) {
					StringUtils::trace( "unable to add the event to " + activeForm->getName() + "\n") ;
					throw e;		
				}
			}
		}
	}
}