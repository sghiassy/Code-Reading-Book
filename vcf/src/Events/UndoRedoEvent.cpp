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
#include "ApplicationKit.h"
#include "UndoRedoEvent.h"


using namespace VCF;


UndoRedoEvent::UndoRedoEvent( Object* source, const unsigned long& eventType, Command* command ):
	Event( source, eventType )
{
	m_command = command;
	m_allowUndo = true;
	m_allowRedo = true;
	this->m_allowExecute = true;
}

UndoRedoEvent::~UndoRedoEvent()
{

}

void UndoRedoEvent::setAllowsUndo( const bool& allowsUndo ){
	m_allowUndo = allowsUndo;
	if ( false == m_allowUndo ) {
		this->setConsumed( true );//stop processing the events
	}		
}

void UndoRedoEvent::setAllowsRedo( const bool& allowsRedo ){
	m_allowRedo = allowsRedo;
	if ( false == m_allowRedo ) {
		this->setConsumed( true );//stop processing the events
	}		
}