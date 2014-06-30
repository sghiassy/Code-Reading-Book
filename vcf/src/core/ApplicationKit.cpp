//ApplicationKit.cpp

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



#include "ApplicationKit.h"
#include "Panel.h"
#include "DefaultPropertyEditors.h"
#include "TabbedPages.h"
#include "DefaultListItem.h"
#include "DefaultListModel.h"
#include "DefaultTextModel.h"
#include "DefaultTreeItem.h"
#include "DefaultTreeModel.h"
#include "Label.h"
#include "ListViewControl.h"
#include "TextControl.h"
#include "TreeControl.h"
#include "ListBoxControl.h"
#include "MultilineTextControl.h"
#include "ImageControl.h"

//#include "ToolbarDock.h"

#include "Basic3DBorder.h"
#include "LibraryApplication.h"
#include "Desktop.h"
#include "ToolbarDock.h"
#include "ComponentEditorManager.h"


using namespace VCF;

static bool ApplicationKitIsInitialized = false;

static ApplicationKitRefCount = 0;

std::map<String,std::vector<Class*>*>* Component::registeredComponentMap = NULL;

EnumeratorContainer<std::vector<Class*>,Class*>* Component::registeredCompContainer = NULL;

EnumeratorContainer<std::vector<String>,String>* Component::registeredCategoryContainer = NULL;

std::vector<String>* Component::registeredCategory = NULL;

PropertyEditorManager* PropertyEditorManager::propertyEditorMgr = NULL;

std::map<String,LibraryApplication*>* LibraryApplication::namedLibraryAppMap = NULL;

EnumeratorMapContainer<std::map<String,LibraryApplication*>, LibraryApplication*>* LibraryApplication::namedLibAppContainer = NULL;

CursorManager* CursorManager::cursorMgrInstance = NULL;

Desktop* Desktop::desktopInstance = NULL;

DockManager* DockManager::dockManagerInstance = NULL;

void initApplicationKit()
{
	ApplicationKitRefCount ++;
	if ( false == ApplicationKitIsInitialized ) {
		
		initFoundationKit();
		initGraphicsKit();
		
		REGISTER_CLASSINFO( Item );
		REGISTER_CLASSINFO( ListItem );
		REGISTER_CLASSINFO( Model );
		REGISTER_CLASSINFO( ListModel );
		REGISTER_CLASSINFO( MenuItem );	
		REGISTER_CLASSINFO(TabModel);
		REGISTER_CLASSINFO(TabPage);
		REGISTER_CLASSINFO( TextModel );
		REGISTER_CLASSINFO( TreeItem );
		REGISTER_CLASSINFO( TreeModel );
		REGISTER_CLASSINFO( Border );
		REGISTER_CLASSINFO( AbstractListModel );
		REGISTER_CLASSINFO( AbstractTextModel );
		REGISTER_CLASSINFO( AbstractTreeModel );
		REGISTER_CLASSINFO( Component );
		REGISTER_CLASSINFO( Control );
		REGISTER_CLASSINFO( CustomControl );
		REGISTER_CLASSINFO( ControlContainer );
		REGISTER_CLASSINFO( TabbedPages );	
		REGISTER_CLASSINFO( DefaultListItem );
		REGISTER_CLASSINFO( DefaultListModel );
		REGISTER_CLASSINFO( DefaultTextModel );
		REGISTER_CLASSINFO( DefaultTreeItem );
		REGISTER_CLASSINFO( DefaultTreeModel );
		REGISTER_CLASSINFO( Frame );
		REGISTER_CLASSINFO( Dialog );
		REGISTER_CLASSINFO( Label );	
		REGISTER_CLASSINFO( ListViewControl );
		REGISTER_CLASSINFO( Panel );
		REGISTER_CLASSINFO( TextControl );
		REGISTER_CLASSINFO( MultilineTextControl );
		REGISTER_CLASSINFO(TreeControl );
		REGISTER_CLASSINFO(Window );
		REGISTER_CLASSINFO( CheckBoxControl );	
		REGISTER_CLASSINFO( Menu );
		REGISTER_CLASSINFO( MenuBar );
		REGISTER_CLASSINFO( PopupMenu );
		REGISTER_CLASSINFO( ListBoxControl );
		REGISTER_CLASSINFO( Basic3DBorder );
		REGISTER_CLASSINFO( CommandButton );	
		REGISTER_CLASSINFO( OpenGLControl );
		REGISTER_CLASSINFO( ComboBoxControl );
		REGISTER_CLASSINFO( ImageControl );
		REGISTER_CLASSINFO( ImageFilenameString );
		//REGISTER_CLASSINFO( ToolbarDock );
		
		PropertyEditorManager::initPropertyEditorManager();

		ComponentEditorManager::initComponentEditorManager();

		UIToolkit::initToolKit();
		
		LibraryApplication::initLibraryRegistrar();

		DockManager::create();
		
		Desktop::create();

		CursorManager::create();

		//Component::registerComponent( "VCF::ToolbarDock", STANDARD_CATEGORY );	
		/**
		*Register known classes here
		*/
		
	}
	ApplicationKitIsInitialized = true;
}

void terminateApplicationKit()
{
	if ( ApplicationKitRefCount > 0 ){
		ApplicationKitRefCount--;
	}

	if ( ApplicationKitRefCount == 0 ) {
		UIToolkit::closeToolkit();
		LibraryApplication::clearLibraryRegistrar();

		DockManager::getDockManager()->free();

		Desktop::getDesktop()->free();

		CursorManager::getCursorManager()->free();

		ComponentEditorManager::closeComponentEditorManager();

		PropertyEditorManager::closePropertyEditorManager();

		terminateGraphicsKit();

		terminateFoundationKit();
	}	
}