//CommandGroup.h
#include "ApplicationKit.h"
#include "CommandGroup.h"

using namespace VCF;

CommandGroup::CommandGroup()
{

}

CommandGroup::~CommandGroup()
{
	std::vector<Command*>::iterator it = m_commands.begin();
	while ( it != m_commands.end() ) {
		Command* command = *it;
		delete command;
		command = NULL;
		it++;	
	}
}

void CommandGroup::undo()
{
	std::vector<Command*>::iterator it = m_commands.begin();
	while ( it != m_commands.end() ) {
		Command* command = *it;
		command->undo();
		it++;	
	}
}

void CommandGroup::redo()
{
	std::vector<Command*>::iterator it = m_commands.begin();
	while ( it != m_commands.end() ) {
		Command* command = *it;
		command->redo();
		it++;	
	}
}

void CommandGroup::execute()
{
	std::vector<Command*>::iterator it = m_commands.begin();
	while ( it != m_commands.end() ) {
		Command* command = *it;
		command->execute();
		it++;	
	}
}
