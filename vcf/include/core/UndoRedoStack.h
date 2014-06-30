//UndoRedoStack.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _UNDOREDOSTACK_H__
#define _UNDOREDOSTACK_H__



#include <deque>



namespace VCF  {

/**
*Class UndoRedoStack documentation
*/
	
class APPKIT_API UndoRedoStack : public ObjectWithEvents { 
public:
	UndoRedoStack();

	virtual ~UndoRedoStack();

	virtual void undo();

	virtual void redo();

	virtual bool hasUndoableItems();

	virtual bool hasRedoableItems();

	virtual void addCommand( Command* command, const bool& autoExecute=true );

	virtual Command* getCurrentUndoComand();

	virtual Command* getCurrentRedoComand();

	void clearCommands();

	EVENT_HANDLER_LIST(UndoCommand);
	EVENT_HANDLER_LIST(RedoCommand);
	EVENT_HANDLER_LIST(StackCleared);
	EVENT_HANDLER_LIST(StackChanged);
	EVENT_HANDLER_LIST(ExecuteCommand);

	ADD_EVENT(UndoCommand);
	ADD_EVENT(RedoCommand);
	ADD_EVENT(StackCleared);
	ADD_EVENT(StackChanged);
	ADD_EVENT(ExecuteCommand);

	REMOVE_EVENT(UndoCommand);
	REMOVE_EVENT(RedoCommand);
	REMOVE_EVENT(StackCleared);
	REMOVE_EVENT(StackChanged);
	REMOVE_EVENT(ExecuteCommand);

	FIRE_EVENT(UndoCommand,UndoRedoEvent);
	FIRE_EVENT(RedoCommand,UndoRedoEvent);
	FIRE_EVENT(StackCleared,UndoRedoEvent);
	FIRE_EVENT(StackChanged,UndoRedoEvent);
	FIRE_EVENT(ExecuteCommand,UndoRedoEvent);

	
protected:
	void movetToRedoStack( Command* command );

	std::deque<Command*> m_undoStack;
	std::deque<Command*> m_redoStack;
private:
};


}; //end of namespace VCF

#endif //_UNDOREDOSTACK_H__


