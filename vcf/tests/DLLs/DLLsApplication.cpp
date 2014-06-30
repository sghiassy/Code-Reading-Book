//DLLsApplication.h
#include "ApplicationKit.h"
#include "DLLsApplication.h"
#include "MainWindow.h"



using namespace VCF;



DLLsApplication::DLLsApplication()
{
	
}

DLLsApplication::~DLLsApplication()
{
	
}

bool DLLsApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "DLLs" );
	
	return result;
}

