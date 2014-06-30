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

#ifndef _GTKAPPLICATION_H__
#define _GTKAPPLICATION_H__

#include "Object.h"
#include "ApplicationPeer.h"

namespace VCF {

class GTKApplication : public Object, public ApplicationPeer {

public:
	GTKApplication();

	virtual ~GTKApplication();

	/**
	*initializes the application Peer. Any window specific system stuff can 
	*go in the implementation here.
	*/
	virtual bool initApp( const std::vector<String>& appCmdLine );

	/**
	*terminates the application Peer
	*/
	virtual void terminateApp(); 
	
	/**
	*get the Application instance associated with the 
	*application implmenter
	*/
	virtual Application* getApplication();

	/**
	*set the Application instance ot be associated with the 
	*application implmenter
	*/
	virtual void setApplication( Application* application );	

	virtual ResourceBundle* getResourceBundle();

	virtual String getFileName();

	virtual void run();

	virtual String getCurrentDirectory();

	virtual void setCurrentDirectory( const String& currentDirectory );

private:
	Application* m_app;
	ResourceBundle* m_resBundle;
};

}; //end of namespace VCF

#endif //_GTKAPPLICATION_H__