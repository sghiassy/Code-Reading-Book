//UndoAppApplication.h

#ifndef _ERRORLOGGINGAPPAPPLICATION_H__
#define _ERRORLOGGINGAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class ErrorLoggingApplication documentation
*/
class ErrorLoggingApplication : public VCF::Application { 
public:
	ErrorLoggingApplication();

	virtual ~ErrorLoggingApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static ErrorLoggingApplication* getErrorLoggingApplication() {
		return (ErrorLoggingApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_ERRORLOGGINGAPPAPPLICATION_H__


