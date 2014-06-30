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
#include "ApplicationKit.h"
#include "ComponentEditorManager.h"
#include "ComponentEditor.h"

using namespace VCF;

ComponentEditorManager* ComponentEditorManager::componentEditorMgr = NULL;


ComponentEditorManager::ComponentEditorManager()
{

}

ComponentEditorManager::~ComponentEditorManager()
{
	std::map<String,ComponentEditor*>::iterator it = m_componentEditorMap.begin();
	while ( it != m_componentEditorMap.end() ) {
		delete it->second;
		it->second = NULL;
		it ++;
	}
	
	m_componentEditorMap.clear();
}

ComponentEditor* ComponentEditorManager::findComponentEditor( const String& className )
{
	ComponentEditor* result = NULL;
	std::map<String,ComponentEditor*>::iterator found = 
		ComponentEditorManager::componentEditorMgr->m_componentEditorMap.find( className );
	if ( found != ComponentEditorManager::componentEditorMgr->m_componentEditorMap.end() ) {
		result = found->second;
	}

	return result;
}

void ComponentEditorManager::registerComponentEditor( ComponentEditor* componentEditor, const String& className )
{
	ComponentEditorManager::componentEditorMgr->m_componentEditorMap[className] = componentEditor;
}


void ComponentEditorManager::initComponentEditorManager()
{
	ComponentEditorManager::componentEditorMgr = new ComponentEditorManager();
	ComponentEditorManager::componentEditorMgr->init();
}

void ComponentEditorManager::closeComponentEditorManager()
{

	if ( NULL != ComponentEditorManager::componentEditorMgr ) {
		delete ComponentEditorManager::componentEditorMgr;
	}
	ComponentEditorManager::componentEditorMgr = NULL;

}