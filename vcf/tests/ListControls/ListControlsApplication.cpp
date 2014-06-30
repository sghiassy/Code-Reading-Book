//ListControlsApplication.h
#include "ApplicationKit.h"
#include "ListControlsApplication.h"
#include "MainWindow.h"



using namespace VCF;



ListControlsApplication::ListControlsApplication()
{
	
}

ListControlsApplication::~ListControlsApplication()
{
	
}

bool ListControlsApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "ListControls" );
	
	return result;
}

