//UndoRedoEvent.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _UNDOREDOEVENT_H__
#define _UNDOREDOEVENT_H__




#define UNDOREDO_EVENT_UNDO				CUSTOM_EVENT_TYPES + 1000
#define UNDOREDO_EVENT_REDO				CUSTOM_EVENT_TYPES + 1001
#define UNDOREDO_EVENT_EXECUTE			CUSTOM_EVENT_TYPES + 1002
#define UNDOREDO_EVENT_STACK_CLEARED	CUSTOM_EVENT_TYPES + 1003
#define UNDOREDO_EVENT_STACK_CHANGED	CUSTOM_EVENT_TYPES + 1004

namespace VCF  {

class Command;

/**
*Class UndoRedoEvent documentation
*/
class APPKIT_API UndoRedoEvent : public Event { 
public:
	UndoRedoEvent( Object* source, const unsigned long& eventType, Command* command=NULL );

	virtual ~UndoRedoEvent();

	Command* getCommand(){
		return m_command;
	}

	void setCommand( Command* command ) {
		m_command = command;
	}

	bool getAllowsUndo() {
		return 	m_allowUndo;
	}

	void setAllowsUndo( const bool& allowsUndo );

	bool getAllowsRedo() {
		return 	m_allowRedo;
	}

	void setAllowsRedo( const bool& allowsRedo );
protected:
	Command* m_command;
	bool m_allowUndo;
	bool m_allowRedo;
	bool m_allowExecute;
private:
};


}; //end of namespace VCF

#endif //_UNDOREDOEVENT_H__


