//VCFBuilderApp.h
#include "ApplicationKit.h"
#include "UndoRedoEvent.h"
#include "UndoRedoListener.h"
#include "VCFBuilderApp.h"
#include "VCFBuilderUI.h"

#include "MainWindow.h"
#include "Project.h"
#include <algorithm>
#include "Desktop.h"
#include "MainWindow.h"


using namespace VCF;

using namespace VCFBuilder;


VCFBuilderApp::VCFBuilderApp()
{
	
}

VCFBuilderApp::~VCFBuilderApp()
{

}

bool VCFBuilderApp::initRunningApplication(){
	bool result = Application::initRunningApplication();
	
	m_VCFBuidlerUILib = VCFBuilderUIApplication::getVCFBuilderUI();	

	MainWindow* mainWnd = new MainWindow();

	setMainWindow( mainWnd );

	mainWnd->setBounds( &Rect( 200, 200, 500, 500 ) );
	
	return result;
}
