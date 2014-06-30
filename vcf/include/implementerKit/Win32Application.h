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

// Win32Application

#ifndef _WIN32APPLICATION_H__
#define _WIN32APPLICATION_H__



namespace VCF
{

class Win32ResourceBundle;

class APPKIT_API Win32Application : public Object, public ApplicationPeer  
{
public:
	Win32Application();
	virtual ~Win32Application();

	virtual bool initApp( const std::vector<VCF::String>& appCmdLine );

	virtual void terminateApp(); 

	virtual VCF::AbstractApplication* getApplication();

	virtual void setApplication( VCF::AbstractApplication* application );
	
	virtual ResourceBundle* getResourceBundle();

	virtual String getFileName();

	virtual void run();

	virtual String getCurrentDirectory();

	virtual void setCurrentDirectory( const String& currentDirectory );

	virtual long getHandleID();

	virtual void setHandleID( const long& handleID );
private:
	VCF::AbstractApplication* m_app;
	HINSTANCE m_instanceHandle;
	Win32ResourceBundle* m_resBundle;
};

};
#endif 
