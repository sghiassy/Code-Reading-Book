//HelloWorld.cpp
/**
*A hello world application
*This demonstrates the simplest 
*code possible to create a running app
*and display a window with the text
*"Hello World" in it's caption bar.
*/


/**
*include the ApplicationKit.h
*this has all the core files you'll need to get started.
*In general, you'll probably want to make this 
*the first header you include within your .cpp files
*/
#include "ApplicationKit.h"

/**
*use the VCF namespace to make it more convenient 
*to refer to VCF classes
*/
using namespace VCF;

/**
*here's the heart of the program, in good old main.
*yes this will work on Windows without displaying any 
*console
*/
int main(int argc, char *argv[])
{
	/**
	*create an instance of the Application class
	*on the stack. This will be the only instance 
	*of this class in the process. You can derive your
	*own app class from it if you wish, but there can 
	*still only be one instance of it
	*/
	Application app;

	/**
	*create the main Window here.
	*Note that is on the heap, not the stack.
	*The application will clean it up for you,
	*so do NOT make this a stack based object
	*/
	Window* mainWindow = new Window();
	
	/**
	*set the Application's main window.
	*Failure to do this will prevent the Application
	*of being notified to close correctly, and it will not
	*be able to free up the heap based memory for the 
	*Window object. 
	*In the VCF there is always a main window for GUI based 
	*applications, which can be a Dialog or a Window object. 
	*When this main window closes, the app shuts down as well.
	*/
	app.setMainWindow( mainWindow );

	/**
	*Set the caption of the main window. Setting the caption 
	*cause the text passed in to the method to appear on
	*the window frame's caption bar.
	*/
	mainWindow->setCaption( "Hello World" );

	/**
	*show the main window.
	*By default the window is invisible until 
	*either show() or setVisible(true) is called.
	*We could have called mainWindow->setVisible(true)
	*and it would have worked as well.
	*/
	mainWindow->show();
	
	/**
	*hand off the command line to the static appMain() call
	*which in turn begins the main application loop.
	*/
	Application::appMain( argc, argv );
	
	return 0;
}

