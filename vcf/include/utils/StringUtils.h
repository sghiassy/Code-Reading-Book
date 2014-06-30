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

// StringUtils.h

#ifndef _STRINGUTILS_H__
#define _STRINGUTILS_H__


#define MAX_TRACE_STRING		5000



/**
*The StringUtils class is a collection of static 
*utility methods for operating on strings.
*/
class FRAMEWORK_API StringUtils  
{
public:
	StringUtils();
	virtual ~StringUtils();
	
	/**
	*gets the file directory portion of a fully qualified 
	*file name
	*@param String the fully qualified filename to parse through
	*@return String the directory portion of the filename
	*/
	static VCF::String getFileDir( const VCF::String& fileName );

	/**
	*gets the just the file name name from a fully qualified path
	*@param String the fully qualified path
	*@return String the file name 
	*/
	static VCF::String getFileName( const VCF::String& fileName );

	/**
	*gets the file extenstion, if it exists, includes the "." , so
	*passing in filename.foo would return ".foo"
	*@param String the filename
	*@return String the file extension
	*/
	static VCF::String getFileExt( const VCF::String& fileName );

	/**
	*outs the text to the debugger output
	*@param String the text to output 
	*/
	static void trace( const VCF::String& text );

	/**
	*Similar to trace() but allows variable arguments. Uses the 
	*same formatting rules as printf().
	*@param String the text to trace out after formatting. See printf()
	*for formatting rules.
	*/
	static void traceWithArgs( VCF::String text,... );
	
	/**
	*formats a string. Uses the same formatting rules as 
	*sprintf().
	*@param String the format text to use
	*@return String the formatted string
	*/
	static VCF::String format( VCF::String formatText, ... );

	/**
	*converts the string to lower case
	*@param String the text to convert
	*@return String the converted string
	*/
	static VCF::String lowerCase( const VCF::String& text );	

	/**
	*converts the string to upper case
	*@param String the text to convert
	*@return String the converted string
	*/
	static VCF::String upperCase( const VCF::String& text );	

	/**
	*converts the value to a string
	*@param int the value to convert
	*@return String the string representation of the int value
	*/
	static VCF::String toString( const int& value );

	/**
	*converts the value to a string
	*@param long the value to convert
	*@return String the string representation of the long value
	*/
	static VCF::String toString( const long& value );

	/**
	*converts the value to a string
	*@param float the value to convert
	*@return String the string representation of the float value
	*/
	static VCF::String toString( const float& value );

	/**
	*converts the value to a string
	*@param double the value to convert
	*@return String the string representation of the double value
	*/
	static VCF::String toString( const double& value );

	/**
	*converts the value to a string
	*@param unsigned long the value to convert
	*@return String the string representation of the unsigned long value
	*/
	static VCF::String toString( const VCF::ulong32& value );

	/**
	*converts the value to a string
	*@param unsigned int the value to convert
	*@return String the string representation of the unsigned int value
	*/
	static VCF::String toString( const VCF::uint32& value );

	/**
	*converts the value to a string
	*@param char int the value to convert
	*@return String the string representation of the char value
	*/
	static VCF::String toString( const char& value );

	/**
	*converts the value to a string
	*@param char bool the value to convert
	*@return String the string representation of the bool value 
	*(either "true" or "false")
	*/
	static VCF::String toString( const bool& value );

	/**
	*generates a new UUID and returns the string representation
	*@return String the new UUID as a string value
	*/
	static VCF::String newUUID();

	/**
	*gets the class name from a type_info struct. 
	*@param type_info the value returned from the 
	*expression :
	*<pre>
	*	typeid(AClass)
	*</pre>
	*@return String the name of the class the typeInfo references 
	*/
	static VCF::String getClassNameFromTypeInfo( const type_info& typeInfo  );
};



#endif 

	
