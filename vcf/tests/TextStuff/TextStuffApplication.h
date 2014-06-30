//UndoAppApplication.h

#ifndef _TEXTSTUFFAPPAPPLICATION_H__
#define _TEXTSTUFFAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class TextStuffApplication documentation
*/
class TextStuffApplication : public VCF::Application { 
public:
	TextStuffApplication();

	virtual ~TextStuffApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static TextStuffApplication* getTextStuffApplication() {
		return (TextStuffApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_TEXTSTUFFAPPAPPLICATION_H__


