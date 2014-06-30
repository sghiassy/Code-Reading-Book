//testApp.cpp


#include "ApplicationKit.h"
#include "Window.h"
#include "Rect.h"

using namespace VCF;


class testAppApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* helloWorldWindow = new Window();
		setMainWindow(helloWorldWindow);
		helloWorldWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		helloWorldWindow->setCaption( "Hello World !!!!" );

		return result;
	}

};


int main(int argc, char *argv[])
{
	testAppApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

