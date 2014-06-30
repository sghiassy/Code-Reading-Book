//SplashTestApplication.h
#include "ApplicationKit.h"
#include "SplashTestApplication.h"
#include "MainWindow.h"

#include "SplashScreen.h"


using namespace VCF;



SplashTestApplication::SplashTestApplication()
{
	
}

SplashTestApplication::~SplashTestApplication()
{
	
}

bool SplashTestApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	SplashScreen* splash = new SplashScreen();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "SplashTest" );
	
	return result;
}

