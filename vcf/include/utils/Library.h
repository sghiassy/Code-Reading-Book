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

#ifndef _LIBRARY_H__
#define _LIBRARY_H__




namespace VCF {

class LibraryPeer;

/**
*A Library represnts a DLL or SO. It can be created by passing 
*in a fully qualified file name to the constructor. The specified 
*DLL/SO will then be dynamically loaded (and unloaded when the 
*Library obejct is destroyed if autoUnloadLib is true).
*Alternatively you may call the load() method which will do the same.
*/
class FRAMEWORK_API Library : public Object{

public:
	Library( const String& libraryName = "", const bool& autoUnloadLib=false );

	virtual ~Library();
	
	/**
	*dynamically loads the DLL/SO specified by the libraryFilename
	*argument
	*@param String the fully qualified file name of the DLL or SO to 
	*load
	*/
	void load( const String& libraryFilename );

	/**
	*unloads the DLL/SO
	*/
	void unload();

	/**
	*returns an exported function of the library
	*@param String the name of the function to return
	*@return void* a void pointer to the exported function.
	*Callers are responsible for actually typecasting this 
	*to a meaningful function pointer. For example:
	*<pre>
	*	typedef int (*MyFunctionPtr)( int, bool);
	*	... //more code
	*	MyFunctionPtr funcPtr = (MyFunctionPtr) lib->getFunction( "DummyFunction" );
	*	int res = funcPtr( 12, true );
	*</pre>
	*if the function does not exist then the method returns null
	*/
	void* getFunction( const String& functionName );
	
	/**
	*initializes the library object
	*/
	void init();
private:
	LibraryPeer* m_libPeer;
	String m_currentLibName;
	bool m_autoUnloadLib;
};

}; //end of namespace VCF

#endif //_LIBRARY_H__