//SelectionManager.h

#ifndef _SELECTIONMANAGER_H__
#define _SELECTIONMANAGER_H__




namespace VCFBuilder  {


#define SELECTION_MGR_ITEM_ADDED		CUSTOM_EVENT_TYPES + 6000
#define SELECTION_MGR_ITEM_REMOVED		CUSTOM_EVENT_TYPES + 6001
#define SELECTION_MGR_ITEMS_CLEARED		CUSTOM_EVENT_TYPES + 6002


class SelectionManagerEvent : public VCF::Event {
public:
	SelectionManagerEvent( VCF::Object* source, const ulong32& eventType ) :
	  Event( source, eventType ){ 

	  }

	  virtual ~SelectionManagerEvent(){};


};

template <class SOURCE_TYPE> class SelectionManagerEventHandler : public EventHandlerInstance<SOURCE_TYPE,SelectionManagerEvent> {
public:
	SelectionManagerEventHandler( SOURCE_TYPE* source,
		EventHandlerInstance<SOURCE_TYPE,SelectionManagerEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,SelectionManagerEvent>( source, handlerMethod, handlerName ) {
			
	}
	
	virtual ~SelectionManagerEventHandler(){};
};


/**
*Class SelectionManager documentation
*/
class SelectionManager : public VCF::Object { 
public:
	SelectionManager();

	virtual ~SelectionManager();

	EVENT_HANDLER_LIST(SelectionChanged)
	ADD_EVENT(SelectionChanged) 
	REMOVE_EVENT(SelectionChanged) 
	FIRE_EVENT(SelectionChanged,SelectionManagerEvent)
	

	void init();

	Enumerator<Component*>* getSelectionSet();

	bool isComponentInSelectionSet( Component* component );

	void addToSelectionSet( Component* component );

	void removeFromSelectionSet( Component* component );

	void clearSelectionSet();

	bool hasMultipleSelections() {
		return m_selectionSet.size() > 1;
	}

	ulong32 getSelectionSetCount() {
		return m_selectionSet.size();
	}

	Component* getSelectedItemAt( const ulong32& index ) {
		return m_selectionSet[index];
	}

	static SelectionManager* getSelectionMgr();

	void scheduleControlForDeletion( VCF::Control* control );

	void cleanUpDeletableControls();

protected:
	std::vector<VCF::Component*> m_selectionSet;
	std::vector<VCF::Control*> m_scheduledControlsToBeDeleted;
	std::vector<VCF::Control*> m_controlsToBeDeleted;
	EnumeratorContainer<std::vector<VCF::Component*>,VCF::Component*> m_selectionContainer;
private:
	static SelectionManager selectionMgrInstance;
};


}; //end of namespace VCFBuilder

#endif //_SELECTIONMANAGER_H__



