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

//FrameworkConfig.h

/**
*Basic setup defines for Foundationkit Classes
*/

#ifndef _FRAMEWORK_CONFIG_H__
#define _FRAMEWORK_CONFIG_H__


#define KEEP_NAMESPACE_IN_CLASSNAME
//#define USE_GARBAGE_COLLECTION
#ifdef _WIN32
	#ifdef _DEBUG
	//	#define USE_VCF_NEW
	#endif
#endif


//this section will turn the advanced RTTI features on or off. By default they are on
//Note: Turning them off means that code that depends on the RTTI API's will fail. 


#define VCF_RTTI	//comment this define out to turn off RTTI in the framework

//apparently NULL may not always be 0 - this makes it so...
#ifdef VCF_GCC
#undef NULL
#endif //VCF_GCC
#define NULL		0



#ifdef _WIN32

//include std windoze headers for peers....
#include <windows.h>
#include <commctrl.h> //make sure to link with comctl32.lib
#include <shlobj.h>

/**
*The defines below come partly from the following article on STL exporting/importing
*from MS DLL's. 
*Remember Luke, Microsoft sucks, Microsoft is the Dark Side of the Force.
*HOWTO: Exporting STL Components Inside & Outside of a Class
*Last reviewed: January 19, 1998
*Article ID: Q168958 
*/
//disable warnings on 255 char debug symbols
	#pragma warning (disable : 4786)
//disable warnings on extern before template instantiation
	#pragma warning (disable : 4231)

	#pragma warning (disable : 4251)

	#pragma warning (disable : 4275)

	#ifdef FRAMEWORK_DLL

		#if defined(FRAMEWORK_EXPORTS) 
			#define FRAMEWORK_API __declspec(dllexport)
			#define FRAMEWORK_EXPIMP_TEMPLATE
		#else
			#define FRAMEWORK_API __declspec(dllimport)
			#define FRAMEWORK_EXPIMP_TEMPLATE extern
		#endif
	#else	
		#define FRAMEWORK_API
	#endif //FRAMEWORK_DLL

	#ifdef GRAPHICSKIT_DLL

		#if defined(GRAPHICSKIT_EXPORTS) 
			#define GRAPHICSKIT_API __declspec(dllexport)
			#define GRAPHICSKIT_EXPIMP_TEMPLATE
		#else
			#define GRAPHICSKIT_API __declspec(dllimport)
			#define GRAPHICSKIT_EXPIMP_TEMPLATE extern
		#endif
	#else	
		#define GRAPHICSKIT_API
	#endif //GRAPHICSKIT_DLL

	#ifdef APPKIT_DLL

		#if defined(APPKIT_EXPORTS) 
			#define APPKIT_API __declspec(dllexport)
			#define APPKIT_EXPIMP_TEMPLATE
		#else
			#define APPKIT_API __declspec(dllimport)
			#define APPKIT_EXPIMP_TEMPLATE extern
		#endif
	#else	
		#define APPKIT_API
	#endif //APPKIT_DLL

	#ifdef NETKIT_DLL

		#if defined(NETKIT_EXPORTS) 
			#define NETKIT_API __declspec(dllexport)
			#define NETKIT_EXPIMP_TEMPLATE
		#else
			#define NETKIT_API __declspec(dllimport)
			#define NETKIT_EXPIMP_TEMPLATE extern
		#endif
	#else	
		#define NETKIT_API
	#endif //APPKIT_DLL
#endif //_WIN32

#ifdef VCF_MAC
	#define FRAMEWORK_API
#endif //VCF_MAC

#ifdef VCF_GCC
    #define FRAMEWORK_API
	#define GRAPHICSKIT_API
	#define APPKIT_API
	#define NETKIT_API
	
#endif //VCF_GCC


#endif

