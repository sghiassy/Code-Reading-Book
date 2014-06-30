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

#ifndef _LIBRARYPEER_H__
#define _LIBRARYPEER_H__


namespace VCF {

/**
*The LibraryPeer represents a generic wrapper
*around using DLL/SOs on systems. Implemented 
*by a specific LibraryPeer derived OS class.
*/
class FRAMEWORK_API LibraryPeer {
public:

	virtual ~LibraryPeer(){};

	/**
	*loads a dynamic library 
	*/
	virtual void load( const String& libraryFilename ) = 0;

	/**
	*returns a pointer to the function named functionName.
	*@param String the name of the function to get a pointer to
	*@return void* a pointer to the function. Must be typcasted into
	*a valid function pointer on the caller's side.
	*Throws RuntimeException if a failure to load the library occurs
	*/
	virtual void* getFunction( const String& functionName ) = 0;

	virtual void unload() = 0;

};

}; //end of namespace VCF

#endif //_LIBRARYPEER_H__