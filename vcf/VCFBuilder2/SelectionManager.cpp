//SelectionManager.h
#include "ApplicationKit.h"
#include "SelectionManager.h"
#include <algorithm>



using namespace VCF;

using namespace VCFBuilder;

SelectionManager SelectionManager::selectionMgrInstance;


SelectionManager::SelectionManager()
{
	init();
}

SelectionManager::~SelectionManager()
{
	m_scheduledControlsToBeDeleted.clear();
	m_controlsToBeDeleted.clear();
}

void SelectionManager::init()
{
	INIT_EVENT_HANDLER_LIST(SelectionChanged)
	m_selectionContainer.initContainer( m_selectionSet );
}

Enumerator<Component*>* SelectionManager::getSelectionSet()
{
	return m_selectionContainer.getEnumerator();
}

bool SelectionManager::isComponentInSelectionSet( Component* component )
{
	bool result = false;

	std::vector<Component*>::iterator found = std::find( m_selectionSet.begin(), m_selectionSet.end() , component );
	result = ( found != m_selectionSet.end() );

	return result;
}

void SelectionManager::addToSelectionSet( Component* component )
{
	m_selectionSet.push_back( component );
	SelectionManagerEvent event( this, SELECTION_MGR_ITEM_ADDED );
	fireOnSelectionChanged( &event );
}

void SelectionManager::removeFromSelectionSet( Component* component )
{
	std::vector<Component*>::iterator found = std::find( m_selectionSet.begin(), m_selectionSet.end() , component );
	if ( found != m_selectionSet.end() ) {
		SelectionManagerEvent event( this, SELECTION_MGR_ITEM_REMOVED );
		fireOnSelectionChanged( &event );

		m_selectionSet.erase( found );		
	}
}

SelectionManager* SelectionManager::getSelectionMgr()
{
	return &SelectionManager::selectionMgrInstance;
}

void SelectionManager::clearSelectionSet()
{
	SelectionManagerEvent event( this, SELECTION_MGR_ITEMS_CLEARED );
	fireOnSelectionChanged( &event );

	m_selectionSet.clear();	
}

void SelectionManager::scheduleControlForDeletion( Control* control )
{
	m_scheduledControlsToBeDeleted.push_back( control );
}


void SelectionManager::cleanUpDeletableControls()
{
	std::vector<Control*>::iterator it = m_scheduledControlsToBeDeleted.begin();
	while ( it != m_scheduledControlsToBeDeleted.end() ) {
		Control* control = *it;
		control->setComponentState( CS_DESTROYING );
		it ++;
	}

	it = m_controlsToBeDeleted.begin();
	while ( it != m_controlsToBeDeleted.end() ) {
		Control* control = *it;
		delete control;
		control = NULL;
		it ++;
	}
	m_controlsToBeDeleted.clear();

	it = m_scheduledControlsToBeDeleted.begin();
	while ( it != m_scheduledControlsToBeDeleted.end() ) {
		Control* control = *it;
		m_controlsToBeDeleted.push_back( control );		
		it ++;
	}
	m_scheduledControlsToBeDeleted.clear();
}