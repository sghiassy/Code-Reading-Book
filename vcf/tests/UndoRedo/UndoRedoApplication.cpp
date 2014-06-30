//UndoRedoApplication.h
#include "ApplicationKit.h"
#include "UndoRedoApplication.h"
#include "MainWindow.h"



using namespace VCF;



UndoRedoApplication::UndoRedoApplication()
{
	
}

UndoRedoApplication::~UndoRedoApplication()
{
	
}

bool UndoRedoApplication::initRunningApplication(){
	bool result = Application::initRunningApplication();
	MainWindow* mainWindow = new MainWindow();
	
	setMainWindow(mainWindow);
	mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
	mainWindow->setVisible( true );
	mainWindow->setCaption( "UndoRedo" );
	
	return result;
}

