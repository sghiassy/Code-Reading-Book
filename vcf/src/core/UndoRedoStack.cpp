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
#include "ApplicationKit.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"


using namespace VCF;
UndoRedoStack::UndoRedoStack()
{
	INIT_EVENT_HANDLER_LIST(UndoCommand);
	INIT_EVENT_HANDLER_LIST(RedoCommand);
	INIT_EVENT_HANDLER_LIST(StackCleared);
	INIT_EVENT_HANDLER_LIST(StackChanged);
	INIT_EVENT_HANDLER_LIST(ExecuteCommand);
}

UndoRedoStack::~UndoRedoStack()
{
	clearCommands();
}

void UndoRedoStack::clearCommands()
{
	UndoRedoEvent event( this, UNDOREDO_EVENT_STACK_CLEARED );
	fireOnStackCleared( &event );

	std::deque<Command*>::iterator it = m_undoStack.begin();
	while ( it != m_undoStack.end() ) {
		Command* command = *it;
		delete command;
		command = NULL;
		it++;
	}

	it = m_redoStack.begin();
	while ( it != m_redoStack.end() ) {
		Command* command = *it;
		delete command;
		command = NULL;
		it++;
	}
	m_undoStack.clear();
	m_redoStack.clear();
}

void UndoRedoStack::undo()
{
	Command* firstCommand = m_undoStack.back();
	
	UndoRedoEvent event( this, UNDOREDO_EVENT_UNDO,firstCommand );
	fireOnUndoCommand( &event );	
	if ( true == event.getAllowsUndo() ) {
		firstCommand->undo();
		m_undoStack.pop_back();
		m_redoStack.push_front( firstCommand );
	}
}

void UndoRedoStack::redo()
{
	Command* firstCommand = m_redoStack.front();
	UndoRedoEvent event( this, UNDOREDO_EVENT_REDO,firstCommand );
	fireOnRedoCommand( &event );	
	if ( true == event.getAllowsRedo() ) {
		firstCommand->redo();
		m_redoStack.pop_front();
		m_undoStack.push_back( firstCommand );
	}
}

bool UndoRedoStack::hasUndoableItems()
{
	return !m_undoStack.empty();
}

bool UndoRedoStack::hasRedoableItems()
{
	return !m_redoStack.empty();
}

void UndoRedoStack::addCommand( Command* command, const bool& autoExecute )
{
	command->setOwningStack( this );
	m_undoStack.push_back( command );
	m_redoStack.clear();
	if ( true == autoExecute ) {		
		command->execute();
		UndoRedoEvent event( this, UNDOREDO_EVENT_EXECUTE,command );
		fireOnExecuteCommand( &event );		
	}

	UndoRedoEvent event( this, UNDOREDO_EVENT_STACK_CHANGED, command );
	fireOnStackChanged( &event );	
}

void UndoRedoStack::movetToRedoStack( Command* command )
{
	
}

Command* UndoRedoStack::getCurrentUndoComand()
{
	return m_undoStack.back();
}

Command* UndoRedoStack::getCurrentRedoComand()
{
	return m_redoStack.front();
}