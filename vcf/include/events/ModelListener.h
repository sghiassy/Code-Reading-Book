//ModelListener.h

#ifndef _MODELLISTENER_H__
#define _MODELLISTENER_H__


namespace VCF {

class ModelEvent;

/**
*ModelEventHandler
*handles the following:
*onModelEmptied
*/
template <class SOURCE_TYPE> class APPKIT_API ModelEventHandler : public EventHandlerInstance<SOURCE_TYPE,ModelEvent> {
public:
	ModelEventHandler( SOURCE_TYPE* source,
		EventHandlerInstance<SOURCE_TYPE,ModelEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,ModelEvent>( source, handlerMethod, handlerName ) {
			
	}
	
	virtual ~ModelEventHandler(){};
	
};

}; //namespace VCF

#endif //_MODELLISTENER_H__