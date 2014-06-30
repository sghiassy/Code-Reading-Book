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

#ifndef _PROCESSPEER_H__
#define _PROCESSPEER_H__


namespace VCF {

/**
*a peer class that wraps working with processes
*on OS's
*/ 
class FRAMEWORK_API ProcessPeer {

public:
	ProcessPeer() {} ;

	virtual ~ProcessPeer() {};
	
	/**
	*returns an int that uniquely identifies the process on a given machine
	*/
	virtual int getProcessID() = 0;
	
	/**
	*returns the process's main thread ID
	*/
	virtual int getProcessThreadID() = 0;

	/**
	*creates a new process specified by the fully 
	*qualified file name in the processName argument.
	*@param String the fully qualified file name of 
	*process to start. This must represent an actual
	*executable file
	*@return bool returns true if the process was successfully started
	*otherwise false.
	*/
	virtual bool createProcess( const String& processName ) = 0;

	/**
	*returns the fully qualified file name of the process the 
	*object represents.
	*/
	virtual String getProcessFileName() = 0;
};

}; //end of namespace VCF

#endif //_PROCESSPEER_H__