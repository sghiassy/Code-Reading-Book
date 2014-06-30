//ErrorLoggingApplication.h
#include "ApplicationKit.h"
#include "ErrorLoggingApplication.h"
#include "MainWindow.h"



using namespace VCF;



ErrorLoggingApplication::ErrorLoggingApplication()
{
	
}

ErrorLoggingApplication::~ErrorLoggingApplication()
{
	
}

bool ErrorLoggingApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "ErrorLogging" );
	
	return result;
}

