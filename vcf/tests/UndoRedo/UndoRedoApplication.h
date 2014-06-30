//UndoAppApplication.h

#ifndef _UNDOREDOAPPAPPLICATION_H__
#define _UNDOREDOAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class UndoRedoApplication documentation
*/
class UndoRedoApplication : public VCF::Application { 
public:
	UndoRedoApplication();

	virtual ~UndoRedoApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static UndoRedoApplication* getUndoRedoApplication() {
		return (UndoRedoApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_UNDOREDOAPPAPPLICATION_H__


