//Drawing.cpp


#include "ApplicationKit.h"


using namespace VCF;


class DrawingWindow : public Window {
public:
	DrawingWindow() {
		setCaption( "Drawing" );
		setVisible( true );
	}

	virtual ~DrawingWindow(){};

};




class DrawingApplication : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		Window* mainWindow = new DrawingWindow();
		setMainWindow(mainWindow);
		mainWindow->setBounds( &Rect( 100.0, 100.0, 500.0, 500.0 ) );
		
		return result;
	}

};


int main(int argc, char *argv[])
{
	DrawingApplication app;

	Application::appMain( argc, argv );
	
	return 0;
}

