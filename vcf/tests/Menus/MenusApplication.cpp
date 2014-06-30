//MenusApplication.h
#include "ApplicationKit.h"
#include "MenusApplication.h"
#include "MainWindow.h"



using namespace VCF;



MenusApplication::MenusApplication()
{
	
}

MenusApplication::~MenusApplication()
{
	
}

bool MenusApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "Menus" );
	
	StringUtils::traceWithArgs( "" );
	return result;
}

