//UndoAppApplication.h

#ifndef _PERSISTENCEAPPAPPLICATION_H__
#define _PERSISTENCEAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class PersistenceApplication documentation
*/
class PersistenceApplication : public VCF::Application { 
public:
	PersistenceApplication();

	virtual ~PersistenceApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static PersistenceApplication* getPersistenceApplication() {
		return (PersistenceApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_PERSISTENCEAPPAPPLICATION_H__


