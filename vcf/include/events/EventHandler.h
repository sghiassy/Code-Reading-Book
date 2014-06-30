//EventHandler.h

/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#ifndef _EVENTHANDLER_H__
#define _EVENTHANDLER_H__


namespace VCF  {

/**
*EventHandler's form the back bone of the event 
*model in the VCF. The EventHandler base class
*provides a clean way to invoke the class 
*specific member function pointers in the 
*derived EventHandlerInstance template class. 
*
*/
class FRAMEWORK_API EventHandler : public VCF::Object { 
public:
	typedef std::vector<EventHandler*> Vector;

	EventHandler(){};

	virtual ~EventHandler(){};

	/**
	*called during the events dispatch cycle
	*The implemtnation will end up calling the 
	*appropriate call back method 
	*/
	virtual void invoke( Event* e ) = 0;

	/**
	*adds the EventHandler to it's source object.
	*For this to work the source must be derived 
	*from ObjectWithEvents. By adding the event handler
	*to the source, the handler's memory will be cleaned 
	*up by the source when the source is destroyed.
	*The EventHandler may be retreived at any time 
	*by calling the getEventHandler() method on the 
	*source (once again, assuming the source is derived
	*from ObjectWithEvents).
	*@param Object the source to which the EventHandler
	*will be added.
	*@param String the name the EventHandler is referred to.
	*This should be a reasonably unique name. 
	*/
	void addHandlerToSource( Object* source, const String& handlerName );

	/**
	*adds the handler list to the event owner. The eventOwner must derive 
	*from ObjectWithEvent for this to work successfully.
	*/
	static void addHandlerList( Object* eventOwner, EventHandler::Vector* handlerList );
};

/**
*EventHandlerInstances are used to provide a 
*typesafe wrapper around specific class members method pointers.
*In addition, when the are created, if the source passed in is
*derived from VCF::ObjectWithEvents, then the handler will be 
*maintained in a list by the source, and destroyed when the source
*is destroyed, freeing the creator of the handler from worrying about
*memory leaks.
*/
template <class SOURCE, class EVENT> class FRAMEWORK_API EventHandlerInstance : public EventHandler {
public:
	typedef void (SOURCE::*OnEventHandlerMethod)( EVENT* e );

	EventHandlerInstance( SOURCE* source, OnEventHandlerMethod handlerMethod, const String& handlerName="" ) {
		m_source = source;
		m_handlerMethod = handlerMethod; 
		
		addHandlerToSource( source, handlerName );	
	}

	virtual ~EventHandlerInstance(){

	}

	virtual void invoke( Event* e ) {
		if ( (NULL != m_source) && (NULL != m_handlerMethod) ) {
			(m_source->*m_handlerMethod)( dynamic_cast<EVENT*>(e) );
		}
	}

protected:
	SOURCE* m_source;
	OnEventHandlerMethod m_handlerMethod;
};





/**
*Macros for default implementation of event handling 
*and adding support for events in your classes
*These macros a OPTIONAL !. You can do this implementation
*yourself, but these simplify life greatly, as the code 
*is almost always repetitive copy and paste 
*/

/**
*The EVENT_HANDLER_LIST creates a member variable that is a pointer to
*a vector<EventHandler*> (also typedef'ed as EventHandler::Vector for convenience)
*The member variable takes the form m_(HandlerType)List, so the macro
*EVENT_HANDLER_LIST(PropertyChangeHandler) creates a member like so:
*	EventHandler::Vector* m_PropertyChangeHandlerList;
*This member must, Must, MUST be initialized to NULL in the constructor
*of the class that it is declared on. This is done, so that no extra memory is 
*taken up up by the list. Since there will be one list for each handler
*type the object want to expose, it makes it more prudent to try and 
*save a bit. The savings for a class like Control is 68 guaranteed bytes per 
*instance vs. 272 guranteed bytes if the vector were stack based. In addition
*no memory is allocated for the vector if no handlers are added.
*Since the vectors a heap allocated, the object will need to destroy them, 
*which, if the class is derived from VCF::ObjectWithEvents, will be taken
*of automatically for you
*/
#define EVENT_HANDLER_LIST(HandlerType)\
	EventHandler::Vector* m_##HandlerType##List;\
	bool m_##HandlerType##ListChanged;\
	\


/**
*This macro initializes the handler vector to NULL
*This MUST be called in the classes constructor, other wise
*a crash will occur when trying to access the pointer
*/
#define INIT_EVENT_HANDLER_LIST(HandlerType)\
	m_##HandlerType##List = NULL;\



/**
*This creates a member function that allows adding event handlers
*to the class. If the class derives from VCF::ObjectWithEvents then
*the newly created vector will automatically be reclaimed when the 
*object is destroyed. 
*The form of this method is as follows:
*void add(HandlerType)Handler( EventHandler* handler ). In other words
*ADD_EVENT(PropertyChanged) produces the following:
*	void addPropertyChangedHandler( EventHandler* handler )
*/
#define ADD_EVENT(HandlerType)\
	void add##HandlerType##Handler( EventHandler* handler ) {\
		if ( NULL == m_##HandlerType##List ) {\
			m_##HandlerType##List = new EventHandler::Vector();\
			EventHandler::addHandlerList( this, m_##HandlerType##List );\
		}\
		EventHandler::Vector::iterator it = m_##HandlerType##List->begin();\
		while ( it != m_##HandlerType##List->end() ) {\
			if ( *it == handler ) {\
				return;\
			}\
			it ++;\
		}\
		m_##HandlerType##List->push_back( handler );\
	}\
	\

/**
*similar to the ADD_EVENT macro, only this only provides the 
*function body implementation, not the function declaration
* expects the EventHandler passed in to a pointer to 
*an EventHandler class, with the parameter named "handler"
*/ 
#define ADD_EVENT_IMPL(HandlerType)\
		if ( NULL == m_##HandlerType##List ) {\
			m_##HandlerType##List = new EventHandler::Vector();\
			EventHandler::addHandlerList( this, m_##HandlerType##List );\
		}\
		EventHandler::Vector::iterator it = m_##HandlerType##List->begin();\
		while ( it != m_##HandlerType##List->end() ) {\
			if ( *it == handler ) {\
				return;\
			}\
			it ++;\
		}\
		m_##HandlerType##List->push_back( handler );\
		\



/**
*creates a member function for removing an event handler instance from the 
*class.
*The form of this method is as follows:
*void remove(HandlerType)( EventHandler* handler ). In other words
*REMOVE_EVENT(PropertyChangeHandler) produces the following:
*	void removePropertyChangeHandler( EventHandler* handler )
*/
#define REMOVE_EVENT(HandlerType)\
	void remove##HandlerType##Handler( EventHandler* handler ) {\
		EventHandler::Vector::iterator found = std::find( m_##HandlerType##List->begin(), m_##HandlerType##List->end(), handler );\
		if ( found != m_##HandlerType##List->end() ) {\
			m_##HandlerType##List->erase( found );\
			m_##HandlerType##ListChanged = true;\
		}\
	}\
	\

/**
*similar to the REMOVE_EVENT macro, only this only provides the 
*function body implementation, not the function declaration
* expects the EventHandler passed in to a pointer to 
*an EventHandler class, with the parameter named "handler"
*/ 
#define REMOVE_EVENT_IMPL(HandlerType)\
		EventHandler::Vector::iterator found = std::find( m_##HandlerType##List->begin(), m_##HandlerType##List->end(), handler );\
		if ( found != m_##HandlerType##List->end() ) {\
			m_##HandlerType##List->erase( found );\
			m_##HandlerType##ListChanged = true;\
		}\
	\



/**
*This macro creates a member function that can be used to
*iterate through the list of event handler, and call their
*invoke() method, using the event object passed in as a parameter.
*The implementation provided here will correctly handle the 
*case of a handler being removed from object during this methods execution.
*The form of this method is as follows:
*void fireOn(HandlerType)( EventType* event ). In other words
*FIRE_EVENT(PropertyChangeHandler,PropertyChangeEvent) produces the following:
*	void fireOnPropertyChangeHandler( PropertyChangeEvent* event )
*/
#define FIRE_EVENT(HandlerType,EventType)\
	void fireOn##HandlerType( EventType* event ) {\
		if ( NULL == m_##HandlerType##List ) {\
			return;\
		}\
		EventHandler::Vector tmp##HandlerType##List = *m_##HandlerType##List;\
		EventHandler::Vector::iterator it = tmp##HandlerType##List.begin();\
		event->m_consumed = false;\
		bool origListChanged = m_##HandlerType##ListChanged;\
		m_##HandlerType##ListChanged = false;\
		while ( it != tmp##HandlerType##List.end() ) { \
			if ( true == event->m_consumed ) {\
				break;\
			}\
			if ( true == m_##HandlerType##ListChanged ) {\
				bool found = false;\
				EventHandler::Vector::iterator it2 = m_##HandlerType##List->begin();\
				while ( it2 != m_##HandlerType##List->end() ) {\
					if ( *it2 == *it ) {\
						found = true;\
						break;\
					}\
					it2 ++;\
				}\
				if ( !found )\
					continue;\
			}\
			(*it)->invoke(event);\
			it++;\
		}\
		m_##HandlerType##ListChanged = origListChanged;\
	}\
	\



}; //end of namespace VCF

#endif //_EVENTHANDLER_H__


