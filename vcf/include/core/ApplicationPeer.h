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


//ApplicationPeer.h


#ifndef _APPLICATION_Peer_H__
#define _APPLICATION_Peer_H__


namespace VCF
{

class AbstractApplication;

class ResourceBundle;

/**
*System neutral interface for intializing the windowing system application
*
*@version 1.0
@author Jim Crafton
*/
class APPKIT_API ApplicationPeer {
public:
	virtual ~ApplicationPeer(){};
	
	/**
	*initializes the application Peer. Any window specific system stuff can 
	*go in the implementation here.
	*/
	virtual bool initApp( const std::vector<String>& appCmdLine )=0;

	/**
	*terminates the application Peer
	*/
	virtual void terminateApp() = 0; 
	
	/**
	*get the Application instance associated with the 
	*application implmenter
	*/
	virtual AbstractApplication* getApplication() = 0;

	/**
	*set the Application instance ot be associated with the 
	*application implmenter
	*/
	virtual void setApplication( AbstractApplication* application ) = 0;	

	/**
	*returns the resource bundle that contains
	*all the application's resources
	*/
	virtual ResourceBundle* getResourceBundle() = 0;

	/**
	*returns the fully qualified file name
	*for the application, i.e. "C:\Stuff\My App\Foo.exe"
	*/
	virtual String getFileName() = 0;

	/**
	*starts the application's main event loop
	*this is may be a no-op where not appropriate
	*Typically this is where the loop that starts to
	*listen to OS specific message will begin
	*/
	virtual void run() = 0;

	virtual String getCurrentDirectory() = 0;

	virtual void setCurrentDirectory( const String& currentDirectory ) = 0;

	/**
	*returns an identifier that is specific to the OS that uniquely identifies this
	*Application. Under Win32 this corresponds to the hInstance that is used in 
	*an application or in-process DLL.
	*/
	virtual long getHandleID()  = 0;

	virtual void setHandleID( const long& handleID ) = 0;
};


};
	
#endif