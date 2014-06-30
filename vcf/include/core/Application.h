/**
*Copyright (c) 2000-2001, Jim Crafton
*All rights reserved.
*Redistribution and use in source and binary forms, with or without
*modification, are permitted provided that the following conditions
*are met:
*	Redistributions of source code must retain the above copyright
*	notice, this list of conditions and the following disclaimer.
*
*	Redistributions in binary form must reproduce the above copyright
*	notice, this list of conditions and the following disclaimer in 
*	the documentation and/or other materials provided with the distribution.
*
*THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
*AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
*OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
*EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
*PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
*PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
*LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
*NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
*SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*NB: This software will not save the world.
*/


//Application.h



#ifndef APPLICATION_H
#define APPLICATION_H



namespace VCF
{

class Window;

class WindowEvent;

class Library;

/**
 *Represents the main process for a application. 
 *Each application contains a main window, pointed to by  m_mainWindow. 
 *Application are started by some entry point of the executing process.
 *On Win32 this is represented as a WinMain() function. On the other Windowing
 *systems this is more typcally a main() function. Once the entry point has been
 *called, the static function Application::appMain() is called starting the application to initialize it self.
 *
 *@version 1.0
 *@author Jim Crafton
 */

class APPKIT_API Application : public AbstractApplication {
public:
	Application();

	virtual ~Application();

	/**
	*The entry point into the application object starting up. 
	*/
	static void appMain( const std::vector<String>& appCmdLine );
	
	static void appMain( int argc, char** argv );

	static Application* getRunningInstance();

	/**
	*sets the command line arguments for the application. called by Application::appMain() when 
	*the application is first starting up
	*/
	void setCommandLineArgs( const std::vector<String>& appCmdLine );

	/**
	*starts the application windows message pump running
	*/
	void run();
	
	/**
	*This happens after the Application object is constructed, after the initialize() method,
	*and just before the app's run() method is called
	*successful. if false, it calls terminate() and then kills the app process.
	*this method also builds the accellerator table used for keyboard shortcuts
	*@return bool this tells whether or not the initialization of the application was 
	*/
	virtual bool initRunningApplication();
	
	/**
	*terminates the running application
	*/
	virtual void terminateRunningApplication();	

	/**
	*gets the main window of the application. All applications have a main window of some
	*sort. This can a be a tool type window, a dialog, or just a regular window. When this
	*window is closed, the application begins it's shutdown priocess
	*@return Window* the main window
	*/
	Window* getMainWindow();

	/**
	*sets the main window for the application
	*/
	void setMainWindow( Window* mainWindow );

	/**
	*event handler for the main window closing down.
	*/
	void onMainWindowClose( WindowEvent* event );	

	
	/**
	*load a VPL, calls the initPackage() function, and adds the library
	*to it's list. The library will be unloaded when the app quits.
	*/
	void loadVPL( const String& vplFileName );		
	
protected:
	static Application* m_appInstance;
	std::vector<String> m_cmdLine;
	std::map<String,Library*> m_VPLMap;
private:    
    Window * m_mainWindow;		
};

};


#endif //APPLICATION_H__