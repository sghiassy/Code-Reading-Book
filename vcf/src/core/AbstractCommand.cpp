//AbstractCommand.cpp
#include "ApplicationKit.h"
#include "Command.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "AbstractCommand.h"
#include "UndoRedoStack.h"


using namespace VCF;


AbstractCommand::AbstractCommand()
{
	m_stack = NULL;
	m_commandName = "";
}

AbstractCommand::~AbstractCommand()
{

}