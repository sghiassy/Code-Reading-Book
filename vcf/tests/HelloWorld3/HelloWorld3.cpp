//HelloWorld3.cpp

/**
*The purpose of this example is to demonstrate creating not only 
*our own custom application class, but our first case of deriving 
*a new window class
*/

#include "ApplicationKit.h"


using namespace VCF;


/**
*This is our main window class. The code that was previously
*in the initialization method of our app class, but will now 
*migrate to our constructor
*/
class HelloWorld3Window : public Window {
public:
	HelloWorld3Window() {
		/**
		*Here we get the current running Application instance for 
		*this process
		*/
		Application* runningApp = Application::getRunningInstance();

		/**
		*This gets the app instance's class name to put in the 
		*caption of our man window
		*/
		String appname = runningApp->getClassName();

		/**
		*this sets the caption of the main window
		*/
		setCaption( appname + " - Hello World!" );
		
		/*
		*This will set the bounds of the window, see the 
		*previous HelloWorld example for a more in depth
		*discussion of this.
		*/
		Rect bounds( 100.0, 100.0, 500.0, 500.0 );
		setBounds( &bounds );

		/**
		*Here we can use the debug trace with variable 
		*arguments just like printf(), in this
		*case printing out the value of our "this" pointer
		*and the string value returned by the Rect's toString()
		*method
		*/
		StringUtils::traceWithArgs( "HelloWorld3Window constructor @%p, bounds: %s\n", this, bounds.toString().c_str() );

		/**
		*Show the main window. Previously we had used 
		*the show(), but setVisible(true) also works
		*/
		setVisible( true );
	}

	/**
	*Always, always, always make our destructor virtual 
	*/
	virtual ~HelloWorld3Window(){};

};




class HelloWorld3Application : public Application {
public:

	virtual bool initRunningApplication(){
		bool result = Application::initRunningApplication();
		
		/**
		*allocate some dummy temporary memory - we
		*will clean this up in the termination method of our 
		*app class
		*/
		m_tmpDummyBuffer = new char[4096];

		/**
		*Create a new instance of our window class
		*/
		Window* mainWindow = new HelloWorld3Window();

		/**
		*set the app's main window
		*/
		setMainWindow(mainWindow);		

		return result;
	}

	/**
	*terminates the running application. This is where
	*you can do your clean up of resources
	*In our case we'll clean up the memory we had allocated in our 
	*initRunningApplication() method above
	*and then call the super classes terminateRunningApplication()
	*/
	virtual void terminateRunningApplication() {		
		delete [] m_tmpDummyBuffer;
		m_tmpDummyBuffer = NULL;

		/**
		*here's an example of outputting to 
		*debug messages. In Win32 these get 
		*resolved to a call to OutputDebugString
		*thus if you're debugging with VC++ you'll
		*see this in the Output window.
		*/
		StringUtils::trace( "HelloWorld3Application::terminateRunningApplication() cleaned up memory\n" );
		Application::terminateRunningApplication();
	}


	char* m_tmpDummyBuffer;
};


int main(int argc, char *argv[])
{
	HelloWorld3Application app;

	Application::appMain( argc, argv );
	
	return 0;
}

