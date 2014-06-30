//Command.h

/**
This program is free software; you can redistribute it and/or
modify it as you choose. In fact, you can do anything you would like
with it, so long as credit is given if used in commercial applications.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

NB: This software will not save the world. 
*/

#ifndef _COMMAND_H__
#define _COMMAND_H__




namespace VCF  {

class UndoRedoStack;


/**
*Class Command documentation
*/
class APPKIT_API Command { 
public:
	virtual ~Command(){};

	virtual void undo() = 0;

	virtual void redo() = 0;

	virtual void execute() = 0;

	virtual UndoRedoStack* getOwningStack() = 0;
	
	virtual void setOwningStack( UndoRedoStack* stack ) = 0;

	virtual String getName() = 0;

	virtual void setName( const String& name ) = 0;
protected:

private:
};


}; //end of namespace VCF

#endif //_COMMAND_H__


