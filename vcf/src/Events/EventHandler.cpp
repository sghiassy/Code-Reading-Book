//EventHandler.cpp
#include "FoundationKit.h"

using namespace VCF;

void EventHandler::addHandlerToSource( Object* source, const String& handlerName )
{
	ObjectWithEvents* objEventOwner = dynamic_cast<ObjectWithEvents*>(source);
	if ( NULL != objEventOwner ) {
		if ( ! handlerName.empty() ) {
			objEventOwner->addEventHandler( handlerName, this );
		}
		else {
			//StringUtils::trace( "this event handler does not have a name, and will not have it's memory cleaned up. \nExpect memory leaks from this!\n" );
		}
	}
	else {
		//StringUtils::traceWithArgs( "this event handler does not have a source object (class Name: %s) that can clean up it's memory. \nExpect memory leaks from this!\n", 
		//							source->getClassName().c_str() );
	}
}

void EventHandler::addHandlerList( Object* eventOwner, EventHandler::Vector* handlerList )
{
	ObjectWithEvents* objWithEvents = dynamic_cast<ObjectWithEvents*>(eventOwner);
	if ( NULL != objWithEvents ) {
		objWithEvents->addEventHandlerList( handlerList ); 
	}
	else {
		//StringUtils::traceWithArgs( "this EventHandler::Vector* does not have a source (class Name: %s) object that can clean up it's memory. \nExpect memory leaks from this!\n",
		//							eventOwner->getClassName().c_str() );
	}
}