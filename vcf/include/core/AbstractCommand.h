//AbstractCommand.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _ABSTRACTCOMMAND_H__
#define _ABSTRACTCOMMAND_H__




namespace VCF  {

/**
*Class AbstractCommand documentation
*/
class APPKIT_API AbstractCommand : public Object, public Command { 
public:
	AbstractCommand();

	virtual ~AbstractCommand();		

	virtual UndoRedoStack* getOwningStack() {
		return m_stack;	
	}
	
	virtual void setOwningStack( UndoRedoStack* stack ) {
		m_stack = stack;
	}

	virtual String getName() {
		return m_commandName;
	}

	virtual void setName( const String& name ) {
		m_commandName = name;
	}
protected:
	UndoRedoStack* m_stack;
	String m_commandName;
private:
};


}; //end of namespace VCF

#endif //_ABSTRACTCOMMAND_H__


