//TabModelListener.h

#ifndef _TABMODEL_LISTENER_H__
#define _TABMODEL_LISTENER_H__


namespace VCF {



/**
*TabModelEventHandler
*handles the following:
*onTabPageAdded
*onTabPageRemoved
*onTabPageSelected
*/
template <class SOURCE_TYPE>  class APPKIT_API TabModelEventHandler : public EventHandlerInstance<SOURCE_TYPE,TabModelEvent> {
public:
	TabModelEventHandler( SOURCE_TYPE* source,
		EventHandlerInstance<SOURCE_TYPE,TabModelEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,TabModelEvent>( source, handlerMethod, handlerName ) {
			
	}
	
	virtual ~TabModelEventHandler(){};

};

};
#endif