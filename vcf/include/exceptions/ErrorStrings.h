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
//
//ErrorStrings.h

#ifndef _ERROR_STRINGS_H__
#define _ERROR_STRINGS_H__

namespace VCF
{

#ifdef _DEBUG
#define MAKE_ERROR_MSG(message)			String(message) +	String("\nException in file: ") + String( __FILE__ )
#define MAKE_ERROR_MSG_2(message)		String(message) +	String("\nException in file: ") + String( __FILE__ ) + String(" at line: ") + StringUtils::toString(__LINE__)
#else
#define MAKE_ERROR_MSG(message)			String(message)
#define MAKE_ERROR_MSG_2(message)		String(message)
#endif

#define NO_TOOLKIT_ERROR				"Error in VCF Framework: No toolkit found."
#define NO_GFX_TOOLKIT_ERROR			"Error in VCF Framework: No Graphics toolkit found."
#define CANT_GET_INTERFACE_ERROR		"Error retreiving interface. COM can't find or can't instantiate the requested Interface."
#define NO_PEER							"Attempt to use an invalid Peer."
#define CLASS_NOT_FOUND					"Requested class not found in the Framework Class Registry."
#define INVALID_POINTER_ERROR			"Invalid pointer found. The pointer is either null or pointing to bad memory."
#define APPLICATION_EXCEPTION			"Application error."
#define BAD_COMPONENTSTATE_EXCEPTION	"The component is in an invalid state."
#define BAD_ITEMSTATE_EXCEPTION			"The item is in an invalid state."
#define BAD_IMAGE_BITS_EXCEPTION		"The image's pixel bits are corrupt or the pointer to them is referencing bad memory."
#define BAD_MODEL_STATE_EXCEPTION		"The model's is corrupt or incorrect."	
#define OUT_OF_BOUNDS_EXCEPTION			"The requested index is outside of the coleections bounds."					
#define NO_SUCH_ELEMENT_FOUND			"No corresponding element can be found with the index specified."
#define NO_FREE_MEMORY_AVALIABLE		"No more memory is available on the heap for this process."


#define INVALID_IMAGE					"This is image is invalid."

#define CANT_ACCESS_FILE				"Error attempting to access file: "				
#define CANT_WRITE_TO_FILE				"Error attempting to write to file: "
#define PROPERTY_CHANGE_BAD				"The requested value(s) cannot be set on the given object."

#define RUNTIME_ERROR					"A System runtime exception has occurred. Doh - you probably already knew that !"

};

#endif
