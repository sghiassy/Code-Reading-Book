
//FoundationKit.h
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


#ifndef _FOUNDATION_KIT_H__
#define _FOUNDATION_KIT_H__


#include "VCF.h"
#include "VCFChar.h"
#include "VCFString.h"
#include "CommonDefines.h"
#include "VCFMath.h"
#include "Enumerator.h"
#include "Interface.h"
#include "Object.h"
#include "ErrorStrings.h"
#include "BasicException.h"
#include "InvalidPeer.h"
#include "InvalidPointerException.h"
#include "NoFreeMemException.h"
#include "NoPeerFoundException.h"
#include "NoSuchElementException.h"
#include "OutOfBoundsException.h"
#include "PropertyChangeException.h"
#include "RuntimeException.h"
#include "TypeCastException.h"
#include "BasicFileError.h"
#include "FileNotFound.h"
#include "CantCreateObjectException.h"
#include "ClassNotFound.h"
#include "Event.h"
#include "EventHandler.h"
#include "NotifyListener.h"
#include "NotifyEvent.h"	
#include <typeinfo>
#include "StringUtils.h"
#include "ClassNotFound.h"
#include "Class.h"
#include "InterfaceClass.h"
#include "Enum.h"
#include "Persistable.h"
#include "PropertyListener.h"
#include "PropertyChangeException.h"
#include "BasicFileError.h"
#include "FileIOError.h"
#include "MemStreamUtils.h"
#include "Stream.h"
#include "TextInputStream.h"
#include "TextOutputStream.h"
#include "BasicInputStream.h"
#include "BasicOutputStream.h"
#include "PropertyChangeEvent.h"
#include "Property.h"
#include "Method.h"
#include "ClassInfo.h"
#include "ObjectWithEvents.h"
#include "Runnable.h"
#include "File.h"
#include "Directory.h"
#include "Library.h"
#include "Locales.h"
#include "FileStream.h"
#include "Registry.h"
#include "Point.h"
#include "Size.h"
#include "Rect.h"
#include "System.h"

/**
*initializes the Foundation Kit runtime.
*This includes registering the basic classes in the runtime system
*This MUST be called first off in the main() function of the app
*/



FRAMEWORK_API void initFoundationKit();

FRAMEWORK_API void terminateFoundationKit();


#endif