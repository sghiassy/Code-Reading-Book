//UndoAppApplication.h

#ifndef _$$ROOT$$APPAPPLICATION_H__
#define _$$ROOT$$APPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class $$Root$$Application documentation
*/
class $$Root$$Application : public VCF::Application { 
public:
	$$Root$$Application();

	virtual ~$$Root$$Application();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static $$Root$$Application* get$$Root$$Application() {
		return ($$Root$$Application*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_$$ROOT$$APPAPPLICATION_H__


