//UndoAppApplication.h

#ifndef _TOOLBARSAPPAPPLICATION_H__
#define _TOOLBARSAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class ToolbarsApplication documentation
*/
class ToolbarsApplication : public VCF::Application { 
public:
	ToolbarsApplication();

	virtual ~ToolbarsApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static ToolbarsApplication* getToolbarsApplication() {
		return (ToolbarsApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_TOOLBARSAPPAPPLICATION_H__


