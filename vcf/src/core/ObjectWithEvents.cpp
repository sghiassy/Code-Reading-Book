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

#include "FoundationKit.h"


using namespace VCF;


ObjectWithEvents::ObjectWithEvents()
{
	INIT_EVENT_HANDLER_LIST(NotifyHandler)
}

ObjectWithEvents::~ObjectWithEvents()
{
	//destroy all the event handlers
	std::map<String,EventHandler*>::iterator it = m_eventHandlers.begin();
	while ( it != m_eventHandlers.end() ){
		if ( NULL != it->second ){
			delete it->second;
		}
		it++;
	}
	m_eventHandlers.clear();

	std::vector<EventHandler::Vector*>::iterator handlerIt = m_masterHandlerList.begin();
	while ( handlerIt != m_masterHandlerList.end() ) {
		EventHandler::Vector* list = *handlerIt;
		delete list;
		list = NULL;
		handlerIt ++;
	}
	m_masterHandlerList.clear();
}

void ObjectWithEvents::addEventHandler( const String& handlerName, EventHandler* handler )
{
	m_eventHandlers[handlerName] = handler;
}

EventHandler* ObjectWithEvents::getEventHandler( const String& handlerName )
{
	EventHandler* result = NULL;
	std::map<String,EventHandler*>::iterator found = m_eventHandlers.find( handlerName );
	if ( found != m_eventHandlers.end() ){
		result = found->second;
	}
	return result;
}

void ObjectWithEvents::addEventHandlerList( EventHandler::Vector* eventHandlerList )
{
	m_masterHandlerList.push_back( eventHandlerList );
}