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


#ifndef BADCOMPONENTSTATEEXCEPTION_H
#define BADCOMPONENTSTATEEXCEPTION_H


namespace VCF
{

/**
*This exception should be thrown when a component is used in an incorrect way. FOr 
*example, trying to display a component that requires a parent when the component
*has no parent would result in a BadComponentStateException being thrown.
*
*@version 1.0
*@author Jim Crafton
*/
class APPKIT_API BadComponentStateException : public BasicException {
public:

	BadComponentStateException( const VCF::String & message ):
		BasicException( message ){};

	BadComponentStateException():
		BasicException( BAD_COMPONENTSTATE_EXCEPTION ){};
	
	BadComponentStateException( const VCF::String & message, const int lineNumber ):
		VCF::BasicException(message, lineNumber){};
	virtual ~BadComponentStateException(){};


};

};
#endif //BADCOMPONENTSTATEEXCEPTION_HSN_HS