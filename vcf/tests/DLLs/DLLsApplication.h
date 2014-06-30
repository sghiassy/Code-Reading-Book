//UndoAppApplication.h

#ifndef _DLLSAPPAPPLICATION_H__
#define _DLLSAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class DLLsApplication documentation
*/
class DLLsApplication : public VCF::Application { 
public:
	DLLsApplication();

	virtual ~DLLsApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static DLLsApplication* getDLLsApplication() {
		return (DLLsApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_DLLSAPPAPPLICATION_H__


