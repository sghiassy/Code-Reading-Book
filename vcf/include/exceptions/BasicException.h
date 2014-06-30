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


#ifndef BASICEXCEPTION_H
#define BASICEXCEPTION_H
#include "VCFString.h"
#include "FrameworkConfig.h"
#include "ErrorStrings.h"
#include <exception>


namespace VCF{

/**
*The base exception class in the Framework. 
*All exceptions should try and describe what went wrong as clearly as possible.
*For convenience a basic set of error strings are defined in ErrorStrings.h.
*
*@version 1.0
*@author Jim Crafton
*/
class FRAMEWORK_API BasicException : public std::exception {
public:

	/**
	*Basic constructor
	*All Exceptions take a text message identifying (in human readable terms ) what
	*went wrong. 
	*/
	BasicException( const VCF::String & message );
	
	BasicException( const VCF::String & message, const int lineNumber );

	virtual ~BasicException(){};

	/**
	*returns the message text associated with the exception
	*@return String - the error text
	*/
    VCF::String getMessage();  

private:
    VCF::String m_message;
};

};

#endif //BASICEXCEPTION_HXCEPTION_H