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

//Application.cpp

#include "ApplicationKit.h"
#include "Library.h"
#include "InvalidPointerException.h"
//#include "ResourceException.h"

using namespace VCF;


Application* Application::m_appInstance = NULL;

Application::Application()
{
	initApplicationKit();

	//initialize the toolkit
	this->m_mainWindow = NULL;
	
	
	Application::m_appInstance = this;
	UIToolkit* toolkit = UIToolkit::getDefaultUIToolkit();

	this->m_applicationPeer = toolkit->createApplicationPeer();
	m_applicationPeer->setApplication( this );

	//gets added automatically to the app list of event handlers
	WindowEventHandler<Application>* wh = 
		new WindowEventHandler<Application>( this,Application::onMainWindowClose, "AppWindowHandler" );
	
}	

Application::~Application()
{	
	delete m_applicationPeer;
	if ( NULL != this->m_mainWindow ){
		delete m_mainWindow;
	}

	std::map<String,Library*>::iterator it = m_VPLMap.begin();
	while ( it != m_VPLMap.end() ){
		Library* lib = it->second;
		delete lib;
		lib = NULL;
		it++;
	}

	terminateApplicationKit();
}

void Application::setCommandLineArgs( const std::vector<String>& appCmdLine )
{
	m_cmdLine = appCmdLine;
}

void Application::appMain( int argc, char** argv )
{	
	std::vector<String> commandLine;
	for ( int i=0;i<argc;i++){
		String tmp = argv[i];
		commandLine.push_back( tmp );
	}
	Application::appMain( commandLine );
}

void Application::appMain( const std::vector<String>& appCmdLine )
{
	//main try...catch 
	try {
		if ( NULL == Application::getRunningInstance() ){
			//throw exception			
		}
		else{//enter the main loop		
			
			Application* runningInstance = Application::getRunningInstance();
			/**
			*initialize the impementer first - we are not going to rely	
			*on someone remembering to call the base class functionality of the Application
			*if the Peer returns false then bomb out and terminate
			*/
			ApplicationPeer* appPeer = runningInstance->getPeer();
			if ( NULL == appPeer ){
				//throw exception - this is very BADDDDD
			}
			if ( true != appPeer->initApp( appCmdLine ) ){
				//ouch - something bad happened - terminate the app
				appPeer->terminateApp();
			}
			else {			
				runningInstance->setCommandLineArgs( appCmdLine );	
				
				if ( true != runningInstance->initRunningApplication() ){ //initialization failed
					runningInstance->terminateRunningApplication();
					appPeer->terminateApp();
				}
				else{				
					runningInstance->run();		
					
					runningInstance->terminateRunningApplication();

					appPeer->terminateApp();
				}
			}
		}
	}

	catch ( BasicException& e  ){
		String errString = e.getMessage().c_str();
		
		Dialog::showMessage( errString, MS_ERROR, "Framework Exception" );	

#ifdef _DEBUG
		throw e;
#endif
	}
	catch (std::exception& e){
		String errString = "STL C++ exception throw.\nError : \"";
#ifdef VCF_RTTI
		String clasName = typeid(e).name();
#else
		String clasName = "Unknown - RTTI not enabled";
#endif
		errString += " (Exception of type : " + clasName + ")";
		errString += e.what();
		errString += "\".\nApplication exiting abnormally.";
		
		Dialog::showMessage( errString, MS_ERROR, "Framework Exception" );	

#ifdef _DEBUG
		throw e;
#endif
	}
	catch (...){		
		Dialog::showMessage( "Unknown exception occurred. Application exiting abnormally.", 
			                 MS_ERROR, "Framework Exception" );
//#ifdef _DEBUG
		throw ;
//#endif
	}
	
}

Application* Application::getRunningInstance()
{
	return Application::m_appInstance;
}

Window* Application::getMainWindow()
{
	return this->m_mainWindow;
}

void Application::setMainWindow( Window* mainWindow )
{
	EventHandler* wl = getEventHandler("AppWindowHandler");
	if ( NULL != this->m_mainWindow ){
		this->m_mainWindow->removeWindowCloseHandler( wl ); 
	}
	this->m_mainWindow = mainWindow;
	this->m_mainWindow->addWindowCloseHandler( wl ); 
}

bool Application::initRunningApplication()
{
	return true;
}

void Application::onMainWindowClose( WindowEvent* event )
{
	Control* comp = (Control*)event->getSource();
	
	
}

void Application::terminateRunningApplication()
{
	
}

void Application::run()
{

	m_applicationPeer->run();
	//finished with events - app can terminate

#ifdef USE_VCF_NEW
	//dump any remaing memory - kindof buggy right now - use with caution !!!
	Object::dump();
#endif
	
}

void Application::loadVPL( const String& vplFileName )
{
	std::map<String,Library*>::iterator found = m_VPLMap.find( vplFileName );
	if ( found == m_VPLMap.end() ){
		Library* vplLib = new Library();
		try{
			vplLib->load( vplFileName );
			typedef void (*InitFunc)();
			InitFunc initFunc = (InitFunc)vplLib->getFunction("initPackage");
			initFunc();

			m_VPLMap[vplFileName] = vplLib; 	
		}
		catch ( BasicException& ex ){
			Dialog::showMessage( ex.getMessage(), MS_ERROR, "Framework Exception" );
		}		
	}	
}
