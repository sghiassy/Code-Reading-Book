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

#ifndef _FILE_H__
#define _FILE_H__



namespace VCF {


class FilePeer;

/**
*The File class represents a single file or 
*directory.
*/
class FRAMEWORK_API File : public Object{

public:	

	File( const String& fileName = "");

	virtual ~File();

	/**
	*removes the file from the file system.
	*If the file is a directory then the contents 
	*are removed as well
	*/
	void remove();

	/**
	*creates a new file
	*if the last character in the filename is a 
	*directory character, then a directory is created
	*instead of a file.
	*/
	void create();


	/**
	*returns the size of the file in bytes
	*returns 0 if the file is a directory
	*/
	uint32 getSize();

	/**
	*returns the name of the file/directory
	*/
	String getName();

	/**
	*sets the name of the file object
	*/
	void setName( const String& fileName );

	/**
	*copies the contents of the file to the specified directory
	*/
	void copyTo( const String& copyFileName );

	/**
	*is this File object a directory
	*@return bool true if the object represents a directory, otherwise false.
	*/
	bool isDirectory();
	
	FilePeer* getPeer() {
		return m_filePeer;
	}
protected:
	FilePeer* m_filePeer;
};

}; //end of namespace VCF

#endif //_FILE_H__