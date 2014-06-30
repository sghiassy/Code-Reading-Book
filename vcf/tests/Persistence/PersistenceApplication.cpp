//PersistenceApplication.h
#include "ApplicationKit.h"
#include "PersistenceApplication.h"
#include "MainWindow.h"



using namespace VCF;



PersistenceApplication::PersistenceApplication()
{
	
}

PersistenceApplication::~PersistenceApplication()
{
	
}

bool PersistenceApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "Persistence" );
	
	return result;
}

