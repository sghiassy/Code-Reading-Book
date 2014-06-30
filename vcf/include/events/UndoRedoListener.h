//UndoRedoListener.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _UNDOREDOLISTENER_H__
#define _UNDOREDOLISTENER_H__




namespace VCF  {

class UndoRedoEvent;

/**
*UndoRedoEventHandler
*handles the following:
*onUndoCommand
*onRedoCommand
*onStackCleared
*onStackChanged
*onExecuteCommand
*/
template <class SOURCE_TYPE> class APPKIT_API UndoRedoEventHandler : public EventHandlerInstance<SOURCE_TYPE,UndoRedoEvent> {
public:
	UndoRedoEventHandler( SOURCE_TYPE* source,
		EventHandlerInstance<SOURCE_TYPE,UndoRedoEvent>::OnEventHandlerMethod handlerMethod, 
		const String& handlerName="") :
			EventHandlerInstance<SOURCE_TYPE,UndoRedoEvent>( source, handlerMethod, handlerName ) {
			
	}
	
	virtual ~UndoRedoEventHandler(){};

};


}; //end of namespace VCF

#endif //_UNDOREDOLISTENER_H__


