//////////////////////////////////////////////////////////////////////
// FILE: dgl_dllextensions.h
// PURPOSE: include file of implementation of the CExtensions class.
//////////////////////////////////////////////////////////////////////
// Part of DemoGL Demo System sourcecode. See version information
//////////////////////////////////////////////////////////////////////
// COPYRIGHTS:
// Copyright (c)1999-2001 Solutions Design. All rights reserved.
// Central DemoGL Website: www.demogl.com.
// 
// Released under the following license: (BSD)
// -------------------------------------------
// Redistribution and use in source and binary forms, with or without modification, 
// are permitted provided that the following conditions are met: 
//
// 1) Redistributions of source code must retain the above copyright notice, this list of 
//    conditions and the following disclaimer. 
// 2) Redistributions in binary form must reproduce the above copyright notice, this list of 
//    conditions and the following disclaimer in the documentation and/or other materials 
//    provided with the distribution. 
// 
// THIS SOFTWARE IS PROVIDED BY SOLUTIONS DESIGN ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, 
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL SOLUTIONS DESIGN OR CONTRIBUTORS BE LIABLE FOR 
// ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
// STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
// USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
//
// The views and conclusions contained in the software and documentation are those of the authors 
// and should not be interpreted as representing official policies, either expressed or implied, 
// of Solutions Design. 
//
// See also License.txt in the sourcepackage.
//
//////////////////////////////////////////////////////////////////////
// Contributers to the code:
//		- Frans Bouma [FB]
//////////////////////////////////////////////////////////////////////
// VERSION INFORMATION.
//
// v1.31: Added extension string parser to the class.
// v1.3:  No changes.
// v1.2:  No changes.
// v1.1:  Added to codebase.
//////////////////////////////////////////////////////////////////////

#ifndef _DGL_DLLEXTENSIONS_H
#define _DGL_DLLEXTENSIONS_H

////////////////////////////////////////////////////////////////////////
//                          
//						DEFINES
//
////////////////////////////////////////////////////////////////////////
// maximum of supported extensions
#define	MAX_EXT				1024

// Maximum length of the name of 1 extension.
#define	MAX_EXT_STRLENGTH	256

////////////////////////////////////////////////////////////////////////
//                          
//						TYPEDEFS
//
////////////////////////////////////////////////////////////////////////

// Purpose: typedef to hold parse information for the extension string parse routine
typedef struct
{
	char		m_sName[MAX_EXT_STRLENGTH];
	int			m_iID;
} SExtParseInfo;

////////////////////////////////////////////////////////////////////////
//                          
//						CLASS DEFINITIONS
//
////////////////////////////////////////////////////////////////////////
class CExtensions
{
	// PUBLIC MEMBERS NOT ALLOWED!	
	
	// public method declarations
	public:
						CExtensions(void);
		virtual			~CExtensions(void);
		bool			GetExtensionAvailability(const int iExtension);
		void			SetExtensionAvailability(int iExtension);
		void			ParseExtensionsString(void);

	// private method declarations
	private:

	// private member declarations
	private:
		bool			m_arrbExtensions[MAX_EXT];
};

#endif	// _DGL_DLLEXTENSIONS_H