//LibraryApplication.h

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

#ifndef _LIBRARYAPPLICATION_H__
#define _LIBRARYAPPLICATION_H__



namespace VCF  {

/**
*Class LibraryApplication documentation
*/
class APPKIT_API LibraryApplication : public AbstractApplication { 
public:
	LibraryApplication();

	virtual ~LibraryApplication();

	virtual bool initRunningApplication(){
		return true;
	}

	virtual void terminateRunningApplication(){};

	/**
	*allows for the use of programmable, user friendly names for Library Apps
	*/ 
	String getName() {
		return m_libName;	
	}

	void setName( const String& name );

	/**
	*gets a collection of named libraries that have been registered
	*/
	static Enumerator<LibraryApplication*>* getRegisteredLibraries();

	static void registerLibrary( LibraryApplication* libraryApp );	

	static void unRegisterLibrary( LibraryApplication* libraryApp );	

	static LibraryApplication* getRegisteredLibraryApplication( const String& libName );

	static void load( LibraryApplication* libraryApp );

	static void unload( LibraryApplication* libraryApp );

	static void initLibraryRegistrar();

	static void clearLibraryRegistrar();
protected:	
	String m_libName;
private:
	static std::map<String,LibraryApplication*>* namedLibraryAppMap;	
	static EnumeratorMapContainer<std::map<String,LibraryApplication*>, LibraryApplication*>* namedLibAppContainer;
};


}; //end of namespace VCF

#endif //_LIBRARYAPPLICATION_H__


