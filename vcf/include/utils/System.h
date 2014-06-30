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

//System.h

#ifndef _SYSTEM_H__
#define _SYSTEM_H__


namespace VCF
{

class SystemPeer;

class ErrorLog;

class BasicException;

/**
*The System object represents basic lower level OS functions
*/
class FRAMEWORK_API System : public Object {
public:
	static System* create();

	System();
	virtual ~System();

	static unsigned long getTickCount();

	static void sleep( const uint32& milliseconds );
	
	/**
	*sets the System's ErrorLog.
	*Note the Errolog is NOT owned by the System,
	*so it is the caller's responsibility to clean up the
	*memory. The suggested use is to create an ErrorLog
	*object on the stack in your main() function and 
	*then pass it to the System after initialization of the 
	*FoundationKit is done (i.e. after initFoundationKit() )
	*/
	static void setErrorLog( ErrorLog* errorLog );

	static void print( String text, ... );

	static void errorPrint( BasicException* exception );
	
	static System* getSystemInstance() {
		return System::systemInstance;
	}
private:
	SystemPeer* m_systemPeer;
	static System* systemInstance;

	ErrorLog* m_errorLogInstance;	
};

};

#endif