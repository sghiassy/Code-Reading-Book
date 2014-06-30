//ComponentEditorManager.h

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

#ifndef _COMPONENTEDITORMANAGER_H__
#define _COMPONENTEDITORMANAGER_H__


namespace VCF  {

class ComponentEditor;


#define REGISTER_FOR_ALL_COMPONENTS		"*"

/**
*Class ComponentEditorManager documentation
*/
class APPKIT_API ComponentEditorManager : public VCF::Object { 
public:
	ComponentEditorManager();

	virtual ~ComponentEditorManager();

	/**
	*find a suitable Component editor for a given 
	*classname.
	*/
	static ComponentEditor* findComponentEditor( const String& className );

	static void registerComponentEditor( ComponentEditor* componentEditor, const String& className );

	/**
	*initializes the Component editor manager for use - must be called at start up
	*/
	static void initComponentEditorManager();

	/**
	*frees any memory used by initComponentEditorManager(). Must be called before the 
	*runtime shuts down
	*/
	static void closeComponentEditorManager();
protected:
	static ComponentEditorManager* componentEditorMgr;

	std::map<String,ComponentEditor*> m_componentEditorMap;
};


}; //end of namespace VCF

#endif //_COMPONENTEDITORMANAGER_H__


