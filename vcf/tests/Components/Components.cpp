//Components.cpp


#include "ApplicationKit.h"


using namespace VCF;


class ComponentsWindow : public Window {
public:
	ComponentsWindow() {
		setCaption( "Components" );
		setVisible( true );
	}

	virtual ~ComponentsWindow(){};

};




class ComponentsApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new ComponentsWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		
		return result;
	}

};


int main(int argc, char *argv[])
{
	ComponentsApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

