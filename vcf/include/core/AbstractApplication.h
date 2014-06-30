//AbstractApplication.h

/**
Copyright (c) 2000-2001, Jim Crafton
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
	Redistributions of source code must retain the above copyright
	notice, this list of conditions and the following disclaimer.

	Redistributions in binary form must reproduce the above copyright
	notice, this list of conditions and the following disclaimer in 
	the documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS
OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

NB: This software will not save the world. 
*/

#ifndef _ABSTRACTAPPLICATION_H__
#define _ABSTRACTAPPLICATION_H__



namespace VCF  {

class ApplicationPeer;

class ResourceBundle;

class Frame;



/**
*An AbstractApplication is a base class for deriving new application types
*Currently there are two main types, standard Application classes, which 
*represent the current process the application is associated with and of which there 
*should only be one, and LibraryApplication classes which are tied to 
*shared librarys or DLL's
*/
class APPKIT_API AbstractApplication : public ObjectWithEvents { 
public:
	AbstractApplication();

	virtual ~AbstractApplication();

	/**
	*This happens after the Application object is constructed, after the initialize() method,
	*and just before the app's run() method is called
	*successful. if false, it calls terminate() and then kills the app process.
	*this method also builds the accellerator table used for keyboard shortcuts
	*@return bool this tells whether or not the initialization of the application was 
	*/
	virtual bool initRunningApplication() = 0;
	
	/**
	*terminates the running application
	*/
	virtual void terminateRunningApplication() = 0;

	/**
	*retreives the system specific Peer for this application
	*@return ApplicationPeer - the application Peer
	*/
	ApplicationPeer* getPeer();

	/**
	*returns the application's resource bundle, which
	*contains all the application's resources, such as
	*images, strings, or form files, or other custom
	*data.
	*/
	ResourceBundle* getResourceBundle();

	/**
	*returns the full path for the application executable
	*/
	String getFileName();

	/**
	*returns the string which identifies what the 
	*application "sees" as it's current directory
	*/
	String getCurrentDirectory();

	/**
	*Sets the current directory for the application
	*/
	void setCurrentDirectory( const String& currentDirectory );

	/**
	*Creates a Frame from a frame derived class name
	*@param String the name of the frame derived class name
	*this is also the name of the VFF (Visual Form File) resource
	*which is loaded. If this can't be found or the class name
	*doesn't exist in the ClassRegistry then the return is NULL.
	*@return Frame a pointer to new instance of the type specified
	*in the frameClassName argument. NULL if the form resource doesn't exist
	*or the class doesn't exist. Otherwise it represent a full loaded 
	*instance, using the data from the resource VFF.
	*/
	Frame* createFrame( const String& frameClassName );

	/**
	*Loads an already existing Frame from a frame derived class name	
	*@param Frame a pointer to new instance of the type specified
	*in the frameClassName argument. 
	*/
	void loadFrame( Frame** frame );

	virtual void idleTime();
protected:
	ApplicationPeer* m_applicationPeer;
private:
};


}; //end of namespace VCF

#endif //_ABSTRACTAPPLICATION_H__



