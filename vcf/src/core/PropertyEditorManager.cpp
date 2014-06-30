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

// PropertyEditorManager.cpp

#include "ApplicationKit.h"


using namespace VCF;


PropertyEditorManager::PropertyEditorManager()
{
	
}

PropertyEditorManager::~PropertyEditorManager()
{
	std::map<String,PropertyEditor*>::iterator it = m_propertEditorMap.begin();
	while ( it != m_propertEditorMap.end() ){
		PropertyEditor* pe = it->second;
		delete pe;
		it++;
	}
	m_propertEditorMap.clear();
}

PropertyEditor* PropertyEditorManager::findEditor( const String& className )
{
	std::map<String,PropertyEditor*>::iterator found = 
		PropertyEditorManager::propertyEditorMgr->m_propertEditorMap.find( className );
	PropertyEditor* result = NULL;
	if ( found != PropertyEditorManager::propertyEditorMgr->m_propertEditorMap.end() ){
		result = found->second;
	}
	return result;
}

void PropertyEditorManager::registerPropertyEditor( PropertyEditor* editor, const String& className )
{
	PropertyEditorManager::propertyEditorMgr->m_propertEditorMap[className] = editor;
}

void PropertyEditorManager::initPropertyEditorManager()
{
	PropertyEditorManager::propertyEditorMgr = new PropertyEditorManager();
	PropertyEditorManager::propertyEditorMgr->init();
}

void PropertyEditorManager::closePropertyEditorManager()
{
	
	try{
		if ( NULL != PropertyEditorManager::propertyEditorMgr ){
			delete PropertyEditorManager::propertyEditorMgr;
		}
		PropertyEditorManager::propertyEditorMgr = NULL;
	}
	catch(...) {

	}
	
}