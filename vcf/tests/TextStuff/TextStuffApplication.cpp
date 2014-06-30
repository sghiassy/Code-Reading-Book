//TextStuffApplication.h
#include "ApplicationKit.h"
#include "TextStuffApplication.h"
#include "MainWindow.h"



using namespace VCF;



TextStuffApplication::TextStuffApplication()
{
	
}

TextStuffApplication::~TextStuffApplication()
{
	
}

bool TextStuffApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "TextStuff" );
	
	return result;
}

