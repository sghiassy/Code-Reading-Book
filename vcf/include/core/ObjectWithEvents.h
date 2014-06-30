//ObjectWithEvents.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _OBJECTWITHEVENTS_H__
#define _OBJECTWITHEVENTS_H__


#define OBJECTWITHEVENTS_CLASSID		"eadd685c-e57b-49c5-b93d-00648641102a"


namespace VCF  {

class NotifyEvent;

/**
*ObjectWithEvents is an object that may
*have 0 or more EventHandlers associated with it,
*and will automatically clean up these EventHandlers
*when it is destroyed. This simplifies handling 
*who has to clean up EventHandlers that have been
*allocated on the heap (which they always will be).
*/
class FRAMEWORK_API ObjectWithEvents : public Object { 
public:
	BEGIN_CLASSINFO(ObjectWithEvents, "VCF::ObjectWithEvents", "VCF::Object", OBJECTWITHEVENTS_CLASSID)
	EVENT( "VCF::NotifyHandler","VCF::NotifyEvent", "fireOnNotifyHandler" )
	END_CLASSINFO(ObjectWithEvents)

	ObjectWithEvents();

	virtual ~ObjectWithEvents();

	/**
	*This is a utility method for adding event handlers to an object.
	*Adding a handler provides a convenient way to store the various events
	*an object may wish to listen to. Handlers still need to have their method 
	*callbacks set, otherwise nothing will happen. When the object is destroyed
	*all the event handlers in this list are also destroyed.
	*/
	void addEventHandler( const String& handlerName, EventHandler* handler );

	/**
	*Finds a EventHandler handler for a given name. If the object has that 
	*handler then a pointer to the is returned otherwise null
	*is returned
	*/
	EventHandler* getEventHandler( const String& handlerName );

	/**
	*Adds the vector of handlers to the master list. This list will be 
	*destroyed when the destructor is called
	*/
	void addEventHandlerList( EventHandler::Vector* eventHandlerList );

	/**
	*Notifier events
	*/
	EVENT_HANDLER_LIST(NotifyHandler);
	ADD_EVENT(NotifyHandler);
	REMOVE_EVENT(NotifyHandler);
	FIRE_EVENT(NotifyHandler,NotifyEvent);

protected:
	/**
	*This map holds a list of EventHandlers that have been 
	*added to the object
	*/
	std::map<String,EventHandler*> m_eventHandlers;

	/**
	*this vector holds a list of vector that in turn hold a series 
	*of 0 or more handlers. The idea is to conserve memrory, as each
	*vector defined as a member on the stace in a class adds 16 bytes to 
	*the class size, which begins to add up, since there will now 
	*be a vector for each type of event method fired, as opposed to a 
	*listener for the set
	*/
	std::vector<EventHandler::Vector*> m_masterHandlerList;
private:
};


}; //end of namespace VCF

#endif //_OBJECTWITHEVENTS_H__


