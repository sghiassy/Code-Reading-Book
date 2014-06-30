//$$Root$$Application.h
#include "ApplicationKit.h"
#include "$$Root$$Application.h"
#include "$$MainWindow$$.h"

$$IF(SPLASH_SCR)
#include "$$SplashScreen$$.h"
$$ENDIF


using namespace VCF;



$$Root$$Application::$$Root$$Application()
{
	
}

$$Root$$Application::~$$Root$$Application()
{
	
}

bool $$Root$$Application::initRunningApplication(){
	bool result = Application::initRunningApplication();
$$IF(SPLASH_SCR)
	$$SplashScreen$$* splash = new $$SplashScreen$$();
$$ENDIF
	$$MainWindow$$* mainWindow = new $$MainWindow$$();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "$$Root$$" );
	
	return result;
}

