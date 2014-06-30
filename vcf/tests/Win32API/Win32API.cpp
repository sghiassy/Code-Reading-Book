//Win32API.cpp


#include "ApplicationKit.h"


using namespace VCF;


class Win32APIWindow : public Window {
public:
	Win32APIWindow() {
		setCaption( "Win32API" );
		setVisible( true );
	}

	virtual ~Win32APIWindow(){};

};




class Win32APIApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new Win32APIWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		
		return result;
	}

};


int main(int argc, char *argv[])
{
	Win32APIApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

