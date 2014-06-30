//ToolbarsApplication.h
#include "ApplicationKit.h"
#include "ToolbarsApplication.h"
#include "MainWindow.h"



using namespace VCF;



ToolbarsApplication::ToolbarsApplication()
{
	
}

ToolbarsApplication::~ToolbarsApplication()
{
	
}

bool ToolbarsApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);	
	mainWindow->setVisible( true );
	mainWindow->setCaption( "Toolbars" );
	
	return result;
}

