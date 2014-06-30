//Directory.h

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

#ifndef _DIRECTORY_H__
#define _DIRECTORY_H__


namespace VCF  {

/**
*A Directory represents a directory in a file system.
*Directories have all the features of a File, and in addition
*they may be searched for files based on a search filter.
*/
class FRAMEWORK_API Directory : public File { 
public:
	/**
	*A Finder object is used to iterate a 
	*set of files contained in a Directory object
	*that represents the directory.
	*Only Directory objects may create a Finder object.
	*A Finder is used just like an Enumerator, allowing 
	*the user to enumerate through the file names of the 
	*search. The only difference is that the very last element 
	*will be an empty string, so callers should test for this 
	*when retrieving the value from the nextElement() method.
	*/
	class FRAMEWORK_API Finder : public Object, public Enumerator<String>{
	public:

		virtual ~Finder();

		/*
		*Are there any more elements in this Finder ?
		*@param bool backward is ignored and makes no difference
		*in the enumeration order
		*/
		virtual bool hasMoreElements(const bool& backward=false);

		/**
		*gets the next file name in the search.
		*Test for an empty string, as the last element in the 
		*search will be empty.
		*/
		virtual String nextElement();

		/**
		*same as nextElement()
		*/
		virtual String prevElement();

		/**
		*resets the search. The backward parameter is 
		*ignored
		*@param bool backward ignored
		*/
		virtual void reset(const bool& backward=false);			
		
		/**
		*returns the Directory object that created
		*this Finder.
		*@return Directory the owning directory for this finder.
		*/
		Directory* getOwningDirectory() {
			return m_owningDirectory;
		}
		
		/**
		*returns the search filter used by this 
		*Finder. See Directory::findFiles() for 
		*more information about how the 
		*search filter is constructed
		*@return String the search filter used by this Finder
		*/
		String getSearchFilter() {
			return m_searchFilter;
		}
		
		friend class Directory;		

	protected:
		Finder( Directory* directoryToSearch, const String& filter );

		Directory* m_owningDirectory;
		String m_searchFilter;
		bool m_searchHasElements;
	};

	/**
	*Directory takes a file name for the constructor
	*argument, with by default is empty. Unlike a 
	*File, passing a name with out an ending "\" will
	*still work, as the "\" will be automatically added 
	*for you.
	*/
	Directory( const String& fileName = "" );

	virtual ~Directory();

	/**
	*performs a search of files under this Directory object.
	*@param String a filter that represents one or more wildcards. Each wild
	*card expression (*foo, or *.txt, etc) should be separated by a ";" 
	*character.
	*@return Finder 
	*/
	Finder* findFiles( const String& filter );
protected:

private:
};


}; //end of namespace VCF

#endif //_DIRECTORY_H__


