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

#ifndef _FILEPEER_H__
#define _FILEPEER_H__


namespace VCF {


class File;

class Directory::Finder;

/**
*The FilePeer interface is used to describe basic 
*file operations that are then implemented for the 
*specific OS platform. The File class uses an internal 
*instance of this class to perform the actual work
*of dealing with files.
*/
class FRAMEWORK_API FilePeer {
public:

	virtual ~FilePeer(){};

	virtual void setFile( File* file ) = 0;
	/**
	*deletes the file from the file system
	*/
	virtual void remove() = 0;

	/**
	*creates a new file
	*if the last character in the filename is a 
	*directory character, then a directory is created
	*instead of a file.
	*/
	virtual void create() = 0;


	/**
	*returns the size of the file in bytes
	*/
	virtual uint32 getSize() = 0;

	virtual String getName() = 0;

	virtual void setName( const String& fileName ) = 0;
	
	virtual void copyTo( const String& copyFileName ) = 0;	

	virtual bool beginFileSearch( Directory::Finder* finder ) = 0;

	virtual String findNextFileInSearch( Directory::Finder* finder ) = 0;

	virtual void endFileSearch( Directory::Finder* finder ) = 0;
};

}; //end of namespace VCF

#endif //_FILEPEER_H__