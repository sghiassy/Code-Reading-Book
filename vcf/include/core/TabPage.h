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

#ifndef _TABPAGE_H__
#define _TABPAGE_H__




namespace VCF {

#define TABPAGE_CLASSID		"86F02172-3E7F-11d4-8EA7-00207811CFAB"

class APPKIT_API TabPage : public Item{

public:
	BEGIN_ABSTRACT_CLASSINFO(TabPage, "VCF::TabPage", "VCF::Object", TABPAGE_CLASSID)
	END_CLASSINFO(TabPage)
	TabPage(){
		
	};

	virtual ~TabPage(){};

	virtual void setPageName( const String& name ) = 0;

	virtual String getPageName() = 0;

	virtual Control* getPageComponent() = 0;

	virtual void setPageComponent( Control* component ) = 0;

	virtual ulong32 getPreferredHeight() = 0;	
};

}; //end of namespace VCF

#endif //_TABPAGE_H__