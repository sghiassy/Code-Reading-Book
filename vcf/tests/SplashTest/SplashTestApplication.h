//UndoAppApplication.h

#ifndef _SPLASHTESTAPPAPPLICATION_H__
#define _SPLASHTESTAPPAPPLICATION_H__


#include "VCF.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "UndoRedoStack.h"

using namespace VCF;


/**
*Class SplashTestApplication documentation
*/
class SplashTestApplication : public VCF::Application { 
public:
	SplashTestApplication();

	virtual ~SplashTestApplication();

	virtual bool initRunningApplication();
	
	UndoRedoStack* getCommandStack() {
		return &m_commandStack;
	}
	
	UndoRedoStack m_commandStack;

	static SplashTestApplication* getSplashTestApplication() {
		return (SplashTestApplication*)Application::getRunningInstance();
	}
protected:

private:
};


#endif //_SPLASHTESTAPPAPPLICATION_H__


