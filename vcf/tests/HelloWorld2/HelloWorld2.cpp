//HelloWorld2.cpp
/**
*This builds on our work in the first example, HelloWorld
*This will demonstrate customizing the VCF::Application class
*and positioning the window where we want it
*/

/**
*standard headers here
*/
#include "ApplicationKit.h"


using namespace VCF;

/**
*This is our application derived class that 
*we are going to customize.
*We override one method: the initRunningApplication() method,
*which surprisingly enough, is called to initialize the running
*application.
*In this method we can execute whatever appropriate start up code is 
*neccessary for our application, including createing the main
*window like we'll do below.
*/
class HelloWorld2Application : public Application {
public:

	virtual bool initRunningApplication(){
		/**
		*call the super classes initRunningApplication()
		*and store off the result.
		*If anything goes wrong here that will prevent our app
		*from starting, we should return false, to attempt 
		*to gracefully allow the application to shut down
		*/
		bool result = Application::initRunningApplication();
		
		/**
		*Create the window as before
		*/
		Window* mainWindow = new Window();

		/**
		*set the main window, again as we did in the first 
		*HelloWorld example
		*/
		setMainWindow( mainWindow );

		/**
		*set the bounds for the main window.
		*First create a VCF::Rect object on the stack
		*passing in the desired left, top, right, and 
		*bottom coordinates. Our coordinates are screen
		*coordinates, since this is the main window and
		*it does not have a parent. As usual, the coordinates
		*origin is in the top left corner of the screen
		*
		*In our case we will pass in 100.0, 100.0 for the left
		*and top coordinates, and 500.0, and 500.0 for the 
		*right and bottom, thus giving us a window with a
		*width and height of 400.0 and 400.0 respectively.
		*/
		Rect mainWindowBounds( 100.0, 100.0, 500.0, 500.0 );

		/**
		*pass in a pointer to the rect to the window
		*via the setBounds() call
		*/
		mainWindow->setBounds( &mainWindowBounds );
		
		/**
		*Show the main window
		*/
		mainWindow->show();

		return result;
	}

};


/**
*the applications one and only main function
*/
int main(int argc, char *argv[])
{
	/**
	*here we create an instance of our application class on the stack
	*/
	HelloWorld2Application app;

	/**
	*call the appMain() method
	*/
	Application::appMain( argc, argv );
	
	return 0;
}

